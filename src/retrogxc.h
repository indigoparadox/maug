
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

#define RETROGXC_ASSET_TYPE_NONE    0
#define RETROGXC_ASSET_TYPE_BITMAP  1
#define RETROGXC_ASSET_TYPE_FONT    2

#define retrogxc_load_bitmap( res_p, flags ) \
   retrogxc_load_asset( res_p, retrogxc_loader_bitmap, flags )

#define retrogxc_load_xpm( res_p, flags ) \
   retrogxc_load_asset( res_p, retrogxc_loader_xpm, flags )

typedef int8_t RETROGXC_ASSET_TYPE;

typedef RETROGXC_ASSET_TYPE (*retrogxc_loader)(
   retroflat_asset_path res_p, MAUG_MHANDLE* handle_p, uint8_t flags );

struct RETROFLAT_CACHE_ASSET {
   uint8_t type;
   MAUG_MHANDLE handle;
   retroflat_asset_path id;
};

MERROR_RETVAL retrogxc_init();

void retrogxc_clear_cache();

void retrogxc_shutdown();

RETROGXC_ASSET_TYPE retrogxc_loader_bitmap(
   retroflat_asset_path res_p, MAUG_MHANDLE* handle_p, uint8_t flags );

RETROGXC_ASSET_TYPE retrogxc_loader_xpm(
   retroflat_asset_path res_p, MAUG_MHANDLE* handle_p, uint8_t flags );

int16_t retrogxc_load_asset(
   retroflat_asset_path res_p, retrogxc_loader l, uint8_t flags );

MERROR_RETVAL retrogxc_blit_bitmap(
   struct RETROFLAT_BITMAP* target, size_t bitmap_idx,
   uint16_t s_x, uint16_t s_y, uint16_t d_x, uint16_t d_y,
   uint16_t w, uint16_t h );

#ifdef RETROGXC_C

static MAUG_MHANDLE gs_retrogxc_handle = (MAUG_MHANDLE)NULL;
static size_t gs_retrogxc_sz = 0;

/* === */

MERROR_RETVAL retrogxc_init() {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_CACHE_ASSET* assets = NULL;

   gs_retrogxc_handle = maug_malloc(
      RETROGXC_INITIAL_SZ, sizeof( struct RETROFLAT_CACHE_ASSET ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, gs_retrogxc_handle );
   gs_retrogxc_sz = RETROGXC_INITIAL_SZ;

   size_printf( RETROGXC_TRACE_LVL,
      "asset struct", sizeof( struct RETROFLAT_CACHE_ASSET ) );
   size_printf( RETROGXC_TRACE_LVL, "initial graphics cache",
      sizeof( struct RETROFLAT_CACHE_ASSET ) * gs_retrogxc_sz );

   maug_mlock( gs_retrogxc_handle, assets );
   maug_cleanup_if_null_alloc( struct RETROFLAT_CACHE_ASSET*, assets );
   maug_mzero(
      assets,
      RETROGXC_INITIAL_SZ * sizeof( struct RETROFLAT_CACHE_ASSET ) );

cleanup:

   if( NULL != assets ) {
      maug_munlock( gs_retrogxc_handle, assets );
   }

   return retval;
}

/* === */

void retrogxc_clear_cache() {
   size_t i = 0,
      dropped_count = 0;
   struct RETROFLAT_CACHE_ASSET* assets = NULL;
   struct RETROFLAT_BITMAP* bitmap = NULL;

   maug_mlock( gs_retrogxc_handle, assets );
   for( i = 0 ; gs_retrogxc_sz > i ; i++ ) {
      if( RETROGXC_ASSET_TYPE_BITMAP == assets[i].type ) {
         if( NULL == assets[i].handle ) {
            error_printf( "invalid asset handle in asset #" SIZE_T_FMT, i );
            continue;
         }

         maug_mlock( assets[i].handle, bitmap );
         if( NULL != bitmap ) {
            retroflat_destroy_bitmap( bitmap );
         }
         maug_munlock( assets[i].handle, bitmap );
         maug_mfree( assets[i].handle );
         dropped_count++;
      }
   }
   maug_mzero( assets,
      sizeof( struct RETROFLAT_CACHE_ASSET ) * gs_retrogxc_sz );
   maug_munlock( gs_retrogxc_handle, assets );
   
   debug_printf( RETROGXC_TRACE_LVL,
      "graphics cache cleared (" SIZE_T_FMT " of " SIZE_T_FMT " assets)",
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

RETROGXC_ASSET_TYPE retrogxc_loader_bitmap(
   retroflat_asset_path res_p, MAUG_MHANDLE* handle_p, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_BITMAP* bitmap = NULL;

   assert( (MAUG_MHANDLE)NULL == *handle_p );

   *handle_p = maug_malloc( 1, sizeof( struct RETROFLAT_BITMAP ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *handle_p );

   maug_mlock( *handle_p, bitmap );
   maug_cleanup_if_null_alloc( struct RETROFLAT_BITMAP*, bitmap );

   /* Load requested bitmap into the cache. */
   retval = retroflat_load_bitmap( res_p, bitmap, flags );
   maug_cleanup_if_not_ok();

cleanup:

   if( NULL != bitmap ) {
      maug_munlock( *handle_p, bitmap );
   }

   if( MERROR_OK == retval ) {
      return RETROGXC_ASSET_TYPE_BITMAP;
   } else {
      if( NULL != *handle_p ) {
         maug_mfree( *handle_p );
      }
      return RETROGXC_ASSET_TYPE_NONE;
   }
}

/* === */

#ifdef RETROFLAT_XPM

RETROGXC_ASSET_TYPE retrogxc_loader_xpm(
   retroflat_asset_path res_p, MAUG_MHANDLE* handle_p, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_BITMAP* bitmap = NULL;

   assert( (MAUG_MHANDLE)NULL == *handle_p );

   *handle_p = maug_malloc( 1, sizeof( struct RETROFLAT_BITMAP ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *handle_p );

   maug_mlock( *handle_p, bitmap );
   maug_cleanup_if_null_alloc( struct RETROFLAT_BITMAP*, bitmap );

   /* Load requested bitmap into the cache. */
   retval = retroflat_load_xpm( res_p, bitmap, flags );
   maug_cleanup_if_not_ok();

cleanup:

   if( NULL != bitmap ) {
      maug_munlock( *handle_p, bitmap );
   }

   if( MERROR_OK == retval ) {
      return RETROGXC_ASSET_TYPE_BITMAP;
   } else {
      if( NULL != *handle_p ) {
         maug_mfree( *handle_p );
      }
      return RETROGXC_ASSET_TYPE_NONE;
   }
}

#endif /* RETROFLAT_XPM */

/* === */

int16_t retrogxc_load_asset(
   retroflat_asset_path res_p, retrogxc_loader l, uint8_t flags
) {
   int16_t idx = RETROGXC_ERROR_CACHE_MISS,
      i = 0;
   struct RETROFLAT_CACHE_ASSET* assets = NULL;
   RETROGXC_ASSET_TYPE asset_type = RETROGXC_ASSET_TYPE_NONE;

   maug_mlock( gs_retrogxc_handle, assets );

   /* Try to find the bitmap already in the cache. */
   for( i = 0 ; gs_retrogxc_sz > i ; i++ ) {
      if(
         RETROGXC_ASSET_TYPE_BITMAP == assets[i].type &&
         0 == retroflat_cmp_asset_path( assets[i].id, res_p )
      ) {
         idx = i;
         goto cleanup;
      }
   }

   /* Bitmap not found. */
   debug_printf( RETROGXC_TRACE_LVL,
      "asset %s not found in cache; loading...", res_p );
   for( i = 0 ; gs_retrogxc_sz > i ; i++ ) {
      /* Find a new empty slot. */
      if( RETROGXC_ASSET_TYPE_NONE != assets[i].type ) {
         continue;
      }

      debug_printf( RETROGXC_TRACE_LVL,
         "found free cache slot: %d (%p)", i, &(assets[i]) );

      /* Call the format-specific loader. */
      assert( (MAUG_MHANDLE)NULL == assets[i].handle );
      asset_type = l( res_p, &(assets[i].handle), flags );
      if( RETROGXC_ASSET_TYPE_NONE != asset_type ) {
         assets[i].type = asset_type;
         idx = i;
         debug_printf( RETROGXC_TRACE_LVL,
            "asset type %d, \"%s\" assigned cache ID: %d",
            asset_type, res_p, idx );
      }

      goto cleanup;
   }

   /* Still not found! */
   error_printf( "unable to load asset; cache full?" );

cleanup:

   if( NULL != assets ) {
      maug_munlock( gs_retrogxc_handle, assets );
   }

   return idx;
}

/* === */

MERROR_RETVAL retrogxc_blit_bitmap(
   struct RETROFLAT_BITMAP* target, size_t bitmap_idx,
   uint16_t s_x, uint16_t s_y, uint16_t d_x, uint16_t d_y,
   uint16_t w, uint16_t h
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_CACHE_ASSET* assets = NULL;
   struct RETROFLAT_BITMAP* bitmap = NULL;

   assert( 0 <= bitmap_idx );

   maug_mlock( gs_retrogxc_handle, assets );
   if( NULL == assets ) {
      error_printf( "could not lock cache!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   if( RETROGXC_ASSET_TYPE_BITMAP != assets[bitmap_idx].type ) {
      error_printf(
         "index " SIZE_T_FMT " not present in cache or not bitmap (%d)!",
         bitmap_idx, assets[bitmap_idx].type );
      retval = MERROR_FILE;
      goto cleanup;
   }

   maug_mlock( assets[bitmap_idx].handle, bitmap );

   retroflat_blit_bitmap( target, bitmap, s_x, s_y, d_x, d_y, w, h );

cleanup:

   if( NULL != bitmap ) {
      maug_munlock( assets[bitmap_idx].handle, bitmap );
   }

   if( NULL != assets ) {
      maug_munlock( gs_retrogxc_handle, assets );
   }

   return retval;
}

#endif /* RETROGXC_C */

#endif /* !RETROGXC_H */

