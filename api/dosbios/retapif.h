
#ifndef RETPLTF_H
#define RETPLTF_H

static volatile retroflat_ms_t g_ms = 0;

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

void __interrupt __far retroflat_timer_handler() {
   static unsigned long count = 0;

   ++g_ms;
   count += RETROFLAT_DOS_TIMER_DIV; /* Original DOS timer in parallel. */
   if( 65536 <= count ) {
      /* Call the original handler. */
      count -= 65536;
      _chain_intr( g_retroflat_state->old_timer_interrupt );
   } else {
      /* Acknowledge interrupt */
      outp( 0x20, 0x20 );
   }
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

   switch( g_retroflat_state->screen_mode ) {
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

   switch( g_retroflat_state->screen_mode ) {
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
         g_retroflat_state->screen_mode );
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

