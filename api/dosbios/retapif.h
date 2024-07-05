
#ifndef RETPLTF_H
#define RETPLTF_H

static volatile retroflat_ms_t g_ms = 0;

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

static
int retroflat_cli_rfm( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfm", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_mode = atoi( arg );
      debug_printf( 3, "choosing screen mode: %u", args->screen_mode );
   }
   return RETROFLAT_OK;
}

/* === */

static
int retroflat_cli_rfm_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == args->screen_mode ) {
      /* TODO: Autodetect best available? */
      args->screen_mode = RETROFLAT_SCREEN_MODE_VGA;
   }
   return RETROFLAT_OK;
}

/* === */

static MERROR_RETVAL retroflat_init_platform(
   struct RETROFLAT_ARGS* args
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
   g_retroflat_state->platform.screen_mode = args->screen_mode;
   switch( args->screen_mode ) {
   case RETROFLAT_SCREEN_MODE_CGA:
      debug_printf( 3, "using CGA 320x200x4 colors" );
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
      error_printf( "unsupported video mode: %d", args->screen_mode );
      retval = MERROR_GUI;
      goto cleanup;
   }

   memset( &r, 0, sizeof( r ) );
   r.h.al = args->screen_mode;
   int86( 0x10, &r, &r ); /* Call video interrupt. */

   debug_printf(
      3, "graphics initialized (mode 0x%02x)...", args->screen_mode );

   /* Initialize color table. */
#     define RETROFLAT_COLOR_TABLE_CGA_COLORS_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->platform.cga_color_table[idx] = RETROFLAT_CGA_COLOR_ ## cgac;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CGA_COLORS_INIT )

   /* Initialize dither table. */
#     define RETROFLAT_COLOR_TABLE_CGA_DITHER_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->platform.cga_dither_table[idx] = RETROFLAT_CGA_COLOR_ ## cgad;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CGA_DITHER_INIT )

cleanup:

   return retval;
}

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

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

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

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int16_t w, int16_t h
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
      break;
   }

}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   union REGS inregs;
   union REGS outregs;
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   /* TODO: Poll the mouse. */

   /* TODO: Implement RETROFLAT_MOD_SHIFT. */

   if( kbhit() ) {
      /* Poll the keyboard. */
      key_out = getch();
      debug_printf( 2, "key: 0x%02x", key_out );
      if( 0 == key_out ) {
         /* Special key was pressed that returns two scan codes. */
         key_out = getch();
         switch( key_out ) {
         case 0x48: key_out = RETROFLAT_KEY_UP; break;
         case 0x4b: key_out = RETROFLAT_KEY_LEFT; break;
         case 0x4d: key_out = RETROFLAT_KEY_RIGHT; break;
         case 0x50: key_out = RETROFLAT_KEY_DOWN; break;
         case 0x4f: key_out = RETROFLAT_KEY_HOME; break;
         case 0x47: key_out = RETROFLAT_KEY_END; break;
         case 0x51: key_out = RETROFLAT_KEY_PGDN; break;
         case 0x49: key_out = RETROFLAT_KEY_PGUP; break;
         case 0x53: key_out = RETROFLAT_KEY_DELETE; break;
         }
      }
      if(
         RETROFLAT_FLAGS_KEY_REPEAT !=
         (RETROFLAT_FLAGS_KEY_REPEAT & g_retroflat_state->retroflat_flags)
      ) {
         while( kbhit() ) {
            getch();
         }
      }
   }

   return key_out;
}

#endif /* !RETPLTF_H */

