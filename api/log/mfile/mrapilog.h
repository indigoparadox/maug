
#if !defined( MAUG_API_LOG_H_DEFS )
#define MAUG_API_LOG_H_DEFS

#include <stdarg.h>

#ifndef MAUG_LOG_WIN_BUF_SZ_MAX
#  define MAUG_LOG_WIN_BUF_SZ_MAX 255
#endif /* !MAUG_LOG_WIN_BUF_SZ_MAX */

#  ifdef NO_DEBUG_LINE_NUMBER
#     define LINE_NUMBER() 0
#  else
#     define LINE_NUMBER() __LINE__
#  endif

void _internal_debug_printf( int lvl, size_t line, const char* fmt, ... );

#  define debug_printf( lvl, ... ) \
      _internal_debug_printf( lvl, __LINE__, __VA_ARGS__ )

#  define error_printf( ... ) \
      _internal_debug_printf( -1, __LINE__, __VA_ARGS__ )

#  define size_printf( lvl, name, sz ) \
      _internal_debug_printf( lvl, __LINE__, \
         name " size is " SIZE_T_FMT " bytes", (sz) );

#  define size_multi_printf( lvl, name, sz, max ) \
      _internal_debug_printf( lvl, __LINE__, \
         "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

#  ifdef LOG_TO_FILE
#     ifndef MFILE_C
extern struct MFILE_CADDY SEG_MGLOBAL g_log_file;
#     endif /* MFILE_C */
MERROR_RETVAL _internal_logging_init();
#     define logging_init() \
         retval = _internal_logging_init(); maug_cleanup_if_not_ok();
void logging_shutdown();
#  else
#     define logging_init()
#     define logging_shutdown()
#  endif /* LOG_TO_FILE */

#elif defined( MFILE_C )

#  ifdef LOG_TO_FILE

struct MFILE_CADDY SEG_MGLOBAL g_log_file;

void _internal_debug_printf( int lvl, size_t line, const char* fmt, ... ) {
   va_list vargs;

   if( lvl < DEBUG_THRESHOLD ) {
      return;
   }

   /* Try to start the line out with the preamble. */
   g_log_file.printf( &g_log_file, 0,
      "(%d) " __FILE__ ": " SIZE_T_FMT ": ", lvl, line );

   va_start( vargs, fmt );
   g_log_file.vprintf( &g_log_file, 0, fmt, vargs );
   va_end( vargs );

   /* Try to append a newline. */
   g_log_file.printf( &g_log_file, 0, "\0" );

}

/* === */

MERROR_RETVAL _internal_logging_init() {
   return mfile_open_write( LOG_FILE_NAME, &g_log_file );
}

/* === */

void logging_shutdown() {
   mfile_close( &g_log_file );
}

#  endif /* LOG_TO_FILE */

#endif /* !MAUG_API_LOG_H_DEFS */

