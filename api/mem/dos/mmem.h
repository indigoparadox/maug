
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

#  define maug_is_locked( handle, ptr ) \
      (NULL == (handle) && NULL != (ptr))

#else

/*! \} */ /* maug_mem */

#endif /* !MMEM_H */

