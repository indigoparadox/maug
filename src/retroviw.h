
#ifndef RETROVIW_H
#define RETROVIW_H

/**
 * \addtogroup maug_retroflt_viewport RetroFlat Viewport API
 * \brief A flexible API to facilitate tile-based views using hardware
 *        acceleration where available.
 *
 * If no hardware acceleration is available on the platform, then the platform
 * API header should define RETROFLAT_SOFT_VIEWPORT to enable the _generic
 * functions and suffixes for this functionality.
 * \{
 */

/**
 * \brief The viewport data struct.
 * \warning Many of the fields in this struct are calculated based on each
 *          other, and so updates should only be made through
 *          retroflat_viewport_set_pos_size()!
 */
struct RETROFLAT_VIEWPORT {
   /**
    * \brief X position of the viewport in real screen memory in pixels.
    *        Should only be retrieved through retroflat_viewport_screen_get_x()
    *        and set through retroflat_viewport_set_pos_size().
    * */
   retroflat_pxxy_t screen_x;
   /**
    * \brief Y position of the viewport in real screen memory in pixels.
    *        Should only be retrieved through retroflat_viewport_screen_get_y()
    *        and set through retroflat_viewport_set_pos_size()
    * */
   retroflat_pxxy_t screen_y;
   /**
    * \brief The X offset, in pixels, of the viewport on the world tilemap.
    *        Should only be retrieved through retroflat_viewport_world_x() and
    *        set through retroflat_viewport_set_world_pos().
    */
   retroflat_pxxy_t world_x;
   /**
    * \brief The Y offset, in pixels, of the viewport on the world tilemap.
    *        Should only be retrieved through retroflat_viewport_world_y() and
    *        set through retroflat_viewport_set_world_pos().
    */
   retroflat_pxxy_t world_y;
   /**
    * \brief The width of the entire world tilemap in pixels.
    *        Should only be retrieved through retroflat_viewport_world_w() and
    *        set through retroflat_viewport_set_world().
    */
   retroflat_pxxy_t world_w;
   /**
    * \brief The height of the entire world tilemap in pixels.
    *        Should only be retrieved through retroflat_viewport_world_h() and
    *        set through retroflat_viewport_set_world().
    */
   retroflat_pxxy_t world_h;
   /**
    * \brief Viewport width in pixels.
    *        Should only be retrieved through retroflat_viewport_screen_w() and
    *        set through retroflat_viewport_set_pos_size().
    */
   retroflat_pxxy_t screen_w;
   /**
    * \brief Viewport height in pixels.
    *        Should only be retrieved through retroflat_viewport_screen_w() and
    *        set through retroflat_viewport_set_pos_size().
    */
   retroflat_pxxy_t screen_h;
   /**
    * \brief Difference between viewport width and screen width in pixels.
    *        Should only be retrieved through
    *        retroflat_viewport_screen_w_remainder() and
    *        calculated through retroflat_viewport_set_pos_size().
    */
   retroflat_pxxy_t screen_w_remainder;
   /**
    * \brief Difference between viewport height and screen height in pixels.
    *        Should only be retrieved through
    *        retroflat_viewport_screen_h_remainder() and
    *        calculated through retroflat_viewport_set_pos_size().
    */
   retroflat_pxxy_t screen_h_remainder;
   /**
    * \brief The number of tiles across that fit in the viewport.
    *        Should only be retrieved through
    *        retroflat_viewport_screen_tile_w() and
    *        calculated through retroflat_viewport_set_pos_size().
    */
   int16_t screen_tile_w;
   /**
    * \brief The number of tiles high that fit in the viewport.
    *        Should only be retrieved through
    *        retroflat_viewport_screen_tile_h() and
    *        calculated through retroflat_viewport_set_pos_size().
    */
   int16_t screen_tile_h;
   int16_t world_tile_x;
   int16_t world_tile_y;
#ifndef RETROFLAT_NO_VIEWPORT_REFRESH
   MAUG_MHANDLE refresh_grid_h;
   /**
    * \brief A grid of tile values representing the last-drawn values on-screen.
    *
    * If the value for a tile in this grid matches the value about to be drawn,
    * the draw will be skipped. This increases performance a LOT on systems with
    * slow video memory access.
    *
    * This functionality may be disabled by defining the macro
    * RETROFLAT_NO_VIEWPORT_REFRESH on build.
    */
   retroflat_tile_t* refresh_grid;
#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */
};

#  define retroflat_screen_colors() (g_retroflat_state->screen_colors)

#ifndef DOCUMENTATION

#  define retroflat_viewport_world_x_generic() \
   (g_retroflat_state->viewport.world_x)

#  define retroflat_viewport_world_y_generic() \
   (g_retroflat_state->viewport.world_y)

#  define retroflat_viewport_world_tile_x_generic() \
   (g_retroflat_state->viewport.world_tile_x)

#  define retroflat_viewport_world_tile_y_generic() \
   (g_retroflat_state->viewport.world_tile_y)

#  define retroflat_viewport_world_w_generic() \
   (g_retroflat_state->viewport.world_w)

#  define retroflat_viewport_world_h_generic() \
   (g_retroflat_state->viewport.world_h)

#  define retroflat_viewport_screen_tile_w_generic() \
   (g_retroflat_state->viewport.screen_tile_w)

#  define retroflat_viewport_screen_tile_h_generic() \
   (g_retroflat_state->viewport.screen_tile_h)

#  define retroflat_viewport_screen_w_generic() \
   (g_retroflat_state->viewport.screen_w)

#  define retroflat_viewport_screen_h_generic() \
   (g_retroflat_state->viewport.screen_h)

#  define retroflat_viewport_screen_w_remainder_generic() \
   (g_retroflat_state->viewport.screen_w_remainder)

#  define retroflat_viewport_screen_h_remainder_generic() \
   (g_retroflat_state->viewport.screen_h_remainder)

#  define retroflat_viewport_set_world_generic( w, h ) \
   debug_printf( 1, "setting viewport size to %d x %d...", \
      (int16_t)(w), (int16_t)(h) ); \
   (g_retroflat_state->viewport.world_w) = w; \
   (g_retroflat_state->viewport.world_h) = h;

#  define retroflat_viewport_set_world_pos_generic( x, y ) \
   debug_printf( 1, "setting viewport world pos to %d, %d...", x, y ); \
   (g_retroflat_state->viewport.world_x) = x; \
   (g_retroflat_state->viewport.world_y) = y; \
   (g_retroflat_state->viewport.world_tile_x) = (x) >> RETROFLAT_TILE_W_BITS; \
   (g_retroflat_state->viewport.world_tile_y) = (y) >> RETROFLAT_TILE_H_BITS;

#  define retroflat_viewport_set_pos_size_generic( x_px, y_px, w_px, h_px ) \
   g_retroflat_state->viewport.screen_x = (x_px); \
   g_retroflat_state->viewport.screen_y = (y_px); \
   g_retroflat_state->viewport.screen_tile_w = \
      ((w_px) / RETROFLAT_TILE_W); \
   g_retroflat_state->viewport.screen_tile_h = \
      ((h_px) / RETROFLAT_TILE_H); \
   /* We're not adding the extra room here since this won't be used for
   * indexing or allocation but rather pixel detection.
   */ \
   g_retroflat_state->viewport.screen_w = \
      ((w_px) / RETROFLAT_TILE_W) * RETROFLAT_TILE_W; \
   g_retroflat_state->viewport.screen_h = \
      ((h_px) / RETROFLAT_TILE_H) * RETROFLAT_TILE_H; \
   g_retroflat_state->viewport.screen_w_remainder = \
      (x_px) + (w_px) - g_retroflat_state->viewport.screen_w; \
   g_retroflat_state->viewport.screen_h_remainder = \
      (y_px) + (h_px) - g_retroflat_state->viewport.screen_h;

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH

#  define retroflat_viewport_lock_refresh_generic() \
   if( NULL == g_retroflat_state->viewport.refresh_grid ) { \
      maug_mlock( \
         g_retroflat_state->viewport.refresh_grid_h, \
         g_retroflat_state->viewport.refresh_grid ); \
      maug_cleanup_if_null_lock( retroflat_tile_t*, \
         g_retroflat_state->viewport.refresh_grid ); \
   }

#  define retroflat_viewport_unlock_refresh_generic() \
   if( NULL != g_retroflat_state->viewport.refresh_grid ) { \
      maug_munlock( \
         g_retroflat_state->viewport.refresh_grid_h, \
         g_retroflat_state->viewport.refresh_grid ); \
   }

#  define _retroflat_viewport_refresh_tile_x( x_px ) \
   (((x_px) + RETROFLAT_TILE_W) >> RETROFLAT_TILE_W_BITS)

#  define _retroflat_viewport_refresh_tile_y( y_px ) \
   (((y_px) + RETROFLAT_TILE_H) >> RETROFLAT_TILE_H_BITS)

#  define retroflat_viewport_tile_is_stale( x_px, y_px, tile_id ) \
      ((tile_id) != \
      g_retroflat_state->viewport.refresh_grid[ \
         ((_retroflat_viewport_refresh_tile_y( y_px ) + 1) * \
            (g_retroflat_state->viewport.screen_tile_w + 2)) + \
               (_retroflat_viewport_refresh_tile_x( x_px ) + 1)])

#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

MERROR_RETVAL retroflat_viewport_set_refresh_generic(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px, retroflat_tile_t tid );

uint8_t retroflat_viewport_move_x_generic( int16_t x );

uint8_t retroflat_viewport_move_y_generic( int16_t y );

uint8_t retroflat_viewport_focus_generic(
   retroflat_pxxy_t x1, retroflat_pxxy_t y1,
   retroflat_pxxy_t range, retroflat_pxxy_t speed );

#  define retroflat_viewport_screen_x_generic( world_x ) \
   (g_retroflat_state->viewport.screen_x + \
      ((world_x) - retroflat_viewport_world_x()))

#  define retroflat_viewport_screen_y_generic( world_y ) \
   (g_retroflat_state->viewport.screen_y + \
      ((world_y) - retroflat_viewport_world_y()))

#  define retroflat_viewport_screen_get_x_generic() \
   (g_retroflat_state->viewport.screen_x)

#  define retroflat_viewport_screen_get_y_generic() \
   (g_retroflat_state->viewport.screen_y)

#endif /* !DOCUMENTATION */

#if defined( RETROFLAT_SOFT_VIEWPORT ) || defined( DOCUMENTATION )

#  ifndef RETROFLAT_NO_VIEWPORT_REFRESH
      /* These clamp world coordinates to tile borders to allow refresh grid to
       * function properly (smooth-scrolling tiles will always be in motion).
       */

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current viewport X position in the world in pixels.
 */
#     define retroflat_viewport_world_x() \
         ((retroflat_viewport_world_x_generic() \
            >> RETROFLAT_TILE_W_BITS) << RETROFLAT_TILE_W_BITS)

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current viewport Y position in the world in pixels.
 */
#     define retroflat_viewport_world_y() \
         ((retroflat_viewport_world_y_generic() \
            >> RETROFLAT_TILE_H_BITS) << RETROFLAT_TILE_H_BITS)
#  else
#     define retroflat_viewport_world_x() retroflat_viewport_world_x_generic()
#     define retroflat_viewport_world_y() retroflat_viewport_world_y_generic()
#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

#     define retroflat_viewport_world_tile_x() \
         retroflat_viewport_world_tile_x_generic()
#     define retroflat_viewport_world_tile_y() \
         retroflat_viewport_world_tile_y_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current width of the world in pixels.
 */
#  define retroflat_viewport_world_w() \
   retroflat_viewport_world_w_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current height of the world in pixels.
 */
#  define retroflat_viewport_world_h() \
   retroflat_viewport_world_h_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current width of the world in tiles.
 */
#  define retroflat_viewport_screen_tile_w() \
   retroflat_viewport_screen_tile_w_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current height of the world in tiles.
 */
#  define retroflat_viewport_screen_tile_h() \
   retroflat_viewport_screen_tile_h_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the width of the viewport in pixels.
 */
#  define retroflat_viewport_screen_w() \
   retroflat_viewport_screen_w_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the height of the viewport in pixels.
 */
#  define retroflat_viewport_screen_h() \
   retroflat_viewport_screen_h_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the difference in pixels between the viewport X + width and the
 *        screen width.
 */
#  define retroflat_viewport_screen_w_remainder() \
   retroflat_viewport_screen_w_remainder_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the difference in pixels between the viewport Y + height and
 *        the screen height.
 */
#  define retroflat_viewport_screen_h_remainder() \
   retroflat_viewport_screen_h_remainder_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Set the pixel width and height of the world so the viewport knows
 *        how far it may scroll.
 * \param w The width of the world in pixels (tile_width * map_tile_width).
 * \param h The height of the world in pixels (tile_height * map_tile_height).
 */
#  define retroflat_viewport_set_world( w, h ) \
   retroflat_viewport_set_world_generic( w, h )

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Set the position of the viewport in the world in pixels.
 */
#  define retroflat_viewport_set_world_pos( x, y ) \
   retroflat_viewport_set_world_pos_generic( x, y )

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Set the pixel width and height of the viewport, as well as some other
 *        dependent values frequently used in screen updates.
 */
#  define retroflat_viewport_set_pos_size( x_px, y_px, w_px, h_px ) \
   retroflat_viewport_set_pos_size_generic( x_px, y_px, w_px, h_px )

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Lock access to RETROFLAT_VIEWPORT::refresh_grid in memory.
 *
 * This should be called before making references to the refresh grid e.g. with
 * retroflat_viewport_tile_is_stale().
 */
#  define retroflat_viewport_lock_refresh() \
   retroflat_viewport_lock_refresh_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Unlock access to RETROFLAT_VIEWPORT::refresh_grid in memory.
 *
 * This should be called when references to the refresh grid are no longer in
 * use and may be invalidated by the system.
 */
#  define retroflat_viewport_unlock_refresh() \
   retroflat_viewport_unlock_refresh_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Set the tile at the given *screen* pixel coordinates to the given
 *        tile ID.
 *
 * If these coordinates are from the world, they should subtract
 * retroflat_viewport_screen_x()/retroflat_viewport_screen_y() first!
 *
 * When the viewport is redrawn, e.g. with retrotile_topdown_draw(), it will
 * not redraw this tile if the tile ID is still the same.
 */
#  define retroflat_viewport_set_refresh( x, y, tid ) \
   retroflat_viewport_set_refresh_generic( x, y, tid )

#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

/**
 * \brief Move the viewport in a direction or combination thereof so that
 *        it's focusing the given x1/y1 within the given range.
 * \param x1 The X coordinate to focus on.
 * \param y1 The Y coordinate to focus on.
 * \param range The number of pixels from the center of the screen to keep
 *        the given X and Y inside.
 * \param speed The increment by which to move the viewport if the given X and
 *        Y are *not* in focus.
 * \warning The speed parameter should always divide evenly into the tile size,
 *          or problems may occur!
 */
#  define retroflat_viewport_focus( x1, y1, range, speed ) \
   retroflat_viewport_focus_generic( x1, y1, range, speed )

/**
 * \brief Return the screenspace X coordinate at which something at the given
 *        world coordinate should be drawn.
 */
#  define retroflat_viewport_screen_x( world_x ) \
   retroflat_viewport_screen_x_generic( world_x )

/**
 * \brief Return the screenspace Y coordinate at which something at the given
 *        world coordinate should be drawn.
 */
#  define retroflat_viewport_screen_y( world_y ) \
   retroflat_viewport_screen_y_generic( world_y )

#  define retroflat_viewport_screen_get_x() \
   retroflat_viewport_screen_get_x_generic()

#  define retroflat_viewport_screen_get_y() \
   retroflat_viewport_screen_get_y_generic()

#  ifndef RETROFLAT_VIEWPORT_OVERRIDE_MOVE
#     define retroflat_viewport_move_x( x ) \
         retroflat_viewport_move_x_generic( x )

#     define retroflat_viewport_move_y( y ) \
         retroflat_viewport_move_y_generic( y )
#  endif /* !RETROFLAT_VIEWPORT_OVERRIDE_MOVE */

#endif /* RETROFLAT_SOFT_VIEWPORT || DOCUMENTATION */

/*! \} */ /* maug_retroflt_viewport */

#elif defined( RETROVIW_C )

MERROR_RETVAL retroflat_viewport_set_refresh_generic(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px, retroflat_tile_t tid
) {
   int tile_x = 0;
   int tile_y = 0;

   /* Add +1 tile to make off-screen "-1" tile positive. */
   tile_x = _retroflat_viewport_refresh_tile_x( x_px ) + 1;
   tile_y = _retroflat_viewport_refresh_tile_y( y_px ) + 1;

   assert( NULL != g_retroflat_state->viewport.refresh_grid );
   if(
      /* Expand the range by -1 to account for just off-screen tile. */
      0 > tile_x || 0 > tile_y ||
      g_retroflat_state->viewport.screen_tile_w + 2 <= tile_x ||
      g_retroflat_state->viewport.screen_tile_h + 2 <= tile_y
   ) {
      error_printf( "invalid viewport refresh coord: %d, %d", tile_x, tile_y );
      debug_printf( 1,
         "viewport is %dx%d tiles!",
         g_retroflat_state->viewport.screen_tile_w + 2,
         g_retroflat_state->viewport.screen_tile_h + 2 );
      return MERROR_GUI;
   }

   g_retroflat_state->viewport.refresh_grid[
      (tile_y * (g_retroflat_state->viewport.screen_tile_w + 2)) + tile_x] =
         tid;

   return MERROR_OK;
}

/* === */

uint8_t retroflat_viewport_move_x_generic( int16_t x ) {
   int16_t new_world_x = g_retroflat_state->viewport.world_x + x;

   /* Keep the viewport in the world arena. */
   if(
      0 <= new_world_x &&
      g_retroflat_state->viewport.world_w >= new_world_x +
         g_retroflat_state->viewport.screen_w
   ) {
      g_retroflat_state->viewport.world_x += x;
      g_retroflat_state->viewport.world_tile_x += x >> RETROFLAT_TILE_W_BITS;
      return 1;
   }

   return 0;
}

/* === */

uint8_t retroflat_viewport_move_y_generic( int16_t y ) {
   int16_t new_world_y = g_retroflat_state->viewport.world_y + y;

   /* Keep the viewport in the world arena. */
   if(
      0 <= new_world_y &&
      g_retroflat_state->viewport.world_h >= new_world_y +
         g_retroflat_state->viewport.screen_h
   ) {
      g_retroflat_state->viewport.world_y += y;
      g_retroflat_state->viewport.world_tile_y += y >> RETROFLAT_TILE_H_BITS;
      return 1;
   }

   return 0;
}

/* === */

uint8_t retroflat_viewport_focus_generic(
   retroflat_pxxy_t x1, retroflat_pxxy_t y1,
   retroflat_pxxy_t range, retroflat_pxxy_t speed
) {
   uint8_t moved = 0,
      new_moved = 0;
   int16_t new_pt = 0;

#  define _retroflat_viewport_focus_dir( n, xy, wh, gl, pm, dir, range, speed ) \
      new_pt = n - retroflat_viewport_world_ ## xy(); \
      if( new_pt gl (retroflat_screen_ ## wh() >> 1) pm range ) { \
         new_moved = retroflat_viewport_move_ ## xy( \
            gc_retroflat_offsets8_ ## xy[RETROFLAT_DIR8_ ## dir] * speed ); \
         if( !moved && new_moved ) { \
            moved = new_moved; \
         } \
      }

   _retroflat_viewport_focus_dir( x1, x, w, <, -, WEST, range, speed );
   _retroflat_viewport_focus_dir( x1, x, w, >, +, EAST, range, speed );
   _retroflat_viewport_focus_dir( y1, y, h, <, -, NORTH, range, speed );
   _retroflat_viewport_focus_dir( y1, y, h, >, +, SOUTH, range, speed );

   return moved;
}

#endif /* RETROVIW_C */

