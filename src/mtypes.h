
#ifndef MTYPES_H
#define MTYPES_H

/**
 * stdint.h is great, but it's C99 and thus not available on older platforms.
 *
 * This header attempts to provide a specific but cross-platform
 * implementation that can be called from legacy projects.
 */

#ifdef MAUG_ANCIENT_C
typedef int32_t ssize_t;
#endif /* MAUG_ANCIENT_C */

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

#endif /* !MTYPES_H */

