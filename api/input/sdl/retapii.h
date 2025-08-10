
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

#include <SDL.h>

#ifndef RETROFLAT_PREV_PAD_DELAY
#  define RETROFLAT_PREV_PAD_DELAY 4
#endif /* !RETROFLAT_PREV_PAD_DELAY */

#define RETROFLAT_KEY_UP	SDLK_UP
#define RETROFLAT_KEY_DOWN  SDLK_DOWN
#define RETROFLAT_KEY_RIGHT	SDLK_RIGHT
#define RETROFLAT_KEY_LEFT	SDLK_LEFT
#define RETROFLAT_KEY_A	   SDLK_a
#define RETROFLAT_KEY_B	   SDLK_b
#define RETROFLAT_KEY_C	   SDLK_c
#define RETROFLAT_KEY_D	   SDLK_d
#define RETROFLAT_KEY_E	   SDLK_e
#define RETROFLAT_KEY_F	   SDLK_f
#define RETROFLAT_KEY_G	   SDLK_g
#define RETROFLAT_KEY_H	   SDLK_h
#define RETROFLAT_KEY_I	   SDLK_i
#define RETROFLAT_KEY_J	   SDLK_j
#define RETROFLAT_KEY_K	   SDLK_k
#define RETROFLAT_KEY_L	   SDLK_l
#define RETROFLAT_KEY_M	   SDLK_m
#define RETROFLAT_KEY_N	   SDLK_n
#define RETROFLAT_KEY_O	   SDLK_o
#define RETROFLAT_KEY_P	   SDLK_p
#define RETROFLAT_KEY_Q	   SDLK_q
#define RETROFLAT_KEY_R	   SDLK_r
#define RETROFLAT_KEY_S	   SDLK_s
#define RETROFLAT_KEY_T	   SDLK_t
#define RETROFLAT_KEY_U	   SDLK_u
#define RETROFLAT_KEY_V	   SDLK_v
#define RETROFLAT_KEY_W	   SDLK_w
#define RETROFLAT_KEY_X	   SDLK_x
#define RETROFLAT_KEY_Y	   SDLK_y
#define RETROFLAT_KEY_Z	   SDLK_z
#define RETROFLAT_KEY_0     SDLK_0
#define RETROFLAT_KEY_1     SDLK_1
#define RETROFLAT_KEY_2     SDLK_2
#define RETROFLAT_KEY_3     SDLK_3
#define RETROFLAT_KEY_4     SDLK_4
#define RETROFLAT_KEY_5     SDLK_5
#define RETROFLAT_KEY_6     SDLK_6
#define RETROFLAT_KEY_7     SDLK_7
#define RETROFLAT_KEY_8     SDLK_8
#define RETROFLAT_KEY_9     SDLK_9
#define RETROFLAT_KEY_TAB	SDLK_TAB
#define RETROFLAT_KEY_SPACE	SDLK_SPACE
#define RETROFLAT_KEY_ESC	SDLK_ESCAPE
#define RETROFLAT_KEY_ENTER	SDLK_RETURN
#define RETROFLAT_KEY_HOME	SDLK_HOME
#define RETROFLAT_KEY_END	SDLK_END
#define RETROFLAT_KEY_DELETE   SDLK_DELETE
#define RETROFLAT_KEY_PGUP     SDLK_PAGEUP
#define RETROFLAT_KEY_PGDN     SDLK_PAGEDOWN
#define RETROFLAT_KEY_GRAVE SDLK_BACKQUOTE
#define RETROFLAT_KEY_SLASH SDLK_SLASH
#define RETROFLAT_KEY_BKSP  SDLK_BACKSPACE
#define RETROFLAT_KEY_SEMICOLON   SDLK_SEMICOLON
#define RETROFLAT_KEY_PERIOD   SDLK_PERIOD
#define RETROFLAT_KEY_COMMA    SDLK_COMMA
#define RETROFLAT_KEY_EQUALS   SDLK_EQUALS
#define RETROFLAT_KEY_DASH     SDLK_MINUS
#define RETROFLAT_KEY_BACKSLASH  SDLK_BACKSLASH
#define RETROFLAT_KEY_QUOTE      SDLK_QUOTE
#define RETROFLAT_KEY_BRACKETL   SDLK_LEFTBRACKET
#define RETROFLAT_KEY_BRACKETR   SDLK_RIGHTBRACKET
#define RETROFLAT_KEY_INSERT   SDLK_INSERT

#define RETROFLAT_MOUSE_B_LEFT    (-1)
#define RETROFLAT_MOUSE_B_RIGHT   (-2)

#define RETROFLAT_PAD_LEFT          (-10)
#define RETROFLAT_PAD_RIGHT         (-11)
#define RETROFLAT_PAD_UP            (-12)
#define RETROFLAT_PAD_DOWN          (-13)
#define RETROFLAT_PAD_SHOULDER_R    (-14)
#define RETROFLAT_PAD_SHOULDER_L    (-15)
#define RETROFLAT_PAD_START         (-16)
#define RETROFLAT_PAD_SELECT        (-17)
#define RETROFLAT_PAD_A             (-18)
#define RETROFLAT_PAD_B             (-19)
#define RETROFLAT_PAD_X             (-20)
#define RETROFLAT_PAD_Y             (-21)

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
   int mouse_state;
   int prev_pad;
   int prev_pad_delay;
   SDL_Keycode prev_key;
#ifdef RETROFLAT_API_SDL2
   SDL_GameController* pad;
#elif defined( RETROFLAT_API_SDL1 )
   SDL_Joystick* pad;
#  endif /* RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 */
};

#ifdef RETROFLAT_API_SDL2
typedef int32_t RETROFLAT_IN_KEY;
#elif defined( RETROFLAT_API_SDL1 )
typedef int16_t RETROFLAT_IN_KEY;
#endif /* RETROFLAT_API_SDL2 */

#elif defined( RETROFLT_C )

#  ifdef RETROFLAT_API_SDL2

MERROR_RETVAL _retroflat_sdl_init_joystick( int joystick_id ) {
   assert( 0 <= joystick_id );
   if( 1 > SDL_NumJoysticks() ) {
      error_printf( "no gamepad connected!" );

   } else if( SDL_IsGameController( joystick_id ) ) {
      g_retroflat_state->input.pad =
         SDL_GameControllerOpen( joystick_id );
      if( NULL == g_retroflat_state->input.pad ) {
         error_printf( "unable to open gamepad: %s", SDL_GetError() );
      } else {
         debug_printf( 1, "initialized gamepad: %d", joystick_id );
         return MERROR_OK;
      }

   } else {
      error_printf( "joystick %d is not a known gamepad!", joystick_id );
   }

   return MERROR_USR;
}

#  endif

MERROR_RETVAL retroflat_init_input( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   g_retroflat_state->retroflat_flags |= 
      (args->flags & RETROFLAT_FLAGS_KEY_REPEAT);

#  ifdef RETROFLAT_API_SDL1
   /* Setup key repeat. */
   if(
      RETROFLAT_FLAGS_KEY_REPEAT == (RETROFLAT_FLAGS_KEY_REPEAT & args->flags)
   ) {
      if( 0 != SDL_EnableKeyRepeat(
         1, SDL_DEFAULT_REPEAT_INTERVAL
      ) ) {
         error_printf( "could not enable key repeat!" );
      } else {
         debug_printf( 2, "key repeat enabled" );
      }
   }

   /* Setup joysticks. */
   if( 1 > SDL_NumJoysticks() ) {
      error_printf( "no gamepad connected!" );
   } else {
      g_retroflat_state->input.pad = SDL_JoystickOpen( args->joystick_id );
      if( NULL == g_retroflat_state->input.pad ) {
         error_printf( "unable to open gamepad: %s", SDL_GetError() );
      } else {
         debug_printf( 1, "initialized gamepad: %d", args->joystick_id );
      }
   }

#  elif defined( RETROFLAT_API_SDL2 )
   if(
      RETROFLAT_FLAGS_KEY_REPEAT == (RETROFLAT_FLAGS_KEY_REPEAT & args->flags)
   ) {
      debug_printf( 2, "key repeat enabled" );
   }

   /* Break this out so it can be shared with insertion detection below. */
   if( 0 > args->joystick_id ) {
      for( i = 0 ; SDL_NumJoysticks() > i ; i++ ) {
         if( MERROR_OK == _retroflat_sdl_init_joystick( i ) ) {
            break;
         }
      }
   } else {
      _retroflat_sdl_init_joystick( args->joystick_id );
   }
#  endif /* RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 */

   return retval;
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   int eres = 0;
   SDL_Event event;
   RETROFLAT_IN_KEY key_out = 0;
   int mouse_x, mouse_y;

   assert( NULL != input );

   maug_mzero( &event, sizeof( SDL_Event ) );

   input->key_flags = 0;

   /* Always get mouse state, e.g. for hover detection. */
   SDL_GetMouseState( &mouse_x, &mouse_y );
   input->mouse_x = mouse_x / g_retroflat_state->scale;
   input->mouse_y = mouse_y / g_retroflat_state->scale;

   SDL_PollEvent( &event );

   switch( event.type ) {
   case SDL_QUIT:
      /* Handle SDL window close. */
      debug_printf( RETROINPUT_TRACE_LVL, "quit event!" );
      retroflat_quit( 0 );
      break;

#  ifdef RETROFLAT_API_SDL1
   case SDL_JOYBUTTONDOWN:
      /*
      switch( event.jbutton.button ) {
      }
      */
      /* TODO: Work out mappings for XBox controller. */
      debug_printf(
         RETROINPUT_TRACE_LVL, "gamebutton: %d", event.jbutton.button );

#  elif defined( RETROFLAT_API_SDL2 )
   case SDL_CONTROLLERBUTTONDOWN:
      switch( event.cbutton.button ) {
      case SDL_CONTROLLER_BUTTON_DPAD_UP:
         key_out = RETROFLAT_PAD_UP; break;
      case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
         key_out = RETROFLAT_PAD_DOWN; break;
      case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
         key_out = RETROFLAT_PAD_RIGHT; break;
      case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
         key_out = RETROFLAT_PAD_LEFT; break;
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
         key_out = RETROFLAT_PAD_SHOULDER_L; break;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
         key_out = RETROFLAT_PAD_SHOULDER_R; break;
      case SDL_CONTROLLER_BUTTON_START:
         key_out = RETROFLAT_PAD_START; break;
      case SDL_CONTROLLER_BUTTON_BACK:
         key_out = RETROFLAT_PAD_SELECT; break;
      case SDL_CONTROLLER_BUTTON_A:
         key_out = RETROFLAT_PAD_A; break;
      case SDL_CONTROLLER_BUTTON_B:
         key_out = RETROFLAT_PAD_B; break;
      case SDL_CONTROLLER_BUTTON_X:
         key_out = RETROFLAT_PAD_X; break;
      case SDL_CONTROLLER_BUTTON_Y:
         key_out = RETROFLAT_PAD_Y; break;
      }

#  endif /* RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 */

      g_retroflat_state->input.prev_pad = key_out;
      g_retroflat_state->input.prev_pad_delay = RETROFLAT_PREV_PAD_DELAY;

      debug_printf( RETROINPUT_TRACE_LVL, "pad button: %d", key_out );

      /* Flush event buffer to improve responsiveness. */
      while( (eres = SDL_PollEvent( &event )) );
      break;

   /* Detect button release for repeat emulation. */
#  ifdef RETROFLAT_API_SDL1
   case SDL_JOYBUTTONUP:
      g_retroflat_state->input.prev_pad = 0;
      g_retroflat_state->input.prev_pad_delay = 0;

#  elif defined( RETROFLAT_API_SDL2 )
   case SDL_CONTROLLERBUTTONUP:
      g_retroflat_state->input.prev_pad = 0;
      g_retroflat_state->input.prev_pad_delay = 0;

#  endif /* RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 */
      debug_printf( RETROINPUT_TRACE_LVL, "pad button reset to 0" );
      break;

   case SDL_KEYDOWN:
      if(
         RETROFLAT_FLAGS_KEY_REPEAT ==
         (RETROFLAT_FLAGS_KEY_REPEAT & g_retroflat_state->retroflat_flags) ||
         event.key.keysym.sym != g_retroflat_state->input.prev_key
      ) {
         debug_printf( RETROINPUT_TRACE_LVL, "key: %d", event.key.keysym.sym );
         key_out = event.key.keysym.sym;
         g_retroflat_state->input.prev_key = event.key.keysym.sym;
      }

      if(
         KMOD_RSHIFT == (KMOD_RSHIFT & event.key.keysym.mod) ||
         KMOD_LSHIFT == (KMOD_LSHIFT & event.key.keysym.mod) 
      ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_SHIFT;
      }

      if( KMOD_CTRL == (KMOD_CTRL & event.key.keysym.mod) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_CTRL;
      }

      if( KMOD_ALT == (KMOD_ALT & event.key.keysym.mod) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_ALT;
      }

      /* Flush event buffer to improve responsiveness. */
      while( (eres = SDL_PollEvent( &event )) );
      break;

   case SDL_KEYUP:
      g_retroflat_state->input.prev_key = 0;
      break;

   case SDL_MOUSEBUTTONUP:
      /* Stop dragging. */
      g_retroflat_state->input.mouse_state = 0;
      break;

   case SDL_MOUSEBUTTONDOWN:

      /* Begin dragging. */

      input->mouse_x = event.button.x / g_retroflat_state->scale;
      input->mouse_y = event.button.y / g_retroflat_state->scale;

      /* Differentiate which button was clicked. */
      if( SDL_BUTTON_LEFT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_LEFT;
         g_retroflat_state->input.mouse_state = RETROFLAT_MOUSE_B_LEFT;
      } else if( SDL_BUTTON_RIGHT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_RIGHT;
         g_retroflat_state->input.mouse_state = RETROFLAT_MOUSE_B_RIGHT;
      }

      /* Flush key buffer to improve responsiveness. */
      /*while( (eres = SDL_PollEvent( &event )) );*/
      break;

#  if !defined( RETROFLAT_API_SDL1 )
   case SDL_WINDOWEVENT:
      switch( event.window.event ) {
      case SDL_WINDOWEVENT_RESIZED:
         retroflat_on_resize( event.window.data1, event.window.data2 );
         if( NULL != g_retroflat_state->on_resize ) {
            g_retroflat_state->on_resize(
               event.window.data1, event.window.data2,
               g_retroflat_state->on_resize_data );
         }
         break;
      }
      break;
#  endif /* !RETROFLAT_API_SDL1 */

   default:
      /* Check for mouse dragging if mouse was previously held down. */
      if( 0 != g_retroflat_state->input.mouse_state ) {
         /* Update coordinates and keep dragging. */
         SDL_GetMouseState( &(input->mouse_x), &(input->mouse_y) );
         key_out = g_retroflat_state->input.mouse_state;
      }
      break;
   }

   key_out = retroflat_repeat_input( key_out, input,
      &(g_retroflat_state->input.prev_pad),
      &(g_retroflat_state->input.prev_pad_delay) );

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

