
#if !defined( RETPLTS_H_DEFS )
#define RETPLTS_H_DEFS

#include <SDL.h>

struct RETROFLAT_SOUND_ARGS {
   uint8_t flags;
};

struct RETROFLAT_SOUND_STATE {
   uint8_t flags;
   int audio_open;
   struct RETROSND_CHANNEL channels[RETROSND_CHANNEL_CT_MAX];
   SDL_AudioSpec spec;
};

#elif defined( RETROFLT_C )

void retrosnd_sdl_audio_callback( void* userdata, Uint8* stream, int len ) {
   int16_t* out = (int16_t*)stream;
   int i = 0;
   int samples = len / sizeof( int16_t );
   struct RETROFLAT_SOUND_STATE* state =
      (struct RETROFLAT_SOUND_STATE*)userdata;

   for( i = 0 ; i < samples ; i++ ) {
      out[i] = _retrosnd_generate_note( state->channels );
   }
}

/* === */

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
   size_t i = 0;

   maug_mzero(
      &g_retroflat_state->sound, sizeof( struct RETROFLAT_SOUND_STATE ) );

   for( i = 0 ; RETROSND_CHANNEL_CT_MAX > i ; i++ ) {
      g_retroflat_state->sound.channels[i].note = -1;
   }

   /*
   if( 0 > SDL_Init( SDL_INIT_AUDIO ) ) {
      error_printf( "couldn't initialize SDL audio: %s", SDL_GetError() );
      retval = MERROR_SND;
      goto cleanup;
   }
   */

   g_retroflat_state->sound.spec.freq = RETROSND_SAMPLE;
   g_retroflat_state->sound.spec.format = AUDIO_S16SYS;
   g_retroflat_state->sound.spec.channels = 1;
   g_retroflat_state->sound.spec.samples = 512;
   g_retroflat_state->sound.spec.callback = retrosnd_sdl_audio_callback;
   g_retroflat_state->sound.spec.userdata = &(g_retroflat_state->sound);

   SDL_OpenAudio( &(g_retroflat_state->sound.spec), NULL );

   g_retroflat_state->sound.flags |= RETROSND_FLAG_INIT;

   SDL_PauseAudio( 0 );
 
cleanup:

   return retval;
}

/* === */

void retrosnd_set_voice( uint8_t channel, uint8_t voice ) {
   g_retroflat_state->sound.channels[channel].voice = voice;
}

/* === */

void retrosnd_set_control( uint8_t channel, uint8_t key, uint8_t val ) {
   switch( key ) {
   case RETROSND_CONTROL_VOL:
      g_retroflat_state->sound.channels[channel].vol = val;
      break;

   default:
      error_printf( "unimplemented control message: %d!", key );
      break;
   }
}

/* === */

void retrosnd_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   g_retroflat_state->sound.channels[channel].note = pitch;
}

/* === */

void retrosnd_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   g_retroflat_state->sound.channels[channel].note = -1;
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

   g_retroflat_state->sound.flags &= ~RETROSND_FLAG_INIT;
}

#endif /* !RETPLTS_H || RETROFLT_C */

