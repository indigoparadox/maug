
#define MAUG_C

#include "maugchck.h"

#define main_add_test_proto( suite_name ) \
   Suite* suite_name ## _suite();

#define main_add_test( suite_name ) \
   Suite* s_ ## suite_name = suite_name ## _suite(); \
   SRunner* sr_ ## suite_name = srunner_create( s_ ## suite_name ); \
   /* srunner_set_fork_status( sr_ ## suite_name, CK_NOFORK ); */ \
   srunner_run_all( sr_ ## suite_name, CK_VERBOSE ); \
   number_failed += srunner_ntests_failed( sr_ ## suite_name ); \
   srunner_free( sr_ ## suite_name );

main_add_test_proto( mdat )
main_add_test_proto( mfmt )
main_add_test_proto( mlsp )
main_add_test_proto( mfil )
main_add_test_proto( mser )

int main( void ) {
   int number_failed = 0;

   main_add_test( mdat );
   main_add_test( mfmt );
   main_add_test( mlsp );
   main_add_test( mfil );
   main_add_test( mser );

   return( number_failed == 0 ) ? 0 : 1;
}

