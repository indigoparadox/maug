
#include "maugchck.h"

#include "chkbmps.h"

START_TEST( test_mfmt_decode_rle_4bit ) {
   mfile_t check_rle_file;
   MAUG_MHANDLE check_rle_out_h = (MAUG_MHANDLE)NULL;
   uint8_t* check_rle_out = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   check_rle_out_h = maug_malloc( 1, sizeof( gc_check_rle_raw ) );
   mfile_lock_buffer(
      gc_check_rle, sizeof( gc_check_rle_raw ), &check_rle_file );

   retval = mfmt_decode_rle(
      &check_rle_file, 0, sizeof( gc_check_rle ), 32,
      check_rle_out_h, sizeof( gc_check_rle_raw ), MFMT_DECOMP_FLAG_4BIT );

   ck_assert_uint_eq( retval, MERROR_OK );

   maug_mlock( check_rle_out_h, check_rle_out );

   for( i = 0 ; sizeof( gc_check_rle_raw ) > i ; i++ ) {
      debug_printf( MFMT_TRACE_RLE_LVL,
         "i: " SIZE_T_FMT " (of " SIZE_T_FMT ") test: "
         "0x%02x good: 0x%02x",
         i, sizeof( gc_check_rle_raw ),
         check_rle_out[i], gc_check_rle_raw[i] );
      ck_assert_uint_eq( check_rle_out[i], gc_check_rle_raw[i] );
   }

   maug_munlock( check_rle_out_h, check_rle_out );
   maug_mfree( check_rle_out_h );
}
END_TEST

START_TEST( test_mfmt_bmp_px_4bit ) {
   mfile_t check_4bit_file;
   MAUG_MHANDLE check_8bit_out_h = (MAUG_MHANDLE)NULL;
   uint8_t* check_8bit_out = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   struct MFMT_STRUCT_BMPINFO header_bmp_info;

   check_8bit_out_h = maug_malloc( 1, sizeof( gc_check_8bit ) );
   mfile_lock_buffer(
      gc_check_4bit, sizeof( gc_check_8bit ), &check_4bit_file );

   maug_mlock( check_8bit_out_h, check_8bit_out );

   header_bmp_info.sz = 40;
   header_bmp_info.compression = 0;
   header_bmp_info.width = 32;
   header_bmp_info.height = 32;
   header_bmp_info.bpp = 4;
   header_bmp_info.img_sz = sizeof( gc_check_8bit );
   header_bmp_info.palette_ncolors = 16;

   retval = mfmt_read_bmp_px(
      (struct MFMT_STRUCT*)&header_bmp_info,
      check_8bit_out, sizeof( gc_check_8bit ),
      &check_4bit_file, 0, sizeof( gc_check_4bit ),
      /* Force inversion to match test case. */
      MFMT_PX_FLAG_INVERT_Y );

   ck_assert_uint_eq( retval, MERROR_OK );

   for( i = 0 ; sizeof( gc_check_8bit ) > i ; i++ ) {
      debug_printf( MFMT_TRACE_BMP_LVL,
         "i: " SIZE_T_FMT " (of " SIZE_T_FMT ") test: "
         "0x%02x good: 0x%02x",
         i, sizeof( gc_check_8bit ),
         check_8bit_out[i], gc_check_8bit[i] );
      ck_assert_uint_eq( check_8bit_out[i], gc_check_8bit[i] );
   }

   maug_munlock( check_8bit_out_h, check_8bit_out );
   maug_mfree( check_8bit_out_h );
}
END_TEST

Suite* mfmt_suite( void ) {
   Suite* s;
   TCase* tc_decode;

   s = suite_create( "mfmt" );

   tc_decode = tcase_create( "Decode" );

   /* TODO: FIXME */
   /* tcase_add_test( tc_decode, test_mfmt_decode_rle_4bit ); */
   tcase_add_test( tc_decode, test_mfmt_bmp_px_4bit );

   suite_add_tcase( s, tc_decode );

   return s;
}

