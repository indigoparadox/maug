
#ifndef MLISP_H
#define MLISP_H

/**
 * \addtogroup mlisp MLISP Interpreter
 * \{
 */

#ifndef MLISP_TOKEN_SZ_MAX
#  define MLISP_TOKEN_SZ_MAX 4096
#endif /* !MLISP_TOKEN_SZ_MAX */

#ifndef MLISP_STACK_DEPTH_MAX
#  define MLISP_STACK_DEPTH_MAX 100
#endif /* !MLISP_STACK_DEPTH_MAX */

#ifndef MLISP_TRACE_LVL
#  define MLISP_TRACE_LVL 0
#endif /* !MLISP_TRACE_LVL */

#ifndef MLISP_PARSE_TRACE_LVL
#  define MLISP_PARSE_TRACE_LVL 0
#endif /* !MLISP_PARSE_TRACE_LVL */

#ifndef MLISP_EXEC_TRACE_LVL
#  define MLISP_EXEC_TRACE_LVL 0
#endif /* !MLISP_EXEC_TRACE_LVL */

#ifndef MLISP_AST_IDX_CHILDREN_MAX
#  define MLISP_AST_IDX_CHILDREN_MAX 10
#endif /* !MLISP_AST_IDX_CHILDREN_MAX */

#define MLISP_AST_FLAG_LAMBDA 0x02

/**
 * \addtogroup mlisp_types MLISP Types
 * \{
 */

/**
 * \brief Table of numeric types.
 *
 * These are carved out because they can usually be handled in a similar way.
 */
#define MLISP_NUM_TYPE_TABLE( f ) \
   f( 1, int16_t,        integer,         INT,     "%d", val->integer ) \
   f( 2, float,          floating,        FLOAT,   "%f", val->floating )

/**
 * \brief Table of other types.
 *
 * These are special cases that must usually be handled manually.
 */
#define MLISP_TYPE_TABLE( f ) \
   MLISP_NUM_TYPE_TABLE( f ) \
   f( 3, mdata_strpool_idx_t, strpool_idx, STR, "%s", &(strpool[val->strpool_idx]) ) \
   f( 4, mlisp_env_cb_t, cb,              CB,      "%p", val->cb )

#define MLISP_PARSER_PSTATE_TABLE( f ) \
   f( MLISP_PSTATE_NONE, 0 ) \
   f( MLISP_PSTATE_SYMBOL_OP, 1 ) \
   f( MLISP_PSTATE_SYMBOL, 2 ) \
   f( MLISP_PSTATE_STRING, 3 )

/*! \} */ /* mlisp_types */

struct MLISP_PARSER;
struct MLISP_EXEC_STATE;

typedef MERROR_RETVAL (*mlisp_env_cb_t)(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

#define _MLISP_TYPE_TABLE_FIELDS( idx, ctype, name, const_name, fmt, iv ) \
   ctype name;

union MLISP_VAL {
   MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_FIELDS );
};

struct MLISP_ENV_NODE {
   uint8_t type;
   mdata_strpool_idx_t name_strpool_idx;
   union MLISP_VAL value;
};

struct MLISP_STACK_NODE {
   uint8_t type;
   union MLISP_VAL value;
};

struct MLISP_AST_NODE {
   uint8_t flags;
   mdata_strpool_idx_t token_idx;
   size_t token_sz;
   ssize_t ast_idx_parent;
   ssize_t env_idx_op;
   ssize_t ast_idx_children[MLISP_AST_IDX_CHILDREN_MAX];
   /*! \brief Number of children in MLISP_AST_NODE::ast_idx_children. */
   size_t ast_idx_children_sz;
};

struct MLISP_EXEC_STATE {
   struct MDATA_VECTOR per_node_child_idx;
   struct MLISP_STACK_NODE stack[MLISP_STACK_DEPTH_MAX];
   size_t stack_idx;
};

struct MLISP_PARSER {
   struct MPARSER base;
   struct MDATA_STRTABLE strpool;
   MAUG_MHANDLE env_h;
   struct MDATA_VECTOR ast;
   ssize_t ast_node_iter;
   size_t env_sz;
   size_t env_sz_max;
   struct MDATA_VECTOR env;
};

/**
 * \addtogroup mlisp_parser MLISP Abstract Syntax Tree Parser
 * \{
 */

#define mlisp_parser_pstate( parser ) \
   ((parser)->base.pstate_sz > 0 ? \
      (parser)->base.pstate[(parser)->base.pstate_sz - 1] : MLISP_PSTATE_NONE)

#ifdef MPARSER_TRACE_NAMES
#  define mlisp_parser_pstate_push( parser, new_pstate ) \
      mparser_pstate_push( \
         "mlisp", &((parser)->base), new_pstate, gc_mlisp_pstate_names );

#  define mlisp_parser_pstate_pop( parser ) \
      mparser_pstate_pop( \
         "mlisp", &((parser)->base), gc_mlisp_pstate_names );
#else
#  define mlisp_parser_pstate_push( parser, new_pstate ) \
      mparser_pstate_push( "mlisp", &((parser)->base), new_pstate )

#  define mlisp_parser_pstate_pop( parser ) \
      mparser_pstate_pop( "mlisp", &((parser)->base) )
#endif /* MPARSER_TRACE_NAMES */

#define mlisp_parser_invalid_c( parser, c, retval ) \
   mparser_invalid_c( mlisp, &((parser)->base), c, retval )

#define mlisp_parser_reset_token( parser ) \
   mparser_reset_token( "mlisp", &((parser)->base) )

#define mlisp_parser_append_token( parser, c ) \
   mparser_append_token( "mlisp", &((parser)->base), c )

#define mlisp_parser_parse_token( parser ) \
   parser->token_parser( \
      (parser)->token, (parser)->token_sz, (parser)->token_parser_arg )

MERROR_RETVAL mlisp_ast_dump(
   struct MLISP_PARSER* parser, size_t ast_node_idx, size_t depth, char ab );

/*! \} */ /* mlisp_parser */

/**
 * \addtogroup mlisp_stack MLISP Execution Stack
 * \{
 */

#define mlisp_stack_peek( exec ) \
   (0 < (exec)->stack_idx ? \
      (exec)->stack[(parser)->stack_idx - 1].type : 0)

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
 * \return Pointer to env_node in MLISP_PARSER::env. This is not a copy, so
 *         the pointer is only valid for so long as MLISP_PARSER:env remains
 *         locked!
 */
struct MLISP_ENV_NODE* mlisp_env_get_strpool(
   struct MLISP_PARSER* parser, const char* strpool,
   size_t token_strpool_idx, size_t token_strpool_sz );

MERROR_RETVAL mlisp_env_set(
   struct MLISP_PARSER* parser, const char* token, size_t token_sz,
   uint8_t env_type, void* data );

MERROR_RETVAL mlisp_step(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

MERROR_RETVAL mlisp_parse_c( struct MLISP_PARSER* parser, char c );

MERROR_RETVAL mlisp_parser_init( struct MLISP_PARSER* parser );

void mlisp_parser_free( struct MLISP_PARSER* parser );

/*! \} */ /* mlisp */

#ifdef MLISP_C

#define _MLISP_TYPE_TABLE_CONSTS( idx, ctype, name, const_name, fmt, iv ) \
   MAUG_CONST uint8_t SEG_MCONST MLISP_TYPE_ ## const_name = idx;

MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_CONSTS );

MLISP_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_CONST )

MPARSER_PSTATE_NAMES( MLISP_PARSER_PSTATE_TABLE, mlisp )

/* === */

/* AST Functions */

/* === */

static MERROR_RETVAL
_mlisp_ast_add_child( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_AST_NODE* n_parent = NULL;
   struct MLISP_AST_NODE ast_node;
   ssize_t parent_child_idx = -1;
   ssize_t new_idx_out = 0;
   size_t i = 0;

   /* Setup the new node to copy. */
   maug_mzero( &ast_node, sizeof( struct MLISP_AST_NODE ) );
   ast_node.ast_idx_parent = parser->ast_node_iter;
   for( i = 0 ; MLISP_AST_IDX_CHILDREN_MAX > i ; i++ ) {
      ast_node.ast_idx_children[i] = -1;
   }
   ast_node.token_idx = -1;
   ast_node.ast_idx_children_sz = 0;

   debug_printf( MLISP_PARSE_TRACE_LVL, "adding node under " SSIZE_T_FMT "...",
      ast_node.ast_idx_parent );

   /* Add the node to the AST and set it as the current node. */
   new_idx_out = mdata_vector_append(
      &(parser->ast), &ast_node, sizeof( struct MLISP_AST_NODE ) );
   if( 0 > new_idx_out ) {
      retval = mdata_retval( new_idx_out );
   }

   /* Find an available child slot on the parent, if there is one. */
   if( 0 <= ast_node.ast_idx_parent ) {
      mdata_vector_lock( &(parser->ast) );

      n_parent = mdata_vector_get(
         &(parser->ast), ast_node.ast_idx_parent, struct MLISP_AST_NODE );

      /* Find the first free child slot. */
      parent_child_idx = 0;
      while( -1 != n_parent->ast_idx_children[parent_child_idx] ) {
         parent_child_idx++;
      }

      n_parent->ast_idx_children[parent_child_idx] = new_idx_out;
      n_parent->ast_idx_children_sz++;

      n_parent = NULL;
      mdata_vector_unlock( &(parser->ast) );
   } else {
      /* Find the first free child slot *on the parser*. */
      /*
      parent_child_idx = 0;
      while( -1 != parser->ast_idx_children[parent_child_idx] ) {
         parent_child_idx++;
      }

      parser->ast_idx_children[parent_child_idx] = new_idx_out;
      */
   }

   parser->ast_node_iter = new_idx_out;

   debug_printf( MLISP_PARSE_TRACE_LVL, "added node " SSIZE_T_FMT
      " under parent: " SSIZE_T_FMT " as child " SSIZE_T_FMT,
      new_idx_out, ast_node.ast_idx_parent, parent_child_idx );

cleanup:

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_ast_set_child_token(
   struct MLISP_PARSER* parser, mdata_strpool_idx_t token_idx, size_t token_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   char* strpool = NULL;
   struct MLISP_AST_NODE* n = NULL;

   mdata_vector_lock( &(parser->ast) );

   n = mdata_vector_get(
      &(parser->ast), parser->ast_node_iter, struct MLISP_AST_NODE );

   mdata_strpool_lock( &(parser->strpool), strpool );
   if( 0 == token_sz ) {
      token_sz = strlen( &(strpool[token_idx]) );
   }
   assert( 0 < token_sz );

   /* Setup flags based on token name. */
   if( 0 == strncmp( &(strpool[token_idx]), "lambda", token_sz ) ) {
      /* Special node: lambda. */
      debug_printf( MLISP_PARSE_TRACE_LVL,
         "setting node \"%s\" (" SIZE_T_FMT ") flag: LAMBDA",
         &(strpool[token_idx]), token_sz );
      n->flags |= MLISP_AST_FLAG_LAMBDA;
   }

   /* Debug report. */
   debug_printf( MLISP_PARSE_TRACE_LVL, "setting node " SSIZE_T_FMT
      " token: \"%s\" (" SIZE_T_FMT ")",
      parser->ast_node_iter, &(strpool[token_idx]), token_sz );
   mdata_strpool_unlock( &(parser->strpool), strpool );

   /* Set the token from the strpool. */
   n->token_idx = token_idx;
   n->token_sz = token_sz;

cleanup:

   mdata_vector_unlock( &(parser->ast) );

   return retval;
}

/* === */

static
MERROR_RETVAL _mlisp_ast_traverse_parent( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_AST_NODE* n = NULL;

   mdata_vector_lock( &(parser->ast) );

   assert( 0 <= parser->ast_node_iter );

   n = mdata_vector_get(
      &(parser->ast), parser->ast_node_iter, struct MLISP_AST_NODE );

   parser->ast_node_iter = n->ast_idx_parent;

   debug_printf( MLISP_PARSE_TRACE_LVL, "moved up to node: " SSIZE_T_FMT,
      parser->ast_node_iter );

cleanup:

   mdata_vector_unlock( &(parser->ast) );

   return retval;
}

/* === */

static
MERROR_RETVAL _mlisp_ast_add_raw_token( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   mdata_strpool_idx_t str_idx = -1;

   str_idx = mdata_strpool_append( &(parser->strpool),
      parser->base.token, parser->base.token_sz );
   if( 0 > str_idx ) {
      error_printf( "invalid str_idx: " SSIZE_T_FMT, str_idx );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   _mlisp_ast_add_child( parser );
   _mlisp_ast_set_child_token( parser, str_idx, parser->base.token_sz );
   mlisp_parser_reset_token( parser );
   retval = _mlisp_ast_traverse_parent( parser );

cleanup:
   return retval;
}

/* === */

MERROR_RETVAL mlisp_ast_dump(
   struct MLISP_PARSER* parser, size_t ast_node_idx, size_t depth, char ab
) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t autolock = 0;
   struct MLISP_AST_NODE* n = NULL;
   char indent[101];
   size_t i = 0;
   char* strpool = NULL;

   if( NULL == parser->ast.data_bytes ) {
      autolock = 1;
      mdata_vector_lock( &(parser->ast) );
      debug_printf( MLISP_TRACE_LVL, "--- BEGIN AST DUMP ---" );
   }

   /* Make indent. */
   maug_mzero( indent, 101 );
   assert( depth < 100 );
   for( i = 0 ; depth > i ; i++ ) {
      indent[i] = ' ';
   }

   if( 0 == ab ) {
      ab = 'X';
   }

   /* Iterate node and children .*/
   n = mdata_vector_get( &(parser->ast), ast_node_idx, struct MLISP_AST_NODE );
   mdata_strpool_lock( &(parser->strpool), strpool );
   debug_printf( MLISP_TRACE_LVL,
      "%s%c: \"%s\" (i: " SIZE_T_FMT ", t: " SSIZE_T_FMT
         ", c: " SSIZE_T_FMT ", f: 0x%02x)",
      indent, ab, 0 <= n->token_idx ? &(strpool[n->token_idx]) : "",
      ast_node_idx, n->token_idx, n->ast_idx_children_sz, n->flags );
   mdata_strpool_unlock( &(parser->strpool), strpool );
   for( i = 0 ; MLISP_AST_IDX_CHILDREN_MAX > i ; i++ ) {
      if( -1 == n->ast_idx_children[i] ) {
         continue;
      }

      mlisp_ast_dump( parser, n->ast_idx_children[i], depth + 1, '0' + i );
   }

cleanup:

   if( NULL != parser->ast.data_bytes && autolock ) {
      mdata_vector_unlock( &(parser->ast) );
      debug_printf( MLISP_TRACE_LVL, "--- END AST DUMP ---" );
   }

   return retval;
}

/* === */

/* Stack Functions */

/* === */

MERROR_RETVAL mlisp_stack_dump(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   char* strpool = NULL;
   union MLISP_VAL* val = NULL;

#  define _MLISP_TYPE_TABLE_DUMPS( idx, ctype, name, const_name, fmt, iv ) \
      if( MLISP_TYPE_ ## const_name == exec->stack[i].type ) { \
         val = &(exec->stack[i].value); \
         debug_printf( MLISP_TRACE_LVL, \
            "stack " SIZE_T_FMT " (" #const_name "): " fmt, \
            i, iv ); \
      }

   mdata_strpool_lock( &(parser->strpool), strpool ); \
   while( i < exec->stack_idx ) {
      MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_DUMPS );
      i++;
   }
   mdata_strpool_unlock( &(parser->strpool), strpool ); \

cleanup:

   return retval;
}

/* === */

#define _MLISP_TYPE_TABLE_PUSH( idx, ctype, name, const_name, fmt, iv ) \
   MERROR_RETVAL _mlisp_stack_push_ ## ctype( \
      struct MLISP_EXEC_STATE* exec, ctype i \
   ) { \
      MERROR_RETVAL retval = MERROR_OK; \
      if( exec->stack_idx + 1 >= MLISP_STACK_DEPTH_MAX ) { \
         error_printf( "stack overflow!" ); \
         retval = MERROR_OVERFLOW; \
         goto cleanup; \
      } \
      exec->stack[exec->stack_idx].type = MLISP_TYPE_ ## const_name; \
      exec->stack[exec->stack_idx].value.name = i; \
      exec->stack_idx++; \
   cleanup: \
      return retval; \
   }

MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_PUSH );

/* === */

MERROR_RETVAL mlisp_stack_pop(
   struct MLISP_EXEC_STATE* exec, struct MLISP_STACK_NODE* o
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Check for valid stack pointer. */
   if( exec->stack_idx == 0 ) {
      error_printf( "stack underflow!" );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   /* Perform the pop! */
   exec->stack_idx--;

   debug_printf( MLISP_EXEC_TRACE_LVL, "popping: " SSIZE_T_FMT, exec->stack_idx );

   memcpy(
      o,
      &(exec->stack[exec->stack_idx]),
      sizeof( struct MLISP_STACK_NODE ) );

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
   union MLISP_VAL* val = NULL;
   struct MLISP_ENV_NODE* e = NULL;

#  define _MLISP_TYPE_TABLE_DUMPE( idx, ctype, name, const_name, fmt, iv ) \
      if( MLISP_TYPE_ ## const_name == e->type ) { \
         val = &(e->value); \
         debug_printf( MLISP_TRACE_LVL, \
            "env \"%s\" (" #const_name "): " fmt, \
            &(strpool[e->name_strpool_idx]), iv ); \
      }

   mdata_strpool_lock( &(parser->strpool), strpool ); \
   mdata_vector_lock( &(parser->env) );
   while( i < mdata_vector_ct( &(parser->env) ) ) {
      e = mdata_vector_get( &(parser->env), i, struct MLISP_ENV_NODE );
      MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_DUMPE );
      i++;
   }
   mdata_vector_unlock( &(parser->env) );
   mdata_strpool_unlock( &(parser->strpool), strpool ); \

cleanup:

   return retval;
}

/* === */

struct MLISP_ENV_NODE* mlisp_env_get_strpool(
   struct MLISP_PARSER* parser, const char* strpool,
   size_t token_strpool_idx, size_t token_strpool_sz
) {
   struct MLISP_ENV_NODE* node_out = NULL;
   struct MLISP_ENV_NODE* node_test = NULL;
   size_t i = 0;

   /* This requires env be locked before entrance! */
   /* TODO: Autolock? */
   
   while( parser->env.ct > i ) {
      node_test = mdata_vector_get( &(parser->env), i, struct MLISP_ENV_NODE );
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
   struct MLISP_PARSER* parser, const char* token, size_t token_sz,
   uint8_t env_type, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_ENV_NODE env_node;
   ssize_t new_idx_out = -1;

#  define _MLISP_TYPE_TABLE_ASGN( idx, ctype, name, const_name, fmt, iv ) \
      case idx: \
         debug_printf( 1, \
            "setting env: %s: #" fmt, \
               token, (ctype)*((ctype*)data) ); \
         env_node.value.name = *((ctype*)data); \
         break;

   /* Setup the new node to copy. */
   maug_mzero( &env_node, sizeof( struct MLISP_ENV_NODE ) );
   env_node.name_strpool_idx =
      mdata_strpool_append( &(parser->strpool), token, token_sz );
   if( 0 > env_node.name_strpool_idx ) {
      retval = mdata_retval( env_node.name_strpool_idx );
   }
   maug_cleanup_if_not_ok();
   env_node.type = env_type;
   switch( env_type ) {
   MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_ASGN );

   /* Special cases: */

   case 3 /* MLISP_TYPE_STR */:
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "setting env: %s: strpool(" SSIZE_T_FMT ")",
            token, *((ssize_t*)data) );
      env_node.value.strpool_idx = *((mdata_strpool_idx_t*)data);
      break;

   case 4 /* MLISP_TYPE_CB */:
      debug_printf( MLISP_EXEC_TRACE_LVL,
         "setting env: %s): 0x%p", token, (mlisp_env_cb_t)data );
      env_node.value.cb = (mlisp_env_cb_t)data;
      break;

   default:
      error_printf( "attempted to define invalid type: %d", env_type );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   /* Add the node to the env. */
   new_idx_out = mdata_vector_append(
      &(parser->env), &env_node, sizeof( struct MLISP_ENV_NODE ) );
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
MERROR_RETVAL _mlisp_env_cb_multiply(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE mults[2];
   char* strpool = NULL;
   /* TODO: Vary type based on multiplied types. */
   int product = 1;
   size_t i = 0;

#  define _MLISP_TYPE_TABLE_MULT( idx, ctype, name, const_name, fmt, iv ) \
      } else if( MLISP_TYPE_ ## const_name == mults[i].type ) { \
         debug_printf( MLISP_EXEC_TRACE_LVL, \
            "multiply: %d * " fmt, product, mults[i].value.name ); \
         product *= mults[i].value.name;

   for( i = 0 ; 2 > i ; i++ ) {
      retval = mlisp_stack_pop( exec, &(mults[i]) );
      maug_cleanup_if_not_ok();

      if( 0 ) {
      MLISP_NUM_TYPE_TABLE( _MLISP_TYPE_TABLE_MULT )
      } else {
         error_printf( "multiply: invalid type!" );
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

static
MERROR_RETVAL _mlisp_env_cb_define(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE key;
   struct MLISP_STACK_NODE val;
   char* strpool = NULL;
   MAUG_MHANDLE key_tmp_h = NULL;
   size_t key_sz = 0;
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

   mdata_strpool_lock( &(parser->strpool), strpool );

   /* Allocate a temporary string to hold the key. */
   key_sz = strlen( &(strpool[key.value.strpool_idx]) );
   key_tmp_h = maug_malloc( key_sz + 1, 1 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, key_tmp_h );

   maug_mlock( key_tmp_h, key_tmp );
   maug_cleanup_if_null_lock( char*, key_tmp );

   maug_mzero( key_tmp, key_sz + 1 );
   strncpy( key_tmp, &(strpool[key.value.strpool_idx]), key_sz );

   debug_printf( MLISP_EXEC_TRACE_LVL, "define \"%s\" (strpool(" SIZE_T_FMT "))...",
      &(strpool[key.value.strpool_idx]), key.value.strpool_idx );

   mdata_strpool_unlock( &(parser->strpool), strpool );
   assert( NULL == strpool );

   retval = mlisp_env_set( parser, key_tmp, 0, val.type, &(val.value) );

cleanup:

   if( NULL != key_tmp ) {
      maug_munlock( key_tmp_h, key_tmp );
   }

   if( NULL != key_tmp_h ) {
      maug_mfree( key_tmp_h );
   }

   if( NULL != strpool ) {
      mdata_strpool_unlock( &(parser->strpool), strpool );
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

   /* Check for special types like lambda, that are lazily evaluated. */
   if( MLISP_AST_FLAG_LAMBDA == (MLISP_AST_FLAG_LAMBDA & n->flags) ) {
      /* TODO: When executing a lambda, pop the stack for each child of the
       *       first s-expression after the name (second absolute?).
       */
      debug_printf( MLISP_EXEC_TRACE_LVL, "skipping lambda..." );
      goto cleanup;
   }

   /* Grab the current exec index for the child vector for this node. */
   p_child_idx = mdata_vector_get(
      &(exec->per_node_child_idx), n_idx, size_t );
   assert( NULL != p_child_idx );
   debug_printf( MLISP_EXEC_TRACE_LVL,
      "child idx for AST node " SIZE_T_FMT ": " SIZE_T_FMT,
      n_idx, *p_child_idx );

   if( n->ast_idx_children_sz > *p_child_idx ) {
      /* Call the next uncalled child. */
      n_child = mdata_vector_get(
         &(parser->ast), n->ast_idx_children[*p_child_idx],
         struct MLISP_AST_NODE );
      retval = _mlisp_step_iter(
         parser, n_child, n->ast_idx_children[*p_child_idx], exec );
      if( MERROR_EXEC != retval ) {
         mdata_strpool_lock( &(parser->strpool), strpool );
         assert( 0 < strlen( &(strpool[n->token_idx]) ) );
         debug_printf( MLISP_EXEC_TRACE_LVL,
            "eval step " SSIZE_T_FMT " under %s...",
            *p_child_idx, &(strpool[n->token_idx]) );
         mdata_strpool_unlock( &(parser->strpool), strpool );

         /* Increment this node, since the child actually executed. */
         (*p_child_idx)++;
      }
      /* Could not exec *this* node yet, so don't increment its parent. */
      retval = MERROR_EXEC;
      goto cleanup;
   }

cleanup:

   return retval;
}

/* === */

static MERROR_RETVAL _mlisp_step_iter(
   struct MLISP_PARSER* parser, struct MLISP_AST_NODE* n,
   size_t n_idx, struct MLISP_EXEC_STATE* exec
) {
   MERROR_RETVAL retval = MERROR_OK;
   char* strpool = NULL;
   struct MLISP_ENV_NODE* env_node_p = NULL;
   struct MLISP_ENV_NODE env_node;

   if(
      MERROR_EXEC ==
      (retval = _mlisp_step_iter_children( parser, n, n_idx, exec ))
   ) {
      goto cleanup;
   }

   /* Now that the children have been evaluated above, evaluate this node.
    * Assume all the previously called children are now on the stack.
    */

   /* Grab the token for this node and figure out what it is. */
   mdata_strpool_lock( &(parser->strpool), strpool );
   mdata_vector_lock( &(parser->env) );
   assert( 0 < strlen( &(strpool[n->token_idx]) ) );
   debug_printf( MLISP_EXEC_TRACE_LVL, "eval %s", &(strpool[n->token_idx]) );
   if( NULL != (env_node_p = mlisp_env_get_strpool(
      parser, strpool, n->token_idx, n->token_sz
   ) ) ) {
      /* A literal found in the environment. */
      debug_printf( MLISP_EXEC_TRACE_LVL, "found %s in env!",
         &(strpool[env_node_p->name_strpool_idx]) );

      /* Copy onto native stack so we can unlock env in case this is a
       * callback that needs to execute. */
      memcpy( &env_node, env_node_p, sizeof( struct MLISP_ENV_NODE ) );
      env_node_p = NULL;

   } else if( maug_is_num( &(strpool[n->token_idx]), n->token_sz ) ) {
      /* A numeric literal. */
      env_node.value.integer = atoi( &(strpool[n->token_idx]) );
      env_node.type = MLISP_TYPE_INT;

   } else if( maug_is_float( &(strpool[n->token_idx]), n->token_sz ) ) {
      env_node.value.floating = atof( &(strpool[n->token_idx]) );
      env_node.type = MLISP_TYPE_FLOAT;
   }
   mdata_vector_unlock( &(parser->env) );
   mdata_strpool_unlock( &(parser->strpool), strpool );

   /* Put the token or its result (if callable) on the stack. */
#  define _MLISP_TYPE_TABLE_ENVE( idx, ctype, name, const_name, fmt, iv ) \
   } else if( MLISP_TYPE_ ## const_name == env_node.type ) { \
      _mlisp_stack_push_ ## ctype( exec, env_node.value.name );

   if( MLISP_TYPE_CB == env_node.type ) {
      /* This is a special case... rather than pushing the callback, *execute*
       * it and let it push its result to the stack. This will create a 
       * redundant case below, but that can't be helped...
       */
      retval = env_node.value.cb( parser, exec );
      maug_cleanup_if_not_ok();

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

   /* Find next unevaluated symbol. */
   retval = _mlisp_step_iter( parser, n, parser->ast_node_iter, exec );
   if( MERROR_EXEC == retval ) {
      retval = 0;
   } else if( 0 == retval ) {
      retval = MERROR_EXEC;
   }
  
cleanup:

   debug_printf( MLISP_EXEC_TRACE_LVL, "heartbeat end: %x", retval );

   mdata_vector_unlock( &(exec->per_node_child_idx) );
   mdata_vector_unlock( &(parser->ast) );

   return retval;
}

/* === */

/* Parse Functions */

/* === */

MERROR_RETVAL mlisp_parse_c( struct MLISP_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;
   mdata_strpool_idx_t str_idx = -1;

#ifdef MPARSER_TRACE_NAMES
   debug_printf( MLISP_PARSE_TRACE_LVL,
      SIZE_T_FMT ": \"%c\" (last: \"%c\") (%s (%d)) (sz: " SIZE_T_FMT ")",
      parser->base.i, c, parser->base.last_c,
      gc_mlisp_pstate_names[mlisp_parser_pstate( parser )],
      mlisp_parser_pstate( parser ),
      parser->base.pstate_sz );
#endif /* MPARSER_TRACE_NAMES */

   switch( c ) {
   case '\r':
   case '\n':
   case '\t':
   case ' ':
      if(
         MLISP_PSTATE_SYMBOL_OP == mlisp_parser_pstate( parser )
         /* Don't terminate the current symbol if the last_c was *any* of the
          * other terminating characters.
          */
         && '\r' != parser->base.last_c
         && '\n' != parser->base.last_c
         && '\t' != parser->base.last_c
         && ' ' != parser->base.last_c
         && ')' != parser->base.last_c
         && '(' != parser->base.last_c
      ) {
         assert( 0 < parser->base.token_sz );
         debug_printf( MLISP_PARSE_TRACE_LVL, "found symbol: %s (" SIZE_T_FMT ")",
            parser->base.token, parser->base.token_sz );

         /* Grab the symbol to use for the op of the child created by the last
          * open paren.
          */
         str_idx = mdata_strpool_append( &(parser->strpool),
            parser->base.token, parser->base.token_sz );
         mlisp_parser_reset_token( parser );
         _mlisp_ast_set_child_token( parser, str_idx, parser->base.token_sz );

         /* Switch from OP to SYMBOL for subsequent tokens. */
         mlisp_parser_pstate_pop( parser );
         retval = mlisp_parser_pstate_push( parser, MLISP_PSTATE_SYMBOL );
         maug_cleanup_if_not_ok();

         /* mlisp_ast_dump( parser, 0, 0, 0 ); */

      } else if(
         MLISP_PSTATE_SYMBOL == mlisp_parser_pstate( parser )
         /* Don't terminate the current symbol if the last_c was *any* of the
          * other terminating characters.
          */
         && '\r' != parser->base.last_c
         && '\n' != parser->base.last_c
         && '\t' != parser->base.last_c
         && ' ' != parser->base.last_c
         && ')' != parser->base.last_c
         && '(' != parser->base.last_c
      ) {
         assert( 0 < parser->base.token_sz );
         debug_printf( MLISP_PARSE_TRACE_LVL, "found symbol: %s (" SIZE_T_FMT ")",
            parser->base.token, parser->base.token_sz );

         /* A raw token without parens terminated by whitespace can't have
          * children, so just create a one-off.
          */
         _mlisp_ast_add_raw_token( parser );

      } else if( MLISP_PSTATE_STRING == mlisp_parser_pstate( parser ) ) {
         retval = mlisp_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      }
      break;

   case '(':
      if(
         MLISP_PSTATE_NONE == mlisp_parser_pstate( parser ) ||
         MLISP_PSTATE_SYMBOL == mlisp_parser_pstate( parser )
      ) {
         /* First symbol after an open paren is an op. */
         retval = mlisp_parser_pstate_push( parser, MLISP_PSTATE_SYMBOL_OP );
         maug_cleanup_if_not_ok();
         mlisp_parser_reset_token( parser );

         /* Add a new empty child to be filled out when tokens are parsed. */
         _mlisp_ast_add_child( parser );

      } else if( MLISP_PSTATE_STRING == mlisp_parser_pstate( parser ) ) {
         retval = mlisp_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mlisp_parser_invalid_c( parser, c, retval );
      }
      break;

   case ')':
      if(
         MLISP_PSTATE_SYMBOL_OP == mlisp_parser_pstate( parser ) ||
         MLISP_PSTATE_SYMBOL == mlisp_parser_pstate( parser )
      ) {
         if( 0 < parser->base.token_sz ) {
            /* A raw token without parens terminated by whitespace can't have
            * children, so just create a one-off.
            */
            _mlisp_ast_add_raw_token( parser );
         }

         /* Reset the parser and AST cursor up one level. */
         mlisp_parser_pstate_pop( parser );
         _mlisp_ast_traverse_parent( parser );

         /* mlisp_ast_dump( parser, 0, 0, 0 ); */

      } else if( MLISP_PSTATE_STRING == mlisp_parser_pstate( parser ) ) {
         retval = mlisp_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mlisp_parser_invalid_c( parser, c, retval );
      }
      break;

   default:
      retval = mlisp_parser_append_token( parser, c );
      maug_cleanup_if_not_ok();
      break;
   }

   mparser_wait( &(parser->base) );

   parser->base.i++;

cleanup:

   parser->base.last_c = c;

   return retval;
}

/* === */

MERROR_RETVAL mlisp_parser_init( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t append_retval = 0;

   debug_printf( MLISP_TRACE_LVL,
      "initializing mlisp parser (" SIZE_T_FMT " bytes)...",
      sizeof( struct MLISP_PARSER ) );

   maug_mzero( parser, sizeof( struct MLISP_PARSER ) );

   parser->ast_node_iter = -1;
   /*
   for( i = 0 ; MLISP_AST_IDX_CHILDREN_MAX > i ; i++ ) {
      parser->ast_idx_children[i] = -1;
   }
   */

   /* Allocate the vectors for AST and ENV. */
 
   append_retval = mdata_vector_alloc(
      &(parser->ast), sizeof( struct MLISP_AST_NODE ), MDATA_VECTOR_INIT_SZ );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();

   append_retval = mdata_vector_alloc(
      &(parser->env), sizeof( struct MLISP_ENV_NODE ), MDATA_VECTOR_INIT_SZ );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();

   /* Setup initial env. */

   retval = mlisp_env_set(
      parser, "define", 6, MLISP_TYPE_CB, _mlisp_env_cb_define );
   maug_cleanup_if_not_ok();
   retval = mlisp_env_set(
      parser, "*", 1, MLISP_TYPE_CB, _mlisp_env_cb_multiply );
   maug_cleanup_if_not_ok();

cleanup:

   if( MERROR_OK != retval ) {
      error_printf( "mlisp parser initialization failed: %d", retval );
   }

   return retval;
}

/* === */

void mlisp_parser_free( struct MLISP_PARSER* parser ) {
   mdata_strpool_free( &(parser->strpool) );
   mdata_vector_free( &(parser->ast) );
   mdata_vector_free( &(parser->env) );
   /* maug_mfree( parser->ast_h ); */
   /* TODO */
}

/* === */

void mlisp_exec_free( struct MLISP_EXEC_STATE* exec ) {
   mdata_vector_free( &(exec->per_node_child_idx) );
}

#else

#  define MLISP_PSTATE_TABLE_CONST( name, idx ) \
      extern MAUG_CONST uint8_t SEG_MCONST name;

MLISP_PARSER_PSTATE_TABLE( MLISP_PSTATE_TABLE_CONST )

#ifdef MPARSER_TRACE_NAMES
extern MAUG_CONST char* SEG_MCONST gc_mlisp_pstate_names[];
#endif /* MPARSER_TRACE_NAMES */

#endif /* MLISP_C */

#endif /* !MLISP_H */

