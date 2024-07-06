
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#define RETROFLAT_NO_GENERIC_LOOP 1

#  ifndef RETROFLAT_OPENGL
#     error "RETROFLAT_API_GLUT specified without RETROFLAT_OPENGL!"
#     define RETROFLAT_OPENGL
#  endif /* !RETROFLAT_OPENGL */

#  ifdef __WATCOMC__
#     define GLUT_DISABLE_ATEXIT_HACK
#  endif

#  include <GL/glut.h>

#  ifndef RETROFLAT_CONFIG_USE_FILE
#     define RETROFLAT_CONFIG_USE_FILE
#  endif /* !RETROFLAT_CONFIG_USE_FILE */

typedef int16_t RETROFLAT_IN_KEY;
typedef uint32_t retroflat_ms_t;

#  define RETROFLAT_MS_FMT "%lu"

typedef float MAUG_CONST* RETROFLAT_COLOR_DEF;

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   struct RETROFLAT_GLTEX tex;
   ssize_t w;
   ssize_t h;
};

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

#  define GLUT_SPECIAL_KEY_OFFSET 0x80

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

#  define RETROFLAT_KEY_UP	      (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_UP)
#  define RETROFLAT_KEY_DOWN     (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_DOWN)
#  define RETROFLAT_KEY_RIGHT	   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_RIGHT)
#  define RETROFLAT_KEY_LEFT	   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_LEFT)
#  define RETROFLAT_KEY_HOME	   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_HOME)
#  define RETROFLAT_KEY_END	   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_END)
#  define RETROFLAT_KEY_PGUP     (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_PAGE_UP)
#  define RETROFLAT_KEY_PGDN     \
   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_PAGE_DOWN)
#  define RETROFLAT_KEY_DELETE   0x7f
#  define RETROFLAT_KEY_ESC      0x1b
#  define RETROFLAT_KEY_ENTER    0x0d
#  define RETROFLAT_KEY_TAB	   '\t'
#  define RETROFLAT_KEY_SPACE	   ' '
#  define RETROFLAT_KEY_GRAVE    '`'
#  define RETROFLAT_KEY_SLASH    '/'
#  define RETROFLAT_KEY_BKSP     0x08
#  define RETROFLAT_KEY_SEMICOLON   ';'
#  define RETROFLAT_KEY_PERIOD   '.'
#  define RETROFLAT_KEY_COMMA    ','
#  define RETROFLAT_KEY_EQUALS   '='
#  define RETROFLAT_KEY_DASH     '-'
#  define RETROFLAT_KEY_A		   'a'
#  define RETROFLAT_KEY_B		   'b'
#  define RETROFLAT_KEY_C		   'c'
#  define RETROFLAT_KEY_D		   'd'
#  define RETROFLAT_KEY_E		   'e'
#  define RETROFLAT_KEY_F		   'f'
#  define RETROFLAT_KEY_G		   'g'
#  define RETROFLAT_KEY_H		   'h'
#  define RETROFLAT_KEY_I		   'i'
#  define RETROFLAT_KEY_J		   'j'
#  define RETROFLAT_KEY_K		   'k'
#  define RETROFLAT_KEY_L		   'l'
#  define RETROFLAT_KEY_M		   'm'
#  define RETROFLAT_KEY_N		   'n'
#  define RETROFLAT_KEY_O		   'o'
#  define RETROFLAT_KEY_P		   'p'
#  define RETROFLAT_KEY_Q		   'q'
#  define RETROFLAT_KEY_R		   'r'
#  define RETROFLAT_KEY_S		   's'
#  define RETROFLAT_KEY_T		   't'
#  define RETROFLAT_KEY_U		   'u'
#  define RETROFLAT_KEY_V		   'v'
#  define RETROFLAT_KEY_W		   'w'
#  define RETROFLAT_KEY_X		   'x'
#  define RETROFLAT_KEY_Y		   'y'
#  define RETROFLAT_KEY_Z		   'z'
#  define RETROFLAT_KEY_0		   '0'
#  define RETROFLAT_KEY_1		   '1'
#  define RETROFLAT_KEY_2		   '2'
#  define RETROFLAT_KEY_3		   '3'
#  define RETROFLAT_KEY_4		   '4'
#  define RETROFLAT_KEY_5		   '5'
#  define RETROFLAT_KEY_6		   '6'
#  define RETROFLAT_KEY_7		   '7'
#  define RETROFLAT_KEY_8		   '8'
#  define RETROFLAT_KEY_9		   '9'
/* TODO */
#  define RETROFLAT_KEY_BRACKETL '['
/* TODO */
#  define RETROFLAT_KEY_BRACKETR ']'
/* TODO */
#  define RETROFLAT_KEY_BACKSLASH   '\\'
/* TODO */
#  define RETROFLAT_KEY_QUOTE    '\''
/* TODO */
#  define RETROFLAT_KEY_EQUALS    '='

struct RETROFLAT_PLATFORM {
   size_t               retroflat_next;
   retroflat_loop_iter  loop_iter;
   retroflat_loop_iter  frame_iter;
   int16_t              retroflat_last_key;
};

#endif /* !RETPLTD_H */

