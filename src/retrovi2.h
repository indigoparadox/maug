
#ifndef RETROVI2_H
#define RETROVI2_H

struct RETROFLAT_VIEWPORT {
   retrotile_coord_t world_tile_x;
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
   MAUG_MHANDLE grid_h;
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

#define retroview_invalidate_px( x_px, y_px ) \
   retroview_invalidate( (x_px) / RETROFLAT_TILE_W, (y_px) / RETROFLAT_TILE_H )

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

/**
 * \param x_px X coordinate of pixel to check.
 * \param y_px Y coordinate of pixel to check.
 * \note The X and Y coordinates are assumed to have been normalized (i.e. +1
 *       has been added so that they are never negative!
 */
#define retroview_grid_at_px( x_px, y_px ) \
   (g_retroflat_state->viewport.grid[ \
      (((y_px) >> RETROFLAT_TILE_H_BITS) * \
         g_retroflat_state->viewport.screen_tile_w) + \
            ((x_px) >> RETROFLAT_TILE_W_BITS)])

/**
 * \brief Internal viewport movement to be called by platform-specific viewport
 *        implementation, as part of its viewport handling.
 * \param xy "x" or "y" depending on dimension being moved.
 * \param wh "w" or "h" depending on whether we're moving "x" or "y".
 * \param tile_sz RETROFLAT_TILE_W or RETROFLAT_TILE_H depending on "x" or "y".
 */
#define _retroview_move_xy( xy_px, move, xy, wh, tile_sz ) \
   assert( (tile_sz) > (xy_px) ); \
   if( \
      /* Don't worry about the px/tile conversion so much, here, just see if
       * the positiveness/negativeness would take us out of the world.
       */ \
      0 > g_retroflat_state->viewport.world_tile_ ## xy + \
         (((xy_px) > 0) - ((xy_px) < 0)) || \
      g_retroflat_state->viewport.world_tile_ ## wh <= \
         g_retroflat_state->viewport.world_tile_ ## xy + \
            (((xy_px) > 0) - ((xy_px) < 0)) \
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

void retroview_invalidate( retrotile_coord_t x_tile, retrotile_coord_t y_tile );

uint8_t retroview_move_x( retroflat_pxxy_t x_px );

uint8_t retroview_move_y( retroflat_pxxy_t y_px );

#elif defined( RETROVIW_C )

MERROR_RETVAL retroview_init(
   retrotile_coord_t world_w, retrotile_coord_t world_h
) {
   MERROR_RETVAL retval = MERROR_OK;

   g_retroflat_state->viewport.world_tile_w = world_w;
   g_retroflat_state->viewport.world_tile_h = world_h;

   if( (MAUG_MHANDLE)NULL != g_retroflat_state->viewport.grid_h ) {
      maug_mfree( g_retroflat_state->viewport.grid_h );
   }

   /* Allocate the viewport refresh grid based on current screen. */
   g_retroflat_state->viewport.screen_tile_w = 
      (_retroflat_screen_tile_wh( w, RETROFLAT_TILE_W ) + 2);
   g_retroflat_state->viewport.screen_tile_h = 
      (_retroflat_screen_tile_wh( h, RETROFLAT_TILE_H ) + 2);
   maug_malloc_test(
      g_retroflat_state->viewport.grid_h,
      g_retroflat_state->viewport.screen_tile_w *
      g_retroflat_state->viewport.screen_tile_h,
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

void retroview_invalidate(
   retrotile_coord_t x_tile, retrotile_coord_t y_tile
) {

   /* TODO */

}

/* === */

MERROR_RETVAL retroflat_viewport_trim_px(
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

#endif /* RETROVI2_H */

