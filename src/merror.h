
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

#define MERROR_OK       0x0000

/**
 * \brief Application-specific error.
 *
 * If this error is passed, an additional code may use the 0xFF00 bits to
 * provide more information.
 */
#define MERROR_USR      0x0001

#define MERROR_OVERFLOW 0x0002

#define MERROR_FILE     0x0004

#define MERROR_ALLOC    0x0008

#define MERROR_PARSE    0x0010

#define MERROR_GUI      0x0020

#define MERROR_SND      0x0020

#define MERROR_WAIT     0x0040

#define MERROR_EXEC     0x0040

/* MERROR_EXEC + MERROR_TIMEOUT */
#define MERROR_PREEMPT  0x00c0

#define MERROR_TIMEOUT  0x0080

/*! \} */ /* maug_error_retvals */

#define merror_sz_to_retval( sz ) ((sz) * -1)

#define merror_retval_to_sz( retval ) ((retval) * -1)

#define maug_cleanup_if_null_msg( type, ptr, err, msg ) \
   if( (type)NULL == ptr ) { \
      error_printf( msg ); \
      retval = err; \
      goto cleanup; \
   }

#define maug_cleanup_if_not_null( type, ptr, err ) \
   if( (type)NULL == ptr ) { \
      error_printf( #ptr " is not NULL!" ); \
      retval = err; \
      goto cleanup; \
   }

#define maug_cleanup_if_null( type, ptr, err ) \
   maug_cleanup_if_null_msg( type, ptr, err, "failed to allocate " #ptr "!" )

#define maug_cleanup_if_null_alloc( type, ptr ) \
   maug_cleanup_if_null_msg( type, ptr, MERROR_ALLOC, "failed to allocate " #ptr "!" )

#define maug_cleanup_if_null_lock( type, ptr ) \
   maug_cleanup_if_null_msg( type, ptr, MERROR_ALLOC, "failed to lock " #ptr "!" )

#define maug_cleanup_if_null_file( ptr ) \
   maug_cleanup_if_null_msg( FILE*, ptr, MERROR_FILE, "failed to open FILE!" )

#define maug_cleanup_if_not_ok() \
   if( MERROR_OK != retval ) { \
      goto cleanup; \
   }

#define maug_cleanup_if_not_ok_msg( msg ) \
   if( MERROR_OK != retval ) { \
      error_printf( msg ); \
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

#define maug_cleanup_if_eq( a, b, fmt, err ) \
   if( (a) == (b) ) { \
      error_printf( #a " " fmt " is equal to " #b " " fmt "!", a, b ); \
      retval = err; \
      goto cleanup; \
   }

#define maug_cleanup_if_ne( a, b, fmt, err ) \
   if( (a) != (b) ) { \
      error_printf( #a " " fmt " is NOT equal to " #b " " fmt "!", a, b ); \
      retval = err; \
      goto cleanup; \
   }

/*! \} */ /* maug_error */

#endif /* MERROR_H */

