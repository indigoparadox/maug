
#ifndef MCSS_H
#define MCSS_H

#ifndef MCSS_PARSER_STYLES_INIT_SZ
#  define MCSS_PARSER_STYLES_INIT_SZ 10
#endif /* !MCSS_PARSER_STYLES_INIT_SZ */

#define MCSS_PARSER_PSTATE_TABLE( f ) \
   f( MCSS_PSTATE_NONE, 0 ) \
   f( MCSS_PSTATE_VALUE, 1 )

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
   f( 0, WIDTH, size_t, mcss_style_size_t, mcss_cmp_size_t, 0 ) \
   f( 1, HEIGHT, size_t, mcss_style_size_t, mcss_cmp_size_t, 0 ) \
   f( 2, COLOR, RETROFLAT_COLOR, mcss_style_color, mcss_cmp_color, RETROFLAT_COLOR_NULL ) \
   f( 3, BACKGROUND_COLOR, RETROFLAT_COLOR, mcss_style_color, mcss_cmp_color, RETROFLAT_COLOR_NULL ) \
   f( 4, MARGIN_LEFT, size_t, mcss_style_size_t, mcss_cmp_size_t, 0 ) \
   f( 5, MARGIN_TOP, size_t, mcss_style_size_t, mcss_cmp_size_t, 0 ) \
   f( 6, PADDING_LEFT, size_t, mcss_style_size_t, mcss_cmp_size_t, 0 ) \
   f( 7, PADDING_TOP, size_t, mcss_style_size_t, mcss_cmp_size_t, 0 )

#define MCSS_PROP_TABLE_PROPS( idx, prop_n, prop_t, prop_prse, cmp, def ) \
   prop_t prop_n;

struct MCSS_STYLE {
   MCSS_PROP_TABLE( MCSS_PROP_TABLE_PROPS )
};

struct MCSS_PARSER {
   uint16_t pstate[MPARSER_STACK_SZ_MAX];
   size_t pstate_sz;
   uint16_t prop_key;
   char token[MHTML_PARSER_TOKEN_SZ_MAX];
   size_t token_sz;
   size_t i;
   MAUG_MHANDLE styles_h;
   struct MCSS_STYLE* styles;
   size_t styles_sz;
   size_t styles_sz_max;
   RETROFLAT_COLOR colors[16];
};

MERROR_RETVAL mcss_parse_c( struct MCSS_PARSER* parser, char c );

MERROR_RETVAL mcss_init_parser( struct MCSS_PARSER* parser );

#ifdef MCSS_C

#define MCSS_PROP_TABLE_CONSTS( prop_id, prop_n, prop_t, prop_prse, cmp, def ) \
   MAUG_CONST uint16_t MCSS_PROP_ ## prop_n = prop_id;

MCSS_PROP_TABLE( MCSS_PROP_TABLE_CONSTS )

#define MCSS_PROP_TABLE_NAMES( prop_id, prop_n, prop_t, prop_prse, cmp, def ) \
   #prop_n,

MAUG_CONST char* gc_mcss_prop_names[] = {
   MCSS_PROP_TABLE( MCSS_PROP_TABLE_NAMES )
   ""
};

MCSS_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_CONST )

static MAUG_CONST char* gc_mcss_pstate_names[] = {
   MCSS_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_NAME )
   ""
};

#define MCSS_COLOR_TABLE_NAMES( idx, name_l, name_u, r, g, b ) \
   #name_u,

static MAUG_CONST char* gc_mcss_color_names[] = {
   RETROFLAT_COLOR_TABLE( MCSS_COLOR_TABLE_NAMES )
   ""
};

MERROR_RETVAL mcss_push_prop_key( struct MCSS_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   mparser_normalize_token_case( parser, i );

   /* Figure out style property type. */
   i = 0;
   while( '\0' != gc_mcss_prop_names[i][0] ) {
      if(
         parser->token_sz == strlen( gc_mcss_prop_names[i] ) &&
         0 == strncmp(
            gc_mcss_prop_names[i], parser->token, parser->token_sz )
      ) {
         debug_printf( 1, "selected style (" SSIZE_T_FMT ") property: %s",
            parser->styles_sz - 1, gc_mcss_prop_names[i] );
         parser->prop_key = i;
         goto cleanup;
      }
      i++;
   }

   error_printf( "could not find property: %s", parser->token );

cleanup:

   return retval;
}

MERROR_RETVAL mcss_style_color(
   struct MCSS_PARSER* parser, const char* prop_name, RETROFLAT_COLOR* color_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   mparser_normalize_token_case( parser, i );

   while( '\0' != gc_mcss_color_names[i][0] ) {
      if(
         parser->token_sz == strlen( gc_mcss_color_names[i] ) &&
         0 == strncmp(
            gc_mcss_color_names[i], parser->token, parser->token_sz )
      ) {
         debug_printf( 1, "set %s: %s", prop_name, gc_mcss_color_names[i] );
         *color_out = parser->colors[i];
         goto cleanup;
      }
      i++;
   }

   error_printf( "invalid %s: %s", prop_name, parser->token );

cleanup:

   return retval;
}

int8_t mcss_cmp_color( RETROFLAT_COLOR a, RETROFLAT_COLOR b ) {
   /* TODO: Handle !important. */

   if( RETROFLAT_COLOR_NULL == b ) {
      /* a wins. */
      return 1;
   }

   /* b wins. */
   return -1;
}

MERROR_RETVAL mcss_style_size_t(
   struct MCSS_PARSER* parser, const char* prop_name, size_t* num_out
) {
   MERROR_RETVAL retval = MERROR_OK;

   *num_out = atoi( parser->token );

   debug_printf( 1, "set %s: " SIZE_T_FMT, prop_name, *num_out );

   return retval;
}

int8_t mcss_cmp_size_t( size_t a, size_t b ) {
   /* TODO: Handle !important. */

   if( 0 == b ) {
      /* a wins. */
      return 1;
   }

   /* b wins. */
   return -1;
}

MERROR_RETVAL mcss_push_style_val( struct MCSS_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   #define MCSS_PROP_TABLE_PARSE( idx, prop_n, prop_t, prop_prse, cmp, def ) \
      case idx: \
         retval = prop_prse( \
            parser, #prop_n, \
            &(parser->styles[parser->styles_sz - 1].prop_n) ); \
         maug_cleanup_if_not_ok(); \
         break;

   switch( parser->prop_key ) {
   MCSS_PROP_TABLE( MCSS_PROP_TABLE_PARSE )
   default:
      error_printf( "invalid property: %u", parser->prop_key );
      break;
   }

cleanup:

   return retval;
}

MERROR_RETVAL mcss_parse_c( struct MCSS_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1, "%c", c );

   switch( c ) {
   case ':':
      if( MCSS_PSTATE_NONE == mcss_parser_pstate( parser ) ) {
         retval = mcss_push_prop_key( parser );
         maug_cleanup_if_not_ok();
         mcss_parser_pstate_push( parser, MCSS_PSTATE_VALUE );
         mcss_parser_reset_token( parser );

      } else {
         mcss_parser_invalid_c( parser, c, retval );
      }
      break;

   case ';':
      if( MCSS_PSTATE_VALUE == mcss_parser_pstate( parser ) ) {
         retval = mcss_push_style_val( parser );
         maug_cleanup_if_not_ok();
         mcss_parser_pstate_pop( parser );
         mcss_parser_reset_token( parser );

      } else {
         mcss_parser_invalid_c( parser, c, retval );
      }
      break;

   case '!':
      /* TODO: Handle !important. */
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
      mcss_parser_append_token( parser, c );
      break;
   }

   parser->i++;

cleanup:

   return retval;
}

MERROR_RETVAL mcss_init_style( struct MCSS_STYLE* style ) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( style, sizeof( struct MCSS_STYLE ) );

   #define MCSS_PROP_TABLE_DEFS( idx, prop_n, prop_t, prop_prse, cmp, def ) \
      style->prop_n = def;

   MCSS_PROP_TABLE( MCSS_PROP_TABLE_DEFS )

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

   #define MCSS_COLOR_TABLE_HUES( idx, name_l, name_u, r, g, b ) \
      parser->colors[idx] = RETROFLAT_COLOR_ ## name_u;

   RETROFLAT_COLOR_TABLE( MCSS_COLOR_TABLE_HUES )

cleanup:

   return retval;
}

#endif /* MCSS_C */

#endif /* !MCSS_H */

