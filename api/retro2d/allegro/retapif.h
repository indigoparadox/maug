
#ifndef RETPLTF_H
#define RETPLTF_H

static volatile retroflat_ms_t g_ms = 0;

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

#     if !defined( RETROFLAT_OS_DOS ) || !defined( __WATCOMC__ )
   /* XXX: Broken in DOS on watcom. */
   install_timer();
   install_int( retroflat_on_ms_tick, 1 );
#     endif /* RETROFLAT_OS_DOS */

   /* TODO: More fine-grained color depth handling. */
   g_retroflat_state->screen_colors = 16;

   /* Use palette mode to enable setting colors. */
   set_color_depth( 8 );

#     ifdef RETROFLAT_OS_DOS
   /* Don't try windowed mode in DOS. */
   if(
      set_gfx_mode( GFX_AUTODETECT,
         args->screen_w, args->screen_h,
         args->screen_w + (2 * RETROFLAT_TILE_W),
         args->screen_h + (2 * RETROFLAT_TILE_H) )
   ) {
#     else
   if( 
      /* TODO: Set window position. */
      set_gfx_mode(
         GFX_AUTODETECT_WINDOWED,
         args->screen_w * args->screen_scale,
         args->screen_h * args->screen_scale, 0, 0 )
   ) {
#     endif /* RETROFLAT_OS_DOS */

      allegro_message( "could not setup graphics!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#     define RETROFLAT_COLOR_TABLE_ALLEGRO_INIT( i, name_l, name_u, ri, gi, bi, cgac, cgad ) \
         g_retroflat_state->palette[i].r = ri / 4; \
         g_retroflat_state->palette[i].g = gi / 4; \
         g_retroflat_state->palette[i].b = bi / 4;

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_ALLEGRO_INIT )

   set_palette( g_retroflat_state->palette );

   LOCK_FUNCTION( retroflat_on_close_button );
   set_close_button_callback( retroflat_on_close_button );

#     ifndef RETROFLAT_OS_DOS
   if( NULL != args->title ) {
      retroflat_set_title( args->title );
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

   retroflat_screen_buffer()->b = 
      create_bitmap( args->screen_w, args->screen_h );
   maug_cleanup_if_null(
      BITMAP*, retroflat_screen_buffer()->b, RETROFLAT_ERROR_GRAPHICS );
   retroflat_screen_buffer()->flags |= RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER;

cleanup:

   return retval;

}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {
   if( RETROFLAT_ERROR_ENGINE != retval ) {
      clear_keybuf();
   }

   retroflat_destroy_bitmap( retroflat_screen_buffer() );
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

MERROR_RETVAL retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
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

   if(
      g_retroflat_state->screen_w != g_retroflat_state->screen_v_w ||
      g_retroflat_state->screen_h != g_retroflat_state->screen_v_h
   ) {
      stretch_blit(
         retroflat_screen_buffer()->b, screen,
         0, 0, g_retroflat_state->screen_v_w, g_retroflat_state->screen_v_h,
         0, 0, SCREEN_W, SCREEN_H );
   } else {
      /* Flip the buffer. */
      blit(
         retroflat_screen_buffer()->b, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
   }

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
   maug_path filename_path;
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, RETROFLAT_BITMAP_EXT,
      filename_path, MAUG_PATH_SZ_MAX, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

   bmp_out->b = load_bitmap( filename_path, NULL );
   if( NULL == bmp_out->b ) {
      error_printf( "unable to load bitmap %s", filename_path );
      retval = MERROR_GUI;
      goto cleanup;
   }

   bmp_out->flags = flags;

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   bmp_out->flags = flags;

   bmp_out->b = create_bitmap( w, h );
   maug_cleanup_if_null( BITMAP*, bmp_out->b, MERROR_GUI );
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
   retroflat_pxxy_t s_x, retroflat_pxxy_t s_y,
   retroflat_pxxy_t d_x, retroflat_pxxy_t d_y,
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Trim sprite to stay on-screen. */
   retval = _retroview_trim_px(
      target, instance, &s_x, &s_y, &d_x, &d_y, &w, &h );
   maug_cleanup_if_not_ok();

   assert( NULL != src );
   assert( NULL != target->b );
   assert( NULL != src->b );

   if(
      RETROFLAT_BITMAP_FLAG_OPAQUE !=
      (RETROFLAT_BITMAP_FLAG_OPAQUE & src->flags)
   ) {
      masked_blit( src->b, target->b, s_x, s_y, d_x, d_y, w, h );
   } else {
      blit( src->b, target->b, s_x, s_y, d_x, d_y, w, h );
   }

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

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

   retroflat_viewport_constrain_px( x, y, target, return );

   /* == Allegro == */

   putpixel( target->b, x, y, color_idx );
}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h, uint8_t flags
) {
   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

   if(
      MERROR_OK != _retroview_trim_px( target, 0, NULL, NULL, &x, &y, &w, &h )
   ) {
      return;
   }

   /* == Allegro == */

   assert( NULL != target->b );
   if( RETROFLAT_DRAW_FLAG_FILL == (RETROFLAT_DRAW_FLAG_FILL & flags) ) {
      rectfill( target->b, x, y, x + w, y + h, color_idx );
   } else {
      rect( target->b, x, y, x + w, y + h, color_idx );
   }
}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x1, retroflat_pxxy_t y1,
   retroflat_pxxy_t x2, retroflat_pxxy_t y2, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

   /* == Allegro == */

   assert( NULL != target->b );
   line( target->b, x1, y1, x2, y2, color_idx );

}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

   /* Ellipses in allegro seem to be a bit wonky and need adjustment to match
    * expected sizing from other platforms.
    */

   /*
   if( MERROR_OK != retroflat_trim_px(
      target, 0, NULL, NULL, &x, &y, &w, &h
   ) ) {
      return;
   }
   */

   w -= 1;
   h -= 1;

   /* == Allegro == */

   assert( NULL != target->b );

   if( RETROFLAT_DRAW_FLAG_FILL == (RETROFLAT_DRAW_FLAG_FILL & flags) ) {
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

   *p_rgb = 0;
   *p_rgb |= (g_retroflat_state->palette[idx].r & 0xff) * 4;
   *p_rgb |= ((g_retroflat_state->palette[idx].g & 0xff) << 8) * 4;
   *p_rgb |= ((g_retroflat_state->palette[idx].b & 0xff) << 16) * 4;

}

/* === */

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb ) {
   MERROR_RETVAL retval = MERROR_OK;

   g_retroflat_state->palette[idx].r = (rgb & 0xff) / 4;
   g_retroflat_state->palette[idx].g = ((rgb & 0xff00) >> 8) / 4;
   g_retroflat_state->palette[idx].b = ((rgb & 0xff0000) >> 16) / 4;

   set_palette( g_retroflat_state->palette );

   return retval;
}

/* === */

void retroflat_resize_v() {
   /* TODO */
}

/* === */

uint8_t retroflat_focus_platform() {
#ifdef RETROFLAT_OS_WIN
   HWND hwnd = win_get_window();
   uint8_t flags_out = 0;

   if( !IsIconic( hwnd ) ) {
      flags_out |= RETROFLAT_FOCUS_FLAG_VISIBLE;
   }

   if( GetForegroundWindow() != hwnd ) {
      flags_out |= RETROFLAT_FOCUS_FLAG_ACTIVE;
   }

   return flags_out;
#else
   /* Platform does not support focus. */
   return RETROFLAT_FOCUS_FLAG_VISIBLE | RETROFLAT_FOCUS_FLAG_ACTIVE;
#endif  /* RETROFLAT_OS_WIN | RETROFLAT_OS_UNIX */
}

/* === */

uint8_t retroview_move_x( retroflat_pxxy_t x ) {
   uint8_t move; /* Really a boolean. */

   _retroview_move_xy( x, move, x, w, RETROFLAT_TILE_W );
   if( !move ) {
      goto cleanup;
   }

   g_retroflat_state->platform.scroll_x += x;
   if(
      0 >= g_retroflat_state->platform.scroll_x ||
      RETROFLAT_TILE_W * 2 <= g_retroflat_state->platform.scroll_x
   ) {
      /* Move the viewport back to the center of the real screen buffer. */
      g_retroflat_state->platform.scroll_x = RETROFLAT_TILE_W;
   }

   scroll_screen(
      g_retroflat_state->platform.scroll_x,
      g_retroflat_state->platform.scroll_y );

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

   g_retroflat_state->platform.scroll_y += y;
   if(
      0 >= g_retroflat_state->platform.scroll_y ||
      RETROFLAT_TILE_H * 2 <= g_retroflat_state->platform.scroll_y
   ) {
      /* Move the viewport back to the center of the real screen buffer. */
      g_retroflat_state->platform.scroll_y = RETROFLAT_TILE_H;
   }

   scroll_screen(
      g_retroflat_state->platform.scroll_x,
      g_retroflat_state->platform.scroll_y );

cleanup:

   return move;
}


#endif /* !RETPLTF_H */

