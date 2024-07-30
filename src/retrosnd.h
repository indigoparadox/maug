
#ifndef RETROSND_H
#define RETROSND_H

/**
 * \addtogroup maug_retrosnd RetroSound API
 * \brief Abstraction layer header for sound on retro systems.
 * \{
 *
 * \file retrosnd.h
 * \brief Abstraction layer header for sound on retro systems.
 *
 * RetroSound is a compatibility layer for making sound
 * on various platforms, including Windows, MS-DOS or Linux.
 *
 * To use, define RETROSND_C before including this header from your main.c.
 *
 * You may include this header in other .c files, as well, but RETROSND_C
 * should \b ONLY be declared \b ONCE in the entire program.
 *
 * It is *highly* advised to use this in conjunction with retroflt.h.
 *
 * maug.h should also be included before this header.
 *
 * Special thanks to: "Programming the Adlib/Sound Blaster FM Music Chips"
 * by Jeffrey S. Lee (https://bochs.sourceforge.io/techspec/adlib_sb.txt)
 * and "The Gravis Ultrasound" by neuraldk
 * (http://neuraldk.org/document.php?gus) for DOS platform-specific stuff.
 *
 * And the MPU-401 interface for being so darn simple!
 */

#ifndef RETROSND_TRACE_LVL
#  define RETROSND_TRACE_LVL 0
#endif /* !RETROSND_TRACE_LVL */

#ifndef RETROSND_REG_TRACE_LVL
#  define RETROSND_REG_TRACE_LVL 0
#endif /* !RETROSND_REG_TRACE_LVL */

#define SF_BANK_FILENAME_SZ_MAX 30

/**
 * \addtogroup maug_retrosnd_flags RetroSound State Flags
 * \brief Flags indicating global state for the RETROSND_STATE::flags field.
 * \{
 */

/**
 * \brief Flag in RETROSND_STATE::flags indicating initialization was
 *        successful.
 */
#define RETROSND_FLAG_INIT 0x01

/*! \} */ /* maug_retrosnd_flags */

#define RETROSND_VOICE_BREATH       122

#define RETROSND_VOICE_SEASHORE     123

#define RETROSND_VOICE_BIRD_TWEET   124

#define RETROSND_VOICE_PHONE_RING   125

#define RETROSND_VOICE_HELICOPTER   126

#define RETROSND_VOICE_APPLAUSE     127

#define RETROSND_PC_BIOS_SPKR       0x01
#define RETROSND_PC_BIOS_MPU        0x02
#define RETROSND_PC_BIOS_GUS        0x04
#define RETROSND_PC_BIOS_ADLIB      0x08

/**
 * \brief Parameter for retrosnd_midi_set_voice() indicating a gunshot
 *        sound effect.
 */
#define RETROSND_VOICE_GUNSHOT      128

#if defined( RETROSND_API_PC_BIOS )
#  include <conio.h>
#  define RETROSND_MPU_FLAG_OUTPUT 0x40
#  define RETROSND_MPU_TIMEOUT 30
#elif defined( RETROSND_API_ALSA )
#  include <alsa/asoundlib.h>
#endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA */

/**
 * \brief Internal retrosound state struct. Most fields are platform-specific.
 */
struct RETROSND_STATE {
   /**
    * \brief Bitfield indicating global state with \ref maug_retrosnd_flags.
    */
   uint8_t flags;
#if defined( RETROSND_API_PC_BIOS )
   uint16_t io_base;
   uint8_t io_timeout;
   uint8_t driver;
   struct RETROSND_ADLIB_VOICE* adlib_voices;
   char sf_bank_filename[SF_BANK_FILENAME_SZ_MAX + 1];
#elif defined( RETROSND_API_ALSA )
   snd_seq_t* seq_handle;
   int seq_port;
   int out_client;
   int out_port;
#elif defined( RETROSND_API_WINMM )
   HMIDIOUT mo_handle;
#endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */
};

/**
 * \brief Initialize retrosnd engine.
 * \param args A pointer to the RETROSND_ARGS struct initialized by
 *        the calling program.
 *
 * The RETROSND_ARGS::snd_io_base field must be initialized with the address
 * or other platform-specific indicator of the MIDI device to use.
 */
MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args );

/**
 * \brief Set the name of the voice bank filename to use.
 */
void retrosnd_set_sf_bank( const char* filename_in );

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice );

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val );

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel );

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel );

void retrosnd_shutdown();

#ifdef RETROSND_C

MAUG_CONST uint8_t SEG_MCONST adlib_ch[] = {0, 1, 2, 8, 9, 10, 16, 17, 18};

struct RETROSND_STATE g_retrosnd_state;

#  if defined( RETROSND_API_PC_BIOS )

static void retrosnd_gus_poke( uint32_t loc, uint8_t b ) {
   uint16_t add_lo = loc & 0xffff;
   uint8_t add_hi = (uint32_t)((loc & 0xff0000)) >> 16;

   debug_printf( RETROSND_REG_TRACE_LVL,
      "poke hi: 0x%02x, lo: 0x%04x: writing: %02x", add_hi, add_lo, b );
   
   outp( g_retrosnd_state.io_base + 0x103, 0x43 );
   outpw( g_retrosnd_state.io_base + 0x104, add_lo );
   outp( g_retrosnd_state.io_base + 0x103, 0x44 );
   outp( g_retrosnd_state.io_base + 0x105, add_hi );
   outp( g_retrosnd_state.io_base + 0x107, b );

}

static uint8_t retrosnd_gus_peek( uint32_t loc ) {
   uint8_t b = 0;
   uint16_t add_lo = loc & 0xffff;
   uint8_t add_hi = (uint32_t)((loc & 0xff0000)) >> 16;

   outp( g_retrosnd_state.io_base + 0x103, 0x43 );
   outpw( g_retrosnd_state.io_base + 0x104, add_lo );
   outp( g_retrosnd_state.io_base + 0x103, 0x44 );
   outp( g_retrosnd_state.io_base + 0x105, add_hi );

   b = inp( g_retrosnd_state.io_base + 0x107 );

   debug_printf( RETROSND_REG_TRACE_LVL, "read: 0x%02x", b );

   return b;
}

static MERROR_RETVAL retrosnd_mpu_not_ready( int16_t* timeout ) {
   uint8_t b = 0;
   b = inp( g_retrosnd_state.io_base + 0x01 ) & RETROSND_MPU_FLAG_OUTPUT;
   if( (NULL == timeout || 0 < *timeout) && 0 != b ) {
      if( NULL == timeout ) {
         debug_printf( RETROSND_TRACE_LVL, "waiting for MPU-401..." );
      } else {
         debug_printf( RETROSND_TRACE_LVL,
            "waiting for MPU-401 (%u)...", *timeout );
         (*timeout)--;
      }
      return MERROR_WAIT;
   } else if( NULL != timeout && 0 == *timeout ) {
      error_printf( "timed out waiting for MPU-401!" );
      return MERROR_TIMEOUT;
   } else {
      debug_printf( RETROSND_TRACE_LVL, "MPU ready within time limit" );
      return MERROR_OK;
   }
}

static MERROR_RETVAL retrosnd_mpu_write_byte( uint8_t b_in ) {
   int16_t timeout = RETROSND_MPU_TIMEOUT;
   MERROR_RETVAL retval = MERROR_OK;
   do {
      retval = retrosnd_mpu_not_ready( &timeout );
      if( MERROR_TIMEOUT == retval ) {
         goto cleanup;
      }
   } while( MERROR_WAIT == retval );
   if( MERROR_TIMEOUT != retval ) {
      debug_printf(
         RETROSND_REG_TRACE_LVL, "writing 0x%02x to MPU-401...", b_in );
      outp( g_retrosnd_state.io_base, b_in );
   }
cleanup:
   return retval;
}

static void retrosnd_adlib_poke( uint8_t reg, uint8_t b_in ) {
   int i = 0;
   debug_printf(
      RETROSND_REG_TRACE_LVL, "poke 0x%02x: writing: %02x", reg, b_in );
   outp( g_retrosnd_state.io_base, reg );
   for( i = 0 ; 6 > i ; i++ ) {
      inp( g_retrosnd_state.io_base );
   }
   outp( g_retrosnd_state.io_base + 1, b_in );
   for( i = 0 ; 35 > i ; i++ ) {
      inp( g_retrosnd_state.io_base );
   }
}

static void retrosnd_adlib_clear() {
   int i = 0;
   for( i = 0 ; 245 > i ; i++ ) {
      retrosnd_adlib_poke( i, 0 );
   }
}

#  elif defined( RETROSND_API_ALSA )

static void retrosnd_alsa_ev( snd_seq_event_t* ev ) {
   snd_seq_ev_clear( ev );
   snd_seq_ev_set_direct( ev );
   snd_seq_ev_set_source( ev, g_retrosnd_state.seq_port );
   /* snd_seq_ev_set_dest(
      ev, g_retrosnd_state.out_client, g_retrosnd_state.out_port ); */
   snd_seq_ev_set_subs( ev );
}

static void retrosnd_alsa_ev_send( snd_seq_event_t* ev ) {
   snd_seq_event_output( g_retrosnd_state.seq_handle, ev );
   snd_seq_drain_output( g_retrosnd_state.seq_handle );
}

#  endif /* RETROSND_API_GUS || RETROSND_API_ALSA */

/* === */

MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;
#  ifdef RETROSND_API_PC_BIOS
   uint8_t b = 0;
   int16_t timeout = 0;
#  elif defined( RETROSND_API_ALSA )
#  elif defined( RETROSND_API_WINMM )
   int i = 0;
   MAUG_MHANDLE devs_list_buf_h = NULL;
   char* devs_list_buf = NULL;
   MAUG_MHANDLE devs_list_buf_new = NULL;
   size_t devs_list_buf_sz = 512;
   MIDIOUTCAPS midi_caps;
   uint32_t num_devs = 0;
   size_t devs_list_buf_pos = 0;
   UINT moo_retval = 0;
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_WINMM || RETROSND_API_ALSA */

   assert( 2 <= sizeof( MERROR_RETVAL ) );

#  if defined( RETROSND_API_PC_BIOS )

   /* Clear all flags to start. */
   g_retrosnd_state.flags = 0;

   /* No bank file by default. */
   memset(
      g_retrosnd_state.sf_bank_filename, '\0',
      SF_BANK_FILENAME_SZ_MAX + 1 );

   if( 0 == args->snd_io_base ) {
      /* Select default port. */
      error_printf( "I/O base not specified!" );
      switch( args->snd_driver ) {
      case RETROSND_PC_BIOS_SPKR:
         /* TODO */
         break;

      case RETROSND_PC_BIOS_MPU:
         debug_printf( 3, "assuming 0x330..." );
         args->snd_io_base = 0x330;
         break;

      case RETROSND_PC_BIOS_GUS:
         /* TODO: Read port from ULTRASND env variable. */
         args->snd_io_base = 0x220;
         break;

      case RETROSND_PC_BIOS_ADLIB:
         /* TODO: Read port from BLASTER env variable? */
         debug_printf( 3, "assuming 0x338..." );
         args->snd_io_base = 0x388;
         break;
      }
      return retval;
   }

   g_retrosnd_state.io_base = args->snd_io_base;
   g_retrosnd_state.driver = args->snd_driver;
   
   /* Perform actual init. */
   switch( g_retrosnd_state.driver ) {
   case RETROSND_PC_BIOS_SPKR:
      /* TODO */
      break;

   case RETROSND_PC_BIOS_GUS:
      /* Write values to GUS memory. */
      outp( g_retrosnd_state.io_base + 0x103, 0x4c );
      outp( g_retrosnd_state.io_base + 0x105, 0 );
      sleep( 1 );
      outp( g_retrosnd_state.io_base + 0x103, 0x4c );
      outp( g_retrosnd_state.io_base + 0x105, 1 );

      retrosnd_gus_poke( 0, 0xf );
      retrosnd_gus_poke( 0x100, 0x55 );

      /* Confirm values read are the same as those written. */
      b = retrosnd_gus_peek( 0 );
      if( 0xf != b ) {
         error_printf( "gravis ultrasound not found!" );
         retval = MERROR_SND;
         goto cleanup;
      }

      debug_printf( 3, "gravis ultrasound ready!" );
      break;

   case RETROSND_PC_BIOS_MPU:
      timeout = RETROSND_MPU_TIMEOUT;
      /* Wait for OK-to-write flag. */
      /* TODO: Will this break on some newer cards? */
      do {
         retval = retrosnd_mpu_not_ready( &timeout );
         if( MERROR_TIMEOUT == retval ) {
            error_printf( "cancelling initialization!" );
            retval = MERROR_SND;
            goto cleanup;
         }
      } while( MERROR_WAIT == retval );

      /* Place the MPU-401 in UART mode. */
      debug_printf( 3, "placing MPU-401 in UART mode..." );
      outp( g_retrosnd_state.io_base + 0x01, 0xff );

      debug_printf( 3, "MPU-401 ready!" );
      break;

   case RETROSND_PC_BIOS_ADLIB:
      /* Clear all OPL registers. */
      retrosnd_adlib_clear();
      break;
   }

   g_retrosnd_state.flags |= RETROSND_FLAG_INIT;

cleanup:
#  elif defined( RETROSND_API_ALSA )

   /* TODO: If the /rsl arg was specified, show a list of MIDI devices. */
   
   /* Make destination seq/port configurable. */
   g_retrosnd_state.out_client = args->snd_client;
   g_retrosnd_state.out_port = args->snd_port;

   retval = snd_seq_open(
      &(g_retrosnd_state.seq_handle), "default", SND_SEQ_OPEN_OUTPUT, 0 );
   if( 0 > retval ) {
      error_printf( "could not open sequencer!" );
      retval = MERROR_SND;
      goto cleanup;
   }
   debug_printf( 3, "sequencer initialized" );

   g_retrosnd_state.seq_port = snd_seq_create_simple_port(
      g_retrosnd_state.seq_handle, "retrosnd",
      SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_WRITE,
      SND_SEQ_PORT_TYPE_APPLICATION );

   if( 0 > g_retrosnd_state.seq_port ) {
      error_printf( "could not open MIDI port!" );
      retval = MERROR_SND;
      goto cleanup;
   }
   debug_printf( 3, "sequencer opened port: %d", g_retrosnd_state.seq_port );

   retval = snd_seq_connect_to(
      g_retrosnd_state.seq_handle, g_retrosnd_state.seq_port,
      g_retrosnd_state.out_client, g_retrosnd_state.out_port );
   if( 0 > retval ) {
      error_printf( "could not connect sequencer to %u:%u!",
         g_retrosnd_state.out_client, g_retrosnd_state.out_port );
      retval = MERROR_SND;
      goto cleanup;
   }
   debug_printf( 3, "sequencer connected to to: %u:%u",
      g_retrosnd_state.out_client, g_retrosnd_state.out_port );
   g_retrosnd_state.flags |= RETROSND_FLAG_INIT;

cleanup:
#  elif defined( RETROSND_API_WINMM )

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
      &(g_retrosnd_state.mo_handle), args->snd_dev_id, 0, 0, CALLBACK_NULL );
   if( MMSYSERR_NOERROR != moo_retval ) {
      error_printf( "could not open MIDI device: %s: %d",
         midi_caps.szPname, moo_retval );
      retval = MERROR_SND;
      goto cleanup;
   }
   debug_printf( 3, "opened MIDI device: %s", midi_caps.szPname );
   g_retrosnd_state.flags |= RETROSND_FLAG_INIT;

cleanup:
#  else
#     pragma message( "warning: init not implemented" )
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */

   return retval;
}

/* === */

void retrosnd_set_sf_bank( const char* filename_in ) {
#ifdef RETROSND_API_PC_BIOS
   maug_strncpy( g_retrosnd_state.sf_bank_filename, filename_in,
      SF_BANK_FILENAME_SZ_MAX );
#endif /* RETROSND_API_PC_BIOS */
}

/* === */

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice ) {
#  if defined( RETROSND_API_PC_BIOS )
   MERROR_RETVAL retval = 0;
   mfile_t opl_defs;
   uint8_t byte_buffer = 0;
#  elif defined( RETROSND_API_ALSA )
   snd_seq_event_t ev;
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA */

   if(
      RETROSND_FLAG_INIT !=
      (RETROSND_FLAG_INIT & g_retrosnd_state.flags)
   ) {
      return;
   }

#  if defined( RETROSND_API_PC_BIOS )
   memset( &opl_defs, '\0', sizeof( mfile_t ) );

   switch( g_retrosnd_state.driver ) {
   case RETROSND_PC_BIOS_MPU:
      /* Write MIDI message to MPU port, one byte at a time. */

      /* 0xc0 (program change) | lower-nibble for channel. */
      retval = retrosnd_mpu_write_byte( 0xc0 | (channel & 0x0f) );
      maug_cleanup_if_not_ok();

      retval = retrosnd_mpu_write_byte( voice );
      maug_cleanup_if_not_ok();
      break;

   case RETROSND_PC_BIOS_GUS:
      /* TODO */
      break;

   case RETROSND_PC_BIOS_ADLIB:

      /* TODO: Fail more gracefully on high channel. */
      assert( channel < 6 );

      if( '\0' == g_retrosnd_state.sf_bank_filename[0] ) {
         error_printf( "no bank file specified!" );
         goto cleanup;
      }

      debug_printf(
         RETROSND_TRACE_LVL, "channel: %d, reg: %d", channel,
         adlib_ch[channel] );

      /* TODO: Decouple voices so we can have 10/12 channels? */

      /* Determine offset of voice def in OP2 file. */
      #define adlib_opl2_offset() (8 + ((voice) * 37))

      /* Read specified voice param field into specified reg. */
      #define adlib_read_voice( field, reg, offset ) \
         retval = opl_defs.seek( &opl_defs, adlib_opl2_offset() + offset ); \
         maug_cleanup_if_not_ok(); \
         retval = opl_defs.read_int( &opl_defs, &byte_buffer, 1, 0 ); \
         maug_cleanup_if_not_ok(); \
         debug_printf( RETROSND_TRACE_LVL, \
            "voice %d: " #field ": ofs: %d: 0x%02x -> reg 0x%02x", \
            voice, offset, byte_buffer, reg ); \
      retrosnd_adlib_poke( reg + adlib_ch[channel], byte_buffer );

      /* Actually open the file. */
      retval = mfile_open_read(
         g_retrosnd_state.sf_bank_filename, &opl_defs );
      maug_cleanup_if_not_ok();
      
      debug_printf(
         RETROSND_TRACE_LVL, "reading instrument %d at offset %d",
         voice, adlib_opl2_offset() );

      /* Here we load the requested instrument parameters index from the
       * OP2 file into the Adlib registers.
       *
       * Offsets come from:
       * https://moddingwiki.shikadi.net/wiki/OP2_Bank_Format
       */

      adlib_read_voice( mod_char, 0x20, 4 );
      adlib_read_voice( mod_ad_lvl, 0x60, 5 );
      adlib_read_voice( mod_sr_lvl, 0x80, 6 );
      adlib_read_voice( mod_wave_sel, 0xe0, 7 );
      /* adlib_read_voice( mod_key_scale, 8 );
      adlib_read_voice( mod_out_lvl, 9 ); */ /* TODO */
      adlib_read_voice( feedback, 0xc0, 10 );
      adlib_read_voice( car_char, 0x23, 11 );
      adlib_read_voice( car_ad_lvl, 0x63, 12 );
      adlib_read_voice( car_sr_lvl, 0x83, 13 );
      adlib_read_voice( car_wave_sel, 0xe3, 14 );
      /* adlib_read_voice( car_key_scale, 15 );
      adlib_read_voice( car_out_lvl, 16 ); */ /* TODO */

      /* TODO: Parse these from the file properly. */
      retrosnd_adlib_poke( 0x40 + adlib_ch[channel], 0x10 ); /* Mod volume. */
      retrosnd_adlib_poke( 0x43 + adlib_ch[channel], 0x00 ); /* Carrier volume. */

      break;
   }

cleanup:

   mfile_close( &opl_defs );

#  elif defined( RETROSND_API_ALSA )
   debug_printf(
      RETROSND_TRACE_LVL,
      "setting channel %u to voice: %u", channel, voice );
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_pgmchange( &ev, channel, voice );
   retrosnd_alsa_ev_send( &ev );
#  elif defined( RETROSND_API_WINMM )
   midiOutShortMsg( g_retrosnd_state.mo_handle,
      (((voice & 0xff) << 8) | 0xc0 | (channel & 0x0f)) );
#  else
#     pragma message( "warning: set_voice not implemented" )
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */
}

/* === */

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val ) {
#  if defined( RETROSND_API_PC_BIOS )
   MERROR_RETVAL retval = 0;
#  elif defined( RETROSND_API_ALSA )
   snd_seq_event_t ev;
#  elif defined( RETROSND_API_WINMM )
   uint32_t midi_msg = 0;
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retrosnd_state.flags) ) {
      return;
   }

#  if defined( RETROSND_API_PC_BIOS )

   switch( g_retrosnd_state.driver ) {
   case RETROSND_PC_BIOS_MPU:
      /* Write MIDI message to MPU port, one byte at a time. */

      /* 0xb0 (controller) | lower-nibble for channel. */
      retval = retrosnd_mpu_write_byte( 0xb0 | (channel & 0x0f) );
      maug_cleanup_if_not_ok();

      retval = retrosnd_mpu_write_byte( key );
      maug_cleanup_if_not_ok();

      retval = retrosnd_mpu_write_byte( val );
      maug_cleanup_if_not_ok();
      break;

   case RETROSND_PC_BIOS_GUS:
      /* TODO */
      break;

   case RETROSND_PC_BIOS_ADLIB:
      /* TODO */
      break;
   }

cleanup:
   return;
#  elif defined( RETROSND_API_ALSA )
   debug_printf( RETROSND_TRACE_LVL,
      "setting channel %u controller %u to: %u", 
      channel, key, val );
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_controller( &ev, channel, key, val );
   retrosnd_alsa_ev_send( &ev );
#  elif defined( RETROSND_API_WINMM )
   midi_msg |= (val & 0xff);
   midi_msg <<= 8;
   midi_msg |= (key & 0xff);
   midi_msg <<= 8;
   midi_msg |= (channel & 0x0f) | 0xb0;
   midiOutShortMsg( g_retrosnd_state.mo_handle, midi_msg );
#  else
#     pragma message( "warning: set_control not implemented" )
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */
}

/* === */

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {
#  if defined( RETROSND_API_PC_BIOS )
   MERROR_RETVAL retval = 0;
#  elif defined( RETROSND_API_ALSA )
   snd_seq_event_t ev;
#  elif defined( RETROSND_API_WINMM )
   uint32_t midi_msg = 0;
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retrosnd_state.flags) ) {
      return;
   }

#  if defined( RETROSND_API_PC_BIOS )

   switch( g_retrosnd_state.driver ) {
   case RETROSND_PC_BIOS_SPKR:
      /* TODO */
      break;

   case RETROSND_PC_BIOS_MPU:
      /* Write MIDI message to MPU port, one byte at a time. */

      /* 0x90 (note on) | lower-nibble for channel. */
      retval = retrosnd_mpu_write_byte( 0x90 | (channel & 0x0f) );
      maug_cleanup_if_not_ok();

      retval = retrosnd_mpu_write_byte( pitch );
      maug_cleanup_if_not_ok();

      retval = retrosnd_mpu_write_byte( vel );
      maug_cleanup_if_not_ok();
      break;

   case RETROSND_PC_BIOS_GUS:
      /* TODO */
      break;

   case RETROSND_PC_BIOS_ADLIB:
      /* TODO: Fail more gracefully on high channel. */
      assert( channel < 6 );
      /* Voice frequency. */
      retrosnd_adlib_poke( 0xa0 + channel, pitch );
      /* Turn voice on! */
      retrosnd_adlib_poke( 0xb0 + channel, 0x31 );
      break;
   }

cleanup:
   return;
#  elif defined( RETROSND_API_ALSA )
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_noteon( &ev, channel, pitch, vel );
   retrosnd_alsa_ev_send( &ev );
#  elif defined( RETROSND_API_WINMM )
   midi_msg |= (vel & 0xff);
   midi_msg <<= 8;
   midi_msg |= (pitch & 0xff);
   midi_msg <<= 8;
   midi_msg |= (channel & 0x0f) | 0x90;
   midiOutShortMsg( g_retrosnd_state.mo_handle, midi_msg );
#  else
#     pragma message( "warning: note_on not implemented" )
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */

}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {
#  if defined( RETROSND_API_PC_BIOS )
   MERROR_RETVAL retval = 0;
#  elif defined( RETROSND_API_ALSA )
   snd_seq_event_t ev;
#  elif defined( RETROSND_API_WINMM )
   uint32_t midi_msg = 0;
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retrosnd_state.flags) ) {
      return;
   }

#  if defined( RETROSND_API_PC_BIOS )

   switch( g_retrosnd_state.driver ) {
   case RETROSND_PC_BIOS_SPKR:
      /* TODO */
      break;

   case RETROSND_PC_BIOS_MPU:
      /* Write MIDI message to MPU port, one byte at a time. */

      /* 0x80 (note off) | lower-nibble for channel. */
      retval = retrosnd_mpu_write_byte( 0x80 | (channel & 0x0f) );
      maug_cleanup_if_not_ok();

      retval = retrosnd_mpu_write_byte( pitch );
      maug_cleanup_if_not_ok();

      retval = retrosnd_mpu_write_byte( vel );
      maug_cleanup_if_not_ok();
      break;

   case RETROSND_PC_BIOS_GUS:
      /* TODO */
      break;

   case RETROSND_PC_BIOS_ADLIB:
      /* TODO: Fail more gracefully on high channel. */
      assert( channel < 6 );
      /* Turn voice off. */
      retrosnd_adlib_poke( 0xb0 + channel, 0x11 );
      break;
   }

cleanup:
   return;
#  elif defined( RETROSND_API_ALSA )
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_noteoff( &ev, channel, pitch, vel );
   retrosnd_alsa_ev_send( &ev );
#  elif defined( RETROSND_API_WINMM )
   midi_msg |= (vel & 0xff);
   midi_msg <<= 8;
   midi_msg |= (pitch & 0xff);
   midi_msg <<= 8;
   midi_msg |= (channel & 0x0f) | 0x80;
   midiOutShortMsg( g_retrosnd_state.mo_handle, midi_msg );
#  else
#     pragma message( "warning: note_off not implemented" )
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */

}

/* === */

void retrosnd_shutdown() {

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retrosnd_state.flags) ) {
      return;
   }

#  ifdef RETROSND_API_PC_BIOS
   switch( g_retrosnd_state.driver ) {
   case RETROSND_PC_BIOS_MPU:
      /* TODO */
      break;
   
   case RETROSND_PC_BIOS_GUS:
      /* TODO */
      break;

   case RETROSND_PC_BIOS_ADLIB:
      retrosnd_adlib_clear();
      break;
   }
#  elif defined( RETROSND_API_ALSA )
   snd_seq_close( g_retrosnd_state.seq_handle );
#  elif defined( RETROSND_API_WINMM )
   midiOutClose( g_retrosnd_state.mo_handle );
#  else
#     pragma message( "warning: shutdown not implemented" )
#  endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */
}

#endif /* RETROSND_C */

/*! \} */ /* maug_retrosnd */

#endif /* !RETROSND_H */

