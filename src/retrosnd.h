
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

struct RETROSND_CHANNEL {
   uint8_t flags;
   /**
    * \brief Instrument for this channel to play.
    */
   int8_t voice;
   /**
    * \brief Note for this channel to play its instrument at.
    */
   int8_t note;
   /**
    * \brief Volume for this channel to play its instrument at.
    */
   uint8_t vol;
   /**
    * \brief Current step of the waveform to generate samples for.
    */
   uint32_t phase;
};

#ifndef RETROSND_CHANNEL_CT_MAX
#  define RETROSND_CHANNEL_CT_MAX 4
#endif /* !RETROSND_CHANNEL_CT_MAX */

#ifndef RETROSND_TRACE_LVL
#  define RETROSND_TRACE_LVL 0
#endif /* !RETROSND_TRACE_LVL */

#ifndef RETROSND_REG_TRACE_LVL
#  define RETROSND_REG_TRACE_LVL 0
#endif /* !RETROSND_REG_TRACE_LVL */

#ifdef RETROSND_SAMPLE_44100
#  define RETROSND_SAMPLE 44100
#endif /* RETROSND_SAMPLE_44100 */

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

/**
 * \brief Parameter for retrosnd_set_voice() indicating a gunshot
 *        sound effect.
 */
#define RETROSND_VOICE_GUNSHOT      128

#define RETROSND_CONTROL_VOL        7

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

void retrosnd_set_voice( uint8_t channel, uint8_t voice );

void retrosnd_set_control( uint8_t channel, uint8_t key, uint8_t val );

void retrosnd_note_on( uint8_t channel, uint8_t pitch, uint8_t vel );

void retrosnd_note_off( uint8_t channel, uint8_t pitch, uint8_t vel );

void retrosnd_shutdown();

int16_t _retrosnd_generate_note( struct RETROSND_CHANNEL* channels );

/*! \} */ /* maug_retrosnd */

#ifdef RETROSND_C

#ifdef RETROSND_SAMPLE_44100

uint32_t gc_phase_inc[] = {
   12,  /* Idx: 0, Freq: 8.175799 */
   12,  /* Idx: 1, Freq: 8.661957 */
   13,  /* Idx: 2, Freq: 9.177024 */
   14,  /* Idx: 3, Freq: 9.722718 */
   15,  /* Idx: 4, Freq: 10.300861 */
   16,  /* Idx: 5, Freq: 10.913382 */
   17,  /* Idx: 6, Freq: 11.562326 */
   18,  /* Idx: 7, Freq: 12.249857 */
   19,  /* Idx: 8, Freq: 12.978272 */
   20,  /* Idx: 9, Freq: 13.750000 */
   21,  /* Idx: 10, Freq: 14.567618 */
   22,  /* Idx: 11, Freq: 15.433853 */
   24,  /* Idx: 12, Freq: 16.351598 */
   25,  /* Idx: 13, Freq: 17.323914 */
   27,  /* Idx: 14, Freq: 18.354048 */
   28,  /* Idx: 15, Freq: 19.445436 */
   30,  /* Idx: 16, Freq: 20.601722 */
   32,  /* Idx: 17, Freq: 21.826764 */
   34,  /* Idx: 18, Freq: 23.124651 */
   36,  /* Idx: 19, Freq: 24.499715 */
   38,  /* Idx: 20, Freq: 25.956544 */
   40,  /* Idx: 21, Freq: 27.500000 */
   43,  /* Idx: 22, Freq: 29.135235 */
   45,  /* Idx: 23, Freq: 30.867706 */
   48,  /* Idx: 24, Freq: 32.703196 */
   51,  /* Idx: 25, Freq: 34.647829 */
   54,  /* Idx: 26, Freq: 36.708096 */
   57,  /* Idx: 27, Freq: 38.890873 */
   61,  /* Idx: 28, Freq: 41.203445 */
   64,  /* Idx: 29, Freq: 43.653529 */
   68,  /* Idx: 30, Freq: 46.249303 */
   72,  /* Idx: 31, Freq: 48.999429 */
   77,  /* Idx: 32, Freq: 51.913087 */
   81,  /* Idx: 33, Freq: 55.000000 */
   86,  /* Idx: 34, Freq: 58.270470 */
   91,  /* Idx: 35, Freq: 61.735413 */
   97,  /* Idx: 36, Freq: 65.406391 */
   102,  /* Idx: 37, Freq: 69.295658 */
   109,  /* Idx: 38, Freq: 73.416192 */
   115,  /* Idx: 39, Freq: 77.781746 */
   122,  /* Idx: 40, Freq: 82.406889 */
   129,  /* Idx: 41, Freq: 87.307058 */
   137,  /* Idx: 42, Freq: 92.498606 */
   145,  /* Idx: 43, Freq: 97.998859 */
   154,  /* Idx: 44, Freq: 103.826174 */
   163,  /* Idx: 45, Freq: 110.000000 */
   173,  /* Idx: 46, Freq: 116.540940 */
   183,  /* Idx: 47, Freq: 123.470825 */
   194,  /* Idx: 48, Freq: 130.812783 */
   205,  /* Idx: 49, Freq: 138.591315 */
   218,  /* Idx: 50, Freq: 146.832384 */
   231,  /* Idx: 51, Freq: 155.563492 */
   244,  /* Idx: 52, Freq: 164.813778 */
   259,  /* Idx: 53, Freq: 174.614116 */
   274,  /* Idx: 54, Freq: 184.997211 */
   291,  /* Idx: 55, Freq: 195.997718 */
   308,  /* Idx: 56, Freq: 207.652349 */
   326,  /* Idx: 57, Freq: 220.000000 */
   346,  /* Idx: 58, Freq: 233.081881 */
   366,  /* Idx: 59, Freq: 246.941651 */
   388,  /* Idx: 60, Freq: 261.625565 */
   411,  /* Idx: 61, Freq: 277.182631 */
   436,  /* Idx: 62, Freq: 293.664768 */
   462,  /* Idx: 63, Freq: 311.126984 */
   489,  /* Idx: 64, Freq: 329.627557 */
   518,  /* Idx: 65, Freq: 349.228231 */
   549,  /* Idx: 66, Freq: 369.994423 */
   582,  /* Idx: 67, Freq: 391.995436 */
   617,  /* Idx: 68, Freq: 415.304698 */
   653,  /* Idx: 69, Freq: 440.000000 */
   692,  /* Idx: 70, Freq: 466.163762 */
   733,  /* Idx: 71, Freq: 493.883301 */
   777,  /* Idx: 72, Freq: 523.251131 */
   823,  /* Idx: 73, Freq: 554.365262 */
   872,  /* Idx: 74, Freq: 587.329536 */
   924,  /* Idx: 75, Freq: 622.253967 */
   979,  /* Idx: 76, Freq: 659.255114 */
   1037,  /* Idx: 77, Freq: 698.456463 */
   1099,  /* Idx: 78, Freq: 739.988845 */
   1165,  /* Idx: 79, Freq: 783.990872 */
   1234,  /* Idx: 80, Freq: 830.609395 */
   1307,  /* Idx: 81, Freq: 880.000000 */
   1385,  /* Idx: 82, Freq: 932.327523 */
   1467,  /* Idx: 83, Freq: 987.766603 */
   1555,  /* Idx: 84, Freq: 1046.502261 */
   1647,  /* Idx: 85, Freq: 1108.730524 */
   1745,  /* Idx: 86, Freq: 1174.659072 */
   1849,  /* Idx: 87, Freq: 1244.507935 */
   1959,  /* Idx: 88, Freq: 1318.510228 */
   2075,  /* Idx: 89, Freq: 1396.912926 */
   2199,  /* Idx: 90, Freq: 1479.977691 */
   2330,  /* Idx: 91, Freq: 1567.981744 */
   2468,  /* Idx: 92, Freq: 1661.218790 */
   2615,  /* Idx: 93, Freq: 1760.000000 */
   2771,  /* Idx: 94, Freq: 1864.655046 */
   2935,  /* Idx: 95, Freq: 1975.533205 */
   3110,  /* Idx: 96, Freq: 2093.004522 */
   3295,  /* Idx: 97, Freq: 2217.461048 */
   3491,  /* Idx: 98, Freq: 2349.318143 */
   3698,  /* Idx: 99, Freq: 2489.015870 */
   3918,  /* Idx: 100, Freq: 2637.020455 */
   4151,  /* Idx: 101, Freq: 2793.825851 */
   4398,  /* Idx: 102, Freq: 2959.955382 */
   4660,  /* Idx: 103, Freq: 3135.963488 */
   4937,  /* Idx: 104, Freq: 3322.437581 */
   5230,  /* Idx: 105, Freq: 3520.000000 */
   5542,  /* Idx: 106, Freq: 3729.310092 */
   5871,  /* Idx: 107, Freq: 3951.066410 */
   6220,  /* Idx: 108, Freq: 4186.009045 */
   6590,  /* Idx: 109, Freq: 4434.922096 */
   6982,  /* Idx: 110, Freq: 4698.636287 */
   7397,  /* Idx: 111, Freq: 4978.031740 */
   7837,  /* Idx: 112, Freq: 5274.040911 */
   8303,  /* Idx: 113, Freq: 5587.651703 */
   8797,  /* Idx: 114, Freq: 5919.910763 */
   9320,  /* Idx: 115, Freq: 6271.926976 */
   9874,  /* Idx: 116, Freq: 6644.875161 */
   10461,  /* Idx: 117, Freq: 7040.000000 */
   11084,  /* Idx: 118, Freq: 7458.620184 */
   11743,  /* Idx: 119, Freq: 7902.132820 */
   12441,  /* Idx: 120, Freq: 8372.018090 */
   13181,  /* Idx: 121, Freq: 8869.844191 */
   13965,  /* Idx: 122, Freq: 9397.272573 */
   14795,  /* Idx: 123, Freq: 9956.063479 */
   15675,  /* Idx: 124, Freq: 10548.081821 */
   16607,  /* Idx: 125, Freq: 11175.303406 */
   17594,  /* Idx: 126, Freq: 11839.821527 */
   18641,  /* Idx: 127, Freq: 12543.853951 */
   19749 /* Idx: 128, Freq: 13289.750323 */
};

#else
#  error "warning: no sample rate specified!"
#endif /* RETROSND_SAMPLE_* */

/* === */

int16_t _retrosnd_generate_note( struct RETROSND_CHANNEL* channels ) {
   int32_t mix = 0;
   int i = 0;

   /* Mix each of the channels into a single sample. */
   for( i = 0 ; RETROSND_CHANNEL_CT_MAX > i ; i++ ) {
      if( 0 > channels[i].note ) {
         continue;
      }

      channels[i].phase += gc_phase_inc[channels[i].note];

      switch( channels[i].voice ) {
      case 0:
         /* Triangle wave. */
         if( 32768 > channels[i].phase ) {
            mix += (channels[i].phase - 16384) * 2;
         } else {
            mix += ((uint16_t)((((65536 - channels[i].phase)
               - 16384) * 2)) * channels[i].vol) >> 8;
         }
         break;

      case 1:
         /* Square wave. */
         mix += ((INT16_MAX >= (uint16_t)(channels[i].phase) ? 30000 : -30000)
               * channels[i].vol) >> 8;
         break;

      case 2:
         mix += retroflat_get_rand() % INT16_MAX;
         break;
      }
   }

   /* Clip the sample and assign it. */
   if( INT16_MAX <= mix ) {
      return INT16_MAX;
   } else if( INT16_MIN > mix ) {
      return INT16_MIN;
   } else {
      return mix;
   }
}

#endif /* RETROSND_C */

#endif /* !RETROSND_H */

