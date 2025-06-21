
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
   uint8_t key_code;
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
#  define RETROFLAT_KEY_A	         0x00
#  define RETROFLAT_KEY_B	         0x0b
#  define RETROFLAT_KEY_C	         0x08
#  define RETROFLAT_KEY_D	         0x02
#  define RETROFLAT_KEY_E	         0x0e
#  define RETROFLAT_KEY_F	         0x03
#  define RETROFLAT_KEY_G	         0x05
#  define RETROFLAT_KEY_H	         0x04
#  define RETROFLAT_KEY_I	         0x22
#  define RETROFLAT_KEY_J	         0x26
#  define RETROFLAT_KEY_K	         0x28
#  define RETROFLAT_KEY_L	         0x25
#  define RETROFLAT_KEY_M	         0x2e
#  define RETROFLAT_KEY_N	         0x2d
#  define RETROFLAT_KEY_O	         0x1f
#  define RETROFLAT_KEY_P	         0x23
#  define RETROFLAT_KEY_Q	         0x0c
#  define RETROFLAT_KEY_R	         0x0f
#  define RETROFLAT_KEY_S	         0x01
#  define RETROFLAT_KEY_T	         0x11
#  define RETROFLAT_KEY_U	         0x20
#  define RETROFLAT_KEY_V	         0x09
#  define RETROFLAT_KEY_W	         0x0d
#  define RETROFLAT_KEY_X	         0x07
#  define RETROFLAT_KEY_Y	         0x10
#  define RETROFLAT_KEY_Z	         0x06
#  define RETROFLAT_KEY_0           0x1d
#  define RETROFLAT_KEY_1           0x12
#  define RETROFLAT_KEY_2           0x13
#  define RETROFLAT_KEY_3           0x14
#  define RETROFLAT_KEY_4           0x15
#  define RETROFLAT_KEY_5           0x17
#  define RETROFLAT_KEY_6           0x16
#  define RETROFLAT_KEY_7           0x1a
#  define RETROFLAT_KEY_8           0x1c
#  define RETROFLAT_KEY_9           0x19
#  define RETROFLAT_KEY_TAB	      0x30
#  define RETROFLAT_KEY_SPACE	      0x31
#  define RETROFLAT_KEY_ESC	      0x35
#  define RETROFLAT_KEY_ENTER	      0x24
#  define RETROFLAT_KEY_HOME	      0x73
#  define RETROFLAT_KEY_END	      0x77
#  define RETROFLAT_KEY_DELETE      0x75
#  define RETROFLAT_KEY_PGUP        0x74
#  define RETROFLAT_KEY_PGDN        0x79
#  define RETROFLAT_KEY_GRAVE       0x32
#  define RETROFLAT_KEY_SLASH       0x2c
#  define RETROFLAT_KEY_BKSP        0x33
#  define RETROFLAT_KEY_SEMICOLON   -1
#  define RETROFLAT_KEY_PERIOD      0x2f
#  define RETROFLAT_KEY_COMMA       0x2b
#  define RETROFLAT_KEY_EQUALS      0x18
#  define RETROFLAT_KEY_DASH        0x1b
#  define RETROFLAT_KEY_BACKSLASH   0x2a
#  define RETROFLAT_KEY_QUOTE       0x27
#  define RETROFLAT_KEY_BRACKETL    0x21
#  define RETROFLAT_KEY_BRACKETR    0x1e
#  define RETROFLAT_KEY_INSERT      -2

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

/*! \} */ /* maug_retroflt_keydefs */

/*! \} */ /* maug_retroflt_input */

#elif defined( RETROFLT_C )

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   /* TODO */
   if( 0xff != g_retroflat_state->input.key_code ) {
      key_out = g_retroflat_state->input.key_code;
      g_retroflat_state->input.key_code = 0xff;
      debug_printf( 1, "key: 0x%02x", key_out );
   }

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

