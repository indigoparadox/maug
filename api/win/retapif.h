
#ifndef RETPLTF_H
#define RETPLTF_H

#  ifdef RETROFLAT_SCREENSAVER

/* Windows screensaver (.scr) command-line arguments. */

static int retroflat_cli_p( const char* arg, struct RETROFLAT_ARGS* args ) {
#ifdef __WIN64__
   /* 64-bit Windows has 64-bit pointers! */
   intptr_t hwnd_long = 0;
#else
   long hwnd_long = 0;
#endif /* __GNUC__ */
   if( 0 == strncmp( MAUG_CLI_SIGIL "p", arg, MAUG_CLI_SIGIL_SZ + 2 ) ) {
      /* The next arg must be the new var. */
   } else {
#ifdef __WIN64__
      hwnd_long = atoll( arg );
#else
      hwnd_long = atol( arg );
#endif /* __GNUC__ */
      g_retroflat_state->platform.parent = (HWND)hwnd_long;
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_s( const char* arg, struct RETROFLAT_ARGS* args ) {
   debug_printf( 3, "using screensaver mode..." );
   args->flags |= RETROFLAT_FLAGS_SCREENSAVER;
   return RETROFLAT_OK;
}

#  endif /* RETROFLAT_SCREENSAVER */

/* === */

LPSTR* retroflat_win_cli( LPSTR cmd_line, int* argc_out ) {
   LPSTR* argv_out = NULL;
   int i = 0,
      arg_iter = 0,
      arg_start = 0,
      arg_idx = 0,
      arg_longest = 10; /* Program name. */
   MERROR_RETVAL retval = MERROR_OK;

   /* This uses calloc() to simplify things, since this works on Windows, the
    * only platform where this routine is used, anyway. */

   debug_printf( 1, "retroflat: win cli: %s", cmd_line );

   /* Get the number of args. */
   *argc_out = 1; /* Program name. */
   for( i = 0 ; '\0' != cmd_line[i - 1] ; i++ ) {
      if( ' ' != cmd_line[i] && '\0' != cmd_line[i] ) {
         arg_iter++;
      } else if( 0 < i ) {
         (*argc_out)++;
         if( arg_iter > arg_longest ) {
            /* This is the new longest arg. */
            arg_longest = arg_iter;
         }
         arg_iter = 0;
      }
   }

   argv_out = calloc( *argc_out, sizeof( char* ) );
   maug_cleanup_if_null_alloc( char**, argv_out );

   /* NULL program name. */
   argv_out[0] = calloc( 1, sizeof( char ) );
   maug_cleanup_if_null_alloc( char*, argv_out[0] );

   /* Copy args into array. */
   arg_idx = 1;
   for( i = 0 ; '\0' != cmd_line[i - 1] ; i++ ) {
      if( ' ' != cmd_line[i] && '\0' != cmd_line[i] ) {
         /* If this isn't a WS char, it's an arg. */
         if( 0 < i && ' ' == cmd_line[i - 1] ) {
            /* If this is first non-WS char, it's start of a new arg. */
            arg_start = i;
            arg_iter = 0;
         }
         arg_iter++;
         continue;
      }

      if( 0 < i && ' ' != cmd_line[i - 1] ) {
         /* If this is first WS char, it's the end of an arg. */
         assert( NULL == argv_out[arg_idx] );
         argv_out[arg_idx] = calloc( arg_iter + 1, sizeof( char ) );
         maug_cleanup_if_null_alloc( char*, argv_out[arg_idx] );
         strncpy( argv_out[arg_idx], &(cmd_line[arg_start]), arg_iter );
         arg_idx++; /* Start next arg. */
         arg_iter = 0; /* New arg is 0 long. */
         arg_start = i; /* New arg starts here (maybe). */
      }
   }

cleanup:

   if( MERROR_OK != retval && NULL != argv_out ) {
      for( i = 0 ; *argc_out > i ; i++ ) {
         free( argv_out[i] );
         argv_out[i] = NULL;
      }
      free( argv_out );
      argv_out = NULL;
   }

   return argv_out;
}

/* === */

static LRESULT CALLBACK WndProc(
   HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
) {
#     ifndef RETROFLAT_OPENGL
   PAINTSTRUCT ps;
   HDC hdc_paint = (HDC)NULL;
#     endif /* !RETROFLAT_OPENGL */
#     if defined( RETROFLAT_OPENGL )
   int pixel_fmt_int = 0;
   static PIXELFORMATDESCRIPTOR pixel_fmt = {
      sizeof( PIXELFORMATDESCRIPTOR ),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,
      RETROFLAT_OPENGL_BPP,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      16,
      0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
   };
#     endif /* RETROFLAT_OPENGL */

   switch( message ) {
      case WM_CREATE:

         g_retroflat_state->platform.hdc_win = GetDC( hWnd );

#     if defined( RETROFLAT_OPENGL )
         
         pixel_fmt_int =
            ChoosePixelFormat( g_retroflat_state->hdc_win, &pixel_fmt );
         SetPixelFormat(
            g_retroflat_state->hdc_win, pixel_fmt_int, &pixel_fmt );

         debug_printf( 1, "setting up OpenGL context..." );

         g_retroflat_state->hrc_win =
            wglCreateContext( g_retroflat_state->hdc_win );
         if(
            FALSE == wglMakeCurrent( g_retroflat_state->hdc_win,
               g_retroflat_state->hrc_win )
         ) {
            retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
               "Error", "Error creating OpenGL context: %d",
               GetLastError() );
         }

#     else

         /* Setup the screen buffer. */
         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            debug_printf( 1, "retroflat: creating window buffer (%d x %d)...",
               g_retroflat_state->screen_w, g_retroflat_state->screen_h );
            /* Do this in its own function so a one-time setup isn't using stack
            * in our WndProc!
            */
            retroflat_create_bitmap(
               g_retroflat_state->screen_v_w,
               g_retroflat_state->screen_v_h,
               &(g_retroflat_state->buffer),
               RETROFLAT_FLAGS_SCREEN_BUFFER | RETROFLAT_FLAGS_OPAQUE );
            if( (HDC)NULL == g_retroflat_state->buffer.hdc_b ) {
               retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
                  "Error", "Could not determine buffer device context!" );
               g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
               retroflat_quit( g_retroflat_state->retval );
               break;
            }

         }
         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
               "Error", "Could not create screen buffer!" );
            g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retroflat_state->retval );
            break;
         }

#     endif /* RETROFLAT_OPENGL */
         break;

      case WM_CLOSE:
#     if defined( RETROFLAT_OPENGL )
         wglMakeCurrent( g_retroflat_state->hdc_win, NULL );
         wglDeleteContext( g_retroflat_state->hrc_win );
#     endif /* RETROFLAT_OPENGL */

         /* Quit on window close. */
         retroflat_quit( 0 );
         break;

#     if !defined( RETROFLAT_OPENGL )
      case WM_PAINT:

         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            error_printf( "screen buffer not ready!" );
            break;
         }

         /* Create HDC for window to blit to. */
         /* maug_mzero( &ps, sizeof( PAINTSTRUCT ) ); */
         hdc_paint = BeginPaint( hWnd, &ps );
         if( (HDC)NULL == hdc_paint ) {
            retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
               "Error", "Could not determine window device context!" );
            g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retroflat_state->retval );
            break;
         }

#        if defined( RETROFLAT_VDP )
         retroflat_vdp_call( "retroflat_vdp_flip" );
#        endif /* RETROFLAT_VDP */

#        ifdef RETROFLAT_WING
         if( (WinGStretchBlt_t)NULL != g_w.WinGStretchBlt ) {
            g_w.WinGStretchBlt(
               hdc_paint,
               0, 0,
               g_retroflat_state->screen_w, g_retroflat_state->screen_h,
               g_retroflat_state->buffer.hdc_b,
               0, 0,
               g_retroflat_state->screen_w,
               g_retroflat_state->screen_h
            );
#           ifdef RETROFLAT_API_WIN32
            GdiFlush();
#           endif /* RETROFLAT_API_WIN32 */
         } else {
#        endif /* RETROFLAT_WING */
         StretchBlt(
            hdc_paint,
            0, 0,
            g_retroflat_state->screen_w, g_retroflat_state->screen_h,
            g_retroflat_state->buffer.hdc_b,
            0, 0,
            g_retroflat_state->screen_v_w,
            g_retroflat_state->screen_v_h,
            SRCCOPY
         );
#        ifdef RETROFLAT_WING
         }
#        endif /* RETROFLAT_WING */

         DeleteDC( hdc_paint );
         EndPaint( hWnd, &ps );
         break;

#     endif /* !RETROFLAT_OPENGL */

      case WM_ERASEBKGND:
         return 1;

      case WM_KEYDOWN:
         switch( wParam ) {
         case VK_SHIFT:
            g_retroflat_state->platform.vk_mods |= RETROFLAT_INPUT_MOD_SHIFT;
            break;

         case VK_CONTROL:
            g_retroflat_state->platform.vk_mods |= RETROFLAT_INPUT_MOD_CTRL;
            break;

         /* TODO: Alt? */

         default:
            /*
            TODO: Fix in win64.
            debug_printf( RETROFLAT_KB_TRACE_LVL, "0x%x", lParam );
            */
            g_retroflat_state->platform.last_key = wParam | ((lParam & 0x800000) >> 8);
            break;
         }
         break;

      case WM_KEYUP:
         switch( wParam ) {
         case VK_SHIFT:
            g_retroflat_state->platform.vk_mods &= ~RETROFLAT_INPUT_MOD_SHIFT;
            break;

         case VK_CONTROL:
            g_retroflat_state->platform.vk_mods |= RETROFLAT_INPUT_MOD_CTRL;
            break;

         /* TODO: Alt? */

         }
         break;

      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
         g_retroflat_state->platform.last_mouse = wParam;
         g_retroflat_state->platform.last_mouse_x = GET_X_LPARAM( lParam );
         g_retroflat_state->platform.last_mouse_y = GET_Y_LPARAM( lParam );
         break;

      /* TODO: Handle resize message. */

      case WM_DESTROY:
         if( retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            DeleteObject( g_retroflat_state->buffer.b );
         }
         PostQuitMessage( 0 );
         break;

      case WM_SIZE:
         retroflat_on_resize( LOWORD( lParam ), HIWORD( lParam ) );
         if( NULL != g_retroflat_state->on_resize ) {
            g_retroflat_state->on_resize(
               LOWORD( lParam ), HIWORD( lParam ),
               g_retroflat_state->on_resize_data );
         }
         break;

      case WM_TIMER:
         if(
#     ifdef RETROFLAT_OPENGL
            (HGLRC)NULL == g_retroflat_state->hrc_win ||
#     else
            !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ||
#     endif /* !RETROFLAT_OPENGL */
            hWnd != g_retroflat_state->platform.window
         ) {
            /* Timer message was called prematurely. */
            break;
         }

         if( RETROFLAT_WIN_FRAME_TIMER_ID == wParam ) {
            /* Frame timer has expired. */
            assert( NULL != g_retroflat_state->platform.frame_iter );
            g_retroflat_state->platform.frame_iter(
               g_retroflat_state->loop_data );
         } else if( RETROFLAT_WIN_LOOP_TIMER_ID == wParam ) {
            /* Loop/tick timer has expired. */
            assert( NULL != g_retroflat_state->platform.loop_iter );
            g_retroflat_state->platform.loop_iter(
               g_retroflat_state->loop_data );
         }
         break;

      case WM_COMMAND:
         g_retroflat_state->platform.last_idc = LOWORD( wParam );
         break;

      default:
         return DefWindowProc( hWnd, message, wParam, lParam );
   }

   return 0;
}

/* === */

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

/* === */

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

   SetWindowText( g_retroflat_state->platform.window, title );

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
      if( (HWND)NULL != g_retroflat_state->platform.window ) {
         InvalidateRect( g_retroflat_state->platform.window, 0, TRUE );
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
#  elif defined( RETROFLAT_API_WIN32 ) && !defined( RETROFLAT_OPENGL )
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

   bmp_out->b = CreateCompatibleBitmap( g_retroflat_state->platform.hdc_win,
      bmp_out->bmi.header.biWidth, bmp_out->bmi.header.biHeight );
   maug_cleanup_if_null( HBITMAP, bmp_out->b, RETROFLAT_ERROR_BITMAP );

   /* Turn the bits into a bitmap. */
   SetDIBits( g_retroflat_state->platform.hdc_win, bmp_out->b, 0,
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
#  ifndef RETROFLAT_OPENGL
   int i = 0;
   PALETTEENTRY palette[RETROFLAT_BMP_COLORS_SZ_MAX];
#  endif /* !RETROFLAT_OPENGL */

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
      g_retroflat_state->platform.hdc_win, 0,
      RETROFLAT_BMP_COLORS_SZ_MAX, palette );
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

   bmp_out->b = CreateCompatibleBitmap(
      g_retroflat_state->platform.hdc_win, w, h );
   maug_cleanup_if_null( HBITMAP, bmp_out->b, RETROFLAT_ERROR_BITMAP );

   if(
      RETROFLAT_FLAGS_SCREEN_BUFFER == (RETROFLAT_FLAGS_SCREEN_BUFFER & flags)
   ) {
      debug_printf( 1, "creating screen device context..." );
      bmp_out->hdc_b = CreateCompatibleDC(
         g_retroflat_state->platform.hdc_win );
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
#  ifndef RETROFLAT_OPENGL
   int retval = 0;
   int locked_src_internal = 0;

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

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   if( g_retroflat_state->platform.last_key ) {
      /* Return g_retroflat_state->last_key, which is set in WndProc when a
       * keypress msg is received.
       */
      key_out = g_retroflat_state->platform.last_key;
      input->key_flags = g_retroflat_state->platform.vk_mods;

      debug_printf( RETROFLAT_KB_TRACE_LVL, "raw key: 0x%04x", key_out );

      /* Reset pressed key. */
      g_retroflat_state->platform.last_key = 0;

   } else if( g_retroflat_state->platform.last_mouse ) {
      if(
         MK_LBUTTON == (MK_LBUTTON & g_retroflat_state->platform.last_mouse)
      ) {
         input->mouse_x = g_retroflat_state->platform.last_mouse_x;
         input->mouse_y = g_retroflat_state->platform.last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_LEFT;
      } else if(
         MK_RBUTTON == (MK_RBUTTON & g_retroflat_state->platform.last_mouse)
      ) {
         input->mouse_x = g_retroflat_state->platform.last_mouse_x;
         input->mouse_y = g_retroflat_state->platform.last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_RIGHT;
      }
      g_retroflat_state->platform.last_mouse = 0;
      g_retroflat_state->platform.last_mouse_x = 0;
      g_retroflat_state->platform.last_mouse_y = 0;
   }

#     ifdef RETROFLAT_SCREENSAVER
   if( 
      (RETROFLAT_FLAGS_SCREENSAVER ==
      (RETROFLAT_FLAGS_SCREENSAVER & g_retroflat_state->retroflat_flags))
      && 0 != key_out
   ) {
      /* retroflat_quit( 0 ); */
   }
#     endif /* RETROFLAT_SCREENSAVER */

   return key_out;
}

#endif /* !RETPLTF_H */

