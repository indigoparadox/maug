
#ifndef RETPLTF_H
#define RETPLTF_H

static MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;

   srand( time( NULL ) );

   g_retroflat_state->screen_colors = 16;

#  define RETROFLAT_COLOR_TABLE_INIT( i, name_l, name_u, ri, gi, bi, cgac, cgad ) \
      g_retroflat_state->palette[i] = (ri) | (gi << 8) | (bi << 16);

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_INIT )

   g_retroflat_state->platform.server =
      rfbGetScreen( &argc, argv, args->screen_w, args->screen_h, 8, 3, 4 );

   /* Allocate the screen buffer bitmap. */
   g_retroflat_state->platform.screen_buffer.sz =
      sizeof( struct RETROFLAT_BITMAP );
   g_retroflat_state->platform.screen_buffer.w = args->screen_w;
   g_retroflat_state->platform.screen_buffer.h = args->screen_h;
   g_retroflat_state->platform.screen_buffer.buffer =
      calloc( args->screen_w * args->screen_h, 4 );
   
   g_retroflat_state->platform.server->frameBuffer =
      (char*)g_retroflat_state->platform.screen_buffer.buffer;
   rfbInitServer( g_retroflat_state->platform.server );

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {
   /* TODO */
#  pragma message( "warning: shutdown not implemented" )
}

/* === */

MERROR_RETVAL retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Just skip to the generic loop. */
   retval = retroflat_loop_generic( frame_iter, loop_iter, data );

   /* This should be set by retroflat_quit(). */
   return retval;
}

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   maug_mzero( msg_out, RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   /* TODO */
#  pragma message( "warning: message not implemented" )

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Build the title. */
   va_start( vargs, format );
   maug_mzero( title, RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

   /* TODO */
#  pragma message( "warning: set_title not implemented" )

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms( void ) {
   struct timespec spec;
   uint32_t ms_out = 0,
      ms_launch_delta = 0;
   
   clock_gettime( CLOCK_MONOTONIC, &spec );

   /* Get the seconds since program launched. Multiply by 1000, so we want a
    * smaller number than seconds since the epoch. */
   ms_launch_delta = spec.tv_sec - g_retroflat_state->platform.s_launch;
   ms_out += ms_launch_delta * 1000;
   ms_out += spec.tv_nsec / 1000000;

   return ms_out;
}

/* === */

uint32_t retroflat_get_rand( void ) {
   return rand();
}

/* === */

MERROR_RETVAL retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = RETROFLAT_OK;

   /* TODO */
#  pragma message( "warning: draw_lock not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;
   long usec;

   if( NULL == bmp || retroflat_screen_buffer() == bmp ) {
      /* Update screen and process events. */
      usec = g_retroflat_state->platform.server->deferUpdateTime * 1000;
      rfbProcessEvents( g_retroflat_state->platform.server, usec );
   }

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap_px_cb(
   void* data, uint8_t px, int32_t x, int32_t y,
   void* header_info, uint8_t flags
) {
   struct RETROFLAT_BITMAP* b = (struct RETROFLAT_BITMAP*)data;

   if( MFMT_PX_FLAG_NEW_LINE != (MFMT_PX_FLAG_NEW_LINE & flags) ) {
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );
   bmp_out->w = w;
   bmp_out->h = h;
   bmp_out->buffer = calloc( w * h, 4 );

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

   /* TODO */
#  pragma message( "warning: destroy_bitmap not implemented" )

}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   retroflat_pxxy_t s_x, retroflat_pxxy_t s_y,
   retroflat_pxxy_t d_x, retroflat_pxxy_t d_y,
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != src );

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      RETROFLAT_BITMAP_FLAG_RO == (RETROFLAT_BITMAP_FLAG_RO & target->flags)
   ) {
      return MERROR_GUI;
   }

   /* TODO: This is only needed for a soft viewport... if the viewport can
    * scroll then use that!
    */
   /* Trim sprite to stay on-screen. */
   retval = _retroview_trim_px(
      target, instance, &s_x, &s_y, &d_x, &d_y, &w, &h );
   maug_cleanup_if_not_ok();

   /* TODO */
#  pragma message( "warning: blit_bitmap not implemented" )

cleanup:

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x, retroflat_pxxy_t y, uint8_t flags
) {
   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      RETROFLAT_BITMAP_FLAG_RO == (RETROFLAT_BITMAP_FLAG_RO & target->flags)
   ) {
      return;
   }

   retroflat_viewport_constrain_px( x, y, target, return );

   target->buffer[(y * retroflat_bitmap_w( target )) + x] =
         g_retroflat_state->palette[color_idx];

   rfbMarkRectAsModified( g_retroflat_state->platform.server, x, y, 1, 1 );
}

/* === */

void retroflat_resize_v( void ) {

#  pragma message( "warning: resize_v not implemented" )

}

/* === */

uint8_t retroflat_focus_platform( void ) {

#  pragma message( "warning: focus_platform not implemented" )

   return RETROFLAT_FOCUS_FLAG_VISIBLE | RETROFLAT_FOCUS_FLAG_ACTIVE;
}

/* === */

uint8_t retroview_move_x( retroflat_pxxy_t x ) {
   uint8_t move; /* Really a boolean. */

   _retroview_move_xy( x, move, x, w, RETROFLAT_TILE_W );
   if( !move ) {
      goto cleanup;
   }

#if 0
   /* Example of what we're expected to do here.
    *
    * viewport_rect.y is the pixel offset of the viewport inside of a
    * framebuffer that is 1 tile bigger than the viewport on all sides.
    *
    * Scoot the framebuffer over a bit until it hits the border and then shunt
    * it right back.
    *
    * The refresh grid handles which tiles need to be redrawn.
    *
    * Look up Adaptive Tile Refresh for more info.
    */
   g_retroflat_state->platform.viewport_rect.x += x;
   if(
      0 >= g_retroflat_state->platform.viewport_rect.x ||
      RETROFLAT_TILE_W * 2 <= g_retroflat_state->platform.viewport_rect.x
   ) {
      /* Move the viewport back to the center of the real screen buffer. */
      g_retroflat_state->platform.viewport_rect.x = RETROFLAT_TILE_W;
   }
#endif

cleanup:

   return move;
}

/* === */

uint8_t retroview_move_y( retroflat_pxxy_t y ) {
   uint8_t move; /* Really a boolean. */

   _retroview_move_xy( y, move, y, h, RETROFLAT_TILE_H );
   if( !move ) {
      goto cleanup;
   }

#if 0
   /* Example of what we're expected to do here.
    *
    * viewport_rect.y is the pixel offset of the viewport inside of a
    * framebuffer that is 1 tile bigger than the viewport on all sides.
    *
    * Scoot the framebuffer over a bit until it hits the border and then shunt
    * it right back.
    *
    * The refresh grid handles which tiles need to be redrawn.
    *
    * Look up Adaptive Tile Refresh for more info.
    */
   g_retroflat_state->platform.viewport_rect.y += y;
   if(
      0 >= g_retroflat_state->platform.viewport_rect.y ||
      RETROFLAT_TILE_H * 2 <= g_retroflat_state->platform.viewport_rect.y
   ) {
      /* Move the viewport back to the center of the real screen buffer. */
      g_retroflat_state->platform.viewport_rect.y = RETROFLAT_TILE_H;
   }
#endif

cleanup:

   return move;
}

#endif /* !RETPLTF_H */

