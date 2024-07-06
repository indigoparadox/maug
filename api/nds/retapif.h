
#ifndef RETPLTF_H
#define RETPLTF_H

MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
   int i = 0;

   /* Setup color constants. */
#  define RETROFLAT_COLOR_TABLE_NDS_RGBS_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->palette[idx] = ARGB16( 1, r, g, b );
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_NDS_RGBS_INIT )

   /* Force screen size. */
   args->screen_w = 256;
   args->screen_h = 192;

   powerOn( POWER_ALL );

#     ifdef RETROFLAT_OPENGL

   debug_printf( 3, "setting up GL subsystem..." );

   videoSetMode( MODE_0_3D );

   vramSetBankA( VRAM_A_TEXTURE );

   glInit();
   
   /* TODO: Setup NDS 3D engine! */

#     else
   videoSetMode( MODE_5_2D );
	videoSetModeSub( MODE_0_2D );

   /* Setup the upper screen for background and sprites. */
	vramSetBankA( VRAM_A_MAIN_BG );
	vramSetBankB( VRAM_B_MAIN_SPRITE );

   /* Setup the lower screen for background and sprites. */
	vramSetBankC( VRAM_C_MAIN_BG );
	vramSetBankD( VRAM_D_SUB_SPRITE );

   bgExtPaletteEnable();

   /* Setup the background engine. */

   /* Put map at base 2, but stow tiles up after the bitmap BG at base 7. */
   g_retroflat_state->platform.bg_id =
      bgInit( 0, BgType_Text8bpp, BgSize_T_256x256, 2, 7 );
   dmaFillWords( 0, g_retroflat_state->platform.bg_tiles,
      sizeof( g_retroflat_state->platform.bg_tiles ) );
   bgSetPriority( g_retroflat_state->platform.bg_id, 2 );

   /* Put map at base 3, and tiles at base 0. */
   g_retroflat_state->platform.window_id =
      bgInit( 1, BgType_Text8bpp, BgSize_T_256x256, 3, 0 );
   dmaFillWords( 0, g_retroflat_state->platform.window_tiles,
      sizeof( g_retroflat_state->platform.window_tiles ) );
   bgSetPriority( g_retroflat_state->platform.window_id, 1 );

   /* Put bitmap BG at base 1, leaving map-addressable space at base 0. */
   g_retroflat_state->platform.px_id =
      bgInit( 2, BgType_Bmp16, BgSize_B16_256x256, 1, 0 );
   bgSetPriority( g_retroflat_state->platform.px_id, 0 );

   /* Setup the sprite engines. */
	oamInit( NDS_OAM_ACTIVE, SpriteMapping_1D_128, 0 );

   /* Allocate sprite frame memory. */
   for( i = 0 ; NDS_SPRITES_ACTIVE > i ; i++ ) {
      g_retroflat_state->platform.sprite_frames[i] = oamAllocateGfx(
         NDS_OAM_ACTIVE, SpriteSize_16x16, SpriteColorFormat_256Color );
   }

#     endif /* RETROFLAT_OPENGL */

   /* Setup the timer. */
   TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024;
   TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {
   /* TODO */
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
#  ifdef RETROFLAT_API_SDL2
   uint32_t sdl_msg_flags = 0;
#  elif (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_WIN )) || \
   (defined( RETROFLAT_API_GLUT) && defined( RETROFLAT_OS_WIN )) || \
   defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   uint32_t win_msg_flags = 0;
#  endif

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   /* TODO */
   error_printf( msg_out );

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   return ((TIMER1_DATA * (1 << 16)) + TIMER0_DATA) / 32;
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

#  if defined( RETROFLAT_OPENGL )
   retval = retroglu_draw_lock( bmp );
#  else

   /* TODO */

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

int retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

#  if defined( RETROFLAT_OPENGL )
   retval = retroglu_draw_release( bmp );
#  else

   /* TODO */

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

#  ifdef RETROFLAT_OPENGL
   retval = retroglu_load_bitmap( filename_path, bmp_out, flags );
   assert( 0 < retroflat_bitmap_w( bmp_out ) );
   assert( 0 < retroflat_bitmap_h( bmp_out ) );
#  else

   /* TODO */

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

#  if defined( RETROFLAT_OPENGL )
   retval = retroglu_create_bitmap( w, h, bmp_out, flags );
   assert( 0 < retroflat_bitmap_w( bmp_out ) );
   assert( 0 < retroflat_bitmap_h( bmp_out ) );
#  else

   /* TODO */

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

#  if defined( RETROFLAT_OPENGL )
   retroglu_destroy_bitmap( bmp );
#  else

   /* TODO */

#  endif /* RETROFLAT_OPENGL */

}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int16_t w, int16_t h
) {
#  ifndef RETROFLAT_OPENGL
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#  endif /* RETROFLAT_OPENGL */

   assert( NULL != src );

#  if defined( RETROFLAT_OPENGL )
   retroglu_blit_bitmap( target, src, s_x, s_y, d_x, d_y, w, h );
#  else

   /* TODO */

#  endif /* RETROFLAT_OPENGL */
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

#  if defined( RETROFLAT_OPENGL )

   retroglu_px( target, color_idx, x, y, flags );

#  else

   /* == Nintendo DS == */

   uint16_t* px_ptr = NULL;

   px_ptr = bgGetGfxPtr( g_retroflat_state->platform.px_id );
   px_ptr[(y * 256) + x] = g_retroflat_state->palette[color_idx];

#  endif /* RETROFLAT_OPENGL */
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   /* TODO */

   return key_out;
}

/* === */

#endif /* !RETPLTF_H */

