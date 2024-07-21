
#ifndef MLISP_H
#define MLISP_H

#ifndef MLISP_TOKEN_SZ_MAX
#  define MLISP_TOKEN_SZ_MAX 4096
#endif /* !MLISP_TOKEN_SZ_MAX */

#ifndef MLISP_TRACE_LVL
#  define MLISP_TRACE_LVL 0
#endif /* !MLISP_TRACE_LVL */

#ifndef MLISP_AST_INIT_SZ
#  define MLISP_AST_INIT_SZ 10
#endif /* !MLISP_AST_INIT_SZ */

#ifndef MLISP_AST_IDX_CHILDREN_MAX
#  define MLISP_AST_IDX_CHILDREN_MAX 2
#endif /* !MLISP_AST_IDX_CHILDREN_MAX */

#define MLISP_AST_FLAG_ACTIVE 0x01

#define MLISP_PARSER_PSTATE_TABLE( f ) \
   f( MLISP_PSTATE_NONE, 0 ) \
   f( MLISP_PSTATE_SYMBOL_OP, 1 ) \
   f( MLISP_PSTATE_SYMBOL, 2 ) \
   f( MLISP_PSTATE_STRING, 3 )

typedef MERROR_RETVAL
(*mlisp_parse_token_cb)( const char* token, size_t token_sz, void* arg );

typedef MERROR_RETVAL (*mlisp_cb_t)( void* data );

union MLISP_ENV_VAL {
   ssize_t str_table_index;
   mlisp_cb_t cb;
   int16_t integer;
};

struct MLISP_ENV {
   uint8_t type;
   ssize_t name_str_table_index;
   union MLISP_ENV_VAL value;
};

struct MLISP_AST_NODE {
   uint8_t flags;
   size_t token_idx;
   ssize_t ast_idx_parent;
   ssize_t env_idx_op;
   ssize_t ast_idx_children[MLISP_AST_IDX_CHILDREN_MAX];
};

struct MLISP_PARSER {
   struct MPARSER base;
   struct MDATA_STRTABLE str_table;
   MAUG_MHANDLE env_h;
   size_t env_sz;
   size_t env_sz_max;
   MAUG_MHANDLE ast_h;
   size_t ast_sz;
   size_t ast_sz_max;
   ssize_t ast_node_iter;
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

MERROR_RETVAL mlisp_ast_dump(
   struct MLISP_PARSER* parser, size_t ast_node_idx,
   struct MLISP_AST_NODE* ast_nodes, size_t depth, char ab );

MERROR_RETVAL mlisp_parse_c( struct MLISP_PARSER* parser, char c );

void mlisp_parser_free( struct MLISP_PARSER* parser );

#ifdef MLISP_C

MLISP_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_CONST )

MPARSER_PSTATE_NAMES( MLISP_PARSER_PSTATE_TABLE, mlisp )

/* TODO: Switch back to retval directly. */
static MERROR_RETVAL
_mlisp_ast_add_child( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_AST_NODE* ast_nodes = NULL;
   MAUG_MHANDLE ast_h_new = NULL;
   ssize_t parser_child_idx_out = 0;
   ssize_t child_idx_iter = -1;
   size_t i = 0;
   
   /* Make sure there are free nodes. */
   if( (MAUG_MHANDLE)NULL == parser->ast_h ) {
      assert( 0 == parser->ast_sz_max );

      parser->ast_sz_max = MLISP_AST_INIT_SZ;
      debug_printf(
         MDATA_TRACE_LVL, "creating AST of " SIZE_T_FMT " nodes...",
         parser->ast_sz_max );
      parser->ast_h = maug_malloc(
         parser->ast_sz_max, sizeof( struct MLISP_AST_NODE ) );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, parser->ast_h );

   } else {
      debug_printf(
         MDATA_TRACE_LVL, "enlarging string table to " SIZE_T_FMT "...",
         parser->ast_sz_max * 2 );
      maug_mrealloc_test(
         ast_h_new, parser->ast_h, parser->ast_sz_max * 2,
         sizeof( struct MLISP_AST_NODE ) );
      parser->ast_sz_max *= 2;
   }

   maug_mlock( parser->ast_h, ast_nodes );
   maug_cleanup_if_null_lock( struct MLISP_AST_NODE*, ast_nodes );

   /* Find a free node. */
   while(
      MLISP_AST_FLAG_ACTIVE ==
      (MLISP_AST_FLAG_ACTIVE & ast_nodes[parser_child_idx_out].flags)
   ) {
      parser_child_idx_out++;
   }

   assert( parser_child_idx_out < parser->ast_sz_max );

   /* Find an available child slot. */
   if( 0 <= parser->ast_node_iter ) {
      child_idx_iter = 0;
      while(
         -1 !=
         ast_nodes[parser->ast_node_iter].ast_idx_children[child_idx_iter]
      ) {
         child_idx_iter++;
      }
   }

   debug_printf( MLISP_TRACE_LVL, "setting up node " SSIZE_T_FMT
      " under parent: " SSIZE_T_FMT " as child " SSIZE_T_FMT,
      parser_child_idx_out, parser->ast_node_iter, child_idx_iter );

   if( 0 <= parser->ast_node_iter ) {
      /* Add child node based on parser state. */
      /* TODO: Don't use parster start! Assign to first -1! */
      ast_nodes[parser->ast_node_iter].ast_idx_children[child_idx_iter] =
         parser_child_idx_out;
   }

   ast_nodes[parser_child_idx_out].flags |= MLISP_AST_FLAG_ACTIVE;
   ast_nodes[parser_child_idx_out].ast_idx_parent = parser->ast_node_iter;
   for( i = 0 ; MLISP_AST_IDX_CHILDREN_MAX > i ; i++ ) {
      ast_nodes[parser_child_idx_out].ast_idx_children[i] = -1;
   }
   ast_nodes[parser_child_idx_out].token_idx = -1;
   parser->ast_node_iter = parser_child_idx_out;

cleanup:

   if( NULL != ast_nodes ) {
      maug_munlock( parser->ast_h, ast_nodes );
   }

   return retval;
}

/* === */

static MERROR_RETVAL
_mlisp_ast_set_child_token( struct MLISP_PARSER* parser, ssize_t token_idx ) {
   MERROR_RETVAL retval = MERROR_OK;
   char* str_table = NULL;
   struct MLISP_AST_NODE* ast_nodes = NULL;

   maug_mlock( parser->ast_h, ast_nodes );
   maug_cleanup_if_null_lock( struct MLISP_AST_NODE*, ast_nodes );

   /* Debug report. */
   mdata_strtable_lock( &(parser->str_table), str_table );
   assert( 0 < strlen( &(str_table[token_idx]) ) );
   debug_printf( MLISP_TRACE_LVL, "setting node " SSIZE_T_FMT
      " token: \"%s\"",
      parser->ast_node_iter, &(str_table[token_idx]) );
   mdata_strtable_unlock( &(parser->str_table), str_table );

   /* Set the token from the str_table. */
   ast_nodes[parser->ast_node_iter].token_idx = token_idx;

cleanup:

   if( NULL != ast_nodes ) {
      maug_munlock( parser->ast_h, ast_nodes );
   }

   return retval;
}

/* === */

static
MERROR_RETVAL _mlisp_ast_traverse_parent( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MLISP_AST_NODE* ast_nodes = NULL;

   maug_mlock( parser->ast_h, ast_nodes );
   maug_cleanup_if_null_lock( struct MLISP_AST_NODE*, ast_nodes );

   assert( 0 <= parser->ast_node_iter );

   parser->ast_node_iter = ast_nodes[parser->ast_node_iter].ast_idx_parent;

   debug_printf( MLISP_TRACE_LVL, "moved up to node: " SSIZE_T_FMT,
      parser->ast_node_iter );

cleanup:

   if( NULL != ast_nodes ) {
      maug_munlock( parser->ast_h, ast_nodes );
   }

   return retval;
}

/* === */

MERROR_RETVAL mlisp_ast_dump(
   struct MLISP_PARSER* parser, size_t ast_node_idx,
   struct MLISP_AST_NODE* ast_nodes, size_t depth, char ab
) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t autolock = 0;
   struct MLISP_AST_NODE* n = NULL;
   char indent[101];
   size_t i = 0;
   char* str_table = NULL;

   if( NULL == ast_nodes ) {
      autolock = 1;
      maug_mlock( parser->ast_h, ast_nodes );
      maug_cleanup_if_null_lock( struct MLISP_AST_NODE*, ast_nodes );
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
   n = &(ast_nodes[ast_node_idx]);
   mdata_strtable_lock( &(parser->str_table), str_table );
   debug_printf( MLISP_TRACE_LVL,
      "%s%c: \"%s\" (i: " SIZE_T_FMT ", t: " SIZE_T_FMT ")",
      indent, ab, &(str_table[n->token_idx]),
      ast_node_idx, n->token_idx );
   mdata_strtable_unlock( &(parser->str_table), str_table );
   for( i = 0 ; MLISP_AST_IDX_CHILDREN_MAX > i ; i++ ) {
      if( -1 == n->ast_idx_children[i] ) {
         continue;
      }

      mlisp_ast_dump(
         parser, n->ast_idx_children[i], ast_nodes, depth + 1, '0' + i );
   }

cleanup:

   if( NULL != ast_nodes && autolock ) {
      maug_munlock( parser->ast_h, ast_nodes );
      debug_printf( MLISP_TRACE_LVL, "--- END AST DUMP ---" );
   }

   return retval;
}

/* === */

MERROR_RETVAL _mlisp_ast_add_raw_token( struct MLISP_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t str_idx = -1;

   str_idx = mdata_strtable_append( &(parser->str_table),
      parser->base.token, parser->base.token_sz );
   if( 0 > str_idx ) {
      error_printf( "invalid str_idx: " SSIZE_T_FMT, str_idx );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   _mlisp_ast_add_child( parser );
   _mlisp_ast_set_child_token( parser, str_idx );
   mlisp_parser_reset_token( parser );
   retval = _mlisp_ast_traverse_parent( parser );

cleanup:
   return retval;
}

/* === */

MERROR_RETVAL mlisp_parse_c( struct MLISP_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t str_idx = -1;
   mparser_pstate_t prev_state = 0;

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
         str_idx = mdata_strtable_append( &(parser->str_table),
            parser->base.token, parser->base.token_sz );
         mlisp_parser_reset_token( parser );
         _mlisp_ast_set_child_token( parser, str_idx );

         /* Switch from OP to SYMBOL for subsequent tokens. */
         mlisp_parser_pstate_pop( parser );
         retval = mlisp_parser_pstate_push( parser, MLISP_PSTATE_SYMBOL );
         maug_cleanup_if_not_ok();

         mlisp_ast_dump( parser, 0, NULL, 0, 0 );

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

         mlisp_ast_dump( parser, 0, NULL, 0, 0 );

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

void mlisp_parser_free( struct MLISP_PARSER* parser ) {
   mdata_strtable_free( &(parser->str_table) );
   maug_mfree( parser->ast_h );
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

