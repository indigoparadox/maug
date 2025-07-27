
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
   uint8_t font_sz;
   uint8_t font_height;
   int16_t font_idx;
};

#if defined( RETROFNT_C )

MERROR_RETVAL retrofont_try_mac68k(
   struct RETROFONT* font, const char* sub_name, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;
   Str255 sub_name_buf;

   maug_str_c2p( sub_name, (char*)sub_name_buf, 254 );
      
   GetFNum( sub_name_buf, &(font->font_idx) );

   if( 0 == font->font_idx ) {
      error_printf( "problem loading font: \"%s\"", &(sub_name_buf[1]) );
      retval = MERROR_PARSE;
   } else {
      debug_printf( 2, "loaded font sub: \"%s\"", &(sub_name_buf[1]) );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFONT* font = NULL;
   FontInfo font_info;

   if( 0 == glyph_h ) {
      glyph_h = retrofont_sz_from_filename( font_name );
   }
   if( 0 == glyph_h ) {
      error_printf( "unable to determine font height!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   *p_font_h = maug_malloc( 1, sizeof( struct RETROFONT ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_font_h );

   maug_mlock( *p_font_h, font );
   maug_cleanup_if_null_lock( struct RETROFONT*, font );

   font->font_sz = glyph_h;

   retval = retrofont_load_stub( font_name, font, retrofont_try_mac68k, NULL );
   maug_cleanup_if_not_ok();

   TextFont( font->font_idx );
   TextSize( font->font_sz );

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
   size_t i = 0, str_start = 0, y_offset = 0, line_sz = 0;

   if( 0 >= str_sz ) {
      str_sz = strlen( str );
   }

   maug_mlock( font_h, font );
   maug_cleanup_if_null_lock( struct RETROFONT*, font );

   /* Handle font face and effects! */
   TextFont( font->font_idx );
   TextSize( font->font_sz );
   if( RETROFLAT_COLOR_BLACK == color ) {
      ForeColor( blackColor );
   } else {
      if( RETROFLAT_COLOR_WHITE != color ) {
         debug_printf(
            RETRO2D_TRACE_LVL, "alert! high color used: %d", color );
      }
      ForeColor( whiteColor );
   }

   /* Break up draw calls for wrapping and newlines. */
   for( i = 0 ; str_sz >= i ; i++ ) {

      /* TODO: Iterate and check string width at each space to keep it within
       *       max_w and wrap when it's not.
       */

      if( '\n' != str[i] && '\r' != str[i] && str_sz != i ) {
         continue;
      }

      /* Start first line from font_height offset due to quirks of MoveTo(). */
      y_offset += font->font_height;

      MoveTo( x, y + y_offset );

      line_sz = i - str_start;

      if( line_sz >= 127 ) {
         error_printf( "string too long (%d): %s", line_sz, &(str[str_start]) );
         goto cleanup;
      }

      debug_printf( 1,
         "drawing (%d at %d): %s",
         line_sz, str_start, &(str[str_start]) );

      /* We're using this to chop out a section of the string, which will
       * cause the retval to always be not-OK, so ignore it here.
       *
       * Also add +1 to line_sz for the pascal size byte.
       */
      maug_str_c2p(
         &(str[str_start]), (char*)str_buf, line_sz + 1 );

      DrawString( str_buf );

      /* Place the new start after the line-terminating char. */
      str_start = i + 1;
   }

   /* Reset drawing effects. */
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
   size_t i = 0;

   maug_mlock( font_h, font );
   maug_cleanup_if_null_lock( struct RETROFONT*, font );

   retval = maug_str_c2p( str, (char*)str_buf, 127 );
   maug_cleanup_if_not_ok();

   TextSize( font->font_sz );

   *p_out_w = StringWidth( str_buf );

   *p_out_h = font->font_height;

   /* Handle wrapping and newlines. */
   for( i = 0 ; str_sz > i ; i++ ) {
      /* TODO: Iterate and check string width at each space to keep it within
       *       max_w and wrap when it's not.
       */
      if( '\n' == i || '\r' == i ) {
         *p_out_h += font->font_height;
      }
   }

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

