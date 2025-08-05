
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

#include <gccore.h>
#include <wiiuse/wpad.h>

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
};

typedef uint32_t RETROFLAT_IN_KEY;

#  ifdef RETROFLAT_NDS_WASD
#     define RETROFLAT_KEY_A           WPAD_BUTTON_LEFT
#     define RETROFLAT_KEY_D           WPAD_BUTTON_RIGHT
#     define RETROFLAT_KEY_W           WPAD_BUTTON_UP
#     define RETROFLAT_KEY_S           WPAD_BUTTON_DOWN
#  else
#     define RETROFLAT_KEY_LEFT        WPAD_BUTTON_LEFT
#     define RETROFLAT_KEY_RIGHT       WPAD_BUTTON_RIGHT
#     define RETROFLAT_KEY_UP          WPAD_BUTTON_UP
#     define RETROFLAT_KEY_DOWN        WPAD_BUTTON_DOWN
#  endif /* RETROFLAT_NDS_WASD */
#  define RETROFLAT_KEY_ENTER       WPAD_BUTTON_PLUS
#  define RETROFLAT_KEY_SPACE       WPAD_BUTTON_1
#  define RETROFLAT_KEY_ESC         WPAD_BUTTON_HOME
#  define RETROFLAT_MOUSE_B_LEFT    WPAD_BUTTON_B
#  define RETROFLAT_MOUSE_B_RIGHT   WPAD_BUTTON_A

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

