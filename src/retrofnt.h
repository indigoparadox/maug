
#ifndef RETROFNT_H
#define RETROFNT_H

/**
 * \addtogroup retrofont RetroFont API
 * \{
 * \file retrofnt.h
 */

#define RETROFONT_PRESENT 1

#ifndef RETROFONT_LINE_SZ
#  define RETROFONT_LINE_SZ 80
#endif /* !RETROFONT_LINE_SZ */

#ifndef RETROFONT_TRACE_LVL
#  define RETROFONT_TRACE_LVL 0
#endif /* !RETROFONT_TRACE_LVL */

struct RETROFONT;

MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count );

void retrofont_string(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t x, size_t y,
   size_t max_w, size_t max_h, uint8_t flags );

MERROR_RETVAL retrofont_string_sz(
   retroflat_blit_t* target, const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t max_w, size_t max_h,
   size_t* p_out_w, size_t* p_out_h, uint8_t flags );

#ifdef RETROFNT_C

/* Provide this utility to all font API internal mechanisms. */

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

