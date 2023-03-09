
#ifndef RETROSFT_H
#define RETROSFT_H

#define RETROSOFT_SETS_COUNT 2
#define RETROSOFT_GLYPHS_COUNT 95
#define RETROSOFT_GLYPH_W_SZ 8
#define RETROSOFT_GLYPH_H_SZ 8

MERROR_RETVAL retrosoft_load_glyph(
   size_t set_idx, size_t glyph_idx, struct RETROFLAT_BITMAP* bmp );

MERROR_RETVAL retrosoft_init();

void retrosoft_shutdown();

void retrosoft_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, uint8_t flags );

void retrosoft_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags );

void retrosoft_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags );

void retrosoft_string_sz(
   struct RETROFLAT_BITMAP* target, const char* str, int str_sz,
   const char* font_str, int* w_out, int* h_out );

void retrosoft_string(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   const char* str, int str_sz, const char* font_str, int x_orig, int y_orig,
   uint8_t flags );

#ifdef RETROSFT_C

#  include "mfont8x8.h"

static struct RETROFLAT_BITMAP
gc_font_bmps[RETROSOFT_SETS_COUNT][RETROSOFT_GLYPHS_COUNT];

/* === */

MERROR_RETVAL retrosoft_load_glyph(
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

   /* Draw the glyph onto the bitmap. */
   for( y = 0 ; RETROSOFT_GLYPH_H_SZ > y ; y++ ) {
      for( x = 0 ; RETROSOFT_GLYPH_W_SZ > x ; x++ ) {
         if( 1 == ((glyph_dots[y] >> x) & 0x01) ) {
            retroflat_px( bmp, RETROFLAT_COLOR_WHITE, x, y, 0 );
         }
      }
   }

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

   for( i = 0 ; RETROSOFT_SETS_COUNT > i ; i++ ) {
      for( j = 0 ; RETROSOFT_GLYPHS_COUNT > j ; j++ ) {
         retval = retrosoft_load_glyph( i, j, &(gc_font_bmps[i][j]) );
         maug_cleanup_if_not_ok();
      }
   }

cleanup:

   /* TODO: Unload loaded bitmaps if retval not OK. */

   return retval;
}

/* === */

void retrosoft_shutdown() {
   size_t i = 0,
      j = 0;

   for( i = 0 ; RETROSOFT_SETS_COUNT > i ; i++ ) {
      for( j = 0 ; RETROSOFT_GLYPHS_COUNT > j ; j++ ) {
         retroflat_destroy_bitmap( &(gc_font_bmps[i][j]) );
      }
   }
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
   int lock_ret = 0,
      locked_target_internal = 0;

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

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

void retrosoft_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags
) {
   int32_t i = 0,
      i_prev = 0;
   int16_t lock_ret = 0,
      locked_target_internal = 0,
      px_x1 = 0,
      px_y1 = 0,
      px_x2 = 0,
      px_y2 = 0;

   /* TODO: Switch to Bresenham algorithm. */

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

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
   struct RETROFLAT_BITMAP* target, const char* str, int str_sz,
   const char* font_str, int* w_out, int* h_out
) {
   /* TODO: Put a little more effort into sizing. */
   *w_out = RETROSOFT_GLYPH_W_SZ * str_sz;
   *h_out = RETROSOFT_GLYPH_H_SZ;
}

/* === */

void retrosoft_string(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   const char* str, int str_sz, const char* font_str, int x_orig, int y_orig,
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

      retroflat_blit_bitmap(
         target, &(gc_font_bmps[0][glyph_idx]), 0, 0, x, y_orig,
         RETROSOFT_GLYPH_W_SZ, RETROSOFT_GLYPH_H_SZ );

      x += 8;
   }
}

#else

#endif /* RETROSFT_C */

#endif /* !RETROSFT_H */

