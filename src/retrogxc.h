
#ifndef RETROGXC_H
#define RETROGXC_H

#include <maug.h>

/* TODO: Unified asset type that automatically uses RetroGXC if present and
 *       directly loads assets to handles if not.
 *
 *       *or*
 *
 *       Loader function that passes back a pointer to the cached object
 *       to be used with the normal functions associated with it.
 */

/* TODO: The loaders here pass path to file-loading functions. What they
 *       *should* do is open those paths into mfiles on their own, so that
 *       a shim can be inserted to force loading from arrays in memory.
 */

#define RETROGXC_PRESENT 1

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
   retrogxc_load_asset( res_p, retrogxc_loader_bitmap, NULL, flags )

typedef int8_t RETROGXC_ASSET_TYPE;

typedef RETROGXC_ASSET_TYPE (*retrogxc_loader)(
   const retroflat_asset_path res_p, MAUG_MHANDLE* handle_p,
   void* data, uint8_t flags );

struct RETROFLAT_CACHE_ASSET {
   uint8_t type;
   MAUG_MHANDLE handle;
   retroflat_asset_path id;
};

struct RETROGXC_FONT_PARMS {
   uint8_t glyph_h;
   uint16_t first_glyph;
   uint16_t glyphs_count;
};

MERROR_RETVAL retrogxc_init();

void retrogxc_clear_cache();

void retrogxc_shutdown();

RETROGXC_ASSET_TYPE retrogxc_loader_bitmap(
   const retroflat_asset_path res_p, MAUG_MHANDLE* handle_p,
   void* data, uint8_t flags );

RETROGXC_ASSET_TYPE retrogxc_loader_xpm(
   const retroflat_asset_path res_p, MAUG_MHANDLE* handle_p,
   void* data, uint8_t flags );

RETROGXC_ASSET_TYPE retrogxc_loader_font(
   const retroflat_asset_path res_p, MAUG_MHANDLE* handle_p,
   void* data, uint8_t flags );

int16_t retrogxc_load_font(
   const retroflat_asset_path font_name,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count );

int16_t retrogxc_load_asset(
   const retroflat_asset_path res_p, retrogxc_loader l, void* data,
   uint8_t flags );

MERROR_RETVAL retrogxc_blit_bitmap(
   retroflat_blit_t* target, size_t bitmap_idx,
   size_t s_x, size_t s_y, size_t d_x, size_t d_y,
   size_t w, size_t h, int16_t instance );

MERROR_RETVAL retrogxc_string(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   size_t font_idx, size_t x, size_t y,
   size_t max_w, size_t max_h, uint8_t flags );

MERROR_RETVAL retrogxc_string_sz(
   retroflat_blit_t* target, const char* str, size_t str_sz,
   size_t font_idx, size_t max_w, size_t max_h,
   size_t* out_w_p, size_t* out_h_p, uint8_t flags );

MERROR_RETVAL retrogxc_bitmap_wh(
   size_t bitmap_idx, retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h );

#ifdef RETROGXC_C

static struct MDATA_VECTOR gs_retrogxc_bitmaps;

/* === */

MERROR_RETVAL retrogxc_init() {
   MERROR_RETVAL retval = MERROR_OK;

   /*
   size_printf( RETROGXC_TRACE_LVL,
      "asset struct", sizeof( struct RETROFLAT_CACHE_ASSET ) );
   size_printf( RETROGXC_TRACE_LVL, "initial graphics cache",
      sizeof( struct RETROFLAT_CACHE_ASSET ) * gs_retrogxc_sz );
   */

   return retval;
}

/* === */

void retrogxc_clear_cache() {
   size_t dropped_count = 0;
   struct RETROFLAT_CACHE_ASSET* asset = NULL;
   retroflat_blit_t* bitmap = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   mdata_vector_lock( &gs_retrogxc_bitmaps );

   while( 0 < mdata_vector_ct( &gs_retrogxc_bitmaps ) ) {
      asset = mdata_vector_get(
         &gs_retrogxc_bitmaps, 0, struct RETROFLAT_CACHE_ASSET );
      assert( NULL != asset );

      /* Asset-type-specific cleanup. */
      switch( asset->type ) {
      case RETROGXC_ASSET_TYPE_BITMAP:
         maug_mlock( asset->handle, bitmap );
         if( NULL != bitmap ) {
            retroflat_2d_destroy_bitmap( bitmap );
         }
         maug_munlock( asset->handle, bitmap );
         maug_mfree( asset->handle );
         dropped_count++;

      case RETROGXC_ASSET_TYPE_FONT:
         /* Fonts are just a blob of data after a struct, so just free it! */
         maug_mfree( asset->handle );
         dropped_count++;
      }

      mdata_vector_unlock( &gs_retrogxc_bitmaps );
      mdata_vector_remove( &gs_retrogxc_bitmaps, 0 );
      mdata_vector_lock( &gs_retrogxc_bitmaps );
   }
   
   debug_printf( RETROGXC_TRACE_LVL,
      "graphics cache cleared (" SIZE_T_FMT " assets)", dropped_count );

cleanup:

   if( MERROR_OK == retval ) {
      mdata_vector_unlock( &gs_retrogxc_bitmaps );
   }

   return;
}

/* === */

void retrogxc_shutdown() {
   retrogxc_clear_cache();
   mdata_vector_free( &gs_retrogxc_bitmaps );
}

/* === */

RETROGXC_ASSET_TYPE retrogxc_loader_bitmap(
   const retroflat_asset_path res_p, MAUG_MHANDLE* handle_p, void* data,
   uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   retroflat_blit_t* bitmap = NULL;

   assert( (MAUG_MHANDLE)NULL == *handle_p );

   *handle_p = maug_malloc( 1, sizeof( retroflat_blit_t ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *handle_p );

   maug_mlock( *handle_p, bitmap );
   maug_cleanup_if_null_alloc( retroflat_blit_t*, bitmap );

   /* Load requested bitmap into the cache. */
#ifdef RETROFLAT_XPM
   retval = retroflat_load_xpm( res_p, bitmap, flags );
#else
   retval = retroflat_2d_load_bitmap( res_p, bitmap, flags );
#endif /* RETROFLAT_XPM */
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

#ifdef RETROFONT_PRESENT

RETROGXC_ASSET_TYPE retrogxc_loader_font(
   const retroflat_asset_path res_p, MAUG_MHANDLE* handle_p, void* data,
   uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROGXC_FONT_PARMS* parms = (struct RETROGXC_FONT_PARMS*)data;

   assert( (MAUG_MHANDLE)NULL == *handle_p );

   debug_printf( 1, "loading font into cache: %s (%d, %d, %d)",
      res_p, parms->glyph_h, parms->first_glyph, parms->glyphs_count );

   retval = retrofont_load( res_p, handle_p,
      parms->glyph_h, parms->first_glyph, parms->glyphs_count );
   maug_cleanup_if_not_ok();

cleanup:

   if( MERROR_OK == retval ) {
      return RETROGXC_ASSET_TYPE_FONT;
   } else {
      return RETROGXC_ASSET_TYPE_NONE;
   }
}

#endif /* RETROFONT_PRESENT */

/* === */

int16_t retrogxc_load_asset(
   const retroflat_asset_path res_p, retrogxc_loader l, void* data,
   uint8_t flags
) {
   int16_t idx = RETROGXC_ERROR_CACHE_MISS,
      i = 0;
   struct RETROFLAT_CACHE_ASSET asset_new;
   struct RETROFLAT_CACHE_ASSET* asset_iter = NULL;
   RETROGXC_ASSET_TYPE asset_type = RETROGXC_ASSET_TYPE_NONE;
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( &asset_new, sizeof( struct RETROFLAT_CACHE_ASSET ) );

   if( 0 == mdata_vector_ct( &gs_retrogxc_bitmaps ) ) {
      goto just_load_asset;
   }

   /* Try to find the bitmap already in the cache. */
   mdata_vector_lock( &gs_retrogxc_bitmaps );
   for( i = 0 ; mdata_vector_ct( &gs_retrogxc_bitmaps ) > (size_t)i ; i++ ) {
      asset_iter = mdata_vector_get(
         &gs_retrogxc_bitmaps, i, struct RETROFLAT_CACHE_ASSET );
      assert( NULL != asset_iter );
      debug_printf( RETROGXC_TRACE_LVL, "\"%s\" vs \"%s\"",
        asset_iter->id, res_p );
      if( 0 == retroflat_cmp_asset_path( asset_iter->id, res_p ) ) {
         debug_printf( RETROGXC_TRACE_LVL,
            "found asset \"%s\" at index %d with type %d!",
            res_p, i, asset_iter->type );
         idx = i;
         mdata_vector_unlock( &gs_retrogxc_bitmaps );
         goto cleanup;
      }
   }
   mdata_vector_unlock( &gs_retrogxc_bitmaps );

just_load_asset:

   /* Bitmap not found. */
   debug_printf( RETROGXC_TRACE_LVL,
      "asset %s not found in cache; loading...", res_p );

   /* Call the format-specific loader. */
   asset_type = l( res_p, &asset_new.handle, data, flags );
   if( RETROGXC_ASSET_TYPE_NONE != asset_type ) {
      asset_new.type = asset_type;
      retroflat_assign_asset_path( asset_new.id, res_p );
      idx = mdata_vector_append(
         &gs_retrogxc_bitmaps, &asset_new,
         sizeof( struct RETROFLAT_CACHE_ASSET ) );
      if( 0 > idx ) {
         goto cleanup;
      }
      debug_printf( RETROGXC_TRACE_LVL,
         "asset type %d, \"%s\" assigned cache ID: %d",
         asset_type, res_p, idx );
      goto cleanup;
   }

   /* Still not found! */
   error_printf( "unable to load asset; cache full or not initialized?" );

cleanup:

   if( MERROR_OK != retval ) {
      idx = retval * -1;
   }

   return idx;
}

/* === */

MERROR_RETVAL retrogxc_blit_bitmap(
   retroflat_blit_t* target, size_t bitmap_idx,
   size_t s_x, size_t s_y, size_t d_x, size_t d_y,
   size_t w, size_t h, int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_CACHE_ASSET* asset = NULL;
   retroflat_blit_t* bitmap = NULL;

   assert( NULL != gs_retrogxc_bitmaps.data_h );

   mdata_vector_lock( &gs_retrogxc_bitmaps );

   if( mdata_vector_ct( &gs_retrogxc_bitmaps ) <= bitmap_idx ) {
      error_printf( "invalid bitmap index: " SIZE_T_FMT, bitmap_idx );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   asset = mdata_vector_get(
      &gs_retrogxc_bitmaps, bitmap_idx, struct RETROFLAT_CACHE_ASSET );

   if( RETROGXC_ASSET_TYPE_BITMAP != asset->type ) {
      error_printf(
         "index " SIZE_T_FMT " not present in cache or not bitmap (%d)!",
         bitmap_idx, asset->type );
      retval = MERROR_FILE;
      goto cleanup;
   }

   maug_mlock( asset->handle, bitmap );

   retval = retroflat_2d_blit_bitmap(
      target, bitmap, s_x, s_y, d_x, d_y, w, h, instance );

cleanup:

   if( NULL != bitmap ) {
      maug_munlock( asset->handle, bitmap );
   }

   mdata_vector_unlock( &gs_retrogxc_bitmaps );

   return retval;
}

/* === */

MERROR_RETVAL retrogxc_bitmap_wh(
   size_t bitmap_idx, retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_CACHE_ASSET* asset = NULL;
   retroflat_blit_t* bitmap = NULL;

   mdata_vector_lock( &gs_retrogxc_bitmaps );

   if( mdata_vector_ct( &gs_retrogxc_bitmaps ) <= bitmap_idx ) {
      error_printf( "invalid bitmap index: " SIZE_T_FMT, bitmap_idx );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   asset = mdata_vector_get(
      &gs_retrogxc_bitmaps, bitmap_idx, struct RETROFLAT_CACHE_ASSET );

   if( RETROGXC_ASSET_TYPE_BITMAP != asset->type ) {
      error_printf(
         "index " SIZE_T_FMT " not present in cache or not bitmap (%d)!",
         bitmap_idx, asset->type );
      retval = MERROR_FILE;
      goto cleanup;
   }

   maug_mlock( asset->handle, bitmap );

   if( NULL != p_w ) {
      *p_w = retroflat_2d_bitmap_w( bitmap );
   }

   if( NULL != p_h ) {
      *p_h = retroflat_2d_bitmap_h( bitmap );
   }

cleanup:

   if( NULL != bitmap ) {
      maug_munlock( asset->handle, bitmap );
   }

   mdata_vector_unlock( &gs_retrogxc_bitmaps );

   return retval;
}

/* === */

#ifdef RETROFONT_PRESENT

int16_t retrogxc_load_font(
   const retroflat_asset_path font_name,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count 
) {
   int16_t idx = -1;
   struct RETROGXC_FONT_PARMS parms;

   parms.glyph_h = glyph_h;
   parms.first_glyph = first_glyph;
   parms.glyphs_count = glyphs_count;

   idx = retrogxc_load_asset( font_name, retrogxc_loader_font, &parms, 0 );

   return idx;
}

/* === */

MERROR_RETVAL retrogxc_string(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   size_t font_idx, size_t x, size_t y,
   size_t max_w, size_t max_h, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_CACHE_ASSET* asset = NULL;

   mdata_vector_lock( &gs_retrogxc_bitmaps );

   if( mdata_vector_ct( &gs_retrogxc_bitmaps ) <= font_idx ) {
      error_printf( "invalid font index: " SIZE_T_FMT, font_idx );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   asset = mdata_vector_get(
      &gs_retrogxc_bitmaps, font_idx, struct RETROFLAT_CACHE_ASSET );

   if( RETROGXC_ASSET_TYPE_FONT != asset->type ) {
      error_printf(
         "index " SIZE_T_FMT " not present in cache or not font (%d)!",
         font_idx, asset->type );
      retval = MERROR_FILE;
      goto cleanup;
   }

   retrofont_string(
      target, color, str, str_sz, asset->handle, x, y, max_w, max_h, flags );

cleanup:

   mdata_vector_unlock( &gs_retrogxc_bitmaps );

   return retval;
}

/* === */

MERROR_RETVAL retrogxc_string_sz(
   retroflat_blit_t* target, const char* str, size_t str_sz,
   size_t font_idx, size_t max_w, size_t max_h,
   size_t* out_w_p, size_t* out_h_p, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_CACHE_ASSET* asset = NULL;

   mdata_vector_lock( &gs_retrogxc_bitmaps );

   if( mdata_vector_ct( &gs_retrogxc_bitmaps ) <= font_idx ) {
      error_printf( "invalid font index: " SIZE_T_FMT, font_idx );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   asset = mdata_vector_get(
      &gs_retrogxc_bitmaps, font_idx, struct RETROFLAT_CACHE_ASSET );

   if( RETROGXC_ASSET_TYPE_FONT != asset->type ) {
      error_printf(
         "index " SIZE_T_FMT " not present in cache or not font (%d)!",
         font_idx, asset->type );
      retval = MERROR_FILE;
      goto cleanup;
   }

   retrofont_string_sz(
      target, str, str_sz, asset->handle,
      max_w, max_h, out_w_p, out_h_p, flags );

cleanup:

   mdata_vector_unlock( &gs_retrogxc_bitmaps );

   return retval;
}

#endif /* RETROFONT_PRESENT */

#endif /* RETROGXC_C */

#endif /* !RETROGXC_H */

