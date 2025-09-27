
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

MERROR_RETVAL open_temp( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
   char filename_path[MAUG_PATH_SZ_MAX + 1];

   maug_mzero( filename_path, MAUG_PATH_SZ_MAX + 1 );

   strncat( filename_path, TMP_PATH, MAUG_PATH_SZ_MAX );
   strncat( filename_path, filename, MAUG_PATH_SZ_MAX );

   remove( filename_path );

   retval = mfile_open_write( filename_path, p_file );

   return retval;
}

void close_temp( mfile_t* p_file ) {
   remove( p_file->filename );
   mfile_close( p_file );
}

int main( void ) {
   int number_failed = 0;
   MAUGCHK_TABLE( MAUGCHK_VARDEFS )

   MAUGCHK_TABLE( MAUGCHK_EXECS )

   printf( "all checks complete! %d failures.\n", number_failed );

   return( number_failed == 0 ) ? 0 : 1;
}

