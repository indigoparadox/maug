
#include "maugchck.h"

struct MDATA_VECTOR g_test_append;
struct MDATA_VECTOR g_test_insert;

int g_test_data[8] = { 16, 32, 64, 128, 88, 512, 1024, 2048 };

START_TEST( test_mdat_vector_append ) {
   MAUG_MHANDLE check_rle_out_h = (MAUG_MHANDLE)NULL;
   MERROR_RETVAL retval = MERROR_OK;
   int* p_int = NULL;

   mdata_vector_lock( &g_test_append );

   p_int = mdata_vector_get( &g_test_append, _i, int );

   ck_assert_int_eq( g_test_data[_i], *p_int );

cleanup:
   mdata_vector_unlock( &g_test_append );
}
END_TEST

START_TEST( test_mdat_vector_insert ) {
   MAUG_MHANDLE check_rle_out_h = (MAUG_MHANDLE)NULL;
   MERROR_RETVAL retval = MERROR_OK;
   int* p_int = NULL;

   mdata_vector_lock( &g_test_insert );

   p_int = mdata_vector_get( &g_test_insert, 7 - _i, int );

   ck_assert_int_eq( g_test_data[_i], *p_int );

cleanup:
   mdata_vector_unlock( &g_test_append );
}
END_TEST

void vector_setup() {
   size_t i = 0;

   maug_mzero( &g_test_append, sizeof( struct MDATA_VECTOR ) );
   for( i = 0 ; 8 > i ; i++ ) {
      mdata_vector_append( &g_test_append, &(g_test_data[i]), sizeof( int ) );
   }

   maug_mzero( &g_test_insert, sizeof( struct MDATA_VECTOR ) );
   for( i = 0 ; 8 > i ; i++ ) {
      mdata_vector_insert(
         &g_test_insert, &(g_test_data[i]), 0, sizeof( int ) );
   }
}

void vector_teardown() {

}

Suite* mdat_suite( void ) {
   Suite* s;
   TCase* tc_vector;

   s = suite_create( "mlsp" );

   tc_vector = tcase_create( "Vector" );

   tcase_add_checked_fixture( tc_vector, vector_setup, vector_teardown );
   
   tcase_add_loop_test( tc_vector, test_mdat_vector_append, 0, 8 );
   tcase_add_loop_test( tc_vector, test_mdat_vector_insert, 0, 8 );

   suite_add_tcase( s, tc_vector );

   return s;
}

