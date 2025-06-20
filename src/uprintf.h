
#ifndef UPRINTF_H
#define UPRINTF_H

#ifndef MAUG_NO_LEGACY
#  include <mlegacy.h>
#endif /* !MAUG_NO_LEGACY */

/**
 * \addtogroup maug_uprintf Micro-PrintF API
 * \brief Independently-implemented printf API for old compilers.
 * \{
 */

/**
 * \file uprintf.h
 */

/* == Autodetection == */

/* == Manual Config == */

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

MERROR_RETVAL maug_str_c2p(
   const char* str_in, char* str_out, size_t str_out_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t str_sz = 0;
   char* str_out_buf = &(str_out[1]);
   int8_t* p_str_out_buf_sz = (int8_t*)&(str_out[0]);

   str_sz = strlen( str_in );

   if( str_sz >= str_out_sz - 1 || 127 < str_sz ) {
      error_printf( "input string too long!" );
      retval = MERROR_OVERFLOW;
   }

   *p_str_out_buf_sz = str_sz;

   /* -1 for the size at the beginning. */
   strncpy( str_out_buf, str_in, str_out_sz - 1 );

   return retval;
}

#include <mrapilog.h>

#ifdef UPRINTF_C

#include <mrapilog.h>

#endif /* UPRINTF_C */

/*! \} */ /* maug_uprintf */

#endif /* !UPRINTF_H */

