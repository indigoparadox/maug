
#if !defined( RETPLTS_H_DEFS )
#define RETPLTS_H_DEFS

#include <SDL_mixer.h>

struct RETROFLAT_SOUND_ARGS {
   uint8_t flags;
};

struct RETROFLAT_SOUND_STATE {
   uint8_t flags;
   Mix_Music* music;
   int audio_open;
};

#elif defined( RETROFLT_C )

MERROR_RETVAL retrosnd_cli_rsd(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO */

   return retval;
}

/* === */

MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero(
      &g_retroflat_state->sound, sizeof( struct RETROFLAT_SOUND_STATE ) );

   if( 0 > SDL_Init( SDL_INIT_AUDIO ) ) {
      error_printf( "couldn't initialize SDL audio: %s", SDL_GetError() );
      retval = MERROR_SND;
      goto cleanup;
   }

   if( !Mix_Init( MIX_INIT_OGG ) ) {
      error_printf( "couldn't initialize SDL mixer: %s", Mix_GetError() );
      retval = MERROR_SND;
      goto cleanup;
   }

   if(
      0 > Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 0 )
   ) {
      error_printf( "couldn't open SDL mixer audio: %s", Mix_GetError() );
      retval = MERROR_SND;
      goto cleanup;
   }

   g_retroflat_state->sound.flags |= RETROSND_FLAG_INIT;

cleanup:

   return retval;
}

/* === */

void retrosnd_midi_set_sf_bank( const char* filename_in ) {
   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#     pragma message( "warning: set_sf_bank not implemented" )
}

/* === */

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice ) {
   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#     pragma message( "warning: set_voice not implemented" )
}

/* === */

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val ) {
   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#     pragma message( "warning: set_control not implemented" )
}

/* === */

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#     pragma message( "warning: note_on not implemented" )
}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

#     pragma message( "warning: note_off not implemented" )
}

/* === */

MERROR_RETVAL retrosnd_midi_play_smf( const char* filename ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO: Use chunks and play at pitch/velocity? */

   if( Mix_PlayingMusic() ) {
      debug_printf( RETROSND_TRACE_LVL, "stopping music..." );
      Mix_HaltMusic();
   }

   if( NULL != g_retroflat_state->sound.music ) {
      debug_printf( RETROSND_TRACE_LVL, "unloading music..." );
      Mix_FreeMusic( g_retroflat_state->sound.music );
      g_retroflat_state->sound.music = NULL;
   }

   if( NULL == filename ) {
      goto cleanup;
   }

   /* Attempt to load requested music. */

   g_retroflat_state->sound.music = Mix_LoadMUS( filename );
   if( NULL == g_retroflat_state->sound.music ) {
      error_printf( "could not load \"%s\": %s", filename, Mix_GetError() );
      retval = MERROR_SND;
   }

   debug_printf( RETROSND_TRACE_LVL, "%s loaded, playing...", filename );

   Mix_PlayMusic( g_retroflat_state->sound.music, -1 );

cleanup:

   return retval;
}

/* === */

uint8_t retrosnd_midi_is_playing_smf() {
   return Mix_PlayingMusic();
}

/* === */

void retrosnd_shutdown() {

   if( NULL == g_retroflat_state ) {
      error_printf(
         "retroflat not initialized! premature retroflat shutdown?" );
      return;
   }

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   if( Mix_PlayingMusic() ) {
      Mix_HaltMusic();
   }
   if( g_retroflat_state->sound.music ) {
      Mix_FreeMusic( g_retroflat_state->sound.music );
      g_retroflat_state->sound.music = NULL;
   }
   if( g_retroflat_state->sound.audio_open ) {
      Mix_CloseAudio();
      g_retroflat_state->sound.audio_open = 0;
   }

   Mix_Quit();

}

#endif /* !RETPLTS_H || RETROFLT_C */

