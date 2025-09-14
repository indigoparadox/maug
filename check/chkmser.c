
#include "maugchck.h"

START_TEST( test_mser_struct_write ) {
}
END_TEST

START_TEST( test_mser_struct_read ) {
}
END_TEST

Suite* mser_suite( void ) {
   Suite* s;
   TCase* tc_struct;

   s = suite_create( "mser" );

   tc_struct = tcase_create( "Struct" );

   tcase_add_test( tc_struct, test_mser_struct_read );
   tcase_add_test( tc_struct, test_mser_struct_write );

   suite_add_tcase( s, tc_struct );

   return s;
}

