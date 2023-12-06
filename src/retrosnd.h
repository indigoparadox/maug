
#ifndef RETROSND_H
#define RETROSND_H

#define RETROSND_FLAG_INIT 0x01

#define RETROSND_VOICE_GUNSHOT   128

#if defined( RETROSND_API_GUS )
#  include <conio.h>
#elif defined( RETROSND_API_MPU )
#  include <conio.h>
#  define RETROSND_MPU_FLAG_OUTPUT 0x40
#  define RETROSND_MPU_TIMEOUT 30
#elif defined( RETROSND_API_ALSA )
#  include <alsa/asoundlib.h>
#  define retrosnd_set_alsa_seq( seq, port ) \
   (((uint16_t)(seq) << 8) | ((port) & 0xff))
#endif /* RETROSND_API_GUS || RETROSND_API_MPU */

struct RETROSND_STATE {
   uint8_t flags;
#if defined( RETROSND_API_GUS )
   uint16_t io_base;
#elif defined( RETROSND_API_MPU )
   uint16_t io_base;
   uint8_t io_timeout;
#elif defined( RETROSND_API_ALSA )
   snd_seq_t* seq_handle;
   int seq_port;
   int out_client;
   int out_port;
#endif /* RETROSND_API_GUS || RETROSND_API_MPU */
};

int16_t retrosnd_init( struct RETROFLAT_ARGS* args );

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice );

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val );

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel );

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel );

void retrosnd_shutdown();

#ifdef RETROSND_C

struct RETROSND_STATE g_retrosnd_state;

#  if defined( RETROSND_API_GUS )

static void retrosnd_gus_poke( uint32_t loc, uint8_t b ) {
   uint16_t add_lo = loc & 0xffff;
   uint8_t add_hi = (uint32_t)((loc & 0xff0000)) >> 16;

   debug_printf( 1,
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

   debug_printf( 1, "read: 0x%02x", b );

   return b;
}

#  elif defined( RETROSND_API_MPU )

static MERROR_RETVAL retrosnd_mpu_not_ready( int16_t* timeout ) {
   uint8_t b = 0;
   b = inp( g_retrosnd_state.io_base + 0x01 ) & RETROSND_MPU_FLAG_OUTPUT;
   if( (NULL == timeout || 0 < *timeout) && 0 != b ) {
      if( NULL == timeout ) {
         debug_printf( 2, "waiting for MPU-401..." );
      } else {
         debug_printf( 2, "waiting for MPU-401 (%u)...", *timeout );
         (*timeout)--;
      }
      return MERROR_WAIT;
   } else if( NULL != timeout && 0 == *timeout ) {
      error_printf( "timed out waiting for MPU-401!" );
      return MERROR_TIMEOUT;
   } else {
      debug_printf( 2, "MPU ready within time limit" );
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
      debug_printf( 1, "writing 0x%02x to MPU-401...", b_in );
      outp( g_retrosnd_state.io_base, b_in );
   }
cleanup:
   return retval;
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

int16_t retrosnd_init( struct RETROFLAT_ARGS* args ) {
   int16_t retval = MERROR_OK;
#  ifdef RETROSND_API_GUS
   uint8_t b = 0;
#  elif defined( RETROSND_API_MPU )
   int16_t timeout = 0;
#  elif defined( RETROSND_API_ALSA )
#  endif /* RETROSND_API_GUS || RETROSND_API_MPU || RETROSND_API_ALSA */

   assert( 2 <= sizeof( MERROR_RETVAL ) );

   if( 0 == args->snd_io_base ) {
      error_printf( "I/O base not specified!" );
#  if defined( RETROSND_API_MPU )
      debug_printf( 3, "assuming 0x330..." );
      args->snd_io_base = 0x330;
#  else
      return retval;
#  endif /* RETROSND_API_MPU */
   }

   /* Clear all flags to start. */
   g_retrosnd_state.flags = 0;

#  ifdef RETROSND_API_GUS

   /* TODO: Make I/O base configurable. */
   g_retrosnd_state.io_base = 0x220;
   
   outp( g_retrosnd_state.io_base + 0x103, 0x4c );
   outp( g_retrosnd_state.io_base + 0x105, 0 );
   sleep( 1 );
   outp( g_retrosnd_state.io_base + 0x103, 0x4c );
   outp( g_retrosnd_state.io_base + 0x105, 1 );

   retrosnd_gus_poke( 0, 0xf );
   retrosnd_gus_poke( 0x100, 0x55 );

   b = retrosnd_gus_peek( 0 );
   if( 0xf == b ) {
      debug_printf( 3, "found gravis ultrasound!" );
      g_retrosnd_state.flags |= RETROSND_FLAG_INIT;
   }

#  elif defined( RETROSND_API_MPU )

   g_retrosnd_state.io_base = 0x330;

   timeout = RETROSND_MPU_TIMEOUT;
   do {
      retval = retrosnd_mpu_not_ready( &timeout );
      if( MERROR_TIMEOUT == retval ) {
         error_printf( "cancelling initialization!" );
         retval = MERROR_SND;
         goto cleanup;
      }
   } while( MERROR_WAIT == retval );

   debug_printf( 3, "placing MPU-401 in UART mode..." );
   outp( g_retrosnd_state.io_base + 0x01, 0xff );

   debug_printf( 3, "MPU-401 ready!" );
   g_retrosnd_state.flags |= RETROSND_FLAG_INIT;

cleanup:
#  elif defined( RETROSND_API_ALSA )
   
   /* TODO: Make destination seq/port configurable. */
   g_retrosnd_state.out_client = (args->snd_io_base >> 8) & 0xff;
   g_retrosnd_state.out_port = args->snd_io_base & 0xff;

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
      goto cleanup;
   }
   debug_printf( 3, "sequencer connected to to: %u:%u",
      g_retrosnd_state.out_client, g_retrosnd_state.out_port );
   g_retrosnd_state.flags |= RETROSND_FLAG_INIT;

cleanup:
#  endif /* RETROSND_API_GUS || RETROSND_API_MPU || RETROSND_API_ALSA */

   return retval;
}

/* === */

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice ) {
#  if defined( RETROSND_API_MPU )
   MERROR_RETVAL retval = 0;
#  elif defined( RETROSND_API_ALSA )
   snd_seq_event_t ev;
#  endif /* RETROSND_API_ALSA */

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retrosnd_state.flags) ) {
      return;
   }

#  ifdef RETROSND_API_GUS
   /* TODO */
#  elif defined( RETROSND_API_MPU )

   /* Write MIDI message to MPU port, one byte at a time. */

   /* 0xc0 (program change) | lower-nibble for channel. */
   retval = retrosnd_mpu_write_byte( 0xc0 | (channel & 0x0f) );
   maug_cleanup_if_not_ok();

   retval = retrosnd_mpu_write_byte( voice );
   maug_cleanup_if_not_ok();

cleanup:
   return;
#  elif defined( RETROSND_API_ALSA )
   debug_printf( 3, "setting channel %u to voice: %u", channel, voice );
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_pgmchange( &ev, channel, voice );
   retrosnd_alsa_ev_send( &ev );
#  endif /* RETROSND_API_ALSA */
}

/* === */

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val ) {
#  if defined( RETROSND_API_MPU )
   MERROR_RETVAL retval = 0;
#  elif defined( RETROSND_API_ALSA )
   snd_seq_event_t ev;
#  endif /* RETROSND_API_ALSA */

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retrosnd_state.flags) ) {
      return;
   }

#  ifdef RETROSND_API_GUS
   /* TODO */
#  elif defined( RETROSND_API_MPU )

   /* Write MIDI message to MPU port, one byte at a time. */

   /* 0xb0 (controller) | lower-nibble for channel. */
   retval = retrosnd_mpu_write_byte( 0xb0 | (channel & 0x0f) );
   maug_cleanup_if_not_ok();

   retval = retrosnd_mpu_write_byte( key );
   maug_cleanup_if_not_ok();

   retval = retrosnd_mpu_write_byte( val );
   maug_cleanup_if_not_ok();

cleanup:
   return;
#  elif defined( RETROSND_API_ALSA )
   debug_printf( 3, "setting channel %u controller %u to: %u", 
      channel, key, val );
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_controller( &ev, channel, key, val );
   retrosnd_alsa_ev_send( &ev );
#  endif /* RETROSND_API_ALSA */
}

/* === */

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {
#  if defined( RETROSND_API_MPU )
   MERROR_RETVAL retval = 0;
#  elif defined( RETROSND_API_ALSA )
   snd_seq_event_t ev;
#  endif /* RETROSND_API_ALSA */

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retrosnd_state.flags) ) {
      return;
   }

#  ifdef RETROSND_API_GUS
   /* TODO */
#  elif defined( RETROSND_API_MPU )

   /* Write MIDI message to MPU port, one byte at a time. */

   /* 0x90 (note on) | lower-nibble for channel. */
   retval = retrosnd_mpu_write_byte( 0x90 | (channel & 0x0f) );
   maug_cleanup_if_not_ok();

   retval = retrosnd_mpu_write_byte( pitch );
   maug_cleanup_if_not_ok();

   retval = retrosnd_mpu_write_byte( vel );
   maug_cleanup_if_not_ok();

cleanup:
   return;
#  elif defined( RETROSND_API_ALSA )
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_noteon( &ev, channel, pitch, vel );
   retrosnd_alsa_ev_send( &ev );
#  endif /* RETROSND_API_ALSA */

}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {
#  if defined( RETROSND_API_MPU )
   MERROR_RETVAL retval = 0;
#  elif defined( RETROSND_API_ALSA )
   snd_seq_event_t ev;
#  endif /* RETROSND_API_ALSA */

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retrosnd_state.flags) ) {
      return;
   }

#  ifdef RETROSND_API_GUS
   /* TODO */
#  elif defined( RETROSND_API_MPU )

   /* Write MIDI message to MPU port, one byte at a time. */

   /* 0x80 (note off) | lower-nibble for channel. */
   retval = retrosnd_mpu_write_byte( 0x80 | (channel & 0x0f) );
   maug_cleanup_if_not_ok();

   retval = retrosnd_mpu_write_byte( pitch );
   maug_cleanup_if_not_ok();

   retval = retrosnd_mpu_write_byte( vel );
   maug_cleanup_if_not_ok();

cleanup:
   return;
#  elif defined( RETROSND_API_ALSA )
   retrosnd_alsa_ev( &ev );
   snd_seq_ev_set_noteoff( &ev, channel, pitch, vel );
   retrosnd_alsa_ev_send( &ev );
#  endif /* RETROSND_API_ALSA */

}

/* === */

void retrosnd_shutdown() {

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retrosnd_state.flags) ) {
      return;
   }

#  ifdef RETROSND_API_GUS
   /* TODO */
#  elif defined( RETROSND_API_ALSA )
   snd_seq_close( g_retrosnd_state.seq_handle );
#  endif /* RETROSND_API_ALSA */
}

#endif /* RETROSND_C */

#endif /* !RETROSND_H */

