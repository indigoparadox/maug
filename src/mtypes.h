
#ifndef MTYPES_H
#define MTYPES_H

/**
 * stdint.h is great, but it's C99 and thus not available on older platforms.
 *
 * This header attempts to provide a specific but cross-platform
 * implementation that can be called from legacy projects.
 */

#if defined( MAUG_OS_PALM )

typedef UInt8 uint8_t;
typedef Int8 int8_t;
typedef UInt16 uint16_t;
typedef Int16 int16_t;
typedef UInt32 uint32_t;
typedef Int32 int32_t;

typedef UInt32 maug_ms_t;

#define UPRINTF_MS_FMT "%lu"

#elif defined( MAUG_OS_WIN )

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

typedef unsigned long maug_ms_t;

#define UPRINTF_MS_FMT "%lu"

#	if defined( RETROFLAT_API_WINCE )
typedef unsigned long off_t;
#	endif /* RETROFLAT_API_WINCE */

#else

#  include <stdint.h>

#  if defined( MAUG_OS_DOS_REAL )
typedef uint16_t maug_ms_t;
#define UPRINTF_MS_FMT "%u"
#  else
typedef uint32_t maug_ms_t;
#define UPRINTF_MS_FMT "%u"
#  endif /* MAUG_OS_DOS_REAL */

#endif /* MAUG_OS_* */

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

#define UPRINTF_S16_FMT "%d"
#define UPRINTF_U16_FMT "%u"

#define UPRINTF_S16_DIGITS 5

#ifndef UPRINTF_S32_FMT
#  if __LONG_WIDTH__ == 64 || __EMSCRIPTEN__
#     define UPRINTF_S32_FMT "%d"
#  elif __LONG_WIDTH__ == 32 || defined( __WATCOMC__ ) || defined( __BORLANDC__ ) || _MSC_VER <= 1200
#     define UPRINTF_S32_FMT "%ld"
#  endif /* __LONG_WIDTH__ */
#endif /* !UPRINTF_S32_FMT */

#ifndef UPRINTF_U32_FMT
#  if __LONG_WIDTH__ == 64 || __EMSCRIPTEN__
#     define UPRINTF_U32_FMT "%u"
#  elif __LONG_WIDTH__ == 32 || defined( __WATCOMC__ ) || defined( __BORLANDC__ ) || _MSC_VER <= 1200
#     define UPRINTF_U32_FMT "%lu"
#  endif /* __LONG_WIDTH__ */
#endif /* !UPRINTF_U32_FMT */

#ifndef UPRINTF_X32_FMT
#  if __LONG_WIDTH__ == 64 || __EMSCRIPTEN__
#     define UPRINTF_X32_FMT "%x"
#  elif __LONG_WIDTH__ == 32 || defined( __WATCOMC__ ) || defined( __BORLANDC__ ) || _MSC_VER <= 1200
#     define UPRINTF_X32_FMT "%lx"
#  endif /* __LONG_WIDTH__ */
#endif /* !UPRINTF_X32_FMT */

#ifndef NEWLINE_STR
/* TODO: Autodetect. */
#  define NEWLINE_STR "\n"
#endif /* !NEWLINE_STR */

#endif /* !MTYPES_H */

