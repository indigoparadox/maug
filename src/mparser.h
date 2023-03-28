
#ifndef MPARSER_H
#define MPARSER_H

#ifndef MPARSER_TRACE_LVL
#  define MPARSER_TRACE_LVL 0
#endif /* !MPARSER_TRACE_LVL */

#ifndef MPARSER_STACK_SZ_MAX
#  define MPARSER_STACK_SZ_MAX 256
#endif /* !MPARSER_STACK_SZ_MAX */

typedef MERROR_RETVAL (*mparser_cb)( void* parser, char c );

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

MERROR_RETVAL mparser_file(
   const char* filename, void* parser, mparser_cb parser_cb );

#ifdef MPARSER_C

MERROR_RETVAL mparser_file(
   const char* filename, void* parser, mparser_cb parser_cb
) {
   MERROR_RETVAL retval = MERROR_OK;
   FILE* input_file = NULL;
   size_t input_file_sz = 0,
      read_sz = 0,
      i = 0;
   MAUG_MHANDLE input_file_data_h = (MAUG_MHANDLE)NULL;
   char* input_file_data = NULL;

   debug_printf( 1, "opening %s...", filename );

   input_file = fopen( filename, "r" );
   maug_cleanup_if_null( FILE*, input_file, MERROR_FILE );

   fseek( input_file, 0, SEEK_END );
   input_file_sz = ftell( input_file );
   fseek( input_file, 0, SEEK_SET );

   input_file_data_h = maug_malloc( input_file_sz, 1 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, input_file_data_h );

   maug_mlock( input_file_data_h, input_file_data );
   maug_cleanup_if_null_alloc( char*, input_file_data );

   read_sz = fread( input_file_data, 1, input_file_sz, input_file );
   maug_cleanup_if_lt_overflow( read_sz, input_file_sz );

   /* Parse file. */
   maug_cleanup_if_not_ok();
   for( i = 0 ; input_file_sz > i ; i++ ) {
      retval = parser_cb( parser, input_file_data[i] );
      maug_cleanup_if_not_ok();
   }

cleanup:

   if( NULL != input_file_data ) {
      maug_munlock( input_file_data_h, input_file_data );
   }

   if( NULL != input_file_data_h ) {
      maug_mfree( input_file_data_h );
   }

   if( NULL != input_file ) {
      fclose( input_file );
   }

   return retval;
}



#endif /* MPARSER_C */

#endif /* MPARSER_H */

