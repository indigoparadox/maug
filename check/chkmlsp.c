
#include "maugchck.h"

struct MLISP_PARSER g_parser;
struct MLISP_EXEC_STATE g_exec;

char g_script[] = "(begin (define x 3) (define y (+ x 6)))";
int g_baseline_env_ct = 0;

START_TEST( test_mlsp_exec_step ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_visit_ct = NULL;
   size_t i = 0;
   struct MLISP_ENV_NODE* e = NULL;

   for( i = 0 ; _i > i ; i++ ) {
      retval = mlisp_step( &g_parser, &g_exec );
      ck_assert_int_eq( retval, MERROR_OK );
   }

   /* debug_printf( 1, " ------ %d -------", _i );
   mlisp_env_dump( &g_parser, &g_exec, 0 ); */
   /* mlisp_ast_dump( &g_parser, 0, 0, 0 ); */

   /* These step counts are based on an initial benchmark from the g_script
    * testing script above.
    */
   if( 2 == _i ) {
      ck_assert_int_eq(
         mdata_vector_ct( &(g_exec.env) ), g_baseline_env_ct );
   
   } else if( 3 == _i ) {
      ck_assert_int_eq(
         mdata_vector_ct( &(g_exec.env) ), g_baseline_env_ct + 1 );

      /* Check visit count. */
      mdata_vector_lock( &(g_exec.per_node_visit_ct) );
      p_visit_ct = mdata_vector_get( &(g_exec.per_node_visit_ct), 1, size_t );
      assert( NULL != p_visit_ct );
      ck_assert_int_eq( *p_visit_ct, _i );

      /* Check env. */
      mdata_vector_lock( &(g_exec.env) );
      e = mlisp_env_get( &g_parser, &g_exec, "x" );
      ck_assert_ptr_ne( e, NULL );
      ck_assert_int_eq( e->value.integer, 3 );
   } else if( 7 == _i ) {
      ck_assert_int_eq(
         mdata_vector_ct( &(g_exec.env) ), g_baseline_env_ct + 1 );

   } else if( 8 == _i ) {
      ck_assert_int_eq(
         mdata_vector_ct( &(g_exec.env) ), g_baseline_env_ct + 2 );

      mdata_vector_lock( &(g_exec.env) );
      e = mlisp_env_get( &g_parser, &g_exec, "y" );
      ck_assert_ptr_ne( e, NULL );
      ck_assert_int_eq( e->value.integer, 9 );
      e = mlisp_env_get( &g_parser, &g_exec, "x" );
      ck_assert_ptr_ne( e, NULL );
      ck_assert_int_eq( e->value.integer, 3 );
   }

cleanup:
   if( 8 == _i || 3 == _i ) {
      mdata_vector_unlock( &(g_exec.env) );
   } else if( 3 == _i ) {
      mdata_vector_unlock( &(g_exec.per_node_visit_ct) );
   }
}
END_TEST

void mlisp_setup() {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   /* Setup parser. */
   retval = mlisp_parser_init( &g_parser );
   ck_assert_int_eq( retval, MERROR_OK );

   for( i = 0 ; strlen( g_script ) > i ; i++ ) {
      retval = mlisp_parse_c( &g_parser, g_script[i] );
      ck_assert_int_eq( retval, MERROR_OK );
   }
   
   /* Setup exec. */
   retval = mlisp_exec_init( &g_parser, &g_exec, 0 );
   ck_assert_int_eq( retval, MERROR_OK );

   /* Grab count including builtins. */
   g_baseline_env_ct = mdata_vector_ct( &(g_exec.env) );
}

void mlisp_teardown() {

}

Suite* mlsp_suite( void ) {
   Suite* s;
   TCase* tc_exec;

   s = suite_create( "mlsp" );

   tc_exec = tcase_create( "Exec" );

   tcase_add_checked_fixture( tc_exec, mlisp_setup, mlisp_teardown );

   tcase_add_loop_test( tc_exec, test_mlsp_exec_step, 0, 9 );

   suite_add_tcase( s, tc_exec );

   return s;
}

