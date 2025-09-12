
#ifndef MLISPE_H
#define MLISPE_H

#include <mlisps.h>

/**
 * \addtogroup mlisp MLISP Interpreter
 * \{
 * \file mlispe.h
 * \brief MLISP Interpreter Functions and Macros
 */

/**
 * \addtogroup maug_mlisp_commands MLISP Commands
 * \{
 * \page maug_mlisp_commands_page Command Definitions
 * The mlisp interpreter provides the following commands:
 *
 * * *define:* Define a variable in the environment local to the current
 *             ::MLISP_EXEC_STATE.
 * * *gdefine:* Define a variable in the global environment set by
 *              mlisp_exec_set_global_env().
 * * *and:* True if all of an arbitrary number of arguments are true.
 * * *or:* True if any of an arbitrary number of arguments are true.
 * * *<:* True if the first argument is less than the second.
 * * *>:* True if the first argument is greater than the second.
 * * *=:* True if the first argument is equal to the second.
 * * *if:* If the condition defined by the first argument is true, then execute
 *         the second argument. Otherwise, execute the optional third argument.
 * * **:* Multiply an arbitrary number of arguments and push the result on the
 *        stack.
 * * *+:* Sum an arbitrary number of arguments and push the result on the
 *        stack.
 * * *\/:* Divide an arbitrary number of arguments and push the result on the
 *         stack.
 * * *%:* Divide an arbitrary number of arguments and push the remainder on the
 *         stack.
 * * *random:* A constant that will always return a different random number.
 * \}
 */

#ifndef MLISP_TOKEN_SZ_MAX
#  define MLISP_TOKEN_SZ_MAX 4096
#endif /* !MLISP_TOKEN_SZ_MAX */

#ifndef MLISP_EXEC_TRACE_LVL
#  define MLISP_EXEC_TRACE_LVL 0
#endif /* !MLISP_EXEC_TRACE_LVL */

#ifndef MLISP_STACK_TRACE_LVL
#  define MLISP_STACK_TRACE_LVL 0
#endif /* !MLISP_STACK_TRACE_LVL */

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

#define MLISP_ENV_FLAG_ARI_DIV   0x40

#define MLISP_ENV_FLAG_ARI_MOD   0x80

#define MLISP_ENV_FLAG_ANO_OR    0x10

#define MLISP_ENV_FLAG_ANO_AND   0x20

/*! \brief Flag for _mlisp_env_cb_define() specifying global env. */
#define MLISP_ENV_FLAG_DEFINE_GLOBAL   0x10

/**
 * \addtogroup mlisp_stack MLISP Execution Stack
 * \{
 */

/**
 * \brief Flag for mlisp_stack_pop_ex() indicating the value should not be
 *        removed from the stack.
 */
#define MLISP_STACK_FLAG_PEEK    0x01

/**
 * \brief Wrapper for mlisp_stack_pop() with no flags.
 */
#define mlisp_stack_pop( exec, o ) mlisp_stack_pop_ex( exec, o, 0 )

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

#if defined( MLISP_DUMP_ENABLED ) || defined( DOCUMENTATION )

/**
 * \brief Dump the stack from the given parser/exec combination.
 * \warning This is only available if MLISP_DUMP_ENABLED is defined at compile
 *          time!
 */
MERROR_RETVAL mlisp_stack_dump(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

#endif /* MLISP_DUMP_ENABLED || DOCUMENTATION */

/**
 * \brief Pop a value off of (removing from) MLISP_EXEC_STATE::stack and copy
 *        it to a provided output.
 *
 * \warning MLISP_EXEC_STATE::stack should be *unlocked* prior to calling!
 */
MERROR_RETVAL mlisp_stack_pop_ex(
   struct MLISP_EXEC_STATE* exec, struct MLISP_STACK_NODE* o, uint8_t flags );

MERROR_RETVAL mlisp_stack_peek(
   struct MLISP_EXEC_STATE* exec, struct MLISP_STACK_NODE* o );

/*! \} */ /* mlisp_stack */

#if defined( MLISP_DUMP_ENABLED ) || defined( DOCUMENTATION )

/**
 * \brief Dump the environment from the given parser/exec combination.
 * \warning This is only available if MLISP_DUMP_ENABLED is defined at compile
 *          time!
 */
MERROR_RETVAL mlisp_env_dump(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, uint8_t global );

#endif /* MLISP_DUMP_ENABLED || DOCUMENTATION */

struct MLISP_ENV_NODE* mlisp_env_get(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* key );

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

MERROR_RETVAL mlisp_env_unset(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* token, size_t token_sz );

MERROR_RETVAL mlisp_env_set(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* token, size_t token_sz, uint8_t env_type, const void* data,
   void* cb_data, uint8_t flags );

ssize_t mlisp_count_builtins(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

MERROR_RETVAL mlisp_check_state(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

/**
 * \brief Iterate the current exec_state() starting from the next MLISP_AST_NODE
 *        to be executed according to the tree of
 *        MLISP_EXEC_STATE::per_node_child_idx values.
 */
MERROR_RETVAL mlisp_step(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

/**
 * \brief Iterate the current exec_state() starting from the lambda named.
 *
 * This should be called repeatedly in place of mlisp_step() until
 * MERROR_PREEMPT is no longer returned so that the lambda is able to complete
 * its execution.
 *
 * \warning The script should be run normally with mlisp_step() until it
 *          returns MERROR_EXEC, so that lambdas defined by the script are
 *          present in the env to be stepped from!
 */
MERROR_RETVAL mlisp_step_lambda(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* lambda );

MERROR_RETVAL mlisp_exec_add_env_builtins(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

MERROR_RETVAL mlisp_exec_init(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, uint8_t flags );

/**
 * \brief Set the given exec state to use the given vector as a global variable
 *        environment. This can be long-lived and shared with other exec states.
 */
MERROR_RETVAL mlisp_exec_set_global_env(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   struct MDATA_VECTOR* global_env );

void mlisp_exec_free( struct MLISP_EXEC_STATE* exec );

MERROR_RETVAL mlisp_deserialize_prepare_EXEC_STATE(
   struct MLISP_EXEC_STATE* exec, size_t i );

#define _MLISP_TYPE_TABLE_PUSH_PROTO( idx, ctype, name, const_name, fmt ) \
   MERROR_RETVAL _mlisp_stack_push_ ## ctype( \
      struct MLISP_EXEC_STATE* exec, ctype i );

MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_PUSH_PROTO )

/*! \} */ /* mlisp */

#define mlisp_ast_has_ready_children( exec_child_idx, n ) \
   ((exec_child_idx) < (n)->ast_idx_children_sz)

#ifdef MLISPE_C

uint16_t g_mlispe_last_uid = 0;

/**
 * \brief Increment/decrement/otherwise set the child node pointer (analogous
 *        to a per-AST-node cascading program counter attached to
 *        MLISP_EXEC_STATE) for a parent node to the requested value.
 * \param retval Return value of the last iterated statement of a parent node.
 * \param caller Brief readable description of the parent node.
 * \param n_idx AST node index of the parent node.
 * \param new_idx Child node index to set to if retval is MERROR_OK.
 * \return New retval with verdect on whether to preempt or not.
 */
static MERROR_RETVAL _mlisp_preempt(
   MERROR_RETVAL retval, const char* caller, struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec, size_t new_idx );

static MERROR_RETVAL _mlisp_step_iter(
   struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec );

static MERROR_RETVAL _mlisp_reset_child_pcs(
   struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec );

/* === */

/* Stack Functions */

/* === */

#ifdef MLISP_DUMP_ENABLED

MERROR_RETVAL mlisp_stack_dump(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   struct MLISP_STACK_NODE* n_stack = NULL;

#  define _MLISP_TYPE_TABLE_DUMPS( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == n_stack->type ) { \
         debug_printf( MLISP_STACK_TRACE_LVL, \
            "%u: " MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (" #const_name "): " fmt, \
            exec->uid, i, n_stack->value.name );

   mdata_vector_lock( &(exec->stack) );
   mdata_strpool_lock( &(parser->strpool) ); \
   while( i < mdata_vector_ct( &(exec->stack) ) ) {
      n_stack = mdata_vector_get( &(exec->stack), i, struct MLISP_STACK_NODE );

      /* Handle special exceptions. */
      if( MLISP_TYPE_STR == n_stack->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (STR): %s",
            exec->uid, i, mdata_strpool_get(
               &(parser->strpool), n_stack->value.strpool_idx ) );

      } else if( MLISP_TYPE_CB == n_stack->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (CB): %p",
            exec->uid, i, n_stack->value.cb );

      } else if( MLISP_TYPE_LAMBDA == n_stack->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (LAMBDA): "
               SIZE_T_FMT,
            exec->uid, i, n_stack->value.lambda );

      } else if( MLISP_TYPE_ARGS_S == n_stack->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (ARGS_S): "
               SIZE_T_FMT,
            exec->uid, i, n_stack->value.args_start );

      } else if( MLISP_TYPE_ARGS_E == n_stack->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (ARGS_E): "
               SIZE_T_FMT,
            exec->uid, i, n_stack->value.args_end );

      } else if( MLISP_TYPE_BEGIN == n_stack->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " stack " SIZE_T_FMT " (BEGIN): "
               SIZE_T_FMT,
            exec->uid, i, n_stack->value.begin );

      /* Handle numeric types. */
      MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_DUMPS );
      } else {
         error_printf( "invalid stack type: %u", n_stack->type );
      }
      i++;
   }
   mdata_strpool_unlock( &(parser->strpool) );
   mdata_vector_unlock( &(exec->stack) );

cleanup:

   assert( mdata_strpool_is_locked( &(parser->strpool) ) );

   return retval;
}

#endif /* MLISP_DUMP_ENABLED */

/* === */

#define _MLISP_TYPE_TABLE_PUSH( idx, ctype, name, const_name, fmt ) \
   MERROR_RETVAL _mlisp_stack_push_ ## ctype( \
      struct MLISP_EXEC_STATE* exec, ctype i \
   ) { \
      struct MLISP_STACK_NODE n_stack; \
      MERROR_RETVAL retval = MERROR_OK; \
      debug_printf( MLISP_STACK_TRACE_LVL, \
         "%u: pushing " #const_name " onto stack: " fmt, exec->uid, i ); \
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

MERROR_RETVAL mlisp_stack_pop_ex(
   struct MLISP_EXEC_STATE* exec, struct MLISP_STACK_NODE* o, uint8_t flags
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

#if MLISP_STACK_TRACE_LVL > 0
#  define _MLISP_TYPE_TABLE_POPD( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == o->type ) { \
         if( MLISP_STACK_FLAG_PEEK == (MLISP_STACK_FLAG_PEEK & flags) ) { \
            debug_printf( MLISP_STACK_TRACE_LVL, \
               "%u: peeking: " SSIZE_T_FMT ": " fmt, \
               exec->uid, n_idx, o->value.name ); \
         } else { \
            debug_printf( MLISP_STACK_TRACE_LVL, \
               "%u: popping: " SSIZE_T_FMT ": " fmt, \
               exec->uid, n_idx, o->value.name ); \
         }

   if( 0 ) {
   MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_POPD )
   }
#endif /* MLISP_STACK_TRACE_LVL */

   if( MLISP_STACK_FLAG_PEEK != (MLISP_STACK_FLAG_PEEK & flags) ) {
      retval = mdata_vector_remove( &(exec->stack), n_idx );
   }

cleanup:

   return retval;
}

/* === */

/* Env Functons */

/* === */

static struct MLISP_ENV_NODE* _mlisp_env_get_internal(
   struct MDATA_VECTOR* env, ssize_t key_idx, size_t key_sz
) {
   struct MLISP_ENV_NODE* e = NULL,
      * node_test = NULL;
   ssize_t i = 0;

   /* At the very least, the caller using this should be in the same lock
    * context as this search, since we're returning a pointer. So no autolock!
    */
   if( !mdata_vector_is_locked( env ) ) {
      error_printf( "env must be locked for shared context!" );
      goto cleanup;
   }

   /* Start from the end so that later definitions (in a deeper/more relevant
    * scope) are evaluated first.
    */
   i = mdata_vector_ct( env ) - 1;

   while( 0 <= i ) {
      node_test = mdata_vector_get( env, i, struct MLISP_ENV_NODE );
      if( NULL == node_test ) {
         error_printf( "invalid node!" );
         goto cleanup;
      }
      if(
         node_test->name_strpool_sz == key_sz &&
         node_test->name_strpool_idx == key_idx
      ) {
         e = node_test;
         break;
      }
      i--;
   }

cleanup:

   return e;
}

/* === */

static struct MDATA_VECTOR* _mlisp_env_select( struct MLISP_EXEC_STATE* exec ) {
   return &(exec->env);
}

/* === */

#if defined( MLISP_DUMP_ENABLED )

MERROR_RETVAL mlisp_env_dump(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, uint8_t global
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   struct MLISP_ENV_NODE* e = NULL;
   struct MDATA_VECTOR* env = NULL;

#  define _MLISP_TYPE_TABLE_DUMPE( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == e->type ) { \
         debug_printf( 1, \
            "%u: " MLISP_TRACE_SIGIL " env " SIZE_T_FMT \
               " \"%s\" (" #const_name "): " fmt, \
            exec->uid, i, mdata_strpool_get( \
               &(parser->strpool), e->name_strpool_idx ), e->value.name );

   if( global ) {
      env = exec->global_env;
   } else {
      env = _mlisp_env_select( exec );
   }

   if(
      NULL == env ||
      (MAUG_MHANDLE)NULL == env->data_h
   ) {
      error_printf( "no env present!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   /*
   if( (MAUG_MHANDLE)NULL == parser->strpool.str_h ) {
      error_printf( "no env strpool present!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }
   */

   mdata_strpool_lock( &(parser->strpool) );
   mdata_vector_lock( env );
   while( i < mdata_vector_ct( env ) ) {
      assert( mdata_vector_is_locked( env ) );
      e = mdata_vector_get( env, i, struct MLISP_ENV_NODE );
      assert( NULL != e );

      if( MLISP_ENV_FLAG_BUILTIN == (MLISP_ENV_FLAG_BUILTIN & e->flags) ) {
         /* Skip builtins. */
         i++;
         continue;
      }

      if( 0 ) {
      MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_DUMPE );
      /* Handle special exceptions. */
      } else if( MLISP_TYPE_STR == e->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " env " SIZE_T_FMT " \"%s\" (STR): %s",
            exec->uid, i, mdata_strpool_get(
               &(parser->strpool), e->name_strpool_idx ),
            mdata_strpool_get( &(parser->strpool), e->value.strpool_idx ) );

      } else if( MLISP_TYPE_CB == e->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " env " SIZE_T_FMT " \"%s\" (CB): %p",
            exec->uid, i, mdata_strpool_get(
               &(parser->strpool), e->name_strpool_idx ), e->value.cb );

      } else if( MLISP_TYPE_LAMBDA == e->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " env " SIZE_T_FMT
               " \"%s\" (LAMBDA): " SIZE_T_FMT,
            exec->uid, i, mdata_strpool_get(
               &(parser->strpool), e->name_strpool_idx ), e->value.lambda );

      } else if( MLISP_TYPE_ARGS_S == e->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " env " SIZE_T_FMT
               " \"%s\" (ARGS_S): " SIZE_T_FMT,
            exec->uid, i, mdata_strpool_get(
               &(parser->strpool), e->name_strpool_idx ), e->value.args_start );

      } else if( MLISP_TYPE_ARGS_E == e->type ) {
         debug_printf( 1,
            "%u: " MLISP_TRACE_SIGIL " env " SIZE_T_FMT
               " \"%s\" (ARGS_E): " SIZE_T_FMT,
            exec->uid, i, mdata_strpool_get(
               &(parser->strpool), e->name_strpool_idx ), e->value.args_end );

      } else {
         error_printf( MLISP_TRACE_SIGIL " invalid env type: %u", e->type );
      }
      i++;
   }
   mdata_vector_unlock( env );
   mdata_strpool_unlock( &(parser->strpool) );

cleanup:

   return retval;
}

#endif /* MLISP_DUMP_ENABLED */

/* === */

struct MLISP_ENV_NODE* mlisp_env_get(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, const char* key
) {
   struct MLISP_ENV_NODE* e = NULL;
   struct MDATA_VECTOR* env = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t key_sz = 0;
   ssize_t key_strpool_idx = 0;

   env = _mlisp_env_select( exec );

   /* At the very least, the caller using this should be in the same lock
    * context as this search, since we're returning a pointer. So no autolock!
    */
   assert( mdata_vector_is_locked( env ) );

   /* Find the key in the parser strpool. If it's not in there... it's not in
    * any env, since all the env keys are strpool indexes!
    */
   key_sz = strlen( key );
   key_strpool_idx = mdata_strpool_find(
      &(parser->strpool), key, key_sz );
   if( 0 > key_strpool_idx || 0 == key_sz ) {
      goto cleanup;
   }

   e = _mlisp_env_get_internal( env, key_strpool_idx, key_sz );
   if( NULL != e ) {
      /* Found something, so short-circuit! */
      goto cleanup;
   }

   /* Did not find anything in the local env, so try the global env if there
    * is one!
    */
   if( NULL != exec->global_env ) {
      assert( mdata_vector_is_locked( exec->global_env ) );
      e = _mlisp_env_get_internal( exec->global_env, key_strpool_idx, key_sz );
   }

cleanup:

   if( MERROR_OK != retval ) {
      e = NULL;
   }

   return e;
}

/* === */

struct MLISP_ENV_NODE* mlisp_env_get_strpool(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* strpool, size_t token_strpool_idx, size_t token_strpool_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_ENV_NODE* e = NULL;
   struct MDATA_VECTOR* env = NULL;

   env = _mlisp_env_select( exec );

   /* At the very least, the caller using this should be in the same lock
    * context as this search, since we're returning a pointer. So no autolock!
    */
   if( !mdata_vector_is_locked( env ) ) {
      error_printf( "env must be locked for shared context!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL, "%u: (strpool) checking env...",
      exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */

   e = _mlisp_env_get_internal( env, token_strpool_idx, token_strpool_sz );
   if( NULL != e ) {
      /* Found something, so short-circuit! */
      goto cleanup;
   }

   /* Did not find anything in the local env, so try the global env if there
    * is one!
    */
   if( NULL != exec->global_env ) {
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: (strpool) checking global env...",
         exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */

      if( !mdata_vector_is_locked( exec->global_env ) ) {
         error_printf( "global env must be locked for shared context!" );
         retval = MERROR_EXEC;
         goto cleanup;
      }
      e = _mlisp_env_get_internal(
         exec->global_env, token_strpool_idx, token_strpool_sz );
   }

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL, "%u: (strpool) found env: %p",
      exec->uid, e );
#endif /* MLISP_EXEC_TRACE_LVL */

cleanup:

   if( MERROR_OK != retval ) {
      e = NULL;
   }

   return e;
}

/* === */

static ssize_t _mlisp_env_unset_internal(
   struct MDATA_VECTOR* env, const char* token, size_t token_sz,
   char* strpool
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;
   struct MLISP_ENV_NODE* e = NULL;
   int autolock_env = 0;

   if( !mdata_vector_is_locked( env ) ) {
      mdata_vector_lock( env );
      autolock_env = 1;
   }

   /* Search for the given token in the env. */
   for( i = mdata_vector_ct( env ) - 1 ; 0 <= i ; i-- ) {
      e = mdata_vector_get( env, i, struct MLISP_ENV_NODE );

      if( MLISP_TYPE_ARGS_E == e->type ) {
#if MLISP_STACK_TRACE_LVL > 0
         debug_printf( MLISP_STACK_TRACE_LVL,
            "reached end of env stack frame: " SSIZE_T_FMT, i );
#endif /* MLISP_STACK_TRACE_LVL */
         goto cleanup;
      }

      if( 0 != strncmp(
         token, &(strpool[e->name_strpool_idx]), token_sz + 1 )
      ) {
         continue;
      }

      /* Remove the token. */
      mdata_vector_unlock( env );
      retval = mdata_vector_remove( env, i );
      mdata_vector_lock( env );
      goto cleanup;
   }

cleanup:

   if( autolock_env ) {
      mdata_vector_unlock( env );
   }

   return retval;
}

/* === */

MERROR_RETVAL mlisp_env_unset(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* token, size_t token_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MDATA_VECTOR* env = NULL;
   ssize_t rem_idx = 0;

   env = _mlisp_env_select( exec );

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: attempting to undefine %s...", exec->uid, token );
#endif /* MLISP_EXEC_TRACE_LVL */

   mdata_strpool_lock( &(parser->strpool) );

   /* XXX: Don't pass the strpool pointer! Redo the function prototype! */
   rem_idx = _mlisp_env_unset_internal(
      env, token, token_sz, parser->strpool.str_p );

   if( 0 <= rem_idx ) {
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: found and removed env token: %s (" SSIZE_T_FMT ")",
         exec->uid, token, rem_idx );
#endif /* MLISP_EXEC_TRACE_LVL */
   } else {
      retval = merror_sz_to_retval( rem_idx );
   }

cleanup:

   if( mdata_strpool_is_locked( &(parser->strpool) ) ) {
      mdata_strpool_unlock( &(parser->strpool) );
   }

   return retval;
}

/* === */

static ssize_t _mlisp_env_set_internal(
   struct MDATA_VECTOR* env, struct MDATA_STRPOOL* strpool_d, uint16_t uid,
   const char* token, size_t token_sz, uint8_t env_type, const void* data,
   void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_ENV_NODE e;
   ssize_t new_idx_out = -1;

   assert( NULL != env );
   assert( 0 < token_sz );

   assert( !mdata_vector_is_locked( env ) );

   mdata_strpool_lock( strpool_d );

   /* Find previous env nodes with same token and change. */
   /* Ignore the retval, since it doesn't really matter if this fails. */
   /* XXX: Don't pass the strpool pointer! Redo the function prototype! */
   _mlisp_env_unset_internal( env, token, token_sz, strpool_d->str_p );

   mdata_strpool_unlock( strpool_d );

#if MLISP_EXEC_TRACE_LVL > 0
#  define _MLISP_TYPE_TABLE_ASGN( idx, ctype, name, const_name, fmt ) \
      case idx: \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "%u: setting env: \"%s\": #" fmt, \
            uid, token, (ctype)*((ctype*)data) ); \
         e.value.name = *((ctype*)data); \
         break;
#else
#  define _MLISP_TYPE_TABLE_ASGN( idx, ctype, name, const_name, fmt ) \
      case idx: \
         e.value.name = *((ctype*)data); \
         break;
#endif /* MLISP_EXEC_TRACE_LVL */

   /* Setup the new node to copy. */
   maug_mzero( &e, sizeof( struct MLISP_ENV_NODE ) );
   e.flags = flags;
   e.name_strpool_idx = mdata_strpool_append(
      strpool_d, token, token_sz, MDATA_STRPOOL_FLAG_DEDUPE );
   e.name_strpool_sz = token_sz;
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
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: setting env: \"%s\": strpool(" SSIZE_T_FMT ")",
         uid, token, *((ssize_t*)data) );
#endif /* MLISP_EXEC_TRACE_LVL */
      e.value.strpool_idx = *((mdata_strpool_idx_t*)data);
      break;

   case 5 /* MLISP_TYPE_CB */:
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: setting env: \"%s\": 0x%p",
         uid, token, (mlisp_env_cb_t)data );
#endif /* MLISP_EXEC_TRACE_LVL */
      e.value.cb = (mlisp_env_cb_t)data;
      break;

   case 6 /* MLISP_TYPE_LAMBDA */:
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: setting env: \"%s\": node #" SSIZE_T_FMT,
         uid, token, *((mlisp_lambda_t*)data) );
#endif /* MLISP_EXEC_TRACE_LVL */
      e.value.lambda = *((mlisp_lambda_t*)data);
      break;

   case 7: /* MLISP_TYPE_ARGS_S */
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: setting env: \"%s\": node #" SSIZE_T_FMT,
         uid, token, *((mlisp_args_t*)data) );
#endif /* MLISP_EXEC_TRACE_LVL */
      e.value.args_start = *((mlisp_args_t*)data);
      break;

   case 8: /* MLISP_TYPE_ARGS_E */
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: setting env: \"%s\": node #" SSIZE_T_FMT,
         uid, token, *((mlisp_arge_t*)data) );
#endif /* MLISP_EXEC_TRACE_LVL */
      e.value.args_end = *((mlisp_arge_t*)data);
      break;

   case 10: /* MLISP_TYPE_BEGIN */
      /* We probably called a lambda that takes an arg without placing an
       * arg on the stack for it to take up!
       *
       * This could be a script error, but it could also be a lambda being
       * itered into after its finished executing (and thus has no arg on the
       * stack waiting for it).
       * 
       * MERROR_RESET signals the calling program we're embedded in to deal
       * with this situation, maybe by restarting the script with a fresh env.
       */
      error_printf(
         "%u: underflow %s: missing lambda arg?",
         uid, token );
      new_idx_out = merror_retval_to_sz( MERROR_RESET );
      goto cleanup;

   default:
      error_printf( "invalid type: %d", env_type );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   /* Add the node to the env. */
   if( MLISP_ENV_FLAG_BUILTIN == (MLISP_ENV_FLAG_BUILTIN & flags) ) {
      /* Put it at the beginning. */
      new_idx_out = mdata_vector_insert(
         env, &e, 0, sizeof( struct MLISP_ENV_NODE ) );
   } else {
      /* Put it at the end. */
      new_idx_out = mdata_vector_append(
         env, &e, sizeof( struct MLISP_ENV_NODE ) );
   }

cleanup:

   return new_idx_out;
}

/* === */

MERROR_RETVAL mlisp_env_set(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* token, size_t token_sz, uint8_t env_type, const void* data,
   void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t env_idx = 0;
   struct MDATA_VECTOR* env = NULL;

   env = _mlisp_env_select( exec );

   if( 0 == token_sz ) {
      token_sz = maug_strlen( token );
   }

   env_idx = _mlisp_env_set_internal(
      env, &(parser->strpool), exec->uid,
      token, token_sz, env_type, data, cb_data, flags );
   if( 0 > env_idx ) {
      retval = merror_sz_to_retval( env_idx );
      goto cleanup;
   }

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: setup env node " SSIZE_T_FMT ": %s",
      exec->uid, env_idx, token );
#endif /* MLISP_EXEC_TRACE_LVL */

cleanup:

   return retval;
}

/* === */

static ssize_t _mlisp_env_get_env_frame(
   struct MLISP_EXEC_STATE* exec, struct MLISP_PARSER* parser,
   struct MLISP_ENV_NODE* e_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t ret_idx = 0;
   struct MLISP_ENV_NODE* e = NULL;
   ssize_t i = 0;
   uint8_t autolock = 0;
   struct MDATA_VECTOR* env = NULL;

   env = _mlisp_env_select( exec );

   if( !mdata_vector_is_locked( env ) ) {
      mdata_vector_lock( env );
      autolock = 1;
   }

   for( i = mdata_vector_ct( env ) - 1; 0 <= i ; i-- ) {
      assert( mdata_vector_is_locked( env ) );
      e = mdata_vector_get( env, i, struct MLISP_ENV_NODE );
      assert( NULL != e );

      if( MLISP_TYPE_ARGS_S != e->type ) {
         /* Hunt for the initial env arg separator. */
         continue;
      }

#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: initial env arg " SSIZE_T_FMT " with ret: " SSIZE_T_FMT,
         exec->uid, i, e->value.args_start );
#endif /* MLISP_EXEC_TRACE_LVL */

      ret_idx = i;
      if( NULL != e_out ) {
         memcpy( e_out, e, sizeof( struct MLISP_ENV_NODE ) );
      }
      break;
   }

cleanup:

   if( autolock ) {
      mdata_vector_unlock( env );
   }
 
   if( MERROR_OK != retval ) {
      ret_idx = retval * -1;
   }

   return ret_idx;
}

/* === */

static ssize_t _mlisp_env_prune_args(
   struct MLISP_EXEC_STATE* exec, struct MLISP_PARSER* parser
) {
   ssize_t ret_idx = 0;
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;
   struct MLISP_ENV_NODE* e = NULL;
   size_t removed = 0;
   struct MDATA_VECTOR* env = NULL;

   env = _mlisp_env_select( exec );

   /* This function modifies the env, so existing locks might break. */
   assert( !mdata_vector_is_locked( env ) );

   assert( 0 < mdata_vector_ct( env ) );

   mdata_vector_lock( env );

   /* Get the most recent start frame in the env. */
   i = _mlisp_env_get_env_frame( exec, parser, NULL );
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: pruning env args from frame " SSIZE_T_FMT "...",
      exec->uid, i );
#endif /* MLISP_EXEC_TRACE_LVL */
   e = mdata_vector_get( env, i, struct MLISP_ENV_NODE );
   assert( NULL != e );

   while( MLISP_TYPE_ARGS_E != e->type ) {
      mdata_vector_unlock( env );
      retval = mdata_vector_remove( env, i );
      maug_cleanup_if_not_ok();
      mdata_vector_lock( env );

      /* Refresh e based on what i *now* points to. */
      assert( mdata_vector_is_locked( env ) );
      e = mdata_vector_get( env, i, struct MLISP_ENV_NODE );
      assert( NULL != e );

      removed++;
   }

   /* Remove the actual terminal separator. */
   mdata_vector_unlock( env );
   retval = mdata_vector_remove( env, i );
   maug_cleanup_if_not_ok();
   mdata_vector_lock( env );

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: removed " SIZE_T_FMT " args!", exec->uid, removed );
#endif /* MLISP_EXEC_TRACE_LVL */

cleanup:

   mdata_vector_unlock( env );

   if( MERROR_OK != retval ) {
      ret_idx = retval * -1;
   }

   return ret_idx;
}

/* === */

static MERROR_RETVAL _mlisp_env_cb_cmp(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   size_t args_c, void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE tmp;
   uint8_t truth = 0;
   int a_int,
      b_int,
      a_type,
      b_type;
   int* cur_int = NULL;

   mdata_strpool_lock( &(parser->strpool) );

   /* XXX: If we put a mutable variable first, it gets modified? */

#if MLISP_EXEC_TRACE_LVL > 0
#  define _MLISP_TYPE_TABLE_CMP( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == tmp.type ) { \
         *cur_int = (int)tmp.value.name; \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "%u: cmp: pop " fmt " (%d)", exec->uid, tmp.value.name, *cur_int );
#else
#  define _MLISP_TYPE_TABLE_CMP( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == tmp.type ) { \
         *cur_int = (int)tmp.value.name;
#endif /* MLISP_EXEC_TRACE_LVL */

   retval = mlisp_stack_pop( exec, &tmp );
   maug_cleanup_if_not_ok();
   cur_int = &b_int;
   if( MLISP_TYPE_STR == tmp.type ) {
      /* TODO: Buffer string for later comparison. */
      a_type = MLISP_TYPE_STR;

   MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_CMP )
   } else {
      error_printf( "cmp: invalid type: %d", tmp.type );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   retval = mlisp_stack_pop( exec, &tmp );
   maug_cleanup_if_not_ok();
   cur_int = &a_int;
   if( MLISP_TYPE_STR == tmp.type ) {
      /* TODO: Buffer string for later comparison. */
      b_type = MLISP_TYPE_STR;

   MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_CMP )
   } else {
      error_printf( "cmp: invalid type!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   /* TODO: String comparison? */
   if( MLISP_TYPE_STR == a_type || MLISP_TYPE_STR == b_type ) {
   
      /* TODO: Do a strncmp() and push 1 if true. */
      retval = mlisp_stack_push( exec, 0, mlisp_bool_t );
      goto cleanup;
   }

   /* String comparison didn't catch, so it must be a number comparison? */
   if( MLISP_ENV_FLAG_CMP_GT == (MLISP_ENV_FLAG_CMP_GT & flags) ) {
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL, "%u: cmp %d > %d",
         exec->uid, a_int, b_int );
#endif /* MLISP_EXEC_TRACE_LVL */
      truth = a_int > b_int;
   } else if( MLISP_ENV_FLAG_CMP_LT == (MLISP_ENV_FLAG_CMP_LT & flags) ) {
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL, "%u: cmp %d < %d",
         exec->uid, a_int, b_int );
#endif /* MLISP_EXEC_TRACE_LVL */
      truth = a_int < b_int;
   } else if( MLISP_ENV_FLAG_CMP_EQ == (MLISP_ENV_FLAG_CMP_EQ & flags) ) {
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL, "%u: cmp %d == %d",
         exec->uid, a_int, b_int );
#endif /* MLISP_EXEC_TRACE_LVL */
      truth = a_int == b_int;
   } else {
      error_printf( "invalid parameter provided to _mlisp_env_cb_cmp()!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   retval = mlisp_stack_push( exec, truth, mlisp_bool_t );

cleanup:

   mdata_strpool_unlock( &(parser->strpool) );

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_env_cb_arithmetic(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   size_t args_c, void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE num;
   /* TODO: Vary type based on multiplied types. */
   int16_t num_out = 0;
   size_t i = 0;

#  define _MLISP_TYPE_TABLE_ARI1( idx, ctype, name, const_name, fmt ) \
      } else if( MLISP_TYPE_ ## const_name == num.type ) { \
         num_out = num.value.name;

   retval = mlisp_stack_pop( exec, &num );
   maug_cleanup_if_not_ok();

   if( 0 ) {
   MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_ARI1 )
   } else {
      error_printf( "arithmetic: invalid type!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

#  define _MLISP_TYPE_TABLE_ARI2( idx, ctype, name, const_name, fmt ) \
      } else if( \
         MLISP_TYPE_ ## const_name == num.type && \
         MLISP_ENV_FLAG_ARI_ADD == (MLISP_ENV_FLAG_ARI_ADD & flags) \
      ) { \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "%u: arithmetic: %d + " fmt, exec->uid, num_out, num.value.name ); \
         num_out += num.value.name; \
      } else if( \
         MLISP_TYPE_ ## const_name == num.type && \
         MLISP_ENV_FLAG_ARI_MUL == (MLISP_ENV_FLAG_ARI_MUL & flags) \
      ) { \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "%u: arithmetic: %d * " fmt, exec->uid, num_out, num.value.name ); \
         num_out *= num.value.name; \
      } else if( \
         MLISP_TYPE_ ## const_name == num.type && \
         MLISP_ENV_FLAG_ARI_DIV == (MLISP_ENV_FLAG_ARI_DIV & flags) \
      ) { \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "%u: arithmetic: %d / " fmt, exec->uid, num_out, num.value.name ); \
         num_out /= num.value.name; \

   for( i = 0 ; args_c - 1 > i ; i++ ) {
      retval = mlisp_stack_pop( exec, &num );
      maug_cleanup_if_not_ok();

      if( 0 ) {
      MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_ARI2 )

      } else if(
         MLISP_TYPE_INT == num.type &&
         MLISP_ENV_FLAG_ARI_MOD == (MLISP_ENV_FLAG_ARI_MOD & flags)
      ) {
         /* Modulus is a special case, as you can't mod by float. */
         debug_printf( MLISP_EXEC_TRACE_LVL,
            "%u: arithmetic: %d %% %d", exec->uid, num_out, num.value.integer );
         num_out %= num.value.integer;
      } else {
         error_printf( "arithmetic: invalid type!" );
         retval = MERROR_EXEC;
         goto cleanup;
      }
   }

   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: arithmetic result: %d", exec->uid, num_out );

   retval = mlisp_stack_push( exec, num_out, int16_t );

cleanup:

   mdata_strpool_unlock( &(parser->strpool) );

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_env_cb_define(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   size_t args_c, void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE key;
   struct MLISP_STACK_NODE val;
   MAUG_MHANDLE key_tmp_h = (MAUG_MHANDLE)NULL;
   char* key_tmp = NULL;
   struct MDATA_VECTOR* env = NULL;
   ssize_t env_idx = 0;

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
   assert( (MAUG_MHANDLE)NULL != key_tmp_h );

   maug_mlock( key_tmp_h, key_tmp );
   maug_cleanup_if_null_lock( char*, key_tmp );

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: define \"%s\" (strpool(" SIZE_T_FMT "))...",
      exec->uid, key_tmp, key.value.strpool_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

   /* Figure out the env to use. */
   if(
      MLISP_ENV_FLAG_DEFINE_GLOBAL == (MLISP_ENV_FLAG_DEFINE_GLOBAL & flags)
   ) {
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: using global env...", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */
      env = exec->global_env;
   } else {
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL, "%u: using exec env...", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */
      env = _mlisp_env_select( exec );
   }
   assert( NULL != env );

   /* Perform the insertion. */
   env_idx = _mlisp_env_set_internal(
      env, &(parser->strpool), exec->uid,
      key_tmp, maug_strlen( key_tmp ), val.type, &(val.value), NULL, 0 );
   if( 0 > env_idx ) {
      retval = merror_sz_to_retval( env_idx );
      goto cleanup;
   }

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: setup env node " SSIZE_T_FMT ": %s",
      exec->uid, env_idx, key_tmp );
#endif /* MLISP_EXEC_TRACE_LVL */

cleanup:

   if( NULL != key_tmp ) {
      maug_munlock( key_tmp_h, key_tmp );
   }

   if( (MAUG_MHANDLE)NULL != key_tmp_h ) {
      maug_mfree( key_tmp_h );
   }

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_env_cb_if(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   size_t args_c, void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t* p_if_child_idx = NULL;
   struct MLISP_STACK_NODE s;
   struct MLISP_AST_NODE* n = NULL;

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: qrqrqrqrqr STEP IF qrqrqrqrqr", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */

   /* Grab the current exec index for the child vector for this node. */
   assert( mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   p_if_child_idx = mdata_vector_get(
      &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_if_child_idx );
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: node " SIZE_T_FMT " child idx: " SIZE_T_FMT,
      exec->uid, n_idx, *p_if_child_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

   n = mdata_vector_get( &(parser->ast), n_idx, struct MLISP_AST_NODE );

   if( 0 == *p_if_child_idx ) {
      /* Evaluating if condition. */
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: stepping into condition...", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */
      retval = _mlisp_step_iter(
         parser, n->ast_idx_children[*p_if_child_idx], exec );
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: ...stepped out of condition", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */

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
            retval, "if", parser, n_idx, exec,
            /* Flip boolean and increment. */
            (1 - s.value.boolean) + 1 );
      }

   } else if( args_c > *p_if_child_idx ) { /* 3 if else present, else 2. */
      /* Pursuing TRUE or FALSE clause. */

#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: descending into IF path: " SIZE_T_FMT,
         exec->uid, *p_if_child_idx );
#endif /* MLISP_EXEC_TRACE_LVL */
      
      /* Prepare for stepping. */

      /* Step and check. */
      retval = _mlisp_step_iter(
         parser, n->ast_idx_children[*p_if_child_idx], exec );
      retval = _mlisp_preempt(
         retval, "if", parser, n_idx, exec, 3 );
   }

cleanup:

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: qrqrqrqrqr END STEP IF qrqrqrqrqr", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */

   return retval;
}

/* === */

#ifndef MAUG_NO_RETRO
/* TODO: Define this in retroflat in line with dependency guidelines. */

static MERROR_RETVAL _mlisp_env_cb_random(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   size_t args_c, void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE mod;
   int16_t random_int = 0;

   retval = mlisp_stack_pop( exec, &mod );
   maug_cleanup_if_not_ok();

   if( MLISP_TYPE_INT != mod.type ) {
      /* TODO: Setup float. */
      error_printf( "random: invalid modulus type: %d", mod.type );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   random_int = retroflat_get_rand() % mod.value.integer;

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: random: %d", exec->uid, random_int ); 
#endif /* MLISP_EXEC_TRACE_LVL */

   mlisp_stack_push( exec, random_int, int16_t );

cleanup:

   return retval;
}

#endif /* !MAUG_NO_RETRO */

/* === */

static MERROR_RETVAL _mlisp_env_cb_ano(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   size_t args_c, void* cb_data, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE val;
   mlisp_bool_t val_out =
      MLISP_ENV_FLAG_ANO_OR == (MLISP_ENV_FLAG_ANO_OR & flags) ?
      0 : 1;
   size_t i = 0;

   /* TODO: Switch this to a step_or() function so that we can opt not to
    *       evaluate conditions unless prior stepped children are false.
    */

   for( i = 0 ; args_c > i ; i++ ) {
      retval = mlisp_stack_pop( exec, &val );
      maug_cleanup_if_not_ok();

      if( MLISP_TYPE_BOOLEAN != val.type ) {
         error_printf( "or: invalid boolean type: %d", val.type );
      }

      if( val.value.boolean ) {
#if MLISP_EXEC_TRACE_LVL > 0
         debug_printf( MLISP_EXEC_TRACE_LVL, "%u: found TRUE in %s!",
            exec->uid,
            MLISP_ENV_FLAG_ANO_OR == (MLISP_ENV_FLAG_ANO_OR & flags) ?
            "or" : "and" );
#endif /* MLISP_EXEC_TRACE_LVL */
         val_out =
            MLISP_ENV_FLAG_ANO_OR == (MLISP_ENV_FLAG_ANO_OR & flags) ? 1 : 0;
      }
   }

   retval = _mlisp_stack_push_mlisp_bool_t( exec, val_out );

cleanup:

   return retval;
}

/* === */

/* Execution Functions */

/* === */

static MERROR_RETVAL _mlisp_preempt(
   MERROR_RETVAL retval, const char* caller, struct MLISP_PARSER* parser,
   size_t n_idx, struct MLISP_EXEC_STATE* exec, size_t new_idx
) {
   struct MLISP_AST_NODE* n = NULL;
   size_t* p_child_idx = NULL;

   assert( mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   p_child_idx = mdata_vector_get(
      &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_child_idx );

   n = mdata_vector_get( &(parser->ast), n_idx, struct MLISP_AST_NODE );

   if( 0 <= n->token_idx ) {
      mdata_strpool_lock( &(parser->strpool) );
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: eval step " SSIZE_T_FMT " under (%s) %s...",
         exec->uid, *p_child_idx, caller,
         mdata_strpool_get( &(parser->strpool), n->token_idx ) );
#endif /* MLISP_EXEC_TRACE_LVL */
      mdata_strpool_unlock( &(parser->strpool) );
#if MLISP_EXEC_TRACE_LVL > 0
   } else {
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: eval step " SSIZE_T_FMT " under (%s) (empty token)...",
         exec->uid, *p_child_idx, caller );
#endif /* MLISP_EXEC_TRACE_LVL */
   }

   if( MERROR_OK != retval ) {
      /* Something bad happened, so don't increment! */
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: not incrementing node " SIZE_T_FMT " child idx from "
            SIZE_T_FMT " (retval: 0x%x)!",
         exec->uid, n_idx, *p_child_idx, retval );
#endif /* MLISP_EXEC_TRACE_LVL */
      goto cleanup;
   }

   /* Could not exec *this* node yet, so don't increment its parent. */
   retval = MERROR_PREEMPT;

   /* Increment this node, since the child actually executed. */
   (*p_child_idx) = new_idx;
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: incremented node " SIZE_T_FMT " child idx to: " SIZE_T_FMT,
      exec->uid, n_idx, *p_child_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

cleanup:

   assert( !mdata_strpool_is_locked( &(parser->strpool) ) );

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
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: node " SIZE_T_FMT " child idx: " SIZE_T_FMT,
      exec->uid, n_idx, *p_child_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

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
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: skipping lambda children...", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */
      goto cleanup;
   }

   if( mlisp_ast_has_ready_children( *p_child_idx, n ) ) {
      /* Call the next uncalled child. */

      if(
         MLISP_AST_FLAG_DEFINE == (MLISP_AST_FLAG_DEFINE & n->flags) &&
         0 == *p_child_idx
      ) {
         /* The next child is a term to be defined. */
#if MLISP_EXEC_TRACE_LVL > 0
         debug_printf( MLISP_EXEC_TRACE_LVL,
            "%u: setting MLISP_EXEC_FLAG_DEF_TERM!", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */
         exec->flags |= MLISP_EXEC_FLAG_DEF_TERM;
      } else {
         exec->flags &= ~MLISP_EXEC_FLAG_DEF_TERM;
      }

      /* Step and check. */
      retval = _mlisp_step_iter(
         parser, n->ast_idx_children[*p_child_idx], exec );
      retval = _mlisp_preempt(
         retval, "node", parser, n_idx, exec, (*p_child_idx) + 1 );
      goto cleanup;
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
   MAUG_MHANDLE key_tmp_h = (MAUG_MHANDLE)NULL;
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
      assert( (MAUG_MHANDLE)NULL != key_tmp_h );

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

   if( (MAUG_MHANDLE)NULL != key_tmp_h ) {
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
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: resetting node " SIZE_T_FMT " child idx to 0", exec->uid, n_idx );
#endif /* MLISP_EXEC_TRACE_LVL */
   p_child_idx = mdata_vector_get( &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_child_idx );
   *p_child_idx = 0;

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: resetting node " SIZE_T_FMT " visit count to 0", exec->uid, n_idx );
#endif /* MLISP_EXEC_TRACE_LVL */
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
   struct MDATA_VECTOR* env = NULL;

   env = _mlisp_env_select( exec );

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: resetting lambda " SIZE_T_FMT "...", exec->uid, n_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

   assert( mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   assert( !mdata_vector_is_locked( env ) );

   /* Clear off lambda stack args. */
   ret_idx = _mlisp_env_prune_args( exec, parser );
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

/* This is internal-only and should only be called from _mlisp_step_iter()! */
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
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL, "%u: TRACE TAIL TIME!", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */
      assert(
         !mdata_vector_is_locked( &(parser->env) ) &&
         !mdata_vector_is_locked( &(exec->env) ) );
      _mlisp_reset_lambda( parser, n_idx, exec );
      retval = mdata_vector_remove_last( &(exec->lambda_trace) );
      maug_cleanup_if_not_ok();
   }

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: xvxvxvxvxvxvx STEP LAMBDA " SIZE_T_FMT " xvxvxvxvxvx",
      exec->uid, n_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

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
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: lambda node " SIZE_T_FMT " child idx: " SIZE_T_FMT,
      exec->uid, n_idx, *p_lambda_child_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

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
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: child idx for args AST node " SIZE_T_FMT ": " SIZE_T_FMT,
         exec->uid, *p_lambda_child_idx, *p_args_child_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

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
      if( MERROR_OK != retval && MERROR_PREEMPT != retval ) {
         /* Something bad happened! */
         goto cleanup;
      }

      if( MERROR_OK == retval ) {
         /* Set *after-arg* delimiter in env after last arg. */
         retval = mlisp_env_set(
            parser, exec, "$ARGS_E$", 0, MLISP_TYPE_ARGS_E, &n_idx, NULL, 0 );
         maug_cleanup_if_not_ok();

         /* Increment child idx so we call the exec child on next heartbeat. */
         (*p_lambda_child_idx)++;
#if MLISP_EXEC_TRACE_LVL > 0
         debug_printf( MLISP_EXEC_TRACE_LVL,
            "%u: incremented node " SIZE_T_FMT " child idx to: " SIZE_T_FMT,
            exec->uid, n_idx, *p_lambda_child_idx );
#endif /* MLISP_EXEC_TRACE_LVL */
      }

      /* Set the error to MERROR_PREEMPT so that caller knows this lambda isn't
       * finished executing.
       */
      retval = MERROR_PREEMPT;

   } else if( mlisp_ast_has_ready_children( *p_lambda_child_idx, n ) ) {
      /* Dive into first lambda child until we no longer can. */

      assert(
         !mdata_vector_is_locked( &(parser->env) ) &&
         !mdata_vector_is_locked( &(exec->env) ) );
      assert(
         NULL == exec->global_env ||
         !mdata_vector_is_locked( exec->global_env ) );

      retval = _mlisp_step_iter(
         parser, n->ast_idx_children[*p_lambda_child_idx], exec );

      retval = _mlisp_preempt(
         retval, "lambda", parser, n_idx, exec, (*p_lambda_child_idx) + 1 );

   } else {
      /* No more children to execute! */
      assert(
         !mdata_vector_is_locked( &(parser->env) ) &&
         !mdata_vector_is_locked( &(exec->env) ) );
      assert(
         NULL == exec->global_env ||
         !mdata_vector_is_locked( exec->global_env ) );
      _mlisp_reset_lambda( parser, n_idx, exec );
   }

   /* TODO: If MERROR_PREEMPT is not returned, remove args_s and args_e? */

cleanup:

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: xvxvxvxvxvxvx END STEP LAMBDA " SIZE_T_FMT " xvxvxvxvxvx",
      exec->uid, n_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

   /* Cleanup the passthrough note for this heartbeat. */
   mdata_vector_remove_last( &(exec->lambda_trace) );

   return retval;
}

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

/**
 * \brief Like env_get, but fallback to numbers or literals if the token is
 *        not present in the env.
 */
static MERROR_RETVAL _mlisp_eval_token_strpool(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   size_t token_idx, size_t token_sz, struct MLISP_ENV_NODE* e_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_ENV_NODE* p_e = NULL;

   /* Make sure we're sharing env context with our caller! */
   assert(
      mdata_vector_is_locked( &(parser->env) ) ||
      mdata_vector_is_locked( &(exec->env) ) );
   assert( /* Also make sure we're sharing ctx for global env if present! */
      NULL == exec->global_env ||
      mdata_vector_is_locked( exec->global_env ) );

   mdata_strpool_lock( &(parser->strpool) );
   
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: eval token: \"%s\" (strlen: " SIZE_T_FMT "r/" SIZE_T_FMT "d)",
      exec->uid, &(strpool[token_idx]),
      token_sz, maug_strlen( &(strpool[token_idx]) ) );
#endif /* MLISP_EXEC_TRACE_LVL */
   if(
      0 == strncmp( mdata_strpool_get( &(parser->strpool), token_idx ),
         "begin", token_sz )
   ) {
      /* Fake env node e to signal step_iter() to place/cleanup stack frame. */
      e_out->type = MLISP_TYPE_BEGIN;

   } else if( NULL != (p_e = mlisp_env_get_strpool(
      /* XXX: Don't pass the strpool pointer! Redo the function prototype! */
      parser, exec, parser->strpool.str_p, token_idx, token_sz
   ) ) ) {
      /* A literal found in the environment. */
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL, "%u: found %s in env!",
         exec->uid, mdata_strpool_get(
            &(parser->strpool), p_e->name_strpool_idx ) );
#endif /* MLISP_EXEC_TRACE_LVL */

      /* Copy onto native stack so we can unlock env in case this is a
       * callback that needs to execute. */
      memcpy( e_out, p_e, sizeof( struct MLISP_ENV_NODE ) );
      p_e = NULL;

   } else if( maug_is_num(
      mdata_strpool_get( &(parser->strpool), token_idx ), token_sz, 10, 1
   ) ) {
      /* Fake env node e from a numeric literal. */
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: did not find %s in env, but it is a number...",
         exec->uid, &(strpool[token_idx]) );
#endif /* MLISP_EXEC_TRACE_LVL */
      e_out->value.integer =
         maug_atos32( mdata_strpool_get( &(parser->strpool), token_idx ),
            token_sz );
      e_out->type = MLISP_TYPE_INT;

   } else if( maug_is_float(
      mdata_strpool_get( &(parser->strpool), token_idx ), token_sz
   ) ) {
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: did not find %s in env, but it is a float...",
         exec->uid, mdata_strpool_get( &(parser->strpool), token_idx ) );
#endif /* MLISP_EXEC_TRACE_LVL */
      /* Fake env node e from a floating point numeric literal. */
      e_out->value.floating = maug_atof(
         mdata_strpool_get( &(parser->strpool), token_idx ), token_sz );
      e_out->type = MLISP_TYPE_FLOAT;

   } else {
#if MLISP_EXEC_TRACE_LVL > 0
      error_printf(
         "%u: could not make sense of token: %s",
         exec->uid, &(strpool[token_idx]) );
#endif /* MLISP_EXEC_TRACE_LVL */

   }

cleanup:

   if( mdata_strpool_is_locked( &(parser->strpool) ) ) {
      mdata_strpool_unlock( &(parser->strpool) );
   }

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
   mlisp_env_cb_t e_cb = NULL;
   uint8_t e_flags = 0;
   void* e_cb_data = NULL;
   struct MDATA_VECTOR* env = NULL;
   mlisp_lambda_t e_lambda = 0;

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
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: visit count for AST node " SIZE_T_FMT ": " SIZE_T_FMT,
      exec->uid, n_idx, *p_visit_ct );
#endif /* MLISP_EXEC_TRACE_LVL */

   /* Push a stack frame marker on the first visit to a BEGIN node. */
   if(
      MLISP_AST_FLAG_BEGIN == (MLISP_AST_FLAG_BEGIN & n->flags) &&
      1 == *p_visit_ct
   ) {
      /* Push a stack frame on first visit. */
      retval = _mlisp_stack_push_mlisp_begin_t( exec, n_idx );
      maug_cleanup_if_not_ok();
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
      /* TODO: Assert node above it is a define! */
      mlisp_stack_push( exec, n_idx, mlisp_lambda_t );
      goto cleanup;
   }
 
   /* Now that the children have been evaluated above, evaluate this node.
    * Assume all the previously called children are now on the stack.
    */

   /* Lock the env so we can grab the token from it and evalauate it below
    * in one swoop without an unlock.
    */
   env = _mlisp_env_select( exec );
   assert( !mdata_vector_is_locked( env ) );
   assert(
      NULL == exec->global_env || !mdata_vector_is_locked( exec->global_env ) );
   mdata_vector_lock( env );
   if( NULL != exec->global_env ) {
      mdata_vector_lock( exec->global_env );
   }
   assert( mdata_vector_is_locked( env ) );

   /* Grab the token for this node and figure out what it is. */
   retval = _mlisp_eval_token_strpool(
      parser, exec, n->token_idx, n->token_sz, &e );
   maug_cleanup_if_not_ok();

   /* Prepare to step. */

   /* Put the token or its result (if callable) on the stack. */
#  define _MLISP_TYPE_TABLE_ENVE( idx, ctype, name, const_name, fmt ) \
   } else if( MLISP_TYPE_ ## const_name == e.type ) { \
      retval = _mlisp_stack_push_ ## ctype( exec, e.value.name ); \
      maug_cleanup_if_not_ok();

   if( MLISP_EXEC_FLAG_DEF_TERM == (MLISP_EXEC_FLAG_DEF_TERM & exec->flags) ) {
      /* Avoid a deadlock when *re*-assigning terms caused by term being
       * evaluated before it is defined.
       */
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: special case! pushing literal to stack: " SSIZE_T_FMT,
         exec->uid, n->token_idx );
#endif /* MLISP_EXEC_TRACE_LVL */
      retval = _mlisp_stack_push_mdata_strpool_idx_t( exec, n->token_idx );
      maug_cleanup_if_not_ok();
   } else if( MLISP_TYPE_BEGIN == e.type ) {
      /* Cleanup the stack that's been pushed by children since this BEGIN's
       * initial visit.
       */
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: rewinding stack for begin on node " SSIZE_T_FMT,
         exec->uid, n_idx );
#endif /* MLISP_EXEC_TRACE_LVL */
      retval = _mlisp_stack_cleanup( parser, n_idx, exec );
      maug_cleanup_if_not_ok();

      /* Push a replacement BEGIN that can be caught later and throw an
       * MERROR_RESET.
       */
      retval = _mlisp_stack_push_mlisp_begin_t( exec, n_idx );
      maug_cleanup_if_not_ok();

   } else if( MLISP_TYPE_CB == e.type ) {
      /* This is a special case... rather than pushing the callback, *execute*
       * it and let it push its result to the stack. This will create a 
       * redundant case below, but that can't be helped...
       */

      /* Unlock the env so the callback below can use it if needed. */
      e_cb = e.value.cb;
      e_cb_data = e.cb_data;
      e_flags = e.flags;
      mdata_vector_unlock( env );
      if( NULL != exec->global_env ) {
         mdata_vector_unlock( exec->global_env );
      }

      retval = e_cb(
         parser, exec, n_idx, n->ast_idx_children_sz, e_cb_data, e_flags );

      /* Relock it for the benefit of the unlock in cleanup. */
      mdata_vector_lock( env );
      if( NULL != exec->global_env ) {
         mdata_vector_lock( exec->global_env );
      }

   } else if( MLISP_TYPE_LAMBDA == e.type ) {
      /* Create a "portal" into the lambda. The execution chain stays pointing
       * to this lambda-call node, but _mlisp_step_lambda() returns
       * MERROR_PREEMPT up the chain for subsequent heartbeats, until lambda is
       * done.
       */
      e_lambda = e.value.lambda;
      mdata_vector_unlock( env );
      if( NULL != exec->global_env ) {
         mdata_vector_unlock( exec->global_env );
      }

      retval = _mlisp_step_lambda( parser, e_lambda, exec );

      /* Relock it for the benefit of the unlock in cleanup. */
      mdata_vector_lock( env );
      if( NULL != exec->global_env ) {
         mdata_vector_lock( exec->global_env );
      }

   MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_ENVE )
   } else {
      retval = _mlisp_stack_push_mdata_strpool_idx_t( exec, n->token_idx );
      maug_cleanup_if_not_ok();
   }

cleanup:

   if( NULL != env ) {
      mdata_vector_unlock( env );
   }

   if( NULL != exec->global_env ) {
      mdata_vector_unlock( exec->global_env );
   }

   return retval;
}

/* === */

ssize_t mlisp_count_builtins(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MDATA_VECTOR* env = NULL;
   size_t builtins = 0;
   int autolock = 0;
   size_t i = 0;
   struct MLISP_ENV_NODE* e = NULL;

   /* Prepare env for mlisp_env_get() below. */
   env = _mlisp_env_select( exec );

   if( 0 == mdata_vector_ct( env ) ) {
      goto cleanup;
   }

   if( !mdata_vector_is_locked( env ) ) {
      mdata_vector_lock( env );
      autolock = 1;
   }

   for( i = 0 ; mdata_vector_ct( env ) > i ; i++ ) {
      e = mdata_vector_get( env, i, struct MLISP_ENV_NODE );
      assert( NULL != e );
      if( MLISP_ENV_FLAG_BUILTIN == (MLISP_ENV_FLAG_BUILTIN & e->flags) ) {
         builtins++;
      } else {
         /* Non-builtin found, and builtins are at the beginning! */
         break;
      }
   }

cleanup:

   if( MERROR_OK != retval ) {
      builtins = merror_retval_to_sz( retval );
   }

   if( autolock ) {
      mdata_vector_unlock( env );
   }

   return builtins;
}

/* === */

MERROR_RETVAL mlisp_check_state(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( !mlisp_check_ast( parser  ) ) {
      error_printf( "no valid AST present; could not exec!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   if(
      MLISP_EXEC_FLAG_INITIALIZED != (exec->flags & MLISP_EXEC_FLAG_INITIALIZED)
   ) {
      retval = MERROR_EXEC;
      goto cleanup;
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

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL, "%u: heartbeat start", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */

   /* These can remain locked for the whole step, as they're never added or
    * removed.
    */
   assert( !mdata_vector_is_locked( &(exec->per_node_child_idx) ) );
   assert( !mdata_vector_is_locked( &(exec->per_node_visit_ct) ) );
   assert( !mdata_vector_is_locked( &(parser->ast) ) );
   mdata_vector_lock( &(exec->per_node_child_idx) );
   mdata_vector_lock( &(exec->per_node_visit_ct) );
   mdata_vector_lock( &(parser->ast) );

   /* Disable transient flags. */
   exec->flags &= MLISP_EXEC_FLAG_TRANSIENT_MASK;
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
#if MLISP_EXEC_TRACE_LVL > 0
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: execution terminated successfully", exec->uid );
#endif /* MLISP_EXEC_TRACE_LVL */
      retval = MERROR_EXEC; /* Signal the caller: we're out of instructions! */
#if MLISP_EXEC_TRACE_LVL > 0
   } else {
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: execution terminated with retval: %d", exec->uid, retval );
#endif /* MLISP_EXEC_TRACE_LVL */
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
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: " MLISP_TRACE_SIGIL " HBEXEC (%u): %s",
      exec->uid, ms_end - ms_start, trace_str );
#endif /* MLISP_EXEC_TRACE_LVL */
#endif /* MLISP_DEBUG_TRACE */
  
cleanup:

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "%u: heartbeat end: %x", exec->uid, retval );
#endif /* MLISP_EXEC_TRACE_LVL */

   assert( mdata_vector_is_locked( &(parser->ast) ) );
   mdata_vector_unlock( &(parser->ast) );
   mdata_vector_unlock( &(exec->per_node_visit_ct) );
   mdata_vector_unlock( &(exec->per_node_child_idx) );

   return retval;
}

/* === */

MERROR_RETVAL mlisp_step_lambda(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   const char* lambda
) {
   struct MLISP_ENV_NODE* e = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t autolock = 0;
   struct MDATA_VECTOR* env = NULL;
   mlisp_lambda_t lambda_idx = 0;
   struct MLISP_AST_NODE* n = NULL;

   /* Prepare env for mlisp_env_get() below. */
   env = _mlisp_env_select( exec );

   if( MERROR_OK != mlisp_check_state( parser, exec ) ) {
      error_printf( "mlisp not ready!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   /* Autolock vectors used below. */
   /* TODO: Should this be a reusable macro? */
   if( !mdata_vector_is_locked( env ) ) {
      assert( (MAUG_MHANDLE)NULL != env->data_h );
      mdata_vector_lock( env );
      autolock |= 0x01;
   }
   if( !mdata_vector_is_locked( &(exec->per_node_child_idx) ) ) {
      mdata_vector_lock( &(exec->per_node_child_idx) );
      autolock |= 0x02;
   }
   if( !mdata_vector_is_locked( &(exec->per_node_visit_ct) ) ) {
      mdata_vector_lock( &(exec->per_node_visit_ct) );
      autolock |= 0x04;
   }
   if( !mdata_vector_is_locked( &(parser->ast) ) ) {
      mdata_vector_lock( &(parser->ast) );
      autolock |= 0x08;
   }
   if(
      NULL != exec->global_env &&
      0 < mdata_vector_ct( exec->global_env ) &&
      !mdata_vector_is_locked( exec->global_env )
   ) {
      mdata_vector_lock( exec->global_env );
      autolock |= 0x10;
   }

   /* Find the AST node for the lambda. */
   e = mlisp_env_get( parser, exec, lambda );
   if( NULL == e ) {
      error_printf( "lambda \"%s\" not found!", lambda );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }
   lambda_idx = e->value.lambda;

   /* Autounlock just env so _mlisp_step_lambda() works. */
   /* We use autolock with the env minimally to avoid passing around bad
    * pointers.
    */
   if( 0x01 == (0x01 & autolock) ) {
      mdata_vector_unlock( env );
      autolock &= ~0x01;
   }
   if( 0x10 == (0x10 & autolock) ) {
      mdata_vector_unlock( exec->global_env );
      autolock &= ~0x10;
   }

#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL, "%u: lambda \"%s\" is AST node idx %ld",
      exec->uid, lambda, lambda_idx );
#endif /* MLISP_EXEC_TRACE_LVL */

   n = mdata_vector_get( &(parser->ast), lambda_idx, struct MLISP_AST_NODE );
   assert( MLISP_AST_FLAG_LAMBDA == (MLISP_AST_FLAG_LAMBDA & n->flags) );

   /* Jump execution to the lambda on next iter. */
   retval = _mlisp_step_lambda( parser, lambda_idx, exec );

cleanup:

   /* See autolocks above. */
   if( 0x01 == (0x01 & autolock) ) {
      mdata_vector_unlock( env );
   }
   if( 0x02 == (0x02 & autolock) ) {
      mdata_vector_unlock( &(exec->per_node_child_idx) );
   }
   if( 0x04 == (0x04 & autolock) ) {
      mdata_vector_unlock( &(exec->per_node_visit_ct) );
   }
   if( 0x08 == (0x08 & autolock) ) {
      mdata_vector_unlock( &(parser->ast) );
   }
   if( 0x10 == (0x10 & autolock) ) {
      mdata_vector_unlock( exec->global_env );
   }

   return retval;
}

/* === */

MERROR_RETVAL mlisp_exec_add_env_builtins(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;

   retval = mlisp_env_set(
      parser, exec, "gdefine", 7, MLISP_TYPE_CB, _mlisp_env_cb_define,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_DEFINE_GLOBAL );
   maug_cleanup_if_not_ok();

   retval = mlisp_env_set(
      parser, exec, "and", 3, MLISP_TYPE_CB, _mlisp_env_cb_ano,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_ANO_AND );
   maug_cleanup_if_not_ok();
   
   retval = mlisp_env_set(
      parser, exec, "or", 2, MLISP_TYPE_CB, _mlisp_env_cb_ano,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_ANO_OR );
   maug_cleanup_if_not_ok();
   
#ifndef MAUG_NO_RETRO
/* TODO: Define this in retroflat in line with dependency guidelines. */
   retval = mlisp_env_set(
      parser, exec, "random", 6, MLISP_TYPE_CB, _mlisp_env_cb_random,
      NULL, MLISP_ENV_FLAG_BUILTIN );
   maug_cleanup_if_not_ok();
#endif /* !MAUG_NO_RETRO */
   
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
      parser, exec, "/", 1, MLISP_TYPE_CB, _mlisp_env_cb_arithmetic,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_ARI_DIV );
   maug_cleanup_if_not_ok();
   
   retval = mlisp_env_set(
      parser, exec, "%", 1, MLISP_TYPE_CB, _mlisp_env_cb_arithmetic,
      NULL, MLISP_ENV_FLAG_BUILTIN | MLISP_ENV_FLAG_ARI_MOD );
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

   return retval;
}

/* === */

MERROR_RETVAL mlisp_exec_init(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t append_retval = 0;
   size_t zero = 0;

   assert( 0 == exec->flags );

   maug_mzero( exec, sizeof( struct MLISP_EXEC_STATE ) );

   exec->flags = flags;
   exec->uid = g_mlispe_last_uid++;

   /* Setup lambda visit stack so it can be locked on first step. */
   append_retval = mdata_vector_append(
      &(exec->lambda_trace), &zero, sizeof( size_t ) );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();
   mdata_vector_remove_last( &(exec->lambda_trace) );

   append_retval = mdata_vector_alloc(
      &(exec->env), sizeof( struct MLISP_ENV_NODE ), 0 );
   /* TODO: Cleanup partially allocated object. */
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();

   /* Create the node PCs. */
   append_retval = mdata_vector_append(
      &(exec->per_node_child_idx), &zero, sizeof( size_t ) );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();

   /* Make sure there's an exec child node for every AST node. */
   while(
      mdata_vector_ct( &(exec->per_node_child_idx) ) <= 
      mdata_vector_ct( &(parser->ast) )
   ) {
      append_retval = mdata_vector_append( &(exec->per_node_child_idx), &zero,
         sizeof( size_t ) );
      if( 0 > append_retval ) {
         retval = mdata_retval( append_retval );
      }
      maug_cleanup_if_not_ok();
   }

   /* Create the node visit counters. */
   append_retval = mdata_vector_append(
      &(exec->per_node_visit_ct), &zero, sizeof( size_t ) );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();

   /* Make sure there's an exec visit count for every AST node. */
   while(
      mdata_vector_ct( &(exec->per_node_visit_ct) ) <= 
      mdata_vector_ct( &(parser->ast) )
   ) {
      append_retval = mdata_vector_append( &(exec->per_node_visit_ct), &zero,
         sizeof( size_t ) );
      if( 0 > append_retval ) {
         retval = mdata_retval( append_retval );
      }
      maug_cleanup_if_not_ok();
   }

   exec->flags |= MLISP_EXEC_FLAG_INITIALIZED;

   /* Setup initial env. */

   retval = mlisp_exec_add_env_builtins( parser, exec );

cleanup:

   if( MERROR_OK != retval ) {
      error_printf( "mlisp exec initialization failed: %d", retval );
   }

   return retval;
}

/* === */

MERROR_RETVAL mlisp_exec_set_global_env(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec,
   struct MDATA_VECTOR* global_env
) {
   MERROR_RETVAL retval = MERROR_OK;
   int16_t null_val = 0;
   ssize_t env_idx = 0;

   exec->global_env = global_env;

   if( 0 == mdata_vector_ct( global_env ) ) {
      /* Things get very wonky if the env is completely empty due to how empty
       * vectors respond to locking. This is a simple way of working around
       * that rather than adding a lot of special cases!
       */
      env_idx = mdata_vector_alloc(
         global_env, sizeof( struct MLISP_ENV_NODE ), 0 );
      if( 0 > env_idx ) {
         retval = merror_sz_to_retval( env_idx );
      }
      env_idx = _mlisp_env_set_internal(
         global_env, &(parser->strpool), exec->uid,
         "null", 4, MLISP_TYPE_INT, &null_val, NULL, 0 );
      if( 0 > env_idx ) {
         retval = merror_sz_to_retval( env_idx );
      }
   }

   return retval;
}

/* === */

void mlisp_exec_free( struct MLISP_EXEC_STATE* exec ) {
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL,
         "%u: destroying exec (stack: " SIZE_T_FMT ", env: " SIZE_T_FMT ")...",
         exec->uid,
         mdata_vector_ct( &(exec->stack) ), mdata_vector_ct( &(exec->env) ) );
#endif /* MLISP_EXEC_TRACE_LVL */
   mdata_vector_free( &(exec->per_node_child_idx) );
   mdata_vector_free( &(exec->per_node_visit_ct) );
   mdata_vector_free( &(exec->stack) );
   mdata_vector_free( &(exec->env) );
   mdata_vector_free( &(exec->lambda_trace) );
   exec->flags = 0;
#if MLISP_EXEC_TRACE_LVL > 0
   debug_printf( MLISP_EXEC_TRACE_LVL, "exec destroyed!" );
#endif /* MLISP_EXEC_TRACE_LVL */
}

/* === */

MERROR_RETVAL mlisp_deserialize_prepare_EXEC_STATE(
   struct MLISP_EXEC_STATE* exec, size_t i
) {
   MERROR_RETVAL retval = MERROR_OK;
   /* TODO: Re-add built-in function definitions. */
   /* TODO: Provide mechanism for program using maug to re-add function
    *       definitions!
    */
   return retval;
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

