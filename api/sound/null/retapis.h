
#if !defined( RETPLTS_H_DEFS )
#define RETPLTS_H_DEFS

struct RETROFLAT_SOUND_ARGS {
   uint8_t flags;
};

struct RETROFLAT_SOUND_STATE {
   uint8_t flags;
};

#elif defined( RETROFLT_C )

MERROR_RETVAL retrosnd_cli_rsd(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: cli_select_device not implemented" )
#endif /* !RETROFLAT_NO_SOUND */

   return retval;
}

/* === */

MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( 2 <= sizeof( MERROR_RETVAL ) );

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: init not implemented" )
#endif /* !RETROFLAT_NO_SOUND */

   return retval;
}

/* === */

void retrosnd_midi_set_sf_bank( const char* filename_in ) {

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: set_sf_bank not implemented" )
#endif /* !RETROFLAT_NO_SOUND */
}

/* === */

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: set_voice not implemented" )
#endif /* !RETROFLAT_NO_SOUND */
}

/* === */

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: set_control not implemented" )
#endif /* !RETROFLAT_NO_SOUND */
}

/* === */

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: note_on not implemented" )
#endif /* !RETROFLAT_NO_SOUND */
}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags)
   ) {
      return;
   }

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: note_off not implemented" )
#endif /* !RETROFLAT_NO_SOUND */
}

/* === */

MERROR_RETVAL retrosnd_midi_play_smf( const char* filename ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: midi_play_smf not implemented" )
#endif /* !RETROFLAT_NO_SOUND */

   return retval;
}

/* === */

uint8_t retrosnd_midi_is_playing_smf() {

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: midi_play_smf not implemented" )
#endif /* !RETROFLAT_NO_SOUND */

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

   /* TODO */
#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: shutdown not implemented" )
#endif /* !RETROFLAT_NO_SOUND */
}

#endif /* !RETPLTS_H_DEFS || RETROFLT_C */

