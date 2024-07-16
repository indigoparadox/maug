
#ifndef RETROSFT_H
#define RETROSFT_H

#define RETROFLAT_LINE_X 0
#define RETROFLAT_LINE_Y 1

#ifndef RETROSOFT_TRACE_LVL
#  define RETROSOFT_TRACE_LVL 0
#endif /* RETROSOFT_TRACE_LVL */

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

#  ifndef RETROFLAT_NO_STRING
/* TODO: Create another depth for each color. */
static struct RETROFLAT_BITMAP
gc_font_bmps[RETROFLAT_COLORS_SZ][RETROSOFT_SETS_COUNT][RETROSOFT_GLYPHS_COUNT];
#  endif /* !RETROFLAT_NO_STRING */

/* === */

#  ifndef RETROFLAT_NO_STRING

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
      RETROSOFT_GLYPH_W_SZ, RETROSOFT_GLYPH_H_SZ, bmp, 0 );
   maug_cleanup_if_not_ok();

   /* Normally draw lock is called from the main loop, but we're making an
    * off-screen bitmap, here!
    */
   retroflat_draw_lock( bmp );
   retroflat_px_lock( bmp );

   /* Draw the glyph onto the bitmap. */
   for( y = 0 ; RETROSOFT_GLYPH_H_SZ > y ; y++ ) {
      for( x = 0 ; RETROSOFT_GLYPH_W_SZ > x ; x++ ) {
         if( 1 == ((glyph_dots[y] >> x) & 0x01) ) {
            retroflat_px( bmp, color, x, y, 0 );
         }
      }
   }

   retroflat_px_release( bmp );
   retroflat_draw_release( bmp );

   /*
   retroflat_rect(
      bmp, RETROFLAT_COLOR_RED, 0, 0, 
      RETROSOFT_GLYPH_W_SZ, RETROSOFT_GLYPH_H_SZ, RETROFLAT_FLAGS_FILL );
   */

cleanup:

   return retval;
}

#  endif /* !RETROFLAT_NO_STRING */

/* === */

MERROR_RETVAL retrosoft_init() {
   MERROR_RETVAL retval = MERROR_OK;
#  ifndef RETROFLAT_NO_STRING
   size_t i = 0,
      j = 0;
   RETROFLAT_COLOR h = RETROFLAT_COLOR_WHITE;

#     ifdef RETROSOFT_PRELOAD_COLORS
   for( h = 0 ; RETROFLAT_COLORS_SZ > h ; h++ ) {
      debug_printf( RETROSOFT_TRACE_LVL,
         "loading glyphs in %s...", gc_retroflat_color_names[h] );
#     endif /* RETROSOFT_PRELOAD_COLORS */
   for( i = 0 ; RETROSOFT_SETS_COUNT > i ; i++ ) {
         for( j = 0 ; RETROSOFT_GLYPHS_COUNT > j ; j++ ) {
            debug_printf( RETROSOFT_TRACE_LVL,
               "loading glyph " SIZE_T_FMT "...", j );
            retval = retrosoft_load_glyph( h, i, j, &(gc_font_bmps[h][i][j]) );
            maug_cleanup_if_not_ok();
         }
      }
#     ifdef RETROSOFT_PRELOAD_COLORS
   }
#     endif /* RETROSOFT_PRELOAD_COLORS */

cleanup:

#  endif /* !RETROFLAT_NO_STRING */

   /* TODO: Unload loaded bitmaps if retval not OK. */

   return retval;
}

/* === */

void retrosoft_shutdown() {
#  ifndef RETROFLAT_NO_STRING
   size_t i = 0,
      j = 0;
   RETROFLAT_COLOR h = RETROFLAT_COLOR_WHITE;
#  endif /* !RETROFLAT_NO_STRING */

   debug_printf( RETROSOFT_TRACE_LVL, "retrosoft shutdown called..." );

#  ifndef RETROFLAT_NO_STRING

#ifdef RETROSOFT_PRELOAD_COLORS
   for( h = 0 ; RETROFLAT_COLORS_SZ > h ; h++ ) {
#endif /* RETROSOFT_PRELOAD_COLORS */
      for( i = 0 ; RETROSOFT_SETS_COUNT > i ; i++ ) {
         for( j = 0 ; RETROSOFT_GLYPHS_COUNT > j ; j++ ) {
            debug_printf( RETROSOFT_TRACE_LVL,
               "destroying glyph " SIZE_T_FMT "...", j );
            retroflat_destroy_bitmap( &(gc_font_bmps[h][i][j]) );
         }
      }
#ifdef RETROSOFT_PRELOAD_COLORS
   }
#endif /* RETROSOFT_PRELOAD_COLORS */

#  endif /* !RETROFLAT_NO_STRING */
}

/* === */

void retrosoft_line_strategy(
   int x1, int y1, int x2, int y2,
   uint8_t* p_for_axis, uint8_t* p_off_axis, int16_t dist[2],
   int16_t start[2], int16_t end[2], int16_t iter[2],
   int16_t* p_inc, int16_t* p_delta
) {

   /* Figure out strategy based on line slope. */
   if( abs( y2 - y1 ) < abs( x2 - x1 ) ) {
      if( x1 > x2 ) {
         start[RETROFLAT_LINE_X] = x2;
         start[RETROFLAT_LINE_Y] = y2;
         end[RETROFLAT_LINE_X] = x1;
         end[RETROFLAT_LINE_Y] = y1;
         *p_for_axis = RETROFLAT_LINE_X;
      } else {
         start[RETROFLAT_LINE_X] = x1;
         start[RETROFLAT_LINE_Y] = y1;
         end[RETROFLAT_LINE_X] = x2;
         end[RETROFLAT_LINE_Y] = y2;
         *p_for_axis = RETROFLAT_LINE_X;
      }
   } else {
      if( y2 < y1 ) {
         start[RETROFLAT_LINE_X] = x2;
         start[RETROFLAT_LINE_Y] = y2;
         end[RETROFLAT_LINE_X] = x1;
         end[RETROFLAT_LINE_Y] = y1;
         *p_for_axis = RETROFLAT_LINE_Y;
      } else {
         start[RETROFLAT_LINE_X] = x1;
         start[RETROFLAT_LINE_Y] = y1;
         end[RETROFLAT_LINE_X] = x2;
         end[RETROFLAT_LINE_Y] = y2;
         *p_for_axis = RETROFLAT_LINE_Y;
      }
   }

   /* C89 requires const initializers, so do math down here. */
   *p_off_axis = 1 - *p_for_axis;
   iter[RETROFLAT_LINE_X] = start[RETROFLAT_LINE_X];
   iter[RETROFLAT_LINE_Y] = start[RETROFLAT_LINE_Y];
   dist[RETROFLAT_LINE_X] = end[RETROFLAT_LINE_X] - start[RETROFLAT_LINE_X];
   dist[RETROFLAT_LINE_Y] = end[RETROFLAT_LINE_Y] - start[RETROFLAT_LINE_Y];

   /* Adjust delta/slope for off-axis. */
   *p_delta = (2 * dist[*p_off_axis]) - dist[*p_for_axis];
   if( 0 > dist[*p_off_axis] ) {
      *p_inc = -1;
      dist[*p_off_axis] *= -1;
   } else {
      *p_inc = 1;
   }


}

#if defined( RETROFLAT_OPENGL ) || \
   defined( RETROFLAT_API_PC_BIOS ) || \
   defined( RETROFLAT_SOFT_LINES )

void retrosoft_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, uint8_t flags
) {

   uint8_t for_axis = 0,
      off_axis = 0;
   int16_t dist[2],
      start[2],
      end[2],
      iter[2],
      inc = 1,
      delta = 0;

   /* TODO: Handle thickness. */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   retroflat_px_lock( target );

   retrosoft_line_strategy(
      x1, y1, x2, y2,
      &for_axis, &off_axis, dist, start, end, iter, &inc, &delta );

   for(
      iter[for_axis] = start[for_axis] ;
      end[for_axis] > iter[for_axis] ;
      iter[for_axis]++
   ) {

      /*
      if(
         (size_t)iter[RETROFLAT_LINE_X] < (size_t)retroflat_screen_w() &&
         (size_t)iter[RETROFLAT_LINE_Y] < (size_t)retroflat_screen_h()
      ) {
      */
      retroflat_px(
         target, color,
         iter[RETROFLAT_LINE_X], iter[RETROFLAT_LINE_Y], 0 );
      /* } */

      /* Increment off-axis based on for-axis. */
      if( 0 < delta ) {
         iter[off_axis] += inc;
         delta += (2 * (dist[off_axis] - dist[for_axis]));
      } else {
         delta += (2 * dist[off_axis]);
      }
   }

   retroflat_px_release( target );
}

#endif /* RETROFLAT_OPENGL || RETROFLAT_SOFT_LINES */

/* === */

void retrosoft_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int x, int y, int w, int h, uint8_t flags
) {
   int x_iter = 0,
      y_iter = 0;

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   retroflat_px_lock( target );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {

      for( y_iter = y ; y_iter < y + h ; y_iter++ ) {
         for( x_iter = x ; x_iter < x + w ; x_iter++ ) {
            /* TODO: Optimize filling 4-byte sequences! */
            retroflat_px( target, color_idx, x_iter, y_iter, 0 );
         }
      }

   } else {

#ifdef RETROFLAT_SOFT_LINES
      retrosoft_line( target, color_idx, x, y, x + w, y, 0 );
      retrosoft_line( target, color_idx, x + w, y, x + w, y + h, 0 );
      retrosoft_line( target, color_idx, x + w, y + h, x, y + h, 0 );
      retrosoft_line( target, color_idx, x, y + h, x, y, 0 );
#else
      retroflat_line( target, color_idx, x, y, x + w, y, 0 );
      retroflat_line( target, color_idx, x + w, y, x + w, y + h, 0 );
      retroflat_line( target, color_idx, x + w, y + h, x, y + h, 0 );
      retroflat_line( target, color_idx, x, y + h, x, y, 0 );
#endif

   }

   retroflat_px_release( target );
}

/* === */

void retrosoft_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags
) {
   int32_t i = 0,
      i_prev = 0;
   uint16_t px_x1 = 0,
      px_y1 = 0,
      px_x2 = 0,
      px_y2 = 0;

   /* TODO: Switch to Bresenham algorithm. */

   /* TODO: Filled ellipse. */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   retroflat_px_lock( target );

   do {
      /* For the soft_lut, input numbers are * 1000... so 0.1 becomes 100. */
      for( i = 100 ; 2 * RETROFP_PI + 100 > i ; i += 100 ) {
         i_prev = i - 100;

         px_x1 = x + (w / 2) + retrofp_cos( i_prev, w / 2 );
         px_y1 = y + (h / 2) + retrofp_sin( i_prev, h / 2 );
         px_x2 = x + (w / 2) + retrofp_cos( i, w / 2 );
         px_y2 = y + (h / 2) + retrofp_sin( i, h / 2 );

         /*
         if(
            retroflat_bitmap_w( target ) <= px_x1 ||
            retroflat_bitmap_h( target ) <= px_y1 ||
            retroflat_bitmap_w( target ) <= px_x2 ||
            retroflat_bitmap_h( target ) <= px_y2
         ) {
            continue;
         }
         */

         retroflat_line( target, color, px_x1, px_y1, px_x2, px_y2, 0 );  
      }

      /* Keep shrinking and filling if required. */
      if( w > h ) {
         x++;
         w--;
         if( 0 < h ) {
            y++;
            h--;
         }
      } else if( h > w ) {
         y++;
         h--;
         if( 0 < w ) {
            x++;
            w--;
         }
      } else {
         x++;
         w--;
         y++;
         h--;
      }
      assert( 0 <= w );
      assert( 0 <= h );
      /* debug_printf( 1, "ELLIPSE %d, %d", w, h ); */
   } while(
      (RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags)) &&
      (0 < w || 0 < h)
   );

   retroflat_px_release( target );
}

#  ifndef RETROFLAT_NO_STRING

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

   if( 0 == str_sz ) {
      str_sz = strlen( str );
   }

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
         RETROSOFT_GLYPH_W_SZ, RETROSOFT_GLYPH_H_SZ, RETROFLAT_INSTANCE_NULL );

      x += 8;
   }
}

#  endif /* !RETROFLAT_NO_STRING */

#else

#endif /* RETROSFT_C */

#endif /* !RETROSFT_H */

