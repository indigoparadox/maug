
#ifndef MMEM_H
#define MMEM_H

/**
 * \addtogroup maug_mem Memory Management API
 * \{
 * \file mmem.h
 */

#if defined( MAUG_OS_PALM )

/* TODO */

#else

#  include <stdlib.h> /* For *alloc() */

typedef void* MAUG_MHANDLE;

#define maug_malloc( nmemb, sz ) malloc( sz * nmemb )

/**
 * \warn This does not test that reallocation was successful! Use
 *       maug_mrealloc_test() for that.
 */
#define maug_mrealloc( handle, nmemb, sz ) realloc( handle, (sz) * (nmemb) )

#define maug_mzero( ptr, sz ) memset( ptr, '\0', sz )

#define maug_mfree( handle ) free( handle ); handle = NULL;

#define maug_mlock( handle, ptr ) ptr = handle; handle = NULL;

#define maug_munlock( handle, ptr ) handle = ptr; ptr = NULL;

#endif

#define maug_mrealloc_test( new_handle, handle, nmemb, sz ) \
   maug_cleanup_if_lt_overflow( (sz) * (nmemb), sz ); \
   new_handle = maug_mrealloc( handle, nmemb, sz ); \
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, new_handle ); \
   handle = new_handle;

/*! \} */ /* maug_mem */

#endif /* !MMEM_H */

