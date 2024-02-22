
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
   uint8_t glyph_h;
   uint8_t glyph_sz;
};

MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count );

void retrofont_blit_glyph(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   char c, struct RETROFONT* font, int x, int y, uint8_t flags

void retrofont_string(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   struct RETROFONT* font, int x, int y, uint8_t flags );

/**
 * \brief Get a pointer to the glyph with the given index in the given font.
 */
#define retrofont_glyph_at( p_font, idx ) \
   &(((uint8_t*)(p_font))[(p_font)->sz + \
      ((p_font)->glyph_sz * (idx - (p_font)->first_glyph))])

#ifdef RETROFNT_C

void retrofont_dump_glyph( uint8_t* glyph, uint8_t w, uint8_t h ) {
   size_t x = 0, y = 0;
   char glyph_bin[65];

   for( y = 0 ; h > y ; y++ ) {
      memset( glyph_bin, '\0', 65 );
      
      for( x = 0 ; w > x ; x++ ) {
         glyph_bin[x] = 1 << (w - x) == (glyph[y] & (1 << (w - x))) ? 'x' : '.';
      }
      
      debug_printf( 1, "%s", glyph_bin );
   }
}

MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count
) {
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t font_file;
   char line[RETROFONT_LINE_SZ];
   struct RETROFONT* font = NULL;
   char* line_bytes = NULL;
   uint16_t glyph_idx = 0;
   uint8_t* p_glyph = NULL;
   size_t i = 0;
   uint8_t glyph_w_bytes = 0;
   uint8_t glyph_w = 0;

   /* Try to separate the string into index:glyph bytes.  */
   #define retrofont_split_glyph_line( line, line_bytes ) \
      line_bytes = strchr( line, ':' ); \
      if( NULL == line_bytes ) { \
         error_printf( "invalid line: %s", line ); \
         retval = MERROR_PARSE; \
      } \
      line_bytes[0] = '\0'; \
      line_bytes++;

   retval = mfile_open_read( font_name, &font_file );
   maug_cleanup_if_not_ok();

   /* Figure out font width from file and alloc just enough. */
   retval = mfile_read_line( &font_file, line, RETROFONT_LINE_SZ );
   maug_cleanup_if_not_ok();
   retrofont_split_glyph_line( line, line_bytes );
   maug_cleanup_if_not_ok();
   glyph_w_bytes = (strlen( line_bytes ) / glyph_h) / 2; /* 2 hex per byte */
   debug_printf( 2, "glyph_w_bytes: %u", glyph_w_bytes );
   glyph_w = glyph_w_bytes * 8;

   debug_printf( 2, "glyph_w: %u, glyph_sz: %u",
      glyph_w, glyph_h * glyph_w_bytes );

   /* Alloc enough for each glyph, plus the size of the font header. */
   *p_font_h = maug_malloc( 1,
      sizeof( struct RETROFONT ) +
      (glyph_h * glyph_w_bytes * (1 + glyphs_count)) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_font_h );

   debug_printf( 2, "allocated font %s: " SIZE_T_FMT " bytes",
      font_name, (glyph_h * glyph_w_bytes * (1 + glyphs_count)) + 
         sizeof( struct RETROFONT ) );

   maug_mlock( *p_font_h, font );
   maug_cleanup_if_null_alloc( struct RETROFONT*, font );

   /* Set initial font parameters. */
   font->sz = sizeof( struct RETROFONT );
   font->first_glyph = first_glyph;
   font->glyph_w = glyph_w;
   font->glyph_h = glyph_h;
   font->glyph_sz = glyph_h * glyph_w_bytes;

   while( mfile_has_bytes( &font_file ) ) {
      retval = mfile_read_line( &font_file, line, RETROFONT_LINE_SZ );
      
      retrofont_split_glyph_line( line, line_bytes );
      if( MERROR_PARSE == retval ) {
         retval = MERROR_OK;
         continue;
      }

      /* Figure out the index of this glyph. */
      /* TODO: Maug replacement for C99 crutch. */
      glyph_idx = strtoul( line, NULL, 16 );
      if( glyph_idx < first_glyph || glyph_idx > first_glyph + glyphs_count ) {
         /* Skip glyph out of range. */
         continue;
      }

      /* Find where to put the decoded glyph. */
      p_glyph = retrofont_glyph_at( font, glyph_idx );

      for( i = 0 ; font->glyph_h > i ; i++ ) {
         switch( font->glyph_w ) {
         case 8:
            p_glyph[i] = 0;
            p_glyph[i] |= maug_hctoi( line_bytes[i * 2] ) << 4;
            p_glyph[i] |= maug_hctoi( line_bytes[(i * 2) + 1] );
            break;

         case 16:
            /* TODO */
            break;

         case 32:
            /* TODO */
            break;

         default:
            error_printf( "invalid font width: %u", font->glyph_w );
            retval = MERROR_PARSE;
            goto cleanup;
         }
      }

      if( glyph_idx == 'B' ) {
         retrofont_dump_glyph( p_glyph, glyph_w, glyph_h );
      }

      font->glyphs_count++;

      debug_printf( 1,
         "%u %s (" SIZE_T_FMT " hbytes)", glyph_idx - first_glyph, line_bytes,
         strlen( line_bytes ) );
   }

cleanup:

   if( NULL != font ) {
      maug_munlock( *p_font_h, font );
   }

   mfile_close( &font_file );

   return retval;
}

void retrofont_blit_glyph(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   char c, struct RETROFONT* font, int x, int y, uint8_t flags
) {
   uint8_t* glyph = retrofont_glyph_at( font, c );
   int16_t x_iter, y_iter;

   for( y_iter = 0 ; font->glyph_h > y_iter ; y_iter++ ) {
      for( x_iter = 0 ; font->glyph_w > x_iter ; x_iter++ ) {
         if( 
            1 << (font->glyph_w - x_iter) ==
            (glyph[y_iter] & (1 << (font->glyph_w - x_iter)))
         ) {
            retroflat_px( target, color, x + x_iter, y + y_iter, 0 );
         }
      }
   }
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

      retrofont_blit_glyph( target, color, str[i], font, x_iter, y, flags );

      x_iter += font->glyph_w;
   }
}


#endif /* RETROFNT_C */

/* \} */ /* retrofnt */

#endif /* !RETROFNT_H */

