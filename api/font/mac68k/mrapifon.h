
#if !defined( MAUG_API_FON_H_DEFS )
#define MAUG_API_FON_H_DEFS

/**
 * \brief Flag for retroflat_string() and retroflat_string_sz() to print
 *        text as outline-only.
 * \todo This has not yet been implemented and is present for backward
 *       compatibility.
 */
#define RETROFONT_FLAG_OUTLINE  0x04

struct RETROFONT {
   uint8_t flag;
   uint8_t font_idx;
   uint8_t font_sz;
   uint8_t font_height;
};

#if defined( RETROFNT_C )

MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFONT* font = NULL;
   FontInfo font_info;

   *p_font_h = maug_malloc( 1, sizeof( struct RETROFONT ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_font_h );

   maug_mlock( *p_font_h, font );
   maug_cleanup_if_null_lock( struct RETROFONT*, font );

   font->font_sz = glyph_h;

   GetFontInfo( &font_info );
   font->font_height = font_info.ascent + font_info.descent + font_info.leading;

cleanup:

   if( NULL != font ) {
      maug_munlock( *p_font_h, font );
   }

   return retval;
}

/* === */

void retrofont_string(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t x, size_t y,
   size_t max_w, size_t max_h, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   unsigned char str_buf[128];
   struct RETROFONT* font = NULL;

   maug_mlock( font_h, font );
   maug_cleanup_if_null_lock( struct RETROFONT*, font );

   retval = maug_str_c2p( str, (char*)str_buf, 127 );
   maug_cleanup_if_not_ok();

   /* TODO: Handle font face and effects! */

   MoveTo( x, y + font->font_height );

   if( RETROFLAT_COLOR_BLACK == color ) {
      ForeColor( blackColor );
   } else {
      if( RETROFLAT_COLOR_WHITE != color ) {
         debug_printf(
            RETRO2D_TRACE_LVL, "alert! high color used: %d", color );
      }
      ForeColor( whiteColor );
   }

   DrawString( str_buf );

   ForeColor( blackColor );

cleanup:

   if( NULL != font ) {
      maug_munlock( font_h, font );
   }

   return;
}

/* === */

MERROR_RETVAL retrofont_string_sz(
   retroflat_blit_t* target, const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t max_w, size_t max_h,
   size_t* p_out_w, size_t* p_out_h, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   unsigned char str_buf[128];
   struct RETROFONT* font = NULL;

   maug_mlock( font_h, font );
   maug_cleanup_if_null_lock( struct RETROFONT*, font );

   retval = maug_str_c2p( str, (char*)str_buf, 127 );
   maug_cleanup_if_not_ok();

   *p_out_w = StringWidth( str_buf );

   /* TODO: Handle wrapping and newlines! */
   *p_out_h = font->font_height;

cleanup:

   if( NULL != font ) {
      maug_munlock( font_h, font );
   }

   return retval;
}

/* === */

void retrofont_free( MAUG_MHANDLE* p_font_h ) {
   maug_mfree( *p_font_h );
}

#endif /* !RETROFNT_C */

#endif /* !MAUG_API_FON_H_DEFS */

