
#include "maugchck.h"

#define TEST_MEM_SZ 8

uint8_t g_test_mem[TEST_MEM_SZ] =
   { 0xff, 0xee, 0x33, 0x77, 0x12, 0x34, 0x56, 0x78 };

START_TEST( test_mfil_mem_read ) {
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t test_file;
   uint8_t char_buf;

   retval = mfile_lock_buffer(
      (MAUG_MHANDLE)NULL, g_test_mem, TEST_MEM_SZ, &test_file );
   ck_assert_uint_eq( retval, MERROR_OK );

   test_file.seek( &test_file, _i );
   test_file.read_block( &test_file, &char_buf, 1 );

   ck_assert_int_eq( char_buf, g_test_mem[_i] );

   mfile_close( &test_file );
}
END_TEST

START_TEST( test_mfil_mem_write ) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t test_buf[TEST_MEM_SZ];
   mfile_t test_file;

   retval = mfile_lock_buffer(
      (MAUG_MHANDLE)NULL, test_buf, TEST_MEM_SZ, &test_file );
   ck_assert_uint_eq( retval, MERROR_OK );

   test_file.write_block( &test_file, g_test_mem, TEST_MEM_SZ );

   ck_assert_int_eq( test_buf[_i], g_test_mem[_i] );

   mfile_close( &test_file );
}
END_TEST

START_TEST( test_mfil_mem_insert ) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t test_buf[TEST_MEM_SZ];
   mfile_t test_file;
   size_t i = 0;

   for( i = 0 ; TEST_MEM_SZ > i ; i++ ) {
      test_buf[i] = i + 1;
   }

   retval = mfile_lock_buffer(
      (MAUG_MHANDLE)NULL, test_buf, TEST_MEM_SZ, &test_file );
   ck_assert_uint_eq( retval, MERROR_OK );

   test_file.seek( &test_file, 0 );

   test_file.write_block( &test_file, &(g_test_mem[_i]), 1 );

   ck_assert_int_eq( test_buf[0], g_test_mem[_i] );
   ck_assert_int_eq( test_buf[1], 1 );

   mfile_close( &test_file );
}
END_TEST

START_TEST( test_mfil_mem_cursor ) {
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t test_file;
   char test_buf[TEST_MEM_SZ];

   retval = mfile_lock_buffer(
      (MAUG_MHANDLE)NULL, test_buf, TEST_MEM_SZ, &test_file );
   ck_assert_uint_eq( retval, MERROR_OK );

   test_file.write_block( &test_file, g_test_mem, _i );

   ck_assert_int_eq( test_file.cursor( &test_file ), _i );

   mfile_close( &test_file );
}

START_TEST( test_mfil_file_cursor ) {
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t test_file;

   retval = open_temp( "chkfile", &test_file );
   ck_assert_uint_eq( retval, MERROR_OK );

   test_file.write_block( &test_file, g_test_mem, _i );

   ck_assert_int_eq( test_file.cursor( &test_file ), _i );

   close_temp( &test_file );
}
END_TEST

Suite* mfil_suite( void ) {
   Suite* s;
   TCase* tc_mem;
   TCase* tc_file;

   s = suite_create( "mfil" );

   tc_mem = tcase_create( "Memory" );

   tcase_add_loop_test( tc_mem, test_mfil_mem_read, 0, TEST_MEM_SZ );
   tcase_add_loop_test( tc_mem, test_mfil_mem_write, 0, TEST_MEM_SZ );
   tcase_add_loop_test( tc_mem, test_mfil_mem_insert, 1, TEST_MEM_SZ );
   tcase_add_loop_test( tc_mem, test_mfil_mem_cursor, 0, TEST_MEM_SZ );

   suite_add_tcase( s, tc_mem );

   tc_file = tcase_create( "File" );

   tcase_add_loop_test( tc_file, test_mfil_file_cursor, 0, TEST_MEM_SZ );

   suite_add_tcase( s, tc_file );

   return s;
}

