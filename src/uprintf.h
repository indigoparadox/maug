
#ifndef UPRINTF_H
#define UPRINTF_H

#ifndef NEWLINE_STR
#define NEWLINE_STR "\n"
#endif /* !NEWLINE_STR */

#ifndef platform_file
#define platform_file FILE*
#endif /* !platform_file */

#ifndef platform_fprintf
#define platform_fprintf fprintf
#endif /* !platform_fprintf */

#ifndef platform_fopen
#define platform_fopen fopen
#endif /* !platform_fopen */

#ifndef platform_fflush
#define platform_fflush fflush
#endif /* !platform_fflush */

#ifndef platform_fclose
#define platform_fclose fclose
#endif /* !platform_fclose */

#ifdef LOG_TO_FILE
#  ifndef DEBUG_LOG
#     define DEBUG_LOG
#  endif /* !DEBUG_LOG */
#  define LOG_ERR_TARGET g_log_file
#  define LOG_STD_TARGET g_log_file
#else
#  define LOG_ERR_TARGET stderr
#  define LOG_STD_TARGET stdout
#endif /* LOG_TO_FILE */

#ifdef __GNUC__
#  define SIZE_T_FMT "%lu"
#else
#  define SIZE_T_FMT "%u"
#endif /* __GNUC__ */

#if defined( DEBUG_LOG ) && !defined( DEBUG_THRESHOLD )
#  define DEBUG_THRESHOLD 1
#endif /* DEBUG_LOG && !DEBUG_THRESHOLD */

/* ! */
#if defined( ANCIENT_C )
/* ! */

#include <stdarg.h>

static void debug_printf( int level, const char* fmt, ... ) {
   va_list argp;

   if( level >= DEBUG_THRESHOLD ) {
      va_start( argp, fmt );
      vprintf( fmt, argp );
      va_end( argp );
      printf( "\n" );
   }
}

static void error_printf( const char* fmt, ... ) {
   va_list argp;

   va_start( argp, fmt );
   vprintf( fmt, argp );
   va_end( argp );
   printf( "\n" );
}

#  define size_printf( lvl, name, sz ) debug_printf( lvl, name " size is " SIZE_T_FMT " bytes", (sz) );

#  define size_multi_printf( lvl, name, sz, max ) debug_printf( lvl, "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

/* ! */
#elif defined( DEBUG_LOG )
/* ! */

#  include <stdio.h>

#  define internal_debug_printf( lvl, ... ) if( NULL != LOG_ERR_TARGET && lvl >= DEBUG_THRESHOLD ) { platform_fprintf( LOG_STD_TARGET, "(%d) " __FILE__ ": %d: ", lvl, __LINE__ ); platform_fprintf( LOG_STD_TARGET, __VA_ARGS__ ); platform_fprintf( LOG_STD_TARGET, NEWLINE_STR ); platform_fflush( LOG_STD_TARGET ); }

#  define internal_error_printf( ... ) if( NULL != LOG_ERR_TARGET ) { platform_fprintf( LOG_ERR_TARGET, "(E) " __FILE__ ": %d: ", __LINE__ ); platform_fprintf( LOG_ERR_TARGET, __VA_ARGS__ ); platform_fprintf( LOG_ERR_TARGET, NEWLINE_STR ); platform_fflush( LOG_ERR_TARGET ); }

#  define debug_printf( lvl, ... ) internal_debug_printf( lvl, __VA_ARGS__ )

#  define error_printf( ... ) internal_error_printf( __VA_ARGS__ )

#  define size_printf( lvl, name, sz ) internal_debug_printf( lvl, name " size is " SIZE_T_FMT " bytes", (sz) );

#  define size_multi_printf( lvl, name, sz, max ) internal_debug_printf( lvl, "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

/* ! */
#else /* !DEBUG_LOG, !ANCIENT_C */
/* ! */

#  define debug_printf( ... )
#  define error_printf( ... )
#  define size_printf( ... )
#  define size_multi_printf( ... )

/* ! */
#endif /* DEBUG_LOG, ANCIENT_C */
/* ! */

#ifdef LOG_TO_FILE

#define logging_init() g_log_file = platform_fopen( LOG_FILE_NAME, "w" );
#define logging_shutdown() platform_fclose( g_log_file );

#ifdef MAIN_C
platform_file g_log_file = NULL;
#else /* !MAIN_C */
extern platform_file g_log_file;
#endif /* MAIN_C */

#else

#define logging_init()
#define logging_shutdown()

#endif /* LOG_TO_FILE */

#endif /* !UPRINTF_H */

