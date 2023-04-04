
#ifndef RETROSFT_H
#define RETROSFT_H

MERROR_RETVAL retrosoft_load_glyph(
   RETROFLAT_COLOR color,
   size_t set_idx, size_t glyph_idx, struct RETROFLAT_BITMAP* bmp );

MERROR_RETVAL retrosoft_init();

void retrosoft_shutdown();

void retrosoft_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, uint8_t flags );

void retrosoft_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int x, int y, int w, int h, uint8_t flags );

void retrosoft_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags );

void retrosoft_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags );

void retrosoft_string_sz(
   struct RETROFLAT_BITMAP* target, const char* str, size_t str_sz,
   const char* font_str, size_t* w_out, size_t* h_out, uint8_t flags );

void retrosoft_string(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz, const char* font_str, int x_orig, int y_orig,
   uint8_t flags );

#ifdef RETROSFT_C

#  include "mfont8x8.h"

/* TODO: Create another depth for each color. */
static struct RETROFLAT_BITMAP
gc_font_bmps[RETROFLAT_COLORS_SZ][RETROSOFT_SETS_COUNT][RETROSOFT_GLYPHS_COUNT];

/* === */

MERROR_RETVAL retrosoft_load_glyph(
   RETROFLAT_COLOR color,
   size_t set_idx, size_t glyph_idx, struct RETROFLAT_BITMAP* bmp
) {
   MERROR_RETVAL retval = MERROR_OK;
   int x = 0,
      y = 0;
   const char* glyph_dots = gc_font8x8[set_idx][glyph_idx];
   
   /* Create a transparent bitmap to draw on. */
   retval = retroflat_create_bitmap(
      RETROSOFT_GLYPH_W_SZ, RETROSOFT_GLYPH_H_SZ, bmp );
   maug_cleanup_if_not_ok();

   retroflat_draw_lock( bmp );

   /* Draw the glyph onto the bitmap. */
   for( y = 0 ; RETROSOFT_GLYPH_H_SZ > y ; y++ ) {
      for( x = 0 ; RETROSOFT_GLYPH_W_SZ > x ; x++ ) {
         if( 1 == ((glyph_dots[y] >> x) & 0x01) ) {
            retroflat_px( bmp, color, x, y, 0 );
         }
      }
   }

   retroflat_draw_release( bmp );

   /*
   retroflat_rect(
      bmp, RETROFLAT_COLOR_RED, 0, 0, 
      RETROSOFT_GLYPH_W_SZ, RETROSOFT_GLYPH_H_SZ, RETROFLAT_FLAGS_FILL );
   */

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrosoft_init() {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0,
      j = 0;
   RETROFLAT_COLOR h = RETROFLAT_COLOR_WHITE;

#ifdef RETROSOFT_PRELOAD_COLORS
   for( h = 0 ; RETROFLAT_COLORS_SZ > h ; h++ ) {
      debug_printf( 1, "loading glyphs in %s...", gc_retroflat_color_names[h] );
#endif /* RETROSOFT_PRELOAD_COLORS */
   for( i = 0 ; RETROSOFT_SETS_COUNT > i ; i++ ) {
         for( j = 0 ; RETROSOFT_GLYPHS_COUNT > j ; j++ ) {
            debug_printf( 1, "loading glyph " SIZE_T_FMT "...", j );
            retval = retrosoft_load_glyph( h, i, j, &(gc_font_bmps[h][i][j]) );
            maug_cleanup_if_not_ok();
         }
      }
#ifdef RETROSOFT_PRELOAD_COLORS
   }
#endif /* RETROSOFT_PRELOAD_COLORS */

cleanup:

   /* TODO: Unload loaded bitmaps if retval not OK. */

   return retval;
}

/* === */

void retrosoft_shutdown() {
   size_t i = 0,
      j = 0;
   RETROFLAT_COLOR h = RETROFLAT_COLOR_WHITE;

#ifdef RETROSOFT_PRELOAD_COLORS
   for( h = 0 ; RETROFLAT_COLORS_SZ > h ; h++ ) {
#endif /* RETROSOFT_PRELOAD_COLORS */
      for( i = 0 ; RETROSOFT_SETS_COUNT > i ; i++ ) {
         for( j = 0 ; RETROSOFT_GLYPHS_COUNT > j ; j++ ) {
            retroflat_destroy_bitmap( &(gc_font_bmps[h][i][j]) );
         }
      }
#ifdef RETROSOFT_PRELOAD_COLORS
   }
#endif /* RETROSOFT_PRELOAD_COLORS */
}

/* === */

void retrosoft_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, uint8_t flags
) {
   #define RETROFLAT_LINE_X 0
   #define RETROFLAT_LINE_Y 1

   uint8_t for_axis = 0,
      off_axis = 0;
   int16_t dist[2],
      start[2],
      end[2],
      iter[2],
      inc = 1,
      delta = 0;
   MERROR_RETVAL retval = MERROR_OK;
   int locked_target_internal = 0;

   retroflat_internal_autolock_bitmap( target, locked_target_internal );

   /* TODO: Handle thickness. */

   /* Figure out strategy based on line slope. */
   if( abs( y2 - y1 ) < abs( x2 - x1 ) ) {
      if( x1 > x2 ) {
         start[RETROFLAT_LINE_X] = x2;
         start[RETROFLAT_LINE_Y] = y2;
         end[RETROFLAT_LINE_X] = x1;
         end[RETROFLAT_LINE_Y] = y1;
         for_axis = RETROFLAT_LINE_X;
      } else {
         start[RETROFLAT_LINE_X] = x1;
         start[RETROFLAT_LINE_Y] = y1;
         end[RETROFLAT_LINE_X] = x2;
         end[RETROFLAT_LINE_Y] = y2;
         for_axis = RETROFLAT_LINE_X;
      }
   } else {
      if( y2 < y1 ) {
         start[RETROFLAT_LINE_X] = x2;
         start[RETROFLAT_LINE_Y] = y2;
         end[RETROFLAT_LINE_X] = x1;
         end[RETROFLAT_LINE_Y] = y1;
         for_axis = RETROFLAT_LINE_Y;
      } else {
         start[RETROFLAT_LINE_X] = x1;
         start[RETROFLAT_LINE_Y] = y1;
         end[RETROFLAT_LINE_X] = x2;
         end[RETROFLAT_LINE_Y] = y2;
         for_axis = RETROFLAT_LINE_Y;
      }
   }

   /* C89 requires const initializers, so do math down here. */
   off_axis = 1 - for_axis;
   iter[RETROFLAT_LINE_X] = start[RETROFLAT_LINE_X];
   iter[RETROFLAT_LINE_Y] = start[RETROFLAT_LINE_Y];
   dist[RETROFLAT_LINE_X] = end[RETROFLAT_LINE_X] - start[RETROFLAT_LINE_X];
   dist[RETROFLAT_LINE_Y] = end[RETROFLAT_LINE_Y] - start[RETROFLAT_LINE_Y];

   /* Adjust delta/slope for off-axis. */
   delta = (2 * dist[off_axis]) - dist[for_axis];
   if( 0 > dist[off_axis] ) {
      inc = -1;
      dist[off_axis] *= -1;
   }

   for(
      iter[for_axis] = start[for_axis] ;
      end[for_axis] > iter[for_axis] ;
      iter[for_axis]++
   ) {
      retroflat_px(
         target, color, iter[RETROFLAT_LINE_X], iter[RETROFLAT_LINE_Y], 0 );

      /* Increment off-axis based on for-axis. */
      if( 0 < delta ) {
         iter[off_axis] += inc;
         delta += (2 * (dist[off_axis] - dist[for_axis]));
      } else {
         delta += (2 * dist[off_axis]);
      }
   }

cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }  
}

/* === */

void retrosoft_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int x, int y, int w, int h, uint8_t flags
) {
   size_t x_iter = 0,
      y_iter = 0;

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {

      for( y_iter = y ; y_iter < y + h ; y_iter++ ) {
         for( x_iter = x ; x_iter < x + w ; x_iter++ ) {
            /* TODO: Optimize filling 4-byte sequences! */
            retroflat_px( target, color_idx, x_iter, y_iter, 0 );
         }
      }

   } else {

      retrosoft_line( target, color_idx, x, y, x + w, y, 0 );

      retrosoft_line( target, color_idx, x + w, y, x + w, y + h, 0 );

      retrosoft_line( target, color_idx, x + w, y + h, x, y + h, 0 );

      retrosoft_line( target, color_idx, x, y + h, x, y, 0 );

   }
}

/* === */

void retrosoft_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags
) {
   int32_t i = 0,
      i_prev = 0;
   MERROR_RETVAL retval = MERROR_OK;
   int16_t locked_target_internal = 0,
      px_x1 = 0,
      px_y1 = 0,
      px_x2 = 0,
      px_y2 = 0;

   /* TODO: Switch to Bresenham algorithm. */

   /* TODO: Filled ellipse. */

   retroflat_internal_autolock_bitmap( target, locked_target_internal );

   /* For the soft_lut, input numbers are * 1000... so 0.1 becomes 100. */
   for( i = 100 ; 2 * RETROFP_PI + 100 > i ; i += 100 ) {
      i_prev = i - 100;

      px_x1 = x + (w / 2) + retrofp_cos( i_prev, w / 2 );
      px_y1 = y + (h / 2) + retrofp_sin( i_prev, h / 2 );
      px_x2 = x + (w / 2) + retrofp_cos( i, w / 2 );
      px_y2 = y + (h / 2) + retrofp_sin( i, h / 2 );

      assert( 0 <= px_x1 );
      assert( 0 <= px_y1 );

      retroflat_line( target, color, px_x1, px_y1, px_x2, px_y2, 0 );  
   }

cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }  
}

/* === */

void retrosoft_string_sz(
   struct RETROFLAT_BITMAP* target, const char* str, size_t str_sz,
   const char* font_str, size_t* w_out, size_t* h_out, uint8_t flags
) {
   /* TODO: Put a little more effort into sizing. */
   if( 0 == str_sz ) {
      str_sz = strlen( str );
   }

   *w_out = RETROSOFT_GLYPH_W_SZ * str_sz;
   *h_out = RETROSOFT_GLYPH_H_SZ;
}

/* === */

void retrosoft_string(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   const char* str, size_t str_sz, const char* font_str, int x_orig, int y_orig,
   uint8_t flags
) {
   size_t i = 0,
      glyph_idx = 0;
   int x = x_orig;

   for( i = 0 ; str_sz > i ; i++ ) {
      /* Terminate prematurely at null. */
      if( '\0' == str[i] ) {
         break;
      }

      /* Fonts start at character after space. */
      glyph_idx = str[i] - ' ';

#if defined( RETROFLAT_API_SDL2 ) && !defined( RETROSOFT_PRELOAD_COLORS )
      /* If we're not caching the colors, use SDL2 features if available to
       * tint the glyph before blitting it. */
      SDL_SetSurfaceColorMod(
         gc_font_bmps[RETROFLAT_COLOR_WHITE][0][glyph_idx].surface,
         g_retroflat_state->palette[color].r,
         g_retroflat_state->palette[color].g,
         g_retroflat_state->palette[color].b );
      SDL_DestroyTexture(
         gc_font_bmps[RETROFLAT_COLOR_WHITE][0][glyph_idx].texture );
      gc_font_bmps[RETROFLAT_COLOR_WHITE][0][glyph_idx].texture =
         SDL_CreateTextureFromSurface(
            g_retroflat_state->buffer.renderer,
            gc_font_bmps[RETROFLAT_COLOR_WHITE][0][glyph_idx].surface );
#endif /* RETROFLAT_API_SDL2 && !RETROSOFT_PRELOAD_COLORS */

      retroflat_blit_bitmap(
         target, &(gc_font_bmps[
#ifdef RETROSOFT_PRELOAD_COLORS
         color
#else
         RETROFLAT_COLOR_WHITE
#endif /* !RETROSOFT_PRELOAD_COLORS */
         ][0][glyph_idx]), 0, 0, x, y_orig,
         RETROSOFT_GLYPH_W_SZ, RETROSOFT_GLYPH_H_SZ );

      x += 8;
   }
}

#else

#endif /* RETROSFT_C */

#endif /* !RETROSFT_H */

