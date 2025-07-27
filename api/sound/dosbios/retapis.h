
#if !defined( RETPLTS_H_DEFS )
#define RETPLTS_H_DEFS

#define RETROSND_PC_BIOS_SPKR       0x01
#define RETROSND_PC_BIOS_MPU        0x02
#define RETROSND_PC_BIOS_GUS        0x04
#define RETROSND_PC_BIOS_ADLIB      0x08

#define RETROSND_MPU_FLAG_OUTPUT 0x40
#define RETROSND_MPU_TIMEOUT 30

struct RETROFLAT_SOUND_ARGS {
   uint8_t flags;
   uint16_t snd_io_base;
   uint8_t snd_driver;
};

struct RETROFLAT_SOUND_STATE {
   uint8_t flags;
   uint16_t io_base;
   uint8_t io_timeout;
   uint8_t driver;
   struct RETROSND_ADLIB_VOICE* adlib_voices;
   char sf_bank_filename[MAUG_PATH_MAX + 1];
};

#elif defined( RETROFLT_C )

MAUG_CONST uint8_t SEG_MCONST adlib_ch[] = {0, 1, 2, 8, 9, 10, 16, 17, 18};

static void retrosnd_gus_poke( uint32_t loc, uint8_t b ) {
   uint16_t add_lo = loc & 0xffff;
   uint8_t add_hi = (uint32_t)((loc & 0xff0000)) >> 16;

   debug_printf( RETROSND_REG_TRACE_LVL,
      "poke hi: 0x%02x, lo: 0x%04x: writing: %02x", add_hi, add_lo, b );
   
   outp( g_retroflat_state->sound.io_base + 0x103, 0x43 );
   outpw( g_retroflat_state->sound.io_base + 0x104, add_lo );
   outp( g_retroflat_state->sound.io_base + 0x103, 0x44 );
   outp( g_retroflat_state->sound.io_base + 0x105, add_hi );
   outp( g_retroflat_state->sound.io_base + 0x107, b );

}

/* === */

static uint8_t retrosnd_gus_peek( uint32_t loc ) {
   uint8_t b = 0;
   uint16_t add_lo = loc & 0xffff;
   uint8_t add_hi = (uint32_t)((loc & 0xff0000)) >> 16;

   outp( g_retroflat_state->sound.io_base + 0x103, 0x43 );
   outpw( g_retroflat_state->sound.io_base + 0x104, add_lo );
   outp( g_retroflat_state->sound.io_base + 0x103, 0x44 );
   outp( g_retroflat_state->sound.io_base + 0x105, add_hi );

   b = inp( g_retroflat_state->sound.io_base + 0x107 );

   debug_printf( RETROSND_REG_TRACE_LVL, "read: 0x%02x", b );

   return b;
}

/* === */

static MERROR_RETVAL retrosnd_mpu_not_ready( int16_t* timeout ) {
   uint8_t b = 0;
   b = inp( g_retroflat_state->sound.io_base + 0x01 ) & RETROSND_MPU_FLAG_OUTPUT;
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

/* === */

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
      outp( g_retroflat_state->sound.io_base, b_in );
   }
cleanup:
   return retval;
}

/* === */

static void retrosnd_adlib_poke( uint8_t reg, uint8_t b_in ) {
   int i = 0;
   debug_printf(
      RETROSND_REG_TRACE_LVL, "poke 0x%02x: writing: %02x", reg, b_in );
   outp( g_retroflat_state->sound.io_base, reg );
   for( i = 0 ; 6 > i ; i++ ) {
      inp( g_retroflat_state->sound.io_base );
   }
   outp( g_retroflat_state->sound.io_base + 1, b_in );
   for( i = 0 ; 35 > i ; i++ ) {
      inp( g_retroflat_state->sound.io_base );
   }
}

/* === */

static void retrosnd_adlib_clear() {
   int i = 0;
   for( i = 0 ; 245 > i ; i++ ) {
      retrosnd_adlib_poke( i, 0 );
   }
}

/* === */

MERROR_RETVAL retrosnd_cli_rsd(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
   char* env_var = NULL;
   size_t i = 0;

   if( 0 > arg_c ) {
   if( NULL != env_var ) {
      env_var = getenv( "MAUG_MIDI_DOS" );

      /* Return MERROR_OK since this isn't fatal and will just cause sound
         * init to fail later.
         */
      maug_cleanup_if_null_msg(
         char*, env_var, MERROR_OK, "MAUG_MIDI_DOS variable not found!" );

      debug_printf( 2, "env: MAUG_MIDI_DOS: %s", env_var );

      /* Turn comma separator into NULL split. */
      for( i = 0 ; maug_strlen( env_var ) > i ; i++ ) {
         if( ',' == env_var[i] ) {
            /* Split into two null-terminated strings. */
            env_var[i] = '\0';
         }
      }

      if( 0 == strcmp( env_var, "mpu" ) ) {
         debug_printf( 3, "selecting MIDI driver: mpu" );
         args->sound.snd_driver = 2;
      } else if( 0 == strcmp( env_var, "gus" ) ) {
         debug_printf( 3, "selecting MIDI driver: gus" );
         args->sound.snd_driver = 4;
      } else if( 0 == strcmp( env_var, "adlib" ) ) {
         debug_printf( 3, "selecting MIDI driver: adlib" );
         args->sound.snd_driver = 8;
      }
      /* TODO: Maug replacement for C99 crutch. */
      args->sound.snd_io_base = strtoul( &(env_var[i]), NULL, 16 );
      debug_printf( 3, "setting MIDI I/O base: %u", args->sound.snd_io_base );
   } else {
      /* default */
      debug_printf( 3, "default MIDI driver: adlib" );
      args->sound.snd_driver = 8;
      args->sound.snd_io_base = 0x388;
   }
   } else if(
      0 == strncmp( MAUG_CLI_SIGIL "rsd", arg, MAUG_CLI_SIGIL_SZ + 4 )
   ) {
      /* The next arg must be the new var. */
   } else {
      /* TODO: Parse device. */
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t b = 0;
   int16_t timeout = 0;

   /* Clear all flags to start. */
   g_retroflat_state->sound.flags = 0;

   /* No bank file by default. */
   memset(
      g_retroflat_state->sound.sf_bank_filename, '\0',
      MAUG_PATH_MAX + 1 );

   if( 0 == args->sound.snd_io_base ) {
      /* Select default port. */
      error_printf( "I/O base not specified!" );
      switch( args->sound.snd_driver ) {
      case RETROSND_PC_BIOS_SPKR:
         /* TODO */
         break;

      case RETROSND_PC_BIOS_MPU:
         debug_printf( 3, "assuming 0x330..." );
         args->sound.snd_io_base = 0x330;
         break;

      case RETROSND_PC_BIOS_GUS:
         /* TODO: Read port from ULTRASND env variable. */
         args->sound.snd_io_base = 0x220;
         break;

      case RETROSND_PC_BIOS_ADLIB:
         /* TODO: Read port from BLASTER env variable? */
         debug_printf( 3, "assuming 0x338..." );
         args->sound.snd_io_base = 0x388;
         break;
      }
      return retval;
   }

   g_retroflat_state->sound.io_base = args->sound.snd_io_base;
   g_retroflat_state->sound.driver = args->sound.snd_driver;
   
   /* Perform actual init. */
   switch( g_retroflat_state->sound.driver ) {
   case RETROSND_PC_BIOS_SPKR:
      /* TODO */
      break;

   case RETROSND_PC_BIOS_GUS:
      /* Write values to GUS memory. */
      outp( g_retroflat_state->sound.io_base + 0x103, 0x4c );
      outp( g_retroflat_state->sound.io_base + 0x105, 0 );
      sleep( 1 );
      outp( g_retroflat_state->sound.io_base + 0x103, 0x4c );
      outp( g_retroflat_state->sound.io_base + 0x105, 1 );

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
      outp( g_retroflat_state->sound.io_base + 0x01, 0xff );

      debug_printf( 3, "MPU-401 ready!" );
      break;

   case RETROSND_PC_BIOS_ADLIB:
      /* Clear all OPL registers. */
      retrosnd_adlib_clear();
      break;
   }

   g_retroflat_state->sound.flags |= RETROSND_FLAG_INIT;

cleanup:

   return retval;
}

/* === */

void retrosnd_midi_set_sf_bank( const char* filename_in ) {
   maug_strncpy( g_retroflat_state->sound.sf_bank_filename, filename_in,
      MAUG_PATH_MAX );
}

/* === */

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice ) {
   MERROR_RETVAL retval = 0;
   mfile_t opl_defs;
   uint8_t byte_buffer = 0;

   memset( &opl_defs, '\0', sizeof( mfile_t ) );

   switch( g_retroflat_state->sound.driver ) {
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

      if( '\0' == g_retroflat_state->sound.sf_bank_filename[0] ) {
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
         g_retroflat_state->sound.sf_bank_filename, &opl_defs );
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
}

/* === */

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val ) {
   MERROR_RETVAL retval = 0;

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags) ) {
      return;
   }

   switch( g_retroflat_state->sound.driver ) {
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
}

/* === */

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   MERROR_RETVAL retval = 0;

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags) ) {
      return;
   }

   switch( g_retroflat_state->sound.driver ) {
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
}

/* === */

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel ) {
   MERROR_RETVAL retval = 0;

   if( RETROSND_FLAG_INIT != (RETROSND_FLAG_INIT & g_retroflat_state->sound.flags) ) {
      return;
   }

   switch( g_retroflat_state->sound.driver ) {
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

   switch( g_retroflat_state->sound.driver ) {
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
}

#endif /* !RETPLTS_H_DEFS || RETROFLT_C */

