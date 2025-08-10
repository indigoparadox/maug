
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

#define RETROFLAT_NO_KEYBOARD

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
};

typedef int16_t RETROFLAT_IN_KEY;

#define RETROFLAT_PAD_LEFT        KEY_LEFT
#define RETROFLAT_PAD_RIGHT       KEY_RIGHT
#define RETROFLAT_PAD_UP          KEY_UP
#define RETROFLAT_PAD_DOWN        KEY_DOWN
#define RETROFLAT_PAD_SHOULDER_R  KEY_R
#define RETROFLAT_PAD_SHOULDER_L  KEY_L
#define RETROFLAT_PAD_START       KEY_START
#define RETROFLAT_PAD_A           KEY_A
#define RETROFLAT_PAD_B           KEY_B
#define RETROFLAT_MOUSE_B_LEFT    (-1)
#define RETROFLAT_MOUSE_B_RIGHT   (-2)

#define RETROGUI_KEY_ACTIVATE RETROFLAT_PAD_A
#define RETROGUI_KEY_NEXT RETROFLAT_PAD_RIGHT
#define RETROGUI_KEY_PREV RETROFLAT_PAD_LEFT

#define retroflat_nds_buttons( f ) \
   f( KEY_LEFT ) \
   f( KEY_RIGHT ) \
   f( KEY_UP ) \
   f( KEY_DOWN ) \
   f( KEY_START ) \
   f( KEY_A ) \
   f( KEY_B ) \
   f( KEY_L ) \
   f( KEY_R )

#elif defined( RETROFLT_C )

MERROR_RETVAL retroflat_init_input( struct RETROFLAT_ARGS* args ) {
   return MERROR_OK;
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;
   int keys = 0;

   assert( NULL != input );

   scanKeys();
   keys = keysDown();

   input->key_flags = 0;

#  define retroflat_nds_buttons_poll( btn ) \
   if( btn == (btn & keys) ) { \
      return btn; \
   }

   retroflat_nds_buttons( retroflat_nds_buttons_poll )

   /* TODO: Touch screen taps to mouse. */

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

