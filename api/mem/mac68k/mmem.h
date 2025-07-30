
#if !defined( MAUG_API_MEM_H_DEFS )
#define MAUG_API_MEM_H_DEFS

#  include <Memory.h>

typedef Handle MAUG_MHANDLE;

#  define maug_malloc( nmemb, sz ) NewHandle( (sz) * (nmemb) )

/**
 * \warn This does not test that reallocation was successful! Use
 *       maug_mrealloc_test() for that.
 */
#  define maug_mrealloc( handle, nmemb, sz ) \
      handle; SetHandleSize( handle, (sz) * (nmemb) )

/**
 * \brief Zero the block of memory pointed to by ptr.
 *
 * \warn This should be used on pointers, not handles!
 */
#  define maug_mzero( ptr, sz ) memset( ptr, '\0', sz )

#  define maug_mcpy( ptr_dest, ptr_src, sz ) memcpy( ptr_dest, ptr_src, sz )

#  define maug_mfree( handle ) DisposeHandle( handle ); handle = (Handle)0;

#  define maug_mlock( handle, ptr ) \
      HLock( handle ); ptr = (void*)handle;

#  define maug_munlock( handle, ptr ) \
      HUnlock( handle ); ptr = NULL;

#define maug_mrealloc_test( new_handle, handle, nmemb, sz ) \
   maug_cleanup_if_lt_overflow( (sz) * (nmemb), sz ); \
   maug_mrealloc( handle, nmemb, sz ); \
   if( GetHandleSize( handle ) != (sz) * (nmemb) ) { \
      error_printf( "error reallocating handle!" ); \
      retval = MERROR_ALLOC; \
      goto cleanup; \
   }

#elif defined( MMEM_C )

#endif /* !MAUG_API_MEM_H_DEFS */

