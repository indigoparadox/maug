
#ifndef RETPLTF_H
#define RETPLTF_H

static void retroflat_mac_bwcolor( RETROFLAT_COLOR color_idx ) {
#ifdef RETROFLAT_API_MAC7
   if( 2 < g_retroflat_state->screen_colors ) {
      RGBForeColor( &(g_retroflat_state->palette[color_idx]) );
   } else
#endif /* RETROFLAT_API_MAC7 */
   if( RETROFLAT_COLOR_BLACK == color_idx ) {
      PenPat( &(qd.black) );
   } else if( RETROFLAT_COLOR_GRAY == color_idx ) {
      PenPat( &(qd.gray) );
   } else {
#if RETRO2D_TRACE_LVL > 0
      if( RETROFLAT_COLOR_WHITE != color_idx ) {
         debug_printf(
            RETRO2D_TRACE_LVL, "alert! high color used: %d", color_idx );
      }
#endif /* RETRO2D_TRACE_LVL */
      PenPat( &(qd.white) );
   }
}

/* === */

static MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
   Rect r = { 0, 0, 0, 0 };
   unsigned char title_buf[128];
#ifdef RETROFLAT_API_MAC7
   long cqd_result = 0;
#endif /* RETROFLAT_API_MAC7 */

#ifndef RETROFLAT_API_CARBON
   InitGraf( &qd.thePort );
   InitFonts();
   InitWindows();
   InitMenus();
   TEInit();
   InitDialogs( nil );
#endif /* !RETROFLAT_API_CARBON */
   InitCursor();

   retval = maug_str_c2p( args->title, (char*)title_buf, 128 );
   maug_cleanup_if_not_ok_msg( "title string too long!" );

#ifdef RETROFLAT_API_MAC7
   debug_printf( RETRO2D_TRACE_LVL, "setting up quickdraw color..." );

   /* Detect if Color QuickDraw is present. */
   if(
      NGetTrapAddress( _Gestalt, ToolTrap ) !=
         NGetTrapAddress( _Unimplemented, ToolTrap ) &&
      noErr == Gestalt( gestaltQuickdrawVersion, &cqd_result ) &&
      0x0101 <= cqd_result
   ) {
      debug_printf(
         RETRO2D_TRACE_LVL, "color quickdraw found! using 16 colors..." );
      g_retroflat_state->screen_colors = 16;
   } else {
#endif /* RETROFLAT_API_MAC7 */
      g_retroflat_state->screen_colors = 2;
#ifdef RETROFLAT_API_MAC7
   }
#endif /* RETROFLAT_API_MAC7 */

   debug_printf( RETRO2D_TRACE_LVL, "creating the window..." );

   /* Create the window. */
   /* TODO: Set X/Y from args? */
   SetRect( &r, 50, 50, 50 + args->screen_w, 50 + args->screen_h );
#ifdef RETROFLAT_API_MAC7
   if( 2 < g_retroflat_state->screen_colors ) {
      g_retroflat_state->platform.win = NewCWindow(
         nil, &r, title_buf, true, documentProc, (WindowPtr)-1, false, 0 );
   } else {
#endif /* RETROFLAT_API_MAC7 */
      g_retroflat_state->platform.win = NewWindow(
         nil, &r, title_buf, true, documentProc, (WindowPtr)-1, false, 0 );
#ifdef RETROFLAT_API_MAC7
   }
#endif /* RETROFLAT_API_MAC7 */
   if( nil == g_retroflat_state->platform.win ) {
      error_printf( "unable to create window!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   debug_printf( RETRO2D_TRACE_LVL, "created window!" );

#ifdef RETROFLAT_API_MAC7
   if( 2 < g_retroflat_state->screen_colors ) {
#     define RETROFLAT_COLOR_TABLE_CQD( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
         g_retroflat_state->palette[idx].red = (rd << 8) | (rd); \
         g_retroflat_state->palette[idx].green = (gd << 8) | (gd); \
         g_retroflat_state->palette[idx].blue = (bd << 8 ) | (bd); \
         debug_printf( RETRO2D_TRACE_LVL, \
            "init color " #name_l ": %d, %d, %d", \
            g_retroflat_state->palette[idx].red, \
            g_retroflat_state->palette[idx].green, \
            g_retroflat_state->palette[idx].blue );
      RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CQD )
   }
#endif /* RETROFLAT_COLOR_TABLE_CQD */

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

   maug_mzero( msg_out, RETROFLAT_MSG_MAX + 1 );
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
   maug_mzero( title, RETROFLAT_TITLE_MAX + 1 );
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

MERROR_RETVAL retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = RETROFLAT_OK;

   if(
      g_retroflat_state->platform.port_stack_ct + 1 <
      RETROFLAT_M68K_PORT_STACK_MAX_CT
   ) {
      /* Stow old port to be retrieved on release. */
#ifdef RETROFLAT_API_MAC7
      if( 2 < g_retroflat_state->screen_colors ) {
         GetGWorld(
            &(g_retroflat_state->platform.gworld_stack[
               g_retroflat_state->platform.port_stack_ct]),
            &(g_retroflat_state->platform.gdhandle_stack[
               g_retroflat_state->platform.port_stack_ct]) );
      } else {
#endif /* RETROFLAT_API_MAC7 */
         GetPort( &(g_retroflat_state->platform.port_stack[
            g_retroflat_state->platform.port_stack_ct]) );
#ifdef RETROFLAT_API_MAC7
      }
#endif /* RETROFLAT_API_MAC7 */
      g_retroflat_state->platform.port_stack_ct++;
   } else {
      retval = MERROR_OVERFLOW;
      error_printf( "unable to lock graphics port: port stack overflow!" );
      retroflat_message( 1, /* RETROFLAT_MSG_FLAG_ERROR */
         "Error", "unable to lock graphics port: port stack overflow!" );
      goto cleanup;
   }

   if( NULL == bmp || retroflat_screen_buffer() == bmp ) {
#ifdef RETROFLAT_API_MAC7
      if( 2 < g_retroflat_state->screen_colors ) {
         SetGWorld( GetWindowPort( g_retroflat_state->platform.win ), nil );
      } else {
#endif /* RETROFLAT_API_MAC7 */
         SetPort( g_retroflat_state->platform.win );
#ifdef RETROFLAT_API_MAC7
      }
#endif /* RETROFLAT_API_MAC7 */
   } else {
#ifdef RETROFLAT_API_MAC7
      if( 2 < g_retroflat_state->screen_colors ) {
         SetGWorld( bmp->gworld, nil );
      } else {
#endif /* RETROFLAT_API_MAC7 */
         HLock( bmp->bits_h );
         bmp->bitmap.baseAddr = *(bmp->bits_h);
         OpenPort( &(bmp->port) );
         SetPortBits( &(bmp->bitmap) );
         SetOrigin( 0, 0 );
#ifdef RETROFLAT_API_MAC7
      }
#endif /* RETROFLAT_API_MAC7 */
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   if( 0 < g_retroflat_state->platform.port_stack_ct ) {
      g_retroflat_state->platform.port_stack_ct--;
#ifdef RETROFLAT_API_MAC7
      if( 2 < g_retroflat_state->screen_colors ) {
         SetGWorld(
            g_retroflat_state->platform.gworld_stack[
               g_retroflat_state->platform.port_stack_ct],
            g_retroflat_state->platform.gdhandle_stack[
               g_retroflat_state->platform.port_stack_ct] );
      } else {
#endif /* RETROFLAT_API_MAC7 */
         /* Re-set old port stowed in lock. */
         SetPort( g_retroflat_state->platform.port_stack[
            g_retroflat_state->platform.port_stack_ct] );
#ifdef RETROFLAT_API_MAC7
      }
#endif /* RETROFLAT_API_MAC7 */

   } else {
      retval = MERROR_OVERFLOW;
      retroflat_message( 1, /* RETROFLAT_MSG_FLAG_ERROR */
         "Error", "unable to lock graphics port: port stack underflow!" );
      error_printf( "unable to release graphics port: port stack underflow!" );
      goto cleanup;
   }

   if(
      2 >= g_retroflat_state->screen_colors &&
      NULL != bmp &&
      retroflat_screen_buffer() != bmp 
   ) {
      bmp->bitmap.baseAddr = nil;
      ClosePort( &(bmp->port) );
      HUnlock( bmp->bits_h );
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap_px_cb(
   void* data, uint8_t px, int32_t x, int32_t y,
   void* header_info, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_BITMAP* b = (struct RETROFLAT_BITMAP*)data;

   /* Blit pixels based on input bitmap. Convert mfmt's 8-bit bitmap results
    * into a true 1-bit Quickdraw bitmap.
    */
   if( 0 == px ) {
      retroflat_px( b, RETROFLAT_COLOR_BLACK, x, y, 0 );
   } else {
      retroflat_px( b, RETROFLAT_COLOR_WHITE, x, y, 0 );
   }

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   int16_t width_bytes = 0;
#ifdef RETROFLAT_API_MAC7
   Rect bounds;
   QDErr err;
#endif /* RETROFLAT_API_MAC7 */

   debug_printf( RETRO2D_TRACE_LVL,
      "creating bitmap of " SIZE_T_FMT "x" SIZE_T_FMT " with " SIZE_T_FMT
         " colors...",
      w, h, g_retroflat_state->screen_colors );

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

#ifdef RETROFLAT_API_MAC7
   if( 2 < g_retroflat_state->screen_colors ) {
      SetRect( &bounds, 0, 0, w, h );
      err = NewGWorld(
         &(bmp_out->gworld), 4, &bounds, nil, nil, 0 );
      if( noErr != err ) {
         error_printf( "error setting up gworld: %d", err );
         retval = MERROR_GUI;
         goto cleanup;
      }
   } else {
#endif /* RETROFLAT_API_MAC7 */
      /* Get the bytes per row, rounding up to an even number of bytes. */
      width_bytes = ((w + 15) / 16) * 2;

      /* Allocate the memory for the bitmap bits. */
      bmp_out->bits_h = NewHandleClear( width_bytes * h );
      maug_cleanup_if_null_alloc( Handle, bmp_out->bits_h );

      /* Setup the bitmap bounding region. */
      SetRect( &(bmp_out->bitmap.bounds), 0, 0, w, h );
      bmp_out->bitmap.rowBytes = width_bytes;
#ifdef RETROFLAT_API_MAC7
   }
#endif /* RETROFLAT_API_MAC7 */

   debug_printf( RETRO2D_TRACE_LVL, "bitmap created successfully!" );

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

