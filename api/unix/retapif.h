
static void retrosnd_alsa_ev( snd_seq_event_t* ev ) {
   snd_seq_ev_clear( ev );
   snd_seq_ev_set_direct( ev );
   snd_seq_ev_set_source( ev, g_retroflat_state->sound.seq_port );
   /* snd_seq_ev_set_dest(
      ev, g_retroflat_state->sound.out_client, g_retroflat_state->sound.out_port ); */
   snd_seq_ev_set_subs( ev );
}

static void retrosnd_alsa_ev_send( snd_seq_event_t* ev ) {
   snd_seq_event_output( g_retroflat_state->sound.seq_handle, ev );
   snd_seq_drain_output( g_retroflat_state->sound.seq_handle );
}

/* === */

MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( 2 <= sizeof( MERROR_RETVAL ) );

   /* TODO: If the /rsl arg was specified, show a list of MIDI devices. */
   
   /* Make destination seq/port configurable. */
   g_retroflat_state->sound.out_client = args->snd_client;
   g_retroflat_state->sound.out_port = args->snd_port;

   retval = snd_seq_open(
      &(g_retroflat_state->sound.seq_handle), "default", SND_SEQ_OPEN_OUTPUT, 0 );
   if( 0 > retval ) {
      error_printf( "could not open sequencer!" );
      retval = MERROR_SND;
      goto cleanup;
   }
   debug_printf( 3, "sequencer initialized" );

   g_retroflat_state->sound.seq_port = snd_seq_create_simple_port(
      g_retroflat_state->sound.seq_handle, "retrosnd",
      SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_WRITE,
      SND_SEQ_PORT_TYPE_APPLICATION );

   if( 0 > g_retroflat_state->sound.seq_port ) {
      error_printf( "could not open MIDI port!" );
      retval = MERROR_SND;
      goto cleanup;
   }
   debug_printf( 3, "sequencer opened port: %d", g_retroflat_state->sound.seq_port );

   retval = snd_seq_connect_to(
      g_retroflat_state->sound.seq_handle, g_retroflat_state->sound.seq_port,
      g_retroflat_state->sound.out_client, g_retroflat_state->sound.out_port );
   if( 0 > retval ) {
      error_printf( "could not connect sequencer to %u:%u!",
         g_retroflat_state->sound.out_client, g_retroflat_state->sound.out_port );
      retval = MERROR_SND;
      goto cleanup;
   }
   debug_printf( 3, "sequencer connected to to: %u:%u",
      g_retroflat_state->sound.out_client, g_retroflat_state->sound.out_port );
   g_retroflat_state->sound.flags |= RETROSND_FLAG_INIT;

cleanup:

   return retval;
}

/* === */

void retrosnd_midi_set_sf_bank( const char* filename_in ) {
#     pragma message( "warning: set_sf_bank not implemented" )
}

/* === */

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice ) {
   snd_seq_event_t ev;

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   debug_printf(
      RETROSND_TRACE_LVL,
      "setting channel %u to voice: %u", channel, voice );
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_pgmchange( &ev, channel, voice );
   retrosnd_alsa_ev_send( &ev );
}

/* === */

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val ) {
   snd_seq_event_t ev;

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   debug_printf( RETROSND_TRACE_LVL,
      "setting channel %u controller %u to: %u", 
      channel, key, val );
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_controller( &ev, channel, key, val );
   retrosnd_alsa_ev_send( &ev );
}

/* === */

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   snd_seq_event_t ev;

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_noteon( &ev, channel, pitch, vel );
   retrosnd_alsa_ev_send( &ev );
}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   snd_seq_event_t ev;

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_noteoff( &ev, channel, pitch, vel );
   retrosnd_alsa_ev_send( &ev );
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


   snd_seq_close( g_retroflat_state->sound.seq_handle );
}

