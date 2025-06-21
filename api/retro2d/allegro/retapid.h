
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#define RETROFLAT_SOFT_VIEWPORT

#  ifdef RETROFLAT_OPENGL
#     error "opengl support not implemented for allegro"
#  endif /* RETROFLAT_OPENGL */

#  ifdef RETROFLAT_VDP
#     error "VDP support not implemented for allegro"
#  endif /* RETROFLAT_VDP */

/* == Allegro == */

#  ifdef MAUG_C
#     include <allegro.h>
#  endif /* MAUG_C */

#include <time.h> /* For srand() */

typedef int16_t RETROFLAT_IN_KEY;

#define RETROFLAT_MS_FMT "%u"

/* Inline pass to generic loop. */
#define retroflat_loop( frame_iter, loop_iter, data ) \
   retroflat_loop_generic( frame_iter, loop_iter, data )

struct BITMAP;

typedef struct BITMAP BITMAP;

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   BITMAP* b;
};

typedef int RETROFLAT_COLOR_DEF;

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) (0)
#  define retroflat_bitmap_w( bmp ) (NULL == (bmp) ? \
      retroflat_screen_w() : retroflat_allegro_bmp_w( bmp ))
#  define retroflat_bitmap_h( bmp ) (NULL == (bmp) ? \
      retroflat_screen_h() : retroflat_allegro_bmp_h( bmp ))
#  define retroflat_screen_w() (retroflat_allegro_screen_w())
#  define retroflat_screen_h() (retroflat_allegro_screen_h())
#  define retroflat_screen_buffer() (&(g_retroflat_state->buffer))
#  define retroflat_root_win() (NULL) /* TODO */
#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )
#  ifdef RETROFLAT_VDP
#     define retroflat_vdp_lock( bmp )
#     define retroflat_vdp_release( bmp )
#  endif /* RETROFLAT_VDP */

#  define retroflat_quit( retval_in ) \
   g_retroflat_state->retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING; \
   g_retroflat_state->retval = retval_in;

/* These helper functions keep references to allergo global variables and
 * struct internals in MAIN_C, as the allegro headers on DJGPP don't seem
 * so good and will cause multiple defs otherwise...
 */

int retroflat_allegro_screen_w();
int retroflat_allegro_screen_h();
int retroflat_allegro_bmp_w( struct RETROFLAT_BITMAP* bmp );
int retroflat_allegro_bmp_h( struct RETROFLAT_BITMAP* bmp );

struct RETROFLAT_PLATFORM_ARGS {
   uint8_t flags;
};

struct RETROFLAT_PLATFORM {
   uint8_t flags;
};

#endif /* !RETPLTD_H */

