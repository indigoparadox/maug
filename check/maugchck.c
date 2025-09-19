
#define MAUG_C
#define MFAKECHK_C

#include "maugchck.h"

#define MAUGCHK_PROTOS( suite_name ) \
   Suite* suite_name ## _suite();

#define MAUGCHK_VARDEFS( suite_name ) \
   Suite* s_ ## suite_name = suite_name ## _suite(); \
   SRunner* sr_ ## suite_name = srunner_create( s_ ## suite_name );

#define MAUGCHK_EXECS( suite_name ) \
   /* srunner_set_fork_status( sr_ ## suite_name, CK_NOFORK ); */ \
   srunner_run_all( sr_ ## suite_name, CK_VERBOSE ); \
   number_failed += srunner_ntests_failed( sr_ ## suite_name ); \
   srunner_free( sr_ ## suite_name );

MAUGCHK_TABLE( MAUGCHK_PROTOS )

int main( void ) {
   int number_failed = 0;
   MAUGCHK_TABLE( MAUGCHK_VARDEFS )

   MAUGCHK_TABLE( MAUGCHK_EXECS )

   printf( "all checks complete! %d failures.\n", number_failed );

   return( number_failed == 0 ) ? 0 : 1;
}

