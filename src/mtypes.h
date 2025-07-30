
#ifndef MTYPES_H
#define MTYPES_H

/**
 * \file mtypes.h
 * \brief Macros, constants, and typedefs for cross-platform consistency.
 * \{
 */

/**
 * stdint.h is great, but it's C99 and thus not available on older platforms.
 *
 * This header attempts to provide a specific but cross-platform
 * implementation that can be called from legacy projects.
 */

#if defined( __BYTE_ORDER ) && __BYTE_ORDER == __BIG_ENDIAN || \
defined( __BYTE_ORDER__ ) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ || \
defined( __BIG_ENDIAN__ ) || \
defined( __ARMEB__ ) || \
defined( __THUMBEB__ ) || \
defined( __AARCH64EB__ ) || \
defined( _MIBSEB ) || defined( __MIBSEB ) || defined( __MIBSEB__ ) || \
defined( DOCUMENTATION )

#  ifdef MAUG_END_TRACE
#     pragma message "platform is MSBF..."
#  endif /* MAUG_END_TRACE */

/**
 * \brief Macro indicating the platform is natively most-significant byte first
 *        On platforms that are least-significant byte first, MAUG_LSBF
 *        will be defined, instead.
 */
#  define MAUG_MSBF

#  define maug_msbf_16( n ) (n)
#  define maug_msbf_32( n ) (n)
#  define maug_lsbf_16( n ) (((n) >> 8) | ((n) << 8))
#  define maug_lsbf_32( n ) ((((n) >> 24) & 0xff) | \
      (((n) << 8) & 0xff0000) | (((n) >> 8) & 0xff00) | \
      (((n) << 24) & 0xff000000 ))

#elif defined( __BYTE_ORDER ) && __BYTE_ORDER == __LITTLE_ENDIAN || \
defined( __BYTE_ORDER__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || \
defined( __LITTLE_ENDIAN__ ) || \
defined( __ARMEL__ ) || \
defined( __THUMBEL__ ) || \
defined( __AARCH64EL__ ) || \
defined( _MIPSEL ) || defined( __MIPSEL ) || defined( __MIPSEL__ ) || \
defined( _M_MRX000 ) || defined( _M_SH ) || \
defined( __WATCOMC__ )

#  ifdef MAUG_END_TRACE
#     pragma message "platform is LSBF..."
#  endif /* MAUG_END_TRACE */

#  define MAUG_LSBF

#  define maug_msbf_16( n ) (((n) >> 8) | ((n) << 8))
#  define maug_msbf_32( n ) ((((n) >> 24) & 0xff) | \
      (((n) << 8) & 0xff0000) | (((n) >> 8) & 0xff00) | \
      (((n) << 24) & 0xff000000 ))
#  define maug_lsbf_16( n ) (n)
#  define maug_lsbf_32( n ) (n)

#else
   #error "unable to determine byte order!"
#endif

/* Helpful type-related constants. */

#ifdef __GNUC__
#  ifdef __EMSCRIPTEN__ /* __SIZE_WIDTH__ == 64 */
#     define SIZE_T_FMT "%lu"
#     define SSIZE_T_FMT "%ld"
#     define OFF_T_FMT  "%lld"
#  elif defined( _WIN64 ) /* __SIZE_WIDTH__ == 64 */
#     define SIZE_T_FMT "%I64u"
#     define SSIZE_T_FMT "%I64d"
#     define OFF_T_FMT  "%I32d"
#  else
#     define SIZE_T_FMT "%lu"
#     define SSIZE_T_FMT "%ld"
#     define OFF_T_FMT  "%lu"
#  endif /* __LONG_WIDTH__ */
#else
#  define SIZE_T_FMT "%u"
#  define SSIZE_T_FMT "%d"
#  define OFF_T_FMT  "%lu"
#endif /* __GNUC__ */

#define S16_FMT "%d"
#define U16_FMT "%u"

#define S16_DIGITS 5

#ifndef S32_FMT
#  if __LONG_WIDTH__ == 64 || __EMSCRIPTEN__
#     define S32_FMT "%d"
#  elif __LONG_WIDTH__ == 32 || defined( __WATCOMC__ ) || defined( __BORLANDC__ ) || _MSC_VER <= 1200
#     define S32_FMT "%ld"
#  endif /* __LONG_WIDTH__ */
#endif /* !S32_FMT */

#ifndef U32_FMT
#  if __LONG_WIDTH__ == 64 || __EMSCRIPTEN__
#     define U32_FMT "%u"
#  elif __LONG_WIDTH__ == 32 || defined( __WATCOMC__ ) || defined( __BORLANDC__ ) || _MSC_VER <= 1200
#     define U32_FMT "%lu"
#  endif /* __LONG_WIDTH__ */
#endif /* !U32_FMT */

#ifndef X32_FMT
#  if __LONG_WIDTH__ == 64 || __EMSCRIPTEN__
#     define X32_FMT "%x"
#  elif __LONG_WIDTH__ == 32 || defined( __WATCOMC__ ) || defined( __BORLANDC__ ) || _MSC_VER <= 1200
#     define X32_FMT "%lx"
#  endif /* __LONG_WIDTH__ */
#endif /* !X32_FMT */

#ifndef NEWLINE_STR
/* TODO: Autodetect. */
#  define NEWLINE_STR "\n"
#endif /* !NEWLINE_STR */

#if defined( MAUG_OS_PALM )

typedef UInt8 uint8_t;
typedef Int8 int8_t;
typedef UInt16 uint16_t;
typedef Int16 int16_t;
typedef UInt32 uint32_t;
typedef Int32 int32_t;

typedef UInt32 maug_ms_t;

#define MS_FMT U32_FMT

#elif defined( MAUG_OS_WIN )

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

typedef uint32_t maug_ms_t;

#define MS_FMT U32_FMT

#	if defined( RETROFLAT_API_WINCE )
typedef unsigned long off_t;
#	endif /* RETROFLAT_API_WINCE */

#else

#  include <stdint.h>

#  if defined( MAUG_OS_DOS_REAL )
typedef uint16_t maug_ms_t;
#define MS_FMT U16_FMT
#  else
typedef uint32_t maug_ms_t;
#define MS_FMT U32_FMT
#  endif /* MAUG_OS_DOS_REAL */

#endif /* MAUG_OS_* */

#ifdef MAUG_NO_STDLIB
typedef unsigned long size_t;
#endif

#if defined( MAUG_ANCIENT_C ) || defined( MAUG_NO_STDLIB )
typedef int32_t ssize_t;
#endif /* MAUG_ANCIENT_C */

/*! \} */

#endif /* !MTYPES_H */

