
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#  ifdef RETROFLAT_OPENGL
#     error "opengl support not implemented for allegro"
#  endif /* RETROFLAT_OPENGL */

#  ifdef RETROFLAT_VDP
#     error "VDP support not implemented for allegro"
#  endif /* RETROFLAT_VDP */

/* == Allegro == */

#  ifdef MAUG_C
#     include <allegro.h>
#  else
#     include <allegro/keyboard.h>
#  endif /* MAUG_C */

#  ifdef RETROFLAT_OS_DOS
#     include <dos.h>
#     include <conio.h>
#  endif /* RETROFLAT_OS_DOS */

#include <time.h> /* For srand() */

typedef int16_t RETROFLAT_IN_KEY;
typedef uint32_t retroflat_ms_t;

#define RETROFLAT_MS_FMT "%u"

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
#  define RETROFLAT_KEY_DELETE   KEY_DEL
#  define RETROFLAT_KEY_PGUP     KEY_PGUP
#  define RETROFLAT_KEY_PGDN     KEY_PGDN
#  define RETROFLAT_KEY_GRAVE    KEY_BACKQUOTE
#  define RETROFLAT_KEY_BKSP     KEY_BACKSPACE
#  define RETROFLAT_KEY_SLASH    KEY_SLASH
#  define RETROFLAT_KEY_SEMICOLON   KEY_SEMICOLON
#  define RETROFLAT_KEY_PERIOD   KEY_STOP
#  define RETROFLAT_KEY_COMMA    KEY_COMMA
#  define RETROFLAT_KEY_EQUALS   KEY_EQUALS
#  define RETROFLAT_KEY_DASH     KEY_MINUS
#  define RETROFLAT_KEY_BRACKETL KEY_OPENBRACE
#  define RETROFLAT_KEY_BRACKETR KEY_CLOSEBRACE
#  define RETROFLAT_KEY_BACKSLASH   KEY_BACKSLASH
#  define RETROFLAT_KEY_QUOTE    KEY_QUOTE

/* These helper functions keep references to allergo global variables and
 * struct internals in MAIN_C, as the allegro headers on DJGPP don't seem
 * so good and will cause multiple defs otherwise...
 */

int retroflat_allegro_screen_w();
int retroflat_allegro_screen_h();
int retroflat_allegro_bmp_w( struct RETROFLAT_BITMAP* bmp );
int retroflat_allegro_bmp_h( struct RETROFLAT_BITMAP* bmp );

struct RETROFLAT_PLATFORM {
#  ifdef RETROFLAT_OS_DOS
   unsigned int         last_mouse;
   unsigned int         last_mouse_x;
   unsigned int         last_mouse_y;
#  endif /* RETROFLAT_OS_DOS */
   unsigned int         close_button;
};

#endif /* !RETPLTD_H */

