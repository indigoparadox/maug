
#include <maug.h>

#include <retroflt.h>
#define RETROTIL_C
#include <retrotil.h>

#include <check.h>

Suite* rtil_suite( void ) {
   Suite* s;
   TCase* tc_pathfind;

   s = suite_create( "rtil" );

   tc_pathfind = tcase_create( "Pathfind" );

   suite_add_tcase( s, tc_pathfind );

   return s;
}

