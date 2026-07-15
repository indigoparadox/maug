
#ifndef MVMC_H
#define MVMC_H

#ifndef MVMC_TRACE_LVL
#  define MVMC_TRACE_LVL 0
#endif /* !MVMC_TRACE_LVL */

#define MVMC_PARSER_PSTATE_TABLE( f ) \
   f( MVMC_PSTATE_NONE,    0 ) \
   f( MVMC_PSTATE_LEX_STR, 1 ) \
   f( MVMC_PSTATE_PARSE_ASSIGN,  2 ) \
   f( MVMC_PSTATE_PARSE_CALL,    3 ) \
   f( MVMC_PSTATE_PARSE_VAR_INT, 4 ) \
   f( MVMC_PSTATE_PARSE_VAR_STR, 5 )

#define MVMC_TOK_VAR       1
#define MVMC_TOK_LCBRACE   2
#define MVMC_TOK_RCBRACE   3
#define MVMC_TOK_LPAREN    4
#define MVMC_TOK_RPAREN    5
#define MVMC_TOK_SEMICOLON 6
#define MVMC_TOK_INT       7
#define MVMC_TOK_STR       8

#define MVMC_TOK_ASSIGN    10
#define MVMC_TOK_DIV       11
#define MVMC_TOK_MUL       12
#define MVMC_TOK_ADD       13
#define MVMC_TOK_SUB       14

#define MVMC_TOK_LT        20
#define MVMC_TOK_GT        21

#define MVMC_TOK_RETURN    30
#define MVMC_TOK_IF        31
#define MVMC_TOK_WHILE     32
#define MVMC_TOK_EOF       33

#define MVMC_TYPE_NONE     0
#define MVMC_TYPE_INT      1
#define MVMC_TYPE_STR      2

#define MVMC_TOKEN_SZ_MAX 127

#define MVMC_CODE_SZ_MAX 65536
#define MVMC_FUNCS_CT_MAX 32
#define MVMC_TOKENS_CT_MAX 1024

#define mvmc_parser_pstate( parser ) mparser_pstate( &((parser)->base) )

#ifdef MPARSER_TRACE_NAMES
#  define mvmc_parser_pstate_push( parser, new_pstate ) \
      mparser_pstate_push( \
         "mvmc", &((parser)->base), new_pstate, gc_mvmc_pstate_names )

#  define mvmc_parser_pstate_pop( parser ) \
      mparser_pstate_pop( "mvmc", &(parser->base), gc_mvmc_pstate_names )
#else
#  define mvmc_parser_pstate_push( parser, new_pstate ) \
      mparser_pstate_push( "mvmc", &((parser)->base), new_pstate )
 
#  define mvmc_parser_pstate_pop( parser ) \
      mparser_pstate_pop( "mvmc", &(parser->base) )
#endif /* MPARSER_TRACE_NAMES */

#define mvmc_parser_reset_token( parser ) \
   mparser_reset_token( "mvmc", &((parser)->base) )

#define mvmc_parser_append_token( parser, c ) \
   mparser_append_token( "mvmc", &((parser)->base), c )

#define mvmc_parser_invalid_c( parser, c, retval ) \
   mparser_invalid_c( mvmc, &((parser)->base), c, retval )

#define mvmc_lex_token_push( parser, tok_type, tok_val ) \
   debug_printf( MVMC_TRACE_LVL, \
      "appending token " #tok_type ": %d", tok_val ); \
   (parser)->tokens[parser->tokens_ct].type = (tok_type); \
   (parser)->tokens[parser->tokens_ct].value = (tok_val); \
   (parser)->tokens_ct++;

struct MVMC_FUNC {
   char name[MVMC_TOKEN_SZ_MAX + 1];
   int16_t pos;
};

struct MVMC_TOKEN {
   uint8_t type;
   int16_t value;
};

struct MVMC_PARSER {
   struct MPARSER base;
   struct MDATA_STRPOOL strings;
   struct MDATA_STRPOOL names;
   struct MVMC_TOKEN tokens[MVMC_TOKENS_CT_MAX];
   size_t tokens_ct;
   struct MVMC_FUNC funcs[MVMC_FUNCS_CT_MAX];
   size_t funcs_ct;
   uint8_t code[MVMC_CODE_SZ_MAX];
   size_t code_sz;
   int16_t pos;
};

#ifdef MVMC_C

MVMC_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_CONST )

MPARSER_PSTATE_NAMES( MVMC_PARSER_PSTATE_TABLE, mvmc )

MERROR_RETVAL mvmc_emit8( struct MVMC_PARSER* parser, uint8_t bc ) {
   if( MVMC_CODE_SZ_MAX >= parser->code_sz ) {
      error_printf( "code overflow!" );
      return MERROR_OVERFLOW;
   }

#if MVMC_TRACE_LVL > 0
   debug_printf( MVMC_TRACE_LVL, "emit instruction: %u", bc );
#endif /* MVMC_TRACE_LVL */

   parser->code[parser->code_sz++] = bc;

   return MERROR_OK;
}

/* === Lexer === */

/* === */

MERROR_RETVAL mvmc_lex_token( struct MVMC_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t name_idx = -1;

   /* We must be opening a new token. */
   maug_cleanup_if_ge_overflow( parser->tokens_ct, MVMC_TOKENS_CT_MAX + 1 );
   if( 0 == strncmp( "int", parser->base.token, 4 ) ) {
      /* TODO: Change to func if terminated with close paren. */
      mvmc_lex_token_push( parser, MVMC_TOK_VAR, MVMC_TYPE_INT );

   } else if( 0 == strncmp( "str", parser->base.token, 4 ) ) {
      mvmc_lex_token_push( parser, MVMC_TOK_VAR, MVMC_TYPE_STR );

   } else if( 0 < parser->base.token_sz ) {
      /* Must be some kind of literal! */
      if(
         maug_is_num( parser->base.token, parser->base.token_sz, 10, 1 )
      ) {
         mvmc_lex_token_push(
            parser, MVMC_TOK_INT, atoi( parser->base.token ) );
      } else {
         name_idx = mdata_strpool_append(
            &(parser->names), parser->base.token, parser->base.token_sz, 0 );
         mvmc_lex_token_push( parser, MVMC_TOK_STR, name_idx );
      }
   }

   switch( c ) {
   case '(':
      mvmc_lex_token_push( parser, MVMC_TOK_LPAREN, 0 );
      break;

   case ')':
      mvmc_lex_token_push( parser, MVMC_TOK_RPAREN, 0 );
      break;

   case '{':
      mvmc_lex_token_push( parser, MVMC_TOK_LCBRACE, 0 );
      break;

   case '}':
      mvmc_lex_token_push( parser, MVMC_TOK_RCBRACE, 0 );
      break;

   case '+':
      mvmc_lex_token_push( parser, MVMC_TOK_ADD, 0 );
      break;

   case '-':
      mvmc_lex_token_push( parser, MVMC_TOK_SUB, 0 );
      break;

   case '/':
      mvmc_lex_token_push( parser, MVMC_TOK_DIV, 0 );
      break;

   case '*':
      mvmc_lex_token_push( parser, MVMC_TOK_MUL, 0 );
      break;

   case ';':
      mvmc_lex_token_push( parser, MVMC_TOK_SEMICOLON, 0 );
      break;

   case '=':
      mvmc_lex_token_push( parser, MVMC_TOK_ASSIGN, 0 );
      break;
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mvmc_lex( struct MVMC_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;

   switch( c ) {
   case '\n':
   case '\r':
   case '\t':
   case ' ':
   case '=':
   case ';':
   case '+':
   case '-':
   case '/':
   case '*':
   case '{':
   case '}':
   case ')':
   case '(':
      if( MVMC_PSTATE_LEX_STR == mvmc_parser_pstate( parser ) ) {
         mvmc_parser_append_token( parser, c );
      } else {
         retval = mvmc_lex_token( parser, c );
         maug_cleanup_if_not_ok();
         mvmc_parser_reset_token( parser );
      }
      break;

   case '"':
      if( MVMC_PSTATE_LEX_STR == mvmc_parser_pstate( parser ) ) {
         mvmc_parser_pstate_pop( parser );
      } else {
         mvmc_parser_pstate_push( parser, MVMC_PSTATE_LEX_STR );
      }
      break;

   default:
      mvmc_parser_append_token( parser, c );
      break;
   }

cleanup:

   return retval;
}

#endif /* MVMC_C */

#endif /* !MVMC_H */

