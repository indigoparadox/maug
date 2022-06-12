
#ifndef MTYPES_H
#define MTYPES_H

/**
 * stdint.h is great, but it's C99 and thus not available on older platforms.
 *
 * This header attempts to provide a specific but cross-platform
 * implementation that can be called from legacy projects.
 */

#if defined( PLATFORM_PALM )

typedef UInt8 uint8_t;
typedef Int8 int8_t;
typedef UInt16 uint16_t;
typedef Int16 int16_t;
typedef UInt32 uint32_t;
typedef Int32 int32_t;

#elif defined( PLATFORM_WIN )

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

#else

#include <stdint.h>

#endif /* PLATFORM_PALM || PLATFORM_WIN */

#endif /* !MTYPES_H */

