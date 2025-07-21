
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

/* Sniffed most of these using xev or printf() on the keycode directly and
 * guessed the rest. They appear to be in QWERTY order.
 */
#  define RETROFLAT_KEY_UP	         111
#  define RETROFLAT_KEY_DOWN	      116
#  define RETROFLAT_KEY_RIGHT	      114
#  define RETROFLAT_KEY_LEFT	      113
#  define RETROFLAT_KEY_A	         38
#  define RETROFLAT_KEY_B	         56
#  define RETROFLAT_KEY_C	         54
#  define RETROFLAT_KEY_D	         40
#  define RETROFLAT_KEY_E	         26
#  define RETROFLAT_KEY_F	         41
#  define RETROFLAT_KEY_G	         42
#  define RETROFLAT_KEY_H	         43
#  define RETROFLAT_KEY_I	         31
#  define RETROFLAT_KEY_J	         44
#  define RETROFLAT_KEY_K	         45
#  define RETROFLAT_KEY_L	         46
#  define RETROFLAT_KEY_M	         58
#  define RETROFLAT_KEY_N	         57
#  define RETROFLAT_KEY_O	         32
#  define RETROFLAT_KEY_P	         33
#  define RETROFLAT_KEY_Q	         24
#  define RETROFLAT_KEY_R	         27
#  define RETROFLAT_KEY_S	         39
#  define RETROFLAT_KEY_T	         28
#  define RETROFLAT_KEY_U	         30
#  define RETROFLAT_KEY_V	         55
#  define RETROFLAT_KEY_W	         25
#  define RETROFLAT_KEY_X	         53
#  define RETROFLAT_KEY_Y	         29
#  define RETROFLAT_KEY_Z	         52
#  define RETROFLAT_KEY_0           19
#  define RETROFLAT_KEY_1           10
#  define RETROFLAT_KEY_2           11
#  define RETROFLAT_KEY_3           12
#  define RETROFLAT_KEY_4           13
#  define RETROFLAT_KEY_5           14
#  define RETROFLAT_KEY_6           15
#  define RETROFLAT_KEY_7           16
#  define RETROFLAT_KEY_8           17
#  define RETROFLAT_KEY_9           18
#  define RETROFLAT_KEY_TAB	      23
#  define RETROFLAT_KEY_SPACE	      65
#  define RETROFLAT_KEY_ESC	      9
#  define RETROFLAT_KEY_ENTER	      36
#  define RETROFLAT_KEY_HOME	      110
#  define RETROFLAT_KEY_END	      115
#  define RETROFLAT_KEY_DELETE      119
#  define RETROFLAT_KEY_PGUP        112
#  define RETROFLAT_KEY_PGDN        117
#  define RETROFLAT_KEY_GRAVE       49
#  define RETROFLAT_KEY_SLASH       61
#  define RETROFLAT_KEY_BKSP        22
#  define RETROFLAT_KEY_SEMICOLON   47
#  define RETROFLAT_KEY_PERIOD      60
#  define RETROFLAT_KEY_COMMA       59
#  define RETROFLAT_KEY_EQUALS      21
#  define RETROFLAT_KEY_DASH        20
#  define RETROFLAT_KEY_BACKSLASH   51
#  define RETROFLAT_KEY_QUOTE       48
#  define RETROFLAT_KEY_BRACKETL    34
#  define RETROFLAT_KEY_BRACKETR    35
#  define RETROFLAT_KEY_INSERT      118

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

/*! \} */ /* maug_retroflt_keydefs */

/*! \} */ /* maug_retroflt_input */

#elif defined( RETROFLT_C )

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;
   XEvent event;
   uint16_t mask = 0;

   assert( NULL != input );

   input->key_flags = 0;

   /* TODO: Mouse support. */

   mask = ExposureMask | KeyPressMask | KeyReleaseMask;

   /* XNextEvent( g_display, &event ); */
   if( (
      XCheckWindowEvent(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.window, mask, &event ) ||
      XCheckTypedWindowEvent(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.window, KeyPress, &event )
   ) && KeyPress == event.type ) {
      /* printf( "%lu\n", event.xkey.keycode ); */
      key_out = event.xkey.keycode;
   }

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

