
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

#ifdef RETROFLAT_API_SDL2
typedef int32_t RETROFLAT_IN_KEY;
#else
typedef int16_t RETROFLAT_IN_KEY;
#endif /* RETROFLAT_API_SDL2 */

#elif defined( RETROFLT_C )

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
      debug_printf( 1, "quit event!" );
      retroflat_quit( 0 );
      break;

   case SDL_KEYDOWN:
      key_out = event.key.keysym.sym;

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

      /* Flush key buffer to improve responsiveness. */
      if(
         RETROFLAT_FLAGS_KEY_REPEAT !=
         (RETROFLAT_FLAGS_KEY_REPEAT & g_retroflat_state->retroflat_flags)
      ) {
         while( (eres = SDL_PollEvent( &event )) );
      }
      break;

   case SDL_MOUSEBUTTONUP:
      /* Stop dragging. */
      g_retroflat_state->platform.mouse_state = 0;
      break;

   case SDL_MOUSEBUTTONDOWN:

      /* Begin dragging. */

      input->mouse_x = event.button.x / g_retroflat_state->scale;
      input->mouse_y = event.button.y / g_retroflat_state->scale;

      /* Differentiate which button was clicked. */
      if( SDL_BUTTON_LEFT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_LEFT;
         g_retroflat_state->platform.mouse_state = RETROFLAT_MOUSE_B_LEFT;
      } else if( SDL_BUTTON_RIGHT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_RIGHT;
         g_retroflat_state->platform.mouse_state = RETROFLAT_MOUSE_B_RIGHT;
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
      if( 0 != g_retroflat_state->platform.mouse_state ) {
         /* Update coordinates and keep dragging. */
         SDL_GetMouseState( &(input->mouse_x), &(input->mouse_y) );
         key_out = g_retroflat_state->platform.mouse_state;
      }
      break;
   }

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

