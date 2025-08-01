
#if !defined( MAUG_API_LOG_H_DEFS )
#define MAUG_API_LOG_H_DEFS

#include <stdarg.h>

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

#ifdef MAUG_ANCIENT_C

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

#  define size_printf( lvl, name, sz ) \
      _internal_debug_printf( lvl, __FILE__, __LINE__, \
         name " size is " SIZE_T_FMT " bytes", (sz) );

#  define size_multi_printf( lvl, name, sz, max ) \
      _internal_debug_printf( lvl, __FILE__, __LINE__, \
         "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

#  ifdef LOG_TO_FILE
extern FILE* SEG_MGLOBAL g_log_file;
#  endif /* LOG_TO_FILE */

void logging_init();

void logging_shutdown();

#elif defined( MFILE_C )

#  ifdef LOG_TO_FILE
FILE* SEG_MGLOBAL g_log_file = NULL;
#  endif /* LOG_TO_FILE */

void logging_init() {
#  ifdef LOG_TO_FILE
   g_log_file = fopen( LOG_FILE_NAME, "w" );
#  endif /* LOG_FILE_NAME */
}

/* === */

void logging_shutdown() {
#  ifdef LOG_TO_FILE
   fclose( g_log_file );
#  endif /* LOG_FILE_NAME */
}

/* === */

#ifdef MAUG_ANCIENT_C

void debug_printf( int lvl, const char* fmt, ... ) {
   va_list vargs;

   va_start( vargs, fmt );

   if( NULL != LOG_STD_TARGET && (0 > lvl || lvl >= DEBUG_THRESHOLD) ) {
      fprintf( LOG_STD_TARGET, "(%d): ", lvl );
      vfprintf( LOG_STD_TARGET, fmt, vargs );
      fprintf( LOG_STD_TARGET, NEWLINE_STR );
      fflush( LOG_STD_TARGET );
   }

   va_end( vargs );
}

void error_printf( const char* fmt, ... ) {
   va_list vargs;

   va_start( vargs, fmt );
   
   if( NULL != LOG_ERR_TARGET ) {
      fprintf( LOG_ERR_TARGET, "(E): " );
      vfprintf( LOG_ERR_TARGET, fmt, vargs );
      fprintf( LOG_ERR_TARGET, NEWLINE_STR );
      fflush( LOG_ERR_TARGET );
   }

   va_end( vargs );
}

#else

void _internal_debug_printf(
   int lvl, const char* src, size_t line, const char* fmt, ...
) {
   va_list vargs;

   va_start( vargs, fmt );

   if( NULL != LOG_ERR_TARGET && (0 > lvl || lvl >= DEBUG_THRESHOLD) ) {
      fprintf( LOG_STD_TARGET, "(%d) %s: " SIZE_T_FMT ": ", lvl, src, line );
      vfprintf( LOG_STD_TARGET, fmt, vargs );
      fprintf( LOG_STD_TARGET, NEWLINE_STR );
      fflush( LOG_STD_TARGET );
   }

   va_end( vargs );
}

#endif /* !MAUG_ANCIENT_C */

#endif /* !MAUG_API_LOG_H_DEFS */

