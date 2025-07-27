
#ifndef RETROXPM_H
#define RETROXPM_H

#ifdef RETROFLAT_3D
#error "XPM loader not supported with 3D textures!"
#endif /* RETROFLAT_3D */

#ifndef RETROXPM_TRACE_LVL
#  define RETROXPM_TRACE_LVL 0
#endif /* !RETROXPM_TRACE_LVL */

/**
 * \brief Load a compiled-in XPM image into an API-specific bitmap context.
 * \warn The XPM must have been generated from a bitmap using the rather
 *       restrictive \ref maug_retroflt_bitmap rules.
 */
MERROR_RETVAL retroflat_load_xpm(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags );

#ifdef RETROFLAT_XPM
#  ifdef RETROFLT_C
#     define XPMASSET_C
#  endif /* RETROFLT_C */
#include "xpmasset.h"
#endif /* RETROFLAT_XPM */

#ifdef RETROFLT_C

extern MAUG_CONST char* SEG_MCONST gc_xpm_filenames[];
extern MAUG_CONST char** SEG_MCONST gc_xpm_data[];

MERROR_RETVAL retroflat_load_xpm(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   RETROFLAT_COLOR color;
   int16_t xpm_idx = 0,
      x = 0,
      y = 0,
      bmp_w = 0,
      bmp_h = 0,
      bmp_colors,
      bmp_bypp;
   retroflat_asset_path filename_path;

   retval = retroflat_build_filename_path(
      filename, filename_path, MAUG_PATH_MAX + 1, flags );
   maug_cleanup_if_not_ok();

   debug_printf( RETROXPM_TRACE_LVL,
      "searching for xpm: %s (%s)", filename, filename_path );

   /* Hunt for the requested XPM in the compiled directory. */
   while( '\0' != gc_xpm_filenames[xpm_idx][0] ) {
      if( 0 == strcmp( filename_path, gc_xpm_filenames[xpm_idx] ) ) {
         goto xpm_found;
      }
      xpm_idx++;
   }

   error_printf( "xpm \"%s\" not found!", filename_path );
   retval = RETROFLAT_ERROR_BITMAP;
   goto cleanup;

xpm_found:

   debug_printf( RETROXPM_TRACE_LVL,
      "found xpm: %s", gc_xpm_filenames[xpm_idx] );

   /* Load XPM and draw it to a new bitmap. */

   retval = maug_tok_int( 0, gc_xpm_data[xpm_idx][0], 0, &bmp_w, "\n " );
   maug_cleanup_if_not_ok();
   retval = maug_tok_int( 1, gc_xpm_data[xpm_idx][0], 0, &bmp_h, "\n " );
   maug_cleanup_if_not_ok();
   retval = maug_tok_int( 2, gc_xpm_data[xpm_idx][0], 0, &bmp_colors, "\n " );
   maug_cleanup_if_not_ok();
   retval = maug_tok_int( 3, gc_xpm_data[xpm_idx][0], 0, &bmp_bypp, "\n " );
   maug_cleanup_if_not_ok();

   debug_printf( RETROXPM_TRACE_LVL,
      "bitmap has: %d colors, %d bypp", bmp_colors, bmp_bypp );

   assert( 16 == bmp_colors );
   assert( 1 == bmp_bypp );

   retval = retroflat_create_bitmap( bmp_w, bmp_h, bmp_out, flags );
   if( MERROR_OK != retval ) {
      goto cleanup;
   }

   debug_printf( 1, "created empty canvas: %dx%d", bmp_w, bmp_h );

   /* Draw XPM pixels to canvas. */

   /* Normally draw lock is called from the main loop, but we're making an
    * off-screen bitmap, here!
    */
   retroflat_draw_lock( bmp_out );
   retroflat_px_lock( bmp_out );

   for( y = 0 ; bmp_h > y ; y++ ) {
      for( x = 0 ; bmp_w > x ; x++ ) {
         switch( gc_xpm_data[xpm_idx][17 + y][x] ) {
         case ' ':
            /* Transparent. */
            /* TODO: Global transparency palette ifdef? */
            color = RETROFLAT_COLOR_BLACK;
            continue;
         case '.':
            color = RETROFLAT_COLOR_DARKBLUE;
            break;
         case 'X':
            color = RETROFLAT_COLOR_DARKGREEN;
            break;
         case 'o':
            color = RETROFLAT_COLOR_TEAL;
            break;
         case 'O':
            color = RETROFLAT_COLOR_DARKRED;
            break;
         case '+':
            color = RETROFLAT_COLOR_VIOLET;
            break;
         case '@':
            color = RETROFLAT_COLOR_BROWN;
            break;
         case '#':
            color = RETROFLAT_COLOR_GRAY;
            break;
         case '$':
            color = RETROFLAT_COLOR_DARKGRAY;
            break;
         case '%':
            color = RETROFLAT_COLOR_BLUE;
            break;
         case '&':
            color = RETROFLAT_COLOR_GREEN;
            break;
         case '*':
            color = RETROFLAT_COLOR_CYAN;
            break;
         case '=':
            color = RETROFLAT_COLOR_RED;
            break;
         case '-':
            color = RETROFLAT_COLOR_MAGENTA;
            break;
         case ';':
            color = RETROFLAT_COLOR_YELLOW;
            break;
         case ':':
            color = RETROFLAT_COLOR_WHITE;
            break;
         }

         retroflat_px( bmp_out, color, x, y, 0 );
      }
   }

   retroflat_px_release( bmp_out );
   retroflat_draw_release( bmp_out );

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      retval = retroflat_bitmap_win_transparency( bmp_out,
         bmp_out->bmi.header.biWidth, bmp_out->bmi.header.biHeight );
   }
#  elif defined( RETROFLAT_API_PC_BIOS )
   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      retroflat_bitmap_dos_transparency( bmp_out );
   }
#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   /* SDL_SetColorKey is called in retroflat_create_bitmap(). */
#  else
#     pragma message( "warning: xpm transparency not implemented" )
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

cleanup:
   return retval;
}

#endif /* RETROFLT_C */

#endif /* !RETROXPM_H */

