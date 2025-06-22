
#ifndef RETPLTF_H
#define RETPLTF_H

static void retroflat_mac_bwcolor( RETROFLAT_COLOR color_idx ) {
   if( RETROFLAT_COLOR_BLACK == color_idx ) {
      PenPat( &(qd.black) );
   } else if( RETROFLAT_COLOR_GRAY == color_idx ) {
      PenPat( &(qd.gray) );
   } else {
      if( RETROFLAT_COLOR_WHITE != color_idx ) {
         debug_printf( 1, "alert! high color used: %d", color_idx );
      }
      PenPat( &(qd.white) );
   }
}

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
   InitDialogs( nil );

   retval = maug_str_c2p( args->title, (char*)title_buf, 128 );
   maug_cleanup_if_not_ok_msg( "title string too long!" );

   g_retroflat_state->screen_colors = 2;

   /* Create the window. */
   /* TODO: Set X/Y from args? */
   SetRect( &r, 50, 50, 50 + args->screen_w, 50 + args->screen_h );
   g_retroflat_state->platform.win = NewWindow(
      nil, &r, title_buf, true, documentProc, (WindowPtr)-1, false, 0 );
   if( nil == g_retroflat_state->platform.win ) {
      error_printf( "unable to create window!" );
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
   EventRecord event;
#ifdef RETROFLAT_CYCLES
   int cycles_to_die = RETROFLAT_CYCLES;
#endif /* RETROFLAT_CYCLES */

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
      GetNextEvent( everyEvent, &event );

#ifdef RETROFLAT_CYCLES
      cycles_to_die--;
#endif /* RETROFLAT_CYCLES */

      /* Grab the keycode for later if applicable. */
      switch( event.what ) {
      case keyDown:
         g_retroflat_state->input.key_code = 
            (event.message & keyCodeMask) >> 8;
         break;
      }

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
         Delay( 1, nil );
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
#ifdef RETROFLAT_CYCLES
      0 < cycles_to_die &&
#endif /* RETROFLAT_CYCLES */
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
   MERROR_RETVAL retval = MERROR_OK;
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;
   DialogPtr dialog;
   int16_t item = 0;
   unsigned char msg_buf[128];
   /* TODO: Figure this out dynamically. */
   Rect r = { 100, 100, 200, 300 };

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   retval = maug_str_c2p( msg_out, (char*)msg_buf, 128 );
   maug_cleanup_if_not_ok_msg( "message string too long!" );

   /*
   dialog = NewDialog(
      nil, &r, msg_buf, TRUE, dBoxProc, (WindowPtr)-1, FALSE, 0, nil );
   ModalDialog( nil, &item );
   DisposeDialog( dialog );
   */

cleanup:

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   MERROR_RETVAL retval = MERROR_OK;
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;
   unsigned char title_buf[128];

   /* Build the title. */
   va_start( vargs, format );
   memset( title, '\0', RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

   retval = maug_str_c2p( title, (char*)title_buf, 128 );
   maug_cleanup_if_not_ok_msg( "title string too long!" );

   SetWTitle( g_retroflat_state->platform.win, title_buf );

cleanup:

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   int32_t ticks = 0;
   ticks = TickCount();
   return (ticks << 4) + (ticks >> 1) + (ticks >> 3);
}

/* === */

uint32_t retroflat_get_rand() {
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

   retroflat_mac_bwcolor( color_idx );

   PenSize( 1, 1 );

   MoveTo( x, y );
   LineTo( x + 1, y );
}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {
   Rect r;

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   retroflat_mac_bwcolor( color_idx );

   PenSize( 1, 1 );
   SetRect( &r, x, y, x + w, y + h );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      PaintRect( &r );
   } else {
      FrameRect( &r );
   }
}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   retroflat_mac_bwcolor( color_idx );

   PenSize( 1, 1 );

   MoveTo( x1, y1 );
   LineTo( x2, y2 );
}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {
   Rect r;

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   retroflat_mac_bwcolor( color_idx );

   PenSize( 1, 1 );
   SetRect( &r, x, y, x + w, y + h );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      PaintOval( &r );
   } else {
      FrameOval( &r );
   }
}

/* === */

void retroflat_resize_v() {
   /* Platform does not support resizing. */
}

#endif /* !RETPLTF_H */

