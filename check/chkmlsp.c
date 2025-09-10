
#include "maugchck.h"

START_TEST( test_mlsp_init ) {
   MAUG_MHANDLE check_rle_out_h = (MAUG_MHANDLE)NULL;
   MERROR_RETVAL retval = MERROR_OK;

}
END_TEST

Suite* mlsp_suite( void ) {
   Suite* s;
   TCase* tc_decode;

   s = suite_create( "mlsp" );

   tc_decode = tcase_create( "Exec" );

   tcase_add_test( tc_decode, test_mlsp_init );

   suite_add_tcase( s, tc_decode );

   return s;
}

