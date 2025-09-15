
#include "maugchck.h"

#define VEC_ARR_LOOPS 3

uint8_t g_test_vec_ser[] = {
   0x30, 0x25, 0x02, 0x01, 0x01, 0x02, 0x01, 0x02, 0x02, 0x01, 0x04, 0x02,
   0x01, 0x08, 0x02, 0x01, 0x10, 0x02, 0x01, 0x20, 0x02, 0x01, 0x40, 0x02,
   0x02, 0x00, 0x80, 0x02, 0x02, 0x01, 0x00, 0x02, 0x02, 0x02, 0x00, 0x02,
   0x02, 0x04, 0x00
};

uint8_t g_test_vec_arr_ser[] = {
  0x30, 0x77, 0x30, 0x25, 0x02, 0x01, 0x01, 0x02, 0x01, 0x02, 0x02, 0x01,
  0x04, 0x02, 0x01, 0x08, 0x02, 0x01, 0x10, 0x02, 0x01, 0x20, 0x02, 0x01,
  0x40, 0x02, 0x02, 0x00, 0x80, 0x02, 0x02, 0x01, 0x00, 0x02, 0x02, 0x02,
  0x00, 0x02, 0x02, 0x04, 0x00, 0x30, 0x26, 0x02, 0x01, 0x02, 0x02, 0x01,
  0x04, 0x02, 0x01, 0x08, 0x02, 0x01, 0x10, 0x02, 0x01, 0x20, 0x02, 0x01,
  0x40, 0x02, 0x02, 0x00, 0x80, 0x02, 0x02, 0x01, 0x00, 0x02, 0x02, 0x02,
  0x00, 0x02, 0x02, 0x04, 0x00, 0x02, 0x02, 0x08, 0x00, 0x30, 0x26, 0x02,
  0x01, 0x03, 0x02, 0x01, 0x06, 0x02, 0x01, 0x0c, 0x02, 0x01, 0x18, 0x02,
  0x01, 0x30, 0x02, 0x01, 0x60, 0x02, 0x02, 0x00, 0xc0, 0x02, 0x02, 0x01,
  0x80, 0x02, 0x02, 0x03, 0x00, 0x02, 0x02, 0x06, 0x00, 0x02, 0x02, 0x0c,
  0x00
};

void gen_test_vec( struct MDATA_VECTOR* vec_test, size_t i_mult ) {
   size_t i = 0;
   size_t i_buf = 0;

   maug_mzero( vec_test, sizeof( struct MDATA_VECTOR ) );

   for( i = 1 ; 2048 > i ; i *= 2 ) {
      i_buf = i * i_mult;
      mdata_vector_append( vec_test, &i_buf, sizeof( size_t ) );
   }
}

START_TEST( test_mser_struct_read ) {
}
END_TEST

START_TEST( test_mser_struct_write ) {
}
END_TEST

START_TEST( test_mser_vector_read ) {
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t ser_mem;
   struct MDATA_VECTOR vec_test;
   size_t value_buf = 0;
   ssize_t ser_sz = 0;
   size_t* p_test = NULL;
   struct MDATA_VECTOR vec_gen;
   size_t* p_gen = NULL;

   gen_test_vec( &vec_gen, 1 );

   maug_mzero( &vec_test, sizeof( struct MDATA_VECTOR ) );

   retval = mfile_lock_buffer(
      g_test_vec_ser, sizeof( g_test_vec_ser ), &ser_mem );
   ck_assert_int_eq( retval, MERROR_OK );

   retval = mdeserialize_vector(
      &ser_mem, &vec_test, 1, (mdeserialize_cb_t)mdeserialize_size_t,
      (uint8_t*)&value_buf, sizeof( size_t ), &ser_sz );
   ck_assert_int_eq( retval, MERROR_OK );

   mfile_close( &ser_mem );

   mdata_vector_lock( &vec_test );
   mdata_vector_lock( &vec_gen );
   p_test = mdata_vector_get( &vec_test, _i, size_t );
   p_gen = mdata_vector_get( &vec_gen, _i, size_t );
   ck_assert_int_eq( *p_test, *p_gen );
   mdata_vector_unlock( &vec_gen );
   mdata_vector_unlock( &vec_test );

cleanup:

   ck_assert_int_eq( retval, MERROR_OK );
}

START_TEST( test_mser_vector_read_arr ) {
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t ser_mem;
   struct MDATA_VECTOR vec_test[VEC_ARR_LOOPS];
   size_t value_buf = 0;
   ssize_t ser_sz = 0;
   size_t* p_test = NULL;
   struct MDATA_VECTOR vec_gen[VEC_ARR_LOOPS];
   size_t* p_gen = NULL;
   size_t i = 0;

   for( i = 0 ; VEC_ARR_LOOPS > i ; i++ ) {
      gen_test_vec( &(vec_gen[i]), i + 1 );
      ck_assert( !mdata_vector_is_locked( &(vec_gen[i]) ) );
   }

   maug_mzero( vec_test, VEC_ARR_LOOPS * sizeof( struct MDATA_VECTOR ) );

   retval = mfile_lock_buffer(
      g_test_vec_arr_ser, sizeof( g_test_vec_arr_ser ), &ser_mem );
   ck_assert_int_eq( retval, MERROR_OK );

   retval = mdeserialize_vector(
      &ser_mem, vec_test, VEC_ARR_LOOPS,
      (mdeserialize_cb_t)mdeserialize_size_t,
      (uint8_t*)&value_buf, sizeof( size_t ), &ser_sz );
   ck_assert_int_eq( retval, MERROR_OK );

   mfile_close( &ser_mem );

   for( i = 0 ; VEC_ARR_LOOPS > i ; i++ ) {
      ck_assert( !mdata_vector_is_locked( &(vec_test[i]) ) );
      ck_assert( !mdata_vector_is_locked( &(vec_gen[i]) ) );
      mdata_vector_lock( &(vec_test[i]) );
      mdata_vector_lock( &(vec_gen[i]) );
      p_test = mdata_vector_get( &(vec_test[i]), _i, size_t );
      p_gen = mdata_vector_get( &(vec_gen[i]), _i, size_t );
      ck_assert_ptr_ne( p_gen, NULL );
      ck_assert_ptr_ne( p_test, NULL );
      ck_assert_int_eq( *p_test, *p_gen );
      mdata_vector_unlock( &(vec_gen[i]) );
      mdata_vector_unlock( &(vec_test[i]) );
   }

cleanup:

   ck_assert_int_eq( retval, MERROR_OK );
}

START_TEST( test_mser_vector_write ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MDATA_VECTOR vec_test;
   uint8_t buf_mem[sizeof( g_test_vec_ser )];
   mfile_t ser_mem;

   gen_test_vec( &vec_test, 1 );

   maug_mzero( buf_mem, sizeof( g_test_vec_ser ) );

   retval = mfile_open_write( "chkvec", &ser_mem );
   /*
   retval = mfile_lock_buffer(
      buf_mem, sizeof( g_test_vec_ser ), &ser_mem );
   */
   ck_assert_int_eq( retval, MERROR_OK );

   retval = mserialize_vector( &ser_mem, &vec_test, 1,
      (mserialize_cb_t)mserialize_size_t );
   ck_assert_int_eq( retval, MERROR_OK );

   ser_mem.seek( &ser_mem, 0 );
   ser_mem.read_block( &ser_mem, buf_mem, sizeof( g_test_vec_ser ) );

   ck_assert_mem_eq( buf_mem, g_test_vec_ser, sizeof( g_test_vec_ser ) );

   mfile_close( &ser_mem );
}
END_TEST

START_TEST( test_mser_vector_write_arr ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MDATA_VECTOR vec_test[VEC_ARR_LOOPS];
   uint8_t buf_mem[sizeof( g_test_vec_arr_ser )];
   mfile_t ser_mem;
   size_t i = 0;

   for( i = 0 ; VEC_ARR_LOOPS > i ; i++ ) {
      gen_test_vec( &(vec_test[i]), i + 1 );
   }

   maug_mzero( buf_mem, sizeof( g_test_vec_arr_ser ) );

   retval = mfile_open_write( "chkvecar", &ser_mem );
   /*
   retval = mfile_lock_buffer(
      buf_mem, sizeof( g_test_vec_ser ), &ser_mem );
   */
   ck_assert_int_eq( retval, MERROR_OK );

   retval = mserialize_vector( &ser_mem, vec_test, VEC_ARR_LOOPS,
      (mserialize_cb_t)mserialize_size_t );
   ck_assert_int_eq( retval, MERROR_OK );

   ser_mem.seek( &ser_mem, 0 );
   ser_mem.read_block( &ser_mem, buf_mem, sizeof( g_test_vec_arr_ser ) );

   ck_assert_mem_eq(
      buf_mem, g_test_vec_arr_ser, sizeof( g_test_vec_arr_ser ) );

   mfile_close( &ser_mem );
}
END_TEST

Suite* mser_suite( void ) {
   Suite* s;
   TCase* tc_struct;

   s = suite_create( "mser" );

   tc_struct = tcase_create( "Struct" );

   tcase_add_test( tc_struct, test_mser_struct_read );
   tcase_add_test( tc_struct, test_mser_struct_write );
   tcase_add_loop_test( tc_struct, test_mser_vector_read, 0, 11 );
   tcase_add_loop_test( tc_struct, test_mser_vector_read_arr, 0, 11 );
   tcase_add_test( tc_struct, test_mser_vector_write );
   tcase_add_test( tc_struct, test_mser_vector_write_arr );

   suite_add_tcase( s, tc_struct );

   return s;
}

