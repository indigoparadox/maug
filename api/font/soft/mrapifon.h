
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
   uint16_t sz;
   uint16_t first_glyph;
   uint16_t glyphs_count;
   uint8_t glyph_w;
   uint8_t glyph_h;
   uint8_t glyph_sz;
};

#if defined( RETROFNT_C )

/**
 * \brief Get a pointer to the glyph with the given index in the given font.
 */
#define retrofont_glyph_at( p_font, idx ) \
   &(((uint8_t*)(p_font))[(p_font)->sz + \
      ((p_font)->glyph_sz * (idx - (p_font)->first_glyph))])

/* === */

void retrofont_dump_glyph( uint8_t* glyph, uint8_t w, uint8_t h ) {
   size_t x = 0, y = 0;
   char glyph_bin[65];

   for( y = 0 ; h > y ; y++ ) {
      memset( glyph_bin, '\0', 65 );
      
      for( x = 0 ; w > x ; x++ ) {
         glyph_bin[x] = 1 << (w - x) == (glyph[y] & (1 << (w - x))) ? 'x' : '.';
      }
      
      debug_printf( RETROFONT_TRACE_LVL, "%s", glyph_bin );
   }
}

/* === */

MERROR_RETVAL retrofont_load_glyph(
   mfile_t* font_file, struct RETROFONT* font,
   size_t* p_glyph_idx, char* glyph_idx_str, char** p_glyph_bytes
) {
   MERROR_RETVAL retval = MERROR_OK;

   retval = font_file->read_line(
      font_file, glyph_idx_str, RETROFONT_LINE_SZ, 0 );
   maug_cleanup_if_not_ok();

   *p_glyph_bytes = maug_strchr( glyph_idx_str, ':' );
   if( NULL == *p_glyph_bytes ) { \
      error_printf( "invalid line: %s", glyph_idx_str );
      /* The line couldn't parse, so skip it, but don't give up entirely and
       * keep going to the next line.
       */
      retval = MERROR_WAIT;
      goto cleanup;
   }

   /* Replace : with NULL to split the string and move pointer to actual bytes.
    */
   (*p_glyph_bytes)[0] = '\0'; \
   (*p_glyph_bytes)++;

   /* Hunt for sub statements. */
   if( 0 == strncmp( "SUB", glyph_idx_str, 3 ) ) {
      debug_printf( RETROFONT_TRACE_LVL, "found sub: %s", *p_glyph_bytes );
      retval = MERROR_WAIT;
      goto cleanup;
   }

   /* Figure out the index of this glyph. */
   *p_glyph_idx = maug_atou32( glyph_idx_str, 0, 16 );

   debug_printf(
      RETROFONT_TRACE_LVL, "found glyph: " SIZE_T_FMT, *p_glyph_idx );

cleanup:
   return retval;
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
   size_t glyph_idx = 0;
   uint8_t* p_glyph = NULL;
   size_t i = 0;
   uint8_t glyph_w_bytes = 0;
   uint8_t glyph_w = 0;

   maug_mzero( &font_file, sizeof( mfile_t ) );

   /* TODO: Font loading seems to be very slow on a 486. This needs
    *       investigation.
    */

   if( 0 == glyph_h ) {
      glyph_h = retrofont_sz_from_filename( font_name );
   }
   if( 0 == glyph_h ) {
      error_printf( "unable to determine font height!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Try to separate the string into index:glyph bytes.  */
   #define retrofont_split_glyph_line( line, line_bytes ) \
      line_bytes = maug_strchr( line, ':' ); \
      if( NULL == line_bytes ) { \
         error_printf( "invalid line: %s", line ); \
         retval = MERROR_PARSE; \
      } \
      line_bytes[0] = '\0'; \
      line_bytes++;

   retval = mfile_open_read( font_name, &font_file );
   maug_cleanup_if_not_ok();

   debug_printf( RETROFONT_TRACE_LVL, "font file opened, reading..." );

   /* Figure out font width from file and alloc just enough. */
   while(
      MERROR_WAIT == (retval = retrofont_load_glyph(
         &font_file, font, &glyph_idx, line, &line_bytes ))
   ) {
      /* Can't determine line width from invalid line! */
   }

   glyph_w_bytes = (maug_strlen( line_bytes ) / glyph_h) >> 1; /* 2hex p byte */
   debug_printf( RETROFONT_TRACE_LVL, "glyph: %s, glyph_w_bytes: %u",
      line_bytes, glyph_w_bytes );
   glyph_w = glyph_w_bytes * 8;

#if 0 < RETROFONT_TRACE_LVL
   debug_printf( RETROFONT_TRACE_LVL, "glyph_w: %u, glyph_sz: %u",
      glyph_w, glyph_h * glyph_w_bytes );
#endif

   /* Alloc enough for each glyph, plus the size of the font header. */
   *p_font_h = maug_malloc( 1,
      sizeof( struct RETROFONT ) +
      (glyph_h * glyph_w_bytes * (1 + glyphs_count)) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_font_h );

#if 0 < RETROFONT_TRACE_LVL
   debug_printf( RETROFONT_TRACE_LVL, "allocated font %s: " SIZE_T_FMT " bytes",
      font_name, (glyph_h * glyph_w_bytes * (1 + glyphs_count)) + 
         sizeof( struct RETROFONT ) );
#endif

   maug_mlock( *p_font_h, font );
   maug_cleanup_if_null_alloc( struct RETROFONT*, font );

   /* Set initial font parameters. */
   font->sz = sizeof( struct RETROFONT );
   font->first_glyph = first_glyph;
   font->glyph_w = glyph_w;
   font->glyph_h = glyph_h;
   font->glyph_sz = glyph_h * glyph_w_bytes;

   while( font_file.has_bytes( &font_file ) ) {
      retval = retrofont_load_glyph(
         &font_file, font, &glyph_idx, line, &line_bytes );
      if(
         /* Inactionable line detected. */
         MERROR_WAIT == retval ||
         /* Skip glyph out of range. */
         glyph_idx < first_glyph || glyph_idx > first_glyph + glyphs_count
      ) {
         retval = MERROR_OK;
         continue;
      }

      /* Find where to put the decoded glyph. */
      p_glyph = retrofont_glyph_at( font, glyph_idx );

      for( i = 0 ; font->glyph_h > i ; i++ ) {
         switch( font->glyph_w ) {
         case 8:
            p_glyph[i] = 0;
            p_glyph[i] |= maug_hctoi( line_bytes[i << 1] ) << 4;
            p_glyph[i] |= maug_hctoi( line_bytes[(i << 1) + 1] );
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

#if 0 < RETROFONT_TRACE_LVL
      /* Test dump to verify glyph integrity. */
      if( glyph_idx == '0' ) {
         retrofont_dump_glyph( p_glyph, glyph_w, glyph_h );
      }
#endif

      font->glyphs_count++;

      debug_printf( RETROFONT_TRACE_LVL,
         "parsed %u glyphs...", font->glyphs_count );

      /* If we're not tracing, try to omit printfs from the inner loop! */
#if 0 < RETROFONT_TRACE_LVL
      debug_printf( RETROFONT_TRACE_LVL,
         SIZE_T_FMT " %s (" SIZE_T_FMT " hbytes)",
         glyph_idx - first_glyph, line_bytes, maug_strlen( line_bytes ) );
#endif
   }

cleanup:

   if( NULL != font ) {
      maug_munlock( *p_font_h, font );
   }

   mfile_close( &font_file );

   return retval;
}

/* === */

static void retrofont_blit_glyph(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   char c, struct RETROFONT* font, size_t x, size_t y, uint8_t flags
) {
   uint8_t* glyph = retrofont_glyph_at( font, c );
   int16_t x_iter, y_iter, y_start, y_end, x_end, x_pen_down = -1;
   uint8_t prev_px_was_clear = 0;

   debug_printf( RETROFONT_TRACE_LVL, "blit glyph: %c", c );

   y_start = RETROFONT_FLAG_OUTLINE == (RETROFONT_FLAG_OUTLINE & flags) ?
      -1 : 0;
   y_end = RETROFONT_FLAG_OUTLINE == (RETROFONT_FLAG_OUTLINE & flags) ?
      font->glyph_h + 1 : font->glyph_h;
   x_end = RETROFONT_FLAG_OUTLINE == (RETROFONT_FLAG_OUTLINE & flags) ?
      font->glyph_w + 1 : font->glyph_w;

   #define _retrofont_px_is_clear( x_iter, y_iter ) \
      (0 > (x_iter) || font->glyph_w <= (x_iter) || \
      0 > (y_iter) || font->glyph_h <= (y_iter) || \
         (1 << (font->glyph_w - (x_iter)) != \
            (glyph[(y_iter)] & (1 << (font->glyph_w - (x_iter))))))

   for( y_iter = y_start ; y_end > y_iter ; y_iter++ ) {
      /* Reset drawing state for new "scanline." */
      prev_px_was_clear = 1;
      x_pen_down = -1;

      /* Note the >= here... slightly "overscan" on X so we can close any
       * end-terminating horizontal lines.
       */
      for( x_iter = 0 ; x_end >= x_iter ; x_iter++ ) {
         if( _retrofont_px_is_clear( x_iter, y_iter ) ) {

            if( 0 <= x_pen_down ) {
               /* Draw this "scanline" of the glyph in one go. */
               retroflat_2d_line( 
                  target, color,
                  x + x_pen_down, y + y_iter, x + x_iter, y + y_iter, 0 );
               x_pen_down = -1;
            }

            if(
               RETROFONT_FLAG_OUTLINE == (RETROFONT_FLAG_OUTLINE & flags) &&
               (!prev_px_was_clear ||
               !_retrofont_px_is_clear( x_iter + 1, y_iter ) ||
               !_retrofont_px_is_clear( x_iter, y_iter + 1 ) ||
               !_retrofont_px_is_clear( x_iter, y_iter - 1 ))
            ) {
               /* Draw outline pixel. */
               retroflat_2d_px( 
                  target, RETROFLAT_COLOR_DARKBLUE,
                  x + x_iter, y + y_iter, 0 );
            }

            /* Save a little time on the next clear pixel check. */
            prev_px_was_clear = 1;

         } else {
            /* This pixel is filled. */

            if( 0 > x_pen_down ) {
               /* Store the X coord to start drawing this "scanline." */
               x_pen_down = x_iter;
            }

            prev_px_was_clear = 0;
         }
      }
   }
}

/* === */

void retrofont_string(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t x, size_t y,
   size_t max_w, size_t max_h, uint8_t flags
) {
   size_t i = 0;
   size_t x_iter = x;
   size_t y_iter = y;
   struct RETROFONT* font = NULL;

   if( (MAUG_MHANDLE)NULL == font_h ) {
      error_printf( "NULL font specified!" );
      goto cleanup;
   }

   if( 0 == str_sz ) {
      str_sz = maug_strlen( str );
   }

   maug_mlock( font_h, font );
   if( NULL == font ) {
      error_printf( "could not lock font!" );
      goto cleanup;
   }

   /* TODO: Stop at max_w/max_h */

   for( i = 0 ; str_sz > i ; i++ ) {
      /* Terminate prematurely at null. */
      if( '\0' == str[i] ) {
         break;
      }

      /* Handle forced newline. */
      if( '\r' == str[i] || '\n' == str[i] ) {
         x_iter = x;
         y_iter += font->glyph_h;
         debug_printf(
            RETROFONT_TRACE_LVL,
            "newline: " SIZE_T_FMT ", " SIZE_T_FMT, x_iter, y_iter );
         continue;
      }

      /* Filter out characters not present in this font. */
      if(
         ' ' != str[i] && (
            str[i] < font->first_glyph ||
            str[i] >= font->first_glyph + font->glyphs_count
         )
      ) {
         error_printf( "invalid character: 0x%02x", str[i] );
         continue;
      }

      /* TODO: More dynamic way to determine space character? */
      if( ' ' != str[i] ) {
         retrofont_blit_glyph(
            target, color, str[i], font, x_iter, y_iter, flags );
      }

      x_iter += font->glyph_w;
      if( 0 < max_w && (x + max_w) <= x_iter + font->glyph_w ) {
         x_iter = x;
         y_iter += font->glyph_h;
      }
   }

cleanup:

   if( NULL != font ) {
      maug_munlock( font_h, font );
   }
}

/* === */

MERROR_RETVAL retrofont_string_sz(
   retroflat_blit_t* target, const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t max_w, size_t max_h,
   size_t* p_out_w, size_t* p_out_h, uint8_t flags
) {
   size_t x_iter = 0;
   size_t i = 0;
   size_t out_h = 0; /* Only used if p_out_h is NULL. */
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFONT* font = NULL;

   if( (MAUG_MHANDLE)NULL == font_h ) {
      error_printf( "NULL font specified!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   if( NULL == p_out_h ) {
      p_out_h = &out_h;
   }

   if( 0 == str_sz ) {
      str_sz = maug_strlen( str );
   }

   maug_mlock( font_h, font );
   maug_cleanup_if_null_alloc( struct RETROFONT*, font );

   for( i = 0 ; str_sz > i ; i++ ) {
      /* Terminate prematurely at null. */
      if( '\0' == str[i] ) {
         break;
      }

      /* Handle forced newline. */
      if( '\r' == str[i] || '\n' == str[i] ) {
         x_iter = 0;
         *p_out_h += font->glyph_h;
         continue;
      }

      x_iter += font->glyph_w;

      if( NULL != p_out_w && *p_out_w <= x_iter ) {
         *p_out_w = x_iter;
      }
      if( 0 < max_w && max_w < x_iter + font->glyph_w ) {
         x_iter = 0;
         *p_out_h += font->glyph_h;
         if( 0 < max_h && *p_out_h + font->glyph_h >= max_h && i < str_sz ) {
            error_printf( "string will not fit!" );

            /* Do not quit; just make a note and keep going. */
            retval = MERROR_GUI;
         }
      }
   }

   /* Add the height of the last line. */
   *p_out_h += font->glyph_h;
   if( NULL != p_out_w ) {
      *p_out_w += 1;
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

