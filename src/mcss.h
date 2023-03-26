
#ifndef MCSS_H
#define MCSS_H

#ifndef MCSS_PARSER_STYLES_INIT_SZ
#  define MCSS_PARSER_STYLES_INIT_SZ 10
#endif /* !MCSS_PARSER_STYLES_INIT_SZ */

#define MCSS_PARSER_PSTATE_TABLE( f ) \
   f( MCSS_PSTATE_NONE, 0 ) \
   f( MCSS_PSTATE_KEY, 1 ) \
   f( MCSS_PSTATE_VALUE, 2 )

#define mcss_parser_pstate( parser ) \
   mparser_pstate( parser )

#define mcss_parser_pstate_push( parser, new_pstate ) \
   mparser_pstate_push( mcss, parser, new_pstate )

#define mcss_parser_pstate_pop( parser ) \
   mparser_pstate_pop( mcss, parser )

#define mcss_parser_invalid_c( parser, c, retval ) \
   mparser_invalid_c( mcss, parser, c, retval )

#define mcss_parser_reset_token( parser ) \
   mparser_reset_token( mcss, parser )

#define mcss_parser_append_token( parser, c ) \
   mparser_append_token( mcss, parser, c, MHTML_PARSER_TOKEN_SZ_MAX )

#define mcss_parser_lock( parser ) \
   if( NULL == (parser)->styles ) { \
      maug_mlock( (parser)->styles_h, (parser)->styles ); \
      maug_cleanup_if_null_alloc( struct MCSS_STYLE*, (parser)->styles ); \
   }

#define mcss_parser_unlock( parser ) \
   if( NULL != (parser)->styles ) { \
      maug_munlock( (parser)->styles_h, (parser)->styles ); \
   }

#define MCSS_PROP_TABLE( f ) \
   f( 0, WIDTH, size_t ) \
   f( 1, HEIGHT, size_t ) \
   f( 2, COLOR, RETROFLAT_COLOR ) \
   f( 3, BACKGROUND_COLOR, RETROFLAT_COLOR )

#define MCSS_PROP_TABLE_PROPS( idx, prop_name, prop_type ) \
   prop_type prop_name;

struct MCSS_STYLE {
   MCSS_PROP_TABLE( MCSS_PROP_TABLE_PROPS )
};

struct MCSS_PARSER {
   uint16_t pstate[MPARSER_STACK_SZ_MAX];
   size_t pstate_sz;
   char token[MHTML_PARSER_TOKEN_SZ_MAX];
   size_t token_sz;
   size_t i;
   MAUG_MHANDLE styles_h;
   struct MCSS_STYLE* styles;
   size_t styles_sz;
   size_t styles_sz_max;
};

MERROR_RETVAL mcss_parse_c( struct MCSS_PARSER* parser, char c );

MERROR_RETVAL mcss_init_parser( struct MCSS_PARSER* parser );

#ifdef MCSS_C

MCSS_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_CONST )

MERROR_RETVAL mcss_parse_c( struct MCSS_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;

   switch( c ) {
   case ':':
      break;

   case ';':
      break;

   case '\t':
   case '\r':
   case '\n':
   case ' ':
      if( MCSS_PSTATE_NONE == mcss_parser_pstate( parser ) ) {
         /* Avoid a token that's only whitespace. */
         if( 0 < parser->token_sz ) {
            mcss_parser_append_token( parser, ' ' );
         }
      }
      break;

   default:
      break;
   }

   parser->i++;

cleanup:

   return retval;
}

MERROR_RETVAL mcss_init_parser( struct MCSS_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( 0 == parser->styles_sz );
   assert( (MAUG_MHANDLE)NULL == parser->styles );

   /* Perform initial tag allocation. */
   parser->styles_sz_max = MCSS_PARSER_STYLES_INIT_SZ;
   debug_printf( 1, "allocating " SIZE_T_FMT " styles...",
      parser->styles_sz_max );
   parser->styles_h = maug_malloc(
      parser->styles_sz_max, sizeof( struct MCSS_STYLE ) );
   assert( (MAUG_MHANDLE)NULL != parser->styles_h );
   if( NULL == parser->styles_h ) {
      error_printf( "unable to allocate " SIZE_T_FMT " styles!",
         parser->styles_sz_max );
      goto cleanup;
   }

cleanup:

   return retval;
}

#endif /* MCSS_C */

#endif /* !MCSS_H */

