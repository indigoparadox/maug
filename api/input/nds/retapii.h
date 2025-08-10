
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

#define RETROFLAT_NO_KEYBOARD

#ifndef RETROFLAT_PREV_PAD_DELAY
#  define RETROFLAT_PREV_PAD_DELAY 4
#endif /* !RETROFLAT_PREV_PAD_DELAY */

typedef int16_t RETROFLAT_IN_KEY;

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
   RETROFLAT_IN_KEY prev_pad;
   int prev_pad_delay;
};

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

   g_retroflat_state->retroflat_flags |= 
      (args->flags & RETROFLAT_FLAGS_KEY_REPEAT);

   return MERROR_OK;
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;
   int keys_down = 0,
       keys_up = 0,
       keys_held = 0;

   assert( NULL != input );

   scanKeys();
   keys_down = keysDown();
   keys_up = keysUp();
   keys_held = keysHeld();

   input->key_flags = 0;

   /* Reset prev_pad repeat. */
   if(
      0 != g_retroflat_state->input.prev_pad &&
      g_retroflat_state->input.prev_pad ==
      (g_retroflat_state->input.prev_pad & keys_up)
   ) {
      g_retroflat_state->input.prev_pad = 0;
      g_retroflat_state->input.prev_pad_delay = 0;
      debug_printf( RETROINPUT_TRACE_LVL, "pad button reset to 0" );
   }

#  define retroflat_nds_buttons_poll_down( btn ) \
   if( btn == (btn & keys_down) ) { \
      debug_printf( RETROINPUT_TRACE_LVL, "pad button down: %d", btn ); \
      g_retroflat_state->input.prev_pad = btn; \
      g_retroflat_state->input.prev_pad_delay = RETROFLAT_PREV_PAD_DELAY; \
      key_out = btn; \
      goto cleanup; \
   }

   retroflat_nds_buttons( retroflat_nds_buttons_poll_down )

   /* TODO: Touch screen taps to mouse. */

   /* If no other event happened, repeat the last press. */
   key_out = retroflat_repeat_input( key_out, input,
      &(g_retroflat_state->input.prev_pad),
      &(g_retroflat_state->input.prev_pad_delay) );

cleanup:

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

