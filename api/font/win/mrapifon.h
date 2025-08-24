
#if !defined( MAUG_API_FON_H_DEFS )
#define MAUG_API_FON_H_DEFS

/* Windows only uses the RETROFONT struct during initial font load callback. */
struct RETROFONT {
   MAUG_MHANDLE* p_font_h;
   uint8_t glyph_h;
};

#if defined( RETROFNT_C )

MERROR_RETVAL retrofont_try_win(
   struct RETROFONT* font, const char* sub_name, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;
   LOGFONT lf;

   maug_mzero( &lf, sizeof( LOGFONT ) );
   lf.lfHeight = font->glyph_h;
   lstrcpy( lf.lfFaceName, sub_name );

   *(font->p_font_h) = (MAUG_MHANDLE)CreateFontIndirect( &lf );
   
   if( (MAUG_MHANDLE)NULL == *(font->p_font_h) ) {
      error_printf( "problem loading font: %s", sub_name );
      retval = MERROR_PARSE;
   } else {
      debug_printf( 2, "loaded font sub: %s", sub_name );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFONT load_capsule;

   if( 0 == glyph_h ) {
      glyph_h = retrofont_sz_from_filename( font_name ) + 2;
   }
   if( 0 == glyph_h ) {
      error_printf( "unable to determine font height!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   load_capsule.p_font_h = p_font_h;
   load_capsule.glyph_h = glyph_h;

   retval = retrofont_load_stub(
      font_name, &load_capsule, retrofont_try_win, NULL );
   maug_cleanup_if_not_ok();

cleanup:

   return retval;
}

/* === */

void retrofont_string_indent(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t x, size_t y,
   size_t max_w, size_t max_h, size_t indent, uint8_t flags
) {
   RECT wr;
   HFONT old_font;
   int fmt_flags = 0;
   
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   assert( (HBITMAP)NULL != target->b );

   assert( retroflat_bitmap_locked( target ) );

   if( 0 == str_sz ) {
      str_sz = maug_strlen( str );
   }

   /* Setup drawing params. */
   old_font = SelectObject( target->hdc_b, (HFONT)font_h );
   SetTextColor( target->hdc_b, g_retroflat_state->palette[color] );
   SetBkMode( target->hdc_b, TRANSPARENT );

   fmt_flags = DT_LEFT | DT_TOP;
   if( 0 == max_w || 0 == max_h ) {
      fmt_flags |= DT_NOCLIP;
   } else {
      fmt_flags |= DT_WORDBREAK; 
   }

   /* Size the drawing area. */
   wr.left = x;
   wr.top = y;
   wr.right = x + max_w;
   wr.bottom = y + max_h;
   DrawText( target->hdc_b, str, str_sz, &wr, fmt_flags );

   /* Cleanup drawing params. */
   /* If we don't do this, it makes stuff we draw later weirdly transparent. */
   SetBkMode( target->hdc_b, OPAQUE );
   SetTextColor( target->hdc_b, g_retroflat_state->palette[0] );
   SelectObject( target->hdc_b, old_font );
}

/* === */

MERROR_RETVAL retrofont_string_sz(
   retroflat_blit_t* target, const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, retroflat_pxxy_t max_w, retroflat_pxxy_t max_h,
   retroflat_pxxy_t* p_out_w, retroflat_pxxy_t* p_out_h, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   RECT wr;
   HFONT old_font;
   int fmt_flags = 0;

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   assert( (HBITMAP)NULL != target->b );

   assert( retroflat_bitmap_locked( target ) );

   if( 0 == str_sz ) {
      str_sz = maug_strlen( str );
   }

   if( 0 == str_sz ) {
      /* Short-circuit if no actual text! */
      return retval;
   }

   old_font = SelectObject( target->hdc_b, (HFONT)font_h );

   fmt_flags = DT_CALCRECT | DT_WORDBREAK;
   if( 0 == max_w || 0 == max_h ) {
      fmt_flags |= DT_NOCLIP;
   }

   wr.left = 0;
   wr.top = 0;
   wr.right = max_w;
   wr.bottom = max_h;
   DrawText( target->hdc_b, str, str_sz, &wr, fmt_flags );

   if( NULL != p_out_w ) {
      *p_out_w = wr.right - wr.left;
   }

   if( NULL != p_out_h ) {
      *p_out_h = wr.bottom - wr.top;
   }

   SelectObject( target->hdc_b, old_font );

   return retval;
}

/* === */

void retrofont_free( MAUG_MHANDLE* p_font_h ) {
   DeleteObject( (HFONT)(*p_font_h) );
   *p_font_h = (MAUG_MHANDLE)NULL;
}

#endif /* !RETROFNT_C */

#endif /* !MAUG_API_FON_H_DEFS */

