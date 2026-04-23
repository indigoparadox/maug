
#ifndef RETROVIW_H
#define RETROVIW_H

/**
 * \addtogroup maug_retroflt
 * \{
 * \addtogroup maug_retroflt_viewport RetroFlat Viewport API
 * \brief A flexible API to facilitate tile-based views using hardware
 *        acceleration where available.
 *
 * The viewport fulfills two functions:
 *
 * - It limits drawing tilemaps to a certain portion of the screen.
 *
 * - If set to full-screen, it allows for hardware-based adaptive tile
 *   scrolling.
 *
 * If no hardware acceleration is available on the platform, then the platform
 * API header should define RETROFLAT_SOFT_VIEWPORT to enable the _generic
 * functions and suffixes for this functionality.
 *
 * \warning This functionality is under heavy development and is currently
 *          buggy at best!
 * \{
 * \file retroviw.h
 */

#ifndef RETROFLAT_VIEWPORT_TRACE_LVL
#  define RETROFLAT_VIEWPORT_TRACE_LVL 0
#endif /* !RETROFLAT_VIEWPORT_TRACE_LVL */

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Flag for RETROFLAT_VIEWPORT::flags indicating that the viewport is
 *        fullscreen and hardware scrolling may be used if available.
 */
#define RETROFLAT_VIEWPORT_FLAG_FULLSCREEN 0x01

/**
 * \brief The viewport data struct. There is generally only one of these,
 *        to which all helpers implicitly refer.
 *
 * \warning Many of the fields in this struct are calculated based on each
 *          other, and so updates should only be made through
 *          retroflat_viewport_set_pos_size()!
 */
struct RETROFLAT_VIEWPORT {
   uint8_t flags;
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
    */
   retroflat_tile_t* refresh_grid;
#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */
};

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

#  define _retroflat_viewport_refresh_tile_x( x_px ) \
   (((x_px) + RETROFLAT_TILE_W) >> RETROFLAT_TILE_W_BITS)

#  define _retroflat_viewport_refresh_tile_y( y_px ) \
   (((y_px) + RETROFLAT_TILE_H) >> RETROFLAT_TILE_H_BITS)

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

MERROR_RETVAL retroflat_viewport_set_refresh_generic(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px, retroflat_tile_t tid );

uint8_t retroflat_viewport_move_x_generic( retroflat_pxxy_t x );

uint8_t retroflat_viewport_move_y_generic( retroflat_pxxy_t y );

void retroflat_viewport_set_pos_size_generic(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px,
   retroflat_pxxy_t w_px,  retroflat_pxxy_t h_px );

#  ifndef RETROFLAT_NO_VIEWPORT_REFRESH
#     define _retroflat_viewport_tile_is_stale( x_px, y_px, tile_id ) \
         ((tile_id) != \
         g_retroflat_state->viewport.refresh_grid[ \
            ((_retroflat_viewport_refresh_tile_y( y_px ) + 1) * \
               (g_retroflat_state->viewport.screen_tile_w + 2)) + \
                  (_retroflat_viewport_refresh_tile_x( x_px ) + 1)])
#  endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

#endif /* !DOCUMENTATION */

/**
 * \relates RETROFLAT_VIEWPORT
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
uint8_t retroflat_viewport_focus(
   retroflat_pxxy_t x1, retroflat_pxxy_t y1,
   retroflat_pxxy_t range, retroflat_pxxy_t speed );

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Chop w/h down to fit inside viewport or just fail if it's impossible.
 *
 * This is more expensive than retroflat_viewport_constrain_px(), as it tries
 * to shrink down rectangular regions to fit within drawable limits.
 */
MERROR_RETVAL retroflat_viewport_trim_px(
   struct RETROFLAT_BITMAP* bitmap,
   int16_t instance,
   retroflat_pxxy_t* s_x, retroflat_pxxy_t* s_y,
   retroflat_pxxy_t* d_x, retroflat_pxxy_t* d_y,
   retroflat_pxxy_t* w, retroflat_pxxy_t* h );

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Set all viewport refresh tiles as dirty, starting from 0 and moving
 *        downwards until reaching y_max.
 */
MERROR_RETVAL retroflat_viewport_clear_refresh( retroflat_pxxy_t y_max );

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Shift the viewport refresh grid 1 tile in the given direction.
 * \param shift Whether we're shifting right (positive) or left (negative).
 */
MERROR_RETVAL retroflat_viewport_shift_viewport_x( int8_t shift );

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Shift the viewport refresh grid 1 tile in the given direction.
 * \param shift Whether we're shifting down (positive) or up (negative).
 */
MERROR_RETVAL retroflat_viewport_shift_viewport_y( int8_t shift );

#ifdef RETROFLAT_VIEWPORT_TRACE_CONSTRAIN

#  define retroflat_viewport_constrain_px( x, y, bmp, retact ) \
      if( \
         x >= retroflat_bitmap_w( bmp ) || y >= retroflat_bitmap_h( bmp ) || \
         0 > x || 0 > y \
      ) { \
         error_printf( "attempted draw at %d, %d, out of bounds %d x %d", \
            x, y, retroflat_bitmap_w( bmp ), retroflat_bitmap_h( bmp ) ); \
         retact; \
      }

#else

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Ensure x and y (which must be unsigned!) are inside image boundaries.
 *
 * This also relies on the platform-specific retroflat_bitmap_w() and
 * retroflat_bitmap_h() macros being smart enough to measure the screen buffer
 * if bmp is NULL.
 *
 * \warning This WILL allow drawing outside of the viewport region, if it
 *          is smaller than the screen!
 */
#  define retroflat_viewport_constrain_px( x, y, bmp, retact ) \
      if( \
         x >= retroflat_bitmap_w( bmp ) || y >= retroflat_bitmap_h( bmp ) || \
         0 > x || 0 > y \
      ) { \
         retact; \
      }

#endif /* RETROFLAT_TRACE_CONSTRAIN */

#if defined( RETROFLAT_SOFT_VIEWPORT ) || defined( DOCUMENTATION )

#  define retroflat_viewport_world_tile_x() \
      retroflat_viewport_world_tile_x_generic()

#  define retroflat_viewport_world_tile_y() \
      retroflat_viewport_world_tile_y_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current viewport X position in the world in pixels.
 */
#  define retroflat_viewport_world_x() retroflat_viewport_world_x_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current viewport Y position in the world in pixels.
 */
#  define retroflat_viewport_world_y() retroflat_viewport_world_y_generic()

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

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the screenspace X coordinate at which something at the given
 *        world coordinate should be drawn.
 */
#  define retroflat_viewport_screen_x( world_x ) \
   retroflat_viewport_screen_x_generic( world_x )

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the screenspace Y coordinate at which something at the given
 *        world coordinate should be drawn.
 */
#  define retroflat_viewport_screen_y( world_y ) \
      retroflat_viewport_screen_y_generic( world_y )

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Get the X coordinate of the viewport on-screen in pixels.
 */
#  define retroflat_viewport_screen_get_x() \
      retroflat_viewport_screen_get_x_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Get the Y coordinate of the viewport on-screen in pixels.
 */
#  define retroflat_viewport_screen_get_y() \
      retroflat_viewport_screen_get_y_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Move the viewport horizontally relative to the world in pixels.
 * \params x Number of pixels to move the world, positive is right, negative is
 *           left.
 * \return 1 if move was successful or 0 if move was blocked (e.g. by edge
 *           of world).
 */
#  define retroflat_viewport_move_x( x ) \
      retroflat_viewport_move_x_generic( x )

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Move the viewport vertically relative to the world in pixels.
 * \params y Number of pixels to move the world, positive is down, negative is
 *           up.
 * \return 1 if move was successful or 0 if move was blocked (e.g. by edge
 *           of world).
 */
#  define retroflat_viewport_move_y( y ) \
      retroflat_viewport_move_y_generic( y )

#endif /* RETROFLAT_SOFT_VIEWPORT || DOCUMENTATION */

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Lock access to RETROFLAT_VIEWPORT::refresh_grid in memory.
 *
 * This should be called before making references to the refresh grid e.g. with
 * retroflat_viewport_tile_is_stale().
 */
#  define retroflat_viewport_lock_refresh() \
   if( NULL == g_retroflat_state->viewport.refresh_grid ) { \
      maug_mlock( \
         g_retroflat_state->viewport.refresh_grid_h, \
         g_retroflat_state->viewport.refresh_grid ); \
      maug_cleanup_if_null_lock( retroflat_tile_t*, \
         g_retroflat_state->viewport.refresh_grid ); \
   }

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Unlock access to RETROFLAT_VIEWPORT::refresh_grid in memory.
 *
 * This should be called when references to the refresh grid are no longer in
 * use and may be invalidated by the system.
 */
#  define retroflat_viewport_unlock_refresh() \
   if( NULL != g_retroflat_state->viewport.refresh_grid ) { \
      maug_munlock( \
         g_retroflat_state->viewport.refresh_grid_h, \
         g_retroflat_state->viewport.refresh_grid ); \
   }

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

/*! \} */ /* maug_retroflt_viewport */

/*! \} */ /* maug_retroflt */

#elif defined( RETROVIW_C )

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH

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
#if RETROFLAT_VIEWPORT_TRACE_LVL > 0
      error_printf( "invalid viewport refresh coord: %d, %d", tile_x, tile_y );
      debug_printf( RETROFLAT_VIEWPORT_TRACE_LVL,
         "viewport is %dx%d tiles!",
         g_retroflat_state->viewport.screen_tile_w + 2,
         g_retroflat_state->viewport.screen_tile_h + 2 );
#endif /* RETROFLAT_VIEWPORT_TRACE_LVL */
      return MERROR_GUI;
   }

   g_retroflat_state->viewport.refresh_grid[
      (tile_y * (g_retroflat_state->viewport.screen_tile_w + 2)) + tile_x] =
         tid;

   return MERROR_OK;
}

/* === */

#endif /* RETROFLAT_NO_VIEWPORT_REFRESH */

uint8_t retroflat_viewport_move_x_generic( retroflat_pxxy_t x ) {
   retroflat_pxxy_t new_world_x = g_retroflat_state->viewport.world_x + x;

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

uint8_t retroflat_viewport_move_y_generic( retroflat_pxxy_t y ) {
   retroflat_pxxy_t new_world_y = g_retroflat_state->viewport.world_y + y;

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

uint8_t retroflat_viewport_focus(
   retroflat_pxxy_t x1, retroflat_pxxy_t y1,
   retroflat_pxxy_t range, retroflat_pxxy_t speed
) {
   uint8_t moved = 0,
      new_moved = 0;
   int16_t new_pt = 0;

   /* Test if the screen is scrolling east/west. */
   new_pt = x1 - retroflat_viewport_world_x();
   if( new_pt > (retroflat_screen_w() >> 1) + range ) {
      new_moved = retroflat_viewport_move_x(
         gc_retroflat_offsets8_x[RETROFLAT_DIR8_EAST] * speed );
      if( !moved && new_moved ) {
         moved = new_moved;
      }
   } else if( new_pt < (retroflat_screen_w() >> 1) - range ) {
      new_moved = retroflat_viewport_move_x(
         gc_retroflat_offsets8_x[RETROFLAT_DIR8_WEST] * speed );
      if( !moved && new_moved ) {
         moved = new_moved;
      }
   }

   /* Test if the screen is scrolling north/south. */
   new_pt = y1 - retroflat_viewport_world_y();
   if( new_pt > (retroflat_screen_h() >> 1) + range ) {
      new_moved = retroflat_viewport_move_y(
         gc_retroflat_offsets8_y[RETROFLAT_DIR8_SOUTH] * speed );
      if( !moved && new_moved ) {
         moved = new_moved;
      }
   } else if( new_pt < (retroflat_screen_h() >> 1) - range ) {
      new_moved = retroflat_viewport_move_y(
         gc_retroflat_offsets8_y[RETROFLAT_DIR8_NORTH] * speed );
      if( !moved && new_moved ) {
         moved = new_moved;
      }
   }

   return moved;
}

/* === */

void retroflat_viewport_set_pos_size_generic(
   retroflat_pxxy_t x_px, retroflat_pxxy_t y_px,
   retroflat_pxxy_t w_px,  retroflat_pxxy_t h_px
) {
   /* Setup raw viewport dimensions. */
   g_retroflat_state->viewport.screen_x = (x_px);
   g_retroflat_state->viewport.screen_y = (y_px);
   g_retroflat_state->viewport.screen_tile_w =
      ((w_px) / RETROFLAT_TILE_W);
   g_retroflat_state->viewport.screen_tile_h =
      ((h_px) / RETROFLAT_TILE_H);

   if( w_px == retroflat_screen_w() && h_px == retroflat_screen_h() ) {
#if RETROTILE_TRACE_LVL > 0
      debug_printf( RETROFLAT_VIEWPORT_TRACE_LVL,
         "fullscreen viewport; hardware scrolling enabled!" );
#endif /* RETROTILE_TRACE_LVL */
      g_retroflat_state->viewport.flags |= RETROFLAT_VIEWPORT_FLAG_FULLSCREEN;
   } else {
      g_retroflat_state->viewport.flags &= ~RETROFLAT_VIEWPORT_FLAG_FULLSCREEN;
   }

   /* We're not adding the extra two tiles on each dimension here since this
    * won't be used for indexing or allocation but rather pixel detection.
    */
   g_retroflat_state->viewport.screen_w =
      ((w_px) / RETROFLAT_TILE_W) * RETROFLAT_TILE_W;
   g_retroflat_state->viewport.screen_h =
      ((h_px) / RETROFLAT_TILE_H) * RETROFLAT_TILE_H;
   g_retroflat_state->viewport.screen_w_remainder =
      (x_px) + (w_px) - g_retroflat_state->viewport.screen_w;
   g_retroflat_state->viewport.screen_h_remainder =
      (y_px) + (h_px) - g_retroflat_state->viewport.screen_h;
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
   retroflat_pxxy_t viewport_bottom = 0;
   retroflat_pxxy_t viewport_right = 0;
   retroflat_pxxy_t viewport_left = 0;
   retroflat_pxxy_t viewport_top = 0;

   if( 
      /* On the screen, constrain to the edges of the viewport. */
      retroflat_screen_buffer() == bitmap &&
      /* Probably blitting a window or other element that can go anywhere. */
      RETROFLAT_INSTANCE_NULL != instance &&
      /* The viewport is not fullscreen, so constrain to its tighter limits. */
      RETROFLAT_VIEWPORT_FLAG_FULLSCREEN !=
         (RETROFLAT_VIEWPORT_FLAG_FULLSCREEN &
            g_retroflat_state->viewport.flags)
   ) {
      viewport_left = retroflat_viewport_screen_get_x();
      viewport_top = retroflat_viewport_screen_get_y();
      viewport_bottom =
         (retroflat_viewport_screen_get_y() + retroflat_viewport_screen_h());
      viewport_right =
         (retroflat_viewport_screen_get_x() + retroflat_viewport_screen_w());

   } else {
      /* Constrain to the edges of the arbitrary bitmap. */
      viewport_bottom = retroflat_bitmap_h( bitmap );
      viewport_right = retroflat_bitmap_w( bitmap );
   }

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

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH

MERROR_RETVAL retroflat_viewport_clear_refresh( retroflat_pxxy_t y_max ) {
   MERROR_RETVAL retval = MERROR_OK;
   int y = 0,
      row_sz = g_retroflat_state->viewport.screen_tile_w + 2;

#if RETROTILE_TRACE_LVL > 0
   debug_printf( RETROTILE_TRACE_LVL,
      "clearing " SIZE_T_FMT " vertical viewport pixels (" SIZE_T_FMT
         " rows)...",
      y_max, y_max / RETROFLAT_TILE_H );
#endif /* RETROTILE_TRACE_LVL */

   retroflat_viewport_lock_refresh();
   for( y = 0 ; y_max > y ; y += RETROFLAT_TILE_H ) {
      memset(
         &(g_retroflat_state->viewport.refresh_grid[y * row_sz]),
         -1,
         row_sz );
   }

cleanup:

   retroflat_viewport_unlock_refresh();

   return retval;
}

/* === */

MERROR_RETVAL retroflat_viewport_shift_x( int8_t shift ) {
   MERROR_RETVAL retval = MERROR_OK;
   int y_tile;
   /* Make some of these calculations appear less gnarly. */
   int row_sz = g_retroflat_state->viewport.screen_tile_w + 2;

   if( 0 == shift ) {
      error_printf( "null shift requested!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Shift the adaptive refresh grid over accordingly, row by row. */
   retroflat_viewport_lock_refresh();
   for(
      y_tile = 0;
      (g_retroflat_state->viewport.screen_tile_h + 2) > y_tile;
      y_tile++
   ) {
      if( 0 < shift ) {
         /* Perform the (rightward) shift. */
         memmove(
            &(g_retroflat_state->viewport.refresh_grid[
               (y_tile * row_sz) + 1]),
            &(g_retroflat_state->viewport.refresh_grid[y_tile * row_sz]),
            row_sz - 1 );

         /* Mark the oncoming tile as dirty. */
         g_retroflat_state->viewport.refresh_grid[y_tile * row_sz] = -1;

      } else {
         /* Perform the (leftward) shift. */
         memmove(
            &(g_retroflat_state->viewport.refresh_grid[y_tile * row_sz]),
            &(g_retroflat_state->viewport.refresh_grid[
               (y_tile * row_sz) + 1]),
            g_retroflat_state->viewport.screen_tile_w - 1 );

         /* Mark the oncoming tile as dirty. */
         g_retroflat_state->viewport.refresh_grid[
            (y_tile * row_sz) + row_sz - 1] = -1;
      }
   }
   retroflat_viewport_unlock_refresh();

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_viewport_shift_y( int8_t shift ) {
   MERROR_RETVAL retval = MERROR_OK;
   int y_tile;
   /* Make some of these calculations appear less gnarly. */
   int row_sz = g_retroflat_state->viewport.screen_tile_w + 2;

   /* Shift the adaptive refresh grid up/down accordingly, row by row. */
   retroflat_viewport_lock_refresh();
   if( 0 < shift ) {
      for(
         /* Start on the next-to-last row. */
         y_tile = g_retroflat_state->viewport.screen_tile_h;
         0 <= y_tile;
         /* Move upwards. */
         y_tile--
      ) {
         memmove(
            &(g_retroflat_state->viewport.refresh_grid[
               (y_tile + 1) * row_sz]),
            &(g_retroflat_state->viewport.refresh_grid[
               y_tile * row_sz]),
            row_sz );

      }

      /* Mark the oncoming tiles as dirty. */
      memset(
         &(g_retroflat_state->viewport.refresh_grid[
            (g_retroflat_state->viewport.screen_tile_h + 1) * row_sz]),
         -1,
         row_sz );

   } else {
      for(
         y_tile = 1;
         g_retroflat_state->viewport.screen_tile_h + 2 > y_tile;
         /* Move downwards. */
         y_tile++
      ) {
         memmove(
            &(g_retroflat_state->viewport.refresh_grid[
               (y_tile - 1) * row_sz]),
            &(g_retroflat_state->viewport.refresh_grid[
               y_tile * row_sz]),
            row_sz );

      }

      /* Mark the oncoming tiles as dirty. */
      memset( &(g_retroflat_state->viewport.refresh_grid[0]), -1, row_sz );
   }
   retroflat_viewport_unlock_refresh();

cleanup:

   return retval;
}

#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

#endif /* RETROVIW_C */

