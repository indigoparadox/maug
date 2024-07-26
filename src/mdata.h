
#ifndef MDATA_H
#define MDATA_H

#ifndef MDATA_TRACE_LVL
#  define MDATA_TRACE_LVL 0
#endif /* !MDATA_TRACE_LVL */

#ifndef MDATA_VECTOR_INIT_SZ
#  define MDATA_VECTOR_INIT_SZ 10
#endif /* !MDATA_TRACE_LVL */

typedef ssize_t mdata_strpool_idx_t;

struct MDATA_STRPOOL {
   MAUG_MHANDLE str_h;
   size_t str_sz;
   size_t str_sz_max;
};

struct MDATA_VECTOR {
   MAUG_MHANDLE data_h;
   uint8_t* data_bytes;
   size_t ct_max;
   size_t ct;
   size_t item_sz;
};

ssize_t mdata_strpool_find(
   struct MDATA_STRPOOL* strpool, const char* str, size_t str_sz );

ssize_t mdata_strpool_append(
   struct MDATA_STRPOOL* strpool, const char* str, size_t str_sz );

MERROR_RETVAL mdata_strpool_alloc(
   struct MDATA_STRPOOL* strpool, size_t alloc_sz );

void mdata_strpool_free( struct MDATA_STRPOOL* strpool );

/**
 * \warning The vector must not be locked before an append or allocate!
 *          Reallocation could change pointers gotten during a lock!
 */
ssize_t mdata_vector_append(
   struct MDATA_VECTOR* v, void* item, size_t item_sz );

MERROR_RETVAL mdata_vector_remove( struct MDATA_VECTOR* v, size_t idx );

void* mdata_vector_get_void( struct MDATA_VECTOR* v, size_t idx );

/**
 * \warning The vector must not be locked before an append or allocate!
 *          Reallocation could change pointers gotten during a lock!
 */
MERROR_RETVAL mdata_vector_alloc(
   struct MDATA_VECTOR* v, size_t item_sz, size_t item_ct_init );

void mdata_vector_free( struct MDATA_VECTOR* v );

#define mdata_strpool_lock( strpool, ptr ) \
   maug_mlock( (strpool)->str_h, ptr ); \
   maug_cleanup_if_null_lock( char*, ptr );

#define mdata_strpool_unlock( strpool, ptr ) \
   if( NULL != ptr ) { \
      maug_munlock( (strpool)->str_h, ptr ); \
   }

#define mdata_vector_lock( v ) \
   maug_mlock( (v)->data_h, (v)->data_bytes ); \
   maug_cleanup_if_null_lock( uint8_t*, (v)->data_bytes );

#define mdata_vector_unlock( v ) \
   if( NULL != (v)->data_bytes ) { \
      maug_munlock( (v)->data_h, (v)->data_bytes ); \
   }

#define mdata_vector_get( v, idx, type ) \
   ((type*)mdata_vector_get_void( v, idx ))

#define mdata_vector_ct( v ) ((v)->ct)

#define _mdata_vector_item_ptr( v, idx ) \
   (&((v)->data_bytes[((idx) * ((v)->item_sz))]))

#define mdata_retval( idx ) ((idx) * -1)

#ifdef MDATA_C

#include <string.h> /* strncpy() */

ssize_t mdata_strpool_find(
   struct MDATA_STRPOOL* strpool, const char* str, size_t str_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;
   char* strpool_p = NULL;
   size_t* p_str_iter_sz = NULL;

   if( NULL == strpool->str_h ) {
      error_printf( "strpool not allocated!" );
      i = -1;
      goto cleanup;
   }

   maug_mlock( strpool->str_h, strpool_p );

   while( i < strpool->str_sz ) {
      p_str_iter_sz = (size_t*)&(strpool_p[i]);
      if(
         0 == strncmp( &(strpool_p[i + sizeof( size_t )]), str, str_sz + 1 )
      ) {
         /* String found. Advance past the size before returning. */
         i += sizeof( size_t );
         debug_printf( MDATA_TRACE_LVL,
            "found strpool_idx: " SIZE_T_FMT " (" SIZE_T_FMT " bytes): \"%s\"",
            i, *p_str_iter_sz, &(strpool_p[i]) );

         goto cleanup;
      } else {
         debug_printf( MDATA_TRACE_LVL,
            "skipping strpool_idx: " SIZE_T_FMT " (" SIZE_T_FMT
               " bytes): \"%s\"",
            i + sizeof( size_t ), *p_str_iter_sz,
               &(strpool_p[i + sizeof( size_t )]) );
      }
      i += *p_str_iter_sz;
   }

   /* String not found. */
   i = -1;

cleanup:

   if( MERROR_OK != retval ) {
      i = retval * -1;
   }

   if( NULL != strpool_p ) {
      maug_munlock( strpool->str_h, strpool_p );
   }

   return i;
}

ssize_t mdata_strpool_append(
   struct MDATA_STRPOOL* strpool, const char* str, size_t str_sz
) {
   ssize_t idx_p_out = 0;
   char* strpool_p = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_str_sz = NULL;
   size_t padding = 0;
   size_t alloc_sz = 0;

   if( 0 < strpool->str_sz ) {
      /* Search the str_stable for an identical string and return that index.
      */
      idx_p_out = mdata_strpool_find( strpool, str, str_sz );
      if( -1 != idx_p_out ) {
         /* Found, or error returned. */
         goto cleanup;
      }
   }

   assert( 0 < str_sz );

   /* Pad out allocated space so size_t is always aligned. */
   padding = sizeof( size_t ) - ((str_sz + 1 /* NULL */) % sizeof( size_t ));
   alloc_sz = sizeof( size_t ) + str_sz + 1 /* NULL */ + padding;
   assert( 0 == alloc_sz % 8 );

   debug_printf( MDATA_TRACE_LVL,
      "adding size_t (" SIZE_T_FMT " bytes) + string %s (" SIZE_T_FMT
         " bytes) + 1 NULL + " SIZE_T_FMT " bytes padding to strpool...",
      sizeof( size_t ), str, str_sz, padding );

   retval = mdata_strpool_alloc( strpool, alloc_sz );
   maug_cleanup_if_not_ok();

   maug_mlock( strpool->str_h, strpool_p );
   maug_cleanup_if_null_alloc( char*, strpool_p );

   debug_printf( MDATA_TRACE_LVL,
      "strpool (" SIZE_T_FMT " bytes) locked to: %p",
      strpool->str_sz, strpool_p );

   /* Add this string at the end of the string table. */
   strncpy( &(strpool_p[strpool->str_sz + sizeof( size_t )]), str, str_sz );
   strpool_p[strpool->str_sz + sizeof( size_t ) + str_sz] = '\0';

   /* Add the size of the string to the strpool. */
   assert( 0 == strpool->str_sz % 8 );
   p_str_sz = (size_t*)&(strpool_p[strpool->str_sz]);
   *p_str_sz = alloc_sz;

   idx_p_out = strpool->str_sz + sizeof( size_t );

   debug_printf(
      MDATA_TRACE_LVL, "set strpool_idx: " SIZE_T_FMT ": \"%s\"",
      strpool->str_sz, &(strpool_p[idx_p_out]) );

   /* Set the string table cursor to the next available spot. */
   strpool->str_sz += alloc_sz;

cleanup:

   if( MERROR_OK != retval ) {
      idx_p_out = retval * -1;
   }

   if( NULL != strpool_p ) {
      maug_munlock( strpool->str_h, strpool_p );
   }

   return idx_p_out;
}

/* === */

MERROR_RETVAL mdata_strpool_alloc(
   struct MDATA_STRPOOL* strpool, size_t alloc_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   MAUG_MHANDLE str_h_new = (MAUG_MHANDLE)NULL;

   if( (MAUG_MHANDLE)NULL == strpool->str_h ) {
      debug_printf(
         MDATA_TRACE_LVL, "creating string table of " SIZE_T_FMT " chars...",
         alloc_sz );
      strpool->str_h = maug_malloc( alloc_sz, 1 );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, strpool->str_h );
      strpool->str_sz_max = alloc_sz;

   } else if( strpool->str_sz_max <= strpool->str_sz + alloc_sz ) {
      while( strpool->str_sz_max <= strpool->str_sz + alloc_sz ) {
         debug_printf(
            MDATA_TRACE_LVL, "enlarging string table to " SIZE_T_FMT "...",
            strpool->str_sz_max * 2 );
         maug_mrealloc_test(
            str_h_new, strpool->str_h, strpool->str_sz_max, (size_t)2 );
         strpool->str_sz_max *= 2;
      }
   }

cleanup:
   return retval;
}

/* === */

void mdata_strpool_free( struct MDATA_STRPOOL* strpool ) {
   if( (MAUG_MHANDLE)NULL != strpool->str_h ) {
      maug_mfree( strpool->str_h );
   }
}

/* === */

ssize_t mdata_vector_append(
   struct MDATA_VECTOR* v, void* item, size_t item_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t idx_out = -1;

   mdata_vector_alloc( v, item_sz, MDATA_VECTOR_INIT_SZ );

   /* Lock the vector to work in it a bit. */
   mdata_vector_lock( v );

   if( NULL != item ) {
      idx_out = v->ct;

      debug_printf(
         MDATA_TRACE_LVL, "inserting into vector at index: " SIZE_T_FMT,
         idx_out );

      memcpy( _mdata_vector_item_ptr( v, idx_out ), item, item_sz );

      v->ct++;
   } else {
      idx_out = 0;
   }

cleanup:

   if( MERROR_OK != retval ) {
      error_printf( "error adding to vector: %d", retval );
      idx_out = retval * -1;
      assert( 0 > idx_out );
   }

   mdata_vector_unlock( v );

   return idx_out;
}

/* === */

MERROR_RETVAL mdata_vector_remove( struct MDATA_VECTOR* v, size_t idx ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   if( NULL != v->data_bytes ) {
      error_printf( "vector cannot be resized while locked!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   if( v->ct <= idx ) {
      error_printf( "index out of range!" );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   debug_printf( MDATA_TRACE_LVL, "removing vector item: " SIZE_T_FMT, idx );

   assert( 0 < v->item_sz );

   mdata_vector_unlock( v );

   for( i = idx ; v->ct > i + 1 ; i++ ) {
      debug_printf( MDATA_TRACE_LVL,
         "shifting vector item " SIZE_T_FMT " up by 1...", i );
      memcpy( &(v->data_bytes[i]), &(v->data_bytes[i + 1]), v->item_sz );
   }

   v->ct--;

cleanup:

   mdata_vector_unlock( v );

   return retval;
}

/* === */

void* mdata_vector_get_void( struct MDATA_VECTOR* v, size_t idx ) {

   debug_printf( MDATA_TRACE_LVL,
      "getting vector item " SIZE_T_FMT " (of " SIZE_T_FMT ")...",
      idx, v->ct );

   assert( NULL != v->data_bytes );
   assert( idx < v->ct );

   if( idx >= v->ct ) {
      return NULL;
   } else {
      return _mdata_vector_item_ptr( v, idx );
   }
}

/* === */

MERROR_RETVAL mdata_vector_alloc(
   struct MDATA_VECTOR* v, size_t item_sz, size_t item_ct_init
) {
   MERROR_RETVAL retval = MERROR_OK;
   MAUG_MHANDLE data_h_new = NULL;

   if( NULL != v->data_bytes ) {
      error_printf( "vector cannot be resized while locked!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   /* Make sure there are free nodes. */
   if( (MAUG_MHANDLE)NULL == v->data_h ) {
      assert( 0 == v->ct_max );

      v->ct_max = item_ct_init;
      debug_printf(
         MDATA_TRACE_LVL,
         "creating " SIZE_T_FMT " vector of " SIZE_T_FMT "-byte nodes...",
         v->ct_max, item_sz );
      v->data_h = maug_malloc( v->ct_max, item_sz );
      v->item_sz = item_sz;
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, v->data_h );

   } else if( v->ct_max <= v->ct + 1 ) {
      assert( item_sz == v->item_sz );
      debug_printf(
         MDATA_TRACE_LVL, "enlarging vector to " SIZE_T_FMT "...",
         v->ct_max * 2 );
      maug_mrealloc_test( data_h_new, v->data_h, v->ct_max * 2, item_sz );
      v->ct_max *= 2;
   }

cleanup:

   return retval;
}

/* === */

void mdata_vector_free( struct MDATA_VECTOR* v ) {
   if( (MAUG_MHANDLE)NULL != v->data_h ) {
      maug_mfree( v->data_h );
   }
}

#endif /* MDATA_C */

#endif /* MDATA_H */

