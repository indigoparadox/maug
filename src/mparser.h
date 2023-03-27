
#ifndef MPARSER_H
#define MPARSER_H

#ifndef MPARSER_TRACE_LVL
#  define MPARSER_TRACE_LVL 0
#endif /* !MPARSER_TRACE_LVL */

#ifndef MPARSER_STACK_SZ_MAX
#  define MPARSER_STACK_SZ_MAX 256
#endif /* !MPARSER_STACK_SZ_MAX */

/* Normalize token case. */
#define mparser_token_upper( parser, i ) \
   for( i = 0 ; parser->token_sz > i ; i++ ) { \
      if( 0x61 <= parser->token[i] && 0x7a >= parser->token[i] ) { \
         parser->token[i] -= 0x20; \
      } \
   }

/* Normalize token case. */
#define mparser_token_replace( parser, i, c, r ) \
   for( i = 0 ; parser->token_sz > i ; i++ ) { \
      if( c == parser->token[i] ) { \
         parser->token[i] = r; \
      } \
   }

#define mparser_pstate( parser ) \
   (parser->pstate_sz > 0 ? \
      parser->pstate[parser->pstate_sz - 1] : 0)

#define mparser_pstate_push( ptype, parser, new_pstate ) \
   debug_printf( \
      MPARSER_TRACE_LVL, #ptype " parser pstate PREPUSH: %s", \
      gc_ ## ptype ## _pstate_names[mparser_pstate( parser )] ); \
   /* TODO: Use retval check. */ \
   assert( parser->pstate_sz < MPARSER_STACK_SZ_MAX ); \
   parser->pstate[parser->pstate_sz++] = new_pstate; \
   debug_printf( \
      MPARSER_TRACE_LVL, #ptype " parser pstate PUSH: %s", \
      gc_ ## ptype ## _pstate_names[mparser_pstate( parser )] );

#define mparser_pstate_pop( ptype, parser ) \
   assert( parser->pstate_sz > 0 ); \
   parser->pstate_sz--; \
   debug_printf( \
      MPARSER_TRACE_LVL, #ptype " parser pstate POP: %s", \
      gc_ ## ptype ## _pstate_names[mparser_pstate( parser )] );

#define mparser_invalid_c( ptype, parser, c, retval ) \
   error_printf( #ptype " parser invalid %c detected at char: " SIZE_T_FMT, \
      c, parser->i ); \
   retval = 1;

#define mparser_reset_token( ptype, parser ) \
   parser->token_sz = 0; \
   parser->token[parser->token_sz] = '\0'; \
   debug_printf( MPARSER_TRACE_LVL, #ptype " parser reset token" );

#define mparser_append_token( ptype, parser, c, token_sz_max ) \
   parser->token[parser->token_sz++] = c; \
   parser->token[parser->token_sz] = '\0'; \
   /* If size greater than max, return error indicating more buffer space
    * needed. */ \
   maug_cleanup_if_ge_overflow( \
      parser->token_sz + 1, (size_t)token_sz_max );

#define MPARSER_PSTATE_TABLE_CONST( name, idx ) \
   static MAUG_CONST uint8_t name = idx;

#define MPARSER_PSTATE_TABLE_NAME( name, idx ) \
   #name,

#endif /* MPARSER_H */

