
#if !defined( MAUG_API_MEM_H_DEFS )
#define MAUG_API_MEM_H_DEFS

typedef void* MAUG_MHANDLE;

#  define maug_malloc( nmemb, sz ) (void*)malloc( (sz) * (nmemb) )

/**
 * \warn This does not test that reallocation was successful! Use
 *       maug_mrealloc_test() for that.
 */
#  define maug_mrealloc( handle, nmemb, sz ) \
      (void*)realloc( handle, (sz) * (nmemb) )

/**
 * \brief Zero the block of memory pointed to by ptr.
 *
 * \warn This should be used on pointers, not handles!
 */
#  define maug_mzero( ptr, sz ) memset( ptr, '\0', sz )

#  define maug_mcpy( ptr_dest, ptr_src, sz ) memcpy( ptr_dest, ptr_src, sz )

#  define maug_mfree( handle ) free( handle ); handle = NULL;

#  define maug_mlock( handle, ptr ) ptr = handle; handle = (void*)0xDEADBEEF;

#  define maug_munlock( handle, ptr ) handle = ptr; ptr = NULL;

#  define maug_strncpy( dest, src, len ) strncpy( dest, src, len )

#  define maug_strlen( str ) strlen( str )

#define maug_mrealloc_test( new_handle, handle, nmemb, sz ) \
   maug_cleanup_if_lt_overflow( (sz) * (nmemb), sz ); \
   new_handle = maug_mrealloc( handle, nmemb, sz ); \
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, new_handle ); \
   handle = new_handle;

#elif defined( MMEM_C )

#endif /* !MAUG_API_MEM_H_DEFS */

