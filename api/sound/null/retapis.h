
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

void retrosnd_set_voice( uint8_t channel, uint8_t voice ) {

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

void retrosnd_set_control( uint8_t channel, uint8_t key, uint8_t val ) {

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

void retrosnd_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {

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

void retrosnd_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {

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

void retrosnd_shutdown( void ) {

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

/* === */

void retrosnd_note_on_deadline( 
   uint8_t channel, uint8_t pitch, retroflat_ms_t deadline
) {

#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: note_on_deadline not implemented" )
#endif /* !RETROFLAT_NO_SOUND */

}

/* === */

void retrosnd_pump( void ) {

#ifndef RETROFLAT_NO_SOUND
#  pragma message( "warning: pump not implemented" )
#endif /* !RETROFLAT_NO_SOUND */

}

#endif /* !RETPLTS_H_DEFS || RETROFLT_C */

