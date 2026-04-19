
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#define RETROFLAT_SOFT_VIEWPORT

#  include <time.h> /* For srand() */

#  ifndef RETROFLAT_SDL_BPP
#     define RETROFLAT_SDL_BPP 32
#  endif /* !RETROFLAT_SDL_BPP */

#  if defined( RETROFLAT_OS_WASM )
#     include <emscripten.h>
#  endif /* RETROFLAT_OS_WASM */

#  include <SDL.h>

#  if !defined( RETROFLAT_SOFT_SHAPES )
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

#  if !defined( RETROFLAT_SOFT_LINES )
/* TODO: Do we need soft lines for this? */
#     define RETROFLAT_SOFT_LINES
#  endif /* !RETROFLAT_SOFT_LINES */

#  ifdef RETROFLAT_API_SDL1
#     define RETROFLAT_VDP_LIB_NAME "rvdpsdl1"
#  elif defined( RETROFLAT_API_SDL2 )
#     define RETROFLAT_VDP_LIB_NAME "rvdpsdl2"
#  else
#     error "rvdp .so undefined!"
#  endif

struct RETROFLAT_BITMAP {
#  ifdef RETROFLAT_BMP_TEX
   struct RETROFLAT_3DTEX tex;
#  else
   size_t sz;
   uint8_t flags;
   SDL_Surface* surface;
#  endif /* RETROFLAT_BMP_TEX */
   /* SDL2 texture pointers. */
   SDL_Texture* texture;
   SDL_Renderer* renderer;
};

#  ifdef RETROFLAT_BMP_TEX
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

#     define retroflat_bitmap_locked( bmp ) (NULL != (bmp)->renderer)

#  endif /* RETROFLAT_BMP_TEX */

#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)

#  ifdef RETROFLAT_VDP
#     define retroflat_screen_buffer() \
         (NULL == g_retroflat_state->vdp_buffer ? \
         &(g_retroflat_state->platform.screen_buffer) : \
         g_retroflat_state->vdp_buffer)
#  else
#     define retroflat_screen_buffer() \
         (&(g_retroflat_state->platform.screen_buffer))
#  endif /* RETROFLAT_VDP */
#  define retroflat_root_win() (NULL) /* TODO */

/* Pixel lock above does not apply to SDL2 surfaces or bitmap textures. */
#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )

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
#  endif /* RETROFLAT_BMP_TEX */

struct RETROFLAT_PLATFORM_ARGS {
   uint8_t flags;
};

struct RETROFLAT_PLATFORM {
   uint8_t flags;
   struct RETROFLAT_BITMAP screen_buffer;
   SDL_Window*          window;
   uint8_t focus;
};

#endif /* !RETPLTD_H */

