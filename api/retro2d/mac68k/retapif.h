
#ifndef RETPLTF_H
#define RETPLTF_H

static MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
   Rect r = { 0, 0, 0, 0 };
   unsigned char title_buf[128];

   InitGraf( &qd.thePort );
   InitFonts();
   InitWindows();
   InitMenus();
   InitCursor();
   TEInit();
   InitDialogs( NULL );

   retval = maug_str_c2p( args->title, (char*)title_buf, 128 );
   maug_cleanup_if_not_ok();

   /* Create the window. */
   /* TODO: Set X/Y from args? */
   SetRect( &r, 50, 50, args->screen_w, args->screen_h );
   g_retroflat_state->platform.win = NewWindow(
      nil, &r, title_buf, true, documentProc, (WindowPtr)-1, false, 0 );
   if( nil == g_retroflat_state->platform.win ) {
      retval = MERROR_GUI;
      goto cleanup;
   }

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {
   /* TODO */
   FlushEvents( everyEvent, -1 );
}

/* === */

MERROR_RETVAL retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;
   retroflat_ms_t next = 0,
      now = 0;

   /* Set these to be called from WndProc later. */
   g_retroflat_state->loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;
   g_retroflat_state->frame_iter = (retroflat_loop_iter)frame_iter;

   if(
      RETROFLAT_FLAGS_RUNNING ==
      (g_retroflat_state->retroflat_flags & RETROFLAT_FLAGS_RUNNING)
   ) {
      /* Main loop is already running, so we're just changing the iter call
       * and leaving!
       */
      debug_printf( 1, "main loop already running!" );
      goto cleanup;
   }

   g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_RUNNING;

   /* Handle Windows messages until quit. */
   do {
      SystemTask();
      GetNextEvent( everyEvent, &(g_retroflat_state->platform.event) );

      if(
         /* Not waiting for the next frame? */
         RETROFLAT_FLAGS_WAIT_FOR_FPS !=
         (RETROFLAT_FLAGS_WAIT_FOR_FPS & g_retroflat_state->retroflat_flags) &&
         /* Inter-frame loop present? */
         NULL != g_retroflat_state->loop_iter
      ) {
         /* Run the loop iter as many times as possible. */
         g_retroflat_state->loop_iter( g_retroflat_state->loop_data );
      }
      if(
         RETROFLAT_FLAGS_UNLOCK_FPS !=
         (RETROFLAT_FLAGS_UNLOCK_FPS & g_retroflat_state->retroflat_flags) &&
         retroflat_get_ms() < next
      ) {
         /* Sleep/low power for a bit. */
         Delay( 5, NULL );
         continue;
      }

      retroflat_heartbeat_update();

      if( NULL != g_retroflat_state->frame_iter ) {
         /* Run the frame iterator once per FPS tick. */
         g_retroflat_state->frame_iter( g_retroflat_state->loop_data );
      }
      /* Reset wait-for-frame flag AFTER frame callback. */
      g_retroflat_state->retroflat_flags &= ~RETROFLAT_FLAGS_WAIT_FOR_FPS;
      now = retroflat_get_ms();
      if( now + retroflat_fps_next() > now ) {
         next = now + retroflat_fps_next();
      } else {
         /* Rollover protection. */
         /* TODO: Add difference from now/next to 0 here. */
         next = 0;
      }

   } while(
      RETROFLAT_FLAGS_RUNNING ==
      (g_retroflat_state->retroflat_flags & RETROFLAT_FLAGS_RUNNING)
   );

cleanup:

   /* This should be set by retroflat_quit(). */
   return retval;
}

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   /* TODO */

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Build the title. */
   va_start( vargs, format );
   memset( title, '\0', RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

   /* TODO */

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   /* TODO */
   return TickCount();
}

/* === */

uint32_t retroflat_get_rand() {
   /* TODO */
   return Random();
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

   /* TODO: Stow old port to be retrieved on release. */
   SetPort( g_retroflat_state->platform.win );

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO: Re-set old port stowed in lock. */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, RETROFLAT_PATH_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

   /* TODO */

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   /* TODO */

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

   /* TODO */

}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != src );

   /* TODO */

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags
) {
   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
   ) {
      return;
   }

   /* TODO: Do we need this with Quickdraw? */
   retroflat_constrain_px( x, y, target, return );

   /* TODO */

}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   /* TODO */
}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

#  if defined( RETROFLAT_OPENGL )

   assert( NULL != target );

   retrosoft_line( target, color_idx, x1, y1, x2, y2, flags );

#  else

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   /* TODO */
#  pragma message( "warning: line not implemented" )

#  endif /* RETROFLAT_OPENGL */
}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {

#  if defined( RETROFLAT_OPENGL )

   assert( NULL != target );

   retrosoft_ellipse( target, color, x, y, w, h, flags );

#  elif defined( RETROFLAT_SOFT_SHAPES )

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   /* TODO */
#  pragma message( "warning: ellipse not implemented" )

#  endif /* RETROFLAT_OPENGL */
}

/* === */

void retroflat_resize_v() {
   /* Platform does not support resizing. */
}

#endif /* !RETPLTF_H */

