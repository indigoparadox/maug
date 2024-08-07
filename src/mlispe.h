
#ifndef MLISPE_H
#define MLISPE_H

#include <mlisps.h>

/**
 * \addtogroup mlisp MLISP Interpreter
 * \{
 *
 * \file mlispe.h
 * \brief MLISP Interpreter Functions and Macros
 */

#ifndef MLISP_TOKEN_SZ_MAX
#  define MLISP_TOKEN_SZ_MAX 4096
#endif /* !MLISP_TOKEN_SZ_MAX */

#ifndef MLISP_EXEC_TRACE_LVL
#  define MLISP_EXEC_TRACE_LVL 0
#endif /* !MLISP_EXEC_TRACE_LVL */

#define MLISP_ENV_FLAG_BUILTIN   0x02

/*! \brief Flag for _mlisp_env_cb_cmp() specifying TRUE if A > B. */
#define MLISP_ENV_FLAG_CMP_GT    0x10

/*! \brief Flag for _mlisp_env_cb_cmp() specifying TRUE if A < B. */
#define MLISP_ENV_FLAG_CMP_LT    0x20

/*! \brief Flag for _mlisp_env_cb_cmp() specifying TRUE if A == B. */
#define MLISP_ENV_FLAG_CMP_EQ    0x40

/*! \brief Flag for _mlisp_env_cb_arithmetic() specifying to add A + B. */
#define MLISP_ENV_FLAG_ARI_ADD   0x10

/*! \brief Flag for _mlisp_env_cb_arithmetic() specifying to multiply A * B. */
#define MLISP_ENV_FLAG_ARI_MUL   0x20

/**
 * \addtogroup mlisp_stack MLISP Execution Stack
 * \{
 */

/**
 * \brief Push a value onto MLISP_EXEC_STATE::stack.
 * \param exec Pointer to the running ::MLISP_EXEC_STATE.
 * \param i Value to push.
 * \param C type of the value to push. Please see \ref mlisp_types for more
 *        information.
 * \warning MLISP_EXEC_STATE::stack should be *unlocked* prior to calling!
 */
#define mlisp_stack_push( exec, i, ctype ) \
   (_mlisp_stack_push_ ## ctype( exec, (ctype)i ))

MERROR_RETVAL mlisp_stack_dump(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

/**
 * \brief Pop a value off of (removing from) MLISP_EXEC_STATE::stack and copy
 *        it to a provided output.
 *
 * \warning MLISP_EXEC_STATE::stack should be *unlocked* prior to calling!
 */
MERROR_RETVAL mlisp_stack_pop(
   struct MLISP_EXEC_STATE* exec, struct MLISP_STACK_NODE* o );

/*! \} */ /* mlisp_stack */

/**
 * \brief Get a node from the environment denoted by a string in the strpool.
 * \param parser 
 * \param token_strpool_idx Index of the env key in the strpool.
 * \param token_strpool_sz Length of the env key in the strpool. Use 0 to
 *                         detect automatically by NULL termination.
 * \return Pointer to e in MLISP_PARSER::env. This is not a copy, so
 *         the pointer is only valid for so long as MLISP_PARSER:env remains
 *         locked!
 */
struct MLISP_ENV_NODE* mlisp_env_get_strpool(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* strpool, size_t token_strpool_idx, size_t token_strpool_sz );

MERROR_RETVAL mlisp_env_set(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* token, size_t token_sz, uint8_t env_type, void* data,
   void* cb_data, uint8_t flags );

MERROR_RETVAL mlisp_step(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

/*! \} */ /* mlisp */

#define mlisp_ast_has_ready_children( exec_child_idx, n ) \
   ((exec_child_idx) < (n)->ast_idx_children_sz)

#ifdef MLISPE_C

static MERROR_RETVAL _mlisp_preempt(
   const char* caller, struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec, size_t* p_child_idx,
   size_t new_idx );

static MERROR_RETVAL _mlisp_step_iter(
   struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec );

/* === */

/* Stack Functions */

/* === */

MERROR_RETVAL mlisp_stack_dump(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   char* strpool = NULL;
   struct MLISP_STACK_NODE* n_stack = NULL;

#  define _MLISP_TYPE_TABLE_DUMPS( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == n_stack->type ) { \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (" #const_name "): " fmt, \
            i, n_stack->value.name );

   mdata_vector_lock( &(exec->stack) );
   mdata_strpool_lock( &(parser->strpool), strpool ); \
   while( i < mdata_vector_ct( &(exec->stack) ) ) {
      n_stack = mdata_vector_get( &(exec->stack), i, struct MLISP_STACK_NODE );

      /* Handle special exceptions. */
      if( MLISP_TYPE_STR == n_stack->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (STR): %s",
            i, &(strpool[n_stack->value.strpool_idx]) );

      } else if( MLISP_TYPE_CB == n_stack->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (CB): %p",
            i, n_stack->value.cb );

      } else if( MLISP_TYPE_LAMBDA == n_stack->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (LAMBDA): " SIZE_T_FMT,
               i, n_stack->value.lambda );

      } else if( MLISP_TYPE_ARGS_S == n_stack->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (ARGS_S): " SIZE_T_FMT,
               i, n_stack->value.args_start );

      } else if( MLISP_TYPE_ARGS_E == n_stack->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (ARGS_E): " SIZE_T_FMT,
               i, n_stack->value.args_end );

      } else if( MLISP_TYPE_BEGIN == n_stack->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (BEGIN): " SIZE_T_FMT,
               i, n_stack->value.begin );

      /* Handle numeric types. */
      MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_DUMPS );
      } else {
         error_printf( "invalid stack type: %u", n_stack->type );
      }
      i++;
   }
   mdata_strpool_unlock( &(parser->strpool), strpool );
   mdata_vector_unlock( &(exec->stack) );

cleanup:

   return retval;
}

/* === */

#define _MLISP_TYPE_TABLE_PUSH( idx, ctype, name, const_name, fmt ) \
   MERROR_RETVAL _mlisp_stack_push_ ## ctype( \
      struct MLISP_EXEC_STATE* exec, ctype i \
   ) { \
      struct MLISP_STACK_NODE n_stack; \
      MERROR_RETVAL retval = MERROR_OK; \
      debug_printf( MLISP_EXEC_TRACE_LVL, \
         "pushing " #const_name " onto stack: " fmt, i ); \
      n_stack.type = MLISP_TYPE_ ## const_name; \
      n_stack.value.name = i; \
      retval = mdata_vector_append( \
         &(exec->stack), &n_stack, sizeof( struct MLISP_STACK_NODE ) ); \
      if( 0 > retval ) { \
         retval = mdata_retval( retval ); \
      } else { \
         retval = 0; \
      } \
      return retval; \
   }

MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_PUSH );

/* === */

MERROR_RETVAL mlisp_stack_pop(
   struct MLISP_EXEC_STATE* exec, struct MLISP_STACK_NODE* o
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE* n_stack = NULL;
   size_t n_idx = 0;

   /* Check for valid stack pointer. */
   if( mdata_vector_ct( &(exec->stack) ) == 0 ) {
      error_printf( "stack underflow!" );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   n_idx = mdata_vector_ct( &(exec->stack) ) - 1;

   /* Perform the pop! */
   mdata_vector_lock( &(exec->stack) );
   n_stack = mdata_vector_get(
      &(exec->stack), n_idx, struct MLISP_STACK_NODE );
   assert( NULL != n_stack );
   memcpy( o, n_stack, sizeof( struct MLISP_STACK_NODE ) );
   n_stack = NULL;
   mdata_vector_unlock( &(exec->stack) );

#  define _MLISP_TYPE_TABLE_POPD( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == o->type ) { \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "popping: " SSIZE_T_FMT ": " fmt, n_idx, o->value.name );

   if( 0 ) {
   MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_POPD )
   }

   retval = mdata_vector_remove( &(exec->stack), n_idx );

cleanup:

   return retval;
}

/* === */

/* Env Functons */

/* === */

MERROR_RETVAL mlisp_env_dump(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   char* strpool = NULL;
   struct MLISP_ENV_NODE* e = NULL;

#  define _MLISP_TYPE_TABLE_DUMPE( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == e->type ) { \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT \
               " \"%s\" (" #const_name "): " fmt, \
            i, &(strpool[e->name_strpool_idx]), e->value.name ); \

   mdata_strpool_lock( &(parser->strpool), strpool ); \
   mdata_vector_lock( &(exec->env) );
   while( i < mdata_vector_ct( &(exec->env) ) ) {
      assert( mdata_vector_is_locked( &(exec->env) ) );
      e = mdata_vector_get( &(exec->env), i, struct MLISP_ENV_NODE );

      if( MLISP_ENV_FLAG_BUILTIN == (MLISP_ENV_FLAG_BUILTIN & e->flags) ) {
         /* Skip builtins. */
         i++;
         continue;
      }

      if( 0 ) {
      MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_DUMPE );
      /* Handle special exceptions. */
      } else if( MLISP_TYPE_STR == e->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT " \"%s\" (STR): %s",
            i, &(strpool[e->name_strpool_idx]),
            &(strpool[e->value.strpool_idx]) );

      } else if( MLISP_TYPE_CB == e->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT " \"%s\" (CB): %p",
            i, &(strpool[e->name_strpool_idx]), e->value.cb );

      } else if( MLISP_TYPE_LAMBDA == e->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT
               " \"%s\" (LAMBDA): " SIZE_T_FMT,
            i, &(strpool[e->name_strpool_idx]), e->value.lambda );

      } else if( MLISP_TYPE_ARGS_S == e->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT
               " \"%s\" (ARGS_S): " SIZE_T_FMT,
            i, &(strpool[e->name_strpool_idx]), e->value.args_start );

      } else if( MLISP_TYPE_ARGS_E == e->type ) {
         debug_printf( MLISP_EXEC_TRACE_LVL,
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT
               " \"%s\" (ARGS_E): " SIZE_T_FMT,
            i, &(strpool[e->name_strpool_idx]), e->value.args_end );

      } else {
         error_printf( MLISP_TRACE_SIGIL " invalid env type: %u", e->type );
      }
      i++;
   }
   mdata_vector_unlock( &(exec->env) );
   mdata_strpool_unlock( &(parser->strpool), strpool ); \

cleanup:

   return retval;
}

/* === */

struct MLISP_ENV_NODE* mlisp_env_get_strpool(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* strpool, size_t token_strpool_idx, size_t token_strpool_sz
) {
   struct MLISP_ENV_NODE* node_out = NULL;
   struct MLISP_ENV_NODE* node_test = NULL;
   ssize_t i = mdata_vector_ct( &(exec->env) ) - 1;

   /* This requires env be locked before entrance! */

   while( 0 <= i ) {
      assert( mdata_vector_is_locked( &(exec->env) ) );
      node_test = mdata_vector_get( &(exec->env), i, struct MLISP_ENV_NODE );
      if( 0 == strncmp(
         &(strpool[node_test->name_strpool_idx]),
         &(strpool[token_strpool_idx]),
         token_strpool_sz + 1
      ) ) {
         node_out = node_test;
         break;
      }
      i--;
   }

   return node_out;
}

/* === */

MERROR_RETVAL mlisp_env_set(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* token, size_t token_sz, uint8_t env_type, void* data,
   void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_ENV_NODE e;
   ssize_t new_idx_out = -1;

   if( 0 == token_sz ) {
      token_sz = maug_strlen( token );
   }
   assert( 0 < token_sz );

   /* TODO: Find previous env nodes with same token and change. */

#  define _MLISP_TYPE_TABLE_ASGN( idx, ctype, name, const_name, fmt ) \
      case idx: \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "setting env: \"%s\": #" fmt, \
               token, (ctype)*((ctype*)data) ); \
         e.value.name = *((ctype*)data); \
         break;

   /* Setup the new node to copy. */
   maug_mzero( &e, sizeof( struct MLISP_ENV_NODE ) );
   e.flags = flags;
   e.name_strpool_idx =
      mdata_strpool_append( &(parser->strpool), token, token_sz );
   if( 0 > e.name_strpool_idx ) {
      retval = mdata_retval( e.name_strpool_idx );
   }
   maug_cleanup_if_not_ok();
   e.type = env_type;
   e.cb_data = cb_data;
   switch( env_type ) {
   MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_ASGN );

   /* Special cases: */

   case 4 /* MLISP_TYPE_STR */:
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "setting env: \"%s\": strpool(" SSIZE_T_FMT ")",
         token, *((ssize_t*)data) );
      e.value.strpool_idx = *((mdata_strpool_idx_t*)data);
      break;

   case 5 /* MLISP_TYPE_CB */:
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "setting env: \"%s\": 0x%p", token, (mlisp_env_cb_t)data );
      e.value.cb = (mlisp_env_cb_t)data;
      break;

   case 6 /* MLISP_TYPE_LAMBDA */:
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "setting env: \"%s\": node #" SSIZE_T_FMT,
         token, *((mlisp_lambda_t*)data) );
      e.value.lambda = *((mlisp_lambda_t*)data);
      break;

   case 7: /* MLISP_TYPE_ARGS_S */
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "setting env: \"%s\": node #" SSIZE_T_FMT,
         token, *((mlisp_args_t*)data) );
      e.value.args_start = *((mlisp_args_t*)data);
      break;

   case 8: /* MLISP_TYPE_ARGS_E */
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "setting env: \"%s\": node #" SSIZE_T_FMT,
         token, *((mlisp_arge_t*)data) );
      e.value.args_end = *((mlisp_arge_t*)data);
      break;

   default:
      error_printf( "attempted to define invalid type: %d", env_type );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   /* Add the node to the env. */
   new_idx_out = mdata_vector_append(
      &(exec->env), &e, sizeof( struct MLISP_ENV_NODE ) );
   if( 0 > new_idx_out ) {
      retval = mdata_retval( new_idx_out );
   }
   maug_cleanup_if_not_ok();

   debug_printf( MLISP_EXEC_TRACE_LVL, "setup env node " SSIZE_T_FMT ": %s",
      new_idx_out, token );

cleanup:

   return retval;
}

/* === */

static ssize_t _mlisp_env_get_env_frame(
   struct MLISP_EXEC_STATE* exec, struct MLISP_ENV_NODE* e_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t ret_idx = 0;
   struct MLISP_ENV_NODE* e = NULL;
   ssize_t i = 0;
   uint8_t autolock = 0;

   if( !mdata_vector_is_locked( &(exec->env) ) ) {
      mdata_vector_lock( &(exec->env) );
      autolock = 1;
   }

   for( i = mdata_vector_ct( &(exec->env) ) - 1; 0 <= i ; i-- ) {
      /* debug_printf( MLISP_EXEC_TRACE_LVL,
         "getting frame (trying " SSIZE_T_FMT "...)", i ); */
      assert( mdata_vector_is_locked( &(exec->env) ) );
      e = mdata_vector_get( &(exec->env), i, struct MLISP_ENV_NODE );
      assert( NULL != e );

      if( MLISP_TYPE_ARGS_S != e->type ) {
         /* Hunt for the initial env arg separator. */
         continue;
      }

      debug_printf( MLISP_EXEC_TRACE_LVL,
         "found initial env arg separator " SSIZE_T_FMT " with ret: "
            SSIZE_T_FMT,
         i, e->value.args_start );

      ret_idx = i;
      if( NULL != e_out ) {
         memcpy( e_out, e, sizeof( struct MLISP_ENV_NODE ) );
      }
      break;
   }

cleanup:

   if( autolock ) {
      mdata_vector_unlock( &(exec->env) );
   }
 
   if( MERROR_OK != retval ) {
      ret_idx = retval * -1;
   }

   return ret_idx;
}

/* === */

static ssize_t _mlisp_env_prune_args( struct MLISP_EXEC_STATE* exec ) {
   ssize_t ret_idx = 0;
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;
   struct MLISP_ENV_NODE* e = NULL;
   size_t removed = 0;

   /* This function modifies the env, so existing locks might break. */
   assert( !mdata_vector_is_locked( &(exec->env) ) );

   assert( 0 < mdata_vector_ct( &(exec->env) ) );

   mdata_vector_lock( &(exec->env) );

   /* Get the most recent start frame in the env. */
   i = _mlisp_env_get_env_frame( exec, NULL );
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "pruning env args starting from env frame " SSIZE_T_FMT "...", i );
   e = mdata_vector_get( &(exec->env), i, struct MLISP_ENV_NODE );
   assert( NULL != e );

   while( MLISP_TYPE_ARGS_E != e->type ) {
      mdata_vector_unlock( &(exec->env) );
      retval = mdata_vector_remove( &(exec->env), i );
      maug_cleanup_if_not_ok();
      mdata_vector_lock( &(exec->env) );

      /* Refresh e based on what i *now* points to. */
      assert( mdata_vector_is_locked( &(exec->env) ) );
      e = mdata_vector_get( &(exec->env), i, struct MLISP_ENV_NODE );
      assert( NULL != e );

      removed++;
   }

   /* Remove the actual terminal separator. */
   mdata_vector_unlock( &(exec->env) );
   retval = mdata_vector_remove( &(exec->env), i );
   maug_cleanup_if_not_ok();
   mdata_vector_lock( &(exec->env) );

   debug_printf( MLISP_EXEC_TRACE_LVL,
      "removed " SIZE_T_FMT " args!", removed );

cleanup:

   mdata_vector_unlock( &(exec->env) );

   if( MERROR_OK != retval ) {
      ret_idx = retval * -1;
   }

   return ret_idx;
}

/* === */

static MERROR_RETVAL _mlisp_env_cb_cmp(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE tmp;
   char* strpool = NULL;
   uint8_t truth = 0;
   int a_int,
      b_int;
   int* cur_int = NULL;

#  define _MLISP_TYPE_TABLE_CMP( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == tmp.type ) { \
         *cur_int = (int)tmp.value.name; \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "cmp: pop " fmt " (%d)", tmp.value.name, *cur_int );

   retval = mlisp_stack_pop( exec, &tmp );
   maug_cleanup_if_not_ok();
   cur_int = &b_int;
   if( 0 ) {
   MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_CMP )
   } else {
      error_printf( "cmp: invalid type!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   retval = mlisp_stack_pop( exec, &tmp );
   maug_cleanup_if_not_ok();
   cur_int = &a_int;
   if( 0 ) {
   MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_CMP )
   } else {
      error_printf( "cmp: invalid type!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   /* TODO: String comparison? */

   if( MLISP_ENV_FLAG_CMP_GT == (MLISP_ENV_FLAG_CMP_GT & flags) ) {
      debug_printf( MLISP_EXEC_TRACE_LVL, "cmp %d > %d", a_int, b_int );
      truth = a_int > b_int;
   } else if( MLISP_ENV_FLAG_CMP_LT == (MLISP_ENV_FLAG_CMP_LT & flags) ) {
      debug_printf( MLISP_EXEC_TRACE_LVL, "cmp %d < %d", a_int, b_int );
      truth = a_int < b_int;
   } else if( MLISP_ENV_FLAG_CMP_EQ == (MLISP_ENV_FLAG_CMP_EQ & flags) ) {
      debug_printf( MLISP_EXEC_TRACE_LVL, "cmp %d == %d", a_int, b_int );
      truth = a_int == b_int;
   } else {
      error_printf( "invalid parameter provided to _mlisp_env_cb_cmp()!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   retval = mlisp_stack_push( exec, truth, mlisp_bool_t );

cleanup:

   mdata_strpool_unlock( &(parser->strpool), strpool );


   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_env_cb_arithmetic(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE mults[2];
   char* strpool = NULL;
   /* TODO: Vary type based on multiplied types. */
   int product = 0;
   size_t i = 0;

   if( MLISP_ENV_FLAG_ARI_MUL == (MLISP_ENV_FLAG_ARI_MUL & flags) ) {
      /* Start with 1 and multiply it below. */
      product = 1;
   }

#  define _MLISP_TYPE_TABLE_ARI( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == mults[i].type ) { \
         if( MLISP_ENV_FLAG_ARI_ADD == (MLISP_ENV_FLAG_ARI_ADD & flags) ) { \
            debug_printf( MLISP_EXEC_TRACE_LVL, \
               "arithmetic: %d + " fmt, product, mults[i].value.name ); \
            product += mults[i].value.name; \
         } else if( \
            MLISP_ENV_FLAG_ARI_MUL == (MLISP_ENV_FLAG_ARI_MUL & flags) \
         ) { \
            debug_printf( MLISP_EXEC_TRACE_LVL, \
               "arithmetic: %d * " fmt, product, mults[i].value.name ); \
            product *= mults[i].value.name; \
         }

   for( i = 0 ; 2 > i ; i++ ) {
      retval = mlisp_stack_pop( exec, &(mults[i]) );
      maug_cleanup_if_not_ok();

      if( 0 ) {
      MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_ARI )
      } else {
         error_printf( "arithmetic: invalid type!" );
         retval = MERROR_EXEC;
         goto cleanup;
      }
   }

   retval = mlisp_stack_push( exec, product, int16_t );

cleanup:

   mdata_strpool_unlock( &(parser->strpool), strpool );


   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_env_cb_define(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE key;
   struct MLISP_STACK_NODE val;
   MAUG_MHANDLE key_tmp_h = NULL;
   char* key_tmp = NULL;

   retval = mlisp_stack_pop( exec, &val );
   maug_cleanup_if_not_ok();

   retval = mlisp_stack_pop( exec, &key );
   maug_cleanup_if_not_ok();

   if( MLISP_TYPE_STR != key.type ) {
      /* TODO: Do we want to allow defining other types? */
      /* TODO: We can use _mlisp_eval_token_strpool, maybe? */
      error_printf( "define: invalid key type: %d", key.type );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   key_tmp_h = mdata_strpool_extract(
      &(parser->strpool), key.value.strpool_idx );
   /* TODO: Handle this gracefully. */
   assert( NULL != key_tmp_h );

   maug_mlock( key_tmp_h, key_tmp );
   maug_cleanup_if_null_lock( char*, key_tmp );

   debug_printf( MLISP_EXEC_TRACE_LVL,
      "define \"%s\" (strpool(" SIZE_T_FMT "))...",
      key_tmp, key.value.strpool_idx );

   retval = mlisp_env_set(
      parser, exec, key_tmp, 0, val.type, &(val.value), NULL, 0 );

cleanup:

   if( NULL != key_tmp ) {
      maug_munlock( key_tmp_h, key_tmp );
   }

   if( NULL != key_tmp_h ) {
      maug_mfree( key_tmp_h );
   }

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_env_cb_if(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_if_child_idx = NULL;
   struct MLISP_STACK_NODE s;
   struct MLISP_AST_NODE* n = NULL;

   debug_printf( MLISP_EXEC_TRACE_LVL, "qrqrqrqrqr STEP IF qrqrqrqrqr" );

   /* Grab the current exec index for the child vector for this node. */
   assert( mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   p_if_child_idx = mdata_vector_get(
      &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_if_child_idx );
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "child idx for if AST node " SIZE_T_FMT ": " SIZE_T_FMT,
      n_idx, *p_if_child_idx );

   n = mdata_vector_get( &(parser->ast), n_idx, struct MLISP_AST_NODE );

   if( 0 == *p_if_child_idx ) {
      /* Evaluating if condition. */
      debug_printf( MLISP_EXEC_TRACE_LVL, "stepping into condition..." );
      retval = _mlisp_step_iter(
         parser, n->ast_idx_children[*p_if_child_idx], exec );
      debug_printf( MLISP_EXEC_TRACE_LVL, "...stepped out of condition" );

      /* Vary the child we jump to based on the boolean val on the stack. */
      if( MERROR_OK == retval ) {
         /* Condition evaluation complete. */

         /* Pop the result and check it. */
         retval = mlisp_stack_pop( exec, &s );
         maug_cleanup_if_not_ok();
         if( MLISP_TYPE_BOOLEAN != s.type ) {
            error_printf( "(if) can only evaluate boolean type!" );
            retval = MERROR_EXEC;
            goto cleanup;
         }

         /* Set the child pointer to 1 if TRUE and 2 if FALSE. */
         retval = _mlisp_preempt(
            "if", parser, n_idx, exec, p_if_child_idx,
            /* Flip boolean and increment. */
            (1 - s.value.boolean) + 1 );
      }

   } else if( 3 > *p_if_child_idx ) {
      /* Pursuing TRUE or FALSE clause. */

      debug_printf( MLISP_EXEC_TRACE_LVL,
         "descending into IF path: " SIZE_T_FMT, *p_if_child_idx );
      
      /* Prepare for stepping. */

      /* Step and check. */
      retval = _mlisp_step_iter(
         parser, n->ast_idx_children[*p_if_child_idx], exec );
      if( MERROR_OK == retval ) {
         retval = _mlisp_preempt(
            "if", parser, n_idx, exec, p_if_child_idx, 3 );
      }
   }

cleanup:

   debug_printf( MLISP_EXEC_TRACE_LVL, "qrqrqrqrqr END STEP IF qrqrqrqrqr" );

   return retval;
}

/* === */

/* Execution Functions */

/* === */

static MERROR_RETVAL _mlisp_preempt(
   const char* caller, struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec, size_t* p_child_idx,
   size_t new_idx
) {
   /* Could not exec *this* node yet, so don't increment its parent. */
   MERROR_RETVAL retval = MERROR_PREEMPT;
   char* strpool = NULL;
   struct MLISP_AST_NODE* n = NULL;

   n = mdata_vector_get( &(parser->ast), n_idx, struct MLISP_AST_NODE );

   mdata_strpool_lock( &(parser->strpool), strpool );
   assert( 0 < maug_strlen( &(strpool[n->token_idx]) ) );
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "eval step " SSIZE_T_FMT " under (%s) %s...",
      *p_child_idx, caller, &(strpool[n->token_idx]) );
   mdata_strpool_unlock( &(parser->strpool), strpool );

   /* Increment this node, since the child actually executed. */
   (*p_child_idx) = new_idx;
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "incremented " SIZE_T_FMT " child idx to: " SIZE_T_FMT,
      n_idx, *p_child_idx );

cleanup:

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_step_iter_children(
   struct MLISP_PARSER* parser, size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_child_idx = NULL;
   struct MLISP_AST_NODE* n = NULL;

   /* Grab the current exec index for the child vector for this node. */
   assert( mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   p_child_idx = mdata_vector_get(
      &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_child_idx );
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "child idx for AST node " SIZE_T_FMT ": " SIZE_T_FMT,
      n_idx, *p_child_idx );

   n = mdata_vector_get( &(parser->ast), n_idx, struct MLISP_AST_NODE );

   if(
      (
         MLISP_AST_FLAG_LAMBDA == (MLISP_AST_FLAG_LAMBDA & n->flags) &&
         0 == *p_child_idx
      ) ||
      MLISP_AST_FLAG_IF == (MLISP_AST_FLAG_IF & n->flags) 
   ) {
      /* A lambda definition was found, and its exec counter is still pointing
       * to the arg list. This means the lambda was *not* called on the last
       * heartbeat, and we're probably just enountering its definition.
       *
       * Lambdas are lazily evaluated, so don't pursue it further until it's
       * called (stee _mlisp_step_lambda() for more info on this.
       */
      debug_printf( MLISP_EXEC_TRACE_LVL, "skipping lambda children..." );
      goto cleanup;
   }

   if( mlisp_ast_has_ready_children( *p_child_idx, n ) ) {
      /* Call the next uncalled child. */

      /* Step and check. */
      retval = _mlisp_step_iter(
         parser, n->ast_idx_children[*p_child_idx], exec );
      if( MERROR_OK == retval ) {
         retval = _mlisp_preempt(
            "node", parser, n_idx, exec, p_child_idx, (*p_child_idx) + 1 );
      }
      goto cleanup;

#if 0
   } else {
      /* Reset the node's child pointer to 0... this will allow it to be
       * re-entered later.
       */
      debug_printf( MDATA_TRACE_LVL,
         "resetting node " SIZE_T_FMT " child pointer to 0...",
         n_idx );
      *p_child_idx = 0;
#endif
   }

cleanup:

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_step_lambda_args(
   struct MLISP_PARSER* parser, size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t arg_idx = 0;
   struct MLISP_STACK_NODE stack_n_arg;
   struct MLISP_AST_NODE* ast_n_arg = NULL;
   MAUG_MHANDLE key_tmp_h = NULL;
   char* key_tmp = NULL;
   struct MLISP_AST_NODE* n = NULL;

   /* Pop stack into args into the env. These are all the results of previous
    * evaluations, before the lambda call, so we can just grab them all in
    * one go!
    */

   /* Get the current args node. */
   n = mdata_vector_get( &(parser->ast), n_idx, struct MLISP_AST_NODE );
   arg_idx = n->ast_idx_children_sz - 1;

   while( 0 <= arg_idx ) {
      
      retval = mlisp_stack_pop( exec, &stack_n_arg );
      maug_cleanup_if_not_ok();

      ast_n_arg = mdata_vector_get(
         &(parser->ast), n->ast_idx_children[arg_idx],
         struct MLISP_AST_NODE );

      /* Pull out the arg name from the strpool so we can call env_set(). */
      key_tmp_h = mdata_strpool_extract(
         &(parser->strpool), ast_n_arg->token_idx );
      /* TODO: Handle this gracefully. */
      assert( NULL != key_tmp_h );

      maug_mlock( key_tmp_h, key_tmp );
      maug_cleanup_if_null_lock( char*, key_tmp );

      retval = mlisp_env_set(
         parser, exec, key_tmp, 0, stack_n_arg.type, &(stack_n_arg.value),
         NULL, 0 );
      maug_cleanup_if_not_ok();

      maug_munlock( key_tmp_h, key_tmp );
      maug_mfree( key_tmp_h );

      arg_idx--;
   }

cleanup:

   if( NULL != key_tmp ) {
      maug_munlock( key_tmp_h, key_tmp );
   }

   if( NULL != key_tmp_h ) {
      maug_mfree( key_tmp_h );
   }

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_reset_child_pcs(
   struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_child_idx = NULL;
   size_t* p_visit_ct = NULL;
   struct MLISP_AST_NODE* n = NULL;
   size_t i = 0;

   assert( mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   assert( mdata_vector_is_locked( &(parser->ast) ) );

   /* Perform the actual reset. */
   debug_printf( MLISP_TRACE_LVL, "resetting PC on node: " SIZE_T_FMT, n_idx );
   p_child_idx = mdata_vector_get( &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_child_idx );
   *p_child_idx = 0;

   debug_printf( MLISP_TRACE_LVL,
      "resetting visit count on node: " SIZE_T_FMT, n_idx );
   p_visit_ct = mdata_vector_get( &(exec->per_node_visit_ct), n_idx, size_t );
   assert( NULL != p_visit_ct );
   *p_visit_ct = 0;

   n = mdata_vector_get( &(parser->ast), n_idx, struct MLISP_AST_NODE );

   /* Call reset on all children. */
   for( i = 0 ;  n->ast_idx_children_sz > i ; i++ ) {
      retval = _mlisp_reset_child_pcs( parser, n->ast_idx_children[i], exec );
      maug_cleanup_if_not_ok();
   }

cleanup:
 
   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_reset_lambda(
   struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t ret_idx = 0;

   debug_printf( MLISP_EXEC_TRACE_LVL,
      "resetting lambda " SIZE_T_FMT "...", n_idx );

   assert( mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   assert( !mdata_vector_is_locked( &(exec->env) ) );

   /* Clear off lambda stack args. */
   ret_idx = _mlisp_env_prune_args( exec );
   if( 0 > ret_idx ) {
      retval = ret_idx * -1;
   }
   maug_cleanup_if_not_ok();

   /* Reset per-node program counters. */
   retval = _mlisp_reset_child_pcs( parser, n_idx, exec );

cleanup:

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_step_lambda(
   struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_lambda_child_idx = NULL;
   size_t* p_args_child_idx = NULL;
   struct MLISP_AST_NODE* n = NULL;
   size_t* p_n_last_lambda = NULL;
   ssize_t append_retval = 0;

#ifdef MLISP_DEBUG_TRACE
   exec->trace[exec->trace_depth++] = n_idx;
   assert( exec->trace_depth <= MLISP_DEBUG_TRACE );
#endif /* MLISP_DEBUG_TRACE */

   /* n_idx is the node of this lambda. */
   mdata_vector_lock( &(exec->lambda_trace) );
   p_n_last_lambda = mdata_vector_get_last( &(exec->lambda_trace), size_t );
   mdata_vector_unlock( &(exec->lambda_trace) );
   if( NULL != p_n_last_lambda && n_idx == *p_n_last_lambda ) {
      /* This is a recursive call, so get rid of the lambda context so we can
       * replace it with a new one afterwards.
       */
      debug_printf( MLISP_EXEC_TRACE_LVL, "TRACE TAIL TIME!" );
      _mlisp_reset_lambda( parser, n_idx, exec );
      retval = mdata_vector_remove_last( &(exec->lambda_trace) );
      maug_cleanup_if_not_ok();
   }

   debug_printf( MLISP_EXEC_TRACE_LVL,
      "xvxvxvxvxvxvx STEP LAMBDA " SIZE_T_FMT " xvxvxvxvxvx", n_idx );

   /* Note that we passed through this lambda to detect tail calls later. */
   append_retval = mdata_vector_append(
      &(exec->lambda_trace), &n_idx, sizeof( size_t ) );
   retval = mdata_retval( append_retval );
   maug_cleanup_if_not_ok();

   /* Grab the current exec index for the child vector for this node. */
   assert( mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   p_lambda_child_idx = mdata_vector_get(
      &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_lambda_child_idx );
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "child idx for lambda AST node " SIZE_T_FMT ": " SIZE_T_FMT,
      n_idx, *p_lambda_child_idx );

   n = mdata_vector_get( &(parser->ast), n_idx, struct MLISP_AST_NODE );
   
   /* There needs to be an arg node and an exec node. */
   /* TODO: Handle this gracefully. */
   assert( 1 < n->ast_idx_children_sz );

   if( 0 == *p_lambda_child_idx ) {
      /* Parse the args passed to this lambda into the env, temporarily. */

      /* Get the current args node child index. */
      assert( mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
      p_args_child_idx = mdata_vector_get(
         &(exec->per_node_child_idx),
         n->ast_idx_children[*p_lambda_child_idx], size_t );
      assert( NULL != p_args_child_idx );
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "child idx for args AST node " SIZE_T_FMT ": " SIZE_T_FMT,
         *p_lambda_child_idx, *p_args_child_idx );

      if( 0 == *p_args_child_idx ) {
         /* Set return call in env before first arg, in *before-arg* delimiter,
          * so the args can be stripped off later when we return. */
         retval = mlisp_env_set(
            parser, exec, "$ARGS_S$", 0, MLISP_TYPE_ARGS_S, &n_idx, NULL, 0 );
         maug_cleanup_if_not_ok();
      }

      /* Pop stack into args in the env. */
      retval = _mlisp_step_lambda_args(
         parser, n->ast_idx_children[*p_lambda_child_idx], exec );

      if( MERROR_OK == retval ) {
         /* Set *after-arg* delimiter in env after last arg. */
         retval = mlisp_env_set(
            parser, exec, "$ARGS_E$", 0, MLISP_TYPE_ARGS_E, &n_idx, NULL, 0 );
         maug_cleanup_if_not_ok();

         /* Increment child idx so we call the exec child on next heartbeat. */
         (*p_lambda_child_idx)++;
         debug_printf( MLISP_EXEC_TRACE_LVL,
            "incremented " SIZE_T_FMT " child idx to: " SIZE_T_FMT,
            n_idx, *p_lambda_child_idx );
      }

      /* Set the error to MERROR_PREEMPT so that caller knows this lambda isn't
       * finished executing.
       */
      retval = MERROR_PREEMPT;

   } else if( mlisp_ast_has_ready_children( *p_lambda_child_idx, n ) ) {
      /* Dive into first lambda child until we no longer can. */

      retval = _mlisp_step_iter(
         parser, n->ast_idx_children[*p_lambda_child_idx], exec );

      if( MERROR_OK == retval ) {
         retval = _mlisp_preempt(
            "lambda", parser, n_idx, exec, p_lambda_child_idx,
            (*p_lambda_child_idx) + 1 );
      }

   } else {
      /* No more children to execute! */
      _mlisp_reset_lambda( parser, n_idx, exec );
   }

   /* TODO: If MERROR_PREEMPT is not returned, remove args_s and args_e? */

cleanup:

   debug_printf( MLISP_EXEC_TRACE_LVL,
      "xvxvxvxvxvxvx END STEP LAMBDA " SIZE_T_FMT " xvxvxvxvxvx", n_idx );

   /* Cleanup the passthrough note for this heartbeat. */
   mdata_vector_remove_last( &(exec->lambda_trace) );

   return retval;
}

/* === */

/* === */

static MERROR_RETVAL _mlisp_stack_cleanup(
   struct MLISP_PARSER* parser, size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;
   struct MLISP_STACK_NODE o;

   /* Pop elements off the stack until we hit the matching begin frame. */
   i = mdata_vector_ct( &(exec->stack) ) - 1;
   while( 0 <= i ) {
      
      retval = mlisp_stack_pop( exec, &o );
      maug_cleanup_if_not_ok();

      if( MLISP_TYPE_BEGIN == o.type && n_idx == o.value.begin ) {
         break;
      }

      i--;
   }

cleanup:

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_eval_token_strpool(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   size_t token_idx, size_t token_sz, struct MLISP_ENV_NODE* e_out
) {
   uint8_t autolock = 0;
   char* strpool = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_ENV_NODE* p_e = NULL;

   if( !mdata_vector_is_locked( &(exec->env) ) ) {
      mdata_vector_lock( &(exec->env) );
      autolock = 1;
   }

   mdata_strpool_lock( &(parser->strpool), strpool );
   
   assert( 0 < maug_strlen( &(strpool[token_idx]) ) );

   debug_printf( MLISP_EXEC_TRACE_LVL,
      "eval token: \"%s\" (maug_strlen: " SIZE_T_FMT ")",
      &(strpool[token_idx]), maug_strlen( &(strpool[token_idx]) ) );
   if( 0 == strncmp( &(strpool[token_idx]), "begin", token_sz ) ) {
      /* Fake env node e to signal step_iter() to place/cleanup stack frame. */
      e_out->type = MLISP_TYPE_BEGIN;

   } else if( NULL != (p_e = mlisp_env_get_strpool(
      parser, exec, strpool, token_idx, token_sz
   ) ) ) {
      /* A literal found in the environment. */
      debug_printf( MLISP_EXEC_TRACE_LVL, "found %s in env!",
         &(strpool[p_e->name_strpool_idx]) );

      /* Copy onto native stack so we can unlock env in case this is a
       * callback that needs to execute. */
      memcpy( e_out, p_e, sizeof( struct MLISP_ENV_NODE ) );
      p_e = NULL;

   } else if( maug_is_num( &(strpool[token_idx]), token_sz ) ) {
      /* Fake env node e from a numeric literal. */
      e_out->value.integer = atoi( &(strpool[token_idx]) );
      e_out->type = MLISP_TYPE_INT;

   } else if( maug_is_float( &(strpool[token_idx]), token_sz ) ) {
      /* Fake env node e from a floating point numeric literal. */
      e_out->value.floating = atof( &(strpool[token_idx]) );
      e_out->type = MLISP_TYPE_FLOAT;

   }

cleanup:

   if( autolock ) {
      mdata_vector_unlock( &(exec->env) );
   }

   mdata_strpool_unlock( &(parser->strpool), strpool );

   return retval;
}

static MERROR_RETVAL _mlisp_step_iter(
   struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_ENV_NODE e;
   struct MLISP_AST_NODE* n = NULL;
   size_t* p_visit_ct = NULL;

#ifdef MLISP_DEBUG_TRACE
   exec->trace[exec->trace_depth++] = n_idx;
   assert( exec->trace_depth <= MLISP_DEBUG_TRACE );
#endif /* MLISP_DEBUG_TRACE */

   n = mdata_vector_get( &(parser->ast), n_idx, struct MLISP_AST_NODE );

   assert( mdata_vector_is_locked( &(exec->per_node_visit_ct) ) );
   p_visit_ct = mdata_vector_get(
      &(exec->per_node_visit_ct), n_idx, size_t );
   assert( NULL != p_visit_ct );
   (*p_visit_ct)++;
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "visit count for AST node " SIZE_T_FMT ": " SIZE_T_FMT,
      n_idx, *p_visit_ct );

   /* Push a stack frame marker on the first visit to a BEGIN node. */
   if(
      MLISP_AST_FLAG_BEGIN == (MLISP_AST_FLAG_BEGIN & n->flags) &&
      1 == *p_visit_ct
   ) {
      /* Push a stack frame on first visit. */
      _mlisp_stack_push_mlisp_begin_t( exec, n_idx );
   }

   if(
      MERROR_OK !=
      (retval = _mlisp_step_iter_children( parser, n_idx, exec ))
   ) {
      goto cleanup;
   }

   /* Check for special types like lambda, that are lazily evaluated. */
   if( MLISP_AST_FLAG_LAMBDA == (MLISP_AST_FLAG_LAMBDA & n->flags) ) {
      /* Push the lambda to the stack so that the "define" above it can
       * grab it and associate it with the env.
       */
      mlisp_stack_push( exec, n_idx, mlisp_lambda_t );
      goto cleanup;
   }
 
   /* Now that the children have been evaluated above, evaluate this node.
    * Assume all the previously called children are now on the stack.
    */

   /* Grab the token for this node and figure out what it is. */

   retval = _mlisp_eval_token_strpool(
      parser, exec, n->token_idx, n->token_sz, &e );
   maug_cleanup_if_not_ok();

   /* Prepare to step. */

   /* Put the token or its result (if callable) on the stack. */
#  define _MLISP_TYPE_TABLE_ENVE( idx, ctype, name, const_name, fmt ) \
   } else if( MLISP_TYPE_ ## const_name == e.type ) { \
      _mlisp_stack_push_ ## ctype( exec, e.value.name );

   if( MLISP_TYPE_BEGIN == e.type ) {
      /* Cleanup the stack that's been pushed by children since this BEGIN's
       * initial visit.
       */
      retval = _mlisp_stack_cleanup( parser, n_idx, exec );

   } else if( MLISP_TYPE_CB == e.type ) {
      /* This is a special case... rather than pushing the callback, *execute*
       * it and let it push its result to the stack. This will create a 
       * redundant case below, but that can't be helped...
       */
      retval = e.value.cb( parser, exec, n_idx, e.cb_data, e.flags );

   } else if( MLISP_TYPE_LAMBDA == e.type ) {
      /* Create a "portal" into the lambda. The execution chain stays pointing
       * to this lambda-call node, but _mlisp_step_lambda() returns
       * MERROR_PREEMPT up the chain for subsequent heartbeats, until lambda is
       * done.
       */
      retval = _mlisp_step_lambda( parser, e.value.lambda, exec );

   MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_ENVE )
   } else {
      _mlisp_stack_push_mdata_strpool_idx_t( exec, n->token_idx );
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mlisp_step(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
#ifdef MLISP_DEBUG_TRACE
   size_t i = 0;
   char trace_str[MLISP_DEBUG_TRACE * 5];
   maug_ms_t ms_start = 0;
   maug_ms_t ms_end = 0;

   ms_start = retroflat_get_ms();
#endif /* MLISP_DEBUG_TRACE */

   debug_printf( MLISP_EXEC_TRACE_LVL, "heartbeat start" );

   /* These can remain locked for the whole step, as they're never added or
    * removed.
    */
   assert( !mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   assert( !mdata_vector_is_locked( &(exec->per_node_visit_ct) ) );
   assert( !mdata_vector_is_locked( &(parser->ast) ) );
   mdata_vector_lock( &(exec->per_node_child_idx) );
   mdata_vector_lock( &(exec->per_node_visit_ct) );
   mdata_vector_lock( &(parser->ast) );

   exec->flags = 0;
   assert( 0 == mdata_vector_ct( &(exec->lambda_trace) ) );

#ifdef MLISP_DEBUG_TRACE
   exec->trace_depth = 0;
#endif /* MLISP_DEBUG_TRACE */

   /* Find next unevaluated symbol. */
   retval = _mlisp_step_iter( parser, 0, exec );
   if( MERROR_PREEMPT == retval ) {
      /* There's still more to execute. */
      retval = MERROR_OK;
   } else if( MERROR_OK == retval ) {
      /* The last node executed completely. */
      debug_printf( MLISP_EXEC_TRACE_LVL, "execution terminated successfully" );
      retval = MERROR_EXEC; /* Signal the caller: we're out of instructions! */
   } else {
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "execution terminated with retval: %d", retval );
   }

#ifdef MLISP_DEBUG_TRACE
   ms_end = retroflat_get_ms();

   maug_mzero( trace_str, MLISP_DEBUG_TRACE * 5 );
   for( i = 0 ; exec->trace_depth > i ; i++ ) {
      maug_snprintf(
         &(trace_str[maug_strlen( trace_str )]),
         (MLISP_DEBUG_TRACE * 5) - maug_strlen( trace_str ),
         SIZE_T_FMT ", ", exec->trace[i] );
   }
   debug_printf( MLISP_EXEC_TRACE_LVL,
      MLISP_TRACE_SIGIL " HBEXEC (%u): %s",
      ms_end - ms_start, trace_str );
#endif /* MLISP_DEBUG_TRACE */
  
cleanup:

   debug_printf( MLISP_EXEC_TRACE_LVL, "heartbeat end: %x", retval );

   assert( mdata_vector_is_locked( &(parser->ast) ) );
   mdata_vector_unlock( &(parser->ast) );
   mdata_vector_unlock( &(exec->per_node_visit_ct) );
   mdata_vector_unlock( &(exec->per_node_child_idx) );

   return retval;
}

/* === */

MERROR_RETVAL mlisp_exec_init(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t append_retval = 0;
   size_t zero = 0;

   maug_mzero( exec, sizeof( struct MLISP_EXEC_STATE ) );

   /* Setup lambda visit stack so it can be locked on first step. */
   retval = mdata_vector_append(
      &(exec->lambda_trace), &zero, sizeof( size_t ) );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();
   mdata_vector_remove_last( &(exec->lambda_trace) );

   append_retval = mdata_vector_alloc(
      &(exec->env), sizeof( struct MLISP_ENV_NODE ), MDATA_VECTOR_INIT_SZ );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();

   /* Create the node PCs. */
   retval = mdata_vector_append(
      &(exec->per_node_child_idx), &zero, sizeof( size_t ) );
   maug_cleanup_if_not_ok();

   /* Make sure there's an exec child node for every AST node. */
   while(
      mdata_vector_ct( &(exec->per_node_child_idx) ) <= 
      mdata_vector_ct( &(parser->ast) )
   ) {
      retval = mdata_vector_append( &(exec->per_node_child_idx), &zero,
         sizeof( size_t ) );
   }

   /* Create the node visit counters. */
   retval = mdata_vector_append(
      &(exec->per_node_visit_ct), &zero, sizeof( size_t ) );
   maug_cleanup_if_not_ok();

   /* Make sure there's an exec visit count for every AST node. */
   while(
      mdata_vector_ct( &(exec->per_node_visit_ct) ) <= 
      mdata_vector_ct( &(parser->ast) )
   ) {
      retval = mdata_vector_append( &(exec->per_node_visit_ct), &zero,
         sizeof( size_t ) );
   }

   /* Setup initial env. */

   retval = mlisp_env_set(
      parser, exec, "if", 2, MLISP_TYPE_CB, _mlisp_env_cb_if,
      NULL, MLISP_ENV_FLAG_BUILTIN );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, "define", 6, MLISP_TYPE_CB, _mlisp_env_cb_define,
      NULL, MLISP_ENV_FLAG_BUILTIN );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, "*", 1, MLISP_TYPE_CB, _mlisp_env_cb_arithmetic,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_ARI_MUL );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, "+", 1, MLISP_TYPE_CB, _mlisp_env_cb_arithmetic,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_ARI_ADD );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, "<", 1, MLISP_TYPE_CB, _mlisp_env_cb_cmp,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_CMP_LT );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, ">", 1, MLISP_TYPE_CB, _mlisp_env_cb_cmp,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_CMP_GT );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, "=", 1, MLISP_TYPE_CB, _mlisp_env_cb_cmp,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_CMP_EQ );
   maug_cleanup_if_not_ok();

cleanup:

   if( MERROR_OK != retval ) {
      error_printf( "mlisp exec initialization failed: %d", retval );
   }

   return retval;
}

/* === */

void mlisp_exec_free( struct MLISP_EXEC_STATE* exec ) {
   mdata_vector_free( &(exec->per_node_child_idx) );
   mdata_vector_free( &(exec->per_node_visit_ct) );
   mdata_vector_free( &(exec->stack) );
   mdata_vector_free( &(exec->env) );
   mdata_vector_free( &(exec->lambda_trace) );
}

#else

#  define MLISP_PSTATE_TABLE_CONST( name, idx ) \
      extern MAUG_CONST uint8_t SEG_MCONST name;

MLISP_PARSER_PSTATE_TABLE( MLISP_PSTATE_TABLE_CONST )

#ifdef MPARSER_TRACE_NAMES
extern MAUG_CONST char* SEG_MCONST gc_mlisp_pstate_names[];
#endif /* MPARSER_TRACE_NAMES */

#endif /* MLISPE_C */

#endif /* !MLISPE_H */

