
#include "maugchck.h"

START_TEST( test_mfil_mem_read ) {
}
END_TEST

START_TEST( test_mfil_mem_write ) {
}
END_TEST

Suite* mfil_suite( void ) {
   Suite* s;
   TCase* tc_mem;

   s = suite_create( "mfil" );

   tc_mem = tcase_create( "Memory" );

   tcase_add_test( tc_mem, test_mfil_mem_read );
   tcase_add_test( tc_mem, test_mfil_mem_write );

   suite_add_tcase( s, tc_mem );

   return s;
}

