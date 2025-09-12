
#include "maugchck.h"

char g_script_simple[] = "(begin (define x 3) (define y (+ x 6)))";
char g_script_lambda[] = "(begin (define y 1) (define cb1 (lambda (x) (begin (define y (+ x 6)) (define x (+ y 8))))) (cb1 3) (define q 3))";

MERROR_RETVAL init_mlsp_script(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* script, int iter, ssize_t* p_baseline_env_ct
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   maug_mzero( parser, sizeof( struct MLISP_PARSER ) );
   maug_mzero( exec, sizeof( struct MLISP_EXEC_STATE ) );

   /* Setup parser. */
   retval = mlisp_parser_init( parser );
   ck_assert_int_eq( retval, MERROR_OK );

   for( i = 0 ; strlen( script ) > i ; i++ ) {
      retval = mlisp_parse_c( parser, script[i] );
      ck_assert_int_eq( retval, MERROR_OK );
   }
   
   /* Setup exec. */
   retval = mlisp_exec_init( parser, exec, 0 );
   ck_assert_int_eq( retval, MERROR_OK );

   /* Grab count including builtins. */
   *p_baseline_env_ct = mdata_vector_ct( &(exec->env) );

   /* Execute the script. */
   for( i = 0 ; iter > i ; i++ ) {
      retval = mlisp_step( parser, exec );
      ck_assert_int_eq( retval, MERROR_OK );
   }

   return retval;
}

START_TEST( test_mlsp_exec_step ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_visit_ct = NULL;
   size_t i = 0;
   struct MLISP_ENV_NODE* e = NULL;
   struct MLISP_PARSER parser;
   struct MLISP_EXEC_STATE exec;
   ssize_t baseline_env_ct = 0;

   init_mlsp_script( &parser, &exec, g_script_simple, _i, &baseline_env_ct );

   /* debug_printf( 1, " ------ %d -------", _i );
   mlisp_env_dump( &g_parser, &g_exec, 0 ); */
   /* mlisp_ast_dump( &g_parser, 0, 0, 0 ); */

   /* These step counts are based on an initial benchmark from the g_script
    * testing script above.
    */
   if( 2 == _i ) {
      ck_assert_int_eq(
         mdata_vector_ct( &(exec.env) ), baseline_env_ct );
   
   } else if( 3 == _i ) {
      ck_assert_int_eq(
         mdata_vector_ct( &(exec.env) ), baseline_env_ct + 1 );

      /* Check visit count. */
      mdata_vector_lock( &(exec.per_node_visit_ct) );
      p_visit_ct = mdata_vector_get( &(exec.per_node_visit_ct), 1, size_t );
      assert( NULL != p_visit_ct );
      ck_assert_int_eq( *p_visit_ct, _i );

      /* Check env. */
      mdata_vector_lock( &(exec.env) );
      e = mlisp_env_get( &parser, &exec, "x" );
      ck_assert_ptr_ne( e, NULL );
      ck_assert_int_eq( e->value.integer, 3 );
   } else if( 7 == _i ) {
      ck_assert_int_eq(
         mdata_vector_ct( &(exec.env) ), baseline_env_ct + 1 );

   } else if( 8 == _i ) {
      ck_assert_int_eq(
         mdata_vector_ct( &(exec.env) ), baseline_env_ct + 2 );

      mdata_vector_lock( &(exec.env) );
      e = mlisp_env_get( &parser, &exec, "y" );
      ck_assert_ptr_ne( e, NULL );
      ck_assert_int_eq( e->value.integer, 9 );
      e = mlisp_env_get( &parser, &exec, "x" );
      ck_assert_ptr_ne( e, NULL );
      ck_assert_int_eq( e->value.integer, 3 );
   }

cleanup:
   if( 8 == _i || 3 == _i ) {
      mdata_vector_unlock( &(exec.env) );
   } else if( 3 == _i ) {
      mdata_vector_unlock( &(exec.per_node_visit_ct) );
   }
   ck_assert_int_eq( retval, MERROR_OK );
}
END_TEST

START_TEST( test_mlsp_exec_lambda ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_visit_ct = NULL;
   size_t i = 0;
   struct MLISP_ENV_NODE* e = NULL;
   struct MLISP_PARSER parser;
   struct MLISP_EXEC_STATE exec;
   ssize_t baseline_env_ct = 0;

   retval = init_mlsp_script(
      &parser, &exec, g_script_lambda, _i, &baseline_env_ct );

   debug_printf( 1, " ------ %d -------", _i );
   mlisp_env_dump( &parser, &exec, 0 );

   mdata_vector_lock( &(exec.env) );
   debug_printf( 1, "get y" );
   e = mlisp_env_get( &parser, &exec, "y" );
   ck_assert_ptr_ne( e, NULL );
   ck_assert_int_eq( e->value.integer, 9 );
   debug_printf( 1, "end get y: %d", e->value.integer );

cleanup:

   ck_assert_int_eq( retval, MERROR_OK );
}

Suite* mlsp_suite( void ) {
   Suite* s;
   TCase* tc_exec;

   s = suite_create( "mlsp" );

   tc_exec = tcase_create( "Exec" );

   tcase_add_loop_test( tc_exec, test_mlsp_exec_step, 0, 9 );
   tcase_add_loop_test( tc_exec, test_mlsp_exec_lambda, 0, 25 );

   suite_add_tcase( s, tc_exec );

   return s;
}

