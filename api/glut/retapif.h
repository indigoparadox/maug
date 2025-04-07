
#ifndef RETPLTF_H
#define RETPLTF_H

#ifdef RETROFLAT_OS_OS2
void APIENTRY
#else
void
#endif /* RETROFLAT_OS_OS2 */
retroflat_glut_display( void ) {
   /* TODO: Work in frame_iter if provided. */
   if( NULL != g_retroflat_state->loop_iter ) {
      g_retroflat_state->loop_iter( g_retroflat_state->loop_data );
   }
   if( NULL != g_retroflat_state->frame_iter ) {
      g_retroflat_state->frame_iter( g_retroflat_state->loop_data );
   }
}

/* === */

#ifdef RETROFLAT_OS_OS2
void APIENTRY
#else
void
#endif /* RETROFLAT_OS_OS2 */
retroflat_glut_idle( void ) {
   uint32_t now = 0;

   now = retroflat_get_ms();
   if(
      RETROFLAT_FLAGS_UNLOCK_FPS !=
      (RETROFLAT_FLAGS_UNLOCK_FPS & g_retroflat_state->retroflat_flags) &&
      now < g_retroflat_state->platform.retroflat_next
   ) {
      return;
   }
   
   glutPostRedisplay();

   if( now + retroflat_fps_next() > now ) {
      g_retroflat_state->platform.retroflat_next = now + retroflat_fps_next();
   } else {
      /* Rollover protection. */
      g_retroflat_state->platform.retroflat_next = 0;
   }
}

/* === */

#     ifdef RETROFLAT_OS_OS2
void APIENTRY
#     elif defined( RETROFLAT_OS_WIN )
void 
#     else
void
#     endif /* RETROFLAT_OS_OS2 */
retroflat_glut_key( unsigned char key, int x, int y ) {
#     ifdef RETROFLAT_OS_WIN
      /* key -= 109; */
#     endif /* RETROFLAT_OS_WIN */
   debug_printf( 0, "key: %c (0x%02x)", key, key );
   g_retroflat_state->platform.retroflat_last_key = key;
}

/* === */

MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
   unsigned int glut_init_flags = 0;

   /* == GLUT == */

#     define RETROFLAT_COLOR_TABLE_GLUT( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
         g_retroflat_state->palette[idx] = RETROGLU_COLOR_ ## name_u;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_GLUT )

   /* We cap out at 16 colors. */
   g_retroflat_state->screen_colors = 16;

   g_retroflat_state->screen_v_w = args->screen_w;
   g_retroflat_state->screen_v_h = args->screen_h;

   glutInit( &argc, argv );
   glut_init_flags = GLUT_DEPTH | GLUT_RGBA;
   if(
      RETROFLAT_FLAGS_UNLOCK_FPS != (RETROFLAT_FLAGS_UNLOCK_FPS & args->flags)
   ) {
      glut_init_flags |= GLUT_DOUBLE;
   }
   glutInitDisplayMode( glut_init_flags );
   if( 0 < args->screen_x || 0 < args->screen_y ) {
      glutInitWindowPosition( args->screen_x, args->screen_y );
   }
   /* TODO: Handle screen scaling? */
   glutInitWindowSize(
      g_retroflat_state->screen_w, g_retroflat_state->screen_h );
   glutCreateWindow( args->title );
   glutIdleFunc( retroflat_glut_idle );
   glutDisplayFunc( retroflat_glut_display );
   glutKeyboardFunc( retroflat_glut_key );

   /* TODO: Handle mouse input in GLUT. */

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {

}

/* === */

MERROR_RETVAL retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;

   g_retroflat_state->loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;
   g_retroflat_state->frame_iter = (retroflat_loop_iter)frame_iter;

   if(
      RETROFLAT_FLAGS_RUNNING ==
      (g_retroflat_state->retroflat_flags & RETROFLAT_FLAGS_RUNNING)
   ) {
      /* Main loop is already running, so we're just changing the iter call
       * and leaving!
       */
      goto cleanup;
   }

   g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_RUNNING;

   glutMainLoop();
   retval = g_retroflat_state->retval;

cleanup:

   /* This should be set by retroflat_quit(). */
   return retval;
}

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;
#  ifdef RETROFLAT_API_SDL2
   uint32_t sdl_msg_flags = 0;
#  elif defined( RETROFLAT_OS_WIN )
   uint32_t win_msg_flags = 0;
#  endif

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

#  if defined( RETROFLAT_OS_WIN )

   switch( (flags & RETROFLAT_MSG_FLAG_TYPE_MASK) ) {
   case RETROFLAT_MSG_FLAG_ERROR:
      win_msg_flags |= MB_ICONSTOP;
      break;

   case RETROFLAT_MSG_FLAG_INFO:
      win_msg_flags |= MB_ICONINFORMATION;
      break;

   case RETROFLAT_MSG_FLAG_WARNING:
      win_msg_flags |= MB_ICONEXCLAMATION;
      break;
   }

   MessageBox( retroflat_root_win(), msg_out, title, win_msg_flags );

#  else  

   /* TODO */
   error_printf( "%s", msg_out );

#  endif

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Build the title. */
   va_start( vargs, format );
   memset( title, '\0', RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

   glutSetWindowTitle( title );

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   return glutGet( GLUT_ELAPSED_TIME );
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   return retroglu_draw_lock( bmp );
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   return retroglu_draw_release( bmp );
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, RETROFLAT_PATH_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

   retval = retroglu_load_bitmap( filename_path, bmp_out, flags );

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   return retroglu_create_bitmap( w, h, bmp_out, flags );
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {
   retroglu_destroy_bitmap( bmp );
}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != src );

   retval = 
      retroglu_blit_bitmap( target, src, s_x, s_y, d_x, d_y, w, h, instance );

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags
) {
   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   retroflat_constrain_px( x, y, target, return );

   retroglu_px( target, color_idx, x, y, flags );
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   /* TODO: Implement RETROFLAT_MOD_SHIFT. */

   key_out = g_retroflat_state->platform.retroflat_last_key;
   g_retroflat_state->platform.retroflat_last_key = 0;

   /* TODO: Handle mouse. */

   return key_out;
}

/* === */

void retroflat_resize_v() {
   /* TODO */
}

/* === */

MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( 2 <= sizeof( MERROR_RETVAL ) );

#  pragma message( "warning: init not implemented" )

   return retval;
}

/* === */

void retrosnd_midi_set_sf_bank( const char* filename_in ) {
#  pragma message( "warning: set_sf_bank not implemented" )
}

/* === */

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#  pragma message( "warning: set_voice not implemented" )
}

/* === */

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#  pragma message( "warning: set_control not implemented" )
}

/* === */

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#  pragma message( "warning: note_on not implemented" )

}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#  pragma message( "warning: note_off not implemented" )

}

/* === */

MERROR_RETVAL retrosnd_midi_play_smf( const char* filename ) {
   MERROR_RETVAL retval = MERROR_OK;

#  pragma message( "warning: midi_play_smf not implemented" )

   return retval;
}

/* === */

uint8_t retrosnd_midi_is_playing_smf() {
#  pragma message( "warning: midi_play_smf not implemented" )
   return 1;
}

/* === */

void retrosnd_shutdown() {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#  pragma message( "warning: shutdown not implemented" )
}


#endif /* !RETPLTF_H */

