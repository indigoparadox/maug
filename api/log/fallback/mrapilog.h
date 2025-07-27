
#if !defined( MAUG_API_LOG_H_DEFS )
#define MAUG_API_LOG_H_DEFS

void debug_printf( int level, const char* fmt, ... );
   
void error_printf( const char* fmt, ... );

#  define size_printf( lvl, name, sz ) debug_printf( lvl, name " size is " SIZE_T_FMT " bytes", (sz) );

#  define size_multi_printf( lvl, name, sz, max ) debug_printf( lvl, "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

#elif defined( UPRINTF_C )

void debug_printf( int level, const char* fmt, ... ) {
   va_list argp;

#  ifdef RETROFLAT_API_WINCE
   /* TODO: Use maug_vsnprintf() and Windows file APIs. */
#  else
   if( level >= DEBUG_THRESHOLD ) {
      va_start( argp, fmt );
      vprintf( fmt, argp );
      va_end( argp );
      printf( "\n" );
   }
#  endif /* RETROFLAT_API_WINCE */
}

void error_printf( const char* fmt, ... ) {
   va_list argp;

#  ifdef RETROFLAT_API_WINCE
   /* TODO: Use maug_vsnprintf() and Windows file APIs. */
#else
   va_start( argp, fmt );
   vprintf( fmt, argp );
   va_end( argp );
   printf( "\n" );
#  endif /* RETROFLAT_API_WINCE */
}

#if 0
/* TODO: Wire this up so something calls it in this module? */
void maug_debug_printf(
   FILE* out, uint8_t flags, const char* src_name, size_t line, int16_t lvl,
   const char* fmt, ...
) {
   va_list vargs;

   if( NULL == out ) {
#ifdef MAUG_LOG_FILE
      out = g_log_file;
#else
      out = stdout;
#endif /* MAUG_LOG_FILE */
   }

   if( lvl >= DEBUG_THRESHOLD ) {
      platform_fprintf( out, "(%d) %s : " SIZE_T_FMT ": ",
         lvl, src_name, line );
      
      va_start( vargs, fmt );
      platform_vfprintf( out, fmt, vargs );
      va_end( vargs );

      platform_fprintf( out, NEWLINE_STR );
      platform_fflush( out );
   }
}
#endif

#endif /* !MAUG_API_LOG_H_DEFS */

