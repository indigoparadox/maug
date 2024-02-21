
#ifndef RETROFNT_H
#define RETROFNT_H

/**
 * \addtogroup retrofont RetroFont API
 * \{
 * \file retrofnt.h
 */

#ifndef RETROFONT_LINE_SZ
#  define RETROFONT_LINE_SZ 80
#endif /* !RETROFONT_LINE_SZ */

struct RETROFONT {
   uint16_t sz;
   uint16_t first_glyph;
   uint16_t glyphs_count;
   uint8_t glyph_w;
};

MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint16_t first_glyph, uint16_t glyphs_count );

void retrofont_string(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   struct RETROFONT* font, int x, int y, uint8_t flags );

/**
 * \brief Get a pointer to the glyph with the given index in the given font.
 */
#define retrofont_glyph_at( p_font, idx ) \
   (uint16_t*)(&(((uint8_t*)(p_font))[(p_font)->sz + ((idx - (p_font)->first_glyph) << 1)]))

#ifdef RETROFNT_C

MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint16_t first_glyph, uint16_t glyphs_count
) {
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t font_file;
   char line[RETROFONT_LINE_SZ];
   struct RETROFONT* font = NULL;
   char* line_bytes = NULL;
   uint16_t glyph_idx = 0;
   uint16_t* p_glyph = NULL;

   /* Unifont fonts are implicitly 16 pixels high. */
   *p_font_h = maug_malloc( 16, glyphs_count );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_font_h );

   debug_printf( 2, "allocated font %s: " SIZE_T_FMT " bytes",
      font_name, (16 * glyphs_count * 2) + sizeof( struct RETROFONT ) );

   maug_mlock( *p_font_h, font );
   maug_cleanup_if_null_alloc( struct RETROFONT*, font );

   /* Set initial font parameters. */
   font->sz = sizeof( struct RETROFONT );
   font->first_glyph = first_glyph;

   retval = mfile_open_read( font_name, &font_file );
   maug_cleanup_if_not_ok();

   while( mfile_has_bytes( &font_file ) ) {
      retval = mfile_read_line( &font_file, line, RETROFONT_LINE_SZ );
      
      /* Try to separate the string into index:glyph bytes. */
      line_bytes = strchr( line, ':' );
      if( NULL == line_bytes ) {
         error_printf( "invalid line: %s", line );
         continue;
      }
      line_bytes[0] = '\0';
      line_bytes++;

      /* Figure out the index of this glyph. */
      /* TODO: Maug replacement for C99 crutch. */
      glyph_idx = strtoul( line, NULL, 16 );
      if( glyph_idx < first_glyph || glyph_idx > first_glyph + glyphs_count ) {
         /* Skip glyph out of range. */
         continue;
      }

      /* Glyph width. */
      font->glyph_w = strlen( line_bytes ) / 2;
      assert( 0 == font->glyph_w % 8 );

      p_glyph = retrofont_glyph_at( font, glyph_idx );
      *p_glyph = glyph_idx;

      font->glyphs_count++;

      debug_printf( 1,
         "%u %s (" SIZE_T_FMT " bytes) (%p)", glyph_idx, line_bytes,
         strlen( line_bytes ), p_glyph );
   }

cleanup:

   mfile_close( &font_file );

   return retval;
}

void retrofont_string(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   struct RETROFONT* font, int x, int y, uint8_t flags
) {
   size_t i = 0;
   int16_t x_iter = x;

   if( 0 == str_sz ) {
      str_sz = strlen( str );
   }

   for( i = 0 ; str_sz > i ; i++ ) {
      /* Terminate prematurely at null. */
      if( '\0' == str[i] ) {
         break;
      }

      /* TODO */

      x_iter += font->glyph_w;
   }
}


#endif /* RETROFNT_C */

/* \} */ /* retrofnt */

#endif /* !RETROFNT_H */

