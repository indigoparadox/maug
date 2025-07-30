
#if !defined( MAUG_API_MEM_H_DEFS )
#define MAUG_API_MEM_H_DEFS

typedef void* MAUG_MHANDLE;

/* TODO: These need refinement/tuning for DOS low-memory/segments! */
#  define maug_malloc( nmemb, sz ) \
      (void*)malloc( (sz) * (nmemb) ); \
      debug_printf( 1, \
            "malloc ct: " SIZE_T_FMT ", sz: " SIZE_T_FMT " (" SIZE_T_FMT ")", \
            nmemb, sz, (sz) * (nmemb) );

/**
 * \warn This does not test that reallocation was successful! Use
 *       maug_mrealloc_test() for that.
 */
#  define maug_mrealloc( handle, nmemb, sz ) \
      (void*)realloc( handle, (sz) * (nmemb) ); \
      debug_printf( 1, \
            "realloc ct: " SIZE_T_FMT ", sz: " SIZE_T_FMT " (" SIZE_T_FMT ")", \
            nmemb, sz, (sz) * (nmemb) );

#  define maug_mzero( ptr, sz ) memset( ptr, '\0', sz )

#  define maug_mcpy( ptr_dest, ptr_src, sz ) memcpy( ptr_dest, ptr_src, sz )

#  define maug_mfree( handle ) free( handle ); handle = NULL;

#  define maug_mlock( handle, ptr ) ptr = handle; handle = NULL;

#  define maug_munlock( handle, ptr ) handle = ptr; ptr = NULL;

#define maug_mrealloc_test( new_handle, handle, nmemb, sz ) \
   maug_cleanup_if_lt_overflow( (sz) * (nmemb), sz ); \
   new_handle = maug_mrealloc( handle, nmemb, sz ); \
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, new_handle ); \
   handle = new_handle;

#elif defined( MMEM_C )

#endif /* !MAUG_API_MEM_H_DEFS */

