
#ifndef MGRID_H
#define MGRID_H

#include <math.h>

#define mgrid_palette( p_grid ) \
   (uint32_t*)(((uint8_t*)(p_grid)) + (p_grid)->palette_offset)

#define mgrid_px( p_grid ) \
   (uint8_t*)(((uint8_t*)(p_grid)) + (p_grid)->px_offset)

#define mgrid_px_at( p_grid, p_grid_px, x, y ) \
   (p_grid_px[(y * (p_grid)->w) + x])

#define mgrid_pack_layer( p_grid_pack )

/**
 * \breif Create a new grid pack.
 */
MERROR_RETVAL mgrid_pack_new_h( MAUG_MHANDLE* p_grid_pack_h );

/**
 * \brief Add a new layer grid to the specified grid pack.
 */
MERROR_RETVAL mgrid_pack_add_layer(
   uint32_t w, uint32_t h, uint32_t pal_ncolors, MAUG_MHANDLE* p_grid_pack_h
);

#ifdef MGRID_C

/**
 * \note This is built as a static function for simplicity in making it
 *       available to plugins while still having local variables.
 */
struct MGRID* mgrid_get_layer_p(
   struct MGRID_PACK* grid_pack, uint32_t layer
) {
   struct MGRID* grid_iter = NULL;
   uint8_t* grid_pack_buf = (uint8_t*)grid_pack;

   if( 0 == grid_pack->count || layer >= grid_pack->count ) {
      error_printf( "invalid layer " U32_FMT
         " requested (of " U32_FMT ")!",
         layer, grid_pack->count );
      return NULL;
   }

   /* Advance to first grid. */
   grid_pack_buf += sizeof( struct MGRID_PACK );
   grid_iter = (struct MGRID*)grid_pack_buf;
   while( layer > 0 ) {
      grid_pack_buf += grid_iter->sz;
      grid_iter = (struct MGRID*)grid_pack_buf;
      layer--;
   }

   return grid_iter;
}

MERROR_RETVAL mgrid_pack_new_h( MAUG_MHANDLE* p_grid_pack_h ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MGRID_PACK* grid_pack = NULL;

   debug_printf( 1, "creating empty grid pack (" SIZE_T_FMT " bytes)...",
      sizeof( struct MGRID_PACK ) );

   /* Allocate new grid. */
   *p_grid_pack_h = maug_malloc( 1, sizeof( struct MGRID_PACK ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_grid_pack_h );

   maug_mlock( *p_grid_pack_h, grid_pack );
   maug_cleanup_if_null_alloc( struct MGRID_PACK*, grid_pack );

   maug_mzero( grid_pack, sizeof( struct MGRID_PACK ) );

   /* Setup header. */
   grid_pack->version = 1;
   grid_pack->sz = sizeof( struct MGRID_PACK );
   grid_pack->count = 0;

cleanup:

   if( NULL != grid_pack ) {
      maug_munlock( *p_grid_pack_h, grid_pack );
   }

   if( retval && NULL != *p_grid_pack_h ) {
      error_printf( "cleaning up faulty grid..." );
      maug_mfree( *p_grid_pack_h );
      *p_grid_pack_h = NULL;
   }

   return retval;
}

MERROR_RETVAL mgrid_pack_add_layer(
   uint32_t w, uint32_t h, uint32_t pal_ncolors, MAUG_MHANDLE* p_grid_pack_h
) {
   struct MGRID_PACK* grid_pack = NULL;
   MAUG_MHANDLE grid_pack_new_h = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t grid_new_sz = 0;
   size_t grid_pack_new_sz = 0;
   size_t pal_sz = sizeof( uint32_t ) * pal_ncolors;
   uint32_t* p_palette = NULL;
   struct MGRID* grid_new = NULL;

   maug_mlock( *p_grid_pack_h, grid_pack );
   maug_cleanup_if_null_alloc( struct MGRID_PACK*, grid_pack );

   grid_new_sz =
      sizeof( struct MGRID ) + /* Header */
      (sizeof( uint32_t ) * pal_ncolors) + /* Palette */
      (w * h); /* Pixels */
   grid_pack_new_sz = grid_pack->sz + grid_new_sz;

   maug_munlock( *p_grid_pack_h, grid_pack );
   
   debug_printf( 2,
      "adding %ux%u-pixel %u-color layer (" SIZE_T_FMT
      " bytes) to grid pack...", w, h, pal_ncolors, grid_new_sz );

   /* Allocate space for the new layer. */
   grid_pack_new_h = maug_mrealloc( *p_grid_pack_h, 1, grid_pack_new_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, grid_pack_new_h );
   *p_grid_pack_h = grid_pack_new_h;

   maug_mlock( *p_grid_pack_h, grid_pack );
   maug_cleanup_if_null_alloc( struct MGRID_PACK*, grid_pack );

   /* Configure the grid pack to account for new layer. */
   grid_pack->sz = grid_pack_new_sz;
   grid_pack->count++;

   grid_new = mgrid_get_layer_p( grid_pack, grid_pack->count - 1 );

   maug_mzero( grid_new, grid_new_sz );

   grid_new->version = 1;
   grid_new->sz = grid_new_sz;
   grid_new->w = w;
   grid_new->h = h;
   grid_new->palette_ncolors = pal_ncolors;
   grid_new->palette_offset = sizeof( struct MGRID );
   grid_new->px_offset = sizeof( struct MGRID ) + pal_sz;

   /* Setup palette. */
   debug_printf( 1, "setting up grid palette..." );

   p_palette = mgrid_palette( grid_new );

   #define GRID_PAL_FILL( idx, name, val ) \
      assert( idx < pal_ncolors ); \
      p_palette[idx] = val;

   PERPIX_DEF_COLOR_TABLE( GRID_PAL_FILL );

cleanup:

   if( NULL != grid_pack ) {
      maug_munlock( *p_grid_pack_h, grid_pack );
   }

   return retval;
}

#endif /* MGRID_C */

#endif /* !MGRID_H */

