
#ifndef MERROR_H
#define MERROR_H

/**
 * \addtogroup maug_error Error Handling API
 * \{
 * \file merror.h
 */

/**
 * \addtogroup maug_error_retvals Return Values
 * \{
 */

/**
 * \brief Return type indicating function returns a value from this list.
 */
typedef int MERROR_RETVAL;

#define MERROR_OK       0x00

#define MERROR_MARGE    0x01

#define MERROR_ALLOC    0x02

#define MERROR_OVERFLOW 0x04

#define MERROR_FILE     0x08

#define MERROR_USR      0x0f

/*! \} */ /* maug_error_retvals */

#define maug_cleanup_if_null( type, ptr, err ) \
   if( (type)NULL == ptr ) { \
      error_printf( "failed to allocate " #ptr "!" ); \
      retval = err; \
      goto cleanup; \
   }

#define maug_cleanup_if_null_alloc( type, ptr ) \
   maug_cleanup_if_null( type, ptr, MERROR_ALLOC )

#define maug_cleanup_if_not_ok() \
   if( MERROR_OK != retval ) { \
      goto cleanup; \
   }

#define maug_cleanup_if_lt( a, b, fmt, err ) \
   if( (a) < (b) ) { \
      error_printf( fmt " is less than " fmt "!", a, b ); \
      retval = err; \
      goto cleanup; \
   }

#define maug_cleanup_if_lt_overflow( a, b ) \
   maug_cleanup_if_lt( a, b, SIZE_T_FMT, MERROR_OVERFLOW )

#define maug_cleanup_if_ge( a, b, fmt, err ) \
   if( (a) >= (b) ) { \
      error_printf( fmt " is greater or equal to " fmt "!", a, b ); \
      retval = err; \
      goto cleanup; \
   }

#define maug_cleanup_if_ge_overflow( a, b ) \
   maug_cleanup_if_ge( a, (size_t)(b), SIZE_T_FMT, MERROR_OVERFLOW )


/*! \} */ /* maug_error */

#endif /* MERROR_H */

