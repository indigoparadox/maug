
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#define RETROFLAT_SOFT_VIEWPORT

#ifndef RETROFLAT_NO_VDP
/* If we're not running on a console or something, enable VDP! */
#  define RETROFLAT_VDP
#endif /* !RETROFLAT_NO_VDP */

#  include <time.h> /* For srand() */

#  if defined( RETROFLAT_OS_WASM )
#     include <emscripten.h>
#  endif /* RETROFLAT_OS_WASM */

#  include <SDL.h>
#  ifndef RETROFLAT_OS_WASM
#     include <SDL_getenv.h>
#  endif /* RETROFLAT_OS_WASM */

#  if !defined( RETROFLAT_SOFT_SHAPES )
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

#  if !defined( RETROFLAT_SOFT_LINES )
/* TODO: Do we need soft lines for this? */
#     define RETROFLAT_SOFT_LINES
#  endif /* !RETROFLAT_SOFT_LINES */

#  define RETROFLAT_VDP_LIB_NAME "rvdpsdl1"

struct RETROFLAT_BITMAP {
#  ifdef RETROFLAT_BMP_TEX
   struct RETROFLAT_3DTEX tex;
#  else
   size_t sz;
   uint8_t flags;
   uint16_t id;
   SDL_Surface* surface;
   /* SDL1 autolock counter. */
   ssize_t autolock_refs;
#  endif /* RETROFLAT_OPENGL */
};

#  ifdef RETROFLAT_OPENGL
#     define retroflat_bitmap_w( bmp ) \
         (NULL == (bmp) || \
            (NULL == (bmp)->tex.bytes_h && NULL == (bmp)->tex.bytes) ? \
            g_retroflat_state->screen_v_w : ((bmp)->tex.w))
#     define retroflat_bitmap_h( bmp ) \
         (NULL == (bmp) || \
            (NULL == (bmp)->tex.bytes_h && NULL == (bmp)->tex.bytes) ? \
            g_retroflat_state->screen_v_h : ((bmp)->tex.h))
#     define retroflat_bitmap_locked( bmp ) (NULL != (bmp)->tex.bytes)

#  else

#     define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->surface)
#     define retroflat_bitmap_w( bmp ) \
         (NULL == (bmp) || NULL == (bmp)->surface ? \
            g_retroflat_state->screen_v_w : (size_t)((bmp)->surface->w))
#     define retroflat_bitmap_h( bmp ) \
         (NULL == (bmp) || NULL == (bmp)->surface ? \
            g_retroflat_state->screen_v_h : (size_t)((bmp)->surface->h))
#     define retroflat_bitmap_locked( bmp ) \
         (RETROFLAT_BITMAP_FLAG_LOCK == (RETROFLAT_BITMAP_FLAG_LOCK & (bmp)->flags))

#  endif /* RETROFLAT_OPENGL */

#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)

#  define retroflat_screen_buffer() \
      (&(g_retroflat_state->platform.screen_buffer))
#  define retroflat_root_win() (NULL) /* TODO */

#  ifdef RETROFLAT_OPENGL

/* Pixel lock does not apply to OpenGL textures. */
#     define retroflat_px_lock( bmp )
#     define retroflat_px_release( bmp )

#  else

/* Special pixel lock JUST for SDL1 surfaces. */
#     define retroflat_px_lock( bmp ) \
         if( NULL != bmp ) { \
            (bmp)->autolock_refs++; \
            SDL_LockSurface( (bmp)->surface ); \
         }
#     define retroflat_px_release( bmp ) \
         if( NULL != bmp ) { \
            assert( 0 < (bmp)->autolock_refs ); \
            (bmp)->autolock_refs--; \
            SDL_UnlockSurface( (bmp)->surface ); \
         }
#     ifdef RETROFLAT_VDP
#        define retroflat_vdp_lock( bmp ) retroflat_px_lock( bmp )
#        define retroflat_vdp_release( bmp ) retroflat_px_release( bmp )
#     endif /* RETROFLAT_VDP */

#  endif /* RETROFLAT_OPENGL */

#  define retroflat_quit( retval_in ) \
      debug_printf( 1, "quit called, retval: %d", retval_in ); \
      g_retroflat_state->retroflat_flags &= ~RETROFLAT_STATE_FLAG_RUNNING; \
      g_retroflat_state->retval = retval_in;

#  define END_OF_MAIN()

   /* SDL Icon */
#  if defined( RETROFLT_C ) && defined( RETROFLAT_SDL_ICO )
#     include <sdl_ico.h>
#  endif /* RETROFLT_C && RETROFLAT_SDL_ICO */

#  ifndef RETROFLAT_CONFIG_USE_FILE
#     define RETROFLAT_CONFIG_USE_FILE
#  endif /* !RETROFLAT_CONFIG_USE_FILE */

   /* SDL Colors */
#  ifdef RETROFLAT_BMP_TEX
typedef float RETROFLAT_COLOR_DEF[3];
#else
typedef SDL_Color RETROFLAT_COLOR_DEF;
#  endif /* RETROFLAT_OPENGL */

struct RETROFLAT_PLATFORM_ARGS {
   uint8_t flags;
};

struct RETROFLAT_PLATFORM {
   uint8_t flags;
   struct RETROFLAT_BITMAP screen_buffer;
   struct RETROFLAT_BITMAP screen_final;
#  ifdef RETROFLAT_VDP
   struct RETROFLAT_BITMAP screen_vdp;
#  endif /* RETROFLAT_VDP */
#  ifndef RETROFLAT_NO_SDL1_SCALING
   /* The real screen buffer, if scaling is enabled. Things are drawn onto
    * g_retroflat_state->screen_buffer (technically the actual scaling buffer)
    * before being scaled onto the screen. This is necessary for e.g. WASM,
    * where we can't otherwise easily scale the screen.
    */
   struct RETROFLAT_BITMAP screen_scale;
#  endif /* !RETROFLAT_NO_SDL1_SCALING */
   uint8_t focus;
};

#endif /* !RETPLTD_H */

