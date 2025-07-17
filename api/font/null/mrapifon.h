
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
};

#if defined( RETROFNT_C )

MERROR_RETVAL retrofont_load(
   const char* font_name, MAUG_MHANDLE* p_font_h,
   uint8_t glyph_h, uint16_t first_glyph, uint16_t glyphs_count
) {
   MERROR_RETVAL retval = MERROR_OK;

#  pragma message( "warning: retrofont_load not implemented" )

   return retval;
}

/* === */

void retrofont_blit_glyph(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   char c, struct RETROFONT* font, size_t x, size_t y, uint8_t flags
) {

#  pragma message( "warning: retrofont_blit_glyph not implemented" )

}

/* === */

void retrofont_string(
   retroflat_blit_t* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t x, size_t y,
   size_t max_w, size_t max_h, uint8_t flags
) {

#  pragma message( "warning: retrofont_string not implemented" )

}

/* === */

MERROR_RETVAL retrofont_string_sz(
   retroflat_blit_t* target, const char* str, size_t str_sz,
   MAUG_MHANDLE font_h, size_t max_w, size_t max_h,
   size_t* p_out_w, size_t* p_out_h, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

#  pragma message( "warning: retroflat_string_sz not implemented" )

   return retval;
}

#endif /* !RETROFNT_C */

#endif /* !MAUG_API_FON_H_DEFS */

