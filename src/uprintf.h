
#ifndef UPRINTF_H
#define UPRINTF_H

#if __LONG_WIDTH__ == 32 || defined( __WATCOMC__ )
#  define UPRINTF_S32 "%ld"
#  define UPRINTF_U32 "%lu"
#elif __LONG_WIDTH__ == 64
#  define UPRINTF_S32 "%d"
#  define UPRINTF_U32 "%u"
#endif /* __LONG_WIDTH__ */

#ifndef NEWLINE_STR
#define NEWLINE_STR "\n"
#endif /* !NEWLINE_STR */

#ifndef UPRINTF_BUFFER_SZ_MAX
#  define UPRINTF_BUFFER_SZ_MAX 1024
#endif /* !UPRINTF_BUFFER_SZ_MAX */

#ifndef platform_file
#   define platform_file FILE*
#endif /* !platform_file */

#ifndef platform_fprintf
#   define platform_fprintf fprintf
#endif /* !platform_fprintf */

#ifndef platform_fopen
#   define platform_fopen fopen
#endif /* !platform_fopen */

#ifndef platform_fflush
#   define platform_fflush fflush
#endif /* !platform_fflush */

#ifndef platform_fclose
#   define platform_fclose fclose
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

#  ifdef NO_DEBUG_LINE_NUMBER
#     define LINE_NUMBER() 0
#  else
#     define LINE_NUMBER() __LINE__
#  endif

#  define internal_debug_printf( lvl, ... ) if( NULL != LOG_ERR_TARGET && lvl >= DEBUG_THRESHOLD ) { platform_fprintf( LOG_STD_TARGET, "(%d) " __FILE__ ": %d: ", lvl, LINE_NUMBER() ); platform_fprintf( LOG_STD_TARGET, __VA_ARGS__ ); platform_fprintf( LOG_STD_TARGET, NEWLINE_STR ); platform_fflush( LOG_STD_TARGET ); }

#  define internal_error_printf( ... ) if( NULL != LOG_ERR_TARGET ) { platform_fprintf( LOG_ERR_TARGET, "(E) " __FILE__ ": %d: ", LINE_NUMBER() ); platform_fprintf( LOG_ERR_TARGET, __VA_ARGS__ ); platform_fprintf( LOG_ERR_TARGET, NEWLINE_STR ); platform_fflush( LOG_ERR_TARGET ); }

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

#  define logging_init() g_log_file = platform_fopen( LOG_FILE_NAME, "w" );
#  define logging_shutdown() platform_fclose( g_log_file );

#  ifdef MAIN_C
platform_file g_log_file = NULL;
#  else /* !MAIN_C */
extern platform_file g_log_file;
#  endif /* MAIN_C */

#else

#  define logging_init()
#  define logging_shutdown()

#endif /* LOG_TO_FILE */

#define maug_bufcat( c, buf, buf_idx, buf_sz, cleanup ) \
   buf[buf_idx++] = c; \
   if( buf_idx >= buf_sz ) { goto cleanup; }

#define maug_bufpad( pad_c, pad_sz, i, buf, buf_idx, buf_sz, cleanup ) \
   i = 0; \
   while( i < pad_sz ) { \
      maug_bufcat( pad_c, buffer, buffer_idx, buffer_sz, cleanup ); \
      i++; \
   }

int maug_digits( long int num, int base );

#define maug_xtoa( num, base, rem, dest, dest_idx, digits, digits_done, pad ) \
   dest_idx += digits; \
   while( 0 != num ) { \
      /* Get the 1's place. */ \
      rem = num % base; \
      dest[--dest_idx] = (9 < rem) ? \
         /* > 10, so it's a letter. */ \
         (rem - 10) + 'a' : \
         /* < 10, so it's a number. */ \
         rem + '0'; \
      /* Move the next place value into range. */ \
      num /= base; \
      digits_done++; \
   } \
   while( digits_done < digits ) { \
      dest[--dest_idx] = '0'; \
      digits_done++; \
   }

int maug_itoa( long int num, char* dest, int dest_sz, int base );

int maug_utoa( uint32_t num, char* dest, int dest_sz, int base );

void maug_vsnprintf(
   char* buffer, int buffer_sz, const char* fmt, va_list vargs );

void maug_snprintf( char* buffer, int buffer_sz, const char* fmt, ... );

void maug_printf( const char* fmt, ... );

#ifdef UPRINTF_C

int maug_digits( long int num, int base ) {
   int digits = 0;
   int negative = 0;

   if( 0 > num ) {
      num *= -1;
      negative = 1;
   }

   while( 0 < num ) {
      num /= base;
      digits++;
   }

   if( 0 == digits ) {
      digits = 1; /* 0 */
   }

   if( negative ) {
      digits++; /* - symbol */
   }

   return digits;
}

/* === */

int maug_itoa( long int num, char* dest, int dest_sz, int base ) {
   long int rem;
   int digits;
   int digits_done = 0;
   int dest_idx = 0;
   int negative = 0;

   digits = maug_digits( num, base );
   assert( (0 == num && 1 == digits) || (0 != num && 0 < digits) );
   assert( digits < dest_sz );

   /* Handle 0 explicitly, otherwise empty string is printed for 0. */
   if( 0 == num ) {
      dest[0] = '0';
      digits_done++;
   } else if( 0 > num ) {
      num *= -1;
      negative = 1;
   }

   maug_xtoa( num, base, rem, dest, dest_idx, digits, digits_done, pad );

   if( negative ) {
      dest[dest_idx] = '-';
   }
   dest[digits] = '\0';

   return digits;
}

/* === */

/* TODO: Seems to force 16-bit in Borland... why? */
int maug_utoa( uint32_t num, char* dest, int dest_sz, int base ) {
   uint32_t rem;
   int digits;
   int digits_done = 0;
   int dest_idx = 0;

   digits = maug_digits( num, base );
   assert( (0 == num && 1 == digits) || (0 != num && 0 < digits) );
   assert( digits < dest_sz );

   /* Handle 0 explicitly, otherwise empty string is printed for 0. */
   if( 0 == num ) {
      dest[0] = '0';
      digits_done++;
   }

   maug_xtoa( num, base, rem, dest, dest_idx, digits, digits_done, pad );
   dest[digits] = '\0';

   return digits;
}

/* === */

void maug_vsnprintf(
   char* buffer, int buffer_sz, const char* fmt, va_list vargs
) {
   int i = 0, j = 0;
   char last = '\0';
   union RETROFLAT_FMT_SPEC spec;
   int pad_len = 0;
   char c;
   char pad_char = ' ';
   int buffer_idx = 0;
   int spec_is_long = 0;

   for( i = 0 ; '\0' != fmt[i] ; i++ ) {
      c = fmt[i]; /* Separate so we can play tricks below. */
 
      if( '%' == last ) {
         /* Conversion specifier encountered. */
         /* memset( &spec, '\0', sizeof( union RETROFLAT_FMT_SPEC ) ); */
         switch( fmt[i] ) {
            case 'l':
               spec_is_long = 1;
               /* Skip resetting the last char. */
               continue;

            case 's':
               spec.s = va_arg( vargs, char* );

               /* Print string. */
               j = 0;
               while( '\0' != spec.s[j] ) {
                  maug_bufcat( spec.s[j++], buffer, buffer_idx,
                     buffer_sz, cleanup );
               }
               break;

            case 'u':
               if( spec_is_long ) {
                  spec.u = va_arg( vargs, uint32_t );
               } else {
                  spec.u = va_arg( vargs, unsigned int );
               }
               
               /* Print padding. */
               pad_len -= maug_digits( spec.d, 10 );
               maug_bufpad( pad_char, pad_len, j,
                  buffer, buffer_idx, buffer_sz, cleanup );

               /* Print number. */
               buffer_idx += maug_utoa(
                  spec.d, &(buffer[buffer_idx]), buffer_sz - buffer_idx, 10 );
               break;

            case 'd':
               if( spec_is_long ) {
                  spec.d = va_arg( vargs, long int );
               } else {
                  spec.d = va_arg( vargs, int );
               }
               
               /* Print padding. */
               pad_len -= maug_digits( spec.d, 10 );
               maug_bufpad( pad_char, pad_len, j,
                  buffer, buffer_idx, buffer_sz, cleanup );

               /* Print number. */
               buffer_idx += maug_itoa(
                  spec.d, &(buffer[buffer_idx]), buffer_sz - buffer_idx, 10 );
               break;

            case 'x':
               spec.d = va_arg( vargs, int );

               /* Print padding. */
               pad_len -= maug_digits( spec.d, 16 );
               maug_bufpad( pad_char, pad_len, j,
                  buffer, buffer_idx, buffer_sz, cleanup );

               /* Print number. */
               buffer_idx += maug_utoa(
                  spec.d, &(buffer[buffer_idx]), buffer_sz - buffer_idx, 16 );
               break;

            case 'c':
               spec.c = va_arg( vargs, int );

               /* Print padding. */
               pad_len -= 1;
               maug_bufpad( pad_char, pad_len, j,
                  buffer, buffer_idx, buffer_sz, cleanup );

               /* Print char. */
               maug_bufcat( spec.c, buffer, buffer_idx,
                  buffer_sz, cleanup );
               break;

            case '%':
               /* Literal % */
               last = '\0';
               maug_bufcat( '%', buffer, buffer_idx, buffer_sz, cleanup );
               break;

            case '0':
               /* If we haven't started counting padding with a non-zero number,
                * this must be a 0-padding signifier.
                */
               if( 0 >= pad_len ) {
                  pad_char = '0';
                  c = '%';
                  break;
               }
               /* If we've already started parsing padding count digits, then
                * fall through below as a regular number.
                */
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               /* Handle multi-digit qty padding. */
               pad_len *= 10;
               pad_len += (c - '0'); /* Convert from char to int. */
               c = '%';
               break;
         }
      } else if( '%' != c ) {
         pad_char = ' '; /* Reset padding. */
         pad_len = 0; /* Reset padding. */
         spec_is_long = 0;

         /* Print non-escape characters verbatim. */
         maug_bufcat( c, buffer, buffer_idx,
            buffer_sz, cleanup );
      }

      last = c;
   }

cleanup:
   return;
}

/* === */

void maug_snprintf( char* buffer, int buffer_sz, const char* fmt, ... ) {
   va_list vargs;

   va_start( vargs, fmt );
   maug_vsnprintf( buffer, buffer_sz, fmt, vargs );
   va_end( vargs );
}

/* === */

void maug_printf( const char* fmt, ... ) {
   char buffer[UPRINTF_BUFFER_SZ_MAX];
   va_list vargs;

   va_start( vargs, fmt );
   maug_vsnprintf( buffer, UPRINTF_BUFFER_SZ_MAX, fmt, vargs );
   va_end( vargs );
}

#endif /* UPRINTF_C */

#endif /* !UPRINTF_H */

