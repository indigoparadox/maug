
#ifndef RETPLTF_H
#define RETPLTF_H

static volatile retroflat_ms_t g_ms = 0;

size_t retroflat_allegro_screen_w() {
   return SCREEN_W;
}

size_t retroflat_allegro_screen_h() {
   return SCREEN_H;
}

size_t retroflat_allegro_bmp_w( struct RETROFLAT_BITMAP* bmp ) {
   return bmp->b->w;
}

size_t retroflat_allegro_bmp_h( struct RETROFLAT_BITMAP* bmp ) {
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
   g_retroflat_state->input.close_button = 1;
}
END_OF_FUNCTION( retroflat_on_close_button )

/* === */

MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
#     if 0
   union REGS regs;
   struct SREGS sregs;
#     endif

   /* == Allegro == */

   srand( time( NULL ) );

   if( allegro_init() ) {
      allegro_message( "could not setup allegro!" );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   install_keyboard();
#     if !defined( RETROFLAT_OS_DOS ) || !defined( __WATCOMC__ )
   /* XXX: Broken in DOS on watcom. */
   install_timer();
   install_int( retroflat_on_ms_tick, 1 );
#     endif /* RETROFLAT_OS_DOS */

   /* TODO: More fine-grained color depth handling. */
   g_retroflat_state->screen_colors = 16;

#     ifdef RETROFLAT_OS_DOS
   /* Don't try windowed mode in DOS. */
   if(
      set_gfx_mode( GFX_AUTODETECT, args->screen_w, args->screen_h, 0, 0 )
   ) {
#     else
   if( 
      /* TODO: Set window position. */
      set_gfx_mode(
         GFX_AUTODETECT_WINDOWED, args->screen_w, args->screen_h, 0, 0 )
   ) {
#     endif /* RETROFLAT_OS_DOS */

      allegro_message( "could not setup graphics!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#     define RETROFLAT_COLOR_TABLE_ALLEGRO_INIT( i, name_l, name_u, r, g, b, cgac, cgad ) \
         g_retroflat_state->palette[i] = makecol( r, g, b );

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_ALLEGRO_INIT )

   LOCK_FUNCTION( retroflat_on_close_button );
   set_close_button_callback( retroflat_on_close_button );

#     ifndef RETROFLAT_OS_DOS
   if( NULL != args->title ) {
      retroflat_set_title( args->title );
   }

   /* XXX: Broken in DOS. */
   if( 0 > install_mouse() ) {
      allegro_message( "could not setup mouse!" );
      retval = RETROFLAT_ERROR_MOUSE;
      goto cleanup;
   }
#     endif /* !RETROFLAT_OS_DOS */

#     ifdef RETROFLAT_OS_DOS
#        if 0
   regs.w.ax = 0x9;
   regs.w.bx = 0x0;
   regs.w.cx = 0x0;
   regs.x.edx = FP_OFF( g_retroflat_state->mouse_cursor );
   sregs.es = FP_SEG( g_retroflat_state->mouse_cursor );
   int386x( 0x33, &regs, &regs, &sregs );
#        endif
#     endif /* RETROFLAT_OS_DOS */

   g_retroflat_state->buffer.b = 
      create_bitmap( args->screen_w, args->screen_h );
   maug_cleanup_if_null(
      BITMAP*, g_retroflat_state->buffer.b, RETROFLAT_ERROR_GRAPHICS );

cleanup:

   return retval;

}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {
   if( RETROFLAT_ERROR_ENGINE != retval ) {
      clear_keybuf();
   }

   retroflat_destroy_bitmap( &(g_retroflat_state->buffer) );
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

   allegro_message( "%s", msg_out );

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
   char filename_path[MAUG_PATH_SZ_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, MAUG_PATH_SZ_MAX + 1, flags );
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

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Allegro not setup for hardware scrolling. */
   if(
      0 > d_x || 0 > d_y ||
      retroflat_bitmap_w( target ) + w <= d_x ||
      retroflat_bitmap_h( target ) + h <= d_y
   ) {
      goto cleanup;
   }

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
      /* TODO: Can we determine if there was an error? */
      blit( src->b, target->b, s_x, s_y, d_x, d_y, w, h );
   }

cleanup:

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

   retroflat_constrain_px( x, y, target, return );

   /* == Allegro == */

   putpixel( target->b, x, y, g_retroflat_state->palette[color_idx] );
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

   if(
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
   ) {
      return;
   }

   /* == Allegro == */

   assert( NULL != target->b );
   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      rectfill( target->b, x, y, x + w, y + h,
         g_retroflat_state->palette[color_idx] );
   } else {
      rect( target->b, x, y, x + w, y + h,
         g_retroflat_state->palette[color_idx] );
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

   if(
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
   ) {
      return;
   }

   /* == Allegro == */

   assert( NULL != target->b );
   line( target->b, x1, y1, x2, y2, color_idx );

}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color ) {
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

   /* == Allegro == */

   assert( NULL != target->b );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      /* >> 1 performs better than / 2 on lousy old DOS compilers. */
      ellipsefill(
         target->b, x + (w >> 1), y + (h >> 1), w >> 1, h >> 1, color );
   } else {
      /* >> 1 performs better than / 2 on lousy old DOS compilers. */
      ellipse( target->b, x + (w >> 1), y + (h >> 1), w >> 1, h >> 1, color );
   }

}

/* === */

void retroflat_get_palette( uint8_t idx, uint32_t* p_rgb ) {

   /* TODO */
#  pragma message( "warning: get palette not implemented" )

}

/* === */

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO */
#  pragma message( "warning: set palette not implemented" )

   return retval;
}

/* === */

void retroflat_resize_v() {
   /* TODO */
}

#endif /* !RETPLTF_H */

