
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

#ifndef MLISP_AST_IDX_CHILDREN_MAX
#  define MLISP_AST_IDX_CHILDREN_MAX 2
#endif /* !MLISP_AST_IDX_CHILDREN_MAX */

#define MLISP_AST_FLAG_ACTIVE 0x01

#define MLISP_TYPE_TABLE( f ) \
   f( 1, mdata_strpool_idx_t, strpool_idx, STR, "%s", &(strpool[val->strpool_idx]) ) \
   f( 2, mlisp_env_cb_t, cb,              CB,      "%p", val->cb ) \
   f( 3, int16_t,        integer,         INT,     "%d", val->integer ) \
   f( 4, float,          floating,        FLOAT,   "%f", val->floating )

#define MLISP_PARSER_PSTATE_TABLE( f ) \
   f( MLISP_PSTATE_NONE, 0 ) \
   f( MLISP_PSTATE_SYMBOL_OP, 1 ) \
   f( MLISP_PSTATE_SYMBOL, 2 ) \
   f( MLISP_PSTATE_STRING, 3 )

struct MLISP_PARSER;

typedef MERROR_RETVAL (*mlisp_env_cb_t)( struct MLISP_PARSER* parser );

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
   size_t token_idx;
   size_t token_sz;
   /**
    * \brief Next child to evaluate in MLISP_AST_NODE::ast_idx_children when
    *        _mlisp_step_iter() is called on this node.
    * \todo Don't like keeping this state in the AST... need a parallel
    *       structure with a stack and nested step counter state.
    */
   ssize_t step_iter;
   /*! \brief Number of children in MLISP_AST_NODE::ast_idx_children. */
   ssize_t step_iter_max;
   ssize_t ast_idx_parent;
   ssize_t env_idx_op;
   ssize_t ast_idx_children[MLISP_AST_IDX_CHILDREN_MAX];
};

struct MLISP_PARSER {
   struct MPARSER base;
   struct MDATA_STRTABLE strpool;
   MAUG_MHANDLE env_h;
   size_t env_sz;
   size_t env_sz_max;
   struct MDATA_VECTOR ast;
   struct MDATA_VECTOR env;
   ssize_t ast_node_iter;
   ssize_t ast_idx_children[MLISP_AST_IDX_CHILDREN_MAX];
   struct MLISP_STACK_NODE stack[MLISP_STACK_DEPTH_MAX];
   size_t stack_idx;
};

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

#define mlisp_stack_peek( parser ) \
   (0 < (parser)->stack_idx ? \
      (parser)->stack[(parser)->stack_idx - 1].type : 0)

#define mlisp_stack_push( parser, ctype ) \
   (_mlisp_stack_push_ ## ctype( parser, ctype i ))

MERROR_RETVAL mlisp_ast_dump(
   struct MLISP_PARSER* parser, size_t ast_node_idx, size_t depth, char ab );

MERROR_RETVAL mlisp_stack_dump( struct MLISP_PARSER* parser );

MERROR_RETVAL mlisp_stack_pop(
   struct MLISP_PARSER* parser, struct MLISP_STACK_NODE* o );

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

MERROR_RETVAL mlisp_step( struct MLISP_PARSER* parser );

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

   mdata_vector_lock( &(parser->ast) );

   /* Setup the new node to copy. */
   maug_mzero( &ast_node, sizeof( struct MLISP_AST_NODE ) );
   ast_node.ast_idx_parent = parser->ast_node_iter;
   for( i = 0 ; MLISP_AST_IDX_CHILDREN_MAX > i ; i++ ) {
      ast_node.ast_idx_children[i] = -1;
   }
   ast_node.token_idx = -1;
   ast_node.step_iter = 0;
   ast_node.step_iter_max = 0;

   /* Add the node to the AST and set it as the current node. */
   new_idx_out = mdata_vector_append(
      &(parser->ast), &ast_node, sizeof( struct MLISP_AST_NODE ) );
   if( 0 > new_idx_out ) {
      retval = mdata_retval( new_idx_out );
   }

   /* Find an available child slot on the parent, if there is one. */
   if( 0 <= ast_node.ast_idx_parent ) {
      n_parent = mdata_vector_get(
         &(parser->ast), ast_node.ast_idx_parent, struct MLISP_AST_NODE );

      /* Find the first free child slot. */
      parent_child_idx = 0;
      while( -1 != n_parent->ast_idx_children[parent_child_idx] ) {
         parent_child_idx++;
      }

      n_parent->ast_idx_children[parent_child_idx] = new_idx_out;
      n_parent->step_iter_max++;
   } else {
      /* Find the first free child slot *on the parser*. */
      parent_child_idx = 0;
      while( -1 != parser->ast_idx_children[parent_child_idx] ) {
         parent_child_idx++;
      }

      parser->ast_idx_children[parent_child_idx] = new_idx_out;
   }

   parser->ast_node_iter = new_idx_out;

   debug_printf( MLISP_TRACE_LVL, "setup node " SSIZE_T_FMT
      " under parent: " SSIZE_T_FMT " as child " SSIZE_T_FMT,
      new_idx_out, ast_node.ast_idx_parent, parent_child_idx );

cleanup:

   mdata_vector_unlock( &(parser->ast) );

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

   /* Debug report. */
   mdata_strpool_lock( &(parser->strpool), strpool );
   assert( 0 < strlen( &(strpool[token_idx]) ) );
   debug_printf( MLISP_TRACE_LVL, "setting node " SSIZE_T_FMT
      " token: \"%s\"",
      parser->ast_node_iter, &(strpool[token_idx]) );
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

   debug_printf( MLISP_TRACE_LVL, "moved up to node: " SSIZE_T_FMT,
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
      "%s%c: \"%s\" (i: " SIZE_T_FMT ", t: " SIZE_T_FMT ", s: "
         SSIZE_T_FMT ", s_m: " SSIZE_T_FMT ")",
      indent, ab, &(strpool[n->token_idx]),
      ast_node_idx, n->token_idx, n->step_iter, n->step_iter_max );
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

MERROR_RETVAL mlisp_stack_dump( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   char* strpool = NULL;
   union MLISP_VAL* val = NULL;

#  define _MLISP_TYPE_TABLE_DUMP( idx, ctype, name, const_name, fmt, iv ) \
      if( MLISP_TYPE_ ## const_name == parser->stack[i].type ) { \
         val = &(parser->stack[i].value); \
         debug_printf( MLISP_TRACE_LVL, \
            "stack " SIZE_T_FMT " (" #const_name "): " fmt, \
            i, iv ); \
      }

   mdata_strpool_lock( &(parser->strpool), strpool ); \
   while( i < parser->stack_idx ) {
      MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_DUMP );
      i++;
   }
   mdata_strpool_unlock( &(parser->strpool), strpool ); \

cleanup:

   return retval;
}

/* === */

#define _MLISP_TYPE_TABLE_PUSH( idx, ctype, name, const_name, fmt, iv ) \
   MERROR_RETVAL _mlisp_stack_push_ ## ctype( \
      struct MLISP_PARSER* parser, ctype i \
   ) { \
      MERROR_RETVAL retval = MERROR_OK; \
      char* strpool = NULL; \
      union MLISP_VAL* val = NULL; \
      if( parser->stack_idx + 1 >= MLISP_STACK_DEPTH_MAX ) { \
         error_printf( "stack overflow!" ); \
         retval = MERROR_OVERFLOW; \
         goto cleanup; \
      } \
      parser->stack[parser->stack_idx].type = MLISP_TYPE_ ## const_name; \
      parser->stack[parser->stack_idx].value.name = i; \
      mdata_strpool_lock( &(parser->strpool), strpool ); \
      val = &(parser->stack[parser->stack_idx].value); \
      debug_printf( MLISP_TRACE_LVL, "pushing " #const_name ": " fmt, iv ); \
      mdata_strpool_unlock( &(parser->strpool), strpool ); \
      parser->stack_idx++; \
   cleanup: \
      return retval; \
   }

MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_PUSH );

/* === */

MERROR_RETVAL mlisp_stack_pop(
   struct MLISP_PARSER* parser, struct MLISP_STACK_NODE* o
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Check for valid stack pointer. */
   if( parser->stack_idx == 0 ) {
      error_printf( "stack underflow!" );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   /* Perform the pop! */
   parser->stack_idx--;

   debug_printf( MLISP_TRACE_LVL, "popping: " SSIZE_T_FMT, parser->stack_idx );

   memcpy(
      o,
      &(parser->stack[parser->stack_idx]),
      sizeof( struct MLISP_STACK_NODE ) );

cleanup:

   return retval;
}

/* === */

/* Env Functons */

/* === */

struct MLISP_ENV_NODE* mlisp_env_get_strpool(
   struct MLISP_PARSER* parser, const char* strpool,
   size_t token_strpool_idx, size_t token_strpool_sz
) {
   struct MLISP_ENV_NODE* node_out = NULL;
   struct MLISP_ENV_NODE* node_test = NULL;
   size_t i = 0;

   /* This requires env be locked before entrance! */
   /* TODO: Autolock. */
   
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
         memcpy( &(env_node.value.name), &data, sizeof( ctype ) ); \
         break;

   mdata_vector_lock( &(parser->env) );

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
      MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_ASGN );
   }

   /* Add the node to the env. */
   new_idx_out = mdata_vector_append(
      &(parser->env), &env_node, sizeof( struct MLISP_ENV_NODE ) );
   if( 0 > new_idx_out ) {
      retval = mdata_retval( new_idx_out );
   }
   maug_cleanup_if_not_ok();

   debug_printf( MLISP_TRACE_LVL, "setup env node " SSIZE_T_FMT ": %s",
      new_idx_out, token );

cleanup:

   mdata_vector_unlock( &(parser->env) );

   return retval;
}

/* === */

static
MERROR_RETVAL _mlisp_env_cb_multiply( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   return retval;
}

/* === */

static
MERROR_RETVAL _mlisp_env_cb_define( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_STACK_NODE key;
   struct MLISP_STACK_NODE val;
   char* strpool = NULL;

   retval = mlisp_stack_pop( parser, &val );
   maug_cleanup_if_not_ok();

   retval = mlisp_stack_pop( parser, &key );
   maug_cleanup_if_not_ok();

   debug_printf( 1, "---------DEFINE---------" );

   mdata_strpool_lock( &(parser->strpool), strpool );
   debug_printf( MLISP_TRACE_LVL,
      "key %s: %d", &(strpool[key.value.strpool_idx]), val.value.integer );

cleanup:

   mdata_strpool_unlock( &(parser->strpool), strpool );

   return retval;
}

/* === */

/* Execution Functions */

/* === */

static MERROR_RETVAL _mlisp_step_iter(
   struct MLISP_PARSER* parser, struct MLISP_AST_NODE* n
) {
   MERROR_RETVAL retval = MERROR_OK;
   char* strpool = NULL;
   struct MLISP_ENV_NODE* env_node_p = NULL;
   uint8_t env_type = 0;

#  define _MLISP_TYPE_TABLE_ENVD( idx, ctype, name, const_name, fmt, iv ) \
   ctype env_ ## name = (ctype)0;

   MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_ENVD )

   if( n->step_iter_max > n->step_iter ) {
      /* Call the next uncalled child. */
      retval = _mlisp_step_iter( parser,
         mdata_vector_get( &(parser->ast), n->ast_idx_children[n->step_iter],
            struct MLISP_AST_NODE ) );
      if( MERROR_EXEC != retval ) {
         mdata_strpool_lock( &(parser->strpool), strpool );
         assert( 0 < strlen( &(strpool[n->token_idx]) ) );
         debug_printf( 1, SSIZE_T_FMT " increment: %s",
            n->step_iter, &(strpool[n->token_idx]) );
         mdata_strpool_unlock( &(parser->strpool), strpool );

         /* Increment this node, since the child actually executed. */
         n->step_iter++;
      }
      /* Could not exec *this* node yet, so don't increment its parent. */
      retval = MERROR_EXEC;
      goto cleanup;
   }

   /* Evaluate this node, assuming all the previously called children are on
    * the stack.
    */
   /* TODO */

   /* Grab the token for this node and figure out what it is. */
   mdata_strpool_lock( &(parser->strpool), strpool );
   mdata_vector_lock( &(parser->env) );
   assert( 0 < strlen( &(strpool[n->token_idx]) ) );
   debug_printf( MLISP_TRACE_LVL, SSIZE_T_FMT " exec: %s",
      n->step_iter, &(strpool[n->token_idx]) );
   if( NULL != (env_node_p = mlisp_env_get_strpool(
      parser, strpool, n->token_idx, n->token_sz
   ) ) ) {
      /* A literal found in the environment. */
      debug_printf( MLISP_TRACE_LVL, "found %s in env!",
         &(strpool[env_node_p->name_strpool_idx]) );

      /* Copy onto native stack so we can unlock env in case this is a
       * callback that needs to execute. */
#  define _MLISP_TYPE_TABLE_ENV( idx, ctype, name, const_name, fmt, iv ) \
      } else if( MLISP_TYPE_ ## const_name == env_node_p->type ) { \
         env_ ## name = env_node_p->value.name; \

      if( 0 ) {
      MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_ENV )
      }

      env_type = env_node_p->type;
      env_node_p = NULL;

   } else if( maug_is_num( &(strpool[n->token_idx]), n->token_sz ) ) {
      /* A numeric literal. */
      env_integer = atoi( &(strpool[n->token_idx]) );
      env_type = MLISP_TYPE_INT;
   }
   mdata_vector_unlock( &(parser->env) );
   mdata_strpool_unlock( &(parser->strpool), strpool );

   /* Put the token or its result (if callable) on the stack. */
#  define _MLISP_TYPE_TABLE_ENVE( idx, ctype, name, const_name, fmt, iv ) \
   } else if( MLISP_TYPE_ ## const_name == env_type ) { \
      _mlisp_stack_push_ ## ctype( parser, env_ ## name );

   if( MLISP_TYPE_CB == env_type ) {
      /* This is a special case... rather than pushing the callback, *execute*
       * it and let it push its result to the stack. This will create a 
       * redundant case below, but that can't be helped...
       */
      retval = env_cb( parser );
      maug_cleanup_if_not_ok();

   MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_ENVE )
   } else {
      _mlisp_stack_push_mdata_strpool_idx_t( parser, n->token_idx );
   }

   /*
   } else if( is_num ) {
      _mlisp_stack_push_int16_t( parser, i_tmp );
   } else {
      _mlisp_stack_push_mdata_strpool_idx_t( parser, n->token_idx );
   }
   */

cleanup:

   debug_printf( MLISP_TRACE_LVL, "exec done: %d", retval );

   return retval;
}

/* === */

MERROR_RETVAL mlisp_step( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_AST_NODE* n = NULL;

   mdata_vector_lock( &(parser->ast) );

   if( 0 > parser->ast_node_iter ) {
      error_printf( "invalid AST node: " SSIZE_T_FMT, parser->ast_node_iter );
      retval = MERROR_EXEC;
      goto cleanup;
   }

   n = mdata_vector_get(
      &(parser->ast), parser->ast_node_iter, struct MLISP_AST_NODE );
   assert( NULL != n );

   /* Find next unevaluated symbol. */
   retval = _mlisp_step_iter( parser, n );
   if( MERROR_EXEC == retval ) {
      retval = 0;
   } else if( 0 == retval ) {
      retval = MERROR_EXEC;
   }
  
cleanup:

   debug_printf( MLISP_TRACE_LVL, "step: %x", retval );

   mdata_vector_unlock( &(parser->ast) );

   return retval;
}

/* === */

/* Parse Functions */

/* === */

MERROR_RETVAL mlisp_parse_c( struct MLISP_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;
   mdata_strpool_idx_t str_idx = -1;

   debug_printf( MLISP_TRACE_LVL,
      SIZE_T_FMT ": \"%c\" (last: \"%c\") (%s (%d)) (sz: " SIZE_T_FMT ")",
      parser->base.i, c, parser->base.last_c,
      gc_mlisp_pstate_names[mlisp_parser_pstate( parser )],
      mlisp_parser_pstate( parser ),
      parser->base.pstate_sz );

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
         debug_printf( MLISP_TRACE_LVL, "found symbol: %s (" SIZE_T_FMT ")",
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

         mlisp_ast_dump( parser, 0, 0, 0 );

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
         debug_printf( MLISP_TRACE_LVL, "found symbol: %s (" SIZE_T_FMT ")",
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

         mlisp_ast_dump( parser, 0, 0, 0 );

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
   size_t i = 0;

   debug_printf( MLISP_TRACE_LVL,
      "initializing mlisp parser (" SIZE_T_FMT " bytes)...",
      sizeof( struct MLISP_PARSER ) );

   maug_mzero( parser, sizeof( struct MLISP_PARSER ) );

   parser->ast_node_iter = -1;
   for( i = 0 ; MLISP_AST_IDX_CHILDREN_MAX > i ; i++ ) {
      parser->ast_idx_children[i] = -1;
   }

   /* Allocate the vectors for AST and ENV. */
 
   append_retval = mdata_vector_append(
      &(parser->ast), NULL, sizeof( struct MLISP_AST_NODE ) );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
   }
   maug_cleanup_if_not_ok();

   append_retval = mdata_vector_append(
      &(parser->env), NULL, sizeof( struct MLISP_ENV_NODE ) );
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

#else

#  define MLISP_PSTATE_TABLE_CONST( name, idx ) \
      extern MAUG_CONST uint8_t SEG_MCONST name;

MLISP_PARSER_PSTATE_TABLE( MLISP_PSTATE_TABLE_CONST )

#ifdef MPARSER_TRACE_NAMES
extern MAUG_CONST char* SEG_MCONST gc_mlisp_pstate_names[];
#endif /* MPARSER_TRACE_NAMES */

#endif /* MLISP_C */

#endif /* !MLISP_H */

