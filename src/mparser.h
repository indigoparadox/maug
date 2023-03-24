
#ifndef MPARSER_H
#define MPARSER_H

#ifndef MPARSER_TRACE_LVL
#  define MPARSER_TRACE_LVL 0
#endif /* !MPARSER_TRACE_LVL */

#define mparser_pstate( parser ) \
   (parser->pstate_sz > 0 ? \
      parser->pstate[parser->pstate_sz - 1] : 0)

#define mparser_pstate_push( ptype, parser, new_pstate ) \
   debug_printf( \
      MPARSER_TRACE_LVL, #ptype " parser pstate PREPUSH: %s", \
      gc_ ## ptype ## _pstate_names[mparser_pstate( parser )] ); \
   assert( parser->pstate_sz < MTILEMAP_PSTATE_SZ_MAX ); \
   parser->pstate[parser->pstate_sz++] = new_pstate; \
   debug_printf( \
      MPARSER_TRACE_LVL, #ptype "parser pstate PUSH: %s", \
      gc_ ## ptype ## _pstate_names[mparser_pstate( parser )] );

#define mparser_pstate_pop( ptype, parser ) \
   assert( parser->pstate_sz > 0 ); \
   parser->pstate_sz--; \
   debug_printf( \
      MPARSER_TRACE_LVL, #ptype "parser pstate POP: %s", \
      gc_ ## ptype ## _pstate_names[mparser_pstate( parser )] );

#  define mparser_invalid_c( ptype, parser, c, retval ) \
   error_printf( #ptype "invalid %c detected at char: " SIZE_T_FMT, \
      c, parser->i ); \
   retval = 1;

#  define MPARSER_PSTATE_TABLE_CONST( name, idx ) \
      static MAUG_CONST uint8_t name = idx;

#  define MPARSER_PSTATE_TABLE_NAME( name, idx ) \
      #name,

#endif /* MPARSER_H */

