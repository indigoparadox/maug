
#if !defined( RETPLTS_H_DEFS )
#define RETPLTS_H_DEFS

#  include <mmsystem.h>

struct RETROFLAT_SOUND {
   uint8_t flags;
   HMIDIOUT mo_handle;
};

#elif defined( RETROFLT_C )

MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;
   int i = 0;
   MAUG_MHANDLE devs_list_buf_h = NULL;
   char* devs_list_buf = NULL;
   MAUG_MHANDLE devs_list_buf_new = NULL;
   size_t devs_list_buf_sz = 512;
   MIDIOUTCAPS midi_caps;
   uint32_t num_devs = 0;
   size_t devs_list_buf_pos = 0;
   UINT moo_retval = 0;

   assert( 2 <= sizeof( MERROR_RETVAL ) );

   num_devs = midiOutGetNumDevs();

   /* If the /rsl arg was specified, show a list of MIDI devices. */
   if(
      RETROSND_ARGS_FLAG_LIST_DEVS ==
      (RETROSND_ARGS_FLAG_LIST_DEVS & args->snd_flags)
   ) {
      devs_list_buf_h = maug_malloc( 1, devs_list_buf_sz );

      maug_mlock( devs_list_buf_h, devs_list_buf );
      maug_strncpy( devs_list_buf, "MIDI devices:\n", devs_list_buf_sz );
      maug_munlock( devs_list_buf_h, devs_list_buf );

      for( i = 0 ; num_devs > i ; i++ ) {
         midiOutGetDevCaps( i, &midi_caps, sizeof( MIDIOUTCAPS ) );
         if(
            /* +2 for newline and NULL. */
            devs_list_buf_pos + 2 + maug_strlen( midi_caps.szPname )
            > devs_list_buf_sz
         ) {
            /* Grow buffer if we need to. */
            devs_list_buf_sz *= 2;
            devs_list_buf_new =
               maug_mrealloc( devs_list_buf_h, 1, devs_list_buf_sz );
            maug_cleanup_if_null_alloc( MAUG_MHANDLE, devs_list_buf_new );
            devs_list_buf_h = devs_list_buf_new;
         }
         assert( devs_list_buf_sz + 2 + maug_strlen( midi_caps.szPname )
            > devs_list_buf_sz );

         /* Copy the current device name into the buffer. */
         maug_mlock( devs_list_buf_h, devs_list_buf );
         /* TODO: Replace with maug_strncpy() */
         strcpy( &(devs_list_buf[devs_list_buf_pos]), midi_caps.szPname );
         devs_list_buf_pos += maug_strlen( midi_caps.szPname );
         devs_list_buf[devs_list_buf_pos++] = '\n';
         devs_list_buf[devs_list_buf_pos] = '\0';
         maug_munlock( devs_list_buf_h, devs_list_buf );
      }

      /* Show the list dialog. */
      maug_mlock( devs_list_buf_h, devs_list_buf );
      retroflat_message( RETROFLAT_MSG_FLAG_INFO,
         "MIDI Devices", "%s", devs_list_buf );
      maug_munlock( devs_list_buf_h, devs_list_buf );

      /* Cleanup the list and quit. */
      maug_mfree( devs_list_buf_h );
      retroflat_quit( 0 );
   }

   /* Try to open the specified device. */
   if( 0 == num_devs ) {
      error_printf( "no MIDI devices found!" );
      retval = MERROR_SND;
      goto cleanup;
   } else if( num_devs < args->snd_dev_id ) {
      error_printf( "invalid MIDI device index: %d", args->snd_dev_id );
      retval = MERROR_SND;
      goto cleanup;
   }
   midiOutGetDevCaps( args->snd_dev_id, &midi_caps, sizeof( MIDIOUTCAPS ) );

   debug_printf( 3, "attempting to open MIDI device %u...", args->snd_dev_id );
   moo_retval = midiOutOpen(
      &(g_retroflat_state->sound.mo_handle), args->snd_dev_id, 0, 0, CALLBACK_NULL );
   if( MMSYSERR_NOERROR != moo_retval ) {
      error_printf( "could not open MIDI device: %s: %d",
         midi_caps.szPname, moo_retval );
      retval = MERROR_SND;
      goto cleanup;
   }
   debug_printf( 3, "opened MIDI device: %s", midi_caps.szPname );
   g_retroflat_state->sound.flags |= RETROSND_FLAG_INIT;

cleanup:

   return retval;
}

/* === */

void retrosnd_midi_set_sf_bank( const char* filename_in ) {
}

/* === */

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice ) {
   midiOutShortMsg( g_retroflat_state->sound.mo_handle,
      (((voice & 0xff) << 8) | 0xc0 | (channel & 0x0f)) );
}

/* === */

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val ) {
   uint32_t midi_msg = 0;

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags) ) {
      return;
   }

   midi_msg |= (val & 0xff);
   midi_msg <<= 8;
   midi_msg |= (key & 0xff);
   midi_msg <<= 8;
   midi_msg |= (channel & 0x0f) | 0xb0;
   midiOutShortMsg( g_retroflat_state->sound.mo_handle, midi_msg );
}

/* === */

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   uint32_t midi_msg = 0;

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags) ) {
      return;
   }

   midi_msg |= (vel & 0xff);
   midi_msg <<= 8;
   midi_msg |= (pitch & 0xff);
   midi_msg <<= 8;
   midi_msg |= (channel & 0x0f) | 0x90;
   midiOutShortMsg( g_retroflat_state->sound.mo_handle, midi_msg );
}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   uint32_t midi_msg = 0;

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags) ) {
      return;
   }

   midi_msg |= (vel & 0xff);
   midi_msg <<= 8;
   midi_msg |= (pitch & 0xff);
   midi_msg <<= 8;
   midi_msg |= (channel & 0x0f) | 0x80;
   midiOutShortMsg( g_retroflat_state->sound.mo_handle, midi_msg );
}

/* === */

MERROR_RETVAL retrosnd_midi_play_smf( const char* filename ) {
   MERROR_RETVAL retval = MERROR_OK;
#     pragma message( "warning: midi_play_smf not implemented" )
   return retval;
}

/* === */

uint8_t retrosnd_midi_is_playing_smf() {
#     pragma message( "warning: midi_is_playing_smf not implemented" )
   return 1;
}

/* === */

void retrosnd_shutdown() {
   midiOutClose( g_retroflat_state->sound.mo_handle );
}

#endif /* !RETPLTS_H_DEFS || RETROFLT_C */

