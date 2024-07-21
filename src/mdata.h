
#ifndef MDATA_H
#define MDATA_H

#ifndef MDATA_TRACE_LVL
#  define MDATA_TRACE_LVL 0
#endif /* !MDATA_TRACE_LVL */

struct MDATA_STRTABLE {
   MAUG_MHANDLE str_h;
   size_t str_sz;
   size_t str_sz_max;
};

ssize_t mdata_strtable_find(
   struct MDATA_STRTABLE* str_table, const char* str, size_t str_sz );

ssize_t mdata_strtable_append(
   struct MDATA_STRTABLE* str_table, const char* str, size_t str_sz );

void mdata_strtable_free( struct MDATA_STRTABLE* str_table );

#define mdata_strtable_lock( str_table, ptr ) \
   maug_mlock( (str_table)->str_h, ptr ); \
   maug_cleanup_if_null_lock( char*, ptr );

#define mdata_strtable_unlock( str_table, ptr ) \
   if( NULL != ptr ) { \
      maug_munlock( (str_table)->str_h, ptr ); \
   }

#ifdef MDATA_C

#include <string.h> /* strncpy() */

ssize_t mdata_strtable_find(
   struct MDATA_STRTABLE* str_table, const char* str, size_t str_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;
   char* str_table_p = NULL;

   if( NULL == str_table->str_h ) {
      error_printf( "str_table not allocated!" );
      i = -1;
      goto cleanup;
   }

   maug_mlock( str_table->str_h, str_table_p );

   for( i = 0 ; str_table->str_sz > i ; i++ ) {
      if( 0 == strncmp( &(str_table_p[i]), str, str_sz + 1 ) ) {
         /* String found. */
         debug_printf(
            MDATA_TRACE_LVL, "found str_table_idx: " SIZE_T_FMT ": \"%s\"",
            i, &(str_table_p[i]) );
         goto cleanup;
      }
   }

   /* String not found. */
   i = -1;

cleanup:

   if( MERROR_OK != retval ) {
      i = retval * -1;
   }

   if( NULL != str_table_p ) {
      maug_munlock( str_table->str_h, str_table_p );
   }

   return i;
}

ssize_t mdata_strtable_append(
   struct MDATA_STRTABLE* str_table, const char* str, size_t str_sz
) {
   ssize_t idx_p_out = 0;
   MAUG_MHANDLE str_h_new = (MAUG_MHANDLE)NULL;
   char* str_table_p = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   /* Search the str_stable for an identical string and return that index.
    */
   idx_p_out = mdata_strtable_find( str_table, str, str_sz );
   if( -1 != idx_p_out ) {
      /* Found, or error returned. */
      goto cleanup;
   }

   if( (MAUG_MHANDLE)NULL == str_table->str_h ) {
      debug_printf(
         MDATA_TRACE_LVL, "creating string table of " SIZE_T_FMT " chars...",
         str_sz + 1 );
      str_table->str_h = maug_malloc( str_sz + 1, 1 );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, str_table->str_h );
      str_table->str_sz_max = str_sz + 1;

   } else {
      while( str_table->str_sz_max <= str_table->str_sz + str_sz + 1 ) {
         debug_printf(
            MDATA_TRACE_LVL, "enlarging string table to " SIZE_T_FMT "...",
            str_table->str_sz_max * 2 );
         maug_mrealloc_test(
            str_h_new, str_table->str_h, str_table->str_sz_max, (size_t)2 );
         str_table->str_sz_max *= 2;
      }
   }

   maug_mlock( str_table->str_h, str_table_p );
   maug_cleanup_if_null_alloc( char*, str_table_p );

   /* Add this string at the end of the string table. */
   strncpy( &(str_table_p[str_table->str_sz]), str, str_sz );
   str_table_p[str_table->str_sz + str_sz] = '\0';

   idx_p_out = str_table->str_sz;

   debug_printf(
      MDATA_TRACE_LVL, "set str_table_idx: " SIZE_T_FMT ": \"%s\"",
      str_table->str_sz, &(str_table_p[str_table->str_sz]) );

   /* Set the string table cursor to the next available spot. */
   str_table->str_sz += str_sz + 1; /* +1 for terminating NULL. */

cleanup:

   if( MERROR_OK != retval ) {
      idx_p_out = retval * -1;
   }

   if( NULL != str_table_p ) {
      maug_munlock( str_table->str_h, str_table_p );
   }

   return idx_p_out;
}

/* === */

void mdata_strtable_free( struct MDATA_STRTABLE* str_table ) {
   if( (MAUG_MHANDLE)NULL != str_table->str_h ) {
      maug_mfree( str_table->str_h );
   }
}

#endif /* MDATA_C */

#endif /* MDATA_H */

