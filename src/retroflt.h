
#ifndef RETROFLT_H
#define RETROFLT_H

/**
 * \file retroflt.h
 */

/* === Generic Includes and Defines === */

#define RETROFLAT_OK             0x00
#define RETROFLAT_ERROR_ENGINE   0x01
#define RETROFLAT_ERROR_GRAPHICS 0x02
#define RETROFLAT_ERROR_MOUSE    0x04
#define RETROFLAT_ERROR_BITMAP   0x08
#define RETROFLAT_ERROR_TIMER    0x0f

#define RETROFLAT_FLAGS_FILL     0x01

#define RETROFLAT_FLAGS_RUNNING  0x01

#ifdef DEBUG
#include <assert.h>
#else
#define assert( x )
#endif /* DEBUG */

#define RETROFLAT_BITMAP_EXT "bmp"

typedef void (*retroflat_loop_iter)(void* data);

struct RETROFLAT_INPUT {
   int mouse_x;
   int mouse_y;
};

#ifndef RETROFLAT_LINE_THICKNESS
/*! \brief Line drawing thickness (only works on some platforms). */
#define RETROFLAT_LINE_THICKNESS 1
#endif /* !RETROFLAT_LINE_THICKNESS */

#ifndef RETROFLAT_FPS
/* TODO: Support Allegro! */
/*! \brief Target FPS. */
#define RETROFLAT_FPS 15
#endif /* !RETROFLAT_FPS */

#ifndef RETROFLAT_WINDOW_CLASS
/*! \brief Unique window class to use on some platforms (e.g. Win32). */
#define RETROFLAT_WINDOW_CLASS "RetroFlatWindowClass"
#endif /* !RETROFLAT_WINDOW_CLASS */

#ifndef RETROFLAT_WIN_GFX_TIMER_ID
/**
 * \brief Unique ID for the timer that execute graphics ticks in Win16/Win32.
 */
#define RETROFLAT_WIN_GFX_TIMER_ID 6001
#endif /* !RETROFLAT_WIN_GFX_TIMER_ID */

#ifndef RETROFLAT_MSG_MAX
#define RETROFLAT_MSG_MAX 4096
#endif /* RETROFLAT_MSG_MAX */

/* === Platform-Specific Includes and Defines === */

#if defined( RETROFLAT_API_ALLEGRO )

/* == Allegro == */

#  include <allegro.h>
struct RETROFLAT_BITMAP {
   BITMAP* b;
};
typedef int RETROFLAT_COLOR;
#  define RETROFLAT_COLOR_BLACK     makecol( 0, 0, 0 )
#  define RETROFLAT_COLOR_WHITE     makecol( 255, 255, 255 )
#  define RETROFLAT_COLOR_GRAY      makecol( 128, 128, 128 )

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)

#  define retroflat_screen_w() SCREEN_W
#  define retroflat_screen_h() SCREEN_H

/* TODO: Handle retval. */
#  define retroflat_quit( retval ) \
   g_retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING;

#  define retroflat_loop( loop_iter, data ) \
   g_retroflat_flags |= RETROFLAT_FLAGS_RUNNING; \
   do { \
      loop_iter( data ); \
   } while( \
      RETROFLAT_FLAGS_RUNNING == (RETROFLAT_FLAGS_RUNNING & g_retroflat_flags) \
   );

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

#  define RETROFLAT_KEY_UP	KEY_UP
#  define RETROFLAT_KEY_DOWN	KEY_DOWN
#  define RETROFLAT_KEY_RIGHT	KEY_RIGHT
#  define RETROFLAT_KEY_LEFT	KEY_LEFT
#  define RETROFLAT_KEY_A	KEY_A
#  define RETROFLAT_KEY_B	KEY_B
#  define RETROFLAT_KEY_C	KEY_C
#  define RETROFLAT_KEY_D	KEY_D
#  define RETROFLAT_KEY_E	KEY_E
#  define RETROFLAT_KEY_F	KEY_F
#  define RETROFLAT_KEY_G	KEY_G
#  define RETROFLAT_KEY_H	KEY_H
#  define RETROFLAT_KEY_I	KEY_I
#  define RETROFLAT_KEY_J	KEY_J
#  define RETROFLAT_KEY_K	KEY_K
#  define RETROFLAT_KEY_L	KEY_L
#  define RETROFLAT_KEY_M	KEY_M
#  define RETROFLAT_KEY_N	KEY_N
#  define RETROFLAT_KEY_O	KEY_O
#  define RETROFLAT_KEY_P	KEY_P
#  define RETROFLAT_KEY_Q	KEY_Q
#  define RETROFLAT_KEY_R	KEY_R
#  define RETROFLAT_KEY_S	KEY_S
#  define RETROFLAT_KEY_T	KEY_T
#  define RETROFLAT_KEY_U	KEY_U
#  define RETROFLAT_KEY_V	KEY_V
#  define RETROFLAT_KEY_W	KEY_W
#  define RETROFLAT_KEY_X	KEY_X
#  define RETROFLAT_KEY_Y	KEY_Y
#  define RETROFLAT_KEY_Z	KEY_Z
#  define RETROFLAT_KEY_0  KEY_0
#  define RETROFLAT_KEY_1  KEY_1
#  define RETROFLAT_KEY_2  KEY_2
#  define RETROFLAT_KEY_3  KEY_3
#  define RETROFLAT_KEY_4  KEY_4
#  define RETROFLAT_KEY_5  KEY_5
#  define RETROFLAT_KEY_6  KEY_6
#  define RETROFLAT_KEY_7  KEY_7
#  define RETROFLAT_KEY_8  KEY_8
#  define RETROFLAT_KEY_9  KEY_9
#  define RETROFLAT_KEY_TAB	KEY_TAB
#  define RETROFLAT_KEY_SPACE	KEY_SPACE
#  define RETROFLAT_KEY_ESC	KEY_ESC
#  define RETROFLAT_KEY_ENTER	KEY_ENTER
#  define RETROFLAT_KEY_HOME	KEY_HOME
#  define RETROFLAT_KEY_END	KEY_END

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

/* == Win16/Win32 == */

#  include <windows.h>
struct RETROFLAT_BITMAP {
   HBITMAP b;
   HBITMAP mask;
};
typedef COLORREF RETROFLAT_COLOR;
#  define RETROFLAT_COLOR_BLACK        RGB(0,   0,   0)
#  define RETROFLAT_COLOR_DARKBLUE     RGB(0, 0, 170)
#  define RETROFLAT_COLOR_DARKGREEN    RGB(0, 170, 0)
#  define RETROFLAT_COLOR_TEAL         RGB(0, 170, 170)
#  define RETROFLAT_COLOR_DARKRED      RGB(170, 0, 0)
#  define RETROFLAT_COLOR_VIOLET       RGB(170, 0, 170)
#  define RETROFLAT_COLOR_BROWN        RGB(170, 85, 0)
#  define RETROFLAT_COLOR_GRAY         RGB(170, 170, 170)
#  define RETROFLAT_COLOR_DARKGRAY     RGB(85, 85, 85)
#  define RETROFLAT_COLOR_BLUE         RGB(85, 85, 255)
#  define RETROFLAT_COLOR_GREEN        RGB(85, 255, 85)
#  define RETROFLAT_COLOR_CYAN         RGB(85, 255, 255)
#  define RETROFLAT_COLOR_RED          RGB(255, 85, 85)
#  define RETROFLAT_COLOR_MAGENTA      RGB(255, 85, 255)
#  define RETROFLAT_COLOR_YELLOW       RGB(255, 255, 85)
#  define RETROFLAT_COLOR_WHITE        RGB(255, 255, 255)

/* Transparency background color: black by default, to match Allegro. */
#  ifndef RETROFLAT_TXP_R
#  define RETROFLAT_TXP_R 0x00
#  endif /* !RETROFLAT_TXP_R */

#  ifndef RETROFLAT_TXP_G
#  define RETROFLAT_TXP_G 0x00
#  endif /* !RETROFLAT_TXP_G */

#  ifndef RETROFLAT_TXP_B
#  define RETROFLAT_TXP_B 0x00
#  endif /* !RETROFLAT_TXP_B */

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_screen_w() g_screen_v_w
#  define retroflat_screen_h() g_screen_v_h
#  define retroflat_quit( retval ) PostQuitMessage( retval );

#  define retroflat_loop( iter, data ) \
   g_loop_iter = (retroflat_loop_iter)iter; \
   g_loop_data = (void*)data; \
   do { \
      g_msg_retval = GetMessage( &g_msg, 0, 0, 0 ); \
      TranslateMessage( &g_msg ); \
      DispatchMessage( &g_msg ); \
   } while( 0 < g_msg_retval );

#  define retroflat_bmp_int( type, buf, offset ) *((type*)&(buf[offset]))

#  define RETROFLAT_KEY_UP	   VK_UP
#  define RETROFLAT_KEY_DOWN	VK_DOWN
#  define RETROFLAT_KEY_RIGHT	VK_RIGHT
#  define RETROFLAT_KEY_LEFT	VK_LEFT
#  define RETROFLAT_KEY_A	   0x41
#  define RETROFLAT_KEY_B	   0x42
#  define RETROFLAT_KEY_C	   0x43
#  define RETROFLAT_KEY_D	   0x44
#  define RETROFLAT_KEY_E	   0x45
#  define RETROFLAT_KEY_F	   0x46
#  define RETROFLAT_KEY_G	   0x47
#  define RETROFLAT_KEY_H	   0x48
#  define RETROFLAT_KEY_I	   0x49
#  define RETROFLAT_KEY_J	   0x4a
#  define RETROFLAT_KEY_K	   0x4b
#  define RETROFLAT_KEY_L	   0x4c
#  define RETROFLAT_KEY_M	   0x4d
#  define RETROFLAT_KEY_N	   0x4e
#  define RETROFLAT_KEY_O	   0x4f
#  define RETROFLAT_KEY_P	   0x50
#  define RETROFLAT_KEY_Q	   0x51
#  define RETROFLAT_KEY_R	   0x52
#  define RETROFLAT_KEY_S	   0x53
#  define RETROFLAT_KEY_T	   0x54
#  define RETROFLAT_KEY_U	   0x55
#  define RETROFLAT_KEY_V	   0x56
#  define RETROFLAT_KEY_W	   0x57
#  define RETROFLAT_KEY_X	   0x58
#  define RETROFLAT_KEY_Y	   0x59
#  define RETROFLAT_KEY_Z	   0x60
#  define RETROFLAT_KEY_0     0x30
#  define RETROFLAT_KEY_1     0x31
#  define RETROFLAT_KEY_2     0x32
#  define RETROFLAT_KEY_3     0x33
#  define RETROFLAT_KEY_4     0x34
#  define RETROFLAT_KEY_5     0x35
#  define RETROFLAT_KEY_6     0x36
#  define RETROFLAT_KEY_7     0x37
#  define RETROFLAT_KEY_8     0x38
#  define RETROFLAT_KEY_9     0x39
#  define RETROFLAT_KEY_TAB	VK_TAB
#  define RETROFLAT_KEY_SPACE	VK_SPACE
#  define RETROFLAT_KEY_ESC	VK_ESC
#  define RETROFLAT_KEY_ENTER	VK_ENTER
#  define RETROFLAT_KEY_HOME	VK_HOME
#  define RETROFLAT_KEY_END	VK_END

#  define RETROFLAT_MOUSE_B_LEFT    VK_LBUTTON
#  define RETROFLAT_MOUSE_B_RIGHT   VK_RBUTTON

/* Set the calling convention for WinMain, depending on Win16/Win32. */
#  if defined( RETROFLAT_API_WIN16 )
#     define WINXAPI PASCAL
#  elif defined( RETROFLAT_API_WIN32 )
#     define WINXAPI WINAPI
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* TODO: Verify this on multi-monitor Win32 systems. */
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

/* Improvise a rough WinMain to call main(). */
#define END_OF_MAIN() \
   int WINXAPI WinMain( \
      HINSTANCE hInstance, HINSTANCE hPrevInstance, \
      LPSTR lpCmdLine, int nCmdShow \
   ) { \
      g_instance = hInstance; \
      g_cmd_show = nCmdShow; \
      return main(); \
   }

#endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#ifdef RETROFLAT_OS_DOS
#  define RETROFLAT_PATH_SEP '\\'
#else
#  define RETROFLAT_PATH_SEP '/'
#endif /* RETROFLAT_OS_DOS */

/*! \brief Maximum size of the assets path, to allow room for appending. */
#define RETROFLAT_ASSETS_PATH_MAX (PATH_MAX / 2)

/* === Translation Module === */

/* Declare the prototypes so that internal functions can call each other. */
void retroflat_message( const char* title, const char* format, ... );
int retroflat_init( const char* title, int screen_w, int screen_h );
void retroflat_shutdown( int retval );
void retroflat_set_assets_path( const char* path );
void retroflat_draw_lock();
void retroflat_draw_flip();
int retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out );
void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bitmap );
void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int w, int h );
void retroflat_rect(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags );
void retroflat_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, unsigned char flags );
int retroflat_poll_input();

#ifdef RETROFLT_C

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <stdarg.h>

retroflat_loop_iter g_loop_iter = NULL;
void* g_loop_data = NULL;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
/* Windows-specific global handles for the window/instance. */
HINSTANCE g_instance;
HWND g_window;
MSG g_msg;
int g_msg_retval = 0;
int g_screen_w = 0;
int g_screen_h = 0;
int g_screen_v_w = 0;
int g_screen_v_h = 0;
int g_cmd_show = 0;
const long gc_ms_target = 1000 / RETROFLAT_FPS;
static unsigned long g_ms_start = 0; 
volatile unsigned long g_ms;
unsigned char g_last_key = 0;
unsigned int g_last_mouse = 0;
unsigned int g_last_mouse_x = 0;
unsigned int g_last_mouse_y = 0;
unsigned char g_running;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* === Globals === */

char g_retroflat_assets_path[RETROFLAT_ASSETS_PATH_MAX + 1];
struct RETROFLAT_BITMAP g_screen;
unsigned char g_retroflat_flags = 0;

/* === Function Definitions === */

#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

static LRESULT CALLBACK WndProc(
   HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
) {
   PAINTSTRUCT ps;
   HDC hdcScreen;
   HDC hdcBuffer;
   BITMAP srcBitmap;
   HBITMAP oldHbm;

   switch( message ) {
      case WM_CREATE:
         break;

      case WM_PAINT:

         /* Create HDC for screen. */
         hdcScreen = BeginPaint( hWnd, &ps );
         if( (HDC)NULL == hdcScreen ) {
            break;
         }

         if( !retroflat_bitmap_ok( &g_screen ) ) {
            g_screen.b = CreateCompatibleBitmap( hdcScreen,
               g_screen_v_w, g_screen_v_h );
         }
         if( (HBITMAP)NULL == g_screen.b ) {
            break;
         }

         /* Create a new HDC for buffer and select buffer into it. */
         hdcBuffer = CreateCompatibleDC( hdcScreen );
         if( (HDC)NULL == hdcBuffer ) {
            break;
         }
         oldHbm = SelectObject( hdcBuffer, g_screen.b );

         /* Load parameters of the buffer into info object (srcBitmap). */
         GetObject( g_screen.b, sizeof( BITMAP ), &srcBitmap );

         StretchBlt(
            hdcScreen,
            0, 0,
            g_screen_w, g_screen_h,
            hdcBuffer,
            0, 0,
            srcBitmap.bmWidth,
            srcBitmap.bmHeight,
            SRCCOPY
         );

         /* Return the default object into the HDC. */
         SelectObject( hdcScreen, oldHbm );

         DeleteDC( hdcBuffer );
         DeleteDC( hdcScreen );

         EndPaint( hWnd, &ps );
         break;

      case WM_ERASEBKGND:
         return 1;

      case WM_KEYDOWN:
         g_last_key = wParam;
         break;

      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
         g_last_mouse = wParam;
         g_last_mouse_x = GET_X_LPARAM( lParam );
         g_last_mouse_y = GET_Y_LPARAM( lParam );
         break;

      case WM_DESTROY:
         if( retroflat_bitmap_ok( &g_screen ) ) {
            DeleteObject( g_screen.b );
         }
         PostQuitMessage( 0 );
         break;

      case WM_TIMER:
         g_loop_iter( g_loop_data );

         /* Kind of a hack so that we can have a cheap timer. */
         g_ms += 100;
         break;

      default:
         return DefWindowProc( hWnd, message, wParam, lParam );
   }

   return 0;
}

#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

void retroflat_message( const char* title, const char* format, ... ) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   va_start( vargs, format );
   snprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );
   va_end( vargs );

#  if defined( RETROFLAT_API_ALLEGRO )
   allegro_message( "%s", msg_out );
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   MessageBox( NULL, msg_out, title, MB_OK | MB_TASKMODAL ); 
#  else
#     error "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

int retroflat_init( const char* title, int screen_w, int screen_h ) {
   int retval = 0;
#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   WNDCLASS wc = { 0 };
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  ifdef RETROFLAT_API_ALLEGRO

   /* TODO: Set window title. */

   if( allegro_init() ) {
      allegro_message( "could not setup allegro!" );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   install_keyboard();
#     ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
   install_timer();
#     endif /* RETROFLAT_MOUSE */

#     ifdef RETROFLAT_OS_DOS
   /* Don't try windowed mode in DOS. */
   if( set_gfx_mode( GFX_AUTODETECT, screen_w, screen_h, 0, 0 ) ) {
#     else
   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, screen_w, screen_h, 0, 0 ) ) {
#     endif /* RETROFLAT_OS_DOS */

      allegro_message( "could not setup graphics!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#     ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
   if( 0 > install_mouse() ) {
      allegro_message( "could not setup mouse!" );
      retval = RETROFLAT_ERROR_MOUSE;
      goto cleanup;
   }
#     endif /* RETROFLAT_MOUSE */

   g_screen.b = create_bitmap( screen_w, screen_h );
   if( NULL == g_screen.b ) {
      allegro_message( "unable to allocate screen buffer!" );
      goto cleanup;
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   g_screen_w = screen_w;
   g_screen_h = screen_h;
   g_screen_v_w = screen_w;
   g_screen_v_h = screen_h;

   memset( &g_screen, '\0', sizeof( struct RETROFLAT_BITMAP ) );
   memset( &wc, '\0', sizeof( WNDCLASS ) );

   wc.lpfnWndProc   = (WNDPROC)&WndProc;
   wc.hInstance     = g_instance;
   /* TODO: Window icon. */
   /*
   wc.hIcon         = LoadIcon( g_instance, MAKEINTRESOURCE(
      g_graphics_args.icon_res ) );
   */
   wc.hCursor       = LoadCursor( 0, IDC_ARROW );
   wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );
   /* wc.lpszMenuName  = MAKEINTRESOURCE( IDR_MAINMENU ); */
   wc.lpszClassName = RETROFLAT_WINDOW_CLASS;

   if( !RegisterClass( &wc ) ) {
      retroflat_message( "Error", "Could not register window class!" );
      goto cleanup;
   }

   g_window = CreateWindowEx(
      0, RETROFLAT_WINDOW_CLASS, title, 
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT,
      g_screen_w, g_screen_h, 0, 0, g_instance, 0
   );

   if( !g_window ) {
      retroflat_message( "Error", "Could not create window!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

   assert( (HWND)NULL != g_window );

   if( !SetTimer(
      g_window, RETROFLAT_WIN_GFX_TIMER_ID, (int)(1000 / RETROFLAT_FPS), NULL )
   ) {
      retroflat_message( "Error", "Could not create graphics timer!" );
      retval = RETROFLAT_ERROR_TIMER;
      goto cleanup;
   }

   ShowWindow( g_window, g_cmd_show );

#  else
#     error "not implemented"
#  endif  /* RETROFLAT_API_ALLEGRO */

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown( int retval ) {

#ifdef RETROFLAT_API_ALLEGRO
   if( RETROFLAT_ERROR_ENGINE != retval ) {
      clear_keybuf();
   }

   retroflat_destroy_bitmap( &g_screen );
#endif /* RETROFLAT_API_ALLEGRO */

}

/* === */

void retroflat_set_assets_path( const char* path ) {
   memset( g_retroflat_assets_path, '\0', RETROFLAT_ASSETS_PATH_MAX );
   strncpy( g_retroflat_assets_path, path, RETROFLAT_ASSETS_PATH_MAX );
}

/* === */

void retroflat_draw_lock() {

#  if defined( RETROFLAT_API_ALLEGRO )
#     ifdef RETROFLAT_MOUSE
      /* XXX: Broken in DOS. */
   show_mouse( NULL ); /* Disable mouse before drawing. */
#     endif
   acquire_screen();
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   /* Do nothing. */
#  else
#     error "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */

}

/* === */

void retroflat_draw_flip() {
#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   blit( g_screen.b, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
   release_screen();
#     ifdef RETROFLAT_MOUSE
      /* XXX: Broken in DOS. */
   show_mouse( screen ); /* Enable mouse after drawing. */
#     endif /* RETROFLAT_MOUSE */
   vsync();

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   /* The closest analog to the actual graphics_flip(): */
   if( (HWND)NULL != g_window ) {
      InvalidateRect( g_window, 0, TRUE );
   }

#  else
#     error "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */
}

/* === */

int retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out
) {
   char filename_path[PATH_MAX + 1];
   int retval = 0;
#if defined( RETROFLAT_API_WIN16 ) || defined (RETROFLAT_API_WIN32 )
   HDC hdc_win = (HDC)NULL;
   HDC hdc_mask = (HDC)NULL;
   HDC hdc_bmp = (HDC)NULL;
   long i, x, y, w, h, bpp, offset, sz, read;
   char* buf = NULL;
   BITMAPINFO* bmi = NULL;
   FILE* bmp_file = NULL;
   HBITMAP old_hbm_mask = (HBITMAP)NULL;
   HBITMAP old_hbm_bmp = (HBITMAP)NULL;
   unsigned long txp_color = 0;
   BITMAP bm;
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   assert( NULL != bmp_out );

   /* Build the path to the bitmap. */
   memset( filename_path, '\0', PATH_MAX + 1 );
   snprintf( filename_path, PATH_MAX, "%s%c%s.%s",
      g_retroflat_assets_path, RETROFLAT_PATH_SEP,
      filename, RETROFLAT_BITMAP_EXT );

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

   bmp_out->b = load_bitmap( filename_path, NULL );

   if( NULL != bmp_out->b ) {
      retval = RETROFLAT_OK;
   } else {
      allegro_message( "unable to load %s", filename_path );
      retval = RETROFLAT_ERROR_BITMAP;
   }

#  elif defined( RETROFLAT_API_WIN16 )

   /* == Win16 == */

   /* Load the bitmap file from disk. */
   bmp_file = fopen( filename_path, "rb" );
   if( NULL == bmp_file ) {
      retroflat_message( "Error", "Could not load bitmap: %s", filename_path );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }
   fseek( bmp_file, 0, SEEK_END );
   sz = ftell( bmp_file );
   fseek( bmp_file, 0, SEEK_SET );
   read = fread( buf, sz, 1, bmp_file );
   assert( read == sz );

   bmi = (BITMAPINFO*)&(buf[sizeof( BITMAPFILEHEADER )]);

   bpp = retroflat_bmp_int( unsigned short, buf, 28 );
   offset = retroflat_bmp_int( unsigned long, buf, 10 );

   assert( 0 < bmi->bmiHeader.biWidth );
   assert( 0 < bmi->bmiHeader.biHeight );
   assert( 0 == bmi->bmiHeader.biWidth % 8 );
   assert( 0 == bmi->bmiHeader.biHeight % 8 );

   hdc_win = GetDC( g_window );
   bmp_out->b = CreateCompatibleBitmap( hdc_win,
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );

   SetDIBits( hdc_win, bmp_out->b, 0,
      bmi->bmiHeader.biHeight, &(buf[offset]), bmi,
      DIB_RGB_COLORS );

   /* Setup bitmap transparency mask. */
   bmp_out->mask = CreateBitmap(
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight, 1, 1, NULL );

   hdc_bmp = CreateCompatibleDC( 0 );
   hdc_mask = CreateCompatibleDC( 0 );

   old_hbm_bmp = SelectObject( hdc_bmp, bmp_out->b );
   old_hbm_mask = SelectObject( hdc_mask, bmp_out->mask );

   /* Convert the color key into bitmap format. */
   txp_color |= (RETROFLAT_TXP_B & 0xff);
   txp_color <<= 8;
   txp_color |= (RETROFLAT_TXP_G & 0xff);
   txp_color <<= 8;
   txp_color |= (RETROFLAT_TXP_R & 0xff);
   SetBkColor( hdc_bmp, txp_color );

   /* Create the mask from the color key. */
   BitBlt(
      hdc_mask, 0, 0,
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight,
      hdc_bmp, 0, 0, SRCCOPY );
   BitBlt(
      hdc_bmp, 0, 0,
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight,
      hdc_mask, 0, 0, SRCINVERT );

cleanup:

   if( NULL != buf ) {
      free( buf );
   }

   if( NULL != bmp_file ) {
      fclose( bmp_file );
   }

   if( (HDC)NULL != hdc_win ) {
      ReleaseDC( g_window, hdc_win );
   }

   if( (HDC)NULL != hdc_mask ) {
      SelectObject( hdc_mask, old_hbm_mask );
      DeleteDC( hdc_mask );
   }

   if( (HDC)NULL != hdc_bmp ) {
      SelectObject( hdc_bmp, old_hbm_bmp );
      DeleteDC( hdc_bmp );
   }

#  elif defined( RETROFLAT_API_WIN32 )
   
   /* == Win32 == */

   bmp_out->b = LoadImage( NULL, b->id, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

#  else
#     error "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bitmap ) {
#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( NULL == bitmap->b ) {
      return;
   }

   destroy_bitmap( bitmap->b );
   bitmap->b = NULL;

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16 == */

   if( NULL == bitmap->b ) {
      return;
   }

   DeleteObject( bitmap->b );
   bitmap->b = NULL;

#  else
#     error "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */
}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int w, int h
) {
#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HDC hdcBuffer = (HDC)NULL;
   HDC hdcSrc = (HDC)NULL;
   HBITMAP oldHbmSrc = (HBITMAP)NULL;
   HBITMAP oldHbmBuffer = (HBITMAP)NULL;
   HDC hdcSrcMask = (HDC)NULL;
   HBITMAP oldHbmSrcMask = (HBITMAP)NULL;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_screen);
   }
   assert( NULL != target->b );
   assert( NULL != src );
   assert( NULL != src->b );

#  if defined( RETROFLAT_API_ALLEGRO )
   
   /* == Allegro == */

   /* TODO: Handle partial blit. */
   draw_sprite( target->b, src->b, d_x, d_y );

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* Create HDC for the off-screen buffer to blit to. */
   hdcBuffer = CreateCompatibleDC( (HDC)NULL );
   if( (HDC)NULL == hdcBuffer ) {
      goto cleanup;
   }
   oldHbmBuffer = SelectObject( hdcBuffer, target->b );

   /* Create HDC for source bitmap compatible with the buffer. */
   hdcSrc = CreateCompatibleDC( (HDC)NULL );
   if( (HDC)NULL == hdcSrc ) {
      goto cleanup;
   }
   oldHbmSrc = SelectObject( hdcSrc, src->b );

   /* Create HDC for source bitmap mask compatible with the buffer. */
   hdcSrcMask = CreateCompatibleDC( (HDC)NULL );
   if( (HDC)NULL == hdcSrcMask ) {
      goto cleanup;
   }
   oldHbmSrcMask = SelectObject( hdcSrcMask, src->mask );

   /* Use mask to blit transparency. */

   BitBlt(
      hdcBuffer,
      d_x, d_y, w, h,
      hdcSrcMask,
      s_x, s_y,
      SRCAND
   );

   BitBlt(
      hdcBuffer,
      d_x, d_y, w, h,
      hdcSrc,
      s_x, s_y,
      SRCPAINT
   );

cleanup:

   if( (HDC)NULL != hdcSrcMask ) {
      SelectObject( hdcSrcMask, oldHbmSrcMask );
      DeleteDC( hdcSrcMask );
   }

   if( (HDC)NULL != hdcSrc ) {
      SelectObject( hdcSrc, oldHbmSrc );
      DeleteDC( hdcSrc );
   }

   if( (HDC)NULL != hdcBuffer ) {
      SelectObject( hdcBuffer, oldHbmBuffer );
      DeleteDC( hdcBuffer );
   }

#  else
#     error "not implimented"
#  endif /* RETROFLAT_API_ALLEGRO */
}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags
) {
#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HDC hdcBuffer = (HDC)NULL;
   HBITMAP oldHbmBuffer = (HBITMAP)NULL;
   RECT rect;
   HBRUSH brush = (HBRUSH)NULL;
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_screen);
   }
   assert( NULL != target->b );

#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      rectfill( target->b, x, y, w, h, color );
   } else {
      rect( target->b, x, y, w, h, color );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   rect.left = x;
   rect.top = y;
   rect.right = x + w;
   rect.bottom = y + h;

   /* TODO: Apply thickness? */

   /* Create HDC for the off-screen buffer to blit to. */
   hdcBuffer = CreateCompatibleDC( (HDC)NULL );
   if( (HDC)NULL == hdcBuffer ) {
      goto cleanup;
   }
   oldHbmBuffer = SelectObject( hdcBuffer, target->b );

   brush = CreateSolidBrush( color );
   if( (HBRUSH)NULL == brush ) {
      goto cleanup;
   }

   FillRect( hdcBuffer, &rect, brush );

cleanup:

   if( (HBRUSH)NULL != brush ) {
      DeleteObject( brush );
   }

   if( (HDC)NULL != hdcBuffer ) {
      /* Reselect the initial objects into the provided DCs. */
      SelectObject( hdcBuffer, oldHbmBuffer );
      DeleteDC( hdcBuffer );
   }

#  else
#     error "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, unsigned char flags
) {
#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HDC hdcBuffer = (HDC)NULL;
   HBITMAP oldHbmBuffer = (HBITMAP)NULL;
   HPEN pen = (HPEN)NULL;
   HPEN oldPen = (HPEN)NULL;
   POINT points[2];
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_screen);
   }
   assert( NULL != target->b );

#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   line( target->b, x1, y1, x2, y2, color );

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   /* Create HDC for the off-screen buffer to blit to. */
   hdcBuffer = CreateCompatibleDC( (HDC)NULL );
   oldHbmBuffer = SelectObject( hdcBuffer, target->b );

   pen = CreatePen( PS_SOLID, RETROFLAT_LINE_THICKNESS, color );
   if( (HPEN)NULL == pen ) {
      goto cleanup;
   }
   oldPen = SelectObject( hdcBuffer, pen );
   points[0].x = x1;
   points[0].y = y1;
   points[1].x = x2;
   points[1].y = y2;
   Polyline( hdcBuffer, points, 2 );
   SelectObject( hdcBuffer, oldPen );
   DeleteObject( pen );

cleanup:

   /* Reselect the initial objects into the provided DCs. */
   if( NULL != hdcBuffer ) {
      SelectObject( hdcBuffer, oldHbmBuffer );
      DeleteDC( hdcBuffer );
   }

#  else
#     error "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

int retroflat_poll_input( struct RETROFLAT_INPUT* input ) {

#  ifdef RETROFLAT_API_ALLEGRO
#     ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
   poll_mouse();
   if( mouse_b & 0x01 ) {
      input->mouse_x = mouse_x;
      input->mouse_y = mouse_y;
      return RETROFLAT_MOUSE_B_LEFT;
   } else if( mouse_b & 0x02 ) {
      input->mouse_x = mouse_x;
      input->mouse_y = mouse_y;
      return RETROFLAT_MOUSE_B_RIGHT;
   }
#     endif /* RETROFLAT_MOUSE */

   poll_keyboard();
   if( keypressed() ) {
      return (readkey() >> 8);
   }

   return 0;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int key_out = 0;

   if( g_last_key ) {
      /* Return g_last_key, which is set in WndProc when a keypress msg is
      * received.
      */
      key_out = g_last_key;

      /* Reset pressed key. */
      g_last_key = 0;

   } else if( g_last_mouse ) {
      if( MK_LBUTTON == (MK_LBUTTON & g_last_mouse) ) {
         input->mouse_x = g_last_mouse_x;
         input->mouse_y = g_last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_LEFT;
      } else if( MK_RBUTTON == (MK_RBUTTON & g_last_mouse) ) {
         input->mouse_x = g_last_mouse_x;
         input->mouse_y = g_last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_RIGHT;
      }
      g_last_mouse = 0;
      g_last_mouse_x = 0;
      g_last_mouse_y = 0;
   }

   return key_out;
#  else
#     error "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */

}

#endif /* RETROFLT_C */

#endif /* RETROFLT_H */

