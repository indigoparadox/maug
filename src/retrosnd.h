
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

#ifndef RETROSND_CHANNEL_CT_MAX
#  define RETROSND_CHANNEL_CT_MAX 4
#endif /* !RETROSND_CHANNEL_CT_MAX */

#ifndef RETROSND_TRACE_LVL
#  define RETROSND_TRACE_LVL 0
#endif /* !RETROSND_TRACE_LVL */

#ifndef RETROSND_REG_TRACE_LVL
#  define RETROSND_REG_TRACE_LVL 0
#endif /* !RETROSND_REG_TRACE_LVL */

#ifndef RETROSND_SAMPLES_CT
#  define RETROSND_SAMPLES_CT 2048
#endif /* !RETROSND_SAMPLES_CT */

#ifdef RETROSND_SAMPLE_22050
#  define RETROSND_SAMPLE_RATE 22050
#elif defined( RETROSND_SAMPLE_44100 )
/* Specify the sample rate as a number based on existends of predefined flag.
 */
#  define RETROSND_SAMPLE_RATE 44100
#endif /* RETROSND_SAMPLE_44100 */

struct RETROSND_CHANNEL {
   uint8_t flags;
   /**
    * \brief Instrument for this channel to play.
    */
   int8_t voice;
   /**
    * \brief Note for this channel to play its instrument at.
    */
   uint8_t note;
   /**
    * \brief Volume for this channel to play its instrument at.
    */
   uint8_t vol;
   uint8_t noise_time;
   int16_t noise_last;
   /**
    * \brief Current step of the waveform to generate samples for.
    */
   uint32_t phase;

   retroflat_ms_t deadline;
};

/**
 * \brief Simple tune designed to be played on the software synthesizer.
 *
 * Requires retrosnd_tune_update() to be called at regular intervals.
 */
struct RETROSND_TUNE {
   /*! \brief Size of this struct (useful for serializing). */
   size_t sz;
   /*! \brief Size of the tune in bytes (including this struct header). */
   size_t total_sz;
   uint8_t notes_ct;
   int8_t current_note_idx;
   int16_t ms_per_note;
   retroflat_ms_t next_note_at;
   uint8_t notes[][RETROSND_CHANNEL_CT_MAX];
};

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

void retrosnd_pump();

void retrosnd_note_on_deadline(
   uint8_t channel, uint8_t pitch, retroflat_ms_t after );

MERROR_RETVAL retrosnd_tune_init(
   MAUG_MHANDLE* p_tune_h, size_t notes_ct, uint16_t ms_per_note );

/**
 * \brief Update the currently playing note in the given tune.
 * \return MERROR_OVERFLOW if the tune is finished playing (runs out of notes)
 *         or MERROR_OK otherwise. If it is done playing,
 *         RETROSND_TUNE::current_note_idx will be reset to 0 automatically
 *         so that it will loop.
 *
 * This should be called once per frame with the same tune.
 */
MERROR_RETVAL retrosnd_tune_update( struct RETROSND_TUNE* tune );

void retrosnd_tune_seek( struct RETROSND_TUNE* tune, int8_t idx );

MERROR_RETVAL retrosnd_tune_set_note(
   struct RETROSND_TUNE*, int channel, int index, int8_t note );

#define retrosnd_tune_get_note( tune, channel, idx ) \
   ((tune)->notes[idx][channel])

/**
 * \brief Increment synthesizer phase based on its instrument and system
 *        settings.
 * \note This should only be called by the API-specific implementation, and not
 *       a client program.
 */
int16_t _retrosnd_generate_note( struct RETROSND_CHANNEL* channels );

/**
 * \brief Set controls for the software synthesizer.
 * \note This should only be called by the API-specific implementation, and not
 *       a client program.
 */
MERROR_RETVAL _retrosnd_set_control(
   struct RETROSND_CHANNEL* channel, uint8_t key, uint8_t val );

#define RETROSND_TUNE_NOTE_DISABLED (255)

#define RETROSND_NOTES_START (36)

#define RETROSND_NOTES_OCTAVE_SZ (12)

#define RETROSND_NOTES_SZ (RETROSND_NOTES_OCTAVE_SZ * 3)

#define RETROSND_NOTES_OCTAVE_MIDDLE (48)

#define RETROSND_NOTES_TABLE( f ) \
   f( 36, C2 ) \
   f( 37, D2F ) \
   f( 38, D2 ) \
   f( 39, E2F ) \
   f( 40, E2 ) \
   f( 41, F2 ) \
   f( 42, G2F ) \
   f( 43, G2 ) \
   f( 44, A2F ) \
   f( 45, A2 ) \
   f( 46, B2F ) \
   f( 47, B2 ) \
   f( 48, C3 ) \
   f( 49, D3F ) \
   f( 50, D3 ) \
   f( 51, E3F ) \
   f( 52, E3 ) \
   f( 53, F3 ) \
   f( 54, G3F ) \
   f( 55, G3 ) \
   f( 56, A3F ) \
   f( 57, A3 ) \
   f( 58, B3F ) \
   f( 59, B3 ) \
   f( 60, C4 ) \
   f( 61, D4F ) \
   f( 62, D4 ) \
   f( 63, E4F ) \
   f( 64, E4 ) \
   f( 65, F4 ) \
   f( 66, G4F ) \
   f( 67, G4 ) \
   f( 68, A4F ) \
   f( 69, A4 ) \
   f( 70, B4F ) \
   f( 71, B4 )

/*! \} */ /* maug_retrosnd */

#ifdef RETROSND_C

#define RETROSND_NOTES_TABLE_CONST_NAMES( num, name ) \
   #name,

MAUG_CONST char* gc_retrosnd_note_names[RETROSND_NOTES_SZ] = {
RETROSND_NOTES_TABLE( RETROSND_NOTES_TABLE_CONST_NAMES )
};

#define RETROSND_NOTES_TABLE_CONST_CONSTS( num, name ) \
   MAUG_CONST int8_t RSN_ ## name = num;

RETROSND_NOTES_TABLE( RETROSND_NOTES_TABLE_CONST_CONSTS )

#ifdef RETROSND_SAMPLE_22050

uint32_t gc_phase_inc[] = {
   24,  /* Idx: 0, Freq: 8.175799 */
   25,  /* Idx: 1, Freq: 8.661957 */
   27,  /* Idx: 2, Freq: 9.177024 */
   28,  /* Idx: 3, Freq: 9.722718 */
   30,  /* Idx: 4, Freq: 10.300861 */
   32,  /* Idx: 5, Freq: 10.913382 */
   34,  /* Idx: 6, Freq: 11.562326 */
   36,  /* Idx: 7, Freq: 12.249857 */
   38,  /* Idx: 8, Freq: 12.978272 */
   40,  /* Idx: 9, Freq: 13.750000 */
   43,  /* Idx: 10, Freq: 14.567618 */
   45,  /* Idx: 11, Freq: 15.433853 */
   48,  /* Idx: 12, Freq: 16.351598 */
   51,  /* Idx: 13, Freq: 17.323914 */
   54,  /* Idx: 14, Freq: 18.354048 */
   57,  /* Idx: 15, Freq: 19.445436 */
   61,  /* Idx: 16, Freq: 20.601722 */
   64,  /* Idx: 17, Freq: 21.826764 */
   68,  /* Idx: 18, Freq: 23.124651 */
   72,  /* Idx: 19, Freq: 24.499715 */
   77,  /* Idx: 20, Freq: 25.956544 */
   81,  /* Idx: 21, Freq: 27.500000 */
   86,  /* Idx: 22, Freq: 29.135235 */
   91,  /* Idx: 23, Freq: 30.867706 */
   97,  /* Idx: 24, Freq: 32.703196 */
   102,  /* Idx: 25, Freq: 34.647829 */
   109,  /* Idx: 26, Freq: 36.708096 */
   115,  /* Idx: 27, Freq: 38.890873 */
   122,  /* Idx: 28, Freq: 41.203445 */
   129,  /* Idx: 29, Freq: 43.653529 */
   137,  /* Idx: 30, Freq: 46.249303 */
   145,  /* Idx: 31, Freq: 48.999429 */
   154,  /* Idx: 32, Freq: 51.913087 */
   163,  /* Idx: 33, Freq: 55.000000 */
   173,  /* Idx: 34, Freq: 58.270470 */
   183,  /* Idx: 35, Freq: 61.735413 */
   194,  /* Idx: 36, Freq: 65.406391 */
   205,  /* Idx: 37, Freq: 69.295658 */
   218,  /* Idx: 38, Freq: 73.416192 */
   231,  /* Idx: 39, Freq: 77.781746 */
   244,  /* Idx: 40, Freq: 82.406889 */
   259,  /* Idx: 41, Freq: 87.307058 */
   274,  /* Idx: 42, Freq: 92.498606 */
   291,  /* Idx: 43, Freq: 97.998859 */
   308,  /* Idx: 44, Freq: 103.826174 */
   326,  /* Idx: 45, Freq: 110.000000 */
   346,  /* Idx: 46, Freq: 116.540940 */
   366,  /* Idx: 47, Freq: 123.470825 */
   388,  /* Idx: 48, Freq: 130.812783 */
   411,  /* Idx: 49, Freq: 138.591315 */
   436,  /* Idx: 50, Freq: 146.832384 */
   462,  /* Idx: 51, Freq: 155.563492 */
   489,  /* Idx: 52, Freq: 164.813778 */
   518,  /* Idx: 53, Freq: 174.614116 */
   549,  /* Idx: 54, Freq: 184.997211 */
   582,  /* Idx: 55, Freq: 195.997718 */
   617,  /* Idx: 56, Freq: 207.652349 */
   653,  /* Idx: 57, Freq: 220.000000 */
   692,  /* Idx: 58, Freq: 233.081881 */
   733,  /* Idx: 59, Freq: 246.941651 */
   777,  /* Idx: 60, Freq: 261.625565 */
   823,  /* Idx: 61, Freq: 277.182631 */
   872,  /* Idx: 62, Freq: 293.664768 */
   924,  /* Idx: 63, Freq: 311.126984 */
   979,  /* Idx: 64, Freq: 329.627557 */
   1037,  /* Idx: 65, Freq: 349.228231 */
   1099,  /* Idx: 66, Freq: 369.994423 */
   1165,  /* Idx: 67, Freq: 391.995436 */
   1234,  /* Idx: 68, Freq: 415.304698 */
   1307,  /* Idx: 69, Freq: 440.000000 */
   1385,  /* Idx: 70, Freq: 466.163762 */
   1467,  /* Idx: 71, Freq: 493.883301 */
   1555,  /* Idx: 72, Freq: 523.251131 */
   1647,  /* Idx: 73, Freq: 554.365262 */
   1745,  /* Idx: 74, Freq: 587.329536 */
   1849,  /* Idx: 75, Freq: 622.253967 */
   1959,  /* Idx: 76, Freq: 659.255114 */
   2075,  /* Idx: 77, Freq: 698.456463 */
   2199,  /* Idx: 78, Freq: 739.988845 */
   2330,  /* Idx: 79, Freq: 783.990872 */
   2468,  /* Idx: 80, Freq: 830.609395 */
   2615,  /* Idx: 81, Freq: 880.000000 */
   2771,  /* Idx: 82, Freq: 932.327523 */
   2935,  /* Idx: 83, Freq: 987.766603 */
   3110,  /* Idx: 84, Freq: 1046.502261 */
   3295,  /* Idx: 85, Freq: 1108.730524 */
   3491,  /* Idx: 86, Freq: 1174.659072 */
   3698,  /* Idx: 87, Freq: 1244.507935 */
   3918,  /* Idx: 88, Freq: 1318.510228 */
   4151,  /* Idx: 89, Freq: 1396.912926 */
   4398,  /* Idx: 90, Freq: 1479.977691 */
   4660,  /* Idx: 91, Freq: 1567.981744 */
   4937,  /* Idx: 92, Freq: 1661.218790 */
   5230,  /* Idx: 93, Freq: 1760.000000 */
   5542,  /* Idx: 94, Freq: 1864.655046 */
   5871,  /* Idx: 95, Freq: 1975.533205 */
   6220,  /* Idx: 96, Freq: 2093.004522 */
   6590,  /* Idx: 97, Freq: 2217.461048 */
   6982,  /* Idx: 98, Freq: 2349.318143 */
   7397,  /* Idx: 99, Freq: 2489.015870 */
   7837,  /* Idx: 100, Freq: 2637.020455 */
   8303,  /* Idx: 101, Freq: 2793.825851 */
   8797,  /* Idx: 102, Freq: 2959.955382 */
   9320,  /* Idx: 103, Freq: 3135.963488 */
   9874,  /* Idx: 104, Freq: 3322.437581 */
   10461,  /* Idx: 105, Freq: 3520.000000 */
   11084,  /* Idx: 106, Freq: 3729.310092 */
   11743,  /* Idx: 107, Freq: 3951.066410 */
   12441,  /* Idx: 108, Freq: 4186.009045 */
   13181,  /* Idx: 109, Freq: 4434.922096 */
   13965,  /* Idx: 110, Freq: 4698.636287 */
   14795,  /* Idx: 111, Freq: 4978.031740 */
   15675,  /* Idx: 112, Freq: 5274.040911 */
   16607,  /* Idx: 113, Freq: 5587.651703 */
   17594,  /* Idx: 114, Freq: 5919.910763 */
   18641,  /* Idx: 115, Freq: 6271.926976 */
   19749,  /* Idx: 116, Freq: 6644.875161 */
   20923,  /* Idx: 117, Freq: 7040.000000 */
   22168,  /* Idx: 118, Freq: 7458.620184 */
   23486,  /* Idx: 119, Freq: 7902.132820 */
   24882,  /* Idx: 120, Freq: 8372.018090 */
   26362,  /* Idx: 121, Freq: 8869.844191 */
   27930,  /* Idx: 122, Freq: 9397.272573 */
   29590,  /* Idx: 123, Freq: 9956.063479 */
   31350,  /* Idx: 124, Freq: 10548.081821 */
   33214,  /* Idx: 125, Freq: 11175.303406 */
   35189,  /* Idx: 126, Freq: 11839.821527 */
   37282,  /* Idx: 127, Freq: 12543.853951 */
   39499 /* Idx: 128, Freq: 13289.750323 */
};

#elif defined( RETROSND_SAMPLE_44100 )

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

MERROR_RETVAL retrosnd_tune_init(
   MAUG_MHANDLE* p_tune_h, size_t notes_ct, uint16_t ms_per_note
) {
   struct RETROSND_TUNE* tune = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   maug_malloc_test( *p_tune_h, 1,
      sizeof( struct RETROSND_TUNE ) + (notes_ct * RETROSND_CHANNEL_CT_MAX ) );
   maug_mlock( *p_tune_h, tune );
   maug_cleanup_if_null_lock( struct RETROSND_TUNE*, tune );

   maug_mzero( tune,
      sizeof( struct RETROSND_TUNE ) + (notes_ct * RETROSND_CHANNEL_CT_MAX ) );

   memset( tune->notes, RETROSND_TUNE_NOTE_DISABLED,
      RETROSND_CHANNEL_CT_MAX * notes_ct );

   tune->sz = sizeof( struct RETROSND_TUNE );
   tune->notes_ct = notes_ct;
   tune->total_sz = sizeof( struct RETROSND_TUNE ) +
      (RETROSND_CHANNEL_CT_MAX * tune->notes_ct);
   tune->ms_per_note = ms_per_note;

cleanup:

   if( NULL != tune ) {
      maug_munlock( *p_tune_h, tune );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrosnd_tune_update( struct RETROSND_TUNE* tune ) {
   MERROR_RETVAL retval = MERROR_OK;
   int prev_note = 0, i = 0;

   if( retroflat_get_ms() > tune->next_note_at ) {
      /* It's time to advance the playing note in the tune! */
      tune->next_note_at = retroflat_get_ms() + tune->ms_per_note;
      prev_note = tune->current_note_idx;
      tune->current_note_idx++;
      if( tune->notes_ct <= tune->current_note_idx ) {
         tune->current_note_idx = 0;
         retval = MERROR_OVERFLOW;
      }
#if 0 < RETROSND_TUNE_TRACE_LVL
      debug_printf( RETROSND_TUNE_TRACE_LVL, "next note index: %d\n",
         tune->current_note_idx );
#endif /* RETROSND_TUNE_TRACE_LVL */

      /* Ensure playing blocks are playing and stopped are stopped. */
      for( i = 0 ; RETROSND_CHANNEL_CT_MAX > i ; i++ ) {
         if( 0 <= prev_note ) {
            retrosnd_note_off( i, tune->notes[prev_note][i], 0 );
         }
         if( MERROR_OK == retval ) {
            retrosnd_note_on( i, tune->notes[tune->current_note_idx][i], 0 );
         }
      }
   }

   return retval;
}

/* === */

void retrosnd_tune_seek( struct RETROSND_TUNE* tune, int8_t idx ) {
   int i = 0;
   if( 0 <= tune->current_note_idx ) {
      for( i = 0 ; RETROSND_CHANNEL_CT_MAX > i ; i++ ) {
         retrosnd_note_off( i, tune->notes[tune->current_note_idx][i], 0 );
      }
   }
   tune->current_note_idx = idx;
   tune->next_note_at = 0;
}

/* === */

MERROR_RETVAL retrosnd_tune_set_note(
   struct RETROSND_TUNE* tune, int channel, int index, int8_t note
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( index >= tune->notes_ct ) {
      error_printf( "invalid note index specified: %d", index );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   if( channel >= RETROSND_CHANNEL_CT_MAX ) {
      error_printf( "invalid channel specified: %d", channel );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   tune->notes[index][channel] = note;

cleanup:

   return retval;
}

/* === */

MAUG_CONST char* retrosnd_note_to_str( int8_t note ) {
   if(
      RETROSND_NOTES_START > note ||
      RETROSND_NOTES_START + RETROSND_NOTES_SZ < note
   ) {
      return "?";
   }

   return gc_retrosnd_note_names[note - RETROSND_NOTES_START];
}

#ifndef RETROFLAT_NO_SOUND

/* === */

MERROR_RETVAL _retrosnd_channels_init( struct RETROSND_CHANNEL* channels ) {
   MERROR_RETVAL retval = MERROR_OK;
   int i = 0;

   for( i = 0 ; RETROSND_CHANNEL_CT_MAX > i ; i++ ) {
      channels[i].note = RETROSND_TUNE_NOTE_DISABLED;
   }

   /* Setup synthesizer defaults. */

   retrosnd_set_voice( 0, 0 );
   retrosnd_set_voice( 1, 0 );
   retrosnd_set_voice( 2, 1 );
   retrosnd_set_voice( 3, 2 );

   retrosnd_set_control( 0, RETROSND_CONTROL_VOL, 128 );
   retrosnd_set_control( 1, RETROSND_CONTROL_VOL, 128 );
   retrosnd_set_control( 2, RETROSND_CONTROL_VOL, 128 );
   retrosnd_set_control( 3, RETROSND_CONTROL_VOL, 128 );

   return retval;
}

int16_t _retrosnd_generate_note( struct RETROSND_CHANNEL* channels ) {
   int32_t mix = 0;
   int i = 0;

   /* Mix each of the channels into a single sample. */
   for( i = 0 ; RETROSND_CHANNEL_CT_MAX > i ; i++ ) {
      if( RETROSND_TUNE_NOTE_DISABLED == channels[i].note ) {
         continue;
      }

      if(
         channels[i].deadline > 0 && channels[i].deadline < retroflat_get_ms()
      ) {
#if RETROSND_TRACE_LVL > 0
         debug_printf( RETROSND_TRACE_LVL, "note %d disabled after deadline!",
            i );
#endif /* RETROSND_TRACE_LVL */
         channels[i].note = RETROSND_TUNE_NOTE_DISABLED;
         channels[i].deadline = 0;
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
         if( 0 < channels[i].noise_time ) {
            channels[i].noise_time--;
         } else {
            channels[i].noise_last = retroflat_get_rand() % INT16_MAX;
            channels[i].noise_time = channels[i].note / 2;
         }
         mix += channels[i].noise_last;
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

/* === */

MERROR_RETVAL _retrosnd_set_control(
   struct RETROSND_CHANNEL* channel, uint8_t key, uint8_t val
) {
   MERROR_RETVAL retval = MERROR_OK;

   switch( key ) {
   case RETROSND_CONTROL_VOL:
      channel->vol = val;
      break;

   default:
      error_printf( "unimplemented control message: %d!", key );
      retval = MERROR_OVERFLOW;
      break;
   }

   return retval;
}

#endif /* !RETROFLAT_NO_SOUND */

#else

#define RETROSND_NOTES_TABLE_EXT_CONSTS( num, name ) \
   extern MAUG_CONST int8_t RSN_ ## name;

RETROSND_NOTES_TABLE( RETROSND_NOTES_TABLE_EXT_CONSTS )

#endif /* RETROSND_C */

#endif /* !RETROSND_H */

