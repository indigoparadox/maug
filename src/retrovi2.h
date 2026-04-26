
#ifndef RETROVI2_H
#define RETROVI2_H

/**
 * \addtogroup maug_retroflt
 * \{
 * \addtogroup maug_retroview RetroFlat Grid API
 * \{
 * \file retrovi2.h
 */

struct RETROFLAT_VIEWPORT {
   /**
    * \note Technically this coordinate starts at 1, as 0 will always be
    *       invisible off-screen in the hardware scroll area.
    */
   retrotile_coord_t world_tile_x;
   /**
    * \note Technically this coordinate starts at 1, as 0 will always be
    *       invisible off-screen in the hardware scroll area.
    */
   retrotile_coord_t world_tile_y;
   retrotile_coord_t world_tile_w;
   retrotile_coord_t world_tile_h;
   /**
    * \brief Width of the viewport/screen in tiles, *including* off-screen
    *        buffer portion used for scrolling.
    */
   retrotile_coord_t screen_tile_w;
   /**
    * \brief Width of the viewport/screen in tiles, *including* off-screen
    *        buffer portion used for scrolling.
    */
   retrotile_coord_t screen_tile_h;
   int8_t px_x;
   int8_t px_y;
   size_t grid_ct;
   MAUG_MHANDLE grid_h;
   /**
    * \brief Grid that covers the full screen in tiles, with 1 extra tile just
    *        off-screen on every border. The visible upper-left is 0, 0, the
    *        extra left border is -1 on each axis, etc.
    */
   retroflat_tile_t* grid;
};

#ifndef RETROVIEW_TRACE_LVL
#  define RETROVIEW_TRACE_LVL 0
#endif /* !RETROVIEW_TRACE_LVL */

#define retroflat_viewport_constrain_px( x, y, bmp, retact ) \
   if( \
      x >= retroflat_bitmap_w( bmp ) || y >= retroflat_bitmap_h( bmp ) || \
      0 > x || 0 > y \
   ) { \
      debug_printf( RETROVIEW_TRACE_LVL, \
         "attempted draw at %d, %d, out of bounds %d x %d", \
         x, y, retroflat_bitmap_w( bmp ), retroflat_bitmap_h( bmp ) ); \
      retact; \
   }

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Lock access to RETROFLAT_VIEWPORT::refresh_grid in memory.
 *
 * This should be called before making references to the refresh grid e.g. with
 * retroflat_viewport_tile_is_stale().
 */
#define retroview_lock_grid() \
   if( NULL == g_retroflat_state->viewport.grid ) { \
      maug_mlock( \
         g_retroflat_state->viewport.grid_h, \
         g_retroflat_state->viewport.grid ); \
      maug_cleanup_if_null_lock( retroflat_tile_t*, \
         g_retroflat_state->viewport.grid ); \
   }

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Unlock access to RETROFLAT_VIEWPORT::refresh_grid in memory.
 *
 * This should be called when references to the refresh grid are no longer in
 * use and may be invalidated by the system.
 */
#define retroview_unlock_grid() \
   if( NULL != g_retroflat_state->viewport.grid ) { \
      maug_munlock( \
         g_retroflat_state->viewport.grid_h, \
         g_retroflat_state->viewport.grid ); \
   }

#define retroview_world_px_x() \
   (g_retroflat_state->viewport.world_tile_x * RETROFLAT_TILE_W)

#define retroview_world_px_y() \
   (g_retroflat_state->viewport.world_tile_y * RETROFLAT_TILE_H)

/**
 * \brief Translate a horizontal world tile coordinate to screen tile coordinate
 *        for RETROFLAT_VIEWPORT::grid.
 */
#define retroview_grid_to_px_x( x_tile ) \
   _retroview_grid_to_px_xy( x_tile, x, RETROFLAT_TILE_W )

/**
 * \brief Translate a vertical world tile coordinate to screen tile coordinate
 *        for RETROFLAT_VIEWPORT::grid.
 */
#define retroview_grid_to_px_y( y_tile ) \
   _retroview_grid_to_px_xy( y_tile, y, RETROFLAT_TILE_H )

#define _retroview_grid_to_px_xy( xy_tile, xy, tile_sz ) \
   (((xy_tile) - g_retroflat_state->viewport.world_tile_ ## xy) \
         * tile_sz) /* Translate to pixels. */

/**
 * \brief Internal viewport movement to be called by platform-specific viewport
 *        implementation, as part of its viewport handling.
 * \param xy "x" or "y" depending on dimension being moved.
 * \param wh "w" or "h" depending on whether we're moving "x" or "y".
 * \param tile_sz RETROFLAT_TILE_W or RETROFLAT_TILE_H depending on "x" or "y".
 */
#define _retroview_move_xy( xy_px, move, xy, wh, tile_sz ) \
   assert( (tile_sz) > (xy_px) ); \
   /* TODO: Use outside range check to reduce branches. */ \
   if( \
      /* If we have hardware scrolling, enable the off-screen buffer zone.
       * If not, then the viewport starts at 0, 0.
       */ \
      ((RETROFLAT_STATE_FLAG_HWSCROLLING == \
         (RETROFLAT_STATE_FLAG_HWSCROLLING & \
            g_retroflat_state->retroflat_flags)) ? -1 : 0) > \
      /* Don't worry about the px/tile conversion so much, here, just see if
       * the positiveness/negativeness would take us out of the world.
       */ \
      g_retroflat_state->viewport.world_tile_ ## xy + \
         (((xy_px) > 0) - ((xy_px) < 0)) || \
      \
      g_retroflat_state->viewport.world_tile_ ## wh <= \
         g_retroflat_state->viewport.world_tile_ ## xy + \
            ((((xy_px) > 0) - ((xy_px) < 0)) * \
               (g_retroflat_state->viewport.screen_tile_ ## wh - 1)) \
   ) { \
      /* Unable to move without leaving the world. */ \
      debug_printf( RETROVIEW_TRACE_LVL, \
         "attempting to leave world, " #xy "_wtile %d + " #xy "_px %d = %d " \
         "(wtile_" #wh ": %d)", \
         g_retroflat_state->viewport.world_tile_ ## xy, \
            ((xy_px) > 0) - ((xy_px) < 0), \
         g_retroflat_state->viewport.world_tile_ ## xy + \
            ((xy_px) > 0) - ((xy_px) < 0), \
         g_retroflat_state->viewport.world_tile_ ## wh ); \
      move = 0; \
   } else { \
      /* Just increment the pixel offset. */ \
      g_retroflat_state->viewport.px_ ## xy += (xy_px); \
      move = 1; \
      if( tile_sz <= abs( g_retroflat_state->viewport.px_ ## xy ) ) { \
         /* Pixels exceed tile size, so increment world tile and reset pixel
          * offset to 0.
          */ \
         g_retroflat_state->viewport.world_tile_ ## xy += \
            ((xy_px) > 0) - ((xy_px) < 0); \
         g_retroflat_state->viewport.px_ ## xy = 0; \
      } \
   }

/**
 * 
 */
#define _retroflat_screen_tile_wh( wh, tile_sz ) \
   ((retroflat_screen_ ## wh() / tile_sz) + \
      (0 != (retroflat_screen_ ## wh() % tile_sz)))

MERROR_RETVAL retroview_init(
   retrotile_coord_t world_w, retrotile_coord_t world_h );

void retroview_shutdown();

MERROR_RETVAL retroview_grid_set_tile(
   retrotile_coord_t x_tile, retrotile_coord_t y_tile,
   retroflat_tile_t tile_in );

MERROR_RETVAL retroview_grid_set_px(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px, retroflat_tile_t tile_in );

retroflat_tile_t retroview_grid_get_px(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px );

uint8_t retroview_focus(
   retroflat_pxxy_t x1, retroflat_pxxy_t y1,
   retroflat_pxxy_t range, retroflat_pxxy_t speed );

MERROR_RETVAL retroview_dirty_fuzzy_px(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px );

MERROR_RETVAL _retroview_trim_px(
   struct RETROFLAT_BITMAP* bitmap,
   int16_t instance,
   retroflat_pxxy_t* s_x, retroflat_pxxy_t* s_y,
   retroflat_pxxy_t* d_x, retroflat_pxxy_t* d_y,
   retroflat_pxxy_t* w, retroflat_pxxy_t* h );

MERROR_RETVAL _retroview_hwscroll(
   retroflat_pxxy_t* x_px, retroflat_pxxy_t* y_px,
   retroflat_pxxy_t w_px, retroflat_pxxy_t h_px, int16_t instance );

uint8_t retroview_move_x( retroflat_pxxy_t x_px );

uint8_t retroview_move_y( retroflat_pxxy_t y_px );

/*! \} */ /* maug_retroview */

/*! \} */ /* maug_retroflt */

#elif defined( RETROVIW_C )

MERROR_RETVAL retroview_init(
   retrotile_coord_t world_w, retrotile_coord_t world_h
) {
   MERROR_RETVAL retval = MERROR_OK;

   g_retroflat_state->viewport.world_tile_w = world_w;
   g_retroflat_state->viewport.world_tile_h = world_h;

   if(
      RETROFLAT_STATE_FLAG_HWSCROLLING ==
      (RETROFLAT_STATE_FLAG_HWSCROLLING & g_retroflat_state->retroflat_flags)
   ) {
      /* Start the world tiles at -1 always to keep the left-most border
      * visible.
      */
      g_retroflat_state->viewport.world_tile_x = -1;
      g_retroflat_state->viewport.world_tile_y = -1;
   } else {
      g_retroflat_state->viewport.world_tile_x = 0;
      g_retroflat_state->viewport.world_tile_y = 0;
   }

   if( (MAUG_MHANDLE)NULL != g_retroflat_state->viewport.grid_h ) {
      maug_mfree( g_retroflat_state->viewport.grid_h );
   }

   /* Allocate the viewport refresh grid based on current screen. */
   if(
      RETROFLAT_STATE_FLAG_HWSCROLLING ==
      (RETROFLAT_STATE_FLAG_HWSCROLLING & g_retroflat_state->retroflat_flags)
   ) {
      /* Allocate two extra tiles for the off-screen zone. */
      g_retroflat_state->viewport.screen_tile_w =
         (_retroflat_screen_tile_wh( w, RETROFLAT_TILE_W ) + 2);
      g_retroflat_state->viewport.screen_tile_h =
         (_retroflat_screen_tile_wh( h, RETROFLAT_TILE_H ) + 2);
   } else {
      g_retroflat_state->viewport.screen_tile_w =
         _retroflat_screen_tile_wh( w, RETROFLAT_TILE_W );
      g_retroflat_state->viewport.screen_tile_h =
         _retroflat_screen_tile_wh( h, RETROFLAT_TILE_H );
   }

   g_retroflat_state->viewport.grid_ct = 
      g_retroflat_state->viewport.screen_tile_w *
      g_retroflat_state->viewport.screen_tile_h;
   maug_malloc_test(
      g_retroflat_state->viewport.grid_h,
      g_retroflat_state->viewport.grid_ct,
      sizeof( retroflat_tile_t ) );
   debug_printf( RETROVIEW_TRACE_LVL,
      "viewport refresh grid initialized for %d x %d screen: %d x %d tiles",
      retroflat_screen_w(), retroflat_screen_h(),
      g_retroflat_state->viewport.screen_tile_w,
      g_retroflat_state->viewport.screen_tile_h );

cleanup:

   return retval;
}

/* === */

void retroview_shutdown() {
   if( (MAUG_MHANDLE)NULL != g_retroflat_state->viewport.grid_h ) {
      maug_mfree( g_retroflat_state->viewport.grid_h );
   }
}

/* === */

MERROR_RETVAL retroview_grid_set_tile(
   retrotile_coord_t x_tile, retrotile_coord_t y_tile, retroflat_tile_t tile_in
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t tile_idx = (y_tile * g_retroflat_state->viewport.screen_tile_w)
      + x_tile;

   if( tile_idx >= g_retroflat_state->viewport.grid_ct ) {
      return MERROR_OVERFLOW;
   }

   retroview_lock_grid();
   g_retroflat_state->viewport.grid[tile_idx] = tile_in;
   retroview_unlock_grid();

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroview_grid_set_px(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px, retroflat_tile_t tile_in
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t tile_idx = ((((y_px) + 1) >> RETROFLAT_TILE_H_BITS) *
      g_retroflat_state->viewport.screen_tile_w) +
         (((x_px) + 1) >> RETROFLAT_TILE_W_BITS);

   if( tile_idx >= g_retroflat_state->viewport.grid_ct ) {
      return MERROR_OVERFLOW;
   }

   retroview_lock_grid();
   g_retroflat_state->viewport.grid[tile_idx] = tile_in;
   retroview_unlock_grid();

cleanup:

   return retval;
}

/* === */

retroflat_tile_t retroview_grid_get_px(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t tile_idx = ((((y_px) + 1) >> RETROFLAT_TILE_H_BITS) *
      g_retroflat_state->viewport.screen_tile_w) +
         (((x_px) + 1) >> RETROFLAT_TILE_W_BITS);
   retroflat_tile_t tile_out = -1;

   if( tile_idx >= g_retroflat_state->viewport.grid_ct ) {
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   retroview_lock_grid();
   tile_out = g_retroflat_state->viewport.grid[tile_idx];
   retroview_unlock_grid();

cleanup:

   if( MERROR_OK != retval ) {
      return merror_retval_to_sz( retval );
   }

   return tile_out;
}

/* === */

uint8_t retroview_focus(
   retroflat_pxxy_t x1, retroflat_pxxy_t y1,
   retroflat_pxxy_t range, retroflat_pxxy_t speed
) {
   uint8_t moved = 0,
      new_moved = 0;
   int16_t new_pt = 0;

   /* TODO: Reduce conditional jumps with math. */

   /* Test if the screen is scrolling east/west. */
   new_pt = x1 - (g_retroflat_state->viewport.world_tile_x * RETROFLAT_TILE_W);
   if( new_pt > (retroflat_screen_w() >> 1) + range ) {
      new_moved = retroview_move_x(
         gc_retroflat_offsets8_x[RETROFLAT_DIR8_EAST] * speed );
      if( !moved && new_moved ) {
         moved = new_moved;
      }
   } else if( new_pt < (retroflat_screen_w() >> 1) - range ) {
      new_moved = retroview_move_x(
         gc_retroflat_offsets8_x[RETROFLAT_DIR8_WEST] * speed );
      if( !moved && new_moved ) {
         moved = new_moved;
      }
   }

   /* Test if the screen is scrolling north/south. */
   new_pt = y1 - (g_retroflat_state->viewport.world_tile_y * RETROFLAT_TILE_H);
   if( new_pt > (retroflat_screen_h() >> 1) + range ) {
      new_moved = retroview_move_y(
         gc_retroflat_offsets8_y[RETROFLAT_DIR8_SOUTH] * speed );
      if( !moved && new_moved ) {
         moved = new_moved;
      }
   } else if( new_pt < (retroflat_screen_h() >> 1) - range ) {
      new_moved = retroview_move_y(
         gc_retroflat_offsets8_y[RETROFLAT_DIR8_NORTH] * speed );
      if( !moved && new_moved ) {
         moved = new_moved;
      }
   }

   return moved;
}

/* === */

MERROR_RETVAL retroview_dirty_fuzzy_px(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px
) {
   MERROR_RETVAL retval = MERROR_OK;
   retrotile_coord_t x_tile = 
         ((x_px - retroview_world_px_x()) >> RETROFLAT_TILE_W_BITS),
      y_tile =
         ((y_px - retroview_world_px_y()) >> RETROFLAT_TILE_H_BITS);
   int i = 0;

   for( i = 0 ; 8 > i ; i++ ) {
      retval = retroview_grid_set_tile(
         x_tile + gc_retroflat_offsets8_x[i],
         y_tile + gc_retroflat_offsets8_y[i],
         -1 );
      if( MERROR_OVERFLOW == retval ) {
         retval = MERROR_OK;
      }
      maug_cleanup_if_not_ok();
   }
   retval = retroview_grid_set_tile( x_tile, y_tile, -1 );

cleanup:
   return retval;
}

/* === */

MERROR_RETVAL _retroview_trim_px(
   struct RETROFLAT_BITMAP* bitmap,
   int16_t instance,
   retroflat_pxxy_t* s_x, retroflat_pxxy_t* s_y,
   retroflat_pxxy_t* d_x, retroflat_pxxy_t* d_y,
   retroflat_pxxy_t* w, retroflat_pxxy_t* h
) {
   retroflat_pxxy_t trim_bottom = *d_y + *h;
   retroflat_pxxy_t trim_right = *d_x + *w;
   retroflat_pxxy_t viewport_bottom = retroflat_bitmap_h( bitmap );
   retroflat_pxxy_t viewport_right = retroflat_bitmap_w( bitmap );
   retroflat_pxxy_t viewport_left = 0;
   retroflat_pxxy_t viewport_top = 0;

   /* TODO: Reduce conditional jumps with math. */

   if( viewport_bottom < *d_y || trim_bottom < viewport_top ) {
#ifdef RETROFLAT_TRACE_CONSTRAIN
      error_printf( "attempted to blit bitmap way out of bounds at %d, %d!",
         *d_x, *d_y );
#endif /* RETROFLAT_TRACE_CONSTRAIN */
      return MERROR_GUI;

   } else if( viewport_bottom <= trim_bottom ) {
#ifdef RETROFLAT_TRACE_CONSTRAIN
      error_printf(
         "trimming " SIZE_T_FMT " pixels to get " SIZE_T_FMT " under "
            SIZE_T_FMT,
         trim_bottom - viewport_bottom, trim_bottom, viewport_bottom );
#endif /* RETROFLAT_TRACE_CONSTRAIN */
      *h -= (trim_bottom - viewport_bottom);

   } else if( viewport_top > *d_y ) {
#ifdef RETROFLAT_TRACE_CONSTRAIN
      error_printf(
         "trimming " SIZE_T_FMT " pixels to get " SIZE_T_FMT " under "
            SIZE_T_FMT,
         trim_bottom - viewport_bottom, trim_bottom, viewport_bottom );
#endif /* RETROFLAT_TRACE_CONSTRAIN */
      *h -= (viewport_top - *d_y);
      if( NULL != s_y ) {
         *s_y += (viewport_top - *d_y);
      }
      *d_y += (viewport_top - *d_y);
      assert( viewport_top == *d_y );
   }

   if( viewport_right < *d_x || trim_right < viewport_left ) {
#ifdef RETROFLAT_TRACE_CONSTRAIN
      error_printf( "attempted to blit bitmap way out of bounds at %d, %d!",
         *d_x, *d_y );
#endif /* RETROFLAT_TRACE_CONSTRAIN */
      return MERROR_GUI;

   } else if( viewport_right <= trim_right ) {
#ifdef RETROFLAT_TRACE_CONSTRAIN
      error_printf(
         "trimming " SIZE_T_FMT " pixels to get " SIZE_T_FMT " under "
            SIZE_T_FMT,
         trim_right - viewport_right, trim_right, viewport_right );
#endif /* RETROFLAT_TRACE_CONSTRAIN */
      *w -= (trim_right - viewport_right);

   } else if( viewport_left > *d_x ) {
#ifdef RETROFLAT_TRACE_CONSTRAIN
      error_printf(
         "trimming " SIZE_T_FMT " pixels to get " SIZE_T_FMT " under "
            SIZE_T_FMT,
         trim_bottom - viewport_bottom, trim_bottom, viewport_bottom );
#endif /* RETROFLAT_TRACE_CONSTRAIN */
      *w -= (viewport_left - *d_x);
      if( NULL != s_x ) {
         *s_x += (viewport_left - *d_x);
      }
      *d_x += (viewport_left - *d_x);
      assert( viewport_left == *d_x );
   }

   if( 0 == w && 0 == h ) {
      return MERROR_GUI;
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL _retroview_hwscroll(
   retroflat_pxxy_t* x_px, retroflat_pxxy_t* y_px,
   retroflat_pxxy_t w_px, retroflat_pxxy_t h_px, int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;
   retroflat_pxxy_t i_x, i_y;
   retroflat_tile_t grid_tile;

   if( 0 > instance ) {
      if(
         retroflat_outside_rect( *x_px, *y_px, -1, -1,
            retroflat_screen_w() + (2 * RETROFLAT_TILE_W),
            retroflat_screen_h() + (2 * RETROFLAT_TILE_H) )
      ) {
         /* This tile is truly offscreen. */
         retval = MERROR_OVERFLOW;
         goto cleanup;
      }

      if( 0 > instance ) {
         /* This is a tile, check to see if it needs to be refreshed. */
         grid_tile = retroview_grid_get_px( *x_px, *y_px );
         if( 0 <= grid_tile && (-1 * instance) == grid_tile ) {
            /* Tile does not need to be redrawn. */
            retval = MERROR_PREEMPT;
            goto cleanup;
         }
      }
      retroview_grid_set_px(
         *x_px, *y_px, -1 * instance /* Invert back positive. */ );

      /* There is no bump. The tile is drawn on-screen, unmolested. */

   } else if( 0 < instance ) {

      *x_px -= (g_retroflat_state->viewport.world_tile_x * RETROFLAT_TILE_W);
      *y_px -= (g_retroflat_state->viewport.world_tile_y * RETROFLAT_TILE_H);

   } else {
      /* No instance, sprite or tile. Must be a window or something! */
      
      if(
         ((RETROFLAT_STATE_FLAG_HWSCROLLING == \
            (RETROFLAT_STATE_FLAG_HWSCROLLING & \
               g_retroflat_state->retroflat_flags)))
      ) {
         /* Always keep the window in the visible screen area. */
         *x_px += RETROFLAT_TILE_W + g_retroflat_state->viewport.px_x;
         *y_px += RETROFLAT_TILE_H + g_retroflat_state->viewport.px_y;
      }

      /* Invalidate any tiles under the window. */
      for(
         i_x = *x_px;
         *x_px + w_px + RETROFLAT_TILE_W >= i_x;
         i_x += RETROFLAT_TILE_W
      ) {
         for(
            i_y = *y_px;
            *y_px + h_px + RETROFLAT_TILE_H >= i_y;
            i_y += RETROFLAT_TILE_H
         ) {
            retroview_grid_set_px( i_x, i_y, -1 );
         }
      }
   }

cleanup:

   return retval;
}

#endif /* RETROVI2_H */

