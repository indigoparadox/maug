
#if !defined( MAUG_API_MEM_H_DEFS )
#define MAUG_API_MEM_H_DEFS

/**
 * \addtogroup mmem Maug Memory API
 * \{
 * \file mmem.h
 */

#  if !defined( DOCUMENTATION ) && !defined( MMEM_DEFS_CT_MAX )
#     define MMEM_DEFS_CT_MAX 100
#  endif /* !MMEM_DEFS_CT_MAX */

typedef void* MAUG_MHANDLE;

struct MMEM_DEF {
   void* handle;
   int sz;
};

#  if !defined( DOCUMENTATION ) && MMEM_TRACE_LVL > 0
void debug_mmem_realloc( void* handle, void* new_handle, int sz );
void debug_mmem_alloc( void* handle, int sz );
void debug_mmem_free( void* handle );
#  else
#     define debug_mmem_realloc( handle, new_handle, sz )
#     define debug_mmem_alloc( handle, sz )
#     define debug_mmem_free( handle )
#  endif /* MMEM_TRACE_LVL */

#  define maug_malloc_test( handle, nmemb, sz ) \
      maug_cleanup_if_lt_overflow( (sz) * (nmemb), sz ); \
      handle = (void*)malloc( (sz) * (nmemb) ); \
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, handle ); \
      debug_mmem_alloc( handle, sz );

/**
 * \brief Zero the block of memory pointed to by ptr.
 *
 * \warn This should be used on pointers, not handles!
 */
#  define maug_mzero( ptr, sz ) memset( ptr, '\0', sz );

#  define maug_mcpy( ptr_dest, ptr_src, sz ) memcpy( ptr_dest, ptr_src, sz )

#  define maug_mfree( handle ) \
      debug_mmem_free( handle ); \
      free( handle ); \
      handle = NULL;

#  define maug_mlock( handle, ptr ) ptr = handle; handle = (void*)0xDEADBEEF;

#  define maug_munlock( handle, ptr ) handle = ptr; ptr = NULL;

/**
 * \brief Safely reallocate a handle to a new size, testing for common failures.
 * \param new_handle A "helper" handle to hold the new address for testing.
 * \param handle The handle to realloc and assign the new address to.
 * \param nmemb The number of members to allocate.
 * \param sz The size (in bytes) of each member to allocate (multiplied by
 *           nmemb).
 */
#  define maug_mrealloc_test( new_handle, handle, nmemb, sz ) \
      maug_cleanup_if_lt_overflow( (sz) * (nmemb), sz ); \
      new_handle = realloc( handle, (nmemb) * (sz) ); \
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, new_handle ); \
      debug_mmem_realloc( handle, new_handle, nmemb * sz ); \
      handle = new_handle;

#  if MMEM_TRACE_LVL > 0
extern struct MMEM_DEF g_mmem_defs[MMEM_DEFS_CT_MAX];
extern int g_mmem_defs_ct;
#  endif /* MMEM_TRACE_LVL */

#elif defined( MMEM_C )

#  if MMEM_TRACE_LVL > 0
struct MMEM_DEF g_mmem_defs[MMEM_DEFS_CT_MAX];
int g_mmem_defs_ct = 0;
void* g_mmem_alloc_ptr = NULL;

void debug_mmem_print_total( void ) {
   size_t i = 0;
   int sz_total = 0;
   char prefix = ' ';

#  ifdef MMEM_DUMP_ENABLED
   printf( "---\n" );
#  endif /* MMEM_DUMP_ENABLED */
   for( i = 0 ; g_mmem_defs_ct > i ; i++ ) {
#  ifdef MMEM_DUMP_ENABLED
      printf(
         SIZE_T_FMT ". %p (%d bytes)\n",
         i, g_mmem_defs[i].handle, g_mmem_defs[i].sz );
#  endif /* MMEM_DUMP_ENABLED */
      sz_total += g_mmem_defs[i].sz;
   }
#  ifdef MMEM_DUMP_ENABLED
   printf( "---\n" );
#  endif /* MMEM_DUMP_ENABLED */

   if( 1024 < sz_total ) {
      prefix = 'k';
      sz_total /= 1024;
   }
   if( 1024 < sz_total ) {
      prefix = 'm';
      sz_total /= 1024;
   }
   if( 1024 < sz_total ) {
      prefix = 'g';
      sz_total /= 1024;
   }

   printf( "%d %cbytes in %d allocs\n", sz_total, prefix, g_mmem_defs_ct );
}

/* === */

void debug_mmem_free( void* handle ) {
   size_t i = 0 ;

   for( i = 0 ; g_mmem_defs_ct > i ; i++ ) {
      if( handle == g_mmem_defs[i].handle ) {
         memmove(
            &(g_mmem_defs[i]),
            &(g_mmem_defs[i + 1]),
            (g_mmem_defs_ct - (i + 1)) * sizeof( struct MMEM_DEF ) );
         g_mmem_defs_ct--;
         printf( "alloc %p (" SIZE_T_FMT ") freed, ", handle, i );
         debug_mmem_print_total();
         return;
      }
   }
   printf( "attempted to free invalid alloc %p!\n", handle );
}

/* === */

void debug_mmem_alloc( void* handle, int sz ) {
   g_mmem_defs[g_mmem_defs_ct].handle = handle;
   g_mmem_defs[g_mmem_defs_ct].sz = sz;
   if( g_mmem_defs_ct + 1 < MMEM_DEFS_CT_MAX ) {
      g_mmem_defs_ct++;
   }
   printf(
      "handle %p (%d) alloced as sz: %d, ",
      handle, g_mmem_defs_ct - 1, sz );
   debug_mmem_print_total();
}

/* === */

void debug_mmem_realloc( void* handle, void* new_handle, int sz ) {
   size_t i = 0 ;

   for( i = 0 ; g_mmem_defs_ct > i ; i++ ) {
      if( handle == g_mmem_defs[i].handle ) {
         g_mmem_defs[i].handle = new_handle;
         printf(
            "alloc %p (" SIZE_T_FMT
               ") changed to %p, size changed from %d to %d, ",
            handle, i, new_handle, g_mmem_defs[i].sz, sz );
         debug_mmem_print_total();
         g_mmem_defs[i].sz = sz;
         return;
      }
   }

   printf( "attempted to realloc invalid alloc %p!\n", handle );
}

#  endif /* MMEM_TRACE_LVL */

/*! \} */ /* mmem */

#endif /* !MAUG_API_MEM_H_DEFS */

