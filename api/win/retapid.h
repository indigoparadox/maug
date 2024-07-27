
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

/* Windows doesn't have hardware scrolling and on older systems GDI can be
 * slow, so minimize blitting!
 */
#define RETROFLAT_SOFT_VIEWPORT
#ifdef RETROFLAT_API_WIN16
#  define RETROFLAT_VIEWPORT_ADAPT
#endif

/* Windows uses its own message loop. */
#define RETROFLAT_NO_GENERIC_LOOP 1

#ifdef RETROFLAT_API_WINCE
/* WinCE is very similar to Win32, so we'll mostly use that with some exceptions below. */
#	define RETROFLAT_API_WIN32
#endif /* RETROFLAT_API_WINCE */

/*
#define RETROFLAT_VIEWPORT_OVERRIDE_MOVE

#define retroflat_viewport_move_x( x ) \
   retroflat_viewport_move_x_generic( \
      x >= (retroflat_screen_w() >> 1) ? x : 0 );

#define retroflat_viewport_move_y( y ) \
   retroflat_viewport_move_y_generic( \
      y >= (retroflat_screen_h() >> 1) ? y : 0 );
*/

#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
#  if !defined( RETROFLAT_WIN_STYLE )
#     if defined( RETROFLAT_API_WINCE )
#        define RETROFLAT_WIN_STYLE (WS_VISIBLE)
#     else
#        define RETROFLAT_WIN_STYLE (WS_OVERLAPPEDWINDOW)
#     endif /* RETROFLAT_API_WINCE */
#  endif /* !RETROFLAT_WIN_STYLE */
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* == Win16/Win32 == */

typedef int16_t RETROFLAT_IN_KEY;

#  if defined( RETROFLAT_API_WIN16 ) && defined( RETROFLAT_OPENGL )
#     error "opengl support not implemented for win16"
#  endif /* RETROFLAT_API_SDL2 && RETROFLAT_OPENGL */

#  if defined( RETROFLAT_OPENGL )
#     include <GL/gl.h>
#     include <GL/glu.h>
#  endif /* RETROFLAT_OPENGL */

#  define RETROFLAT_VDP_LIB_NAME "rvdpnt"

#  include <mmsystem.h>

#  ifndef RETROFLAT_API_WINCE
#     include <time.h> /* For srand() */
#  endif /* !RETROFLAT_API_WINCE */

#  ifdef RETROFLAT_WING

#     if defined( RETROFLAT_API_WIN32 )
#        define WINGAPI WINAPI
#     else
#        define WINGAPI WINAPI _loadds
#     endif

#     define RETROFLAT_WING_LLTABLE( f ) \
   f( HDC, WinGCreateDC, 1001 ) \
   f( BOOL, WinGRecommendDIBFormat, 1002 ) \
   f( HBITMAP, WinGCreateBitmap, 1003 ) \
   f( BOOL, WinGStretchBlt, 1009 )

typedef int16_t RETROFLAT_IN_KEY;

#     define RETROFLAT_MS_FMT "%lu"

typedef HDC (WINGAPI *WinGCreateDC_t)();
typedef BOOL (WINGAPI *WinGRecommendDIBFormat_t)( BITMAPINFO FAR* );
typedef HBITMAP (WINGAPI *WinGCreateBitmap_t)(
   HDC, BITMAPINFO const FAR*, void FAR* FAR* );
typedef BOOL (WINGAPI *WinGStretchBlt_t)(
   HDC, int, int, int, int, HDC, int, int, int, int );

#     define RETROFLAT_WING_LLTABLE_STRUCT_MEMBERS( retval, proc, ord ) \
   proc ## _t proc;

struct RETROFLAT_WING_MODULE {
   HMODULE module;
   uint8_t success;
   RETROFLAT_WING_LLTABLE( RETROFLAT_WING_LLTABLE_STRUCT_MEMBERS )
};
#  endif /* RETROFLAT_WING */

struct RETROFLAT_BMI {
   BITMAPINFOHEADER header;
   RGBQUAD colors[RETROFLAT_BMP_COLORS_SZ_MAX];
};

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   HBITMAP b;
   HBITMAP mask;
   HDC hdc_b;
   HDC hdc_mask;
   HBITMAP old_hbm_b;
   HBITMAP old_hbm_mask;
#  ifdef RETROFLAT_API_WIN16
   uint8_t far* bits;
#  else
	uint8_t* bits;
#  endif /* RETROFLAT_API_WIN16 */
   ssize_t autolock_refs;
#  ifdef RETROFLAT_OPENGL
   struct RETROFLAT_GLTEX tex;
#  endif /* RETROFLAT_OPENGL */
   struct RETROFLAT_BMI bmi;
};

LPSTR* retroflat_win_cli( LPSTR cmd_line, int* argc_out );

#  ifdef RETROFLAT_OPENGL

typedef float MAUG_CONST* RETROFLAT_COLOR_DEF;

#  else

/* Use Windoes API and generate brushes/pens for GDI. */

typedef COLORREF RETROFLAT_COLOR_DEF;

/* === Setup Brush Cache === */

/* This will be initialized in setup, so just preserve the number. */
#     define RETROFLAT_COLOR_TABLE_WIN_BRUSH( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         (HBRUSH)NULL,

#     define RETROFLAT_COLOR_TABLE_WIN_BRSET( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         gc_retroflat_win_brushes[idx] = CreateSolidBrush( RGB( r, g, b ) );

#     define RETROFLAT_COLOR_TABLE_WIN_BRRM( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   if( (HBRUSH)NULL != gc_retroflat_win_brushes[idx] ) { \
      DeleteObject( gc_retroflat_win_brushes[idx] ); \
      gc_retroflat_win_brushes[idx] = (HBRUSH)NULL; \
   }

/* === End Setup Brush Cache === */

/* === Setup Pen Cache === */

#     define RETROFLAT_COLOR_TABLE_WIN_PENS( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   (HPEN)NULL,

#     define RETROFLAT_COLOR_TABLE_WIN_PNSET( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         gc_retroflat_win_pens[idx] = CreatePen( \
            PS_SOLID, RETROFLAT_LINE_THICKNESS, RGB( r, g, b ) );

#     define RETROFLAT_COLOR_TABLE_WIN_PENRM( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         if( (HPEN)NULL != gc_retroflat_win_pens[idx] ) { \
            DeleteObject( gc_retroflat_win_pens[idx] ); \
            gc_retroflat_win_pens[idx] = (HPEN)NULL; \
         }

/* === End Setup Pen Cache === */

#     ifdef RETROFLT_C

HBRUSH gc_retroflat_win_brushes[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRUSH )
};

static HPEN gc_retroflat_win_pens[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PENS )
};

#     else

extern HBRUSH gc_retroflat_win_brushes[];

#     endif /* RETROFLT_C */

/* Create a brush and set it to the target HDC. */
#     define retroflat_win_setup_brush( old_brush, target, color, flags ) \
         if( RETROFLAT_FLAGS_FILL != (RETROFLAT_FLAGS_FILL & flags) ) { \
            old_brush = \
               SelectObject( target->hdc_b, GetStockObject( NULL_BRUSH ) ); \
         } else { \
            old_brush = SelectObject( \
               target->hdc_b, gc_retroflat_win_brushes[color] ); \
         }

/* Create a pen and set it to the target HDC. */
#     define retroflat_win_setup_pen( old_pen, target, color, flags ) \
         old_pen = \
            SelectObject( target->hdc_b, gc_retroflat_win_pens[color] );

#     define retroflat_win_cleanup_brush( old_brush, target ) \
         if( (HBRUSH)NULL != old_brush ) { \
            SelectObject( target->hdc_b, old_brush ); \
         }

#     define retroflat_win_cleanup_pen( old_pen, target ) \
         if( (HPEN)NULL != old_pen ) { \
            SelectObject( target->hdc_b, old_pen ); \
         }

#  endif /* RETROFLAT_OPENGL */

/* TODO: This is a parallel bitmap system... maybe move OPENGL stuff into its
 *       own header that takes over graphics stuff in OPENGL mode? */
#  ifdef RETROFLAT_OPENGL
#     define retroflat_bitmap_w( bmp ) \
         (NULL == (bmp) ? \
            g_retroflat_state->screen_v_w : ((bmp)->tex.w))
#     define retroflat_bitmap_h( bmp ) \
         (NULL == (bmp) ? \
            g_retroflat_state->screen_v_h : ((bmp)->tex.h))
#     define retroflat_bitmap_locked( bmp ) (NULL != (bmp)->tex.bytes)
#  else
#     define retroflat_bitmap_w( bmp ) \
         (NULL == (bmp) ? \
            g_retroflat_state->screen_v_w : ((bmp)->bmi.header.biWidth))
#     define retroflat_bitmap_h( bmp ) \
         (NULL == (bmp) ? \
            g_retroflat_state->screen_v_h : ((bmp)->bmi.header.biHeight))
#     define retroflat_bitmap_locked( bmp ) ((HDC)NULL != (bmp)->hdc_b)
#  endif /* RETROFLAT_OPENGL */
/* TODO: Adapt this for the OPENGL test above? */
#  define retroflat_bitmap_ok( bitmap ) ((HBITMAP)NULL != (bitmap)->b)

#  ifdef RETROFLAT_VDP

#     ifdef RETROFLAT_API_WIN16
#        error "VDP not supported in Win16!"
#     endif /* RETROFLAT_API_WIN16 */

#     define retroflat_vdp_lock( bmp ) \
   assert( NULL != (bmp)->hdc_b ); \
   /* Confirm header info. */ \
   (bmp)->autolock_refs++; \
   if( \
      1 == (bmp)->autolock_refs && \
      RETROFLAT_FLAGS_SCREEN_BUFFER != \
         (RETROFLAT_FLAGS_SCREEN_BUFFER & (bmp)->flags) \
   ) { \
      assert( NULL == (bmp)->bits ); \
      assert( (bmp)->bmi.header.biBitCount == 32 ); \
      assert( (bmp)->bmi.header.biWidth > 0 ); \
      assert( (bmp)->bmi.header.biHeight > 0 ); \
      assert( (bmp)->bmi.header.biSizeImage == \
         (bmp)->bmi.header.biWidth * (bmp)->bmi.header.biHeight * 4 ); \
      (bmp)->bits = VirtualAlloc( \
         0, (bmp)->bmi.header.biSizeImage, \
         MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ); \
      /* TODO: Check alloc without assert! */ \
      assert( NULL != (bmp)->bits ); \
      /* Get bitmap bits from bitmap HDC into bitmap->bits so that the bitmap
       * can be altered byte-by-byte.
       */ \
      GetDIBits( (bmp)->hdc_b, (bmp)->b, 0, (bmp)->bmi.header.biHeight, \
         (bmp)->bits, (BITMAPINFO*)&((bmp)->bmi), DIB_RGB_COLORS ); \
   }

#     define retroflat_vdp_release( bmp ) \
   assert( 0 < (bmp)->autolock_refs ); \
   (bmp)->autolock_refs--; \
   if( \
      0 == (bmp)->autolock_refs && \
      RETROFLAT_FLAGS_SCREEN_BUFFER != \
         (RETROFLAT_FLAGS_SCREEN_BUFFER & (bmp)->flags) \
   ) { \
      /* TODO: Causes alpha blending in mdemos? */ \
      if( \
         /* Set bitmap bits from bitmap->bits into HDC. */ \
         SetDIBits( g_retroflat_state->platform.hdc_win, (bmp)->b, 0, \
            (bmp)->bmi.header.biHeight, (bmp)->bits, \
            (BITMAPINFO*)&((bmp)->bmi), DIB_RGB_COLORS ) < \
               (bmp)->bmi.header.biHeight \
      ) { \
         error_printf( "SetDIBits failed!" ); \
      } \
      VirtualFree( (bmp)->bits, 0, MEM_RELEASE ); \
      (bmp)->bits = NULL; \
   }

#     define retroflat_screen_buffer() \
         (NULL == g_retroflat_state->vdp_buffer ? \
         &(g_retroflat_state->buffer) : g_retroflat_state->vdp_buffer)

#  else

#     define retroflat_screen_buffer() (&(g_retroflat_state->buffer))

#  endif /* RETROFLAT_VDP */

#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)
#  define retroflat_root_win() (g_retroflat_state->platform.window)
#  define retroflat_quit( retval_in ) PostQuitMessage( retval_in );

#  ifndef VK_OEM_1
#     define VK_OEM_1 0xba
#  endif /* !VK_OEM_1 */

#  ifndef VK_OEM_2
#     define VK_OEM_2 0xbf
#  endif /* !VK_OEM_2 */

#  ifndef VK_OEM_3
#     define VK_OEM_3 0xc0
#  endif /* !VK_OEM_3 */

#  ifndef VK_OEM_4
#     define VK_OEM_4 0xdb
#  endif /* !VK_OEM_4 */

#  ifndef VK_OEM_5
#     define VK_OEM_5 0xdc
#  endif /* !VK_OEM_5 */

#  ifndef VK_OEM_6
#     define VK_OEM_6 0xdd
#  endif /* !VK_OEM_6 */

#  ifndef VK_OEM_7
#     define VK_OEM_7 0xde
#  endif /* !VK_OEM_7 */

#  ifndef VK_OEM_MINUS
#     define VK_OEM_MINUS 0xbd
#  endif /* !VK_OEM_MINUS */

#  ifndef VK_OEM_PLUS
#     define VK_OEM_PLUS 0xbb
#  endif /* !VK_OEM_PLUS */

#  ifndef VK_OEM_PERIOD
#     define VK_OEM_PERIOD 0xbe
#  endif /* !VK_OEM_PERIOD */

#  ifndef VK_OEM_COMMA
#     define VK_OEM_COMMA 0xbc
#  endif /* !VK_OEM_COMMA */

#  define RETROFLAT_KEY_GRAVE VK_OEM_3
#  define RETROFLAT_KEY_SLASH VK_OEM_2
#  define RETROFLAT_KEY_UP	   VK_UP
#  define RETROFLAT_KEY_DOWN	VK_DOWN
#  define RETROFLAT_KEY_RIGHT	VK_RIGHT
#  define RETROFLAT_KEY_LEFT	VK_LEFT
#  define RETROFLAT_KEY_BKSP  VK_BACK
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
#  define RETROFLAT_KEY_ESC	VK_ESCAPE
#  define RETROFLAT_KEY_ENTER	VK_RETURN
#  define RETROFLAT_KEY_HOME	VK_HOME
#  define RETROFLAT_KEY_END	VK_END
#  define RETROFLAT_KEY_DELETE   VK_DELETE
#  define RETROFLAT_KEY_PGUP     VK_PRIOR
#  define RETROFLAT_KEY_PGDN     VK_NEXT
#  define RETROFLAT_KEY_SEMICOLON   VK_OEM_1
#  define RETROFLAT_KEY_PERIOD      VK_OEM_PERIOD
#  define RETROFLAT_KEY_COMMA       VK_OEM_COMMA
#  define RETROFLAT_KEY_EQUALS      VK_OEM_PLUS
#  define RETROFLAT_KEY_DASH        VK_OEM_MINUS
#  define RETROFLAT_KEY_BACKSLASH   VK_OEM_5
#  define RETROFLAT_KEY_QUOTE      VK_OEM_7
#  define RETROFLAT_KEY_BRACKETL    VK_OEM_4
#  define RETROFLAT_KEY_BRACKETR    VK_OEM_6
#  define RETROFLAT_KEY_INSERT    VK_INSERT

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

#define main( argc, argv ) retroflat_main( argc, argv )

/* Improvise a rough WinMain to call main(). */
#define END_OF_MAIN() \
   int WINXAPI WinMain( \
      HINSTANCE hInstance, HINSTANCE hPrevInstance, \
      LPSTR lpCmdLine, int nCmdShow \
   ) { \
      LPSTR* rf_argv = NULL; \
      int rf_argc = 0; \
      int retval = 0; \
      g_retroflat_instance = hInstance; \
      g_retroflat_cmd_show = nCmdShow; \
      rf_argv = retroflat_win_cli( lpCmdLine, &rf_argc ); \
      retval = retroflat_main( rf_argc, rf_argv ); \
      free( rf_argv ); \
      return retval; \
   }

/* Convenience macro for auto-locking inside of draw functions. */
/* TODO: Use maug_cleanup and assume retval. */
#define retroflat_internal_autolock_bitmap( bmp, lock_auto ) \
   if( !retroflat_bitmap_locked( bmp ) ) { \
      retval = retroflat_draw_lock( bmp ); \
      maug_cleanup_if_not_ok(); \
      lock_auto = 1; \
   }

struct RETROFLAT_PLATFORM {
   HWND                 window;
   int16_t              last_idc; /* Last clicked button. */
#  ifdef RETROFLAT_SCREENSAVER
   HWND                 parent;
#  endif /* RETROFLAT_SCREENSAVER */
   HDC                  hdc_win;
#  ifdef RETROFLAT_OPENGL
   HGLRC                hrc_win;
#  endif /* RETROFLAT_OPENGL */
   int16_t              last_key;
   uint8_t              vk_mods;
   unsigned int         last_mouse;
   unsigned int         last_mouse_x;
   unsigned int         last_mouse_y;
};

#endif /* !RETPLTD_H */

