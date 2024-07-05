
#ifndef RETPLTF_H
#define RETPLTF_H

static volatile retroflat_ms_t g_ms = 0;

int retroflat_allegro_screen_w() {
   return SCREEN_W;
}

int retroflat_allegro_screen_h() {
   return SCREEN_H;
}

int retroflat_allegro_bmp_w( struct RETROFLAT_BITMAP* bmp ) {
   return bmp->b->w;
}

int retroflat_allegro_bmp_h( struct RETROFLAT_BITMAP* bmp ) {
   return bmp->b->w;
}

/* Allegro-specific callbacks for init, below. */

void retroflat_on_ms_tick() {
   if( NULL == g_retroflat_state ) {
      /* debug_printf( 1, "no state!" ); */
   } else {
      g_ms++;
   }
}

void retroflat_on_close_button() {
   g_retroflat_state->platform.close_button = 1;
}
END_OF_FUNCTION( retroflat_on_close_button )

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   allegro_message( "%s", msg_out );

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

   set_window_title( title );

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   return g_ms;
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

   /* == Allegro == */

   if( NULL != bmp ) {
      /* Normal bitmaps don't need to be locked in allegro. */
      goto cleanup;
   }

   /* Lock screen for drawing if bmp is NULL. */

#     ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
   show_mouse( NULL ); /* Disable mouse before drawing. */
#     endif
   acquire_screen();

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* == Allegro == */

   if( NULL != bmp ) {
      /* Don't need to lock bitmaps in Allegro. */
      goto cleanup;
   }

   /* Flip the buffer. */
   blit( g_retroflat_state->buffer.b, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

   /* Release the screen. */
   release_screen();
#     ifdef RETROFLAT_MOUSE
      /* XXX: Broken in DOS. */
   show_mouse( screen ); /* Enable mouse after drawing. */
#     endif /* RETROFLAT_MOUSE */
   vsync();

cleanup:

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

   bmp_out->b = load_bitmap( filename_path, NULL );
   if( NULL == bmp_out->b ) {
      allegro_message( "unable to load %s", filename_path );
      retval = RETROFLAT_ERROR_BITMAP;
   }

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

   bmp_out->b = create_bitmap( w, h );
   maug_cleanup_if_null( BITMAP*, bmp_out->b, RETROFLAT_ERROR_BITMAP );
   clear_bitmap( bmp_out->b );

cleanup:

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

   if( NULL == bmp->b ) {
      return;
   }

   destroy_bitmap( bmp->b );
   bmp->b = NULL;
}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int16_t w, int16_t h
) {

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   assert( NULL != src );
   assert( NULL != target->b );
   assert( NULL != src->b );

   if(
      0 == s_x && 0 == s_y &&
      ((-1 == w && -1 == h ) || (src->b->w == w && src->b->h == h))
   ) {
      draw_sprite( target->b, src->b, d_x, d_y );
   } else {
      /* Handle partial blit. */
      blit( src->b, target->b, s_x, s_y, d_x, d_y, w, h );
   }

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

   retroflat_constrain_px( x, y, target, return );

   /* == Allegro == */

   putpixel( target->b, x, y, g_retroflat_state->palette[color_idx] );
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
#  if defined( RETROFLAT_OS_DOS ) || defined( RETROFLAT_OS_DOS_REAL )
   union REGS inregs;
   union REGS outregs;
#  endif /* RETROFLAT_API_ALLEGRO && RETROFLAT_OS_DOS */
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   if( g_retroflat_state->platform.close_button ) {
      retroflat_quit( 0 );
      return 0;
   }

#     if defined( __WATCOMC__ ) && defined( RETROFLAT_OS_DOS )
   /* Allegro mouse is broken under watcom in DOS. */

   /* Poll the mouse. */
   inregs.w.ax = 3;
   int386( 0x33, &inregs, &outregs );

   if(
      1 == outregs.x.ebx && /* Left button clicked. */
      outregs.w.cx != g_retroflat_state->platform.last_mouse_x &&
      outregs.w.dx != g_retroflat_state->platform.last_mouse_y
   ) { 
      input->mouse_x = outregs.w.cx;
      input->mouse_y = outregs.w.dx;

      /* Prevent repeated clicks. */
      g_retroflat_state->platform.last_mouse_x = input->mouse_x;
      g_retroflat_state->platform.last_mouse_y = input->mouse_y;

      return RETROFLAT_MOUSE_B_LEFT;
   } else {
      g_retroflat_state->platform.last_mouse_x = outregs.w.cx;
      g_retroflat_state->platform.last_mouse_y = outregs.w.dx;
   }

#     else
   poll_mouse();
   if( mouse_b & 0x01 ) {
      input->mouse_x = mouse_x;
      input->mouse_y = mouse_y;
      return RETROFLAT_MOUSE_B_LEFT;
   } else if( mouse_b & 0x02 ) {
      input->mouse_x = mouse_x;
      input->mouse_y = mouse_y;
      return RETROFLAT_MOUSE_B_RIGHT;
   }
#     endif /* RETROFLAT_OS_DOS */

   poll_keyboard();
   if( keypressed() ) {
      /* TODO: ??? */
      if( KB_SHIFT_FLAG == (KB_SHIFT_FLAG & key_shifts) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_SHIFT;
      }

      if( KB_CTRL_FLAG == (KB_CTRL_FLAG & key_shifts) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_CTRL;
      }

      if( KB_ALT_FLAG == (KB_ALT_FLAG & key_shifts) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_ALT;
      }

      return (readkey() >> 8);
   }

   return key_out;
}

#endif /* !RETPLTF_H */

