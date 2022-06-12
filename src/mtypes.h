
#ifndef MTYPES_H
#define MTYPES_H

#ifdef PLATFORM_PALM

typedef UInt8 uint8_t;
typedef Int8 int8_t;
typedef UInt16 uint16_t;
typedef Int16 int16_t;
typedef UInt32 uint32_t;
typedef Int32 int32_t;

#else

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

#endif /* PLATFORM_PALM */

#endif /* !MTYPES_H */

