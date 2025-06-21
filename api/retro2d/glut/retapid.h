
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#define RETROFLAT_NO_GENERIC_LOOP 1

#define RETROFLAT_SOFT_VIEWPORT

#  ifndef RETROFLAT_OPENGL
#     error "RETROFLAT_API_GLUT specified without RETROFLAT_OPENGL!"
#     define RETROFLAT_OPENGL
#  endif /* !RETROFLAT_OPENGL */

#  if defined( RETROFLAT_OPENGL )
#     include <GL/gl.h>
#     include <GL/glu.h>
#  endif /* RETROFLAT_OPENGL */

#  ifdef __WATCOMC__
#     define GLUT_DISABLE_ATEXIT_HACK
#  endif

#  include <GL/glut.h>

#  ifndef RETROFLAT_CONFIG_USE_FILE
#     define RETROFLAT_CONFIG_USE_FILE
#  endif /* !RETROFLAT_CONFIG_USE_FILE */

#  define RETROFLAT_MS_FMT "%lu"

typedef float MAUG_CONST* RETROFLAT_COLOR_DEF;

struct RETROFLAT_BITMAP {
   struct RETROFLAT_3DTEX tex;
};

#  if !defined( RETROFLAT_SOFT_SHAPES )
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

#  if !defined( RETROFLAT_SOFT_LINES )
/* TODO: Do we need soft lines for this? */
#     define RETROFLAT_SOFT_LINES
#  endif /* !RETROFLAT_SOFT_LINES */

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) \
      (RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & (bmp)->flags))
#  define retroflat_bitmap_w( bmp ) \
      (NULL == (bmp) ? g_retroflat_state->screen_v_w : ((bmp)->tex.w))
#  define retroflat_bitmap_h( bmp ) \
      (NULL == (bmp) ? g_retroflat_state->screen_v_h : ((bmp)->tex.h))

/*! \brief Special lock used before per-pixel operations because of SDL1. */
#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )
#  ifdef RETROFLAT_VDP
#     define retroflat_vdp_lock( bmp )
#     define retroflat_vdp_release( bmp )
#  endif /* RETROFLAT_VDP */
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)
#  define retroflat_screen_buffer() (&(g_retroflat_state->buffer))
#  define retroflat_root_win() (NULL) /* TODO */
#  define retroflat_quit( retval_in ) glutDestroyWindow( glutGetWindow() )
#  define END_OF_MAIN()

struct RETROFLAT_PLATFORM_ARGS {
   uint8_t flags;
};

struct RETROFLAT_PLATFORM {
   size_t               retroflat_next;
};

#endif /* !RETPLTD_H */

