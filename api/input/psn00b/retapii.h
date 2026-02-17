
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

#include <psxpad.h>

#define RETROFLAT_NO_KEYBOARD

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
   uint8_t padbuff[2][34];
};

typedef int16_t RETROFLAT_IN_KEY;

#define RETROFLAT_PAD_LEFT        PAD_LEFT
#define RETROFLAT_PAD_RIGHT       PAD_RIGHT
#define RETROFLAT_PAD_UP          PAD_UP
#define RETROFLAT_PAD_DOWN        PAD_DOWN
#define RETROFLAT_PAD_SHOULDER_R  PAD_R1
#define RETROFLAT_PAD_SHOULDER_L  PAD_L1
#define RETROFLAT_PAD_START       PAD_START
#define RETROFLAT_PAD_A           PAD_CROSS
#define RETROFLAT_PAD_B           PAD_CIRCLE
#define RETROFLAT_MOUSE_B_LEFT    (-1)
#define RETROFLAT_MOUSE_B_RIGHT   (-2)

#define RETROGUI_KEY_ACTIVATE RETROFLAT_PAD_A
#define RETROGUI_KEY_NEXT RETROFLAT_PAD_RIGHT
#define RETROGUI_KEY_PREV RETROFLAT_PAD_LEFT

#define retroflat_psx_buttons( f ) \
   f( PAD_LEFT ) \
   f( PAD_RIGHT ) \
   f( PAD_UP ) \
   f( PAD_DOWN ) \
   f( PAD_START ) \
   f( PAD_CIRCLE ) \
   f( PAD_CROSS ) \
   f( PAD_L1 ) \
   f( PAD_R1 )

/*! \} */ /* maug_retroflt_input */

#elif defined( RETROFLT_C )

MERROR_RETVAL retroflat_init_input( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;

   InitPAD(
      g_retroflat_state->input.padbuff[0], 34,
      g_retroflat_state->input.padbuff[1], 34 );
   StartPAD();
   ChangeClearPAD( 0 );

   return retval;
}

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;
   uint16_t pad_status = *(uint16_t*)&(g_retroflat_state->input.padbuff[0][2]);

   assert( NULL != input );

   input->key_flags = 0;

   #define retroflat_psx_buttons_poll_down( b ) \
      } else if( !((b) & pad_status) ) { \
         return b;

   if( 0 ) {
   retroflat_psx_buttons( retroflat_psx_buttons_poll_down )
   }

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

