
#ifndef RETPLTF_H
#define RETPLTF_H

/* TODO */

#ifndef RETROFLAT_OPENGL

static int retroflat_bitmap_win_transparency(
   struct RETROFLAT_BITMAP* bmp_out, int16_t w, int16_t h  
) {
   int retval = RETROFLAT_OK;
   unsigned long txp_color = 0;

   /* Setup bitmap transparency mask. */
   bmp_out->mask = CreateBitmap( w, h, 1, 1, NULL );
   maug_cleanup_if_null( HBITMAP, bmp_out->mask, RETROFLAT_ERROR_BITMAP );

   retval = retroflat_draw_lock( bmp_out );
   maug_cleanup_if_not_ok();

   /* Convert the color key into bitmap format. */
   txp_color |= (RETROFLAT_TXP_B & 0xff);
   txp_color <<= 8;
   txp_color |= (RETROFLAT_TXP_G & 0xff);
   txp_color <<= 8;
   txp_color |= (RETROFLAT_TXP_R & 0xff);
   SetBkColor( bmp_out->hdc_b, txp_color );

   /* Create the mask from the color key. */
   BitBlt(
      bmp_out->hdc_mask, 0, 0, w, h, bmp_out->hdc_b, 0, 0, SRCCOPY );
   BitBlt(
      bmp_out->hdc_b, 0, 0, w, h, bmp_out->hdc_mask, 0, 0, SRCINVERT );

cleanup:

   if( RETROFLAT_OK == retval ) {
      retroflat_draw_release( bmp_out );
   }

   return retval;
}

#endif /* !RETROFLAT_OPENGL */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;
#  if (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_WIN )) || \
   (defined( RETROFLAT_API_GLUT) && defined( RETROFLAT_OS_WIN )) || \
   defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   uint32_t win_msg_flags = 0;
#  endif

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   switch( (flags & RETROFLAT_MSG_FLAG_TYPE_MASK) ) {
   case RETROFLAT_MSG_FLAG_ERROR:
      win_msg_flags |= MB_ICONSTOP;
      break;

   case RETROFLAT_MSG_FLAG_INFO:
      win_msg_flags |= MB_ICONINFORMATION;
      break;

   case RETROFLAT_MSG_FLAG_WARNING:
      win_msg_flags |= MB_ICONEXCLAMATION;
      break;
   }

   MessageBox( retroflat_root_win(), msg_out, title, win_msg_flags );

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

   SetWindowText( g_retroflat_state->window, title );

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   return timeGetTime();
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

   /* == Win16/Win32 == */

   if( NULL == bmp ) {
#     ifdef RETROFLAT_VDP
      if( NULL != g_retroflat_state->vdp_buffer ) {
         bmp = g_retroflat_state->vdp_buffer;
      } else {
#     endif

      /* TODO: Reconcile with VDP! */
      /* The HDC should be created already by WndProc. */
      assert( (HDC)NULL != g_retroflat_state->buffer.hdc_b );
      goto cleanup;

#     ifdef RETROFLAT_VDP
      }
#     endif /* RETROFLAT_VDP */
   }

   /* Sanity check. */
   assert( (HBITMAP)NULL != bmp->b );
   assert( (HDC)NULL == bmp->hdc_b );

   /* Create HDC for source bitmap compatible with the buffer. */
   bmp->hdc_b = CreateCompatibleDC( (HDC)NULL );
   maug_cleanup_if_null( HDC, bmp->hdc_b, RETROFLAT_ERROR_BITMAP );

   if( (HBITMAP)NULL != bmp->mask ) {
      /* Create HDC for source mask compatible with the buffer. */
      bmp->hdc_mask = CreateCompatibleDC( (HDC)NULL );
      maug_cleanup_if_null( HDC, bmp->hdc_mask, RETROFLAT_ERROR_BITMAP );
   }

   /* Select bitmaps into their HDCs. */
   bmp->old_hbm_b = SelectObject( bmp->hdc_b, bmp->b );
   if( (HBITMAP)NULL != bmp->mask ) {
      bmp->old_hbm_mask = SelectObject( bmp->hdc_mask, bmp->mask );
   }

cleanup:

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

#  ifdef RETROFLAT_OPENGL

   retval = retroglu_draw_release( bmp );

#  else

   /* == Win16/Win32 == */

   if( NULL == bmp ) {
      /* Trigger a screen refresh if this was a screen lock. */
      if( (HWND)NULL != g_retroflat_state->window ) {
         InvalidateRect( g_retroflat_state->window, 0, TRUE );
      }

#     ifdef RETROFLAT_VDP
      if( NULL != g_retroflat_state->vdp_buffer ) {
         bmp = g_retroflat_state->vdp_buffer;
      } else {
#     endif

      /* TODO: Reconcile with VDP! */
      goto cleanup;

#     ifdef RETROFLAT_VDP
      }
#     endif
   }

   /* Unlock the bitmap. */
   if( (HDC)NULL != bmp->hdc_b ) {
      SelectObject( bmp->hdc_b, bmp->old_hbm_b );
      DeleteDC( bmp->hdc_b );
      bmp->hdc_b = (HDC)NULL;
      bmp->old_hbm_b = (HBITMAP)NULL;
   }

   /* Unlock the mask. */
   if( (HDC)NULL != bmp->hdc_mask ) {
      SelectObject( bmp->hdc_mask, bmp->old_hbm_mask );
      DeleteDC( bmp->hdc_mask );
      bmp->hdc_mask = (HDC)NULL;
      bmp->old_hbm_mask = (HBITMAP)NULL;
   }

cleanup:

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_API_WIN16 )
   char* buf = NULL;
   mfile_t bmp_file;
   long int i, x, y, w, h, colors, offset, sz, read;
#  elif defined( RETROFLAT_API_WIN32 )
   BITMAP bm;
#  endif /* RETROFLAT_API_WIN32 */

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, RETROFLAT_PATH_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

#  ifdef RETROFLAT_OPENGL

   retval = retroglu_load_bitmap( filename_path, bmp_out, flags );

#  elif defined( RETROFLAT_API_WIN16 )

   /* Win16 has a bunch of extra involved steps for getting a bitmap from
    * disk. These cause a crash in Win32.
    */

   /* == Win16 == */

   /* Load the bitmap file from disk. */
   retval = mfile_open_read( filename_path, &bmp_file );
   maug_cleanup_if_not_ok();

   buf = calloc( mfile_get_sz( &bmp_file ), 1 );
   maug_cleanup_if_null_alloc( char*, buf );

   /*
   read = fread( buf, 1, sz, bmp_file );
   assert( read == sz );
   */
   retval = bmp_file.read_int( &bmp_file, buf, mfile_get_sz( &bmp_file ), 0 );
   maug_cleanup_if_not_ok();

   /* Read bitmap properties from header offsets. */
   retval = bmp_file.seek( &bmp_file, 10 );
   maug_cleanup_if_not_ok();
   retval = bmp_file.read_int( &bmp_file,
      (uint8_t*)&offset, 4, MFILE_READ_FLAG_LSBF );
   maug_cleanup_if_not_ok();

   retval = bmp_file.seek( &bmp_file, 46 );
   maug_cleanup_if_not_ok();
   retval = bmp_file.read_int( &bmp_file,
      (uint8_t*)&colors, 4, MFILE_READ_FLAG_LSBF );
   maug_cleanup_if_not_ok();

   /* Avoid a color overflow. */
   if(
      sizeof( BITMAPFILEHEADER ) +
      sizeof( BITMAPINFOHEADER ) +
      (colors * sizeof( RGBQUAD )) > sz
   ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error",
         "Attempted to load bitmap with too many colors!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

   memcpy( &(bmp_out->bmi),
      &(buf[sizeof( BITMAPFILEHEADER )]),
      /* SetDIBits needs the color palette! */
      /* TODO: Sync with state palette? */
      sizeof( BITMAPINFOHEADER ) + (colors * sizeof( RGBQUAD )) );

   /* This never gets the height right? */
   debug_printf( 1, "bitmap w: %08x, h: %08x, colors: %d",
      bmp_out->bmi.header.biWidth, bmp_out->bmi.header.biHeight, colors );

   assert( 0 < bmp_out->bmi.header.biWidth );
   assert( 0 < bmp_out->bmi.header.biHeight );
   assert( 0 == bmp_out->bmi.header.biWidth % 8 );
   assert( 0 == bmp_out->bmi.header.biHeight % 8 );

   bmp_out->b = CreateCompatibleBitmap( g_retroflat_state->hdc_win,
      bmp_out->bmi.header.biWidth, bmp_out->bmi.header.biHeight );
   maug_cleanup_if_null( HBITMAP, bmp_out->b, RETROFLAT_ERROR_BITMAP );

   /* Turn the bits into a bitmap. */
   SetDIBits( g_retroflat_state->hdc_win, bmp_out->b, 0,
      bmp_out->bmi.header.biHeight, &(buf[offset]),
      (BITMAPINFO*)&(bmp_out->bmi),
      DIB_RGB_COLORS );

   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      retval = retroflat_bitmap_win_transparency( bmp_out,
         bmp_out->bmi.header.biWidth, bmp_out->bmi.header.biHeight );
   }

#  else

   /* Win32 greatly simplifies the loading portion. */

   /* == Win32 == */

#        ifdef RETROFLAT_API_WINCE
   bmp_out->b = SHLoadDIBitmap( filename_path );
#        else
   bmp_out->b = LoadImage(
      NULL, filename_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
#        endif /* RETROFLAT_API_WINCE */
   maug_cleanup_if_null_msg(
      HBITMAP, bmp_out->b, MERROR_FILE, "failed to open FILE!" )

   GetObject( bmp_out->b, sizeof( BITMAP ), &bm );

   bmp_out->bmi.header.biSize = sizeof( BITMAPINFOHEADER );
   bmp_out->bmi.header.biCompression = BI_RGB;
   bmp_out->bmi.header.biWidth = bm.bmWidth;
   bmp_out->bmi.header.biHeight = bm.bmHeight;
   bmp_out->bmi.header.biPlanes = bm.bmPlanes;
   bmp_out->bmi.header.biBitCount = bm.bmBitsPixel;
   bmp_out->bmi.header.biSizeImage =
      bmp_out->bmi.header.biWidth *
      bmp_out->bmi.header.biHeight *
      (bm.bmBitsPixel / sizeof( uint8_t ));

   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      retval = retroflat_bitmap_win_transparency(
         bmp_out, bm.bmWidth, bm.bmHeight );
   }

#  endif /* RETROFLAT_API_WIN16 */

   /* The transparency portion is the same for Win32 and Win16. */

cleanup:

#  ifdef RETROFLAT_API_WIN16
   if( NULL != buf ) {
      free( buf );
   }

   mfile_close( &bmp_file );

#  endif /* RETROFLAT_API_WIN16 */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   int i = 0;
   PALETTEENTRY palette[RETROFLAT_BMP_COLORS_SZ_MAX];

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

#  if defined( RETROFLAT_OPENGL )

   retval = retroglu_create_bitmap( w, h, bmp_out, flags );

#  else

   /* == Win16 / Win32 == */

   /* TODO: Handle opaque flag. */
   bmp_out->mask = (HBITMAP)NULL;

#     ifdef RETROFLAT_WING
   /* Put this first because WinGRecommendDIBFormat sets some header props. */
   if(
      RETROFLAT_FLAGS_SCREEN_BUFFER == 
         (RETROFLAT_FLAGS_SCREEN_BUFFER & flags) &&
      (WinGCreateDC_t)NULL != g_w.WinGCreateDC &&
      (WinGRecommendDIBFormat_t)NULL != g_w.WinGRecommendDIBFormat
   ) {
      bmp_out->hdc_b = g_w.WinGCreateDC();

      if(
         !g_w.WinGRecommendDIBFormat( (BITMAPINFO far*)&(bmp_out->bmi) )
      ) {
         retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
            "Error", "Could not determine recommended format!" );
         retval = RETROFLAT_ERROR_GRAPHICS;
         goto cleanup;
      }
   }
#     endif /* RETROFLAT_WING */

   debug_printf( 0, "creating bitmap..." );

   bmp_out->bmi.header.biSize = sizeof( BITMAPINFOHEADER );
   bmp_out->bmi.header.biPlanes = 1;
   bmp_out->bmi.header.biCompression = BI_RGB;
   bmp_out->bmi.header.biWidth = w;
#     ifdef RETROFLAT_WING
   bmp_out->bmi.header.biHeight *= h;
#     else
   bmp_out->bmi.header.biHeight = h;
#     endif /* RETROFLAT_WING */
   bmp_out->bmi.header.biBitCount = 32;
   bmp_out->bmi.header.biSizeImage = w * h * 4;

   GetSystemPaletteEntries(
      g_retroflat_state->hdc_win, 0, RETROFLAT_BMP_COLORS_SZ_MAX, palette );
   for( i = 0 ; RETROFLAT_BMP_COLORS_SZ_MAX > i ; i++ ) {
      bmp_out->bmi.colors[i].rgbRed = palette[i].peRed;
      bmp_out->bmi.colors[i].rgbGreen = palette[i].peGreen;
      bmp_out->bmi.colors[i].rgbBlue = palette[i].peBlue;
      bmp_out->bmi.colors[i].rgbReserved = 0;
   }

#     ifdef RETROFLAT_WING
   /* Now try to create the WinG bitmap using the header we've built. */
   if(
      RETROFLAT_FLAGS_SCREEN_BUFFER == 
         (RETROFLAT_FLAGS_SCREEN_BUFFER & flags) &&
      (WinGCreateBitmap_t)NULL != g_w.WinGCreateBitmap
   ) {
      /* Setup an optimal WinG hardware screen buffer bitmap. */
      debug_printf( 1, "creating WinG-backed bitmap..." );

      bmp_out->flags |= RETROFLAT_FLAGS_SCREEN_BUFFER;
      bmp_out->b = g_w.WinGCreateBitmap(
         bmp_out->hdc_b,
         (BITMAPINFO far*)(&bmp_out->bmi),
         (void far*)&(bmp_out->bits) );

      debug_printf( 1, "WinG bitmap bits: %p", bmp_out->bits );

   } else {
#     endif /* RETROFLAT_WING */

   bmp_out->b = CreateCompatibleBitmap( g_retroflat_state->hdc_win, w, h );
   maug_cleanup_if_null( HBITMAP, bmp_out->b, RETROFLAT_ERROR_BITMAP );

   if(
      RETROFLAT_FLAGS_SCREEN_BUFFER == (RETROFLAT_FLAGS_SCREEN_BUFFER & flags)
   ) {
      debug_printf( 1, "creating screen device context..." );
      bmp_out->hdc_b = CreateCompatibleDC( g_retroflat_state->hdc_win );
      bmp_out->old_hbm_b = SelectObject( bmp_out->hdc_b, bmp_out->b );
   }

#     ifdef RETROFLAT_WING
   }
#     endif /* RETROFLAT_WING */

cleanup:

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

#  if defined( RETROFLAT_OPENGL )

   retroglu_destroy_bitmap( bmp );

#  else

   /* == Win16 == */

   if( NULL != bmp->old_hbm_b ) {
      SelectObject( bmp->hdc_b, bmp->old_hbm_b );
      bmp->old_hbm_b = (HBITMAP)NULL;
      bmp->old_hbm_b = (HBITMAP)NULL;
   }

   if( (HBITMAP)NULL != bmp->b ) {
      DeleteObject( bmp->b );
      bmp->b = (HBITMAP)NULL;
   }

   if( (HBITMAP)NULL != bmp->mask ) {
      DeleteObject( bmp->mask );
      bmp->mask = (HBITMAP)NULL;
   }

#  endif /* RETROFLAT_OPENGL */

}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int16_t w, int16_t h
) {
   MERROR_RETVAL retval = MERROR_OK;
   int locked_src_internal = 0;

#  ifndef RETROFLAT_OPENGL
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#  endif /* RETROFLAT_OPENGL */

   assert( NULL != src );

#  if defined( RETROFLAT_OPENGL )

   retroglu_blit_bitmap( target, src, s_x, s_y, d_x, d_y, w, h );

#  else

   /* == Win16/Win32 == */

   assert( (HBITMAP)NULL != target->b );
   assert( (HBITMAP)NULL != src->b );

   retroflat_internal_autolock_bitmap( src, locked_src_internal );
   assert( retroflat_bitmap_locked( target ) );

   if( (HBITMAP)NULL != src->mask ) {
      /* Use mask to blit transparency. */
      BitBlt(
         target->hdc_b, d_x, d_y, w, h, src->hdc_mask, s_x, s_y, SRCAND );

      /* Do actual blit. */
      BitBlt(
         target->hdc_b, d_x, d_y, w, h, src->hdc_b, s_x, s_y, SRCPAINT );
   } else {
      /* Just overwrite entire rect. */
      BitBlt(
         target->hdc_b, d_x, d_y, w, h, src->hdc_b, s_x, s_y, SRCCOPY );
   }

cleanup:

   if( locked_src_internal ) {
      retroflat_draw_release( src );
   }

#  endif /* RETROFLAT_OPENGL */
}

#endif /* !RETPLTF_H */

