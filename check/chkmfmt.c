
#include <maug.h>

#include <check.h>

#include "chkbmps.h"

START_TEST( test_mfmt_decode_rle ) {
   mfile_t check_rle_file;
   MAUG_MHANDLE check_rle_out_h = (MAUG_MHANDLE)NULL;
   uint8_t* check_rle_out = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   check_rle_out_h = maug_malloc( 1, sizeof( gc_check_rle_raw ) );
   mfile_lock_buffer( gc_check_rle, &check_rle_file );

   retval = mfmt_decode_rle(
      &check_rle_file, 0, sizeof( gc_check_rle ), 32,
      check_rle_out_h, sizeof( gc_check_rle_raw ), MFMT_DECOMP_FLAG_4BIT );

   ck_assert_uint_eq( retval, MERROR_OK );

   maug_mlock( check_rle_out_h, check_rle_out );

   for( i = 0 ; sizeof( gc_check_rle_raw ) > i ; i++ ) {
      debug_printf( 1, "i: " SIZE_T_FMT " (of " SIZE_T_FMT ") test: "
         "0x%02x good: 0x%02x",
         i, sizeof( gc_check_rle_raw ),
         check_rle_out[i], gc_check_rle_raw[i] );
      ck_assert_uint_eq( check_rle_out[i], gc_check_rle_raw[i] );
   }

   maug_munlock( check_rle_out_h, check_rle_out );
   maug_mfree( check_rle_out_h );
}
END_TEST

Suite* mfmt_suite( void ) {
   Suite* s;
   TCase* tc_decode;

   s = suite_create( "mfmt" );

   tc_decode = tcase_create( "Decode" );

   tcase_add_test( tc_decode, test_mfmt_decode_rle );

   suite_add_tcase( s, tc_decode );

   return s;
}

