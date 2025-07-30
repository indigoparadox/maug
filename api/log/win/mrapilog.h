
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

void _internal_debug_printf(
   HANDLE target, int lvl, size_t line, const char* fmt, ... );

#  define debug_printf( lvl, ... ) \
      _internal_debug_printf( g_log_file, lvl, __LINE__, __VA_ARGS__ )

#  define error_printf( ... ) \
      _internal_debug_printf( g_log_file, -1, __LINE__, __VA_ARGS__ )

#  define size_printf( lvl, name, sz ) \
      _internal_debug_printf( g_log_file, lvl, __LINE__, \
         name " size is " SIZE_T_FMT " bytes", (sz) );

#  define size_multi_printf( lvl, name, sz, max ) \
      _internal_debug_printf( g_log_file, lvl, __LINE__, \
         "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

#  ifdef LOG_TO_FILE
#     ifndef UPRINTF_C
extern HANDLE SEG_MGLOBAL g_log_file;
#     endif /* UPRINTF_C */
MERROR_RETVAL _internal_logging_init();
#     define logging_init() \
         retval = _internal_logging_init(); maug_cleanup_if_not_ok();
void logging_shutdown();
#  else
#     define logging_init()
#     define logging_shutdown()
#  endif /* LOG_TO_FILE */

#elif defined( UPRINTF_C )

#  ifdef LOG_TO_FILE

HANDLE SEG_MGLOBAL g_log_file = (HANDLE)NULL;

void _internal_debug_printf(
   HANDLE target, int lvl, size_t line, const char* fmt, ...
) {
   char lvl_str[2];
   char buffer[MAUG_LOG_WIN_BUF_SZ_MAX + 1];
   size_t buf_sz = 0;
   size_t buf_sz_out = 0;

   maug_mzero( buffer, MAUG_LOG_WIN_BUF_SZ_MAX + 1 );

   assert( 9 > lvl );
   lvl_str[0] = '0' + lvl;

   if( NULL == target || lvl < DEBUG_THRESHOLD ) {
      return;
   }

   /* Try to start the line out with the preamble. */
   va_start( vargs, fmt );
   maug_snprintf(
      buffer, "(%s) " __FILE__ ": " SIZE_T_FMT ": ", lvl_str, line );

   /* Try to append the formatted message. */
   buf_sz = maug_strlen( buffer );
   if( buf_sz >= MAUG_LOG_WIN_BUF_SZ_MAX ) {
      /* Buffer overflow! */
      return;
   }
   maug_vsnprintf(
      &(buffer[buf_sz]), MAUG_LOG_WIN_BUF_SZ_MAX - buf_sz, fmt, vargs );

   /* Try to append a newline. */
   buf_sz = maug_strlen( buffer );
   if( buf_sz + 1 >= MAUG_LOG_WIN_BUF_SZ_MAX ) {
      /* Buffer overflow! */
      return;
   }
   buffer[buf_sz] = '\n';
   buf_sz++;

   WriteFile( target, buffer, buf_sz, &buf_sz_out, NULL );
}

/* === */

MERROR_RETVAL _internal_logging_init() {
   MERROR_RETVAL retval = MERROR_OK;
   g_log_file = CreateFile(
      LOG_FILE_NAME, GENERIC_READ, FILE_SHARE_READ, NULL,
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
   if( INVALID_HANDLE_VALUE == g_log_file ) {
      retval = MERROR_FILE;
   }
   return retval;
}

/* === */

void logging_shutdown() {
   if( INVALID_HANDLE_VALUE != g_log_file ) {
      CloseHandle( g_log_file );
   }
}

#  endif /* LOG_TO_FILE */

#endif /* !MAUG_API_LOG_H_DEFS */

