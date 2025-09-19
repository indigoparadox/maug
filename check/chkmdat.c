
#include "maugchck.h"

struct MDATA_VECTOR g_vector_test_append;
struct MDATA_VECTOR g_vector_test_insert;

struct MDATA_TABLE g_table_test_set;

char g_test_keys[9][8] = {
   "a", "bb", "ccc", "dddd", "eeeee", "ffffff", "ggggggg", "hhhhhhhh" };
int g_test_data[8] = { 16, 32, 64, 128, 88, 512, 1024, 2048 };

START_TEST( test_mdat_vector_append ) {
   MERROR_RETVAL retval = MERROR_OK;
   int* p_int = NULL;

   mdata_vector_lock( &g_vector_test_append );

   p_int = mdata_vector_get( &g_vector_test_append, _i, int );

   ck_assert_ptr_ne( p_int, NULL );
   ck_assert_int_eq( g_test_data[_i], *p_int );

cleanup:
   mdata_vector_unlock( &g_vector_test_append );

   ck_assert_uint_eq( retval, MERROR_OK );
}
END_TEST

START_TEST( test_mdat_vector_insert ) {
   MERROR_RETVAL retval = MERROR_OK;
   int* p_int = NULL;

   mdata_vector_lock( &g_vector_test_insert );

   p_int = mdata_vector_get( &g_vector_test_insert, 7 - _i, int );

   ck_assert_ptr_ne( p_int, NULL );
   ck_assert_int_eq( g_test_data[_i], *p_int );

cleanup:
   mdata_vector_unlock( &g_vector_test_insert );

   ck_assert_uint_eq( retval, MERROR_OK );
}
END_TEST

START_TEST( test_mdat_vector_lockunlock ) {
   MERROR_RETVAL retval = MERROR_OK;
   ck_assert( !mdata_vector_is_locked( &g_vector_test_append ) );
   mdata_vector_lock( &g_vector_test_append );
   ck_assert( mdata_vector_is_locked( &g_vector_test_append ) );
   mdata_vector_unlock( &g_vector_test_append );
   ck_assert( !mdata_vector_is_locked( &g_vector_test_append ) );
cleanup:
   ck_assert_uint_eq( retval, MERROR_OK );
   return;
}
END_TEST

void vector_setup() {
   size_t i = 0;
   ssize_t idx = 0;

   maug_mzero( &g_vector_test_append, sizeof( struct MDATA_VECTOR ) );
   for( i = 0 ; 8 > i ; i++ ) {
      idx = mdata_vector_append(
         &g_vector_test_append, &(g_test_data[i]), sizeof( int ) );
      /* ck_assert_int_eq( idx, i ); */
   }

   maug_mzero( &g_vector_test_insert, sizeof( struct MDATA_VECTOR ) );
   for( i = 0 ; 8 > i ; i++ ) {
      idx = mdata_vector_insert(
         &g_vector_test_insert, &(g_test_data[i]), 0, sizeof( int ) );
      /* ck_assert_int_eq( idx, 0 ); */
   }
}

void vector_teardown() {
   mdata_vector_free( &g_vector_test_insert );
   mdata_vector_free( &g_vector_test_append );
   memset( &g_vector_test_insert, '\0', sizeof( struct MDATA_VECTOR ) );
   memset( &g_vector_test_append, '\0', sizeof( struct MDATA_VECTOR ) );
}

START_TEST( test_mdat_table_set ) {
   int* p_int = NULL;

   debug_printf( MDATA_TRACE_LVL, "test_mdat_table_set" );

   mdata_table_lock( &g_table_test_set );

   p_int = mdata_table_get( &g_table_test_set, g_test_keys[_i], int );
   ck_assert_ptr_ne( p_int, NULL );
   ck_assert_int_eq( *p_int, g_test_data[_i] );

   mdata_table_unlock( &g_table_test_set );
}
END_TEST

START_TEST( test_mdat_table_unset ) {
   MERROR_RETVAL retval = MERROR_OK;
   int* p_int = NULL;

   mdata_table_lock( &g_table_test_set );

   p_int = mdata_table_get( &g_table_test_set, g_test_keys[_i], int );
   ck_assert_ptr_ne( p_int, NULL );
   ck_assert_int_eq( *p_int, g_test_data[_i] );

   /* Only remove the special case (index 3). */
   retval = mdata_table_unset( &g_table_test_set, g_test_keys[3] );
   ck_assert_uint_eq( retval, MERROR_OK );
   ck_assert( mdata_table_is_locked( &g_table_test_set ) );

   p_int = mdata_table_get( &g_table_test_set, g_test_keys[_i], int );

   /* See if *only* the special case was removed. */
   if( 3 == _i ) {
      ck_assert_ptr_eq( p_int, NULL );
   } else {
      ck_assert_ptr_ne( p_int, NULL );
   }

   mdata_table_unlock( &g_table_test_set );

   ck_assert_uint_eq( retval, MERROR_OK );
}
END_TEST

START_TEST( test_mdat_table_lockunlock ) {
   ck_assert( !mdata_table_is_locked( &g_table_test_set ) );
   mdata_table_lock( &g_table_test_set );
   ck_assert( mdata_table_is_locked( &g_table_test_set ) );
   mdata_table_unlock( &g_table_test_set );
   ck_assert( !mdata_table_is_locked( &g_table_test_set ) );
}
END_TEST

START_TEST( test_mdat_table_overwrite ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MDATA_TABLE table_test;
   int i = 0;
   int* p_int;

   maug_mzero( &table_test, sizeof( struct MDATA_TABLE ) );

   ck_assert_int_eq( mdata_table_ct( &table_test ), 0 );

   for( i = 0 ; 10 > i ; i++ ) {
      retval = mdata_table_set( &table_test, "xyz", &i, sizeof( int ) );
      ck_assert_int_eq( MERROR_OK, retval );
   }
   i = 0;

   ck_assert_int_eq( mdata_table_ct( &table_test ), 1 );

   mdata_table_lock( &table_test );

   p_int = mdata_table_get( &table_test, "xyz", int );
   ck_assert_ptr_ne( p_int, NULL );
   ck_assert_int_eq( *p_int, 9 );

   mdata_table_unlock( &table_test );

   mdata_table_free( &table_test );

   ck_assert_uint_eq( retval, MERROR_OK );
}
END_TEST

void table_setup() {
   size_t i = 0;
   MERROR_RETVAL retval = MERROR_OK;

   for( i = 0 ; 8 > i ; i++ ) {
      retval = mdata_table_set(
         &g_table_test_set, g_test_keys[i],
         &(g_test_data[i]), sizeof( int ) );
      /* ck_assert_int_eq( MERROR_OK, retval ); */
   }
}

void table_teardown() {
   mdata_table_free( &g_table_test_set );
}

START_TEST( test_mdat_strpool_add ) {
   struct MDATA_STRPOOL sp_test;
   mdata_strpool_idx_t idx = 0;

   maug_mzero( &sp_test, sizeof( struct MDATA_STRPOOL ) );

   ck_assert_int_eq( mdata_strpool_sz( &sp_test ), 0 );

   idx = mdata_strpool_append(
      &sp_test, "foo", maug_strlen( "foo" ), MDATA_STRPOOL_FLAG_DEDUPE );

   ck_assert_int_eq( idx, sizeof( size_t ) );

   /* strpool is padded so size_t is always aligned in memory, so instead of
    * 4 for "foo" + 0, it's 8 for the 4 bytes of padding 0s on 64-bit.
    */
   ck_assert_int_eq( mdata_strpool_sz( &sp_test ), 
      sizeof( size_t ) + 4 + mdata_strpool_padding( 3 ) );

   mdata_strpool_free( &sp_test );
}
END_TEST

START_TEST( test_mdat_strpool_double_add ) {
   struct MDATA_STRPOOL sp_test;
   mdata_strpool_idx_t idx = 0;

   maug_mzero( &sp_test, sizeof( struct MDATA_STRPOOL ) );

   idx = mdata_strpool_append( &sp_test, "foo", maug_strlen( "foo" ), 0 );
   ck_assert_int_eq( idx, sizeof( size_t ) );

   idx = mdata_strpool_append( &sp_test, "fii", maug_strlen( "foo" ), 0 );

   /* strpool is padded so size_t is always aligned in memory, so instead of
    * 4 for "foo" + 0, it's 8 for the 4 bytes of padding 0s on 64-bit.
    */
   ck_assert_int_eq( idx,
      4 + (mdata_strpool_padding( 3 )) + (2 * sizeof( size_t ) ) );

   idx = mdata_strpool_append( &sp_test, "foo", maug_strlen( "foo" ), 0 );
   ck_assert_int_eq( idx,
      8 + (2 * mdata_strpool_padding( 3 )) + (3 * sizeof( size_t ) ) );

   mdata_strpool_free( &sp_test );
}
END_TEST

START_TEST( test_mdat_strpool_dedupe ) {
   struct MDATA_STRPOOL sp_test;
   mdata_strpool_idx_t idx = 0;

   maug_mzero( &sp_test, sizeof( struct MDATA_STRPOOL ) );

   idx = mdata_strpool_append(
      &sp_test, "foo", maug_strlen( "foo" ), MDATA_STRPOOL_FLAG_DEDUPE );
   ck_assert_int_eq( idx, sizeof( size_t ) );

   idx = mdata_strpool_append(
      &sp_test, "fii", maug_strlen( "foo" ), MDATA_STRPOOL_FLAG_DEDUPE );

   /* strpool is padded so size_t is always aligned in memory, so instead of
    * 4 for "foo" + 0, it's 8 for the 4 bytes of padding 0s on 64-bit.
    */
   ck_assert_int_eq( idx,
      4 + (mdata_strpool_padding( 3 )) + (2 * sizeof( size_t ) ) );

   idx = mdata_strpool_append(
      &sp_test, "foo", maug_strlen( "foo" ), MDATA_STRPOOL_FLAG_DEDUPE );
   ck_assert_int_eq( idx, sizeof( size_t ) );

   mdata_strpool_free( &sp_test );
}
END_TEST

START_TEST( test_mdat_strpool_extract ) {
   struct MDATA_STRPOOL sp_test;
   MAUG_MHANDLE ex_test_h = NULL;
   char* ex_test = NULL;
   mdata_strpool_idx_t idx = 0;

   maug_mzero( &sp_test, sizeof( struct MDATA_STRPOOL ) );

   idx = mdata_strpool_append(
      &sp_test, "foo", maug_strlen( "foo" ), MDATA_STRPOOL_FLAG_DEDUPE );
   ck_assert_int_eq( idx, sizeof( size_t ) );
   
   idx = mdata_strpool_append(
      &sp_test, "fii", maug_strlen( "foo" ), MDATA_STRPOOL_FLAG_DEDUPE );

   mdata_strpool_append(
      &sp_test, "fee", maug_strlen( "foo" ), MDATA_STRPOOL_FLAG_DEDUPE );

   ex_test_h = mdata_strpool_extract( &sp_test, idx );
   maug_mlock( ex_test_h, ex_test );

   ck_assert_str_eq( ex_test, "fii" );

   maug_munlock( ex_test_h, ex_test );
   maug_mfree( ex_test_h );
   
   mdata_strpool_free( &sp_test );
}
END_TEST

START_TEST( test_mdat_strpool_remove ) {
}
END_TEST

Suite* mdat_suite( void ) {
   Suite* s;
   TCase* tc_vector;
   TCase* tc_table;
   TCase* tc_strpool;

   s = suite_create( "mdat" );

   tc_vector = tcase_create( "Vector" );

   tcase_add_checked_fixture( tc_vector, vector_setup, vector_teardown );
   
   tcase_add_loop_test( tc_vector, test_mdat_vector_append, 0, 8 );
   tcase_add_loop_test( tc_vector, test_mdat_vector_insert, 0, 8 );
   tcase_add_test( tc_vector, test_mdat_vector_lockunlock );

   suite_add_tcase( s, tc_vector );

   /* = */

   tc_table = tcase_create( "Table" );

   tcase_add_checked_fixture( tc_table, table_setup, table_teardown );

   tcase_add_loop_test( tc_table, test_mdat_table_set, 0, 8 );
   tcase_add_loop_test( tc_table, test_mdat_table_unset, 0, 8 );
   tcase_add_test( tc_table, test_mdat_table_lockunlock );
   tcase_add_test( tc_table, test_mdat_table_overwrite );

   suite_add_tcase( s, tc_table );

   /* = */

   tc_strpool = tcase_create( "StrPool" );

   tcase_add_test( tc_strpool, test_mdat_strpool_add );
   tcase_add_test( tc_strpool, test_mdat_strpool_double_add );
   tcase_add_test( tc_strpool, test_mdat_strpool_dedupe );
   tcase_add_test( tc_strpool, test_mdat_strpool_extract );
   tcase_add_test( tc_strpool, test_mdat_strpool_remove );

   suite_add_tcase( s, tc_strpool );

   return s;
}

