
#if !defined( MAUG_API_MEM_H_DEFS )
#define MAUG_API_MEM_H_DEFS

typedef HGLOBAL MAUG_MHANDLE;

#  define maug_malloc( nmemb, sz ) GlobalAlloc( GMEM_MOVEABLE, (sz) * (nmemb) )

/**
 * \warn This does not test that reallocation was successful! Use
 *       maug_mrealloc_test() for that.
 */
#  define maug_mrealloc( handle, nmemb, sz ) \
      GlobalReAlloc( handle, (sz) * (nmemb), GMEM_MOVEABLE )

/**
 * \brief Zero the block of memory pointed to by ptr.
 *
 * \warn This should be used on pointers, not handles!
 */
void maug_mzero( void* ptr, size_t sz );

#  define maug_mcpy( ptr_dest, ptr_src, sz ) memcpy( ptr_dest, ptr_src, sz )

#  define maug_mfree( handle ) \
      GlobalFree( handle ); handle = (MAUG_MHANDLE)NULL;

#  define maug_mlock( handle, ptr ) ptr = (void*)GlobalLock( handle );

#  define maug_munlock( handle, ptr ) GlobalUnlock( handle ); ptr = NULL;

#  define maug_mrealloc_test( new_handle, handle, nmemb, sz ) \
      maug_cleanup_if_lt_overflow( (sz) * (nmemb), sz ); \
      new_handle = maug_mrealloc( handle, nmemb, sz ); \
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, new_handle ); \
      handle = new_handle;

#elif defined( MMEM_C )

void maug_mzero( void* ptr, size_t sz ) {
   size_t i = 0;
   for( i = 0 ; sz > i ; i++ ) {
      ((uint8_t*)ptr)[i] = 0;
   }
}

#endif /* !MAUG_API_MEM_H_DEFS */

