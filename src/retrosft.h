
#ifndef RETROSFT_H
#define RETROSFT_H

/**
 * \addtogroup maug_retrosft RetroFlat Soft Shapes API
 * \brief Tools for drawing shape primatives.
 *
 * This library is also used in conjunction with retro3D, in order to
 * manipulate texture bitmaps.
 *
 * \{
 */

/**
 * \file retrosft.h
 * \brief Tools for drawing shape primatives.
 */

#define RETROFLAT_LINE_X 0
#define RETROFLAT_LINE_Y 1

#ifndef RETROSOFT_TRACE_LVL
#  define RETROSOFT_TRACE_LVL 0
#endif /* RETROSOFT_TRACE_LVL */

#ifdef RETROFLAT_3D
typedef struct RETROFLAT_3DTEX retrosoft_target_t;
#else
typedef struct RETROFLAT_BITMAP retrosoft_target_t;
#endif

/**
 * \brief Type of callback function used to produce pixels on a surface.
 *
 * This is switched between ::RETROFLAT_3DTEX and ::RETROFLAT_BITMAP, depending
 * on whether this is a 3D or 2D engine.
 */
typedef void (*retrosoft_px_cb)(
   retrosoft_target_t* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags );

/**
 * \brief Draw a line from x1, y1 to x2, y2.
 * \warning This function does not check if supplied bitmaps are read-only!
 *          Such checks should be performed by wrapper functions calling it!
 */
void retrosoft_line(
   retrosoft_target_t* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, uint8_t flags );

/**
 * \brief Draw a rectangle at the given coordinates, with the given dimensions.
 * \warning This function does not check if supplied bitmaps are read-only!
 *          Such checks should be performed by wrapper functions calling it!
 */
void retrosoft_rect(
   retrosoft_target_t* target, const RETROFLAT_COLOR color_idx,
   int x, int y, int w, int h, uint8_t flags );

/**
 * \brief Draw an ellipsoid at the given coordinates, with the given dimensions.
 * \todo Right now this uses integer polar coordinates; it needs to be redone
 *       to use the Midpoint circle algorithm or similar.
 * \warning This function does not check if supplied bitmaps are read-only!
 *          Such checks should be performed by wrapper functions calling it!
 */
void retrosoft_ellipse(
   retrosoft_target_t* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags );

/*! \} */ /* maug_retrosft */

#ifdef RETROSFT_C

/**
 * \brief Directly addressable callback to produce pixels on a surface.
 *
 * This is assigned in retroflat_init(), as that is when all of the _px()
 * callback functions are defined and available.
 */
retrosoft_px_cb g_retrosoft_px;

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
   retrosoft_target_t* target, RETROFLAT_COLOR color,
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

#ifndef RETROFLAT_3D
   /* Under 3D mode, we should never be drawing directly to the screen! */
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#endif /* !RETROFLAT_3D */

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
      g_retrosoft_px(
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
   retrosoft_target_t* target, const RETROFLAT_COLOR color_idx,
   int x, int y, int w, int h, uint8_t flags
) {
   int x_iter = 0,
      y_iter = 0;

#ifndef RETROFLAT_3D
   /* Under 3D mode, we should never be drawing directly to the screen! */
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#endif /* !RETROFLAT_3D */

   retroflat_px_lock( target );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {

      for( y_iter = y ; y_iter < y + h ; y_iter++ ) {
         for( x_iter = x ; x_iter < x + w ; x_iter++ ) {
            /* TODO: Optimize filling 4-byte sequences! */
            g_retrosoft_px( target, color_idx, x_iter, y_iter, 0 );
         }
      }

   } else {

#if defined( RETROFLAT_SOFT_LINES ) || defined( RETROFLAT_3D )
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
   retrosoft_target_t* target, RETROFLAT_COLOR color,
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

#ifndef RETROFLAT_3D
   /* Under 3D mode, we should never be drawing directly to the screen! */
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#endif /* !RETROFLAT_3D */

   retroflat_px_lock( target );

   do {
      /* For the soft_lut, input numbers are * 1000... so 0.1 becomes 100. */
      for( i = 100 ; 2 * MFIX_PI + 100 > i ; i += 100 ) {
         i_prev = i - 100;

         px_x1 =
            /* Offset circle center by X. */
            x + (w / 2) + 
            /* Get Cartesian coord by multiplying polar coord by radius. */
            /* (Note that we convert to i *after* multiplication so the number
             * is big enough that the precision no longer matters! */
            mfix_to_i( mfix_cos( i_prev ) * w / 2 );

         px_y1 = y + (h / 2) + mfix_to_i( mfix_sin( i_prev ) * (h / 2) );
         px_x2 = x + (w / 2) + mfix_to_i( mfix_cos( i ) * (w / 2) );
         px_y2 = y + (h / 2) + mfix_to_i( mfix_sin( i ) * (h / 2) );

         /* We don't do bounds checks since the low-level API should handle
          * those! Performance!
          */

         retrosoft_line( target, color, px_x1, px_y1, px_x2, px_y2, 0 );  
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

#else

extern retrosoft_px_cb g_retrosoft_px;

#endif /* RETROSFT_C */

#endif /* !RETROSFT_H */

