
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

#define MS_FMT "%lu"

#elif defined( MAUG_OS_WIN )

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

typedef unsigned long maug_ms_t;

#define MS_FMT "%lu"

#	if defined( RETROFLAT_API_WINCE )
typedef unsigned long off_t;
#	endif /* RETROFLAT_API_WINCE */

#else

#  include <stdint.h>

#  if defined( MAUG_OS_DOS_REAL )
typedef uint16_t maug_ms_t;
#define MS_FMT "%u"
#  else
typedef uint32_t maug_ms_t;
#define MS_FMT "%u"
#  endif /* MAUG_OS_DOS_REAL */

#endif /* MAUG_OS_* */

#ifdef MAUG_ANCIENT_C
typedef int32_t ssize_t;
#endif /* MAUG_ANCIENT_C */

#endif /* !MTYPES_H */

