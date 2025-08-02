
#if !defined( MAUG_API_LOG_H_DEFS )
#define MAUG_API_LOG_H_DEFS

#include <stdarg.h>

#ifndef LOG_FILE_NAME
#  pragma message( "warning: no log file name defined!" )
#  define LOG_FILE_NAME "out.log"
#endif /* !LOG_FILE_NAME */

#  ifdef NO_DEBUG_LINE_NUMBER
#     define LINE_NUMBER() 0
#  else
#     define LINE_NUMBER() __LINE__
#  endif

#ifdef MAUG_ANCIENT_C

/* Old compilers can't do variadic macros, but we lose line numbers and
 * filenames this way.
 */

void _internal_debug_printf(
  int lvl, const char* src, size_t line, const char* fmt, va_list vargs );

void debug_printf( int lvl, const char* fmt, ... );

void error_printf( const char* fmt, ... );

#else

void _internal_debug_printf(
  int lvl, const char* src, size_t line, const char* fmt, ... );

#  define debug_printf( lvl, ... ) \
      _internal_debug_printf( lvl, __FILE__, __LINE__, __VA_ARGS__ )

#  define error_printf( ... ) \
      _internal_debug_printf( -1, __FILE__, __LINE__, __VA_ARGS__ )

#endif /* MAUG_ANCIENT_C */

#define size_printf( lvl, name, sz ) \
      _internal_debug_printf( lvl, __FILE__, __LINE__, \
         name " size is " SIZE_T_FMT " bytes", (sz) );

#define size_multi_printf( lvl, name, sz, max ) \
      _internal_debug_printf( lvl, __FILE__, __LINE__, \
         "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

#  ifndef MFILE_C
extern struct MFILE_CADDY SEG_MGLOBAL g_log_file;
#  endif /* MFILE_C */
MERROR_RETVAL _internal_logging_init();
#  define logging_init() \
      retval = _internal_logging_init(); maug_cleanup_if_not_ok();
void logging_shutdown();

#elif defined( MFILE_C )

struct MFILE_CADDY SEG_MGLOBAL g_log_file;

#ifdef MAUG_ANCIENT_C
void _internal_debug_printf(
   int lvl, const char* src, size_t line, const char* fmt, va_list vargs
) {
#else
void _internal_debug_printf(
   int lvl, const char* src, size_t line, const char* fmt, ...
) {
   va_list vargs;

   va_start( vargs, fmt );
   /* vprintf( fmt, vargs ); */
#endif /* MAUG_ANCIENT_C */

   if( NULL == g_log_file.printf || lvl < DEBUG_THRESHOLD ) {
      goto cleanup;
   }

   /* Try to start the line out with the preamble. */
   g_log_file.printf( &g_log_file, 0,
      "(%d) %s: " SIZE_T_FMT ": ", lvl, src, line );

   g_log_file.vprintf( &g_log_file, 0, fmt, vargs );

   /* Try to append a newline. */
   g_log_file.printf( &g_log_file, 0, "\n" );

cleanup:
#ifdef MAUG_ANCIENT_C
   va_end( vargs );
#endif /* MAUG_ANCIENT_C */

   return;
}

#ifdef MAUG_ANCIENT_C

void debug_printf( int lvl, const char* fmt, ... ) {
   va_list vargs;
   va_start( vargs, fmt );
   _internal_debug_printf( lvl, "UNK", 0, fmt, vargs );
   va_end( vargs );
}

void error_printf( const char* fmt, ... ) {
   va_list vargs;
   va_start( vargs, fmt );
   _internal_debug_printf( -1, "UNK", 0, fmt, vargs );
   va_end( vargs );
}

#endif /* MAUG_ANCIENT_C */

/* === */

MERROR_RETVAL _internal_logging_init() {
   MERROR_RETVAL retval = MERROR_OK;
   retval = mfile_open_write( LOG_FILE_NAME, &g_log_file );
   if( MERROR_OK != retval ) {
      retroflat_message( 1, /* RETROFLAT_MSG_FLAG_ERROR */
         "Error", "Unable to initiate logging!" );
   }
   return retval;
}

/* === */

void logging_shutdown() {
   mfile_close( &g_log_file );
}

#endif /* !MAUG_API_LOG_H_DEFS */

