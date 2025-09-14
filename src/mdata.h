
#ifndef MDATA_H
#define MDATA_H

/**
 * \addtogroup maug_data Data Memory API
 * \brief Vectors and tables for storing data.
 * \{
 *
 * \file mdata.h
 */

#ifndef MDATA_TRACE_LVL
#  define MDATA_TRACE_LVL 0
#endif /* !MDATA_TRACE_LVL */

#ifndef MDATA_TABLE_KEY_SZ_MAX
#  define MDATA_TABLE_KEY_SZ_MAX 8
#endif /* !MDATA_TABLE_KEY_SZ_MAX */

/**
 * \addtogroup mdata_vector
 * \{
 */

/**
 * \relates MDATA_VECTOR
 * \brief Flag for MDATA_VECTOR::flags indicating that vector may uses reference
 *        counting for locking.
 *
 * Such a mobile may be locked multiple times, but then must be unlocked an
 * equal number of times to unlock.
 */
#define MDATA_VECTOR_FLAG_REFCOUNT 0x01

#define MDATA_VECTOR_FLAG_IS_LOCKED 0x02

#ifndef MDATA_VECTOR_INIT_STEP_SZ
/**
 * \relates MDATA_VECTOR
 * \brief Default initial value for MDATA_VECTOR::ct_step.
 **/
#  define MDATA_VECTOR_INIT_STEP_SZ 10
#endif /* !MDATA_VECTOR_INIT_STEP_SZ */

/*! \} */

/**
 * \addtogroup mdata_strpool Data Memory String Pools
 * \brief Structure for storing a compact group of mutable, variable-length
 *        character strings.
 * \{
 */

#define MDATA_STRPOOL_FLAG_IS_LOCKED 0x01

#define MDATA_STRPOOL_FLAG_DEDUPE 0x02

typedef ssize_t mdata_strpool_idx_t;

/**
 * \brief A pool of immutable text strings. Deduplicates strings to save memory.
 *
 * Append strings with mdata_strpool_append(), then reference them by the
 * returned character index. Find strings with mdata_strpool_find() later
 * and store that index.
 */
struct MDATA_STRPOOL {
   uint8_t flags;
   MAUG_MHANDLE str_h;
   char* str_p;
   size_t str_sz;
   size_t str_sz_max;
};

/*! \} */ /* mdata_strpool */

/**
 * \addtogroup mdata_vector Data Memory Vectors
 * \brief Structure for storing a linear and compact array of values
 * \{
 */

/**
 * \brief A vector of uniformly-sized objects, stored contiguously.
 *
 * May be initialized with mdata_vector_alloc() before use, but
 * mdata_vector_append() will attempt to do this automatically.
 *
 * \warning Internal details may be subject to change, and should be accessed
 *          with wrapper macros and convenience functions.
 */
struct MDATA_VECTOR {
   /*! \brief Size of this struct (useful for serializing). */
   size_t sz;
   uint8_t flags;
   /*! \brief Handle for allocated items (unlocked). */
   MAUG_MHANDLE data_h;
   /*! \brief Handle for allocated items (locked). */
   uint8_t* data_bytes;
   /*! \brief Maximum number of items currently allocated for. */
   size_t ct_max;
   /*! \brief Maximum number of items actually used. */
   size_t ct;
   /*! \brief Number of items added when more space is needed. */
   size_t ct_step;
   /**
    * \brief Size, in bytes, of each item.
    * \warning Attempting to store items with a different size will fail!
    */
   size_t item_sz;
   /*! \brief Lock count, if MDATA_VECTOR_FLAG_REFCOUNT is enabled. */
   ssize_t locks;
};

/*! \} */ /* mdata_vector */

/**
 * \addtogroup mdata_table Data Memory Tables
 * \{
 */

struct MDATA_TABLE_KEY {
   char string[MDATA_TABLE_KEY_SZ_MAX + 1];
   size_t string_sz;
   uint32_t hash;
};

struct MDATA_TABLE {
   uint16_t flags;
   struct MDATA_VECTOR data_cols[2];
   size_t key_sz;
};

/*! \} */

/**
 * \addtogroup mdata_strpool
 * \{
 */

/**
 * \brief Verify if the given mdata_strpool_idx_t is valid in the given strpool.
 */
MERROR_RETVAL mdata_strpool_check_idx(
   struct MDATA_STRPOOL* sp, mdata_strpool_idx_t idx );

mdata_strpool_idx_t mdata_strpool_find(
   struct MDATA_STRPOOL* sp, const char* str, size_t str_sz );

/**
 * \brief Return a dynamically-allocated memory handle containing the contents
 *        of the string at the given index.
 */
MAUG_MHANDLE mdata_strpool_extract(
   struct MDATA_STRPOOL* sp, mdata_strpool_idx_t idx );

mdata_strpool_idx_t mdata_strpool_append(
   struct MDATA_STRPOOL* sp, const char* str, size_t str_sz, uint8_t flags );

MERROR_RETVAL mdata_strpool_remove(
   struct MDATA_STRPOOL* sp, mdata_strpool_idx_t idx );

MERROR_RETVAL mdata_strpool_alloc(
   struct MDATA_STRPOOL* sp, size_t alloc_sz );

void mdata_strpool_free( struct MDATA_STRPOOL* sp );

/*! \} */

/**
 * \addtogroup mdata_vector
 * \{
 */

/**
 * \relates MDATA_VECTOR
 * \brief Append an item to the specified vector.
 * \param v The vector to append to.
 * \param item The address of an item to copy to the newly-created vector
 *             slow, or NULL to not copy anything.
 * \param item_sz Size (in bytes) of the item to append. If the item is sized
 *                differently from the first item appended, MERROR_OVERFLOW
 *                will be returned and the item will not be appended.
 * \return Index of the appended item or MERROR_RETVAL * -1 if append fails.
 * \warning The vector must not be locked before an append or allocate!
 *          Reallocation could change pointers gotten during a lock!
 */
ssize_t mdata_vector_append(
   struct MDATA_VECTOR* v, const void* item, size_t item_sz );

ssize_t mdata_vector_insert(
   struct MDATA_VECTOR* v, const void* item, ssize_t idx, size_t item_sz );

/**
 * \relates MDATA_VECTOR
 * \brief Remove item at the given index, shifting subsequent items up by 1.
 * \warning The vector must not be locked before a removal!
 */
MERROR_RETVAL mdata_vector_remove( struct MDATA_VECTOR* v, size_t idx );

/**
 * \relates MDATA_VECTOR
 * \brief Get a generic pointer to an item in the MDATA_VECTOR.
 * \param v Vector to request the item from. Should be locked!
 * \param idx Index of item to retrieve.
 * \return Return a generic pointer to the item at the requested index, or NULL
 *         if the index is outside of the vector bounds.
 */
void* mdata_vector_get_void( const struct MDATA_VECTOR* v, size_t idx );

MERROR_RETVAL mdata_vector_copy(
   struct MDATA_VECTOR* v_dest, struct MDATA_VECTOR* v_src );

/**
 * \warning The vector must not be locked before an append or allocate!
 *          Reallocation could change pointers gotten during a lock!
 */
MERROR_RETVAL mdata_vector_alloc(
   struct MDATA_VECTOR* v, size_t item_sz, size_t item_ct_init );

void mdata_vector_free( struct MDATA_VECTOR* v );

/*! \} */ /* mdata_vector */

uint32_t mdata_hash( const char* token, size_t token_sz );

/**
 * \addtogroup mdata_table
 * \{
 */

typedef MERROR_RETVAL (*mdata_table_iter_t)(
   const struct MDATA_TABLE_KEY* key, void* data, size_t data_sz,
   void* cb_data, size_t cb_data_sz, size_t idx );

MERROR_RETVAL mdata_table_lock( struct MDATA_TABLE* t );

MERROR_RETVAL mdata_table_unlock( struct MDATA_TABLE* t );

MERROR_RETVAL mdata_table_iter(
   struct MDATA_TABLE* t,
   mdata_table_iter_t cb, void* cb_data, size_t cb_data_sz );

MERROR_RETVAL mdata_table_set(
   struct MDATA_TABLE* t, const char* key,
   void* value, size_t value_sz );

MERROR_RETVAL mdata_table_unset(
   struct MDATA_TABLE* t, const char* key );

void* mdata_table_get_void( const struct MDATA_TABLE* t, const char* key );

void* mdata_table_hash_get_void(
   struct MDATA_TABLE* t, uint32_t key_hash, size_t key_sz );

void mdata_table_free( struct MDATA_TABLE* t );

/*! \} */

#if MDATA_TRACE_LVL > 0
#  define mdata_debug_printf( fmt, ... ) \
      debug_printf( MDATA_TRACE_LVL, fmt, __VA_ARGS__ );
#else
#  define mdata_debug_printf( fmt, ... )
#endif /* MDATA_TRACE_LVL */

/**
 * \addtogroup mdata_strpool
 * \{
 */

#define mdata_strpool_sz( sp ) ((sp)->str_sz_max)

#define mdata_strpool_is_locked( sp ) \
   (MDATA_STRPOOL_FLAG_IS_LOCKED == \
   (MDATA_STRPOOL_FLAG_IS_LOCKED & (sp)->flags))

#define mdata_strpool_lock( sp ) \
   assert( NULL == (sp)->str_p ); \
   maug_mlock( (sp)->str_h, (sp)->str_p ); \
   maug_cleanup_if_null_lock( char*, (sp)->str_p ); \
   (sp)->flags |= MDATA_STRPOOL_FLAG_IS_LOCKED;

#define mdata_strpool_unlock( sp ) \
   if( NULL != (sp)->str_p ) { \
      maug_munlock( (sp)->str_h, (sp)->str_p ); \
      (sp)->flags &= ~MDATA_STRPOOL_FLAG_IS_LOCKED; \
   }

#define mdata_strpool_get( sp, idx ) \
   ((idx >= 0 && idx < (sp)->str_sz) ? &((sp)->str_p[idx]) : NULL)

#define mdata_strpool_padding( str_sz ) \
   (sizeof( size_t ) - ((str_sz + 1 /* NULL */) % sizeof( size_t )))

/*! \} */ /* mdata_strpool */

/**
 * \addtogroup mdata_vector
 * \{
 */

/**
 * \relates MDATA_VECTOR
 * \brief Lock the vector. This should be done when items from the vector are
 *        actively being referenced, so the system is not allowed to move the
 *        vector's data allocation block.
 * \warning mdata_vector_lock() should never be called *after* the cleanup
 *          label! (This is fine for mdata_vector_unlock(), however.)
 */
#define mdata_vector_lock( v ) \
   if( (MAUG_MHANDLE)NULL == (v)->data_h && NULL == (v)->data_bytes ) { \
      mdata_debug_printf( "locking empty vector..." ); \
      (v)->flags |= MDATA_VECTOR_FLAG_IS_LOCKED; \
   } else if( \
      mdata_vector_get_flag( v, MDATA_VECTOR_FLAG_REFCOUNT ) && \
      0 < (v)->locks \
   ) { \
      (v)->locks++; \
      mdata_debug_printf( "vector " #v " locks: " SSIZE_T_FMT, (v)->locks ); \
   } else { \
      /* assert( !mdata_vector_is_locked( v ) ); */ \
      if( mdata_vector_is_locked( v ) ) { \
         error_printf( "attempting to double-lock vector!" ); \
         retval = MERROR_OVERFLOW; \
         goto cleanup; \
      } \
      if( (MAUG_MHANDLE)NULL == (v)->data_h ) { \
         error_printf( "invalid data handle!" ); \
         retval = MERROR_ALLOC; \
         goto cleanup; \
      } \
      maug_mlock( (v)->data_h, (v)->data_bytes ); \
      maug_cleanup_if_null_lock( uint8_t*, (v)->data_bytes ); \
      (v)->flags |= MDATA_VECTOR_FLAG_IS_LOCKED; \
      mdata_debug_printf( "locked vector " #v ); \
   }

/**
 * \relates MDATA_VECTOR
 * \brief Unlock the vector so items may be added and removed.
 * \note mdata_vector_unlock() may be called after the cleanup label.
 */
#define mdata_vector_unlock( v ) \
   if( (MAUG_MHANDLE)NULL == (v)->data_h && NULL == (v)->data_bytes ) { \
      mdata_debug_printf( "locking empty vector..." ); \
      (v)->flags &= ~MDATA_VECTOR_FLAG_IS_LOCKED; \
   } else { \
      if( \
         mdata_vector_get_flag( v, MDATA_VECTOR_FLAG_REFCOUNT ) && \
         0 < (v)->locks \
      ) { \
         (v)->locks--; \
         mdata_debug_printf( "vector " #v " locks: " SSIZE_T_FMT, \
            (v)->locks ); \
      } \
      if( 0 == (v)->locks && NULL != (v)->data_bytes ) { \
         assert( mdata_vector_is_locked( v ) ); \
         maug_munlock( (v)->data_h, (v)->data_bytes ); \
         (v)->flags &= ~MDATA_VECTOR_FLAG_IS_LOCKED; \
         mdata_debug_printf( "unlocked vector " #v ); \
      } \
   }

#define mdata_vector_get( v, idx, type ) \
   ((type*)mdata_vector_get_void( v, idx ))

#define mdata_vector_get_last( v, type ) \
   (0 < mdata_vector_ct( v ) ? \
      ((type*)mdata_vector_get_void( v, \
         mdata_vector_ct( v ) - 1 )) : NULL)

#define mdata_vector_remove_last( v ) \
   (0 < mdata_vector_ct( v ) ? \
      (mdata_vector_remove( v, mdata_vector_ct( v ) - 1 )) : MERROR_OVERFLOW)

#define mdata_vector_set_ct_step( v, step ) \
   (v)->ct_step = step;

/**
 * \relates MDATA_VECTOR
 * \brief Number of items of MDATA_VECTOR::item_sz bytes actively stored in
 *        this vector.
 * \warning This is not neccessarily equal to amount of memory occupied by the
 *          vector, as it may have allocated more than are currently occupied!
 */
#define mdata_vector_ct( v ) ((v)->ct)

/**
 * \relates MDATA_VECTOR
 * \brief Number of bytes of heap memory occupied by this vector.
 */
#define mdata_vector_sz( v ) (((v)->ct_max) * ((v)->item_sz))

/**
 * \relates MDATA_VECTOR
 * \brief Allocate and mark the new slots as active.
 */
#define mdata_vector_fill( v, ct_new, sz ) \
   retval = mdata_vector_alloc( v, sz, ct_new ); \
   maug_cleanup_if_not_ok(); \
   (v)->ct = (ct_new);

#define mdata_vector_is_locked( v ) \
   (MDATA_VECTOR_FLAG_IS_LOCKED == \
   (MDATA_VECTOR_FLAG_IS_LOCKED & (v)->flags))

/* TODO: Implement insert sorting. */
#define mdata_vector_insert_sort( v, i, t, field )

/* TODO: Implement sorting. */
#define mdata_vector_sort( v, t, field )

#define _mdata_vector_item_ptr( v, idx ) \
   (&((v)->data_bytes[((idx) * ((v)->item_sz))]))

#define mdata_vector_set_flag( v, flag ) (v)->flags |= (flag)

#define mdata_vector_get_flag( v, flag ) ((flag) == ((v)->flags & (flag)))

/*! \} */ /* mdata_vector */

/**
 * \addtogroup mdata_table
 * \{
 */

#define mdata_table_is_locked( t ) \
   (mdata_vector_is_locked( &((t)->data_cols[0]) ))

#define mdata_table_get( t, key, type ) \
   ((type*)mdata_table_get_void( t, key ))

#define mdata_table_hash_get( t, hash, sz, type ) \
   ((type*)mdata_table_hash_get_void( t, hash, sz ))

#define mdata_table_ct( t ) ((t)->data_cols[0].ct)

/*! \} */ /* mdata_table */

#define mdata_retval( idx ) (0 > idx ? ((idx) * -1) : MERROR_OK)

#ifdef MDATA_C

MERROR_RETVAL mdata_strpool_check_idx(
   struct MDATA_STRPOOL* sp, mdata_strpool_idx_t idx
) {
   MERROR_RETVAL retval = MERROR_OVERFLOW;
   mdata_strpool_idx_t i = 0;
   int autolock = 0;

   if( !mdata_strpool_is_locked( sp ) ) {
      mdata_strpool_lock( sp );
      autolock = 1;
   }

   for( i = 0 ; sp->str_sz > i ; i += (size_t)*(&(sp->str_p[i])) ) {
      if( idx == i ) {
         retval = MERROR_OK;
         goto cleanup;
      }
   }

cleanup:

   if( autolock ) {
      mdata_strpool_unlock( sp );
   }

   return retval;
}

/* === */

void mdata_strpool_dump( struct MDATA_STRPOOL* sp ) {
   size_t i = 0;
   char* strpool_p = NULL;

   maug_mlock( sp->str_h, strpool_p );

   for( i = 0 ; mdata_strpool_sz( sp ) > i ; i++ ) {
      printf( "0x%02x ", strpool_p[i] );
   }
   printf( "\n" );

   if( NULL != strpool_p ) {
      maug_munlock( sp->str_h, strpool_p );
   }
}

/* === */

ssize_t mdata_strpool_find(
   struct MDATA_STRPOOL* strpool, const char* str, size_t str_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;
   char* strpool_p = NULL;
   size_t* p_str_iter_sz = NULL;

   if( (MAUG_MHANDLE)NULL == strpool->str_h ) {
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
#if MDATA_TRACE_LVL > 0
         debug_printf( MDATA_TRACE_LVL,
            "found strpool_idx: " SIZE_T_FMT " (" SIZE_T_FMT " bytes): \"%s\"",
            i, *p_str_iter_sz, &(strpool_p[i]) );
#endif /* MDATA_TRACE_LVL */

         goto cleanup;
#if MDATA_TRACE_LVL > 0
      } else {
         debug_printf( MDATA_TRACE_LVL,
            "skipping strpool_idx: " SIZE_T_FMT " (" SIZE_T_FMT
               " bytes): \"%s\"",
            i + sizeof( size_t ), *p_str_iter_sz,
               &(strpool_p[i + sizeof( size_t )]) );
#endif /* MDATA_TRACE_LVL */
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

/* === */

MAUG_MHANDLE mdata_strpool_extract(
   struct MDATA_STRPOOL* sp, mdata_strpool_idx_t idx
) {
   MERROR_RETVAL retval = MERROR_OK;
   MAUG_MHANDLE out_h = (MAUG_MHANDLE)NULL;
   size_t out_sz = 0;
   char* out_tmp = NULL;
   int autolock = 0;
   char* str_src = NULL;

   if( !mdata_strpool_is_locked( sp ) ) {
      mdata_strpool_lock( sp );
      autolock = 1;
   }

   str_src = mdata_strpool_get( sp, idx );
   if( NULL == str_src ) {
      error_printf( "invalid strpool index: " SSIZE_T_FMT, idx );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   out_sz = maug_strlen( str_src );
   out_h = maug_malloc( out_sz + 1, 1 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, out_h );

   maug_mlock( out_h, out_tmp );
   maug_cleanup_if_null_lock( char*, out_tmp );

   maug_mzero( out_tmp, out_sz + 1 );
   maug_strncpy( out_tmp, str_src, out_sz );

cleanup:

   if( NULL != out_tmp ) {
      maug_munlock( out_h, out_tmp );
   }

   if( MERROR_OK != retval && (MAUG_MHANDLE)NULL != out_h ) {
      maug_mfree( out_h );
   }

   if( autolock ) {
      mdata_strpool_unlock( sp );
   }

   return out_h;
}

/* === */

ssize_t mdata_strpool_append(
   struct MDATA_STRPOOL* strpool, const char* str, size_t str_sz, uint8_t flags
) {
   mdata_strpool_idx_t idx_p_out = 0;
   char* strpool_p = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_str_sz = NULL;
   size_t alloc_sz = 0;

   if( 0 == str_sz ) {
      error_printf( "attempted to add zero-length string!" );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   if(
      0 < strpool->str_sz &&
      MDATA_STRPOOL_FLAG_DEDUPE == (MDATA_STRPOOL_FLAG_DEDUPE & flags)
   ) {
      /* Search the str_stable for an identical string and return that index.
      */
      idx_p_out = mdata_strpool_find( strpool, str, str_sz );
      if( -1 != idx_p_out ) {
         /* Found, or error returned. */
#if MDATA_TRACE_LVL > 0
         debug_printf( MDATA_TRACE_LVL,
            "found duplicate string for add at index: " SSIZE_T_FMT,
            idx_p_out );
#endif /* MDATA_TRACE_LVL */
         goto cleanup;
      }
   }

   /* Pad out allocated space so size_t is always aligned. */
   alloc_sz = sizeof( size_t ) + str_sz + 1 /* NULL */ + 
      mdata_strpool_padding( str_sz );
   assert( 0 == alloc_sz % sizeof( size_t ) );

#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL,
      "adding size_t (" SIZE_T_FMT " bytes) + string %s (" SIZE_T_FMT
         " bytes) + 1 NULL + " SIZE_T_FMT " bytes padding to strpool...",
      sizeof( size_t ), str, str_sz, mdata_strpool_padding( str_sz ) );
#endif /* MDATA_TRACE_LVL */

   retval = mdata_strpool_alloc( strpool, alloc_sz );
   maug_cleanup_if_not_ok();

   maug_mlock( strpool->str_h, strpool_p );
   maug_cleanup_if_null_alloc( char*, strpool_p );

#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL,
      "strpool (" SIZE_T_FMT " bytes) locked to: %p",
      strpool->str_sz, strpool_p );
#endif /* MDATA_TRACE_LVL */

   /* Add this string at the end of the string pool. */
   maug_strncpy(
      &(strpool_p[strpool->str_sz + sizeof( size_t )]), str, str_sz );
   strpool_p[strpool->str_sz + sizeof( size_t ) + str_sz] = '\0';

   /* Add the size of the string to the strpool. */
   assert( 0 == strpool->str_sz % sizeof( size_t ) );
   p_str_sz = (size_t*)&(strpool_p[strpool->str_sz]);
   *p_str_sz = alloc_sz;

   idx_p_out = strpool->str_sz + sizeof( size_t );

#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL, "set strpool_idx: " SIZE_T_FMT ": \"%s\"",
      strpool->str_sz, &(strpool_p[idx_p_out]) );
#endif /* MDATA_TRACE_LVL */

   /* Set the string pool cursor to the next available spot. */
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
#if MDATA_TRACE_LVL > 0
      debug_printf(
         MDATA_TRACE_LVL, "creating string pool of " SIZE_T_FMT " chars...",
         alloc_sz );
#endif /* MDATA_TRACE_LVL */
      assert( (MAUG_MHANDLE)NULL == strpool->str_h );
      strpool->str_h = maug_malloc( alloc_sz, 1 );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, strpool->str_h );
      strpool->str_sz_max = alloc_sz;

   } else if( strpool->str_sz_max <= strpool->str_sz + alloc_sz ) {
      while( strpool->str_sz_max <= strpool->str_sz + alloc_sz ) {
#if MDATA_TRACE_LVL > 0
         debug_printf(
            MDATA_TRACE_LVL, "enlarging string pool to " SIZE_T_FMT "...",
            strpool->str_sz_max * 2 );
#endif /* MDATA_TRACE_LVL */
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
   struct MDATA_VECTOR* v, const void* item, size_t item_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t idx_out = -1;

   if( 0 < v->item_sz && item_sz != v->item_sz ) {
      error_printf( "attempting to add item of " SIZE_T_FMT " bytes to vector, "
         "but vector is already sized for " SIZE_T_FMT "-byte items!",
         item_sz, v->item_sz );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   mdata_vector_alloc( v, item_sz, v->ct_step );

   /* Lock the vector to work in it a bit. */
   mdata_vector_lock( v );

   idx_out = v->ct;

   if( NULL != item ) {
      /* Copy provided item. */
#if MDATA_TRACE_LVL > 0
      debug_printf(
         MDATA_TRACE_LVL, "inserting into vector at index: " SIZE_T_FMT,
         idx_out );
#endif /* MDATA_TRACE_LVL */

      memcpy( _mdata_vector_item_ptr( v, idx_out ), item, item_sz );
   }

   v->ct++;

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

ssize_t mdata_vector_insert(
   struct MDATA_VECTOR* v, const void* item, ssize_t idx, size_t item_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;

   assert( 0 <= idx );

   if( 0 < v->item_sz && item_sz != v->item_sz ) {
      error_printf( "attempting to add item of " SIZE_T_FMT " bytes to vector, "
         "but vector is already sized for " SIZE_T_FMT "-byte items!",
         item_sz, v->item_sz );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   if( idx > v->ct ) {
      error_printf( "attempting to insert beyond end of vector!" );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   mdata_vector_alloc( v, item_sz, v->ct_step );

   /* Lock the vector to work in it a bit. */
   mdata_vector_lock( v );

   for( i = v->ct ; idx < i ; i-- ) {
#if MDATA_TRACE_LVL > 0
      debug_printf( MDATA_TRACE_LVL,
         "copying vector item " SSIZE_T_FMT " to " SSIZE_T_FMT "...",
         i - 1, i );
#endif /* MDATA_TRACE_LVL */
      memcpy(
         _mdata_vector_item_ptr( v, i ),
         _mdata_vector_item_ptr( v, i - 1),
         item_sz );
   }

#if MDATA_TRACE_LVL > 0
   debug_printf(
      MDATA_TRACE_LVL, "inserting into vector at index: " SIZE_T_FMT, idx );
#endif /* MDATA_TRACE_LVL */
   if( NULL != item ) {
      /* Copy provided item. */
      memcpy( _mdata_vector_item_ptr( v, idx ), item, item_sz );
   } else {
      /* Just blank the given index. */
      maug_mzero( _mdata_vector_item_ptr( v, idx ), item_sz );
   }

   v->ct++;

cleanup:

   if( MERROR_OK != retval ) {
      error_printf( "error adding to vector: %d", retval );
      idx = retval * -1;
      assert( 0 > idx );
   }

   mdata_vector_unlock( v );

   return idx;
}

/* === */

MERROR_RETVAL mdata_vector_remove( struct MDATA_VECTOR* v, size_t idx ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   if( mdata_vector_is_locked( v ) ) {
      error_printf( "vector cannot be resized while locked!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   if( v->ct <= idx ) {
      error_printf( "index out of range!" );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL, "removing vector item: " SIZE_T_FMT, idx );
#endif /* MDATA_TRACE_LVL */

   assert( 0 < v->item_sz );

   mdata_vector_lock( v );

   for( i = idx ; v->ct > i + 1 ; i++ ) {
#if MDATA_TRACE_LVL > 0
      debug_printf( MDATA_TRACE_LVL,
         "shifting " SIZE_T_FMT "-byte vector item " SIZE_T_FMT " up by 1...",
         v->item_sz, i );
#endif /* MDATA_TRACE_LVL */
      memcpy(
         &(v->data_bytes[i * v->item_sz]),
         &(v->data_bytes[(i + 1) * v->item_sz]),
         v->item_sz );
   }

   v->ct--;

cleanup:

   mdata_vector_unlock( v );

   return retval;
}

/* === */

void* mdata_vector_get_void( const struct MDATA_VECTOR* v, size_t idx ) {

#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL,
      "getting vector item " SIZE_T_FMT " (of " SIZE_T_FMT ")...",
      idx, v->ct );
#endif /* MDATA_TRACE_LVL */

   assert( 0 == v->ct || NULL != v->data_bytes );

   if( idx >= v->ct ) {
      return NULL;
   } else {
      return _mdata_vector_item_ptr( v, idx );
   }
}

/* === */

MERROR_RETVAL mdata_vector_copy(
   struct MDATA_VECTOR* v_dest, struct MDATA_VECTOR* v_src
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL != v_src->data_bytes ) {
      error_printf( "vector cannot be copied while locked!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   assert( 0 < v_src->item_sz );
   assert( 0 < v_src->ct_max );

   v_dest->ct_max = v_src->ct_max;
   v_dest->ct = v_src->ct;
   v_dest->item_sz = v_src->item_sz;
#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL,
      "copying " SIZE_T_FMT " vector of " SIZE_T_FMT "-byte nodes...",
      v_src->ct_max, v_src->item_sz );
#endif /* MDATA_TRACE_LVL */
   assert( (MAUG_MHANDLE)NULL == v_dest->data_h );
   v_dest->data_h = maug_malloc( v_src->ct_max, v_src->item_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, v_dest->data_h );

   mdata_vector_lock( v_dest );
   mdata_vector_lock( v_src );

   memcpy( v_dest->data_bytes, v_src->data_bytes,
      v_src->ct_max * v_src->item_sz );

cleanup:

   mdata_vector_unlock( v_src );
   mdata_vector_unlock( v_dest );

   return retval;
}

/* === */

MERROR_RETVAL mdata_vector_alloc(
   struct MDATA_VECTOR* v, size_t item_sz, size_t item_ct_init
) {
   MERROR_RETVAL retval = MERROR_OK;
   MAUG_MHANDLE data_h_new = (MAUG_MHANDLE)NULL;
   size_t new_ct = item_ct_init,
      new_bytes_start = 0,
      new_bytes_sz = 0;

   if( NULL != v->data_bytes ) {
      error_printf( "vector cannot be resized while locked!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   /* Make sure there are free nodes. */
   if( (MAUG_MHANDLE)NULL == v->data_h ) {
      assert( 0 == v->ct_max );

      if( 0 < item_ct_init ) {
#if MDATA_TRACE_LVL > 0
         debug_printf( MDATA_TRACE_LVL, "setting step sz: " SIZE_T_FMT,
            item_ct_init );
#endif /* MDATA_TRACE_LVL */
         v->ct_step = item_ct_init;
      } else if( 0 == v->ct_step ) {
#if MDATA_TRACE_LVL > 0
         debug_printf( MDATA_TRACE_LVL, "setting step sz: %d",
            MDATA_VECTOR_INIT_STEP_SZ );
#endif /* MDATA_TRACE_LVL */
         v->ct_step = MDATA_VECTOR_INIT_STEP_SZ;
      }
      v->ct_max = v->ct_step;
#if MDATA_TRACE_LVL > 0
      debug_printf( MDATA_TRACE_LVL,
         "creating " SIZE_T_FMT " vector of " SIZE_T_FMT "-byte nodes...",
         v->ct_max, item_sz );
#endif /* MDATA_TRACE_LVL */
      assert( (MAUG_MHANDLE)NULL == v->data_h );
      v->data_h = maug_malloc( v->ct_max, item_sz );
      assert( 0 < item_sz );
      v->item_sz = item_sz;
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, v->data_h );

      /* Zero out the new space. */
      mdata_vector_lock( v );
      maug_mzero( v->data_bytes, v->ct_max * item_sz );
      mdata_vector_unlock( v );

      v->sz = sizeof( struct MDATA_VECTOR );

   } else if( v->ct_max <= v->ct + 1 || v->ct_max <= item_ct_init ) {
      assert( item_sz == v->item_sz );
      
      /* Use ct * 2 or ct_init... whichever is larger! */
      if( item_ct_init < v->ct_max + v->ct_step ) {
         assert( v->ct_max + v->ct_step > v->ct_max );
         new_ct = v->ct_max + v->ct_step;
      }

      /* Perform the resize. */
#if MDATA_TRACE_LVL > 0
      debug_printf( MDATA_TRACE_LVL, "enlarging vector to " SIZE_T_FMT "...",
         new_ct );
#endif /* MDATA_TRACE_LVL */
      maug_mrealloc_test( data_h_new, v->data_h, new_ct, item_sz );
      
      /* Zero out the new space. */
      new_bytes_start = v->ct_max * v->item_sz;
      assert( new_bytes_start >= v->ct_max );
      new_bytes_sz = (new_ct * v->item_sz) - new_bytes_start;
      assert( new_bytes_sz >= v->item_sz );
      mdata_vector_lock( v );
      maug_mzero( &(v->data_bytes[new_bytes_start]), new_bytes_sz );
      mdata_vector_unlock( v );

      v->ct_max = new_ct;
   }

cleanup:

   return retval;
}

/* === */

void mdata_vector_free( struct MDATA_VECTOR* v ) {
   if( 0 < v->ct_max ) {
      maug_mfree( v->data_h );
   }
   v->ct = 0;
   v->ct_max = 0;
   v->item_sz = 0;
}

/* === */

uint32_t mdata_hash( const char* token, size_t token_sz ) {
   uint32_t hash_out = 2166136261u; /* Arbitrary fixed prime. */
   size_t i = 0;
   char c = 0;

   for( i = 0 ; token_sz > i ; i++ ) {
      c = token[i];

      /* Case-insensitive. */
      if( 'A' <= c && 'Z' >= c ) {
         c += 32;
      }

      hash_out ^= c;
      hash_out *= 16777619u;
   }

   return hash_out;
}

/* === */

MERROR_RETVAL mdata_table_lock( struct MDATA_TABLE* t ) {
   MERROR_RETVAL retval = MERROR_OK;

   mdata_vector_lock( &(t->data_cols[0]) );
   mdata_vector_lock( &(t->data_cols[1]) );

cleanup:

   /*
   if( MERROR_OK != retval ) {
      assert( !mdata_vector_is_locked( &(t->data_cols[0]) ) );
   }
   */

   return retval;
}

/* === */

MERROR_RETVAL mdata_table_unlock( struct MDATA_TABLE* t ) {
   MERROR_RETVAL retval = MERROR_OK;

   mdata_vector_unlock( &(t->data_cols[0]) );
   mdata_vector_unlock( &(t->data_cols[1]) );

   if( MERROR_OK != retval ) {
      assert( mdata_vector_is_locked( &(t->data_cols[0]) ) );
   }

   return retval;
}

/* === */

struct MDATA_TABLE_REPLACE_CADDY {
   struct MDATA_TABLE_KEY* key;
   void* data;
};

/* === */

ssize_t _mdata_table_hunt_index(
   const struct MDATA_TABLE* t,
   const char* key, uint32_t key_hash, size_t key_sz
) {
   struct MDATA_TABLE_KEY* key_iter = NULL;
   ssize_t i = -1;

   if( 0 == mdata_table_ct( t ) ) {
      goto cleanup;
   }

   assert( mdata_vector_is_locked( &(t->data_cols[0]) ) );

   /* Hash the key to hunt for if provided. */
   if( NULL != key ) {
      key_sz = maug_strlen( key );
      if( MDATA_TABLE_KEY_SZ_MAX < key_sz ) {
         key_sz = MDATA_TABLE_KEY_SZ_MAX;
      }
      key_hash = mdata_hash( key, key_sz );
   }

   /* Compare the key to what we have. */
   /* TODO: Divide and conquer! */
   for( i = 0 ; mdata_vector_ct( &(t->data_cols[0]) ) > i ; i++ ) {
      key_iter = mdata_vector_get(
         &(t->data_cols[0]), i, struct MDATA_TABLE_KEY );
      assert( NULL != key );
      if(
         key_iter->hash == key_hash &&
         key_iter->string_sz == key_sz
      ) {
#if MDATA_TRACE_LVL > 0
         debug_printf( MDATA_TRACE_LVL, "found value for key: %s", key );
#endif /* MDATA_TRACE_LVL */
         return i;
      }
   }

cleanup:

   return -1;
}

/* === */

static MERROR_RETVAL _mdata_table_replace(
   const struct MDATA_TABLE_KEY* key, void* data, size_t data_sz,
   void* cb_data, size_t cb_data_sz, size_t idx
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MDATA_TABLE_REPLACE_CADDY* caddy =
      (struct MDATA_TABLE_REPLACE_CADDY*)cb_data;

   if(
      key->hash == caddy->key->hash && key->string_sz == caddy->key->string_sz
   ) {
#if MDATA_TRACE_LVL > 0
      debug_printf( MDATA_TRACE_LVL,
         "replacing table data for key %s (%u)...", key->string, key->hash );
#endif /* MDATA_TRACE_LVL */
      memcpy( data, caddy->data, data_sz );
      retval = MERROR_FILE;
   }

   return retval;
}

/* === */

MERROR_RETVAL mdata_table_iter(
   struct MDATA_TABLE* t,
   mdata_table_iter_t cb, void* cb_data, size_t cb_data_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   int autolock = 0;
   struct MDATA_TABLE_KEY* p_key = NULL;
   char* p_value = NULL;

   if( 0 == mdata_table_ct( t ) ) {
      return MERROR_OK;
   }

   if( !mdata_table_is_locked( t ) ) {
#if MDATA_TRACE_LVL > 0
      debug_printf( MDATA_TRACE_LVL, "engaging table autolock..." );
#endif /* MDATA_TRACE_LVL */
      mdata_table_lock( t );
      autolock = 1;
   }

   /* Execute the callback for every item. */
   for( i = 0 ; mdata_table_ct( t ) > i ; i++ ) {
      p_key = mdata_vector_get_void( &(t->data_cols[0]), i );
      assert( NULL != p_key );
      assert( 0 < p_key->string_sz );
      assert( p_key->string_sz == maug_strlen( p_key->string ) );
      p_value = mdata_vector_get_void( &(t->data_cols[1]), i );
      retval = cb(
         p_key, p_value, t->data_cols[1].item_sz, cb_data, cb_data_sz, i );
      maug_cleanup_if_not_ok();
   }

cleanup:

   if( autolock ) {
      mdata_table_unlock( t );
   }

   return retval;
}

/* === */

MERROR_RETVAL mdata_table_set(
   struct MDATA_TABLE* t, const char* key,
   void* value, size_t value_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t idx_key = -1;
   ssize_t idx_val = -1;
   struct MDATA_TABLE_KEY key_tmp;
   struct MDATA_TABLE_REPLACE_CADDY caddy;

   assert( 0 < maug_strlen( key ) );

   assert( !mdata_table_is_locked( t ) );

   assert(
      mdata_vector_ct( &(t->data_cols[0]) ) ==
      mdata_vector_ct( &(t->data_cols[1]) ) );

   /* Get key hash and properties. */
   maug_mzero( &key_tmp, sizeof( struct MDATA_TABLE_KEY ) );
   maug_strncpy( key_tmp.string, key, MDATA_TABLE_KEY_SZ_MAX );
   if( maug_strlen( key ) > MDATA_TABLE_KEY_SZ_MAX + 1 ) {
      error_printf(
         "key %s is longer than maximum key size! truncating to: %s",
         key, key_tmp.string );
   }
   key_tmp.string_sz = strlen( key_tmp.string );
   key_tmp.hash = mdata_hash( key_tmp.string, key_tmp.string_sz );

#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL,
      "attempting to set key %s (%u) to " SIZE_T_FMT "-byte value...",
      key_tmp.string, key_tmp.hash, value_sz );
#endif /* MDATA_TRACE_LVL */

   caddy.key = &key_tmp;
   caddy.data = value;

   /* Search for the hash. */
   /* TODO: Use quicker search. */
   retval = mdata_table_iter( t, _mdata_table_replace, &caddy,
      sizeof( struct MDATA_TABLE_REPLACE_CADDY ) );
   if( MERROR_FILE == retval ) {
      /* _mdata_table_replace returned that it replaced an item, so quit. */
      retval = MERROR_OK;
      goto cleanup;
   }

   /* TODO: Insert in hash order. */

   idx_key = mdata_vector_append(
      &(t->data_cols[0]), &key_tmp, sizeof( struct MDATA_TABLE_KEY ) );
   assert( 0 <= idx_key );

   /* TODO: Atomicity: remove key if value fails! */

   idx_val = mdata_vector_append( &(t->data_cols[1]), value, value_sz );
   assert( 0 <= idx_val );

cleanup:

   /* TODO: Set retval! */

   return retval;
}

/* === */

MERROR_RETVAL mdata_table_unset(
   struct MDATA_TABLE* t, const char* key
) {
   MERROR_RETVAL retval = MERROR_OK;
   int autolock = 0;
   ssize_t idx = 0;

#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL, "unsetting table key: %s", key );
#endif /* MDATA_TRACE_LVL */

   /* Autolock is fine to have for unset, as there is no returned pointer to
    * preserve.
    */
   if( !mdata_table_is_locked( t ) ) {
#if MDATA_TRACE_LVL > 0
      debug_printf( MDATA_TRACE_LVL, "autolocking table vectors" );
#endif /* MDATA_TRACE_LVL */
      assert( !mdata_vector_is_locked( &(t->data_cols[0]) ) );
      assert( !mdata_vector_is_locked( &(t->data_cols[1]) ) );
      mdata_table_lock( t );
      autolock = 1;
   }

   idx = _mdata_table_hunt_index( t, key, 0, 0 );
   if( 0 > idx ) {
      goto cleanup;
   }

   /* Remove the item. */
   mdata_table_unlock( t );
   mdata_vector_remove( &(t->data_cols[0]), idx );
   mdata_vector_remove( &(t->data_cols[1]), idx );

cleanup:

   if( autolock && mdata_table_is_locked( t ) ) {
      mdata_table_unlock( t );
   } else if( !autolock && !mdata_table_is_locked( t ) ) {
      mdata_table_lock( t );
   }
   
   return retval;
}

/* === */

void* mdata_table_get_void( const struct MDATA_TABLE* t, const char* key ) {
   MERROR_RETVAL retval = MERROR_OK;
   void* value_out = NULL;
   ssize_t idx = 0;

   assert( mdata_table_is_locked( t ) );

#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL,
      "searching for key: %s (%u)", key, key_hash );
#endif /* MDATA_TRACE_LVL */

   idx = _mdata_table_hunt_index( t, key, 0, 0 );
   if( 0 > idx ) {
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   value_out = mdata_vector_get_void( &(t->data_cols[1]), idx );

cleanup:
   
   if( MERROR_OK != retval ) {
      value_out = NULL;
   }

   return value_out;
}

/* === */

void* mdata_table_hash_get_void(
   struct MDATA_TABLE* t, uint32_t key_hash, size_t key_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   void* value_out = NULL;
   ssize_t idx = 0;

   assert( mdata_table_is_locked( t ) );

#if MDATA_TRACE_LVL > 0
   debug_printf( MDATA_TRACE_LVL,
      "searching for hash %u (" SIZE_T_FMT ")", key_hash, key_sz );
#endif /* MDATA_TRACE_LVL */

   idx = _mdata_table_hunt_index( t, NULL, key_hash, key_sz );
   if( 0 > idx ) {
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   value_out = mdata_vector_get_void( &(t->data_cols[1]), idx );

cleanup:
   
   if( MERROR_OK != retval ) {
      value_out = NULL;
   }

   return value_out;
}

/* === */

void mdata_table_free( struct MDATA_TABLE* t ) {
   mdata_vector_free( &(t->data_cols[0]) );
   mdata_vector_free( &(t->data_cols[1]) );
   maug_mzero( t, sizeof( struct MDATA_TABLE ) );
}

#endif /* MDATA_C */

/*! \} */ /* maug_data */

#endif /* MDATA_H */

