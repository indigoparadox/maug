
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
   int ttf_retval = 0;

   if( 0 == glyph_h ) {
      glyph_h = retrofont_sz_from_filename( font_name );
   }
   if( 0 == glyph_h ) {
      error_printf( "unable to determine font height!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   if(
      RETROFLAT_FLAGS_FONT_INIT !=
      (RETROFLAT_FLAGS_FONT_INIT & g_retroflat_state->retroflat_flags)
   ) {
      debug_printf( 2, "initializing SDL_TTF..." );
      ttf_retval = TTF_Init();
      if( 0 > ttf_retval ) {
         error_printf( "error initializing TTF subsystem!" );
         retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
            "Error", "Error initializing TTF subsystem!" );
         retval = MERROR_GUI;
         goto cleanup;
      }
   }

#  pragma message( "warning: retrofont_load not implemented" )

cleanup:

   return retval;
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

/* === */

void retrofont_free( MAUG_MHANDLE* p_font_h ) {
#  pragma message( "warning: retroflat_free not implemented" )
}

#endif /* !RETROFNT_C */

#endif /* !MAUG_API_FON_H_DEFS */


