
#ifndef MFAKECHK_H
#define MFAKECHK_H

#define MFAKECHECK_TESTS_CT_MAX 10

#define MFAKECHECK_TCASE_NAME_SZ_MAX 10

#define MFAKECHECK_SUITE_NAME_SZ_MAX 10

#define MFAKECHECK_CASES_CT_MAX 100

#define MFAKECHECK_SUITES_CT_MAX 10

#define CK_VERBOSE 0x01

typedef void (*fakechk_fn)( int _i );

#define START_TEST( f ) void f ( int _i )

#define END_TEST

#define tcase_add_checked_fixture( tcase, setup, teardown )

#define tcase_add_test( case_struct, fn ) \
   tcase_add_loop_test( case_struct, fn, 0, 1 )

#define ck_assert( a )

#define ck_assert_int_eq( a, b )

#define ck_assert_uint_eq( a, b )

#define ck_assert_str_eq( a, b )

#define ck_assert_mem_eq( a, b, sz )

#define ck_assert_ptr_eq( a, b )

#define ck_assert_ptr_ne( a, b )

typedef struct {
   fakechk_fn tests[MFAKECHECK_TESTS_CT_MAX];
   int tests_ct;
   char name[MFAKECHECK_TCASE_NAME_SZ_MAX + 1];
   int loop_starts[MFAKECHECK_TESTS_CT_MAX];
   int loop_ends[MFAKECHECK_TESTS_CT_MAX];
} TCase;

typedef struct {
   char name[MFAKECHECK_SUITE_NAME_SZ_MAX + 1];
   TCase* cases[MFAKECHECK_CASES_CT_MAX];
   int cases_ct;
} Suite;

typedef struct {
   Suite* suite;
} SRunner;

Suite* suite_create( const char* name );

SRunner* srunner_create( Suite* suite );

TCase* tcase_create( const char* name );

void suite_add_tcase( Suite* suite, TCase* tc );

void tcase_add_loop_test(
   TCase* case_struct, fakechk_fn fn, int start, int end );

int srunner_run_all( SRunner* runner, unsigned char flags );

int srunner_ntests_failed( SRunner* runner );

void srunner_free( SRunner* runner );

#ifdef MFAKECHK_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Suite g_mfakecheck_suites[MFAKECHECK_SUITES_CT_MAX] = { 0 };
int g_mfakecheck_suites_ct = 0;

SRunner g_mfakecheck_srunners[MFAKECHECK_SUITES_CT_MAX] = { 0 };
int g_mfakecheck_srunners_ct = 0;

TCase g_mfakecheck_cases[MFAKECHECK_CASES_CT_MAX] = { 0 };
int g_mfakecheck_cases_ct = 0;

Suite* suite_create( const char* name ) {
   Suite* suite = &(g_mfakecheck_suites[g_mfakecheck_suites_ct]);
   printf( "creating suite: %s\n", name );
   memset( suite, '\0', sizeof( Suite ) );
   strncpy( suite->name, name, MFAKECHECK_SUITE_NAME_SZ_MAX );
   g_mfakecheck_suites_ct++;
   if( g_mfakecheck_suites_ct >= MFAKECHECK_SUITES_CT_MAX ) {
      printf( "too many suites!\n" );
      return NULL;
   }
   return suite;
}

SRunner* srunner_create( Suite* suite ) {
   SRunner* srunner = &(g_mfakecheck_srunners[g_mfakecheck_srunners_ct]);
   memset( srunner, '\0', sizeof( SRunner ) );
   srunner->suite = suite;
   g_mfakecheck_srunners_ct++;
   if( g_mfakecheck_srunners_ct >= MFAKECHECK_SUITES_CT_MAX ) {
      printf( "too many suite runners!\n" );
      return NULL;
   }
   return srunner;
}

TCase* tcase_create( const char* name ) {
   TCase* tcase = &(g_mfakecheck_cases[g_mfakecheck_cases_ct]);
   printf( "creating test case: %s\n", name );
   memset( tcase, '\0', sizeof( TCase ) );
   strncpy( tcase->name, name, MFAKECHECK_TCASE_NAME_SZ_MAX );
   g_mfakecheck_cases_ct++;
   if( g_mfakecheck_cases_ct >= MFAKECHECK_CASES_CT_MAX ) {
      printf( "too many test cases!\n" );
      return NULL;
   }
   return tcase;
}

void tcase_add_loop_test(
   TCase* case_struct, fakechk_fn fn, int start, int end
) {
   if( case_struct->tests_ct >= MFAKECHECK_TESTS_CT_MAX ) {
      return;
   }
   case_struct->tests[case_struct->tests_ct] = fn;
   case_struct->loop_starts[case_struct->tests_ct] = start;
   case_struct->loop_ends[case_struct->tests_ct] = end;
   case_struct->tests_ct++;
   if( case_struct->tests_ct >= MFAKECHECK_TESTS_CT_MAX ) {
      printf( "too many tests in case: %s!\n", case_struct->name );
   }
}

void suite_add_tcase( Suite* suite, TCase* tc ) {
   if( suite->cases_ct >= MFAKECHECK_CASES_CT_MAX ) {
      return;
   }
   suite->cases[suite->cases_ct] = tc;
   suite->cases_ct++;
   if( suite->cases_ct >= MFAKECHECK_CASES_CT_MAX ) {
      printf( "too many cases in suite: %s!\n", suite->name );
   }
}

int srunner_run_all( SRunner* runner, unsigned char flags ) {
   size_t i = 0, j = 0;
   int k = 0;

   printf( "running suite: %s\n", runner->suite->name );

   for( i = 0 ; runner->suite->cases_ct > i ; i++ ) {
      printf( "running case: %s\n", runner->suite->cases[i]->name );
      for( j = 0 ; runner->suite->cases[i]->tests_ct > j ; j++ ) {
         for(
            k = runner->suite->cases[i]->loop_starts[j] > k ; 
            runner->suite->cases[i]->loop_ends[j] > k ;
            k++
         ) {
            printf( "running test: %d (%d)\n", j, k );
            runner->suite->cases[i]->tests[j]( k );
         }
      }
   }
}

int srunner_ntests_failed( SRunner* runner ) {

}

void srunner_free( SRunner* runner ) {

}

#endif /* MFAKECHK_C */

#endif /* !MFAKECHK_H */

