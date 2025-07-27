
#ifndef RETROFNT_H
#define RETROFNT_H

/**
 * \addtogroup retrofont RetroFont API
 * \brief Tools for drawing strings using loadable fonts.
 *
 * This API contains platform-specific parts present in MAUG_ROOT/api/font.
 *
 * \{
 * \file retrofnt.h
 * \file Platform-agnostic portions of the RetroFont API.
 */

/*! \brief Tell other modules that retrofont is loaded. */
#define RETROFONT_PRESENT 1

#ifndef RETROFONT_LINE_SZ
#  define RETROFONT_LINE_SZ 80
#endif /* !RETROFONT_LINE_SZ */

#ifndef RETROFONT_TRACE_LVL
#  define RETROFONT_TRACE_LVL 0
#endif /* !RETROFONT_TRACE_LVL */

/**
 * \brief Opaque data structure defined by each platform/API.
 */
struct RETROFONT;

/**
 * \brief Load a font for drawing.
 * \param font_name Name of the font file to load.
 * \param p_font_h Handle into which to load the font data.
 * \param glyph_h Height of the font being loaded.
 * \param first_glyph ASCII index of the first glyph to load from the font.
 * \param glyphs_count Number of glyphs to load from the font.
 */
MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count );

/**
 * \brief Draw a string with the given font.
 * \param target Bitmap, screen, or texture on which to draw string.
 * \param color Color in which to draw string.
 * \param str String to draw on-screen.
 * \param str_sz Size of the string to draw in characters.
 * \param max_w Width in pixels after which string should be wrapped.
 * \param max_h Height in pixels after which string should be truncated.
 */
void retrofont_string(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t x, size_t y,
   size_t max_w, size_t max_h, uint8_t flags );

MERROR_RETVAL retrofont_string_sz(
   retroflat_blit_t* target, const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t max_w, size_t max_h,
   size_t* p_out_w, size_t* p_out_h, uint8_t flags );

void retrofont_free( MAUG_MHANDLE* p_font_h );

/**
 * \brief Callback for platform-specific font substitute loader to attempt to
 *        use font substitute.
 */
typedef MERROR_RETVAL (*retrofont_try_platform_t)(
   struct RETROFONT* font, const char* sub_name, void* data );

#ifdef RETROFNT_C

/* Provide this utility to all font API internal mechanisms. */

static MERROR_RETVAL retrofont_read_line(
   mfile_t* font_file, char* glyph_idx_str, char** p_glyph_bytes
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t last_char_idx = 0;

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
      last_char_idx = strlen( *p_glyph_bytes ) - 1;
      if(
         '\n' == (*p_glyph_bytes)[last_char_idx] ||
         '\r' == (*p_glyph_bytes)[last_char_idx] ||
         '\t' == (*p_glyph_bytes)[last_char_idx] ||
         ' ' == (*p_glyph_bytes)[last_char_idx]
      ) {
         (*p_glyph_bytes)[last_char_idx] = '\0';
      }
      debug_printf( RETROFONT_TRACE_LVL, "found sub: \"%s\"", *p_glyph_bytes );
      retval = MERROR_PARSE;
      goto cleanup;
   }

cleanup:
   return retval;
}

/* === */

static MERROR_RETVAL retrofont_load_stub(
   const char* font_name, struct RETROFONT* font,
   retrofont_try_platform_t try_platform,
   void* try_platform_data
) {
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t font_file;
   retroflat_asset_path font_stub_name;
   char line[RETROFONT_LINE_SZ];
   char* line_bytes = NULL;

   maug_mzero( font_stub_name, sizeof( retroflat_asset_path ) );
   strncpy( font_stub_name, font_name, sizeof( retroflat_asset_path ) - 1 );
   font_stub_name[strlen( font_stub_name ) - 5] = 'x';
   debug_printf( 1, "font_name: %s", font_stub_name );

   /* TODO: Load font stub and find substitute. */
   maug_mzero( &font_file, sizeof( mfile_t ) );

   retval = mfile_open_read( font_stub_name, &font_file );
   maug_cleanup_if_not_ok();

   /* Figure out font width from file and alloc just enough. */
   do {
      retval = retrofont_read_line( &font_file, line, &line_bytes );
      if( MERROR_WAIT == retval || MERROR_OK == retval ) {
         /* Not a sub line. */
         retval = MERROR_PARSE;
         continue;

      } else if( MERROR_PARSE != retval ) {
         goto cleanup;
      }
      debug_printf( RETROFONT_TRACE_LVL, "attempting substitute: %s",
         line_bytes );
      retval = try_platform( font, line_bytes, try_platform_data );
   } while( MERROR_PARSE == retval );

cleanup:

   mfile_close( &font_file );

   return retval;
}

/* === */

static size_t retrofont_sz_from_filename( const char* font_name ) {
   const char* p_c = NULL;
   size_t glyph_h = 0;
   size_t i = 0;
   char glyph_h_buf[10];

   maug_mzero( glyph_h_buf, 10 );

   assert( NULL != font_name );
   assert( ' ' <= font_name[0] );

   p_c = maug_strrchr( font_name, '.' );
   while( p_c - 1 > font_name ) {
      /* Start at the char before the '.' and work backwords until a '-'. */
      p_c--;
      if( '-' == *p_c || '_' == *p_c ) {
         break;
      }

      /* TODO: Break if not a digit! */

      /* Shift existing numbers up by one. */
      for( i = 9 ; 0 < i ; i-- ) {
         glyph_h_buf[i] = glyph_h_buf[i - 1];
      }

      /* Add the most recent number to the beginning. */
      glyph_h_buf[0] = *p_c;
   }

   glyph_h = atoi( glyph_h_buf );

   debug_printf(
      RETROFONT_TRACE_LVL, "detected glyph height: " SIZE_T_FMT, glyph_h );

   return glyph_h;
}

#endif /* RETROFNT_C */

#include <mrapifon.h>

/* \} */ /* retrofnt */

#endif /* !RETROFNT_H */

