
#if !defined( RETPLTS_H_DEFS )
#define RETPLTS_H_DEFS

struct RETROFLAT_SOUND_ARGS {
   uint8_t flags;
};

struct RETROFLAT_SOUND_STATE {
   uint8_t flags;
   uint8_t channels[RETROSND_CHANNEL_CT];
};

#elif defined( RETROFLT_C )

/* Table of MIDI pitch values roughly transcribed to frequencies in Hz...
 * These sound kind of wonky with the square wave generator, though!
 */
static const uint16_t _gc_retroflat_nds_midi_freq[128] = {
       8,     9,     9,    10,    10,    11,    12,    12,
      13,    14,    15,    15,    16,    17,    18,    19,
      21,    22,    23,    24,    26,    28,    29,    31,
      33,    35,    37,    39,    41,    44,    46,    49,
      52,    55,    58,    62,    65,    69,    73,    78,
      82,    87,    92,    98,   104,   110,   117,   123,
     131,   139,   147,   156,   165,   175,   185,   196,
     208,   220,   233,   247,   262,   277,   294,   311,
     330,   349,   370,   392,   415,   440,   466,   494,
     523,   554,   587,   622,   659,   698,   740,   784,
     831,   880,   932,   988,  1047,  1109,  1175,  1245,
    1319,  1397,  1480,  1568,  1661,  1760,  1865,  1976,
    2093,  2217,  2349,  2489,  2637,  2794,  2960,  3136,
    3322,  3520,  3729,  3951,  4186,  4435,  4699,  4978,
    5274,  5588,  5920,  5920,  6645,  7040,  7459,  7902,
    8372,  8870,  9397,  9956, 10548, 11175, 11840, 12544
};

/* === */

MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( 2 <= sizeof( MERROR_RETVAL ) );

   soundEnable();

   g_retroflat_state->sound.flags |= RETROSND_FLAG_INIT;

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


   g_retroflat_state->sound.channels[channel] =
      soundPlayPSG( DutyCycle_87, _gc_retroflat_nds_midi_freq[pitch],
      RETROFLAT_NDS_VOLUME, 64 );

   debug_printf( 1, "playing channel %u at freq %u on DS channel: %u",
      channel, _gc_retroflat_nds_midi_freq[pitch],
      g_retroflat_state->sound.channels[channel] );

}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   soundKill( g_retroflat_state->sound.channels[channel] );

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

   soundDisable();

}

#endif /* !RETPLTS_H_DEFS || RETROFLT_C */

