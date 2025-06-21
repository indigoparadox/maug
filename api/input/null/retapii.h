
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
};

typedef int16_t RETROFLAT_IN_KEY;

/**
 * \addtogroup maug_retroflt_input
 * \{
 *
 * \addtogroup maug_retroflt_keydefs RetroFlat Key Definitions
 * \brief Keyboard and mouse controls RetroFlat is aware of, for use within the
 *        \ref maug_retroflt_input.
 *
 * The precise type and values of these constants vary by platform.
 *
 * \{
 */

#  define RETROFLAT_KEY_UP	         -113
#  define RETROFLAT_KEY_DOWN	      -112
#  define RETROFLAT_KEY_RIGHT	      -111
#  define RETROFLAT_KEY_LEFT	      -110
#  define RETROFLAT_KEY_A	         0x41
#  define RETROFLAT_KEY_B	         0x42
#  define RETROFLAT_KEY_C	         0x43
#  define RETROFLAT_KEY_D	         0x44
#  define RETROFLAT_KEY_E	         0x45
#  define RETROFLAT_KEY_F	         0x46
#  define RETROFLAT_KEY_G	         0x47
#  define RETROFLAT_KEY_H	         0x48
#  define RETROFLAT_KEY_I	         0x49
#  define RETROFLAT_KEY_J	         0x4a
#  define RETROFLAT_KEY_K	         0x4b
#  define RETROFLAT_KEY_L	         0x4c
#  define RETROFLAT_KEY_M	         0x4d
#  define RETROFLAT_KEY_N	         0x4e
#  define RETROFLAT_KEY_O	         0x4f
#  define RETROFLAT_KEY_P	         0x50
#  define RETROFLAT_KEY_Q	         0x51
#  define RETROFLAT_KEY_R	         0x52
#  define RETROFLAT_KEY_S	         0x53
#  define RETROFLAT_KEY_T	         0x54
#  define RETROFLAT_KEY_U	         0x55
#  define RETROFLAT_KEY_V	         0x56
#  define RETROFLAT_KEY_W	         0x57
#  define RETROFLAT_KEY_X	         0x58
#  define RETROFLAT_KEY_Y	         0x59
#  define RETROFLAT_KEY_Z	         0x5a
#  define RETROFLAT_KEY_0           0x30
#  define RETROFLAT_KEY_1           0x31
#  define RETROFLAT_KEY_2           0x32
#  define RETROFLAT_KEY_3           0x33
#  define RETROFLAT_KEY_4           0x34
#  define RETROFLAT_KEY_5           0x35
#  define RETROFLAT_KEY_6           0x36
#  define RETROFLAT_KEY_7           0x37
#  define RETROFLAT_KEY_8           0x38
#  define RETROFLAT_KEY_9           0x39
#  define RETROFLAT_KEY_TAB	      '\t'
#  define RETROFLAT_KEY_SPACE	      ' '
#  define RETROFLAT_KEY_ESC	      -98
#  define RETROFLAT_KEY_ENTER	      '\n'
#  define RETROFLAT_KEY_HOME	      -99
#  define RETROFLAT_KEY_END	      -100
#  define RETROFLAT_KEY_DELETE      -101
#  define RETROFLAT_KEY_PGUP        -102
#  define RETROFLAT_KEY_PGDN        -103
#  define RETROFLAT_KEY_GRAVE       '`'
#  define RETROFLAT_KEY_SLASH       '/'
#  define RETROFLAT_KEY_BKSP        -104
#  define RETROFLAT_KEY_SEMICOLON   ';'
#  define RETROFLAT_KEY_PERIOD      '.'
#  define RETROFLAT_KEY_COMMA       ','
#  define RETROFLAT_KEY_EQUALS      '='
#  define RETROFLAT_KEY_DASH        '-'
#  define RETROFLAT_KEY_BACKSLASH   '\\'
#  define RETROFLAT_KEY_QUOTE       '"'
#  define RETROFLAT_KEY_BRACKETL    '['
#  define RETROFLAT_KEY_BRACKETR    ']'
#  define RETROFLAT_KEY_INSERT      -105

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2


#  define RETROFLAT_MOUSE_B_LEFT    0
#  define RETROFLAT_MOUSE_B_RIGHT   0

/*! \} */ /* maug_retroflt_keydefs */

/*! \} */ /* maug_retroflt_input */

#elif defined( RETROFLT_C )

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   /* TODO */

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

