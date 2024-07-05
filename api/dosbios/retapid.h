
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#  ifdef RETROFLAT_OPENGL
#     error "opengl support not implemented for PC BIOS"
#  endif /* RETROFLAT_OPENGL */

#  ifdef RETROFLAT_VDP
#     error "VDP support not implemented for PC BIOS"
#  endif /* RETROFLAT_VDP */

#  include <time.h> /* For srand() */

#  if defined( MAUG_OS_DOS_REAL ) && \
   defined( MAUG_DOS_MEM_L ) && \
   defined( __WATCOMC__ )
#     define SEG_RETROBMP __based( __segname( "RETROBMP" ) )
#  else
#     define SEG_RETROBMP
#  endif /* __WATCOMC__ */

typedef int16_t RETROFLAT_IN_KEY;
typedef uint16_t retroflat_ms_t;

#  define RETROFLAT_MS_FMT "%u"

/* Explicity screen sizes aren't supported, only screen modes handled in
 * special cases during init.
 */
#  define RETROFLAT_NO_CLI_SZ

#  define RETROFLAT_SCREEN_MODE_CGA       0x04
#  define RETROFLAT_SCREEN_MODE_VGA       0x13

#  define RETROFLAT_CGA_COLOR_BLACK        0
#  define RETROFLAT_CGA_COLOR_CYAN         1
#  define RETROFLAT_CGA_COLOR_MAGENTA      2
#  define RETROFLAT_CGA_COLOR_WHITE        3

#  ifndef RETROFLAT_SOFT_SHAPES
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

#  ifndef RETROFLAT_DOS_TIMER_DIV
/* #define RETROFLAT_DOS_TIMER_DIV 1103 */
#     define RETROFLAT_DOS_TIMER_DIV 100
#  endif /* !RETROFLAT_DOS_TIMER_DIV */

#  define retroflat_bitmap_w( bmp ) ((bmp)->w)
#  define retroflat_bitmap_h( bmp ) ((bmp)->h)

#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )

#  ifdef RETROFLAT_VDP
#     define retroflat_vdp_lock( bmp )
#     define retroflat_vdp_release( bmp )
#  endif /* RETROFLAT_VDP */

#  ifndef NO_I86
#     include <i86.h>
#  endif /* NO_I86 */
#  include <dos.h>
#  include <conio.h>
#  include <malloc.h>

#  define END_OF_MAIN()

#  ifndef RETROFLAT_CONFIG_USE_FILE
#     define RETROFLAT_CONFIG_USE_FILE
#  endif /* !RETROFLAT_CONFIG_USE_FILE */

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->px)

#  define retroflat_quit( retval_in ) \
   g_retroflat_state->retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING; \
   g_retroflat_state->retval = retval_in;

typedef uint8_t RETROFLAT_COLOR_DEF;

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   int16_t w;
   int16_t h;
   uint8_t SEG_FAR* px;
   uint8_t SEG_FAR* mask;
};

#  define retroflat_screen_buffer() (&(g_retroflat_state->buffer))

/* We only explicitly support screen modes with these dimensions anyway.
 */
#  define retroflat_screen_w() (g_retroflat_state->buffer.w)
#  define retroflat_screen_h() (g_retroflat_state->buffer.h)

/* TODO: DOS Keycodes */

#  define RETROFLAT_KEY_BKSP  0x08
/* TODO */
#  define RETROFLAT_KEY_GRAVE /* 0x60 */ '`'
#  define RETROFLAT_KEY_DASH  '-'
#  define RETROFLAT_KEY_SLASH '/'
#  define RETROFLAT_KEY_PERIOD '.'
#  define RETROFLAT_KEY_COMMA ','
#  define RETROFLAT_KEY_SEMICOLON ';'
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
/* TODO */
#  define RETROFLAT_KEY_TAB	0
#  define RETROFLAT_KEY_SPACE	0x20
#  define RETROFLAT_KEY_ESC	0x1b
#  define RETROFLAT_KEY_ENTER	0x0d
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

/* Handle keys that send a double-code. */
#  define RETROFLAT_KEY_UP	      -3
#  define RETROFLAT_KEY_DOWN	   -4
#  define RETROFLAT_KEY_RIGHT	   -5
#  define RETROFLAT_KEY_LEFT	   -6
#  define RETROFLAT_KEY_HOME	   -7
#  define RETROFLAT_KEY_END	   -8
#  define RETROFLAT_KEY_PGUP     -9
#  define RETROFLAT_KEY_PGDN     -10
#  define RETROFLAT_KEY_DELETE   -11

#  define RETROFLAT_MOUSE_B_LEFT    -100
#  define RETROFLAT_MOUSE_B_RIGHT   -200

typedef void (__interrupt __far* retroflat_intfunc)( void );

struct RETROFLAT_PLATFORM {
   retroflat_intfunc old_timer_interrupt;
   uint8_t old_video_mode;
   uint8_t cga_color_table[16];
   uint8_t cga_dither_table[16];
   uint8_t screen_mode;
};

#endif /* !RETPLTD_H */

