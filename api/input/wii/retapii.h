
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

#define RETROFLAT_NO_KEYBOARD

#include <gccore.h>
#include <wiiuse/wpad.h>

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
};

typedef uint32_t RETROFLAT_IN_KEY;

#define RETROFLAT_PAD_LEFT        WPAD_BUTTON_UP
#define RETROFLAT_PAD_RIGHT       WPAD_BUTTON_DOWN
#define RETROFLAT_PAD_UP          WPAD_BUTTON_RIGHT
#define RETROFLAT_PAD_DOWN        WPAD_BUTTON_LEFT
#define RETROFLAT_PAD_START       WPAD_BUTTON_PLUS
#define RETROFLAT_PAD_B           WPAD_BUTTON_1
#define RETROFLAT_PAD_A           WPAD_BUTTON_2
#define RETROFLAT_MOUSE_B_LEFT    WPAD_BUTTON_B
#define RETROFLAT_MOUSE_B_RIGHT   WPAD_BUTTON_A

#define retroflat_wii_buttons( f ) \
   f( WPAD_BUTTON_LEFT ) \
   f( WPAD_BUTTON_RIGHT ) \
   f( WPAD_BUTTON_UP ) \
   f( WPAD_BUTTON_DOWN ) \
   f( WPAD_BUTTON_A ) \
   f( WPAD_BUTTON_B ) \
   f( WPAD_BUTTON_1 ) \
   f( WPAD_BUTTON_2 ) \
   f( WPAD_BUTTON_MINUS ) \
   f( WPAD_BUTTON_PLUS ) \
   f( WPAD_BUTTON_HOME )

#elif defined( RETROFLT_C )

MERROR_RETVAL retroflat_init_input( struct RETROFLAT_ARGS* args ) {
   WPAD_Init();
   return MERROR_OK;
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;
   uint32_t buttons_down = 0;

   assert( NULL != input );

   WPAD_ScanPads();
   buttons_down = WPAD_ButtonsDown( 0 );

   input->key_flags = 0;

#  define retroflat_wii_buttons_poll( btn ) \
   if( btn == (btn & buttons_down) ) { \
      return btn; \
   }

   retroflat_wii_buttons( retroflat_wii_buttons_poll )

   /* TODO: Touch screen taps to mouse. */

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

