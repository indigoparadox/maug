
#ifndef RETPLTF_H
#define RETPLTF_H

static void retroflat_mac_bwcolor( RETROFLAT_COLOR color_idx ) {
   if( RETROFLAT_COLOR_BLACK == color_idx ) {
      PenPat( &(qd.black) );
   } else if( RETROFLAT_COLOR_GRAY == color_idx ) {
      PenPat( &(qd.gray) );
   } else {
      if( RETROFLAT_COLOR_WHITE != color_idx ) {
         debug_printf(
            RETRO2D_TRACE_LVL, "alert! high color used: %d", color_idx );
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
   long cqd_result = 0;

   InitGraf( &qd.thePort );
   InitFonts();
   InitWindows();
   InitMenus();
   InitCursor();
   TEInit();
   InitDialogs( nil );

   retval = maug_str_c2p( args->title, (char*)title_buf, 128 );
   maug_cleanup_if_not_ok_msg( "title string too long!" );

   /* Detect if Color QuickDraw is present. */
   if(
      NGetTrapAddress( _Gestalt, ToolTrap ) !=
         NGetTrapAddress( _Unimplemented, ToolTrap ) &&
      noErr == Gestalt( gestaltQuickdrawVersion, &cqd_result ) &&
      0x0101 <= cqd_result
   ) {
      g_retroflat_state->screen_colors = 16;
   } else {
      g_retroflat_state->screen_colors = 2;
   }

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
      debug_printf( RETRO2D_TRACE_LVL, "main loop already running!" );
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
   /*
   DialogPtr dialog;
   Rect r;
   int16_t item = 0;
   */
   unsigned char msg_buf[128];

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   retval = maug_str_c2p( msg_out, (char*)msg_buf, 128 );
   maug_cleanup_if_not_ok_msg( "message string too long!" );

   /* TODO: Figure this out dynamically. */
   /*
   SetRect( &r, 100, 100, 300, 200 );
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

   if( NULL == bmp || retroflat_screen_buffer() == bmp ) {
      SetPort( g_retroflat_state->platform.win );
   } else {
      HLock( bmp->bits_h );
      bmp->bitmap.baseAddr = *(bmp->bits_h);
      OpenPort( &(bmp->port) );
      bmp->port.portBits = (bmp->bitmap);
      SetPort( &(bmp->port) );
      SetOrigin( 0, 0 );
   }

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO: Re-set old port stowed in lock. */

   if( NULL == bmp || retroflat_screen_buffer() == bmp ) {
      /* TODO */
      SetPort( g_retroflat_state->platform.win );
   } else {
      bmp->bitmap.baseAddr = nil;
      ClosePort( &(bmp->port) );
      HUnlock( bmp->bits_h );
   }

   return retval;
}

/* === */

static MERROR_RETVAL _retroflat_load_bitmap_px_cb(
   void* data, uint8_t px, int32_t x, int32_t y,
   void* header_info, uint8_t flags
) {
   struct RETROFLAT_BITMAP* b = (struct RETROFLAT_BITMAP*)data;

   if( MFMT_PX_FLAG_NEW_LINE == (MFMT_PX_FLAG_NEW_LINE & flags) ) {
      return MERROR_OK;
   }

   if( 0 == px ) {
      retroflat_px( b, RETROFLAT_COLOR_BLACK, x, y, 0 );
   } else {
      retroflat_px( b, RETROFLAT_COLOR_WHITE, x, y, 0 );
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;
   struct MFMT_STRUCT_BMPFILE header_bmp;
   mfile_t bmp_file;
   uint8_t bmp_flags = 0;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, RETROFLAT_PATH_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf(
      RETRO2D_TRACE_LVL, "retroflat: loading bitmap: %s", filename_path );

   /* Open the bitmap file. */
   retval = mfile_open_read( filename_path, &bmp_file );
   maug_cleanup_if_not_ok();

   /* mfmt file detection system. */
   maug_mzero( &header_bmp, sizeof( struct MFMT_STRUCT_BMPFILE ) );
   header_bmp.magic[0] = 'B';
   header_bmp.magic[1] = 'M';
   header_bmp.info.sz = 40;

   retval = mfmt_read_bmp_header(
      (struct MFMT_STRUCT*)&header_bmp,
      &bmp_file, 0, mfile_get_sz( &bmp_file ), &bmp_flags );
   maug_cleanup_if_not_ok();

   debug_printf( RETRO2D_TRACE_LVL, "retroflat: bitmap header: %ldx%ld, %d bpp",
      header_bmp.info.width, header_bmp.info.height, header_bmp.info.bpp );

   retroflat_draw_lock( &bmp_out );
   retval = mfmt_read_bmp_px_cb(
      (struct MFMT_STRUCT*)&header_bmp,
      &bmp_file,
      header_bmp.px_offset,
      mfile_get_sz( &bmp_file ) - header_bmp.px_offset,
      bmp_flags,
      _retroflat_load_bitmap_px_cb,
      bmp_out );
   retroflat_draw_release( &bmp_out );
   maug_cleanup_if_not_ok();

cleanup:

   mfile_close( &bmp_file );

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   int16_t width_bytes = 0;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   /* Get the bytes per row, rounding up to an even number of bytes. */
   width_bytes = ((w + 15) / 16) * 2;

   /* Allocate the memory for the bitmap bits. */
   bmp_out->bits_h = NewHandleClear( width_bytes * h );
   maug_cleanup_if_null_alloc( Handle, bmp_out->bits_h );

   /* Setup the bitmap bounding region. */
   SetRect( &(bmp_out->bitmap.bounds), 0, 0, w, h );
   bmp_out->bitmap.rowBytes = width_bytes;

cleanup:

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {
   DisposeHandle( bmp->bits_h );
}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;
   Rect src_r,
      dest_r;
   GrafPtr target_port;

   assert( NULL != src );

   /* Half-lock the source to copy from. */
   HLock( src->bits_h );
   src->bitmap.baseAddr = *(src->bits_h);
   OpenPort( &(src->port) );
   src->port.portBits = (src->bitmap);

   if( NULL == target || retroflat_screen_buffer() == target ) {
      target_port = g_retroflat_state->platform.win;
   } else {
      target_port = &(target->port);
   }

   /* Perform the actual blit. */
   SetRect( &src_r, s_x, s_y, s_x + w, s_y + h );
   SetRect( &dest_r, d_x, d_y, d_x + w, d_y + h );
   CopyBits(
      &(src->bitmap), &(target_port->portBits),
      &src_r, &dest_r, srcCopy, NULL );

   if( nil != src->bitmap.baseAddr ) {
      ClosePort( &(src->port) );
      src->bitmap.baseAddr = nil;
      HUnlock( src->bits_h );
   }

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

