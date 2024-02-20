
#ifndef RETROGXC_H
#define RETROGXC_H

#include <maug.h>

#ifndef RETROGXC_INITIAL_SZ
#  define RETROGXC_INITIAL_SZ 16
#endif /* !RETROGXC_INITIAL_SZ */

#ifndef RETROGXC_TRACE_LVL
#  define RETROGXC_TRACE_LVL 0
#endif /* !RETROGXC_TRACE_LVL */

#define RETROGXC_ERROR_CACHE_MISS (-1)

struct RETROFLAT_CACHE_BITMAP {
   struct RETROFLAT_BITMAP bitmap;
   retroflat_asset_path id;
};

MERROR_RETVAL retrogxc_init();

void retrogxc_clear_cache();

void retrogxc_shutdown();

int16_t retrogxc_load_bitmap( retroflat_asset_path res_p, uint8_t flags );

int16_t retrogxc_load_xpm( retroflat_asset_path res_p, uint8_t flags );

int16_t retrogxc_blit_bitmap(
   struct RETROFLAT_BITMAP* target, int16_t bitmap_idx,
   uint16_t s_x, uint16_t s_y, uint16_t d_x, uint16_t d_y,
   uint16_t w, uint16_t h );

#ifdef RETROGXC_C

static MAUG_MHANDLE gs_retrogxc_handle = (MAUG_MHANDLE)NULL;
static int16_t gs_retrogxc_sz = 0;

/* === */

MERROR_RETVAL retrogxc_init() {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_CACHE_BITMAP* bitmaps = NULL;

   gs_retrogxc_handle = maug_malloc(
      RETROGXC_INITIAL_SZ, sizeof( struct RETROFLAT_CACHE_BITMAP ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, gs_retrogxc_handle );
   gs_retrogxc_sz = RETROGXC_INITIAL_SZ;

   size_printf( RETROGXC_TRACE_LVL,
      "bitmap struct", sizeof( struct RETROFLAT_CACHE_BITMAP ) );
   size_printf( RETROGXC_TRACE_LVL, "initial graphics cache",
      sizeof( struct RETROFLAT_CACHE_BITMAP ) * gs_retrogxc_sz );

   maug_mlock( gs_retrogxc_handle, bitmaps );
   maug_cleanup_if_null_alloc( struct RETROFLAT_CACHE_BITMAP*, bitmaps );
   maug_mzero(
      bitmaps,
      RETROGXC_INITIAL_SZ * sizeof( struct RETROFLAT_CACHE_BITMAP ) );

cleanup:

   if( NULL != bitmaps ) {
      maug_munlock( gs_retrogxc_handle, bitmaps );
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
   
   debug_printf( RETROGXC_TRACE_LVL,
      "graphics cache cleared (%d of %d items)",
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

int16_t retrogxc_load_bitmap( retroflat_asset_path res_p, uint8_t flags ) {
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
   debug_printf( RETROGXC_TRACE_LVL,
      "bitmap %s not found in cache; loading...", res_p );
   for( i = 0 ; gs_retrogxc_sz > i ; i++ ) {
      if( retroflat_bitmap_ok( &(bitmaps[i].bitmap) ) ) {
         continue;
      }

      if(
         MERROR_OK ==
         retroflat_load_bitmap( res_p, &(bitmaps[i].bitmap), flags )
      ) {
         idx = i;
         debug_printf( RETROGXC_TRACE_LVL,
            "bitmap %s assigned cache ID: %d", res_p, idx );
      }
      goto cleanup;
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

#ifdef RETROFLAT_XPM

int16_t retrogxc_load_xpm( retroflat_asset_path res_p, uint8_t flags ) {
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
   debug_printf( RETROGXC_TRACE_LVL,
      "bitmap %s not found in cache; loading...", res_p );
   for( i = 0 ; gs_retrogxc_sz > i ; i++ ) {
      if( retroflat_bitmap_ok( &(bitmaps[i].bitmap) ) ) {
         continue;
      }

      if(
         MERROR_OK ==
         retroflat_load_xpm( res_p, &(bitmaps[i].bitmap), flags )
      ) {
         idx = i;
         debug_printf( RETROGXC_TRACE_LVL,
            "bitmap %s assigned cache ID: %d", res_p, idx );
      }
      goto cleanup;
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

#endif /* RETROFLAT_XPM */

int16_t retrogxc_blit_bitmap(
   struct RETROFLAT_BITMAP* target, int16_t bitmap_idx,
   uint16_t s_x, uint16_t s_y, uint16_t d_x, uint16_t d_y,
   uint16_t w, uint16_t h
) {
   int16_t retval = 1;
   struct RETROFLAT_CACHE_BITMAP* bitmaps = NULL,
      * bitmap_blit = NULL;

   assert( 0 <= bitmap_idx );

   maug_mlock( gs_retrogxc_handle, bitmaps );
   assert( NULL != bitmaps );

   bitmap_blit = &(bitmaps[bitmap_idx]);
   if( NULL == bitmap_blit ) {
      retval = MERROR_FILE;
      goto cleanup;
   }
   retroflat_blit_bitmap(
      target, &(bitmap_blit->bitmap), s_x, s_y, d_x, d_y, w, h );

cleanup:

   if( NULL != bitmaps ) {
      maug_munlock( gs_retrogxc_handle, bitmaps );
   }

   return retval;
}

#endif /* RETROGXC_C */

#endif /* !RETROGXC_H */

