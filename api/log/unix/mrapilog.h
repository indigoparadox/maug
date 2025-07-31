
#if !defined( MAUG_API_LOG_H_DEFS )
#define MAUG_API_LOG_H_DEFS

#  ifdef NO_DEBUG_LINE_NUMBER
#     define LINE_NUMBER() 0
#  else
#     define LINE_NUMBER() __LINE__
#  endif

#ifndef UPRINTF_BUFFER_SZ_MAX
#  define UPRINTF_BUFFER_SZ_MAX 1024
#endif /* !UPRINTF_BUFFER_SZ_MAX */

#ifdef LOG_TO_FILE
#  ifndef UPRINTF_LOG
#     define UPRINTF_LOG
#  endif /* !UPRINTF_LOG */
#  define LOG_ERR_TARGET g_log_file
#  define LOG_STD_TARGET g_log_file
#else
#  define LOG_ERR_TARGET stderr
#  define LOG_STD_TARGET stdout
#endif /* LOG_TO_FILE */

#if !defined( DEBUG_THRESHOLD )
#  define DEBUG_THRESHOLD 1
#endif /* !DEBUG_THRESHOLD */

#  define internal_debug_printf( lvl, ... ) if( NULL != LOG_ERR_TARGET && lvl >= DEBUG_THRESHOLD ) { fprintf( LOG_STD_TARGET, "(%d) " __FILE__ ": %d: ", lvl, LINE_NUMBER() ); fprintf( LOG_STD_TARGET, __VA_ARGS__ ); fprintf( LOG_STD_TARGET, NEWLINE_STR ); fflush( LOG_STD_TARGET ); }

#  define internal_error_printf( ... ) if( NULL != LOG_ERR_TARGET ) { fprintf( LOG_ERR_TARGET, "(E) " __FILE__ ": %d: ", LINE_NUMBER() ); fprintf( LOG_ERR_TARGET, __VA_ARGS__ ); fprintf( LOG_ERR_TARGET, NEWLINE_STR ); fflush( LOG_ERR_TARGET ); }

#  define debug_printf( lvl, ... ) internal_debug_printf( lvl, __VA_ARGS__ )

#  define error_printf( ... ) internal_error_printf( __VA_ARGS__ )

#  define size_printf( lvl, name, sz ) internal_debug_printf( lvl, name " size is " SIZE_T_FMT " bytes", (sz) );

#  define size_multi_printf( lvl, name, sz, max ) internal_debug_printf( lvl, "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

#  ifdef LOG_TO_FILE
#     ifndef MFILE_C
extern FILE* SEG_MGLOBAL g_log_file;
#     endif /* MFILE_C */
#     define logging_init() g_log_file = fopen( LOG_FILE_NAME, "w" );
#     define logging_shutdown() fclose( g_log_file );
#  else
#     define logging_init()
#     define logging_shutdown()
#  endif /* LOG_TO_FILE */

#elif defined( MFILE_C )

#  ifdef LOG_TO_FILE
FILE* SEG_MGLOBAL g_log_file = NULL;
#  endif /* LOG_TO_FILE */

#endif /* !MAUG_API_LOG_H_DEFS */

