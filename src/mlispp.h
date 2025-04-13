
#ifndef MLISPP_H
#define MLISPP_H

#include <mlisps.h>

/**
 * \addtogroup mlisp MLISP Interpreter
 * \{
 *
 * \file mlispp.h
 * \brief MLISP Parser Functions and Macros
 */

#ifndef MLISP_PARSE_TRACE_LVL
#  define MLISP_PARSE_TRACE_LVL 0
#endif /* !MLISP_PARSE_TRACE_LVL */

#define MLISP_AST_FLAG_LAMBDA 0x02

#define MLISP_AST_FLAG_IF     0x04

#define MLISP_AST_FLAG_DEFINE 0x08

#define MLISP_AST_FLAG_BEGIN  0x20

#define MLISP_PARSER_PSTATE_TABLE( f ) \
   f( MLISP_PSTATE_NONE, 0 ) \
   f( MLISP_PSTATE_SYMBOL_OP, 1 ) \
   f( MLISP_PSTATE_SYMBOL, 2 ) \
   f( MLISP_PSTATE_STRING, 3 ) \
   f( MLISP_PSTATE_LAMBDA_ARGS, 4 ) \
   f( MLISP_PSTATE_COMMENT, 5 )

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

MERROR_RETVAL mlisp_parse_c( struct MLISP_PARSER* parser, char c );

MERROR_RETVAL mlisp_parse_file(
   struct MLISP_PARSER* parser, const retroflat_asset_path ai_path );

MERROR_RETVAL mlisp_parser_init( struct MLISP_PARSER* parser );

MERROR_RETVAL mlisp_exec_init(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec );

void mlisp_parser_free( struct MLISP_PARSER* parser );

/*! \} */ /* mlisp */

#ifdef MLISPP_C

#define _MLISP_TYPE_TABLE_CONSTS( idx, ctype, name, const_name, fmt ) \
   MAUG_CONST uint8_t SEG_MCONST MLISP_TYPE_ ## const_name = idx;

MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_CONSTS );

MLISP_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_CONST )

MPARSER_PSTATE_NAMES( MLISP_PARSER_PSTATE_TABLE, mlisp )

/* === */

/* AST Functions */

/* === */

static MERROR_RETVAL
_mlisp_ast_add_child( struct MLISP_PARSER* parser, uint8_t flags ) {
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
   ast_node.flags = flags;

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
   assert( NULL != n );

   mdata_strpool_lock( &(parser->strpool), strpool );
   if( 0 == token_sz ) {
      token_sz = maug_strlen( &(strpool[token_idx]) );
   }
   assert( 0 < token_sz );

   /* Setup flags based on token name. */
   if( 0 == strncmp( &(strpool[token_idx]), "lambda", token_sz + 1 ) ) {
      /* Special node: lambda. */
      debug_printf( MLISP_PARSE_TRACE_LVL,
         "setting node \"%s\" (" SIZE_T_FMT ") flag: LAMBDA",
         &(strpool[token_idx]), token_sz );
      n->flags |= MLISP_AST_FLAG_LAMBDA;

   } else if( 0 == strncmp( &(strpool[token_idx]), "if", token_sz + 1 ) ) {
      /* Special node: if. */
      debug_printf( MLISP_PARSE_TRACE_LVL,
         "setting node \"%s\" (" SIZE_T_FMT ") flag: IF",
         &(strpool[token_idx]), token_sz );
      n->flags |= MLISP_AST_FLAG_IF;

   } else if( 0 == strncmp( &(strpool[token_idx]), "begin", token_sz + 1 ) ) {
      /* Special node: begin. */
      debug_printf( MLISP_PARSE_TRACE_LVL,
         "setting node \"%s\" (" SIZE_T_FMT ") flag: BEGIN",
         &(strpool[token_idx]), token_sz );
      n->flags |= MLISP_AST_FLAG_BEGIN;

   } else if( 0 == strncmp( &(strpool[token_idx]), "define", token_sz + 1 ) ) {
      /* Special node: define. */
      debug_printf( MLISP_PARSE_TRACE_LVL,
         "setting node \"%s\" (" SIZE_T_FMT ") flag: DEFINE",
         &(strpool[token_idx]), token_sz );
      n->flags |= MLISP_AST_FLAG_DEFINE;
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

   if( NULL != strpool ) {
      mdata_strpool_unlock( &(parser->strpool), strpool );
   }

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

   _mlisp_ast_add_child( parser, 0 );
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
      debug_printf( MLISP_TRACE_LVL,
         MLISP_TRACE_SIGIL " --- BEGIN AST DUMP ---" );
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
      MLISP_TRACE_SIGIL " %s%c: \"%s\" (i: " SIZE_T_FMT ", t: " SSIZE_T_FMT
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
      debug_printf( MLISP_TRACE_LVL,
         MLISP_TRACE_SIGIL " --- END AST DUMP ---" );
   }

   return retval;
}

/* === */

/* Parse Functions */

/* === */

MERROR_RETVAL mlisp_parse_c( struct MLISP_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;
   mdata_strpool_idx_t str_idx = -1;
   uint8_t n_flags = 0;
   size_t n_children = 0;
   struct MLISP_AST_NODE* n = NULL;

#ifdef MPARSER_TRACE_NAMES
   debug_printf( MLISP_PARSE_TRACE_LVL,
      SIZE_T_FMT ": \"%c\" (last: \"%c\") (%s (%d)) (sz: " SIZE_T_FMT ")",
      parser->base.i, c, parser->base.last_c,
      gc_mlisp_pstate_names[mlisp_parser_pstate( parser )],
      mlisp_parser_pstate( parser ),
      parser->base.pstate_sz );
#endif /* MPARSER_TRACE_NAMES */

   mdata_vector_lock( &(parser->ast) );
   n = mdata_vector_get(
      &(parser->ast), parser->ast_node_iter, struct MLISP_AST_NODE );
   if( NULL != n ) {
      n_flags = n->flags;
      n_children = n->ast_idx_children_sz;
   }
   mdata_vector_unlock( &(parser->ast) );

   switch( c ) {
   case '\r':
   case '\n':
      if( MLISP_PSTATE_COMMENT == mlisp_parser_pstate( parser ) ) {
         /* End comment on newline. */
         mlisp_parser_pstate_pop( parser );
         break;
      }
      
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
         debug_printf( MLISP_PARSE_TRACE_LVL,
            "found symbol: %s (" SIZE_T_FMT ")",
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
         (
            MLISP_PSTATE_SYMBOL == mlisp_parser_pstate( parser ) ||
            MLISP_PSTATE_LAMBDA_ARGS == mlisp_parser_pstate( parser )
         )
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
         debug_printf( MLISP_PARSE_TRACE_LVL,
            "found symbol: %s (" SIZE_T_FMT ")",
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
         if(
            MLISP_AST_FLAG_LAMBDA == (MLISP_AST_FLAG_LAMBDA & n_flags) &&
            0 == n_children
         ) {
            /* Special case: all tokens in this parent are lambda args. */
            retval =
               mlisp_parser_pstate_push( parser, MLISP_PSTATE_LAMBDA_ARGS );
         } else {
            /* Otherwise, first symbol after an open paren is an op. */
            retval = mlisp_parser_pstate_push( parser, MLISP_PSTATE_SYMBOL_OP );
         }
         maug_cleanup_if_not_ok();
         mlisp_parser_reset_token( parser );

         /* Add a new empty child to be filled out when tokens are parsed. */
         _mlisp_ast_add_child( parser, 0 );

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
         MLISP_PSTATE_SYMBOL == mlisp_parser_pstate( parser ) ||
         MLISP_PSTATE_LAMBDA_ARGS == mlisp_parser_pstate( parser )
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

   case ';':
      if( MLISP_PSTATE_COMMENT != mlisp_parser_pstate( parser ) ) {
         mlisp_parser_pstate_push( parser, MLISP_PSTATE_COMMENT );
         break;
      }

   default:
      if( MLISP_PSTATE_COMMENT == mlisp_parser_pstate( parser ) ) {
         break;
      }
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

MERROR_RETVAL mlisp_parse_file(
   struct MLISP_PARSER* parser, const retroflat_asset_path ai_path
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MFILE_CADDY ai_file;
   char c;
   size_t i = 0;

   debug_printf( MLISP_TRACE_LVL, "loading mlisp AST..." );

   retval = mfile_open_read( ai_path, &ai_file );
   maug_cleanup_if_not_ok();

   retval = mlisp_parser_init( parser );
   maug_cleanup_if_not_ok();

   for( i = 0 ; mfile_get_sz( &ai_file ) > i ; i++ ) {
      retval = ai_file.read_int( &ai_file, (uint8_t*)&c, 1, 0 );
      maug_cleanup_if_not_ok();
      retval = mlisp_parse_c( parser, c );
      maug_cleanup_if_not_ok();
   }
   mlisp_ast_dump( parser, 0, 0, 0 );
   if( 0 < parser->base.pstate_sz ) {
      error_printf( "invalid parser state!" );
      retval = MERROR_EXEC;
      goto cleanup;
   }

cleanup:

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
}

#else

#  define _MLISP_TYPE_TABLE_CONSTS( idx, ctype, name, const_name, fmt ) \
      extern MAUG_CONST uint8_t SEG_MCONST MLISP_TYPE_ ## const_name;

MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_CONSTS );

#endif /* MLISPP_C */

#endif /* !MLISPP_H */

