
#if !defined( MAUG_API_LOG_H_DEFS )
#define MAUG_API_LOG_H_DEFS

#  include <nds.h>

/* Use a wrapper macro to get the calling line number for a function. */
#  define debug_printf( lvl, ... ) \
      _internal_debug_printf( __FILE__, __LINE__, lvl, __VA_ARGS__ )

#  define logging_init()
#  define logging_shutdown()

void _internal_debug_printf(
   const char* src_file, int line, int level, const char* fmt, ... );

#elif defined( UPRINTF_C )

void _internal_debug_printf(
   const char* src_file, int line, int level, const char* fmt, ...
) {
   va_list argp;
   char buffer[UPRINTF_BUFFER_SZ_MAX + 1];
   char line_buffer[11] = { 0 };

   if( level >= DEBUG_THRESHOLD ) {
      va_start( argp, fmt );
      vsnprintf( buffer, UPRINTF_BUFFER_SZ_MAX, fmt, argp );
      va_end( argp );
      snprintf( line_buffer, 10, "%d", line );
      nocashMessage( src_file );
      nocashMessage( " (" );
      nocashMessage( line_buffer );
      nocashMessage( "): " );
      nocashMessage( buffer );
      nocashMessage( "\n" );
   }
}

static void error_printf( const char* fmt, ... ) {
   va_list argp;
   char buffer[UPRINTF_BUFFER_SZ_MAX + 1];

   va_start( argp, fmt );
   vsnprintf( buffer, UPRINTF_BUFFER_SZ_MAX, fmt, argp );
   va_end( argp );

   nocashMessage( buffer );
   nocashMessage( "\n" );
}

#  define size_printf( lvl, name, sz ) debug_printf( lvl, name " size is " SIZE_T_FMT " bytes", (sz) );

#endif /* !MAUG_API_LOG_H_DEFS */

