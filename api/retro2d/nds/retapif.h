
#ifndef RETPLTF_H
#define RETPLTF_H

#  ifdef USE_NDSASSET
#     include <ndsasset.h>
#  endif /* USE_NDSASSET */

#  ifndef NDSASSET_H
const char* gc_ndsassets_names[] = { NULL };
const unsigned int* gc_ndsassets_tiles[] = {};
const unsigned short* gc_ndsassets_pals[] = {};
const size_t gc_ndsassets_tiles_lens[] = {};
const size_t gc_ndsassets_pals_lens[] = {};
const size_t gc_ndsassets_tiles_widths[] = {};
const size_t gc_ndsassets_tiles_heights[] = {};
#  endif /* !NDSASSET_H */

static const unsigned short _gc_retroflat_nds_pal_default[256]
__attribute__((aligned(4))) = {
	0x0000,0x5400,0x02A0,0x56A0,0x0015,0x5415,0x0155,0x56B5,
	0x294A,0x7D4A,0x2BEA,0x7FEA,0x295F,0x7D5F,0x2BFF,0x7FFF,
	0x0842,0x0842,0x0842,0x0842,0x0842,0x0842,0x0842,0x0842,
	0x0C63,0x0C63,0x0C63,0x0C63,0x0C63,0x0C63,0x0C63,0x0C63,
	0x1084,0x1084,0x1084,0x1084,0x1084,0x1084,0x1084,0x1084,
	0x14A5,0x14A5,0x14A5,0x14A5,0x14A5,0x14A5,0x14A5,0x14A5,
	0x18C6,0x18C6,0x18C6,0x18C6,0x18C6,0x18C6,0x18C6,0x18C6,
	0x1CE7,0x1CE7,0x1CE7,0x1CE7,0x1CE7,0x1CE7,0x1CE7,0x1CE7,

	0x2108,0x2108,0x2108,0x2108,0x2108,0x2108,0x2108,0x2108,
	0x2529,0x2529,0x2529,0x2529,0x2529,0x2529,0x2529,0x2529,
	0x294A,0x294A,0x294A,0x294A,0x294A,0x294A,0x294A,0x294A,
	0x2D6B,0x2D6B,0x2D6B,0x2D6B,0x2D6B,0x2D6B,0x2D6B,0x2D6B,
	0x318C,0x318C,0x318C,0x318C,0x318C,0x318C,0x318C,0x318C,
	0x35AD,0x35AD,0x35AD,0x35AD,0x35AD,0x35AD,0x35AD,0x35AD,
	0x39CE,0x39CE,0x39CE,0x39CE,0x39CE,0x39CE,0x39CE,0x39CE,
	0x3DEF,0x3DEF,0x3DEF,0x3DEF,0x3DEF,0x3DEF,0x3DEF,0x3DEF,

	0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,
	0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,
	0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,
	0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,
	0x5294,0x5294,0x5294,0x5294,0x5294,0x5294,0x5294,0x5294,
	0x56B5,0x56B5,0x56B5,0x56B5,0x56B5,0x56B5,0x56B5,0x56B5,
	0x5AD6,0x5AD6,0x5AD6,0x5AD6,0x5AD6,0x5AD6,0x5AD6,0x5AD6,
	0x5EF7,0x5EF7,0x5EF7,0x5EF7,0x5EF7,0x5EF7,0x5EF7,0x5EF7,

	0x6318,0x6318,0x6318,0x6318,0x6318,0x6318,0x6318,0x6318,
	0x6739,0x6739,0x6739,0x6739,0x6739,0x6739,0x6739,0x6739,
	0x6B5A,0x6B5A,0x6B5A,0x6B5A,0x6B5A,0x6B5A,0x6B5A,0x6B5A,
	0x6F7B,0x6F7B,0x6F7B,0x6F7B,0x6F7B,0x6F7B,0x6F7B,0x6F7B,
	0x739C,0x739C,0x739C,0x739C,0x739C,0x739C,0x739C,0x739C,
	0x77BD,0x77BD,0x77BD,0x77BD,0x77BD,0x77BD,0x77BD,0x77BD,
	0x7BDE,0x7BDE,0x7BDE,0x7BDE,0x7BDE,0x7BDE,0x7BDE,0x7BDE,
	0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,
};

/* Table of MIDI pitch values roughly transcribed to frequencies in Hz...
 * These sound kind of wonky with the square wave generator, though!
 */
static const uint16_t _gc_retroflat_nds_midi_freq[128] = {
       8,     9,     9,    10,    10,    11,    12,    12,
      13,    14,    15,    15,    16,    17,    18,    19,
      21,    22,    23,    24,    26,    28,    29,    31,
      33,    35,    37,    39,    41,    44,    46,    49,
      52,    55,    58,    62,    65,    69,    73,    78,
      82,    87,    92,    98,   104,   110,   117,   123,
     131,   139,   147,   156,   165,   175,   185,   196,
     208,   220,   233,   247,   262,   277,   294,   311,
     330,   349,   370,   392,   415,   440,   466,   494,
     523,   554,   587,   622,   659,   698,   740,   784,
     831,   880,   932,   988,  1047,  1109,  1175,  1245,
    1319,  1397,  1480,  1568,  1661,  1760,  1865,  1976,
    2093,  2217,  2349,  2489,  2637,  2794,  2960,  3136,
    3322,  3520,  3729,  3951,  4186,  4435,  4699,  4978,
    5274,  5588,  5920,  5920,  6645,  7040,  7459,  7902,
    8372,  8870,  9397,  9956, 10548, 11175, 11840, 12544
};

static void _retroflat_nds_change_bg() {

   /* Update background tiles. */
   if(
      _retroflat_nds_platform_flag( CHANGE_BG ) &&
      NULL != g_retroflat_state->platform.bg_map &&
      NULL != g_retroflat_state->platform.bg_bmp
   ) {
      /* Setup bank E to receive extended palettes. */
      vramSetBankE( VRAM_E_LCD );

      debug_printf(
         RETROFLAT_PLATFORM_TRACE_LVL,
         "loading background tiles into VRAM..." );

      dmaCopy(
         g_retroflat_state->platform.bg_bmp->tiles,
         bgGetGfxPtr( g_retroflat_state->platform.bg_id ),
         g_retroflat_state->platform.bg_bmp->tiles_len );

      /* Using extended palettes as a workaround for ImageMagick palette
      * issues.
      */
      dmaCopy(
         g_retroflat_state->platform.bg_bmp->pal,
         &VRAM_E_EXT_PALETTE[0][0],
         g_retroflat_state->platform.bg_bmp->pal_len );

      g_retroflat_state->platform.flags &= ~RETROFLAT_NDS_FLAG_CHANGE_BG;

      /* Tell bank E it can use the extended palettes, now. */
      vramSetBankE( VRAM_E_BG_EXT_PALETTE );
   }

#if 0
   /* Update window tiles. */
   if(
      g_window_bmp_changed && NULL != g_window_tiles && NULL != g_window_bmp
   ) {
      dmaCopy( g_window_bmp->grit->tiles, bgGetGfxPtr( g_window_id ),
         g_window_bmp->grit->tiles_sz );

      /* Using extended palettes as a workaround for ImageMagick palette
         * issues.
         */
      dmaCopy( g_window_bmp->grit->palette, &VRAM_E_EXT_PALETTE[1][0],
         g_window_bmp->grit->palette_sz );

      g_window_bmp_changed = 0;
   }
#endif
}

/* === */

static void _retroflat_nds_blit_sprite(
   struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   int tile_idx = 0,
      tile_x = 0,
      tile_y = 0;
   const int metatile_sz = 
      (RETROFLAT_NDS_BG_TILE_W_PX * RETROFLAT_NDS_BG_TILE_H_PX) * 4;

   assert( RETROFLAT_NDS_SPRITES_ACTIVE > instance );

   /* Blitting a sprite. */

   oamSetXY( RETROFLAT_NDS_OAM_ACTIVE, instance, d_x, d_y );
   g_retroflat_state->platform.oam_dx[instance] = d_x;
   g_retroflat_state->platform.oam_dy[instance] = d_y;

   if(
      g_retroflat_state->platform.oam_entries[instance] == src &&
      s_x == g_retroflat_state->platform.oam_sx[instance] &&
      s_y == g_retroflat_state->platform.oam_sy[instance]
   ) {
      /* Bitmap already loaded, so just move it and avoid a dmaCopy(). */
      goto cleanup;
   }

   g_retroflat_state->platform.oam_entries[instance] = src;
   g_retroflat_state->platform.oam_sx[instance] = s_x;
   g_retroflat_state->platform.oam_sy[instance] = s_y;

   debug_printf(
      RETROFLAT_PLATFORM_TRACE_LVL, "loading bitmap palette into OAM..." );
   dmaCopy( src->pal, SPRITE_PALETTE, src->pal_len );

   assert( NULL != src->tiles );

   if( 0 == s_x && 0 == s_y ) {
      tile_idx = 0;
   } else {
      /* Index = y * w + x, but with each step converted from px to tiles. */
      /* TODO: src->w doesn't seem to be quite right... explosion broken. */
      tile_idx = ((s_y) * (src->w)) + (s_x);
   }
   debug_printf(
      RETROFLAT_PLATFORM_TRACE_LVL, "loading bitmap tiles into OAM..." );
   
   dmaCopy(
      ((uint8_t*)src->tiles) + tile_idx,
      g_retroflat_state->platform.sprite_frames[instance],
         metatile_sz);

   /* TODO: Clear pixel layer if it obscures sprite. */

   oamSet(
      RETROFLAT_NDS_OAM_ACTIVE, instance, d_x, d_y, 0, 0,
      SpriteSize_16x16, SpriteColorFormat_256Color, 
      g_retroflat_state->platform.sprite_frames[instance],
      -1, false, false, false, false, false );

cleanup:

   return;
}

/* === */

void _retroflat_nds_blit_tiles(
   struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h
) {
   uint16_t* bg_map = NULL;
   int tile_idx = 0,
      tile_x = 0,
      tile_y = 0;
   uint16_t row[2];

   /* The tile's physical location on the tilemap. */
   /* Divide by 8 rather than 16 since DS tiles are 8x8. */
   tile_y = d_y / RETROFLAT_NDS_BG_TILE_H_PX;
   tile_x = d_x / RETROFLAT_NDS_BG_TILE_W_PX;

   assert( tile_y >= 0 );
   assert( tile_x >= 0 );
   assert( tile_x < RETROFLAT_NDS_BG_W_TILES );

   if( g_retroflat_state->platform.bg_bmp != src ) {
      g_retroflat_state->platform.bg_bmp = src;
      g_retroflat_state->platform.flags |= RETROFLAT_NDS_FLAG_CHANGE_BG;
   }

   /* Hide window tiles if a tilemap tile was drawn more recently. */
   /*
   g_window_tiles[(tile_y * RETROFLAT_NDS_BG_W_TILES) + tile_x] = 0;
   g_window_tiles[(tile_y * RETROFLAT_NDS_BG_W_TILES) + tile_x + 1] = 0;
   g_window_tiles[((tile_y + 1) * RETROFLAT_NDS_BG_W_TILES) + tile_x] = 0;
   g_window_tiles[((tile_y + 1) * RETROFLAT_NDS_BG_W_TILES) + tile_x + 1] = 0;
   */

   /* TODO: Fill block with transparency on px layer in front. */
   /* graphics_draw_block( d_x, d_y, TILE_W, TILE_H, 0 ); */
   /*
   retroflat_rect(
      NULL, RETROFLAT_COLOR_BLACK, d_x, d_y, w, h,
      RETROFLAT_FLAGS_FILL ); */

   /* DS tiles are 8x8, so each tile is split up into 4, so compensate! */
   /* tile_idx = src->tile_offset * 4; */

   tile_idx = (
      ((s_y >> 4) * (retroflat_bitmap_w( src ) >> 4)) +
      (s_x >> 4));

   tile_idx *= 4;

   /* debug_printf( 1, "tile_idx: %d", tile_idx ); */

   bg_map = bgGetMapPtr( g_retroflat_state->platform.bg_id );

   bg_map[((tile_y + 1) * RETROFLAT_NDS_BG_W_TILES) + tile_x + 1] =
      tile_idx + 3;
   bg_map[((tile_y + 1) * RETROFLAT_NDS_BG_W_TILES) + tile_x] =
      tile_idx + 2;
   bg_map[(tile_y * RETROFLAT_NDS_BG_W_TILES) + tile_x + 1] =
      tile_idx + 1;
   bg_map[(tile_y * RETROFLAT_NDS_BG_W_TILES) + tile_x] =
      tile_idx;
}

/* === */

MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
   int i = 0;

   /* Random seed. */
   srand( time( NULL ) );

   /* Setup color constants. */
#  define RETROFLAT_COLOR_TABLE_NDS_RGBS_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->palette[idx] = ARGB16( 1, \
         120 > r ? 0 : r, \
         120 > g ? 0 : g, \
         120 > b ? 0 : b );
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_NDS_RGBS_INIT )

   /* Force screen size. */
   args->screen_w = 256;
   args->screen_h = 192;

   g_retroflat_state->buffer.w = 256;
   g_retroflat_state->buffer.h = 192;

   g_retroflat_state->screen_colors = 16;

   powerOn( POWER_ALL );

#  ifdef RETROFLAT_OPENGL

   debug_printf( RETROFLAT_PLATFORM_TRACE_LVL, "setting up GL subsystem..." );

   videoSetMode( MODE_0_3D );

   vramSetBankA( VRAM_A_TEXTURE );

   glInit();
   
   /* TODO: Setup NDS 3D engine! */

#  else

   debug_printf(
      RETROFLAT_PLATFORM_TRACE_LVL, "setting up NDS sprite engine..." );

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
   g_retroflat_state->platform.bg_id = bgInit(
      0, BgType_Text8bpp, BgSize_T_256x256,
      RETROFLAT_NDS_BG0_MAP, RETROFLAT_NDS_BG0_TILE );
   g_retroflat_state->platform.bg0_map_base = RETROFLAT_NDS_BG0_MAP;
   dmaFillWords( 0, g_retroflat_state->platform.bg_map,
      sizeof( g_retroflat_state->platform.bg_map ) );
   bgSetPriority( g_retroflat_state->platform.bg_id, 2 );

   /* Put map at base 3, and tiles at base 0. */
   g_retroflat_state->platform.window_id = bgInit(
      1, BgType_Text8bpp, BgSize_T_256x256,
      RETROFLAT_NDS_BG1_MAP, RETROFLAT_NDS_BG1_TILE );
   dmaFillWords( 0, g_retroflat_state->platform.window_tiles,
      sizeof( g_retroflat_state->platform.window_tiles ) );
   bgSetPriority( g_retroflat_state->platform.window_id, 1 );

   /* Put bitmap BG at base 1, leaving map-addressable space at base 0. */
   g_retroflat_state->platform.px_id = bgInit(
      2, BgType_Bmp16, BgSize_B16_256x256,
      RETROFLAT_NDS_BG2_MAP, RETROFLAT_NDS_BG2_TILE );
   bgSetPriority( g_retroflat_state->platform.px_id, 0 );

   bgWrapOff( 0 );

   /* Setup the sprite engines. */
	oamInit( RETROFLAT_NDS_OAM_ACTIVE, SpriteMapping_1D_128, 0 );

   /* Allocate sprite frame memory. */
   for( i = 0 ; RETROFLAT_NDS_SPRITES_ACTIVE > i ; i++ ) {
      g_retroflat_state->platform.sprite_frames[i] = oamAllocateGfx(
         RETROFLAT_NDS_OAM_ACTIVE, SpriteSize_16x16, SpriteColorFormat_256Color );
   }

#  endif /* RETROFLAT_OPENGL */

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

   if( NULL != bmp && retroflat_screen_buffer() != bmp ) {
      /* Regular bitmaps don't need locking. */
      goto cleanup;
   }

#  if defined( RETROFLAT_OPENGL )
   retval = retroglu_draw_lock( bmp );
#  else

   /* TODO */
   swiWaitForVBlank();

#  endif /* RETROFLAT_OPENGL */

cleanup:

   return retval;
}

/* === */

int retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;
   uint16_t* bg_map_inactive;

   if( NULL != bmp && retroflat_screen_buffer() != bmp ) {
      /* Regular bitmaps don't need locking. */
      goto cleanup;
   }

#  if defined( RETROFLAT_OPENGL )
   retval = retroglu_draw_release( bmp );
#  else

   /* TODO */

   _retroflat_nds_change_bg();

   /* Update sprite engines. */
   oamUpdate( RETROFLAT_NDS_OAM_ACTIVE );

#  endif /* RETROFLAT_OPENGL */

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   /* TODO: Assert bitmap isn't already initialized. */

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   while( NULL != gc_ndsassets_names[i] ) {
      if( 0 == strcmp( filename, gc_ndsassets_names[i] ) ) {
         /* RO flag below will protect from write attempts, right? */
         bmp_out->tiles = (unsigned int*)gc_ndsassets_tiles[i];
         bmp_out->pal = (unsigned short*)gc_ndsassets_pals[i];
         bmp_out->tiles_len = gc_ndsassets_tiles_lens[i];
         bmp_out->pal_len = gc_ndsassets_pals_lens[i];
         bmp_out->w = gc_ndsassets_tiles_widths[i];
         bmp_out->h = gc_ndsassets_tiles_heights[i];
         bmp_out->flags |= RETROFLAT_FLAGS_BITMAP_RO;
         debug_printf(
            RETROFLAT_PLATFORM_TRACE_LVL,
            "found bitmap \"%s\" at index: " SIZE_T_FMT,
            filename, i );
         goto cleanup;
      }

      i++;
   }

   error_printf( "could not find bitmap: %s", filename );
   retval = MERROR_FILE;

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   assert( 0 == w % 8 );
   assert( 0 == h % 8 );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   /* TODO: Is this correct? */
   bmp_out->tiles = calloc( w, h );
   maug_cleanup_if_null_alloc( unsigned int*, bmp_out->tiles );
   bmp_out->tiles_len = w * h;
   bmp_out->pal = (unsigned short*)_gc_retroflat_nds_pal_default;
   bmp_out->pal_len = 512;
   bmp_out->w = w;
   bmp_out->h = h;

   debug_printf( 1, "created %d x %d bitmap", w, h );

cleanup:

   if( NULL == bmp_out->tiles || NULL == bmp_out->pal ) {
      /* Alloc failed for one! */
      if( NULL != bmp_out->tiles ) {
         free( bmp_out->tiles );
      }
      /* if( NULL != bmp_out->pal ) {
         free( bmp_out->pal );
      } */
   }

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

   /* TODO: Deallocate tile. */

}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;
   int16_t tile_id = 0;

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   assert( NULL != src );

   /* Clip off-screen drawing. */
   if(
      retroflat_screen_w() <= d_x || 0 > d_x ||
      retroflat_screen_h() <= d_y || 0 > d_y
   ) {
      if( 0 < instance ) {
         oamClearSprite( RETROFLAT_NDS_OAM_ACTIVE, instance );
         g_retroflat_state->platform.oam_entries[instance] = NULL;
         g_retroflat_state->platform.oam_sx[instance] = -1;
         g_retroflat_state->platform.oam_sy[instance] = -1;
      }
      goto cleanup;
   }

   if( 0 < instance ) {
      _retroflat_nds_blit_sprite( src, s_x, s_y, d_x, d_y, w, h, instance );
   } else if( 0 > instance ) {
      tile_id = instance * -1;
         _retroflat_nds_blit_tiles( src, s_x, s_y, d_x, d_y, w, h );
   } else {
      /* TODO */
   }

cleanup:

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags
) {
   uint16_t* px_ptr = NULL;
   size_t metatile_idx = 0,
      subtile_idx = 0,
      px_x = 0,
      px_y = 0,
      px_idx = 0;
   uint8_t* tiles8 = (uint8_t*)(target->tiles);

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

   /*
   retroflat_constrain_px( x, y, target, return );
   */

   /* == Nintendo DS == */

   if( retroflat_screen_buffer() == target ) {
      px_ptr = bgGetGfxPtr( g_retroflat_state->platform.px_id );
      if( RETROFLAT_COLOR_BLACK == color_idx ) {
         px_ptr[(y * target->w) + x] = ARGB16( 0, 0, 0, 0 );
      } else {
         px_ptr[(y * target->w) + x] = g_retroflat_state->palette[color_idx];
      }
   } else {
      /* Get the tile index for the correct metatile. Straightforward, as
       * metatiles are laid out logically like cells in a table.
       */
      metatile_idx = (
         ((y >> 4) * (retroflat_bitmap_w( target ) >> 4)) +
         (x >> 4));

      /* Get the number of lines in the metatile that we have to go past.
       * Remember, meta tiles are:
       *
       * - Visual -  - Memory -
       * |---|---|     |---|
       * | 0>| 1v|     | 0 |
       * |---|---|  >  | 1 |
       * | 2>| 3 |     | 2 |
       * |---|---|     | 3 |
       *               |---|
       */

      px_x = x % 16;
      px_y = y % 16;

      /* TODO: Figure out how to do this more simply with math? */
      if( 8 <= px_x && 8 <= px_y ) {
         subtile_idx = 3;
      } else if( 8 <= px_y ) {
         subtile_idx = 2;
      } else if( 8 <= px_x ) {
         subtile_idx = 1;
      } else {
         subtile_idx = 0;
      }

      /* Get the index of the pixel within the subtile... again, logical. */
      px_idx = ((px_y % 8) * 8) + (px_x % 8);

      debug_printf( RETROFLAT_PLATFORM_TRACE_LVL,
         "px: metatile_idx: " SIZE_T_FMT ", px_x: " SIZE_T_FMT ", px_y: "
         SIZE_T_FMT ", subtile: " SIZE_T_FMT,
         metatile_idx, px_x, px_y, subtile_idx );

      tiles8[
         (metatile_idx * 256 /* Pixels in a metatile */) +
         (subtile_idx * 64 /* Pixels in a subtile. */) + px_idx] =
            color_idx;
   }


}

/* === */

void retroflat_get_palette( uint8_t idx, uint32_t* p_rgb ) {

#  ifdef RETROFLAT_OPENGL

   *p_rgb = 0;
   *p_rgb |= g_retroflat_state->tex_palette[idx][0] & 0xff;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][1] & 0xff) << 8;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][2] & 0xff) << 16;

#  else

   *p_rgb = g_retroflat_state->palette[idx];

#  endif /* RETROFLAT_OPENGL */

}

/* === */

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb ) {
   MERROR_RETVAL retval = MERROR_OK;
   uint32_t r = 0;
   uint32_t g = 0;
   uint32_t b = 0;

   debug_printf( RETROFLAT_PLATFORM_TRACE_LVL,
      "setting texture palette #%u to " UPRINTF_X32_FMT "...",
      idx, rgb );

#  ifdef RETROFLAT_OPENGL

   g_retroflat_state->tex_palette[idx][0] = rgb & 0xff;
   g_retroflat_state->tex_palette[idx][1] = (rgb & 0xff00) >> 8;
   g_retroflat_state->tex_palette[idx][2] = (rgb & 0xff0000) >> 16;

#  else

   b = rgb & 0xff;
   g = (rgb & 0xff00) >> 8;
   r = (rgb & 0xff0000) >> 16;
   g_retroflat_state->palette[idx] = ARGB16( 1, r, g, b );

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;
   int keys = 0;

   assert( NULL != input );

   scanKeys();
   keys = keysDown();

   input->key_flags = 0;

#  define retroflat_nds_buttons_poll( btn ) \
   if( btn == (btn & keys) ) { \
      return btn; \
   }

   retroflat_nds_buttons( retroflat_nds_buttons_poll )

   /* TODO: Touch screen taps to mouse. */

   return key_out;
}

/* === */

void retroflat_resize_v() {
   /* Platform does not support resizing. */
}

/* === */

uint8_t retroflat_viewport_move_x( int16_t x ) {
   uint8_t retval;
   retval = retroflat_viewport_move_x_generic( x );
   return retval;
}

/* === */

uint8_t retroflat_viewport_move_y( int16_t y ) {
   uint8_t retval;
   retval = retroflat_viewport_move_y_generic( y );
   return retval;
}

/* === */

uint8_t retroflat_viewport_focus(
   size_t x1, size_t y1, size_t range, size_t speed
) {
   return retroflat_viewport_focus_generic( x1, y1, range, speed );
}

/* === */

#ifndef NO_RETROSND

MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( 2 <= sizeof( MERROR_RETVAL ) );

   soundEnable();

   g_retroflat_state->sound.flags |= RETROSND_FLAG_INIT;

   return retval;
}

/* === */

void retrosnd_midi_set_sf_bank( const char* filename_in ) {
#  pragma message( "warning: set_sf_bank not implemented" )
}

/* === */

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#  pragma message( "warning: set_voice not implemented" )
}

/* === */

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#  pragma message( "warning: set_control not implemented" )
}

/* === */

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }


   g_retroflat_state->sound.channels[channel] =
      soundPlayPSG( DutyCycle_87, _gc_retroflat_nds_midi_freq[pitch],
      RETROFLAT_NDS_VOLUME, 64 );

   debug_printf( 1, "playing channel %u at freq %u on DS channel: %u",
      channel, _gc_retroflat_nds_midi_freq[pitch],
      g_retroflat_state->sound.channels[channel] );

}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   soundKill( g_retroflat_state->sound.channels[channel] );

}

/* === */

MERROR_RETVAL retrosnd_midi_play_smf( const char* filename ) {
   MERROR_RETVAL retval = MERROR_OK;

#  pragma message( "warning: midi_play_smf not implemented" )

   return retval;
}

/* === */

uint8_t retrosnd_midi_is_playing_smf() {
#  pragma message( "warning: midi_play_smf not implemented" )
   return 1;
}

/* === */

void retrosnd_shutdown() {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   soundDisable();

}

#endif /* !NO_RETROSND */

#endif /* !RETPLTF_H */

