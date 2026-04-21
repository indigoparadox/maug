
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
   volatile int lock;
};

#elif defined( RETROFLT_C )

#ifdef RETROFLAT_OS_WIN
void __cdecl retrosnd_sdl_audio_callback( void* userdata, Uint8* stream, int len ) {
#else
void retrosnd_sdl_audio_callback( void* userdata, Uint8* stream, int len ) {
#endif /* RETROFLAT_OS_WIN */
   int16_t* out = (int16_t*)stream;
   int i = 0;
   int samples = len / sizeof( int16_t );
   struct RETROFLAT_SOUND_STATE* state =
      (struct RETROFLAT_SOUND_STATE*)userdata;

   state->lock = 1;
   for( i = 0 ; i < samples ; i++ ) {
      out[i] = _retrosnd_generate_note( state->channels );
   }
   state->lock = 0;
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

   maug_mzero(
      &g_retroflat_state->sound, sizeof( struct RETROFLAT_SOUND_STATE ) );

   retval = _retrosnd_channels_init( g_retroflat_state->sound.channels );
   maug_cleanup_if_not_ok();

   /*
   if( 0 > SDL_Init( SDL_INIT_AUDIO ) ) {
      error_printf( "couldn't initialize SDL audio: %s", SDL_GetError() );
      retval = MERROR_SND;
      goto cleanup;
   }
   */

   g_retroflat_state->sound.spec.freq = RETROSND_SAMPLE_RATE;
   g_retroflat_state->sound.spec.format = AUDIO_S16SYS;
   g_retroflat_state->sound.spec.channels = 1;
   g_retroflat_state->sound.spec.samples = RETROSND_SAMPLES_CT;
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
   if( RETROSND_CHANNEL_CT_MAX <= channel ) {
      error_printf( "invalid channel: %d", channel );
      return;
   }
   while( 1 == g_retroflat_state->sound.lock ) {
#if RETROSND_TRACE_LVL > 0
      debug_printf( RETROSND_TRACE_LVL, "waiting for sound buffer lock..." );
#endif /* RETROSND_TRACE_LVL */
   }
#if RETROSND_TRACE_LVL > 0
      debug_printf( RETROSND_TRACE_LVL, "setting channel %u voice: %u",
         channel, voice );
#endif /* RETROSND_TRACE_LVL */
   g_retroflat_state->sound.channels[channel].voice = voice;
}

/* === */

void retrosnd_set_control( uint8_t channel, uint8_t key, uint8_t val ) {
   if( RETROSND_CHANNEL_CT_MAX <= channel ) {
      error_printf( "invalid channel: %d", channel );
      return;
   }
   while( 1 == g_retroflat_state->sound.lock ) {
#if RETROSND_TRACE_LVL > 0
      debug_printf( RETROSND_TRACE_LVL, "waiting for sound buffer lock..." );
#endif /* RETROSND_TRACE_LVL */
   }
#if RETROSND_TRACE_LVL > 0
      debug_printf( RETROSND_TRACE_LVL, "setting control %u: %u on %u",
         key, val, channel );
#endif /* RETROSND_TRACE_LVL */
   _retrosnd_set_control(
      &(g_retroflat_state->sound.channels[channel]), key, val );
}

/* === */

void retrosnd_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   if( g_retroflat_state->sound.channels[channel].note == pitch ) {
      return;
   }
   while( 1 == g_retroflat_state->sound.lock ) {
#if RETROSND_TRACE_LVL > 0
      debug_printf( RETROSND_TRACE_LVL, "waiting for sound buffer lock..." );
#endif /* RETROSND_TRACE_LVL */
   }
#if RETROSND_TRACE_LVL > 0
      debug_printf( RETROSND_TRACE_LVL, "setting channel %u note: %u on",
         channel, pitch );
#endif /* RETROSND_TRACE_LVL */
   g_retroflat_state->sound.channels[channel].note = pitch;
   g_retroflat_state->sound.channels[channel].deadline = 0;
}

/* === */

void retrosnd_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   if(
      RETROSND_TUNE_NOTE_DISABLED ==
      g_retroflat_state->sound.channels[channel].note
   ) {
      return;
   }
   while( 1 == g_retroflat_state->sound.lock ) {
#if RETROSND_TRACE_LVL > 0
      debug_printf( RETROSND_TRACE_LVL, "waiting for sound buffer lock..." );
#endif /* RETROSND_TRACE_LVL */
   }
#if RETROSND_TRACE_LVL > 0
      debug_printf( RETROSND_TRACE_LVL, "setting channel %u note: off",
         channel );
#endif /* RETROSND_TRACE_LVL */
   g_retroflat_state->sound.channels[channel].note =
      RETROSND_TUNE_NOTE_DISABLED;
   g_retroflat_state->sound.channels[channel].deadline = 0;
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

/* === */

void retrosnd_note_on_deadline( 
   uint8_t channel, uint8_t pitch, retroflat_ms_t deadline
) {
   retrosnd_note_on( channel, pitch, 0 );
   g_retroflat_state->sound.channels[channel].deadline = deadline;
}

/* === */

void retrosnd_pump() {
   /* Do nothing; sound is pumped in separate thread. */
}

#endif /* !RETPLTS_H || RETROFLT_C */

