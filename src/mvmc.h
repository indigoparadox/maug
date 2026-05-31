
#ifndef MVMC_H
#define MVMC_H

#ifndef MVMC_TRACE_LVL
#  define MVMC_TRACE_LVL 0
#endif /* !MVMC_TRACE_LVL */

#define MVMC_FLAG_INSTR 0x01

#define MVMC_PARSER_PSTATE_TABLE( f ) \
   f( MVMC_PSTATE_NONE,    0 ) \
   f( MVMC_PSTATE_ASSIGN,  1 ) \
   f( MVMC_PSTATE_CALL,    2 ) \
   f( MVMC_PSTATE_VAR_INT, 3 )

#define MVMC_TOKEN_SZ_MAX 31

#define MVMC_CODE_SZ_MAX 65536
#define MVMC_FUNCS_MAX 32

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

struct MVMC_FUNC {
   char name[MVMC_TOKEN_SZ_MAX + 1];
   int16_t pos;
};

struct MVMC_PARSER {
   struct MPARSER base;
   uint8_t flags;
   struct MVMC_FUNC funcs[MVMC_FUNCS_MAX];
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

/* === */

MERROR_RETVAL mvmc_compile_func( struct MVMC_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   if( MVMC_PSTATE_VAR_INT == mvmc_parser_pstate( parser ) ) {
      debug_printf( MVMC_TRACE_LVL, "parsing function definition: %s",
         parser->base.token );

   } else if( MVMC_PSTATE_NONE == mvmc_parser_pstate( parser ) ) {
      debug_printf( MVMC_TRACE_LVL, "parsing function call: %s",
         parser->base.token );

   } else {
      mvmc_parser_invalid_c( parser, '(', retval );
   }

   return retval;
}

/* === */

MERROR_RETVAL mvmc_compile_token( struct MVMC_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   printf( "compile token: %s\n", parser->base.token );

   if( 0 == strncmp( "int", parser->base.token, 4 ) ) {
      mvmc_parser_pstate_push( parser, MVMC_PSTATE_VAR_INT );

   }

   return retval;
}

/* === */

MERROR_RETVAL mvmc_parse( struct MVMC_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;

   switch( c ) {
   case '\n':
   case '\r':
   case '\t':
   case ' ':
      /* printf( "reset token\n" ); */
      if( 0 < parser->base.token_sz ) {
         mvmc_compile_token( parser );
      }
      mvmc_parser_reset_token( parser );
      break;

   case '(':
      if( 0 >= parser->base.token_sz ) {
         /* TODO: Math grouping. */
         mvmc_parser_invalid_c( parser, c, retval );
      } else {
         mvmc_compile_func( parser );
      }
      mvmc_parser_reset_token( parser );
      break;

   case ')':
      break;

   case '"':
      if( MVMC_FLAG_INSTR == (MVMC_FLAG_INSTR & parser->flags) ) {
         parser->flags &= ~MVMC_FLAG_INSTR;
      } else {
         parser->flags |= MVMC_FLAG_INSTR;
      }
      break;

   default:
      /* printf( "append %c to token(%d): %s\n", c, *p_token_sz, token ); */
      if( MVMC_FLAG_INSTR != (MVMC_FLAG_INSTR & parser->flags) ) {
         mvmc_parser_append_token( parser, c );
      }
      break;
   }

   return retval;
}

#endif /* MVMC_C */

#endif /* !MVMC_H */

