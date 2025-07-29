
#ifndef RETPLTF_H
#define RETPLTF_H

/* TODO: WinG runs but just shows an empty window. */

/* For now, these are set by WinMain(), so they need to be outside of the
 * state that's zeroed on init()!
 */
HINSTANCE            g_retroflat_instance;
int                  g_retroflat_cmd_show;

#     ifdef RETROFLAT_WING
struct RETROFLAT_WING_MODULE g_w;
#     endif /* RETROFLAT_WING */

/* === */

#ifndef MAUG_NO_CLI

#  ifdef RETROFLAT_SCREENSAVER

/* Windows screensaver (.scr) command-line arguments. */

static int retroflat_cli_p(
   const char* arg, ssize_t argc, struct RETROFLAT_ARGS* args
) {
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

/* === */

static int retroflat_cli_s(
   const char* arg, ssize_t argc, struct RETROFLAT_ARGS* args
) {
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

#endif /* !MAUG_NO_CLI */

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
         
         pixel_fmt_int = ChoosePixelFormat(
            g_retroflat_state->platform.hdc_win, &pixel_fmt );
         SetPixelFormat(
            g_retroflat_state->platform.hdc_win, pixel_fmt_int, &pixel_fmt );

         debug_printf( 1, "setting up OpenGL context..." );

         g_retroflat_state->platform.hrc_win =
            wglCreateContext( g_retroflat_state->platform.hdc_win );
         if(
            FALSE == wglMakeCurrent( g_retroflat_state->platform.hdc_win,
               g_retroflat_state->platform.hrc_win )
         ) {
            retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
               "Error", "Error creating OpenGL context: %d",
               GetLastError() );
         }

#     else

         /* Setup the screen buffer. */
         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            debug_printf( 1, "retroflat: creating window buffer ("
               SIZE_T_FMT " x " SIZE_T_FMT ")...",
               g_retroflat_state->screen_v_w, g_retroflat_state->screen_v_h );
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
               g_retroflat_state->retval = MERROR_GUI;
               retroflat_quit( g_retroflat_state->retval );
               break;
            }

         }
         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
               "Error", "Could not create screen buffer!" );
            g_retroflat_state->retval = MERROR_GUI;
            retroflat_quit( g_retroflat_state->retval );
            break;
         }

#     endif /* RETROFLAT_OPENGL */
         break;

      case WM_CLOSE:
#     if defined( RETROFLAT_OPENGL )
         wglMakeCurrent( g_retroflat_state->platform.hdc_win, NULL );
         wglDeleteContext( g_retroflat_state->platform.hrc_win );
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
            g_retroflat_state->retval = MERROR_GUI;
            retroflat_quit( g_retroflat_state->retval );
            break;
         }

#        if defined( RETROFLAT_VDP )
         retroflat_vdp_call( "retroflat_vdp_flip" );
#        endif /* RETROFLAT_VDP */

#        ifdef RETROFLAT_WING
         if( (WinGStretchBlt_t)NULL != g_w.WinGStretchBlt ) {
            /* TODO: Are these stretch values right? */
            if( !g_w.WinGStretchBlt(
               hdc_paint,
               0, 0,
               g_retroflat_state->screen_w,
               g_retroflat_state->screen_h,
               g_retroflat_state->buffer.hdc_b,
               0, 0,
               g_retroflat_state->screen_w,
               g_retroflat_state->screen_h
            ) ) {
               retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
                  "Error", "Could not blit to screen!" );
               g_retroflat_state->retval = MERROR_GUI;
               retroflat_quit( g_retroflat_state->retval );
            }
#           ifdef RETROFLAT_API_WIN32
            GdiFlush();
#           endif /* RETROFLAT_API_WIN32 */
         } else {
#        endif /* RETROFLAT_WING */
         StretchBlt(
            hdc_paint,
            0, 0,
            g_retroflat_state->screen_w,
            g_retroflat_state->screen_h,
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
            g_retroflat_state->input.vk_mods |= RETROFLAT_INPUT_MOD_SHIFT;
            break;

         case VK_CONTROL:
            g_retroflat_state->input.vk_mods |= RETROFLAT_INPUT_MOD_CTRL;
            break;

         /* TODO: Alt? */

         default:
            /*
            TODO: Fix in win64.
            debug_printf( RETROFLAT_KB_TRACE_LVL, "0x%x", lParam );
            */
            g_retroflat_state->input.last_key = wParam | ((lParam & 0x800000) >> 8);
            break;
         }
         break;

      case WM_KEYUP:
         switch( wParam ) {
         case VK_SHIFT:
            g_retroflat_state->input.vk_mods &= ~RETROFLAT_INPUT_MOD_SHIFT;
            break;

         case VK_CONTROL:
            g_retroflat_state->input.vk_mods |= RETROFLAT_INPUT_MOD_CTRL;
            break;

         /* TODO: Alt? */

         }
         break;

      case WM_LBUTTONDOWN:
#ifndef RETROFLAT_API_WINCE
      case WM_RBUTTONDOWN:
#endif /* !RETROFLAT_API_WINCE */
         g_retroflat_state->input.last_mouse = wParam;
         g_retroflat_state->input.last_mouse_x =
            GET_X_LPARAM( lParam ) / g_retroflat_state->scale;
         g_retroflat_state->input.last_mouse_y =
            GET_Y_LPARAM( lParam ) / g_retroflat_state->scale;
         break;

      case WM_DESTROY:
#ifndef RETROFLAT_OPENGL
         if( retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            DeleteObject( g_retroflat_state->buffer.b );
         }
#endif /* !RETROFLAT_OPENGL */
         PostQuitMessage( 0 );
         break;

      case WM_SIZE:
         /* Handle resize message. */
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
            (HGLRC)NULL == g_retroflat_state->platform.hrc_win ||
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
            assert( NULL != g_retroflat_state->frame_iter );
            g_retroflat_state->frame_iter(
               g_retroflat_state->loop_data );
         } else if( RETROFLAT_WIN_LOOP_TIMER_ID == wParam ) {
            /* Loop/tick timer has expired. */
            /* TODO: Move this to message-processing loop. */
            assert( NULL != g_retroflat_state->loop_iter );
            g_retroflat_state->loop_iter(
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
   uint8_t autorelock = 0;

   assert( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & bmp_out->flags) );

   /* Unlock the mask if one exists. */
   if( (HDC)NULL == bmp_out->hdc_mask ) {
      debug_printf( 1, "autolocking transparency mask..." );

      /* Create HDC for source mask compatible with the buffer. */
      bmp_out->hdc_mask = CreateCompatibleDC( (HDC)NULL );
      maug_cleanup_if_null( HDC, bmp_out->hdc_mask, RETROFLAT_ERROR_BITMAP );
      
      bmp_out->old_hbm_mask = SelectObject( bmp_out->hdc_mask, bmp_out->mask );

      autorelock = 1;
   }

   assert( (HBITMAP)NULL != bmp_out->mask );
   assert( (HBITMAP)NULL != bmp_out->hdc_mask );

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

   debug_printf( 1, "updated transparency mask for %p", bmp_out );

cleanup:

   if( autorelock ) {
      debug_printf( 1, "autounlocking transparency mask..." );
      SelectObject( bmp_out->hdc_mask, bmp_out->old_hbm_mask );
      DeleteDC( bmp_out->hdc_mask );
      bmp_out->hdc_mask = (HDC)NULL;
      bmp_out->old_hbm_mask = (HBITMAP)NULL;
   }

   if( RETROFLAT_OK == retval ) {
      /* retroflat_draw_release( bmp_out ); */
   }

   return retval;
}

#endif /* !RETROFLAT_OPENGL */

/* === */

MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
   WNDCLASS wc;
   RECT wr = { 0, 0, 0, 0 };
   DWORD window_style = RETROFLAT_WIN_STYLE;
   DWORD window_style_ex = 0;
   int ww = 0,
      wh = 0;
#ifdef MAUG_WCHAR
   wchar_t title_w[RETROFLAT_TITLE_MAX + 1] = { 0 };
   wchar_t class_w[sizeof( RETROFLAT_WINDOW_CLASS ) + 1] = { 0 };
#endif /* MAUG_WCHAR */

   /* == Win16/Win32 == */

#     ifdef MAUG_NO_STDLIB
   srand( GetTickCount() );
#     else
   /* Time makes a better random seed if available. */
   srand( time( NULL ) );
#     endif /* RETROFLAT_API_WINCE */

   /* Setup color palettes. */
   /* TODO: For WinG, try to make the indexes match system palette? */
#     ifndef RETROFLAT_OPENGL
#        define RETROFLAT_COLOR_TABLE_WIN( idx, name_l, name_u, r, g, b, cgac, cgad ) \
            g_retroflat_state->palette[idx] = RGB( r, g, b );
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN )
#     endif /* RETROFLAT_OPENGL */

   /* TODO: Detect screen colors. */
   g_retroflat_state->screen_colors = 16;

#     ifdef RETROFLAT_WING
   debug_printf( 3, "attempting to link WinG..." );
   /* Dynamically load the WinG procedures. */
#        ifdef RETROFLAT_API_WIN32
   g_w.module = LoadLibrary( "wing32.dll" );
   if( (HMODULE)NULL == g_w.module ) {
#        elif defined( RETROFLAT_API_WIN16 )
   g_w.module = LoadLibrary( "wing.dll" );
   if( HINSTANCE_ERROR == g_w.module ) {
#        endif
      g_w.success = 0;
   } else {
      g_w.success = 1;

#        ifdef RETROFLAT_API_WIN32
#           define RETROFLAT_WING_LLTABLE_LOAD_PROC( retval, proc, ord ) \
               g_w.proc = (proc ## _t)GetProcAddress( g_w.module, #proc ); \
               if( (proc ## _t)NULL == g_w.proc ) { \
                  g_w.success = 0; \
               }
#        elif defined( RETROFLAT_API_WIN16 )
#           define RETROFLAT_WING_LLTABLE_LOAD_PROC( retval, proc, ord ) \
               g_w.proc = (proc ## _t)GetProcAddress( \
                  g_w.module, MAKEINTRESOURCE( ord ) ); \
               if( (proc ## _t)NULL == g_w.proc ) { \
                  retroflat_message( \
                     RETROFLAT_MSG_FLAG_ERROR, "Error", \
                     "Unable to link WinG proc: %s", #proc ); \
                  g_w.success = 0; \
               }
#        endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

      RETROFLAT_WING_LLTABLE( RETROFLAT_WING_LLTABLE_LOAD_PROC )

   }

   if( !g_w.success ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Unable to link WinG!" );
   }
#     endif /* RETROFLAT_WING */

   /* Get the *real* size of the window, including titlebar. */
   wr.right = g_retroflat_state->screen_w;
   wr.bottom = g_retroflat_state->screen_h;
#     ifndef RETROFLAT_API_WINCE
   AdjustWindowRect( &wr, RETROFLAT_WIN_STYLE, FALSE );
#     endif /* !RETROFLAT_API_WINCE */

   memset(
      &(g_retroflat_state->buffer), '\0', sizeof( struct RETROFLAT_BITMAP ) );

   debug_printf( 1, "retroflat: creating window class..." );

#     ifdef MAUG_WCHAR
   if( 0 == MultiByteToWideChar(
      CP_ACP, MB_PRECOMPOSED, args->title, -1, title_w,
      RETROFLAT_TITLE_MAX
   ) ) {
      error_printf( "could not create wide window title!" );
      retval = MERROR_GUI;
      goto cleanup;
   }
   if( 0 == MultiByteToWideChar(
      CP_ACP, MB_PRECOMPOSED, RETROFLAT_WINDOW_CLASS, -1, class_w,
      sizeof( RETROFLAT_WINDOW_CLASS )
   ) ) {
      error_printf( "could not create wide window class!" );
      retval = MERROR_GUI;
      goto cleanup;
   }
#     endif /* MAUG_WCHAR */

   memset( &wc, '\0', sizeof( WNDCLASS ) );

   wc.lpfnWndProc   = (WNDPROC)&WndProc;
   wc.hInstance     = g_retroflat_instance;
#     ifdef RETROFLAT_ICO_RES_ID
   wc.hIcon         = LoadIcon(
      g_retroflat_instance, MAKEINTRESOURCE( RETROFLAT_ICO_RES_ID ) );
#     endif /* RETROFLAT_ICO_RES_ID */
#     ifndef RETROFLAT_API_WINCE
   wc.hCursor       = LoadCursor( 0, IDC_ARROW );
#     endif /* !RETROFLAT_API_WINCE */
   wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );
   /* wc.lpszMenuName  = MAKEINTRESOURCE( IDR_MAINMENU ); */
#     ifdef MAUG_WCHAR
   wc.lpszClassName = class_w;
#     else
   wc.lpszClassName = RETROFLAT_WINDOW_CLASS;
#     endif /* MAUG_WCHAR */

   if(
#     ifdef MAUG_WCHAR
      0 == RegisterClassW( &wc )
#     else
      0 == RegisterClass( &wc )
#     endif /* MAUG_WCHAR */
   ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Could not register window class!" );
      goto cleanup;
   }

   debug_printf( 1, "retroflat: creating window..." );
   
#     ifdef RETROFLAT_SCREENSAVER
   if( (HWND)0 != g_retroflat_state->platform.parent ) {
      /* Shrink the child window into the parent. */
      debug_printf( 1, "retroflat: using window parent: %p",
         g_retroflat_state->platform.parent );
      window_style = WS_CHILD;
      GetClientRect( g_retroflat_state->platform.parent, &wr );
   } else if(
      RETROFLAT_FLAGS_SCREENSAVER ==
      (RETROFLAT_FLAGS_SCREENSAVER & g_retroflat_state->retroflat_flags)
   ) {
      /* Make window fullscreen and on top. */
      window_style_ex = WS_EX_TOPMOST;
      window_style = WS_POPUP | WS_VISIBLE;
      /* X/Y are hardcoded to zero below, so just get desired window size. */
      wr.left = 0;
      wr.top = 0;
      wr.right = GetSystemMetrics( SM_CXSCREEN );
      wr.bottom = GetSystemMetrics( SM_CYSCREEN );
   } else {
#     endif /* RETROFLAT_SCREENSAVER */
#     ifndef RETROFLAT_API_WINCE
   /* Open in a centered window. */
   ww = wr.right - wr.left;
   wh = wr.bottom - wr.top;
   if( 0 == args->screen_x ) {
      args->screen_x = (GetSystemMetrics( SM_CXSCREEN ) / 2) - (ww / 2);
   }
   if( 0 == args->screen_y ) {
      args->screen_y = (GetSystemMetrics( SM_CYSCREEN ) / 2) - (wh / 2);
   }
#     endif /* !RETROFLAT_API_WINCE */
#     ifdef RETROFLAT_SCREENSAVER
   }
#     endif /* RETROFLAT_SCREENSAVER */

#     ifdef MAUG_WCHAR
   g_retroflat_state->platform.window = CreateWindowExW(
#     else
   g_retroflat_state->platform.window = CreateWindowEx(
#     endif /* MAUG_WCHAR */
      window_style_ex,
#     ifdef MAUG_WCHAR
      class_w,
      title_w,
#     else
      RETROFLAT_WINDOW_CLASS,
      args->title,
#     endif /* MAUG_WCHAR */
      window_style,
#     ifdef RETROFLAT_API_WINCE
      0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
#     else
      args->screen_x, args->screen_y, ww, wh,
#     endif /* RETROFLAT_API_WINCE */
#     ifdef RETROFLAT_SCREENSAVER
      g_retroflat_state->platform.parent
#     else
      0
#     endif /* RETROFLAT_SCREENSAVER */
      , 0, g_retroflat_instance, 0
   );

#     ifdef RETROFLAT_API_WINCE
   /* Force screen size. */
   /* XXX: Move this to RETROFLAT_DEFAULT_SCREEN_W/H in Makefile! */
   GetClientRect( g_retroflat_state->platform.window, &wr );
   g_retroflat_state->screen_w = wr.right - wr.left;
   g_retroflat_state->screen_h = wr.bottom - wr.top;
#     endif /* RETROFLAT_API_WINCE */

   if( !g_retroflat_state->platform.window ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Could not create window!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   maug_cleanup_if_null_alloc( HWND, g_retroflat_state->platform.window );

#ifndef RETROFLAT_OPENGL
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRSET )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PNSET )
#endif /* !RETROFLAT_OPENGL */

   ShowWindow( g_retroflat_state->platform.window, g_retroflat_cmd_show );

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {

   /* TODO: Windows shutdown? */

   /* Stop frame timer if available. */
   if( NULL != g_retroflat_state->frame_iter ) {
      KillTimer(
         g_retroflat_state->platform.window, RETROFLAT_WIN_FRAME_TIMER_ID );
   }

   /* Stop loop timer if available. */
   if( NULL != g_retroflat_state->loop_iter ) {
      KillTimer(
         g_retroflat_state->platform.window, RETROFLAT_WIN_LOOP_TIMER_ID );
   }

#     ifndef RETROFLAT_OPENGL
   if( (HDC)NULL != g_retroflat_state->buffer.hdc_b ) {
      /* Return the default object into the HDC. */
      SelectObject(
         g_retroflat_state->buffer.hdc_b,
         g_retroflat_state->buffer.old_hbm_b );
      DeleteDC( g_retroflat_state->buffer.hdc_b );
      g_retroflat_state->buffer.hdc_b = (HDC)NULL;

      /* Destroy buffer bitmap! */
      retroflat_destroy_bitmap( &(g_retroflat_state->buffer) );
   }

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRRM )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PENRM )
#     endif /* !RETROFLAT_OPENGL */

#     ifdef RETROFLAT_WING
   if( (HMODULE)NULL != g_w.module ) {
      FreeLibrary( g_w.module );
   }
#     endif /* RETROFLAT_WING */

   if( (HDC)NULL != g_retroflat_state->platform.hdc_win ) {
      ReleaseDC(
         g_retroflat_state->platform.window,
         g_retroflat_state->platform.hdc_win );
   }

}

/* === */

MERROR_RETVAL retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;
   int msg_retval = 0;
   MSG msg;

   /* Set these to be called from WndProc later. */
   g_retroflat_state->loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;
   g_retroflat_state->frame_iter = (retroflat_loop_iter)frame_iter;

   if(
      RETROFLAT_FLAGS_RUNNING ==
      (g_retroflat_state->retroflat_flags & RETROFLAT_FLAGS_RUNNING)
   ) {
      /* Main loop is already running, so we're just changing the iter call
       * and leaving!
       */
      debug_printf( 1, "main loop already running!" );
      goto cleanup;
   }

   if( NULL != frame_iter ) {
      debug_printf( 3, "setting up frame timer %u every %d ms...",
         RETROFLAT_WIN_FRAME_TIMER_ID, (int)(1000 / RETROFLAT_FPS) );
      if( !SetTimer(
         g_retroflat_state->platform.window, RETROFLAT_WIN_FRAME_TIMER_ID,
         (int)(1000 / RETROFLAT_FPS), NULL )
      ) {
         retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
            "Error", "Could not create frame timer!" );
         retval = RETROFLAT_ERROR_TIMER;
         goto cleanup;
      }
   }

   if( NULL != loop_iter ) {
      debug_printf( 3, "setting up loop timer %u every 1 ms...",
         RETROFLAT_WIN_LOOP_TIMER_ID );
      if( !SetTimer(
         g_retroflat_state->platform.window,
         RETROFLAT_WIN_LOOP_TIMER_ID, 1, NULL )
      ) {
         retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
            "Error", "Could not create loop timer!" );
         retval = RETROFLAT_ERROR_TIMER;
         goto cleanup;
      }
   }

   g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_RUNNING;

   /* TODO: loop_iter is artificially slow on Windows! */

   /* Handle Windows messages until quit. */
   do {
      msg_retval = GetMessage( &msg, 0, 0, 0 );
      TranslateMessage( &msg );
      DispatchMessage( &msg );
      if( WM_QUIT == msg.message ) {
         /* Get retval from PostQuitMessage(). */
         retval = msg.wParam;
         debug_printf( 1, "WM_QUIT received, retval: %d", retval );
      }
      retroflat_heartbeat_update();
   } while( WM_QUIT != msg.message && 0 < msg_retval );

cleanup:

   /* This should be set by retroflat_quit(). */
   return retval;
}

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   /* This function avoids dynamic allocation in the vain hope that it will
    * make it more stable, maybe to report something it couldn't, otherwise.
    */
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;
#  if (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_WIN )) || \
   (defined( RETROFLAT_API_GLUT) && defined( RETROFLAT_OS_WIN )) || \
   defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   uint32_t win_msg_flags = 0;
#  endif
#  ifdef MAUG_WCHAR
   wchar_t msg_out_w[RETROFLAT_MSG_MAX + 1] = { 0 };
   wchar_t title_w[RETROFLAT_TITLE_MAX + 1] = { 0 };
#  endif /* MAUG_WCHAR */

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

#  ifdef MAUG_WCHAR
   /* TODO: Fail on conversion failure? */
   MultiByteToWideChar(
      CP_ACP, MB_PRECOMPOSED, title, -1, title_w,
      RETROFLAT_TITLE_MAX );
   MultiByteToWideChar(
      CP_ACP, MB_PRECOMPOSED, msg_out, -1, msg_out_w,
      RETROFLAT_MSG_MAX );
   MessageBox( retroflat_root_win(), msg_out_w, title_w, win_msg_flags );
#  else
   MessageBox( retroflat_root_win(), msg_out, title, win_msg_flags );
#  endif /* MAUG_WCHAR */

   error_printf( "message: %s", msg_out );

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;
#ifdef MAUG_WCHAR
   wchar_t title_w[RETROFLAT_TITLE_MAX + 1] = { 0 };
#endif /* MAUG_WCHAR */

   /* Build the title. */
   va_start( vargs, format );
   memset( title, '\0', RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

#ifdef MAUG_WCHAR
   /* TODO: Fail on conversion failure? */
   MultiByteToWideChar(
      CP_ACP, MB_PRECOMPOSED, title, -1, title_w,
      RETROFLAT_TITLE_MAX );
   SetWindowTextW( g_retroflat_state->platform.window, title_w );
#else
   SetWindowText( g_retroflat_state->platform.window, title );
#endif /* MAUG_WCHAR */

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   return GetTickCount();
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

MERROR_RETVAL retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = RETROFLAT_OK;

#  if defined( RETROFLAT_OPENGL )

   if( NULL != bmp ) {
      retval = retro3d_texture_lock( &(bmp->tex) );
   } else {
      debug_printf( RETRO2D_TRACE_LVL, "called retroflat_draw_lock()!" );
   }

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

   if( NULL == bmp ) {
      /* Windows has its own OpenGL flip function.*/
      SwapBuffers( g_retroflat_state->platform.hdc_win );
   } else {
      debug_printf( RETRO2D_TRACE_LVL, "called retroflat_draw_release()!" );
   }

#  else

   /* == Win16/Win32 == */

   if( NULL == bmp || retroflat_screen_buffer() == bmp ) {
      /* Trigger a screen refresh if this was a screen lock. */
      if( (HWND)NULL != g_retroflat_state->platform.window ) {
         InvalidateRect( g_retroflat_state->platform.window, 0, TRUE );
         UpdateWindow( g_retroflat_state->platform.window );
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
   } else {
      /* Update the transparency mask. */
      if( (HBITMAP)NULL != bmp->mask ) {
         retval = retroflat_bitmap_win_transparency( bmp,
            bmp->bmi.header.biWidth, bmp->bmi.header.biHeight );
         maug_cleanup_if_not_ok();
      }
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
   char filename_path[MAUG_PATH_SZ_MAX + 1] = { 0 };
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_API_WIN16 )
   char* buf = NULL;
   FILE* bmp_file = NULL;
   long int i, x, y, w, h, colors, offset, sz, read;
#  elif !defined( RETROFLAT_OPENGL )
   BITMAP bm;
#  endif /* RETROFLAT_API_WIN32 */
#  ifdef MAUG_WCHAR
   wchar_t filename_path_w[MAUG_PATH_SZ_MAX + 1] = { 0 };
#  endif /* MAUG_WCHAR */

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, MAUG_PATH_SZ_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

#  ifdef RETROFLAT_OPENGL

   assert( NULL != bmp_out );
   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_load_bitmap()!" );
   /*
   retval = retro3d_texture_load_bitmap(
      filename_path, &(bmp_out->tex), flags );
   */

#  elif defined( RETROFLAT_API_WIN16 )

   /* Win16 has a bunch of extra involved steps for getting a bitmap from
    * disk. These cause a crash in Win32.
    */

   /* == Win16 == */

   /* Load the bitmap file from disk. */

   /* This uses the native API because mfile is still wonky when reading large
    * buffers and this will only ever be called from win16 on x86, right?
    */

   bmp_file = fopen( filename_path, "rb" );
   maug_cleanup_if_null_file( bmp_file );
   fseek( bmp_file, 0, SEEK_END );
   sz = ftell( bmp_file );
   fseek( bmp_file, 0, SEEK_SET );

   buf = calloc( sz, 1 );
   maug_cleanup_if_null_alloc( char*, buf );

   read = fread( buf, 1, sz, bmp_file );
   assert( read == sz );

   offset = *((unsigned long*)&(buf[10]));
   colors = *((int*)&(buf[46]));

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

#  else

   /* Win32 greatly simplifies the loading portion. */

   /* == Win32 == */

#        ifdef MAUG_WCHAR
   if( 0 == MultiByteToWideChar(
      CP_ACP, MB_PRECOMPOSED, filename_path, -1, filename_path_w,
      MAUG_PATH_SZ_MAX
   ) ) {
      error_printf( "could not create wide filename path!" );
      retval = MERROR_FILE;
      goto cleanup;
   }
#        endif /* MAUG_WCHAR */

#        ifdef RETROFLAT_API_WINCE
   bmp_out->b = SHLoadDIBitmap( filename_path_w );
#        elif defined( MAUG_WCHAR )
   bmp_out->b = LoadImageA(
      NULL, filename_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
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

#  endif /* RETROFLAT_API_WIN16 */

#  ifndef RETROFLAT_OPENGL
   /* The transparency portion is the same for Win32 and Win16. */
   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      /* Setup bitmap transparency mask. */
      debug_printf( 1, "creating new transparency mask bitmap..." );
      bmp_out->mask = CreateBitmap(
         bmp_out->bmi.header.biWidth,
         bmp_out->bmi.header.biHeight,
         1, 1, NULL );
      maug_cleanup_if_null( HBITMAP, bmp_out->mask, RETROFLAT_ERROR_BITMAP );

      /* Update transparency mask. */
      retval = retroflat_bitmap_win_transparency( bmp_out,
         bmp_out->bmi.header.biWidth, bmp_out->bmi.header.biHeight );
      maug_cleanup_if_not_ok();
   }
#  endif /* !RETROFLAT_OPENGL */

cleanup:

#  ifdef RETROFLAT_API_WIN16
   if( NULL != buf ) {
      free( buf );
   }

   fclose( bmp_file );

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
#     ifndef RETROFLAT_API_WINCE
   PALETTEENTRY palette[RETROFLAT_BMP_COLORS_SZ_MAX];
#     endif /* !RETROFLAT_API_WINCE */
#  endif /* !RETROFLAT_OPENGL */

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

#  if defined( RETROFLAT_OPENGL )

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_create_bitmap()!" );
   /*
   retval = retro3d_texture_create( w, h, &(bmp_out->tex), flags );
   */

#  else

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

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
         retval = MERROR_GUI;
         goto cleanup;
      }
   }
#     endif /* RETROFLAT_WING */

   debug_printf( 0, "creating bitmap..." );

   bmp_out->bmi.header.biWidth = w;
#     ifdef RETROFLAT_WING
   bmp_out->bmi.header.biHeight *= h;
   if(
      RETROFLAT_FLAGS_SCREEN_BUFFER != (RETROFLAT_FLAGS_SCREEN_BUFFER & flags)
   ) {
#     else
   bmp_out->bmi.header.biHeight = h;
#     endif /* RETROFLAT_WING */
   bmp_out->bmi.header.biSize = sizeof( BITMAPINFOHEADER );
   bmp_out->bmi.header.biPlanes = 1;
   bmp_out->bmi.header.biCompression = BI_RGB;
   bmp_out->bmi.header.biBitCount = 32;
   bmp_out->bmi.header.biSizeImage = w * h * 4;
#     ifdef RETROFLAT_WING
   }
#endif /* RETROFLAT_WING */

#ifndef RETROFLAT_API_WINCE
   /* TODO: How should we setup the palette in Windows CE? */
   GetSystemPaletteEntries(
      g_retroflat_state->platform.hdc_win, 0,
      RETROFLAT_BMP_COLORS_SZ_MAX, palette );
   for( i = 0 ; RETROFLAT_BMP_COLORS_SZ_MAX > i ; i++ ) {
      bmp_out->bmi.colors[i].rgbRed = palette[i].peRed;
      bmp_out->bmi.colors[i].rgbGreen = palette[i].peGreen;
      bmp_out->bmi.colors[i].rgbBlue = palette[i].peBlue;
      bmp_out->bmi.colors[i].rgbReserved = 0;
   }
#endif /* !RETROFLAT_API_WINCE */

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
      assert( NULL != bmp_out->b );

      debug_printf( 1, "WinG bitmap bits: %p", bmp_out->bits );

      bmp_out->flags |= RETROFLAT_FLAGS_OPAQUE;

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

   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      /* Setup bitmap transparency mask. */
      debug_printf( 1, "creating new transparency mask bitmap..." );
      bmp_out->mask = CreateBitmap( w, h, 1, 1, NULL );
      maug_cleanup_if_null( HBITMAP, bmp_out->mask, RETROFLAT_ERROR_BITMAP );
   } else {
      bmp_out->flags |= RETROFLAT_FLAGS_OPAQUE;
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

   if( retroflat_bitmap_has_flags( bmp, RETROFLAT_FLAGS_BITMAP_RO ) ) {
      return;
   }

#  if defined( RETROFLAT_OPENGL )

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_destroy_bitmap()!" );
   /*
   retro3d_texture_destroy( &(bmp->tex) );
   */

#  else

   /* == Win16/Win32 == */

   if( (HBITMAP)NULL != bmp->old_hbm_b ) {
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

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = 0;
#  ifndef RETROFLAT_OPENGL
   int locked_src_internal = 0;
#  endif /* RETROFLAT_OPENGL */

   assert( NULL != src );

#if 0
   /* Win API not setup for hardware scrolling. */
   if(
      0 > d_x || 0 > d_y ||
      retroflat_bitmap_w( target ) + w <= d_x ||
      retroflat_bitmap_h( target ) + h <= d_y
   ) {
      return;
   }
#endif

   if( retroflat_bitmap_has_flags( target, RETROFLAT_FLAGS_BITMAP_RO ) ) {
      return MERROR_GUI;
   }

#  if defined( RETROFLAT_OPENGL )

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_blit_bitmap()!" );
   /*
   retval = retro3d_texture_blit(
      &(target->tex), &(src->tex), s_x, s_y, d_x, d_y, w, h, instance );
   */

#  else

   /* == Win16/Win32 == */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   assert( !retroflat_bitmap_locked( src ) );
   assert( retroflat_bitmap_locked( target ) );

   retroflat_draw_lock( src );

   if( (HBITMAP)NULL == target->b ) {
      error_printf( "invalid bitmap blit target!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   if( (HBITMAP)NULL == src->b ) {
      error_printf( "invalid bitmap blit source!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   if( (HBITMAP)NULL != src->mask ) {
      /* Use mask to blit transparency. */
      if(
         !BitBlt(
            target->hdc_b, d_x, d_y, w, h, src->hdc_mask, s_x, s_y, SRCAND )
      ) {
         error_printf(
            "error during mask blit from bitmap %p to bitmap %p",
            src, target );
         retval = MERROR_GUI;
         goto cleanup;
      }

      /* Do actual blit. */
      if(
         !BitBlt(
            target->hdc_b, d_x, d_y, w, h, src->hdc_b, s_x, s_y, SRCPAINT )
      ) {
         error_printf(
            "error during blit from bitmap %p to bitmap %p",
            src, target );
         retval = MERROR_GUI;
         goto cleanup;
      }
   } else {
      /* Just overwrite entire rect. */
      if(
         !BitBlt(
            target->hdc_b, d_x, d_y, w, h, src->hdc_b, s_x, s_y, SRCCOPY )
      ) {
         error_printf(
            "error during blit from bitmap %p to bitmap %p",
            src, target );
         /* retval = MERROR_GUI; */
         goto cleanup;
      }
   }

cleanup:

   retroflat_draw_release( src );

#  endif /* RETROFLAT_OPENGL */
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

   if( retroflat_bitmap_has_flags( target, RETROFLAT_FLAGS_BITMAP_RO ) ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   retroflat_constrain_px( x, y, target, return );

#  if defined( RETROFLAT_OPENGL )

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_px()!" );
   /*
   retro3d_texture_px( &(target->tex), color_idx, x, y, flags );
   */

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( retroflat_bitmap_locked( target ) );

#  ifdef RETROFLAT_WING
   if( NULL != target->bits ) {
      /* Modify target bits directly (faster) if available! */
      /* WinG bitmaps are 8-bit palettized, so use the index directly. */
      if( 0 > retroflat_bitmap_h( target ) ) {
         /* TODO: Are these h/w used correctly? */
         target->bits[((retroflat_bitmap_h( target ) - 1 - y) * retroflat_bitmap_w( target )) + x] =
            g_retroflat_state->palette[color_idx];
      } else {
         target->bits[(y * retroflat_bitmap_w( target )) + x] =
            g_retroflat_state->palette[color_idx];
      }
   } else {
      /* Use slow Windows GDI. */
      SetPixel( target->hdc_b, x, y,
         g_retroflat_state->palette[color_idx] );
   }
#  else
   SetPixel( target->hdc_b, x, y,
      g_retroflat_state->palette[color_idx] );
#  endif /* RETROFLAT_WING */

#  endif /* RETROFLAT_OPENGL */
}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {
#  if !defined( RETROFLAT_OPENGL )
   HBRUSH old_brush = (HBRUSH)NULL;
   HPEN old_pen = (HPEN)NULL;
#  endif /* RETROFLAT_OPENGL */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_FLAGS_BITMAP_RO ) ) {
      return;
   }

#  if defined( RETROFLAT_OPENGL )

   assert( NULL != target );

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_rect()!" );
   /* Draw the rect onto the given 2D texture. */
   /*
   retrosoft_rect( target, color_idx, x, y, w, h, flags );
   */

#  else

   /* == Win16/Win32 == */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   assert( (HBITMAP)NULL != target->b );

   assert( retroflat_bitmap_locked( target ) );

   retroflat_win_setup_brush( old_brush, target, color_idx, flags );
   retroflat_win_setup_pen( old_pen, target, color_idx, flags );

   Rectangle( target->hdc_b, x, y, x + w, y + h );

/* cleanup: */

   retroflat_win_cleanup_brush( old_brush, target )
   retroflat_win_cleanup_pen( old_pen, target )

#  endif /* RETROFLAT_OPENGL */

}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t flags
) {
#  if !defined( RETROFLAT_OPENGL )
   HPEN pen = (HPEN)NULL;
   HPEN old_pen = (HPEN)NULL;
   POINT points[2];
#  endif /* !RETROFLAT_OPENGL */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_FLAGS_BITMAP_RO ) ) {
      return;
   }

#  if defined( RETROFLAT_OPENGL )

   assert( NULL != target );

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_line()!" );
   /*
   retrosoft_line( target, color_idx, x1, y1, x2, y2, flags );
   */

#  else

   /* == Win16/Win32 == */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   assert( (HBITMAP)NULL != target->b );

   assert( retroflat_bitmap_locked( target ) );

   retroflat_win_setup_pen( old_pen, target, color_idx, flags );

   /* Create the line points. */
   points[0].x = x1;
   points[0].y = y1;
   points[1].x = x2;
   points[1].y = y2;

   Polyline( target->hdc_b, points, 2 );

/* cleanup: */

   retroflat_win_cleanup_pen( old_pen, target )

#  endif /* RETROFLAT_OPENGL */
}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {
#  if !defined( RETROFLAT_OPENGL )
   HPEN old_pen = (HPEN)NULL;
   HBRUSH old_brush = (HBRUSH)NULL;
#  endif /* !RETROFLAT_OPENGL */

   if( RETROFLAT_COLOR_NULL == color ) {
      return;
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_FLAGS_BITMAP_RO ) ) {
      return;
   }

#  if defined( RETROFLAT_OPENGL )

   assert( NULL != target );

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_ellipse()!" );
   /*
   retrosoft_ellipse( target, color, x, y, w, h, flags );
   */

#  else

   /* == Win16/Win32 == */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   assert( (HBITMAP)NULL != target->b );
   assert( retroflat_bitmap_locked( target ) );

   retroflat_win_setup_brush( old_brush, target, color, flags );
   retroflat_win_setup_pen( old_pen, target, color, flags );

   Ellipse( target->hdc_b, x, y, x + w, y + h );

/* cleanup: */

   retroflat_win_cleanup_brush( old_brush, target )
   retroflat_win_cleanup_pen( old_pen, target )

#  endif /* RETROFLAT_OPENGL */
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
   /* Platform does not support resizing. */
}

/* === */

#endif /* !RETPLTF_H */

