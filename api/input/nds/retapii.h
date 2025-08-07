
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
};

typedef int16_t RETROFLAT_IN_KEY;

#  ifdef RETROFLAT_NDS_WASD
#     define RETROFLAT_KEY_A           KEY_LEFT
#     define RETROFLAT_KEY_D           KEY_RIGHT
#     define RETROFLAT_KEY_W           KEY_UP
#     define RETROFLAT_KEY_S           KEY_DOWN
#     define RETROFLAT_KEY_Q           KEY_L
#     define RETROFLAT_KEY_E           KEY_R
#  else
#     define RETROFLAT_KEY_LEFT        KEY_LEFT
#     define RETROFLAT_KEY_RIGHT       KEY_RIGHT
#     define RETROFLAT_KEY_UP          KEY_UP
#     define RETROFLAT_KEY_DOWN        KEY_DOWN
#     define RETROFLAT_KEY_INSERT      KEY_R
#     define RETROFLAT_KEY_DELETE      KEY_L
#  endif /* RETROFLAT_NDS_WASD */
#  define RETROFLAT_KEY_ENTER       KEY_START
#  define RETROFLAT_KEY_SPACE       KEY_A
#  define RETROFLAT_KEY_ESC         KEY_B
#  define RETROFLAT_MOUSE_B_LEFT    (-1)
#  define RETROFLAT_MOUSE_B_RIGHT   (-2)

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

