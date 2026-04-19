
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

#define RETROSND_CHANNEL_CT         8

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

/*! \} */ /* maug_retrosnd */

#endif /* !RETROSND_H */

