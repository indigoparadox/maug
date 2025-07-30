
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#ifdef MAUG_NO_STDLIB
/* Define this for END_OF_MAIN below. */
void free( void* );
#endif /* MAUG_NO_STDLIB */

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

#  if defined( RETROFLAT_API_WIN16 ) && defined( RETROFLAT_OPENGL )
#     error "opengl support not implemented for win16"
#  endif /* RETROFLAT_API_SDL2 && RETROFLAT_OPENGL */

#  if defined( RETROFLAT_OPENGL )
#     include <GL/gl.h>
#     include <GL/glu.h>
#  endif /* RETROFLAT_OPENGL */

#  define RETROFLAT_VDP_LIB_NAME "rvdpnt"

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
#  ifdef RETROFLAT_OPENGL
   struct RETROFLAT_3DTEX tex;
#  else
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
   struct RETROFLAT_BMI bmi;
#  endif /* RETROFLAT_OPENGL */
};

#  ifdef MAUG_NO_CLI
#define retroflat_win_cli( cmd_line, argc_out ) 0
#  else
LPSTR* retroflat_win_cli( LPSTR cmd_line, int* argc_out );
#  endif /* !MAUG_NO_CLI */

#  ifdef RETROFLAT_OPENGL

typedef float MAUG_CONST* RETROFLAT_COLOR_DEF;

#  else

/* Use Windoes API and generate brushes/pens for GDI. */

typedef COLORREF RETROFLAT_COLOR_DEF;

/* === Setup Brush Cache === */

#     define RETROFLAT_COLOR_TABLE_WIN_BRSET( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         g_retroflat_state->platform.brushes[idx] = \
            CreateSolidBrush( RGB( r, g, b ) );

#     define RETROFLAT_COLOR_TABLE_WIN_BRRM( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   if( (HBRUSH)NULL != g_retroflat_state->platform.brushes[idx] ) { \
      DeleteObject( g_retroflat_state->platform.brushes[idx] ); \
      g_retroflat_state->platform.brushes[idx] = (HBRUSH)NULL; \
   }

/* === End Setup Brush Cache === */

/* === Setup Pen Cache === */

#     define RETROFLAT_COLOR_TABLE_WIN_PNSET( \
         idx, name_l, name_u, r, g, b, cgac, cgad ) \
            g_retroflat_state->platform.pens[idx] = (HPEN)CreatePen( \
               PS_SOLID, RETROFLAT_LINE_THICKNESS, RGB( r, g, b ) );

#     define RETROFLAT_COLOR_TABLE_WIN_PENRM( \
         idx, name_l, name_u, r, g, b, cgac, cgad ) \
            if( (HPEN)NULL != g_retroflat_state->platform.pens[idx] ) { \
               DeleteObject( g_retroflat_state->platform.pens[idx] ); \
               g_retroflat_state->platform.pens[idx] = (HPEN)NULL; \
            }

/* === End Setup Pen Cache === */

/* Create a brush and set it to the target HDC. */
#     define retroflat_win_setup_brush( old_brush, target, color, flags ) \
         if( RETROFLAT_FLAGS_FILL != (RETROFLAT_FLAGS_FILL & flags) ) { \
            old_brush = \
               SelectObject( target->hdc_b, GetStockObject( NULL_BRUSH ) ); \
         } else { \
            old_brush = SelectObject( \
               target->hdc_b, g_retroflat_state->platform.brushes[color] ); \
         }

#     define retroflat_win_cleanup_brush( old_brush, target ) \
         if( (HBRUSH)NULL != old_brush ) { \
            SelectObject( target->hdc_b, old_brush ); \
         }

/* Create a pen and set it to the target HDC. */
#     define retroflat_win_setup_pen( old_pen, target, color, flags ) \
         old_pen = \
            SelectObject( target->hdc_b, g_retroflat_state->platform.pens[color] );

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

/* Set the calling convention for WinMain, depending on Win16/Win32. */
#  if defined( RETROFLAT_API_WIN16 )
#     define WINXAPI PASCAL
#  else
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

struct RETROFLAT_PLATFORM_ARGS {
   uint8_t flags;
};

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
   HBRUSH               brushes[16];
   HPEN                 pens[16];
};

/* For now, these are set by WinMain(), so they need to be outside of the
 * state that's zeroed on init()!
 */
#ifdef RETROFLT_C
HINSTANCE SEG_MGLOBAL g_retroflat_instance;
int SEG_MGLOBAL g_retroflat_cmd_show;
#else
extern HINSTANCE SEG_MGLOBAL g_retroflat_instance;
extern int SEG_MGLOBAL g_retroflat_cmd_show;
#endif

#endif /* !RETPLTD_H */

