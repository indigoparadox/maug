
#ifndef MPARSER_H
#define MPARSER_H

/**
 * \addtogroup maug_parser Maug Parser API
 * \brief Convenience macros for writing parser state machines.
 * \{
 */

/**
 * \file mparser.h
 */

#ifndef MPARSER_TRACE_LVL
#  define MPARSER_TRACE_LVL 0
#endif /* !MPARSER_TRACE_LVL */

#ifndef MPARSER_STACK_SZ_MAX
#  define MPARSER_STACK_SZ_MAX 256
#endif /* !MPARSER_STACK_SZ_MAX */

#ifndef MPARSER_TOKEN_SZ_MAX
/* TODO: Use a dynamically allocated block and enlarge as needed. */
#  define MPARSER_TOKEN_SZ_MAX 4096
#endif /* !MPARSER_TOKEN_SZ_MAX */

#ifndef MPARSER_WAIT_INC
#  define MPARSER_WAIT_INC 100
#endif /* !MPARSER_WAIT_INC */

#if 0
ifdef MPARSER_TRACE_NAMES
  define mparser_trace_printf( phase, ptype, parser ) \
   debug_printf( \
      MPARSER_TRACE_LVL, #ptype " parser pstate " phase ": %s", \
      gc_ ## ptype ## _pstate_names[mparser_pstate( parser )] ); \
else
endif /* MPARSER_TRACE_NAMES */
#endif
#  define mparser_trace_printf( phase, ptype, parser )

typedef uint8_t mparser_pstate_t;

typedef MERROR_RETVAL (*mparser_cb)( void* parser, char c );

typedef MERROR_RETVAL (*mparser_wait_cb_t)(
   MERROR_RETVAL retval_in, void* parser, void* data );

struct MPARSER {
   mparser_pstate_t pstate[MPARSER_STACK_SZ_MAX];
   size_t pstate_sz;
   mparser_wait_cb_t wait_cb;
   void* wait_data;
   maug_ms_t wait_next;
   char token[MPARSER_TOKEN_SZ_MAX];
   size_t token_sz;
   size_t i;
};

/**
 * \brief Given a pstate table, generate numeric constants.
 */
#define MPARSER_PSTATE_TABLE_CONST( name, idx ) \
   MAUG_CONST uint8_t SEG_MCONST name = idx;

/**
 * \brief Given a pstate table, generate a quoted name for each.
 */
#define MPARSER_PSTATE_TABLE_NAME( name, idx ) \
   #name,

#ifdef MPARSER_TRACE_NAMES
#  define MPARSER_PSTATE_NAMES( table, global_id ) \
      MAUG_CONST char* gc_ ## global_id ## _pstate_names[] = { \
         table( MPARSER_PSTATE_TABLE_NAME ) \
         "" \
      };
#else
/**
 * \brief Given a pstate table, generate a list of quoted names if that
 *        level of debugging is enabled.
 */
#  define MPARSER_PSTATE_NAMES( table, global_id )
#endif /* MPARSER_TRACE_NAMES */

/* TODO: Function names should be verb_noun! */

/* Normalize token case. */
#define mparser_token_upper( parser, i ) \
   for( i = 0 ; (parser)->token_sz > i ; i++ ) { \
      if( 0x61 <= (parser)->token[i] && 0x7a >= (parser)->token[i] ) { \
         (parser)->token[i] -= 0x20; \
      } \
   }

/* Normalize token case. */
#define mparser_token_replace( parser, i, c, r ) \
   for( i = 0 ; (parser)->token_sz > i ; i++ ) { \
      if( c == (parser)->token[i] ) { \
         (parser)->token[i] = r; \
      } \
   }

#define mparser_pstate( parser ) \
   ((parser)->pstate_sz > 0 ? \
      (parser)->pstate[(parser)->pstate_sz - 1] : 0)

#define mparser_pstate_push( ptype, parser, new_pstate ) \
   mparser_trace_printf( "PREPUSH", #ptype, parser ); \
   /* TODO: Use retval check. */ \
   assert( (parser)->pstate_sz < MPARSER_STACK_SZ_MAX ); \
   (parser)->pstate[(parser)->pstate_sz++] = new_pstate; \
   mparser_trace_printf( "POSTPUSH", #ptype, parser );

#define mparser_invalid_c( ptype, parser, c, retval ) \
   error_printf( #ptype " parser invalid %c detected at char: " \
      SIZE_T_FMT ", state: %u", c, (parser)->i, mparser_pstate( parser ) ); \
   retval = MERROR_PARSE;

#define mparser_reset_token( ptype, parser ) \
   (parser)->token_sz = 0; \
   (parser)->token[(parser)->token_sz] = '\0'; \
   debug_printf( MPARSER_TRACE_LVL, #ptype " parser reset token" );

#define mparser_append_token( ptype, parser, c, token_sz_max ) \
   (parser)->token[(parser)->token_sz++] = c; \
   (parser)->token[(parser)->token_sz] = '\0'; \
   /* If size greater than max, return error indicating more buffer space
    * needed. */ \
   maug_cleanup_if_ge_overflow( \
      (parser)->token_sz + 1, (size_t)token_sz_max );

#define mparser_wait( parser ) \
   if( \
      NULL != (parser)->wait_cb && \
      retroflat_get_ms() >= (parser)->wait_next \
   ) { \
      (parser)->wait_next = retroflat_get_ms() + MPARSER_WAIT_INC; \
      retval = (parser)->wait_cb( retval, (parser), (parser)->wait_data ); \
   }

mparser_pstate_t
mparser_pstate_pop( const char* ptype, struct MPARSER* parser );

/* \} */ /* maug_parser */

#ifdef MPARSER_C

mparser_pstate_t
mparser_pstate_pop( const char* ptype, struct MPARSER* parser ) {
   mparser_pstate_t pstate_popped;
   
   assert( (parser)->pstate_sz > 0 );

   pstate_popped = (parser)->pstate_sz - 1;
   (parser)->pstate_sz--;
   mparser_trace_printf( "POP", ptype, parser );

   return pstate_popped;
}

#endif /* MPARSER_C */

#endif /* MPARSER_H */

