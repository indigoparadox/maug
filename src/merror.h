
#ifndef MERROR_H
#define MERROR_H

typedef int MERROR_RETVAL;

#define MERROR_OK 0

#define MERROR_MARGE 1

#define MERROR_ALLOC 2

#define MERROR_OVERFLOW 4

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

#endif /* MERROR_H */

