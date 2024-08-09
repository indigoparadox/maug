
#ifndef RETROXPM_H
#define RETROXPM_H

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

static MERROR_RETVAL retroflat_xpm_header(
   const char* line, size_t line_sz, int* w, int* h, int* colors, int* bypp
) {
   size_t i = 0;
   int16_t i_cur = 0;
   const char* num_start = line;
   uint8_t num_cur = 0;
   MERROR_RETVAL retval = MERROR_FILE;

   /* TODO: Generalize into varags. */

   #define _retroflat_xpm_scan_case( idx, num ) \
      case idx: \
         (num) = maug_atou32( num_start, i_cur, 10 ); \
         /* debug_printf( 1, "%u: \"%s\" became: %d", idx, num_start, num ); */ \
         if( ' ' == line[i] ) { \
            num_start = &(line[i + 1]); \
         } else { \
            num_start = &(line[i]); \
         } \
         i_cur = 0; \
         num_cur++; \
         continue;

   for( i = 0 ; line_sz > i ; i++ ) {
      i_cur++;
      if( i == line_sz - 1 && 3 == num_cur ) {
         retval = MERROR_OK;
      }
      if( ' ' == line[i] || line_sz - 1 == i ) {
         switch( num_cur ) {
         _retroflat_xpm_scan_case( 0, *w )
         _retroflat_xpm_scan_case( 1, *h )
         _retroflat_xpm_scan_case( 2, *colors )
         _retroflat_xpm_scan_case( 3, *bypp )
         }
      } 
   }

cleanup:

   return retval;
}

MERROR_RETVAL retroflat_load_xpm(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   RETROFLAT_COLOR color;
   int xpm_idx = 0,
      x = 0,
      y = 0,
      bmp_w = 0,
      bmp_h = 0,
      bmp_colors,
      bmp_bypp;

   /* Hunt for the requested XPM in the compiled directory. */
   while( '\0' != gc_xpm_filenames[xpm_idx][0] ) {
      if( 0 == strcmp( filename, gc_xpm_filenames[xpm_idx] ) ) {
         goto xpm_found;
      }
      xpm_idx++;
   }

   retval = RETROFLAT_ERROR_BITMAP;
   goto cleanup;

xpm_found:

   debug_printf( 2, "found xpm: %s", gc_xpm_filenames[xpm_idx] );

   /* Load XPM and draw it to a new bitmap. */

   /* sscanf( gc_xpm_data[xpm_idx][0], "%d %d %d %d",
      &bmp_w, &bmp_h, &bmp_colors, &bmp_bypp ); */
   retval = retroflat_xpm_header(
      gc_xpm_data[xpm_idx][0], maug_strlen( gc_xpm_data[xpm_idx][0] ),
      &bmp_w, &bmp_h, &bmp_colors, &bmp_bypp );
   maug_cleanup_if_not_ok();

   debug_printf( 1, "bitmap has: %d colors, %d bypp", bmp_colors, bmp_bypp );

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

