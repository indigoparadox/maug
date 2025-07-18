
#ifndef MMEM_H
#define MMEM_H

/**
 * \addtogroup maug_mem Memory Management API
 * \{
 * \file mmem.h
 */

#if defined( MAUG_OS_PALM )

/* TODO */
#  pragma message( "warning: not implemented!" )

#elif defined( MAUG_OS_DOS_REAL )

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

#  define maug_strncpy( dest, src, len ) strncpy( dest, src, len )

#  define maug_strlen( str ) strlen( str )

#  define maug_is_locked( handle, ptr ) \
      (NULL == (handle) && NULL != (ptr))

#else

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

#  define maug_mlock( handle, ptr ) ptr = handle; handle = NULL;

#  define maug_munlock( handle, ptr ) handle = ptr; ptr = NULL;

#  define maug_strncpy( dest, src, len ) strncpy( dest, src, len )

#  define maug_strlen( str ) strlen( str )

#  define maug_is_locked( handle, ptr ) \
      (NULL == (handle) && NULL != (ptr))

#endif

#define maug_mrealloc_test( new_handle, handle, nmemb, sz ) \
   maug_cleanup_if_lt_overflow( (sz) * (nmemb), sz ); \
   new_handle = maug_mrealloc( handle, nmemb, sz ); \
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, new_handle ); \
   handle = new_handle;

char* maug_strchr( const char* str, char c );

char* maug_strrchr( const char* str, char c );

/*! \} */ /* maug_mem */

#ifdef MMEM_C

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

#endif /* MMEM_C */

#endif /* !MMEM_H */

