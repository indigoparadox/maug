
#if !defined( MAUG_API_LOG_H_DEFS )
#define MAUG_API_LOG_H_DEFS

#include <stdarg.h>
#include <psxsio.h>

#ifndef UPRINTF_BUFFER_SZ_MAX
#  define UPRINTF_BUFFER_SZ_MAX 1024
#endif /* !UPRINTF_BUFFER_SZ_MAX */

#if !defined( DEBUG_THRESHOLD )
#  define DEBUG_THRESHOLD 1
#endif /* !DEBUG_THRESHOLD */

void _internal_debug_printf(
  int lvl, const char* src, size_t line, const char* fmt, ... );

#  define debug_printf( lvl, ... ) \
      _internal_debug_printf( lvl, __FILE__, __LINE__, __VA_ARGS__ )

#  define error_printf( ... ) \
      _internal_debug_printf( -1, __FILE__, __LINE__, __VA_ARGS__ )

#  define size_printf( lvl, name, sz ) \
      _internal_debug_printf( lvl, __FILE__, __LINE__, \
         name " size is " SIZE_T_FMT " bytes", (sz) );

#  define size_multi_printf( lvl, name, sz, max ) \
      _internal_debug_printf( lvl, __FILE__, __LINE__, \
         "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

MERROR_RETVAL logging_init();

void logging_shutdown();

#elif defined( MFILE_C )

MERROR_RETVAL logging_init() {
   MERROR_RETVAL retval = MERROR_OK;
   /*
   AddSIO( 115200 );
   */
   return retval;
}

/* === */

void logging_shutdown() {
}

/* === */

void _internal_debug_printf(
   int lvl, const char* src, size_t line, const char* fmt, ...
) {
   va_list vargs;
   char message[255] = { 0 };

   va_start( vargs, fmt );

   if( 0 > lvl || lvl >= DEBUG_THRESHOLD ) {
      printf( "(%d) %s: " SIZE_T_FMT ": ", lvl, src, line );
      maug_vsnprintf( message, 254, fmt, vargs );
      printf( "%s", message );
      printf( NEWLINE_STR );
   }

   va_end( vargs );
}

#endif /* !MAUG_API_LOG_H_DEFS */

