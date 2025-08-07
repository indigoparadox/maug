
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

struct RETROFLAT_INPUT_STATE {
   int16_t              retroflat_last_key;
};

typedef int16_t RETROFLAT_IN_KEY;

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
/* TODO */
#  define RETROFLAT_KEY_INSERT

#elif defined( RETROFLT_C )

#     ifdef RETROFLAT_OS_OS2
void APIENTRY
#     elif defined( RETROFLAT_OS_WIN )
void 
#     else
void
#     endif /* RETROFLAT_OS_OS2 */
retroflat_glut_key( unsigned char key, int x, int y ) {
#     ifdef RETROFLAT_OS_WIN
      /* key -= 109; */
#     endif /* RETROFLAT_OS_WIN */
   debug_printf( 0, "key: %c (0x%02x)", key, key );
   g_retroflat_state->input.retroflat_last_key = key;
}

/* === */

MERROR_RETVAL retroflat_init_input( struct RETROFLAT_ARGS* args ) {
   glutKeyboardFunc( retroflat_glut_key );
   return MERROR_OK;
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   /* TODO: Implement RETROFLAT_MOD_SHIFT. */

   key_out = g_retroflat_state->input.retroflat_last_key;
   g_retroflat_state->input.retroflat_last_key = 0;

   /* TODO: Handle mouse. */

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

