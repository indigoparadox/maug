
#ifndef RETAPIM_H
#define RETAPIM_H

#define MAUG_MEM_OVERRIDE

typedef void* MAUG_MHANDLE;

/* TODO: These need refinement/tuning for DOS low-memory/segments! */
#  define maug_malloc( nmemb, sz ) (void*)malloc( (sz) * (nmemb) )

/**
 * \warn This does not test that reallocation was successful! Use
 *       maug_mrealloc_test() for that.
 */
#  define maug_mrealloc( handle, nmemb, sz ) \
      (void*)realloc( handle, (sz) * (nmemb) )

#  define maug_mzero( ptr, sz ) memset( ptr, '\0', sz )

#  define maug_mcpy( ptr_dest, ptr_src, sz ) memcpy( ptr_dest, ptr_src, sz )

#  define maug_mfree( handle ) free( handle ); handle = NULL;

#  define maug_mlock( handle, ptr ) ptr = handle; handle = NULL;

#  define maug_munlock( handle, ptr ) handle = ptr; ptr = NULL;

#  define maug_strncpy( dest, src, len ) strncpy( dest, src, len )

#  define maug_strlen( str ) strlen( str )

#  define maug_is_locked( handle, ptr ) \
      (NULL == (handle) && NULL != (ptr))


#ifdef MAUG_C


#endif /* MAUG_C */

#endif /* !RETAPIM_H */

