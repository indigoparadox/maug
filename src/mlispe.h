
#ifndef MLISPE_H
#define MLISPE_H

#include <mlisps.h>

/**
 * \addtogroup mlisp MLISP Interpreter
 * \{
 */

#ifndef MLISP_TOKEN_SZ_MAX
#  define MLISP_TOKEN_SZ_MAX 4096
#endif /* !MLISP_TOKEN_SZ_MAX */

#ifndef MLISP_EXEC_TRACE_LVL
#  define MLISP_EXEC_TRACE_LVL 0
#endif /* !MLISP_EXEC_TRACE_LVL */

#define MLISP_ENV_FLAG_BUILTIN   0x02

#define MLISP_ENV_FLAG_CMP_GT    0x10

#define MLISP_ENV_FLAG_CMP_LT    0x20

#define MLISP_ENV_FLAG_CMP_EQ    0x40

#define MLISP_ENV_FLAG_ARI_ADD   0x10

#define MLISP_ENV_FLAG_ARI_MUL   0x20

/**
 * \addtogroup mlisp_stack MLISP Execution Stack
 * \{
 */

#define mlisp_stack_push( exec, i, ctype ) \
   (_mlisp_stack_push_ ## ctype( exec, (ctype)i ))

MERROR_RETVAL mlisp_stack_dump(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

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
   uint8_t flags );

MERROR_RETVAL mlisp_step(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

/*! \} */ /* mlisp */

#ifdef MLISPE_C

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
         debug_printf( MLISP_TRACE_LVL, \
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (" #const_name "): " fmt, \
            i, n_stack->value.name );

   mdata_vector_lock( &(exec->stack) );
   mdata_strpool_lock( &(parser->strpool), strpool ); \
   while( i < mdata_vector_ct( &(exec->stack) ) ) {
      n_stack = mdata_vector_get( &(exec->stack), i, struct MLISP_STACK_NODE );

      /* Handle special exceptions. */
      if( MLISP_TYPE_STR == n_stack->type ) {
         debug_printf( MLISP_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (STR): %s",
            i, &(strpool[n_stack->value.strpool_idx]) );

      } else if( MLISP_TYPE_CB == n_stack->type ) {
         debug_printf( MLISP_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (CB): %p",
            i, n_stack->value.cb );

      } else if( MLISP_TYPE_LAMBDA == n_stack->type ) {
         debug_printf( MLISP_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (LAMBDA): " SIZE_T_FMT,
               i, n_stack->value.lambda );

      } else if( MLISP_TYPE_ARGS_S == n_stack->type ) {
         debug_printf( MLISP_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (ARGS_S): " SIZE_T_FMT,
               i, n_stack->value.args_start );

      } else if( MLISP_TYPE_ARGS_E == n_stack->type ) {
         debug_printf( MLISP_TRACE_LVL,
            MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (ARGS_E): " SIZE_T_FMT,
               i, n_stack->value.args_end );

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
         debug_printf( MLISP_TRACE_LVL, \
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT \
               " \"%s\" (" #const_name "): " fmt, \
            i, &(strpool[e->name_strpool_idx]), e->value.name ); \

   mdata_strpool_lock( &(parser->strpool), strpool ); \
   mdata_vector_lock( &(exec->env) );
   while( i < mdata_vector_ct( &(exec->env) ) ) {
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
         debug_printf( MLISP_TRACE_LVL,
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT " \"%s\" (STR): %s",
            i, &(strpool[e->name_strpool_idx]),
            &(strpool[e->value.strpool_idx]) );

      } else if( MLISP_TYPE_CB == e->type ) {
         debug_printf( MLISP_TRACE_LVL,
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT " \"%s\" (CB): %p",
            i, &(strpool[e->name_strpool_idx]), e->value.cb );

      } else if( MLISP_TYPE_LAMBDA == e->type ) {
         debug_printf( MLISP_TRACE_LVL,
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT
               " \"%s\" (LAMBDA): " SIZE_T_FMT,
            i, &(strpool[e->name_strpool_idx]), e->value.lambda );

      } else if( MLISP_TYPE_ARGS_S == e->type ) {
         debug_printf( MLISP_TRACE_LVL,
            MLISP_TRACE_SIGIL " env " SIZE_T_FMT
               " \"%s\" (ARGS_S): " SIZE_T_FMT,
            i, &(strpool[e->name_strpool_idx]), e->value.args_start );

      } else if( MLISP_TYPE_ARGS_E == e->type ) {
         debug_printf( MLISP_TRACE_LVL,
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
   size_t i = 0;

   /* This requires env be locked before entrance! */
   /* TODO: Autolock? */

   while( exec->env.ct > i ) {
      node_test = mdata_vector_get( &(exec->env), i, struct MLISP_ENV_NODE );
      if( 0 == strncmp(
         &(strpool[node_test->name_strpool_idx]),
         &(strpool[token_strpool_idx]),
         token_strpool_sz + 1
      ) ) {
         node_out = node_test;
         break;
      }
      i++;
   }

   return node_out;
}

/* === */

MERROR_RETVAL mlisp_env_set(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* token, size_t token_sz, uint8_t env_type, void* data,
   uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_ENV_NODE e;
   ssize_t new_idx_out = -1;

   if( 0 == token_sz ) {
      token_sz = maug_strlen( token );
   }
   assert( 0 < token_sz );

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
      retval = MERROR_USR;
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

static
ssize_t _mlisp_env_prune_args( struct MLISP_EXEC_STATE* exec ) {
   ssize_t ret_idx = 0;
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;
   struct MLISP_ENV_NODE* e = NULL;
   size_t removed = 0;

   assert( 0 < mdata_vector_ct( &(exec->env) ) );

   mdata_vector_lock( &(exec->env) );

   for( i = mdata_vector_ct( &(exec->env) ) - 1; 0 <= i ; i-- ) {
      e = mdata_vector_get( &(exec->env), i, struct MLISP_ENV_NODE );

      if( MLISP_TYPE_ARGS_S != e->type ) {
         /* Hunt for the initial env arg separator. */
         continue;
      }

      debug_printf( MLISP_EXEC_TRACE_LVL,
         "found initial env arg separator " SSIZE_T_FMT " with ret: "
            SSIZE_T_FMT,
         i, e->value.args_start );

      ret_idx = e->value.args_start;

      while( MLISP_TYPE_ARGS_E != e->type ) {
         mdata_vector_unlock( &(exec->env) );
         retval = mdata_vector_remove( &(exec->env), i );
         maug_cleanup_if_not_ok();
         mdata_vector_lock( &(exec->env) );

         /* Refresh e based on what i *now* points to. */
         e = mdata_vector_get( &(exec->env), i, struct MLISP_ENV_NODE );

         removed++;
      }

      /* Remove the actual terminal separator. */
      mdata_vector_unlock( &(exec->env) );
      retval = mdata_vector_remove( &(exec->env), i );
      maug_cleanup_if_not_ok();
      mdata_vector_lock( &(exec->env) );

      debug_printf( MLISP_EXEC_TRACE_LVL,
         "removed " SIZE_T_FMT " args!", removed );

      /* If we made it this far, we're done! */
      break;
   }

cleanup:

   mdata_vector_unlock( &(exec->env) );

   if( MERROR_OK != retval ) {
      ret_idx = retval * -1;
   }

   return ret_idx;
}

/* === */

static
MERROR_RETVAL _mlisp_env_cb_cmp(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, uint8_t flags
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
      retval = MERROR_USR;
      goto cleanup;
   }

   retval = mlisp_stack_pop( exec, &tmp );
   maug_cleanup_if_not_ok();
   cur_int = &a_int;
   if( 0 ) {
   MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_CMP )
   } else {
      error_printf( "cmp: invalid type!" );
      retval = MERROR_USR;
      goto cleanup;
   }

   if( MLISP_ENV_FLAG_CMP_GT == (MLISP_ENV_FLAG_CMP_GT & flags) ) {
      truth = a_int > b_int;
   } else if( MLISP_ENV_FLAG_CMP_LT == (MLISP_ENV_FLAG_CMP_LT & flags) ) {
      truth = a_int < b_int;
   }
   /* TODO: = */

   retval = mlisp_stack_push( exec, truth, mlisp_bool_t );

cleanup:

   mdata_strpool_unlock( &(parser->strpool), strpool );


   return retval;
}

/* === */

static
MERROR_RETVAL _mlisp_env_cb_arithmetic(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, uint8_t flags
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
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "multiply: %d * " fmt, product, mults[i].value.name ); \
         if( MLISP_ENV_FLAG_ARI_ADD == (MLISP_ENV_FLAG_ARI_ADD & flags) ) { \
            product += mults[i].value.name; \
         } else if( \
            MLISP_ENV_FLAG_ARI_MUL == (MLISP_ENV_FLAG_ARI_MUL & flags) \
         ) { \
            product *= mults[i].value.name; \
         }

   for( i = 0 ; 2 > i ; i++ ) {
      retval = mlisp_stack_pop( exec, &(mults[i]) );
      maug_cleanup_if_not_ok();

      if( 0 ) {
      MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_ARI )
      } else {
         error_printf( "arithmetic: invalid type!" );
         retval = MERROR_USR;
         goto cleanup;
      }
   }

   retval = mlisp_stack_push( exec, product, int16_t );

cleanup:

   mdata_strpool_unlock( &(parser->strpool), strpool );


   return retval;
}

/* === */

static
MERROR_RETVAL _mlisp_env_cb_define(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, uint8_t flags
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
      parser, exec, key_tmp, 0, val.type, &(val.value), 0 );

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

/* Execution Functions */

/* === */

static MERROR_RETVAL _mlisp_step_iter(
   struct MLISP_PARSER* parser, struct MLISP_AST_NODE* n,
   size_t n_idx, struct MLISP_EXEC_STATE* exec );

/* === */

static MERROR_RETVAL _mlisp_step_iter_children(
   struct MLISP_PARSER* parser, struct MLISP_AST_NODE* n,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_child_idx = NULL;
   struct MLISP_AST_NODE* n_child = NULL;
   char* strpool = NULL;

   /* Grab the current exec index for the child vector for this node. */
   p_child_idx = mdata_vector_get(
      &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_child_idx );
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "child idx for AST node " SIZE_T_FMT ": " SIZE_T_FMT,
      n_idx, *p_child_idx );

   if(
      (
         MLISP_AST_FLAG_LAMBDA == (MLISP_AST_FLAG_LAMBDA & n->flags) ||
         MLISP_AST_FLAG_IF == (MLISP_AST_FLAG_IF & n->flags) 
      ) &&
      0 == *p_child_idx
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

   if( n->ast_idx_children_sz > *p_child_idx ) {
      /* Call the next uncalled child. */
      n_child = mdata_vector_get(
         &(parser->ast), n->ast_idx_children[*p_child_idx],
         struct MLISP_AST_NODE );
      retval = _mlisp_step_iter(
         parser, n_child, n->ast_idx_children[*p_child_idx], exec );
      if( MERROR_OK == retval ) {
         mdata_strpool_lock( &(parser->strpool), strpool );
         assert( 0 < maug_strlen( &(strpool[n->token_idx]) ) );
         debug_printf( MLISP_EXEC_TRACE_LVL,
            "eval step " SSIZE_T_FMT " under %s...",
            *p_child_idx, &(strpool[n->token_idx]) );
         mdata_strpool_unlock( &(parser->strpool), strpool );

         /* Increment this node, since the child actually executed. */
         (*p_child_idx)++;
         debug_printf( MLISP_EXEC_TRACE_LVL,
            "incremented " SIZE_T_FMT " child idx to: " SIZE_T_FMT,
            n_idx, *p_child_idx );

         /* Could not exec *this* node yet, so don't increment its parent. */
         retval = MERROR_PREEMPT;
      }
      goto cleanup;
   }

cleanup:

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_step_lambda_args(
   struct MLISP_PARSER* parser, struct MLISP_AST_NODE* n,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t arg_idx = 0;
   struct MLISP_STACK_NODE stack_n_arg;
   struct MLISP_AST_NODE* ast_n_arg = NULL;
   MAUG_MHANDLE key_tmp_h = NULL;
   char* key_tmp = NULL;

   /* Pop stack into args into the env. These are all the results of previous
    * evaluations, before the lambda call, so we can just grab them all in
    * one go!
    */

   while( n->ast_idx_children_sz > arg_idx ) {
      
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
         parser, exec, key_tmp, 0, stack_n_arg.type, &(stack_n_arg.value), 0 );
      maug_cleanup_if_not_ok();

      maug_munlock( key_tmp_h, key_tmp );
      maug_mfree( key_tmp_h );

      arg_idx++;
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

static MERROR_RETVAL _mlisp_step_lambda(
   struct MLISP_PARSER* parser, struct MLISP_AST_NODE* n,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_lambda_child_idx = NULL;
   size_t* p_args_child_idx = NULL;
   struct MLISP_AST_NODE* n_args = NULL;
   struct MLISP_AST_NODE* n_child = NULL;
   char* strpool = NULL;
   ssize_t ret_idx = 0;

#ifdef MLISP_DEBUG_TRACE
   exec->trace[exec->trace_depth++] = n_idx;
#endif /* MLISP_DEBUG_TRACE */

   /* TODO: Put args into env between args and arge node types. On finish,
    *       cycle through env in reverse and delete most recently added arg envs.
    */

   /* TODO: When searching env, search in reverse so most recent dupe args are
    *       hit first.
    */

   debug_printf( 1, "xvxvxvxvxvxvx STEP LAMBDA xvxvxvxvxvx" );

   /* Grab the current exec index for the child vector for this node. */
   p_lambda_child_idx = mdata_vector_get(
      &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_lambda_child_idx );
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "child idx for lambda AST node " SIZE_T_FMT ": " SIZE_T_FMT,
      n_idx, *p_lambda_child_idx );

   /* There needs to be an arg node and an exec node. */
   /* TODO: Handle this gracefully. */
   assert( 1 < n->ast_idx_children_sz );

   if( 0 == *p_lambda_child_idx ) {
      /* Parse the args passed to this lambda into the env, temporarily. */

      /* Get the current args node. */
      n_args = mdata_vector_get(
         &(parser->ast), n->ast_idx_children[*p_lambda_child_idx],
         struct MLISP_AST_NODE );

      /* Get the current args node child index. */
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
         /* TODO: Push the index of the CALLER, not the LAMBDA? */
         retval = mlisp_env_set(
            parser, exec, "$ARGS_S$", 0, MLISP_TYPE_ARGS_S, &n_idx, 0 );
         maug_cleanup_if_not_ok();
      }

      /* Pop stack into args in the env. */
      retval = _mlisp_step_lambda_args(
         parser, n_args, n->ast_idx_children[*p_lambda_child_idx], exec );

      if( MERROR_OK == retval ) {
         /* Set *after-arg* delimiter in env after last arg. */
         retval = mlisp_env_set(
            parser, exec, "$ARGS_E$", 0, MLISP_TYPE_ARGS_E, &n_idx, 0 );
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

   } else if( *p_lambda_child_idx >= n->ast_idx_children_sz ) {

      /* No more children to execute! */
      ret_idx = _mlisp_env_prune_args( exec );
      if( 0 > ret_idx ) {
         retval = ret_idx * -1;
      }

   } else {

      /* Dive into first lambda child until we no longer can. */

      n_child = mdata_vector_get(
         &(parser->ast), n->ast_idx_children[*p_lambda_child_idx],
         struct MLISP_AST_NODE );
      retval = _mlisp_step_iter(
         parser, n_child, n->ast_idx_children[*p_lambda_child_idx], exec );
      if( MERROR_OK == retval ) {
         mdata_strpool_lock( &(parser->strpool), strpool );
         assert( 0 < maug_strlen( &(strpool[n->token_idx]) ) );
         debug_printf( MLISP_EXEC_TRACE_LVL,
            "eval step " SSIZE_T_FMT " under %s...",
            *p_lambda_child_idx, &(strpool[n->token_idx]) );
         mdata_strpool_unlock( &(parser->strpool), strpool );

         /* Increment this node, since the child actually executed. */
         (*p_lambda_child_idx)++;
         debug_printf( MLISP_EXEC_TRACE_LVL,
            "incremented " SIZE_T_FMT " child idx to: " SIZE_T_FMT,
            n_idx, *p_lambda_child_idx );

         /* Could not exec *this* node yet, so don't increment its parent. */
         retval = MERROR_PREEMPT;
      }

   }

   /* TODO: If MERROR_PREEMPT is not returned, remove args_s and args_e. */

   debug_printf( 1, "xvxvxvxvxvxvx END STEP LAMBDA xvxvxvxvxvx" );

cleanup:

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_step_if(
   struct MLISP_PARSER* parser, struct MLISP_AST_NODE* n,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;

#ifdef MLISP_DEBUG_TRACE
   exec->trace[exec->trace_depth++] = n_idx;
#endif /* MLISP_DEBUG_TRACE */

   /* TODO: Reset parent child_idx_iter if tail-call achieved, otherwise
    *       dive in.
    */

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_step_iter(
   struct MLISP_PARSER* parser, struct MLISP_AST_NODE* n,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   char* strpool = NULL;
   struct MLISP_ENV_NODE* e_p = NULL;
   struct MLISP_ENV_NODE e;

#ifdef MLISP_DEBUG_TRACE
   exec->trace[exec->trace_depth++] = n_idx;
#endif /* MLISP_DEBUG_TRACE */

   if(
      MERROR_OK !=
      (retval = _mlisp_step_iter_children( parser, n, n_idx, exec ))
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
   mdata_strpool_lock( &(parser->strpool), strpool );
   mdata_vector_lock( &(exec->env) );
   assert( 0 < maug_strlen( &(strpool[n->token_idx]) ) );
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "eval node " SIZE_T_FMT ": \"%s\" [%x] (i: " SSIZE_T_FMT
         ", maug_strlen: " SIZE_T_FMT ")",
      n_idx, &(strpool[n->token_idx]), strpool[n->token_idx], n->token_idx,
         maug_strlen( &(strpool[n->token_idx]) ) );
   if( 0 == strncmp( &(strpool[n->token_idx]), "if", n->token_sz ) ) {
      /* Fake env node e to step_if below. */
      e.type = MLISP_TYPE_IF;

   } else if( NULL != (e_p = mlisp_env_get_strpool(
      parser, exec, strpool, n->token_idx, n->token_sz
   ) ) ) {
      /* A literal found in the environment. */
      debug_printf( MLISP_EXEC_TRACE_LVL, "found %s in env!",
         &(strpool[e_p->name_strpool_idx]) );

      /* Copy onto native stack so we can unlock env in case this is a
       * callback that needs to execute. */
      memcpy( &e, e_p, sizeof( struct MLISP_ENV_NODE ) );
      e_p = NULL;

   } else if( maug_is_num( &(strpool[n->token_idx]), n->token_sz ) ) {
      /* Fake env node e from a numeric literal. */
      e.value.integer = atoi( &(strpool[n->token_idx]) );
      e.type = MLISP_TYPE_INT;

   } else if( maug_is_float( &(strpool[n->token_idx]), n->token_sz ) ) {
      /* Fake env node e from a floating point numeric literal. */
      e.value.floating = atof( &(strpool[n->token_idx]) );
      e.type = MLISP_TYPE_FLOAT;
   }

   /* TODO: Handle undefined symbols with an error. */

   mdata_vector_unlock( &(exec->env) );
   mdata_strpool_unlock( &(parser->strpool), strpool );

   /* Put the token or its result (if callable) on the stack. */
#  define _MLISP_TYPE_TABLE_ENVE( idx, ctype, name, const_name, fmt ) \
   } else if( MLISP_TYPE_ ## const_name == e.type ) { \
      _mlisp_stack_push_ ## ctype( exec, e.value.name );

   if( MLISP_TYPE_IF == e.type ) {
      retval = _mlisp_step_if( parser, n, n_idx, exec );

   } else if( MLISP_TYPE_CB == e.type ) {
      /* This is a special case... rather than pushing the callback, *execute*
       * it and let it push its result to the stack. This will create a 
       * redundant case below, but that can't be helped...
       */
      retval = e.value.cb( parser, exec, e.flags );
      maug_cleanup_if_not_ok();

   } else if( MLISP_TYPE_LAMBDA == e.type ) {
      /* Create a "portal" into the lambda. The execution chain stays pointing
       * to this lambda-call node, but _mlisp_step_lambda() returns
       * MERROR_PREEMPT up the chain for subsequent heartbeats, until lambda is
       * done.
       */
      retval = _mlisp_step_lambda( parser,
         mdata_vector_get(
            &(parser->ast), e.value.lambda, struct MLISP_AST_NODE ),
         e.value.lambda, exec );

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
   struct MLISP_AST_NODE* n = NULL;
   size_t zero = 0;
   size_t* p_child_idx = NULL;
#ifdef MLISP_DEBUG_TRACE
   size_t i = 0;
   char trace_str[MLISP_DEBUG_TRACE * 5];
   maug_ms_t ms_start = 0;
   maug_ms_t ms_end = 0;

   ms_start = retroflat_get_ms();
#endif /* MLISP_DEBUG_TRACE */

   if( 0 == mdata_vector_ct( &(exec->per_node_child_idx) ) ) {
      debug_printf( MLISP_EXEC_TRACE_LVL, "creating exec state..." );
      retval = mdata_vector_append(
         &(exec->per_node_child_idx), &zero, sizeof( size_t ) );
      maug_cleanup_if_not_ok();
   }

   debug_printf( MLISP_EXEC_TRACE_LVL, "heartbeat start" );

   /* Make sure there's an exec child node for every AST node. */
   while(
      mdata_vector_ct( &(exec->per_node_child_idx) ) <= 
      mdata_vector_ct( &(parser->ast) )
   ) {
      retval = mdata_vector_append( &(exec->per_node_child_idx), &zero,
         sizeof( size_t ) );
   }

   mdata_vector_lock( &(parser->ast) );
   mdata_vector_lock( &(exec->per_node_child_idx) );

   p_child_idx = mdata_vector_get( &(exec->per_node_child_idx), 0, size_t );
   assert( NULL != p_child_idx );

   n = mdata_vector_get( &(parser->ast), 0, struct MLISP_AST_NODE );
   assert( NULL != n );

#ifdef MLISP_DEBUG_TRACE
   exec->trace_depth = 0;
#endif /* MLISP_DEBUG_TRACE */

   /* Find next unevaluated symbol. */
   retval = _mlisp_step_iter( parser, n, 0, exec );
   if( MERROR_PREEMPT == retval ) {
      retval = MERROR_OK;
   } else if( MERROR_OK == retval ) {
      retval = MERROR_EXEC;
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
   debug_printf( 1,
      MLISP_TRACE_SIGIL " HBEXEC (%u): %s",
      ms_end - ms_start, trace_str );
#endif /* MLISP_DEBUG_TRACE */
  
cleanup:

   debug_printf( MLISP_EXEC_TRACE_LVL, "heartbeat end: %x", retval );

   mdata_vector_unlock( &(exec->per_node_child_idx) );
   mdata_vector_unlock( &(parser->ast) );

   return retval;
}

/* === */

MERROR_RETVAL mlisp_exec_init(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t append_retval = 0;

   maug_mzero( exec, sizeof( struct MLISP_EXEC_STATE ) );

   append_retval = mdata_vector_alloc(
      &(exec->env), sizeof( struct MLISP_ENV_NODE ), MDATA_VECTOR_INIT_SZ );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();

   /* Setup initial env. */

   retval = mlisp_env_set(
      parser, exec, "define", 6, MLISP_TYPE_CB, _mlisp_env_cb_define,
      MLISP_ENV_FLAG_BUILTIN );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, "*", 1, MLISP_TYPE_CB, _mlisp_env_cb_arithmetic,
      MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_ARI_MUL );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, "+", 1, MLISP_TYPE_CB, _mlisp_env_cb_arithmetic,
      MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_ARI_ADD );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, "<", 1, MLISP_TYPE_CB, _mlisp_env_cb_cmp,
      MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_CMP_LT );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, ">", 1, MLISP_TYPE_CB, _mlisp_env_cb_cmp,
      MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_CMP_GT );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, exec, "=", 1, MLISP_TYPE_CB, _mlisp_env_cb_cmp,
      MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_CMP_EQ );
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
   mdata_vector_free( &(exec->stack) );
   mdata_vector_free( &(exec->env) );
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

