
#include "maugchck.h"

struct MDATA_VECTOR g_vector_test_append;
struct MDATA_VECTOR g_vector_test_insert;

struct MDATA_TABLE g_table_test_set;

char g_test_keys[8][4] = {
   "aaa", "bbb", "ccc", "ddd", "eee", "fff", "ggg", "hhh" };
int g_test_data[8] = { 16, 32, 64, 128, 88, 512, 1024, 2048 };

START_TEST( test_mdat_vector_append ) {
   MAUG_MHANDLE check_rle_out_h = (MAUG_MHANDLE)NULL;
   MERROR_RETVAL retval = MERROR_OK;
   int* p_int = NULL;

   mdata_vector_lock( &g_vector_test_append );

   p_int = mdata_vector_get( &g_vector_test_append, _i, int );

   ck_assert_int_eq( g_test_data[_i], *p_int );

cleanup:
   mdata_vector_unlock( &g_vector_test_append );
}
END_TEST

START_TEST( test_mdat_vector_insert ) {
   MAUG_MHANDLE check_rle_out_h = (MAUG_MHANDLE)NULL;
   MERROR_RETVAL retval = MERROR_OK;
   int* p_int = NULL;

   mdata_vector_lock( &g_vector_test_insert );

   p_int = mdata_vector_get( &g_vector_test_insert, 7 - _i, int );

   ck_assert_int_eq( g_test_data[_i], *p_int );

cleanup:
   mdata_vector_unlock( &g_vector_test_append );
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
      ck_assert_int_eq( idx, i );
   }

   maug_mzero( &g_vector_test_insert, sizeof( struct MDATA_VECTOR ) );
   for( i = 0 ; 8 > i ; i++ ) {
      idx = mdata_vector_insert(
         &g_vector_test_insert, &(g_test_data[i]), 0, sizeof( int ) );
      ck_assert_int_eq( idx, 0 );
   }
}

void vector_teardown() {

}

START_TEST( test_mdat_table_set ) {
   int* p_int = NULL;

   mdata_table_lock( &g_table_test_set );

   p_int = mdata_table_get( &g_table_test_set, g_test_keys[_i], int );
   ck_assert_int_eq( *p_int, g_test_data[_i] );

   mdata_table_unlock( &g_table_test_set );
}
END_TEST

START_TEST( test_mdat_table_unset ) {
   MERROR_RETVAL retval = MERROR_OK;
   int* p_int = NULL;

   mdata_table_lock( &g_table_test_set );

   p_int = mdata_table_get( &g_table_test_set, g_test_keys[_i], int );
   ck_assert_int_eq( *p_int, g_test_data[_i] );

   /* Only remove the special case (index 3). */
   retval = mdata_table_unset( &g_table_test_set, g_test_keys[3] );
   ck_assert_int_eq( retval, MERROR_OK );
   ck_assert( mdata_table_is_locked( &g_table_test_set ) );

   p_int = mdata_table_get( &g_table_test_set, g_test_keys[_i], int );

   /* See if *only* the special case was removed. */
   if( 3 == _i ) {
      ck_assert_ptr_eq( p_int, NULL );
   } else {
      ck_assert_ptr_ne( p_int, NULL );
   }

   mdata_table_unlock( &g_table_test_set );
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

void table_setup() {
   size_t i = 0;
   MERROR_RETVAL retval = MERROR_OK;

   for( i = 0 ; 8 > i ; i++ ) {
      retval = mdata_table_set(
         &g_table_test_set, g_test_keys[i], 3,
         &(g_test_data[i]), sizeof( int ) );
      ck_assert_int_eq( MERROR_OK, retval );
   }
}

void table_teardown() {

}

Suite* mdat_suite( void ) {
   Suite* s;
   TCase* tc_vector;
   TCase* tc_table;

   s = suite_create( "mdat" );

   tc_vector = tcase_create( "Vector" );

   tcase_add_checked_fixture( tc_vector, vector_setup, vector_teardown );
   
   tcase_add_loop_test( tc_vector, test_mdat_vector_append, 0, 8 );
   tcase_add_loop_test( tc_vector, test_mdat_vector_insert, 0, 8 );

   suite_add_tcase( s, tc_vector );

   /* = */

   tc_table = tcase_create( "Table" );

   tcase_add_checked_fixture( tc_table, table_setup, table_teardown );

   tcase_add_loop_test( tc_table, test_mdat_table_set, 0, 8 );
   tcase_add_loop_test( tc_table, test_mdat_table_unset, 0, 8 );
   tcase_add_loop_test( tc_table, test_mdat_table_lockunlock, 0, 8 );

   suite_add_tcase( s, tc_table );

   return s;
}

