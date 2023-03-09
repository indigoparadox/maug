
#ifndef RETROSFT_H
#define RETROSFT_H

void retrosoft_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, uint8_t flags );

void retrosoft_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags );

void retrosoft_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags );

#ifdef RETROSFT_C

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

#else
extern int16_t g_retroflat_soft_cos[];
extern int16_t g_retroflat_soft_sin[];
#endif /* RETROSFT_C */

#endif /* !RETROSFT_H */

