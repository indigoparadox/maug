
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

#include <mrapifon.h>

/* \} */ /* retrofnt */

#endif /* !RETROFNT_H */

