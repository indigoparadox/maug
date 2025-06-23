
#ifndef RETPLTF_H
#define RETPLTF_H

static volatile retroflat_ms_t g_ms = 0;

static unsigned int g_retroflat_mouse_cursor[2][16] = {
	{
		0xfff0,
		0xffe0,
		0xffc0,
		0xff81,
		0xff03,
		0x0607,
		0x000f,
		0x001f,
		0xc03f,
		0xf07f,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff
	},
	{
		0x0000,
		0x0006,
		0x000c,
		0x0018,
		0x0030,
		0x0060,
		0x70c0,
		0x1d80,
		0x0700,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000
	}
};

/* === */

void __interrupt __far retroflat_timer_handler() {
   static unsigned long count = 0;

   ++g_ms;
   count += RETROFLAT_DOS_TIMER_DIV; /* Original DOS timer in parallel. */
   if( 65536 <= count ) {
      /* Call the original handler. */
      count -= 65536;
      _chain_intr( g_retroflat_state->platform.old_timer_interrupt );
   } else {
      /* Acknowledge interrupt */
      outp( 0x20, 0x20 );
   }
}

/* === */

static void retroflat_show_mouse() {
   union REGS r;
   r.x.ax = 0x01;
   int86( 0x33, &r, &r );
}

/* === */

static void retroflat_hide_mouse() {
   union REGS r;
   r.x.ax = 0x02;
   int86( 0x33, &r, &r );
}

/* === */

static
int retroflat_cli_rfm( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfm", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->platform.screen_mode = atoi( arg );
      debug_printf( 3, "choosing screen mode: %u", args->platform.screen_mode );
   }
   return RETROFLAT_OK;
}

/* === */

static
int retroflat_cli_rfm_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == args->platform.screen_mode ) {
      /* TODO: Autodetect best available? */
      args->platform.screen_mode = RETROFLAT_SCREEN_MODE_VGA;
   }
   return RETROFLAT_OK;
}

/* === */

static MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
   union REGS r;
   struct SREGS s;

   srand( time( NULL ) );

   debug_printf( 3, "memory available before growth: %u", _memavl() );
#     ifdef MAUG_DOS_MEM_L
   /* TODO: Should this check/init be in mmem.h instead? */
   _fheapgrow();
#  else
   _nheapgrow();
#  endif /* MAUG_DOS_MEM_L */
   debug_printf( 3, "memory available after growth: %u", _memavl() );

   /* Setup timer handler. */

   _disable();

   /* Backup original handler for later. */
   segread( &s );
   r.h.al = 0x08;
   r.h.ah = 0x35;
   int86x( 0x21, &r, &r, &s );
   g_retroflat_state->platform.old_timer_interrupt =
      (retroflat_intfunc)MK_FP( s.es, r.x.bx );

   /* Install new interrupt handler. */
   g_ms = 0;
   r.h.al = 0x08;
   r.h.ah = 0x25;
   s.ds = FP_SEG( retroflat_timer_handler );
   r.x.dx = FP_OFF( retroflat_timer_handler );
   int86x( 0x21, &r, &r, &s );

   /* Set resolution of timer chip to 1ms. */
   outp( 0x43, 0x36 );
   outp( 0x40, (uint8_t)(RETROFLAT_DOS_TIMER_DIV & 0xff) );
   outp( 0x40, (uint8_t)((RETROFLAT_DOS_TIMER_DIV >> 8) & 0xff) );

   _enable();

   debug_printf( 3, "timers initialized..." );

   /* Setup graphics. */

   memset( &r, 0, sizeof( r ) );
   r.x.ax = 0x0f00; /* Service: Get video mode. */
	int86( 0x10, &r, &r ); /* Call video interrupt. */
   g_retroflat_state->platform.old_video_mode = r.h.al;
   debug_printf( 2, "saved old video mode: 0x%02x",
      g_retroflat_state->platform.old_video_mode );

   /* TODO: Put all screen mode dimensions in a LUT. */
   g_retroflat_state->platform.screen_mode = args->platform.screen_mode;
   switch( args->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_CGA:
      debug_printf( 3, "using CGA 320x200x4 colors" );
      g_retroflat_state->screen_colors = 4;
      g_retroflat_state->screen_v_w = 320;
      g_retroflat_state->screen_v_h = 200;
      g_retroflat_state->screen_w = 320;
      g_retroflat_state->screen_h = 200;
      g_retroflat_state->buffer.px = (uint8_t SEG_FAR*)0xB8000000L;
      g_retroflat_state->buffer.w = 320;
      g_retroflat_state->buffer.h = 200;
      break;

   case RETROFLAT_SCREEN_MODE_VGA:
      debug_printf( 3, "using VGA 320x200x16 colors" );
      g_retroflat_state->screen_colors = 16;
      g_retroflat_state->screen_v_w = 320;
      g_retroflat_state->screen_v_h = 200;
      g_retroflat_state->screen_w = 320;
      g_retroflat_state->screen_h = 200;
      g_retroflat_state->buffer.px = (uint8_t SEG_FAR*)0xA0000000L;
      g_retroflat_state->buffer.w = 320;
      g_retroflat_state->buffer.h = 200;
      g_retroflat_state->buffer.sz = 320 * 200;
      break;

   default:
      error_printf( "unsupported video mode: %d", args->platform.screen_mode );
      retval = MERROR_GUI;
      goto cleanup;
   }

   memset( &r, 0, sizeof( r ) );
   r.h.al = args->platform.screen_mode;
   int86( 0x10, &r, &r ); /* Call video interrupt. */

   debug_printf(
      3, "graphics initialized (mode 0x%02x)...", args->platform.screen_mode );

   /* Initialize color table. */
#     define RETROFLAT_COLOR_TABLE_CGA_COLORS_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->platform.cga_color_table[idx] = RETROFLAT_CGA_COLOR_ ## cgac;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CGA_COLORS_INIT )

   /* Initialize dither table. */
#     define RETROFLAT_COLOR_TABLE_CGA_DITHER_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->platform.cga_dither_table[idx] = RETROFLAT_CGA_COLOR_ ## cgad;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CGA_DITHER_INIT )

   /* Get mouse info. */
   r.x.ax = 0;
   int86( 0x33, &r, &r );
   debug_printf( 3, "mouse status: %d, %d", r.x.ax, r.x.bx );
   if( -1 != r.x.ax ) {
      retval = MERROR_GUI;
      error_printf( "mouse not found!" );
      goto cleanup;
   }

   /* Set mouse cursor. */
   r.x.ax = 0x9;
   r.x.bx = 0; /* Hotspot X */
   r.x.cx = 0; /* Hotspot Y */
   r.x.dx = (unsigned int)g_retroflat_mouse_cursor;
   segread( &s );
   s.es = s.ds;
   int86x( 0x33, &r, &r, &s );

   retroflat_show_mouse();

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {
   union REGS r;
   struct SREGS s;

   if( 0 != g_retroflat_state->platform.old_video_mode ) {
      /* Restore old video mode. */
      debug_printf( 3, "restoring video mode 0x%02x...",
         g_retroflat_state->platform.old_video_mode );

      memset( &r, 0, sizeof( r ) );
      r.x.ax = g_retroflat_state->platform.old_video_mode;
      int86( 0x10, &r, &r ); /* Call video interrupt. */
   }

   if( NULL != g_retroflat_state->platform.old_timer_interrupt ) {
      /* Re-install original interrupt handler. */
      debug_printf( 3, "restoring timer interrupt..." );
      _disable();
      segread( &s );
      r.h.al = 0x08;
      r.h.ah = 0x25;
      s.ds = FP_SEG( g_retroflat_state->platform.old_timer_interrupt );
      r.x.dx = FP_OFF( g_retroflat_state->platform.old_timer_interrupt );
      int86x( 0x21, &r, &r, &s );
   }

   /* Reset timer chip resolution to 18.2...ms. */
   outp( 0x43, 0x36 );
   outp( 0x40, 0x00 );
   outp( 0x40, 0x00 );

   _enable();
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

   /* TODO: Display error somehow. */
   error_printf( "%s", msg_out );

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Do nothing. */

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   return /**((uint16_t far*)0x046c) >> 4;*/ g_ms;
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

   if( NULL == bmp ) {
      retroflat_hide_mouse();
   }

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == bmp ) {
      retroflat_show_mouse();
   }

   return retval;
}

/* === */

static MERROR_RETVAL retroflat_bitmap_dos_transparency(
   struct RETROFLAT_BITMAP* bmp_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   switch( g_retroflat_state->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_VGA:

      debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
         "creating transparency mask for bitmap..." );

      /* Create a transparency mask based on palette 0. */
      bmp_out->mask = _fcalloc( bmp_out->w, bmp_out->h );
      maug_cleanup_if_null_alloc( uint8_t*, bmp_out->mask );
      /* XXX: Wrong sz field! */
      for( i = 0 ; bmp_out->sz > i ; i++ ) {
         if( RETROFLAT_TXP_PAL_IDX == bmp_out->px[i] ) {
            bmp_out->mask[i] = 0xff;
         } else {
            bmp_out->mask[i] = 0x00;
         }
      }
      break;
   }

cleanup:
   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t bmp_file;
   struct MFMT_STRUCT_BMPFILE header_bmp;
   MAUG_MHANDLE bmp_palette_h = (MAUG_MHANDLE)NULL;
   uint32_t bmp_color = 0;
   uint8_t bmp_flags = 0;
   off_t bmp_px_sz = 0;
   off_t i = 0;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, RETROFLAT_PATH_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

   /* TODO: Rework for CGA bitmaps. */

   assert( NULL == bmp_out->px );

   retval = mfile_open_read( filename_path, &bmp_file );
   maug_cleanup_if_not_ok();

   /* TODO: mfmt file detection system. */
   header_bmp.magic[0] = 'B';
   header_bmp.magic[1] = 'M';
   header_bmp.info.sz = 40;

   retval = mfmt_read_bmp_header(
      (struct MFMT_STRUCT*)&header_bmp,
      &bmp_file, 0, mfile_get_sz( &bmp_file ), &bmp_flags );
   maug_cleanup_if_not_ok();

   /* Setup bitmap options from header. */
   bmp_out->w = header_bmp.info.width;
   bmp_out->h = header_bmp.info.height;

#if 0
   retval = mfmt_read_bmp_palette( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_palette, 4 * header_bmp.info.palette_ncolors,
      &bmp_file, 54 /* TODO */, mfile_get_sz( &bmp_file ) - 54, bmp_flags );
   maug_cleanup_if_not_ok();
#endif

   bmp_out->flags = flags;

   /* Allocate a space for the bitmap pixels. */
   bmp_out->sz = header_bmp.info.width * header_bmp.info.height;
   /* We're on PC BIOS... we don't need to lock pointers in this
    * platform-specific code!
    */
   bmp_out->px = _fcalloc( header_bmp.info.height, header_bmp.info.width );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_out->px );

   retval = mfmt_read_bmp_px( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_out->px, bmp_out->sz,
      &bmp_file, header_bmp.px_offset,
      mfile_get_sz( &bmp_file ) - header_bmp.px_offset, bmp_flags );
   maug_cleanup_if_not_ok();

   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      retval = retroflat_bitmap_dos_transparency( bmp_out );
   }

cleanup:

   mfile_close( &bmp_file );

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int i = 0;
   PALETTEENTRY palette[RETROFLAT_BMP_COLORS_SZ_MAX];
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   assert( NULL == bmp_out->px );

   bmp_out->w = w;
   bmp_out->h = h;
   bmp_out->flags = flags;

   /* Allocate a space for the bitmap pixels. */
   bmp_out->sz = w * h;
   /* We're on PC BIOS... we don't need to lock pointers in this
    * platform-specific code!
    */
   bmp_out->px = _fcalloc( w, h );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_out->px );

cleanup:

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

   if( NULL != bmp->px ) {
      _ffree( bmp->px );
      bmp->px = NULL;
   }

   if( NULL != bmp->mask ) {
      _ffree( bmp->mask );
      bmp->mask = NULL;
   }

}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   int16_t y_iter = 0,
      x_iter = 0;
   uint16_t target_line_offset = 0;
   int16_t src_line_offset = 0;
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != src );

   if( NULL == target ) {
      target = &(g_retroflat_state->buffer);
   }

   /* DOS BIOS not setup for hardware scrolling. */
   /* TODO: Make exception if in EGA mode! */
   if(
      0 > d_x || 0 > d_y ||
      retroflat_bitmap_w( target ) + w <= d_x ||
      retroflat_bitmap_h( target ) + h <= d_y
   ) {
      goto cleanup;
   }

   switch( g_retroflat_state->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_VGA:
      for( y_iter = 0 ; h > y_iter ; y_iter++ ) {
         target_line_offset = ((d_y + y_iter) * (target->w)) + d_x;
         src_line_offset = ((s_y + y_iter) * src->w) + s_x;
         if( target->sz <= target_line_offset + w ) {
            continue;
         }
         /* Blit the line. */
         if(
            RETROFLAT_FLAGS_OPAQUE ==
            (RETROFLAT_FLAGS_OPAQUE & src->flags)
         ) {
            /* Copy line-by-line for speed. */
            _fmemcpy(
               &(target->px[target_line_offset]),
               &(src->px[src_line_offset]), w );
         } else {
            for( x_iter = 0 ; w > x_iter ; x_iter++ ) {
               /* AND with mask for transparency cutout. */
               target->px[target_line_offset + x_iter] &=
                  src->mask[src_line_offset + x_iter];
               /* Draw into cutout with OR. */
               target->px[target_line_offset + x_iter] |=
                  src->px[src_line_offset + x_iter];
            }
         }
      }
      break;

   default:
      error_printf( "bitmap blit unsupported in video mode: %d",
         g_retroflat_state->platform.screen_mode );
      retval = MERROR_GUI;
      break;
   }

cleanup:

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags
) {
   uint16_t screen_byte_offset = 0,
      screen_bit_offset = 0;
   uint8_t color = 0;

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

   /* == DOS PC_BIOS == */

   switch( g_retroflat_state->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_CGA:
      /* Divide y by 2 since both planes are SCREEN_H / 2 high. */
      /* Divide result by 4 since it's 2 bits per pixel. */
      screen_byte_offset = (((y >> 1) * target->w) + x) >> 2;
      /* Shift the bits over by the remainder. */
      /* TODO: Factor out this modulo to shift/and. */
      screen_bit_offset = 6 - (((((y >> 1) * target->w) + x) % 4) << 1);

      /* Dither colors on odd/even squares. */
      if( (x & 0x01 && y & 0x01) || (!(x & 0x01) && !(y & 0x01)) ) {
         color = g_retroflat_state->platform.cga_color_table[color_idx];
      } else {
         color = g_retroflat_state->platform.cga_dither_table[color_idx];
      }

      if( target != &(g_retroflat_state->buffer) ) {
         /* TODO: Memory bitmap. */

      } else if( y & 0x01 ) {
         /* 0x2000 = difference between even/odd CGA planes. */
         g_retroflat_state->buffer.px[0x2000 + screen_byte_offset] &=
            /* 0x03 = 2-bit pixel mask. */
            ~(0x03 << screen_bit_offset);
         g_retroflat_state->buffer.px[0x2000 + screen_byte_offset] |= 
            ((color & 0x03) << screen_bit_offset);
      } else {
         /* 0x03 = 2-bit pixel mask. */
         g_retroflat_state->buffer.px[screen_byte_offset] &= 
            ~(0x03 << screen_bit_offset);
         g_retroflat_state->buffer.px[screen_byte_offset] |=
            /* 0x03 = 2-bit pixel mask. */
            ((color & 0x03) << screen_bit_offset);
      }
      break;

   case RETROFLAT_SCREEN_MODE_VGA:
      screen_byte_offset = ((y * target->w) + x);
      if( target->sz <= screen_byte_offset ) {
         break;
      }
      target->px[screen_byte_offset] = color_idx;
      break;

   default:
      error_printf( "pixel blit unsupported in video mode: %d",
         g_retroflat_state->platform.screen_mode );
      break;
   }
}

/* === */

void retroflat_get_palette( uint8_t idx, uint32_t* p_rgb ) {

   /* Set VGA mask register. */
   switch( g_retroflat_state->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_VGA:
      outp( 0x3c6, 0xff );
      outp( 0x3c7, idx );
      *p_rgb = 0;
      *p_rgb |= ((uint32_t)(inp( 0x3c9 ) & 0xff) << 16);
      *p_rgb |= ((uint32_t)(inp( 0x3c9 ) & 0xff) << 8);
      *p_rgb |= (inp( 0x3c9 ) & 0xff);
      break;

   default:
      error_printf( "could not set palette index %d in screen mode %d!",
         idx, g_retroflat_state->platform.screen_mode );
      break;
   }
}

/* === */

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb ) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t byte_buffer = 0;

   debug_printf( 1,
      "setting texture palette #%u to " X32_FMT "...",
      idx, rgb );

   /* Set VGA mask register. */
   switch( g_retroflat_state->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_VGA:
      /* TODO: This doesn't seem to be working in DOSBox? */
      outp( 0x3c6, 0xff );
      outp( 0x3c8, idx );

      byte_buffer = (rgb >> 16) & 0x3f;
      debug_printf( 1, "r: %u", byte_buffer );
      outp( 0x3c9, byte_buffer );

      byte_buffer = (rgb >> 8) & 0x3f;
      debug_printf( 1, "g: %u", byte_buffer );
      outp( 0x3c9, byte_buffer );

      byte_buffer = rgb & 0x3f;
      debug_printf( 1, "b: %u", byte_buffer );
      outp( 0x3c9, byte_buffer );
      break;

   default:
      error_printf( "could not set palette index %d in screen mode %d!",
         idx, g_retroflat_state->platform.screen_mode );
      break;
   }

   return retval;
}

/* === */

void retroflat_resize_v() {
   /* Platform does not support resizing. */
}

#endif /* !RETPLTF_H */

