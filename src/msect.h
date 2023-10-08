
#ifndef MSECT_H
#define MSECT_H

#ifdef MAUG_OS_PALM
#  define CODE_SECTION( name ) __attribute__( (section( name )) )
#else
#  define CODE_SECTION( name )
#endif /* MAUG_OS_PALM */

#endif /* !MSECT_H */

