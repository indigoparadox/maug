
#if !defined( MAUG_API_LOG_H_DEFS )
#define MAUG_API_LOG_H_DEFS

#include <ogc/system.h>

/* Use a wrapper macro to get the calling line number for a function. */
#  define debug_printf( lvl, ... ) \
      _internal_debug_printf( __FILE__, __LINE__, lvl, __VA_ARGS__ )

#  define error_printf( ... ) \
      _internal_error_printf( __FILE__, __LINE__, __VA_ARGS__ )

#  define logging_init() (0)
#  define logging_shutdown()

void _internal_debug_printf(
   const char* src_file, int line, int level, const char* fmt, ... );

void _internal_error_printf(
   const char* src_file, int line, const char* fmt, ... );

#elif defined( MFILE_C )

void _internal_debug_printf(
   const char* src_file, int line, int level, const char* fmt, ...
) {
   va_list argp;
   char buffer[UPRINTF_BUFFER_SZ_MAX + 1];

   if( level >= DEBUG_THRESHOLD ) {
      maug_mzero( buffer, UPRINTF_BUFFER_SZ_MAX + 1 );
      va_start( argp, fmt );
      vsnprintf( buffer, UPRINTF_BUFFER_SZ_MAX, fmt, argp );
      va_end( argp );
      SYS_Report( "(%d) %s: %d: %s\n", level, src_file, line, buffer );
   }
}

void _internal_error_printf(
   const char* src_file, int line, const char* fmt, ...
) {
   va_list argp;
   char buffer[UPRINTF_BUFFER_SZ_MAX + 1];

   maug_mzero( buffer, UPRINTF_BUFFER_SZ_MAX + 1 );
   va_start( argp, fmt );
   vsnprintf( buffer, UPRINTF_BUFFER_SZ_MAX, fmt, argp );
   va_end( argp );
   SYS_Report( "(E) %s: %d: %s\n", src_file, line, buffer );

}

#  define size_printf( lvl, name, sz ) debug_printf( lvl, name " size is " SIZE_T_FMT " bytes", (sz) );

#endif /* !MAUG_API_LOG_H_DEFS */

