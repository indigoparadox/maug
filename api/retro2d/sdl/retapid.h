
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
#  if defined( RETROFLAT_API_SDL1 ) && !defined( RETROFLAT_OS_WASM )
#     include <SDL_getenv.h>
#  endif /* RETROFLAT_API_SDL1 */

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
#     ifdef RETROFLAT_API_SDL1
   /* SDL1 autolock counter. */
   ssize_t autolock_refs;
#     endif /* RETROFLAT_API_SDL1 */
#  endif /* RETROFLAT_OPENGL */
#  ifdef RETROFLAT_API_SDL2
   /* SDL2 texture pointers. */
   SDL_Texture* texture;
   SDL_Renderer* renderer;
#  endif /* RETROFLAT_API_SDL2 */
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

#     if defined( RETROFLAT_API_SDL1 )
#        define retroflat_bitmap_locked( bmp ) \
            (RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & (bmp)->flags))

#     elif defined( RETROFLAT_API_SDL2 )
#        define retroflat_bitmap_locked( bmp ) (NULL != (bmp)->renderer)
#     endif /* RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 */

#  endif /* RETROFLAT_OPENGL */

#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)

#  ifdef RETROFLAT_VDP
#     define retroflat_screen_buffer() \
         (NULL == g_retroflat_state->vdp_buffer ? \
         &(g_retroflat_state->buffer) : g_retroflat_state->vdp_buffer)
#  else
#     define retroflat_screen_buffer() (&(g_retroflat_state->buffer))
#  endif /* RETROFLAT_VDP */
#  define retroflat_root_win() (NULL) /* TODO */

#  if defined( RETROFLAT_API_SDL1 ) && !defined( RETROFLAT_OPENGL )
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
#  else
/* Pixel lock above does not apply to SDL2 surfaces or bitmap textures. */
#     define retroflat_px_lock( bmp )
#     define retroflat_px_release( bmp )
#  endif

#  define retroflat_quit( retval_in ) \
      debug_printf( 1, "quit called, retval: %d", retval_in ); \
      g_retroflat_state->retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING; \
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
#  if defined( RETROFLAT_API_SDL1 ) && !defined( RETROFLAT_NO_SDL1_SCALING )
   /* The real screen buffer, if scaling is enabled. Things are drawn onto
    * g_retroflat_state->buffer (technically the actual scaling buffer) before
    * being scaled onto the screen. This is necessary for e.g. WASM, where we
    * can't otherwise easily scale the screen.
    */
   SDL_Surface* scale_buffer;
   SDL_Rect scale_rect;
#  elif !defined( RETROFLAT_API_SDL1 )
   SDL_Window*          window;
#  endif /* !RETROFLAT_API_SDL1 */
};

#endif /* !RETPLTD_H */

