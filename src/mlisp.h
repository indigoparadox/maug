
#ifndef MLISP_H
#define MLISP_H

#ifndef MLISP_TOKEN_SZ_MAX
#  define MLISP_TOKEN_SZ_MAX 4096
#endif /* !MLISP_TOKEN_SZ_MAX */

#ifndef MLISP_TRACE_LVL
#  define MLISP_TRACE_LVL 0
#endif /* !MLISP_TRACE_LVL */

#define MLISP_PARSER_PSTATE_TABLE( f ) \
   f( MLISP_PSTATE_NONE, 0 ) \
   f( MLISP_PSTATE_SYMBOL_OP, 1 ) \
   f( MLISP_PSTATE_SYMBOL_A, 2 ) \
   f( MLISP_PSTATE_SYMBOL_B, 3 ) \
   f( MLISP_PSTATE_STRING, 4 )

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
   ssize_t env_idx_op;
   ssize_t ast_idx_child_a;
   ssize_t ast_idx_child_b;
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
};

#define mlisp_parser_pstate( parser ) \
   ((parser)->base.pstate_sz > 0 ? \
      (parser)->base.pstate[(parser)->base.pstate_sz - 1] : MLISP_PSTATE_NONE)

#ifdef MPARSER_TRACE_NAMES
#  define mlisp_parser_pstate_push( parser, new_pstate ) \
      mparser_pstate_push( \
         "mlisp", &((parser)->base), new_pstate, gc_mlisp_pstate_names )

#  define mlisp_parser_pstate_pop( parser ) \
      mparser_pstate_pop( "mlisp", &((parser)->base), gc_mlisp_pstate_names )
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

MERROR_RETVAL mlisp_parse_c( struct MLISP_PARSER* parser, char c );

void mlisp_parser_free( struct MLISP_PARSER* parser );

#ifdef MLISP_C

MLISP_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_CONST )

MPARSER_PSTATE_NAMES( MLISP_PARSER_PSTATE_TABLE, mlisp )

MERROR_RETVAL mlisp_parse_c( struct MLISP_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t str_idx = -1;

   debug_printf( MLISP_TRACE_LVL, "%c", c );

   switch( c ) {
   case '\r':
   case '\n':
   case '\t':
   case ' ':
      if(
         MLISP_PSTATE_SYMBOL_OP == mlisp_parser_pstate( parser ) ||
         MLISP_PSTATE_SYMBOL_A == mlisp_parser_pstate( parser )
      ) {
         if( 0 >= parser->base.token_sz ) {
            break;
         }

         debug_printf( MLISP_TRACE_LVL, "found symbol: %s",
            parser->base.token );

         /* TODO: Create symbol expression. */
         str_idx = mdata_strtable_append( &(parser->str_table),
            parser->base.token, parser->base.token_sz );

         mlisp_parser_pstate_pop( parser );
         retval = mlisp_parser_pstate_push( parser,
            MLISP_PSTATE_SYMBOL_OP == mlisp_parser_pstate( parser ) ?
            MLISP_PSTATE_SYMBOL_A : MLISP_PSTATE_SYMBOL_B );
         mlisp_parser_reset_token( parser );

      } else if( MLISP_PSTATE_STRING == mlisp_parser_pstate( parser ) ) {
         retval = mlisp_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else if(
         MLISP_PSTATE_SYMBOL_B == mlisp_parser_pstate( parser ) &&
         0 < parser->base.token_sz
      ) {
         mlisp_parser_invalid_c( parser, c, retval );
      }
      break;

   case '(':
      if(
         MLISP_PSTATE_NONE == mlisp_parser_pstate( parser ) ||
         MLISP_PSTATE_SYMBOL_OP == mlisp_parser_pstate( parser ) ||
         MLISP_PSTATE_SYMBOL_A == mlisp_parser_pstate( parser ) ||
         MLISP_PSTATE_SYMBOL_B == mlisp_parser_pstate( parser )
      ) {
         /* TODO: Push new parser expression. */

         /* Prep the parser to interpret symbols. */
         retval = mlisp_parser_pstate_push( parser, MLISP_PSTATE_SYMBOL_OP );
         maug_cleanup_if_not_ok();
         mlisp_parser_reset_token( parser );

      } else if( MLISP_PSTATE_STRING == mlisp_parser_pstate( parser ) ) {
         retval = mlisp_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();
      }
      break;

   case ')':
      if(
         MLISP_PSTATE_SYMBOL_OP == mlisp_parser_pstate( parser ) ||
         MLISP_PSTATE_SYMBOL_A == mlisp_parser_pstate( parser ) ||
         MLISP_PSTATE_SYMBOL_B == mlisp_parser_pstate( parser )
      ) {
         if( 0 >= parser->base.token_sz ) {
            break;
         }

         /* TODO: Create symbol expression. */
         str_idx = mdata_strtable_append( &(parser->str_table),
            parser->base.token, parser->base.token_sz );

         debug_printf( MLISP_TRACE_LVL, "found symbol: %s",
            parser->base.token );
         mlisp_parser_reset_token( parser );
         mlisp_parser_pstate_pop( parser );

         /* TODO: Move back up to parent expression. */

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

void mlisp_parser_free( struct MLISP_PARSER* parser ) {
   mdata_strtable_free( &(parser->str_table) );
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

