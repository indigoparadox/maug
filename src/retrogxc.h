
#ifndef RETROGXC_H
#define RETROGXC_H

#include <maug.h>

#ifndef RETROGXC_INITIAL_SZ
#  define RETROGXC_INITIAL_SZ 16
#endif /* !RETROGXC_INITIAL_SZ */

#define RETROGXC_ERROR_CACHE_MISS -1

struct RETROFLAT_CACHE_BITMAP {
   struct RETROFLAT_BITMAP bitmap;
   retroflat_asset_path id;
};

int16_t retrogxc_init();

void retrogxc_clear_cache();

void retrogxc_shutdown();

int16_t retrogxc_load_bitmap( retroflat_asset_path res_p );

int16_t retrogxc_blit_at(
   uint16_t bitmap_idx, struct RETROFLAT_BITMAP* target,
   uint16_t s_x, uint16_t s_y, uint16_t d_x, uint16_t d_y,
   uint16_t w, uint16_t h );

#ifdef RETROGXC_C

static MAUG_MHANDLE gs_retrogxc_handle = (MAUG_MHANDLE)NULL;
static int16_t gs_retrogxc_sz = 0;

/* === */

int16_t retrogxc_init() {
   int16_t retval = 0;

   gs_retrogxc_handle = maug_malloc(
      RETROGXC_INITIAL_SZ, sizeof( struct RETROFLAT_CACHE_BITMAP ) );
   gs_retrogxc_sz = RETROGXC_INITIAL_SZ;

   size_printf( 3, "bitmap struct", sizeof( struct RETROFLAT_CACHE_BITMAP ) );

   if( (MAUG_MHANDLE)NULL != gs_retrogxc_handle ) {
      size_printf( 3, "initial graphics cache",
         sizeof( struct RETROFLAT_CACHE_BITMAP ) * RETROGXC_INITIAL_SZ );
   } else {
      error_printf( "unable to initialize graphics cache!" );
      retval = MERROR_GUI;
   }

   return retval;
}

/* === */

void retrogxc_clear_cache() {
   int16_t i = 0,
      dropped_count = 0;
   struct RETROFLAT_CACHE_BITMAP* bitmaps = NULL;

   maug_mlock( gs_retrogxc_handle, bitmaps );
   for( i = 0 ; gs_retrogxc_sz > i ; i++ ) {
      if( retroflat_bitmap_ok( &(bitmaps[i].bitmap) ) ) {
         retroflat_destroy_bitmap( &(bitmaps[i].bitmap) );
         dropped_count++;
      }
   }
   maug_mzero( bitmaps,
      sizeof( struct RETROFLAT_CACHE_BITMAP ) * gs_retrogxc_sz );
   maug_munlock( gs_retrogxc_handle, bitmaps );
   
   debug_printf( 2, "graphics cache cleared (%d of %d items)",
      dropped_count, gs_retrogxc_sz );

#  ifndef NO_GUI
   /* window_reload_frames(); */
#  endif /* !NO_GUI */
}

/* === */

void retrogxc_shutdown() {
   retrogxc_clear_cache();
   maug_mfree( gs_retrogxc_handle );
}

/* === */

int16_t retrogxc_load_bitmap( retroflat_asset_path res_p ) {
   int16_t idx = RETROGXC_ERROR_CACHE_MISS,
      i = 0;
   struct RETROFLAT_CACHE_BITMAP* bitmaps = NULL;

   maug_mlock( gs_retrogxc_handle, bitmaps );

   /* Try to find the bitmap already in the cache. */
   for( i = 0 ; gs_retrogxc_sz > i ; i++ ) {
      if( 0 == retroflat_cmp_asset_path( bitmaps[i].id, res_p ) ) {
         idx = i;
         goto cleanup;
      }
   }

   /* Bitmap not found. */
   debug_printf( 1, "bitmap not found in cache; loading..." );
   for( i = 0 ; gs_retrogxc_sz > i ; i++ ) {
      if( retroflat_bitmap_ok( &(bitmaps[i].bitmap) ) ) {
         if( retroflat_load_bitmap( res_p, &(bitmaps[i].bitmap) ) ) {
            idx = i;
         }
         goto cleanup;
      }
   }

   /* Still not found! */
   error_printf( "unable to load bitmap; cache full?" );

cleanup:

   if( NULL != bitmaps ) {
      maug_munlock( gs_retrogxc_handle, bitmaps );
   }

   return idx;
}

/* === */

int16_t retrogxc_blit_at(
   uint16_t bitmap_idx, struct RETROFLAT_BITMAP* target,
   uint16_t s_x, uint16_t s_y, uint16_t d_x, uint16_t d_y,
   uint16_t w, uint16_t h
) {
   int16_t retval = 1;
   struct RETROFLAT_CACHE_BITMAP* bitmaps = NULL,
      * bitmap_blit = NULL;

   maug_mlock( gs_retrogxc_handle, bitmaps );
   assert( NULL != bitmaps );

   bitmap_blit = &(bitmaps[bitmap_idx]);
   if( NULL == bitmap_blit ) {
      retval = MERROR_FILE;
      goto cleanup;
   }
   retroflat_blit_bitmap(
      &(bitmap_blit->bitmap), target, s_x, s_y, d_x, d_y, w, h );

cleanup:

   if( NULL != bitmaps ) {
      maug_munlock( gs_retrogxc_handle, bitmaps );
   }

   return retval;
}

#endif /* RETROGXC_C */

#endif /* !RETROGXC_H */

