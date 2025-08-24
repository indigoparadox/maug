
#if !defined( MAUG_API_FON_H_DEFS )
#define MAUG_API_FON_H_DEFS

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
      maug_mzero( glyph_bin, 65 );
      
      for( x = 0 ; w > x ; x++ ) {
         glyph_bin[x] = 1 << (w - x) == (glyph[y] & (1 << (w - x))) ? 'x' : '.';
      }
      
#if RETROFONT_TRACE_LVL > 0
      debug_printf( RETROFONT_TRACE_LVL, "%s", glyph_bin );
#endif /* RETROFONT_TRACE_LVL */
   }
}

/* === */

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

#if RETROFONT_TRACE_LVL > 0
   debug_printf( RETROFONT_TRACE_LVL, "font file opened, reading..." );
#endif /* RETROFONT_TRACE_LVL */

   /* Figure out font width from file and alloc just enough. */
   while(
      MERROR_WAIT == (retval = retrofont_read_line(
         &font_file, line, &line_bytes )) || MERROR_PARSE == retval
   ) {
      /* Can't determine line width from invalid line! */
   }

   /* Figure out the index of this glyph. */
   glyph_idx = maug_atou32( line, 0, 16 );
#if RETROFONT_TRACE_LVL > 0
   debug_printf( RETROFONT_TRACE_LVL, "found glyph: " SIZE_T_FMT, glyph_idx );
#endif /* RETROFONT_TRACE_LVL */

   glyph_w_bytes = (maug_strlen( line_bytes ) / glyph_h) >> 1; /* 2hex p byte */
#if RETROFONT_TRACE_LVL > 0
   debug_printf( RETROFONT_TRACE_LVL, "glyph: %s, glyph_w_bytes: %u",
      line_bytes, glyph_w_bytes );
#endif /* RETROFONT_TRACE_LVL */
   glyph_w = glyph_w_bytes * 8;

#if RETROFONT_TRACE_LVL > 0
   debug_printf( RETROFONT_TRACE_LVL, "glyph_w: %u, glyph_sz: %u",
      glyph_w, glyph_h * glyph_w_bytes );
#endif /* RETROFONT_TRACE_LVL */

   /* Alloc enough for each glyph, plus the size of the font header. */
   *p_font_h = maug_malloc( 1,
      sizeof( struct RETROFONT ) +
      (glyph_h * glyph_w_bytes * (1 + glyphs_count)) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_font_h );

#if RETROFONT_TRACE_LVL > 0
   debug_printf( RETROFONT_TRACE_LVL, "allocated font %s: " SIZE_T_FMT " bytes",
      font_name, (glyph_h * glyph_w_bytes * (1 + glyphs_count)) + 
         sizeof( struct RETROFONT ) );
#endif /* RETROFONT_TRACE_LVL */

   maug_mlock( *p_font_h, font );
   maug_cleanup_if_null_alloc( struct RETROFONT*, font );

   /* Set initial font parameters. */
   font->sz = sizeof( struct RETROFONT );
   font->first_glyph = first_glyph;
   font->glyph_w = glyph_w;
   font->glyph_h = glyph_h;
   font->glyph_sz = glyph_h * glyph_w_bytes;

   while( font_file.has_bytes( &font_file ) ) {
      retval = retrofont_read_line( &font_file, line, &line_bytes );
      if( MERROR_WAIT != retval && MERROR_PARSE != retval ) {
         /* Figure out the index of this glyph. */
         glyph_idx = maug_atou32( line, 0, 16 );
#if RETROFONT_TRACE_LVL > 0
         debug_printf( RETROFONT_TRACE_LVL,
            "found glyph: " SIZE_T_FMT, glyph_idx );
#endif /* RETROFONT_TRACE_LVL */
         if(
            glyph_idx < first_glyph || glyph_idx > first_glyph + glyphs_count
         ) {
            /* Skip glyph out of range. */
            retval = MERROR_OK;
            continue;
         }
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

#if RETROFONT_TRACE_LVL > 0
      /* Test dump to verify glyph integrity. */
      if( glyph_idx == '0' ) {
         retrofont_dump_glyph( p_glyph, glyph_w, glyph_h );
      }
#endif /* RETROFONT_TRACE_LVL */

      font->glyphs_count++;

#if RETROFONT_TRACE_LVL > 0
      debug_printf( RETROFONT_TRACE_LVL,
         "parsed %u glyphs...", font->glyphs_count );
#endif /* RETROFONT_TRACE_LVL */

      /* If we're not tracing, try to omit printfs from the inner loop! */
#if RETROFONT_TRACE_LVL > 0
      debug_printf( RETROFONT_TRACE_LVL,
         SIZE_T_FMT " %s (" SIZE_T_FMT " hbytes)",
         glyph_idx - first_glyph, line_bytes, maug_strlen( line_bytes ) );
#endif /* RETROFONT_TRACE_LVL */
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
 
#if RETROFONT_TRACE_LVL > 0
   debug_printf( RETROFONT_TRACE_LVL, "blit glyph: %c", c );
#endif /* RETROFONT_TRACE_LVL */

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

void retrofont_string_indent(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t x, size_t y,
   size_t max_w, size_t max_h, size_t x_iter, uint8_t flags
) {
   size_t i = 0;
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

   /* Add indent to starting X the first time. Subsequent new lines will just
    * reset to starting X.
    */
   x_iter += x;

   for( i = 0 ; str_sz > i ; i++ ) {
      /* Terminate prematurely at null. */
      if( '\0' == str[i] ) {
         break;
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
      if(
         '\r' == str[i] || '\n' == str[i] ||
         (0 < max_w && (x + max_w) <= x_iter + font->glyph_w)
      ) {
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
   MAUG_MHANDLE font_h, retroflat_pxxy_t max_w, retroflat_pxxy_t max_h,
   retroflat_pxxy_t* p_out_w, retroflat_pxxy_t* p_out_h, uint8_t flags
) {
   size_t x_iter = 0;
   size_t i = 0;
   retroflat_pxxy_t out_h; /* Only used if p_out_h is NULL. */
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

   /* Reset output vars to zero. */
   *p_out_h = 0;
   if( NULL == p_out_w ) {
      *p_out_w = 0;
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

      x_iter += font->glyph_w;

      if( NULL != p_out_w && *p_out_w <= x_iter ) {
         *p_out_w = x_iter;
      }
      if(
         '\r' == str[i] || '\n' == str[i] ||
         (0 < max_w && max_w < x_iter + font->glyph_w)
      ) {
         /* There is a max width, but we're still inside of it. */
         *p_out_h += font->glyph_h;
         /* Every new line is at least one char wide. */
         x_iter = font->glyph_w;
         if( 0 < max_h && *p_out_h + font->glyph_h >= max_h && i < str_sz ) {
            /* However, we are not inside the max height! */
#if RETROFONT_TRACE_LVL > 0
            error_printf( "string will not fit!" );
#endif /* RETROFONT_TRACE_LVL */

            /* Do not quit; just make a note and keep going. */
            retval = MERROR_GUI;
         }

         if(
            RETROFONT_FLAG_SZ_MIN == (RETROFONT_FLAG_SZ_MIN & flags) &&
            NULL != p_out_w
         ) {
            /* We're interested in the shortest line, so reset output width. */
            *p_out_w = 0;
         }
      }
   }

   /* Add the height of the last line (unless we're sz_min for cursor). */
   if( RETROFONT_FLAG_SZ_MIN != (RETROFONT_FLAG_SZ_MIN & flags) ) {
      *p_out_h += font->glyph_h;
   }
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

