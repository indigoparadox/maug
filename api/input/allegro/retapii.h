
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

#  ifndef MAUG_C
#     include <allegro/keyboard.h>
#  endif /* MAUG_C */

#  ifdef RETROFLAT_OS_DOS
#     include <dos.h>
#     include <conio.h>
#  endif /* RETROFLAT_OS_DOS */

typedef int16_t RETROFLAT_IN_KEY;

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
#  define RETROFLAT_KEY_INSERT   KEY_INSERT

/* TODO: Make input-specific platform struct. */
struct RETROFLAT_PLATFORM {
#  ifdef RETROFLAT_OS_DOS
   unsigned int         last_mouse;
   unsigned int         last_mouse_x;
   unsigned int         last_mouse_y;
#  endif /* RETROFLAT_OS_DOS */
   unsigned int         close_button;
};

#elif defined( RETROFLT_C )

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
#  if defined( RETROFLAT_OS_DOS ) || defined( RETROFLAT_OS_DOS_REAL )
   union REGS inregs;
   union REGS outregs;
#  endif /* RETROFLAT_API_ALLEGRO && RETROFLAT_OS_DOS */
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   if( g_retroflat_state->platform.close_button ) {
      retroflat_quit( 0 );
      return 0;
   }

#     if defined( __WATCOMC__ ) && defined( RETROFLAT_OS_DOS )
   /* Allegro mouse is broken under watcom in DOS. */

   /* Poll the mouse. */
   inregs.w.ax = 3;
   int386( 0x33, &inregs, &outregs );

   if(
      1 == outregs.x.ebx && /* Left button clicked. */
      outregs.w.cx != g_retroflat_state->platform.last_mouse_x &&
      outregs.w.dx != g_retroflat_state->platform.last_mouse_y
   ) { 
      input->mouse_x = outregs.w.cx;
      input->mouse_y = outregs.w.dx;

      /* Prevent repeated clicks. */
      g_retroflat_state->platform.last_mouse_x = input->mouse_x;
      g_retroflat_state->platform.last_mouse_y = input->mouse_y;

      return RETROFLAT_MOUSE_B_LEFT;
   } else {
      g_retroflat_state->platform.last_mouse_x = outregs.w.cx;
      g_retroflat_state->platform.last_mouse_y = outregs.w.dx;
   }

#     else
   poll_mouse();
   if( mouse_b & 0x01 ) {
      input->mouse_x = mouse_x;
      input->mouse_y = mouse_y;
      return RETROFLAT_MOUSE_B_LEFT;
   } else if( mouse_b & 0x02 ) {
      input->mouse_x = mouse_x;
      input->mouse_y = mouse_y;
      return RETROFLAT_MOUSE_B_RIGHT;
   }
#     endif /* RETROFLAT_OS_DOS */

   poll_keyboard();
   if( keypressed() ) {
      /* TODO: ??? */
      if( KB_SHIFT_FLAG == (KB_SHIFT_FLAG & key_shifts) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_SHIFT;
      }

      if( KB_CTRL_FLAG == (KB_CTRL_FLAG & key_shifts) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_CTRL;
      }

      if( KB_ALT_FLAG == (KB_ALT_FLAG & key_shifts) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_ALT;
      }

      return (readkey() >> 8);
   }

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

