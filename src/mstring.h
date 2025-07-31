
#ifndef MSTRING_H
#define MSTRING_H

#include <mtypes.h>

/**
 * \addtogroup maug_mstring Micro-String API
 * \brief Useful string and number formatting utilities.
 * \{
 */

/**
 * \file mstring.h
 */

union MAUG_FMT_SPEC {
   long int d;
   uint32_t u;
   char c;
   void* p;
   char* s;
   size_t z;
};

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

int maug_zdigits( size_t num, int base );

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

/**
 * \brief Convert a single char hex digit to the int it represents.
 */
#define maug_hctoi( c ) \
   ('9' > (c) ? (c) - '0' : 'a' > (c) ? 10 + (c) - 'A' : 10 + (c) - 'a')

int maug_is_num( const char* str, size_t str_sz, uint8_t base, uint8_t sign );

int maug_is_float( const char* str, size_t str_sz );

int maug_itoa( long int num, char* dest, int dest_sz, int base );

int maug_utoa( uint32_t num, char* dest, int dest_sz, int base );

int maug_ztoa( size_t num, char* dest, int dest_sz, int base );

/**
 * This function does not support base detection e.g. by 0x prefix or h suffix.
 * 
 * \warning This function will do its best to pull a valid number, and should
 *          be used with that in mind. It should not be used to gather numbers
 *          that will be used to size buffers, for example!
 */
uint32_t maug_atou32( const char* buffer, size_t buffer_sz, uint8_t base );

/**
 * This function does not support base detection e.g. by 0x prefix or h suffix.
 *
 * \warning This function will do its best to pull a valid number, and should
 *          be used with that in mind. It should not be used to gather numbers
 *          that will be used to size buffers, for example!
 */
int32_t maug_atos32( const char* buffer, size_t buffer_sz );

float maug_atof( const char* buffer, size_t buffer_sz );

void maug_str_upper( char* line, size_t line_sz );

void maug_str_lower( char* line, size_t line_sz );

/**
 * \brief Split a string into substring tokens by index.
 * \param idx Index of the string separated by sep.
 * \param line String of text to split by separators in sep.
 * \param line_sz Length of line, or 0 to use strlen().
 * \param out Pointer to buffer to write token specified by idx to.
 * \param sep Null-terminated string of separators to split by.
 * \return MERROR_OK if token was found or MERROR_OVERFLOW if EOL was found.
 */
MERROR_RETVAL maug_tok_str(
   size_t idx, const char* line, size_t line_sz, char* out, size_t out_sz,
   char* sep );

MERROR_RETVAL maug_tok_int(
   size_t idx, const char* line, size_t line_sz, int16_t* out, char* sep );

void maug_vsnprintf(
   char* buffer, int buffer_sz, const char* fmt, va_list vargs );

void maug_snprintf( char* buffer, int buffer_sz, const char* fmt, ... );

/**
 * \brief Convert a C string to a pascal string.
 */
MERROR_RETVAL maug_str_c2p(
   const char* str_in, char* str_out, size_t str_out_sz );

/* TODO: void maug_strtou32( const char* str, */

#ifdef MAUG_NO_STDLIB

#  define maug_strncpy( dest, src, len ) maug_snprintf( dest, len, "%s", src )

char* maug_strchr( const char* str, char c );

char* maug_strrchr( const char* str, char c );

size_t maug_strlen( const char* str );

#else

#  define maug_strncpy( dest, src, len ) strncpy( dest, src, len )

#  define maug_strchr( str, c ) strchr( str, c )

#  define maug_strrchr( str, c ) strrchr( str, c )

#  define maug_strlen( str ) strlen( str )

#endif /* MAUG_NO_STDLIB */

/*! \} */ /* maug_mstring */

#ifdef MSTRING_C

int maug_is_num( const char* str, size_t str_sz, uint8_t base, uint8_t sign ) {
   size_t i = 0;

   if( 0 == str_sz ) {
      str_sz = maug_strlen( str );
   }

   for( i = 0 ; str_sz > i ; i++ ) {
      /* TODO: Base detection. */
      if( sign && 0 == i && '-' == str[i] ) {
         continue;
      } else if( '0' > str[i] || '9' < str[i] ) {
         return 0;
      }
   }

   return 1;
}

/* === */

int maug_is_float( const char* str, size_t str_sz ) {
   size_t i = 0;

   if( 0 == str_sz ) {
      str_sz = maug_strlen( str );
   }

   for( i = 0 ; str_sz > i ; i++ ) {
      if( ('0' > str[i] || '9' < str[i]) && '.' != str[i] ) {
         return 0;
      }
   }

   return 1;
}

/* === */

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

int maug_zdigits( size_t num, int base ) {
   int digits = 0;
   int negative = 0;

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

int maug_ztoa( size_t num, char* dest, int dest_sz, int base ) {
   size_t rem;
   int digits;
   int digits_done = 0;
   int dest_idx = 0;

   digits = maug_zdigits( num, base );
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

uint32_t maug_atou32( const char* buffer, size_t buffer_sz, uint8_t base ) {
   size_t i = 0;
   uint32_t u32_out = 0;

   if( 0 == buffer_sz ) {
      buffer_sz = maug_strlen( buffer );
   }

   for( i = 0 ; buffer_sz > i ; i++ ) {
      if( base > 10 && 'a' <= buffer[i] ) {
         u32_out *= base;
         u32_out += (buffer[i] - 'a') + 10;
      } else if( base > 10 && 'A' <= buffer[i] ) {
         u32_out *= base;
         u32_out += (buffer[i] - 'A') + 10;
      } else if( '0' <= buffer[i] ) {
         u32_out *= base;
         u32_out += (buffer[i] - '0');
      } else {
         break;
      }
   }

   return u32_out;
}

/* === */

int32_t maug_atos32( const char* buffer, size_t buffer_sz ) {
   size_t i = 0;
   int32_t s32_out = 0;
   uint32_t is_neg = 0;

   if( 0 == buffer_sz ) {
      buffer_sz = maug_strlen( buffer );
   }

   for( i = 0 ; buffer_sz > i ; i++ ) {
      if( '-' == buffer[i] && 0 == i ) {
         is_neg = 1;
      } else if( '0' <= buffer[i] && '9' >= buffer[i] ) {
         s32_out *= 10;
         s32_out += (buffer[i] - '0');
      } else {
         break;
      }
   }

   if( is_neg ) {
      s32_out *= -1;
   }

   return s32_out;
}

/* === */

float maug_atof( const char* buffer, size_t buffer_sz ) {
   size_t i = 0;
   float float_out = 0;
   float dec_sz = -1;

   if( 0 == buffer_sz ) {
      buffer_sz = maug_strlen( buffer );
   }

   for( i = 0 ; buffer_sz > i ; i++ ) {
      if( '0' <= buffer[i] && '9' >= buffer[i] ) {
         if( 0 > dec_sz ) {
            /* Tack the numbers on before the floating point. */
            float_out *= 10;
            float_out += (buffer[i] - '0');
         } else {
            /* Tack the numbers on after the floating point. */
            float_out += ((buffer[i] - '0') * dec_sz);
            dec_sz *= 0.1;
         }
      } else if( '.' == buffer[i] ) {
         /* Start parsing post-point numbers. */
         dec_sz = 0.1;
      } else {
         break;
      }
   }

   return float_out;
}

/* === */

void maug_str_upper( char* line, size_t line_sz ) {
   size_t i = 0;

   for( i = 0 ; line_sz > i ; i++ ) {
      if( 0x61 <= line[i] && 0x7a >= line[i] ) {
         line[i] -= 0x20;
      }
   }
}

/* === */

void maug_str_lower( char* line, size_t line_sz ) {
   size_t i = 0;

   for( i = 0 ; line_sz > i ; i++ ) {
      if( 0x41 <= line[i] && 0x5a >= line[i] ) {
         line[i] += 0x20;
      }
   }
}

/* === */

MERROR_RETVAL maug_tok_str(
   size_t idx, const char* line, size_t line_sz, char* out, size_t out_sz,
   char* sep
) {
   MERROR_RETVAL retval = MERROR_OVERFLOW;
   size_t idx_iter = 0;
   size_t i_out = 0;
   size_t i_in = 0;
   size_t i_tok = 0;
   uint8_t sep_match = 0;

   if( 0 == line_sz ) {
      line_sz = maug_strlen( line );
   }

   for( i_in = 0 ; line_sz >= i_in ; i_in++ ) {
      /* Try to match the separator. */
      sep_match = 0;
      if( '\0' == line[i_in] ) {
         sep_match = 1;
      } else {
         for( i_tok = 0 ; maug_strlen( sep ) > i_tok ; i_tok++ ) {
            if( sep[i_tok] == line[i_in] ) {
               sep_match = 1;
               break;
            }
         }
      }

      if( sep_match ) {
         /* TODO: Filter out multi-whitespace. */
         idx_iter++;
         if( idx_iter > idx ) {
            /* All chars copied! */
            retval = MERROR_OK;
            out[i_out++] = '\0';
            assert( i_out < out_sz );
            goto cleanup;
         }
         continue;
      } else if( idx_iter == idx ) {
         if( i_out + 1 >= out_sz ) {
            /*
            error_printf(
               "token " SIZE_T_FMT " would exceed buffer sz: " SIZE_T_FMT,
               idx, out_sz );
            */
            goto cleanup;
         }
         /* Copy current char over. */
         out[i_out++] = line[i_in];
      }
   }

   /* error_printf( "token " SIZE_T_FMT " not available in line!", idx ); */

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL maug_tok_int(
   size_t idx, const char* line, size_t line_sz, int16_t* out, char* sep
) {
   MERROR_RETVAL retval = MERROR_OK;
   char out_str[S16_DIGITS + 1];

   maug_mzero( out_str, S16_DIGITS + 1 );

   retval = maug_tok_str(
      idx, line, line_sz, out_str, S16_DIGITS + 1, sep );
   maug_cleanup_if_not_ok();
   
   *out = maug_atos32( out_str, S16_DIGITS );

cleanup:

   return retval;
}

/* === */

/* TODO: Error checking. */
void maug_vsnprintf(
   char* buffer, int buffer_sz, const char* fmt, va_list vargs
) {
   int i = 0, j = 0;
   char last = '\0';
   union MAUG_FMT_SPEC spec;
   int pad_len = 0;
   char c;
   char pad_char = ' ';
   int buffer_idx = 0;
   int spec_is_long = 0;

   for( i = 0 ; '\0' != fmt[i] ; i++ ) {
      c = fmt[i]; /* Separate so we can play tricks below. */
 
      if( '%' == last ) {
         /* Conversion specifier encountered. */
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

            case 'z':
               spec.z = va_arg( vargs, size_t );
               
               /* Print padding. */
               pad_len -= maug_zdigits( spec.z, 10 );
               maug_bufpad( pad_char, pad_len, j,
                  buffer, buffer_idx, buffer_sz, cleanup );

               /* Print number. */
               buffer_idx += maug_ztoa(
                  spec.z, &(buffer[buffer_idx]), buffer_sz - buffer_idx, 10 );
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

   /* Add a terminating null if it'll fit! */
   maug_bufcat( '\0', buffer, buffer_idx,
      buffer_sz, cleanup );

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

MERROR_RETVAL maug_str_c2p(
   const char* str_in, char* str_out, size_t str_out_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t str_sz = 0;
   char* str_out_buf = &(str_out[1]);
   int8_t* p_str_out_buf_sz = (int8_t*)&(str_out[0]);

   str_sz = maug_strlen( str_in );

   if( str_sz >= str_out_sz - 1 || 127 < str_sz ) {
      /* error_printf( "input string too long!" ); */
      retval = MERROR_OVERFLOW;
   }

   *p_str_out_buf_sz = str_sz;

   /* -1 for the size at the beginning. */
   maug_strncpy( str_out_buf, str_in, str_out_sz - 1 );

   return retval;
}

#ifdef MAUG_NO_STDLIB

/* === */

char* maug_strchr( const char* str, char c ) {
   size_t str_sz = 0,
      i = 0;

   str_sz = maug_strlen( str );

   for( i = 0 ; str_sz > i ; i++ ) {
      if( str[i] == c ) {
         return (char*)&(str[i]);
      }
   }

   return NULL;
}

/* === */

char* maug_strrchr( const char* str, char c ) {
   ssize_t str_sz = 0,
      i = 0;

   str_sz = maug_strlen( str );

   for( i = str_sz ; 0 <= i ; i-- ) {
      if( str[i] == c ) {
         return (char*)&(str[i]);
      }
   }

   return NULL;
}

/* === */

size_t maug_strlen( const char* str ) {
   size_t i = 0;

   while( '\0' != str[i] ) {
      i++;
   }

   return i;
}

#endif /* MAUG_NO_STDLIB */

#endif /* MSTRING_C */

#endif /* !MSTRING_H */

