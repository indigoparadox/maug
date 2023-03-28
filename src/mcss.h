
#ifndef MCSS_H
#define MCSS_H

#ifndef MCSS_PARSER_STYLES_INIT_SZ
#  define MCSS_PARSER_STYLES_INIT_SZ 10
#endif /* !MCSS_PARSER_STYLES_INIT_SZ */

#ifndef MCSS_ID_SZ_MAX
#  define MCSS_ID_SZ_MAX 32
#endif /* !MCSS_ID_SZ_MAX */

#ifndef MCSS_CLASS_SZ_MAX
#  define MCSS_CLASS_SZ_MAX 128
#endif /* !MCSS_CLASS_SZ_MAX */

#define MCSS_PROP_FLAG_ACTIVE    0x01

#define MCSS_PROP_FLAG_IMPORTANT 0x02

#define MCSS_PROP_FLAG_AUTO      0x04

#define MCSS_PARSER_PSTATE_TABLE( f ) \
   f( MCSS_PSTATE_NONE, 0 ) \
   f( MCSS_PSTATE_VALUE, 1 ) \
   f( MCSS_PSTATE_RULE, 2 )

#define mcss_style( styler, style_idx ) \
   (0 <= style_idx ? &((styler)->styles[style_idx]) : NULL)

#define mcss_tag_style( parser, tag_idx ) \
   (0 <= tag_idx && 0 <= (parser)->tags[tag_idx].base.style ? \
      &((parser)->styler.styles[(parser)->tags[tag_idx].base.style]) : NULL)

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
   f( 0, WIDTH, ssize_t, mcss_style_size_t, 0 ) \
   f( 1, HEIGHT, ssize_t, mcss_style_size_t, 0 ) \
   f( 2, COLOR, RETROFLAT_COLOR, mcss_style_color, RETROFLAT_COLOR_NULL ) \
   f( 3, BACKGROUND_COLOR, RETROFLAT_COLOR, mcss_style_color, RETROFLAT_COLOR_NULL ) \
   f( 4, MARGIN_LEFT, ssize_t, mcss_style_size_t, 0 ) \
   f( 5, MARGIN_TOP, ssize_t, mcss_style_size_t, 0 ) \
   f( 6, MARGIN_RIGHT, ssize_t, mcss_style_size_t, 0 ) \
   f( 7, MARGIN_BOTTOM, ssize_t, mcss_style_size_t, 0 ) \
   f( 8, PADDING_LEFT, ssize_t, mcss_style_size_t, 0 ) \
   f( 9, PADDING_TOP, ssize_t, mcss_style_size_t, 0 ) \
   f( 10, DISPLAY, uint8_t, mcss_style_display, 0 ) \
   f( 11, POSITION, uint8_t, mcss_style_position, 0 ) \
   f( 12, X, ssize_t, mcss_style_size_t, 0 ) \
   f( 13, Y, ssize_t, mcss_style_size_t, 0 )

#define MCSS_PROP_TABLE_PROPS( idx, prop_n, prop_t, prop_prse, def ) \
   prop_t prop_n; \
   uint8_t prop_n ## _flags;

#define MCSS_POSITION_TABLE( f ) \
   f( 0, RELATIVE ) \
   f( 1, ABSOLUTE )

#define MCSS_DISPLAY_TABLE( f ) \
   f( 0, INLINE ) \
   f( 1, BLOCK ) \
   f( 2, TABLE )

struct MCSS_STYLE {
   char id[MCSS_ID_SZ_MAX];
   char class[MCSS_CLASS_SZ_MAX];
   MCSS_PROP_TABLE( MCSS_PROP_TABLE_PROPS )
};

struct MCSS_PARSER {
   uint16_t pstate[MPARSER_STACK_SZ_MAX];
   size_t pstate_sz;
   int16_t prop_key;
   /*! \brief Flags to push with next pushed prop val. */
   uint8_t prop_flags;
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

void mcss_parser_free( struct MCSS_PARSER* parser );

MERROR_RETVAL mcss_parser_init( struct MCSS_PARSER* parser );

#ifdef MCSS_C

#define MCSS_POSITION_TABLE_CONSTS( pos_id, pos_name ) \
   MAUG_CONST uint16_t MCSS_POSITION_ ## pos_name = pos_id;

MCSS_POSITION_TABLE( MCSS_POSITION_TABLE_CONSTS )

#define MCSS_POSITION_TABLE_NAMES( pos_id, pos_name ) \
   #pos_name,

MAUG_CONST char* gc_mcss_position_names[] = {
   MCSS_POSITION_TABLE( MCSS_POSITION_TABLE_NAMES )
   ""
};

#define MCSS_DISPLAY_TABLE_CONSTS( dis_id, dis_name ) \
   MAUG_CONST uint16_t MCSS_DISPLAY_ ## dis_name = dis_id;

MCSS_DISPLAY_TABLE( MCSS_DISPLAY_TABLE_CONSTS )

#define MCSS_DISPLAY_TABLE_NAMES( dis_id, dis_name ) \
   #dis_name,

MAUG_CONST char* gc_mcss_display_names[] = {
   MCSS_DISPLAY_TABLE( MCSS_DISPLAY_TABLE_NAMES )
   ""
};

#define MCSS_PROP_TABLE_CONSTS( prop_id, prop_n, prop_t, prop_prse, def ) \
   MAUG_CONST uint16_t MCSS_PROP_ ## prop_n = prop_id;

MCSS_PROP_TABLE( MCSS_PROP_TABLE_CONSTS )

#define MCSS_PROP_TABLE_NAMES( prop_id, prop_n, prop_t, prop_prse, def ) \
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

   mparser_token_upper( parser, i );
   mparser_token_replace( parser, i, '-', '_' );

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
   parser->prop_key = -1;

cleanup:

   return retval;
}

MERROR_RETVAL mcss_style_position(
   struct MCSS_PARSER* parser, const char* prop_name,
   uint8_t* position_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   mparser_token_upper( parser, i );
   mparser_token_replace( parser, i, '!', '\0' ); /* !important */

   i = 0;
   while( '\0' != gc_mcss_position_names[i][0] ) {
      if(
         /* Don't use sz check here because we might've shrunk token with
          * ! check above. */
         0 == strncmp(
            gc_mcss_position_names[i], parser->token, parser->token_sz )
      ) {
         debug_printf( 1, "set %s: %s", prop_name, gc_mcss_position_names[i] );
         *position_out = i;
         goto cleanup;
      }
      i++;
   }

   error_printf( "invalid %s: %s", prop_name, parser->token );

cleanup:

   return retval;
}

MERROR_RETVAL mcss_style_display(
   struct MCSS_PARSER* parser, const char* prop_name,
   uint8_t* display_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   mparser_token_upper( parser, i );
   mparser_token_replace( parser, i, '!', '\0' ); /* !important */

   i = 0;
   while( '\0' != gc_mcss_display_names[i][0] ) {
      if(
         /* Don't use sz check here because we might've shrunk token with
          * ! check above. */
         0 == strncmp(
            gc_mcss_display_names[i], parser->token, parser->token_sz )
      ) {
         debug_printf( 1, "set %s: %s", prop_name, gc_mcss_display_names[i] );
         *display_out = i;
         goto cleanup;
      }
      i++;
   }

   error_printf( "invalid %s: %s", prop_name, parser->token );

cleanup:

   return retval;
}

MERROR_RETVAL mcss_style_color(
   struct MCSS_PARSER* parser, const char* prop_name,
   RETROFLAT_COLOR* color_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   mparser_token_upper( parser, i );
   mparser_token_replace( parser, i, '!', '\0' ); /* !important */

   i = 0;
   while( '\0' != gc_mcss_color_names[i][0] ) {
      if(
         /* Don't use sz check here because we might've shrunk token with
          * ! check above. */
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

MERROR_RETVAL mcss_style_size_t(
   struct MCSS_PARSER* parser, const char* prop_name,
   ssize_t* num_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   mparser_token_upper( parser, i );

   if( 0 == strncmp( "AUTO", parser->token, 5 ) ) {
      parser->prop_flags |= MCSS_PROP_FLAG_AUTO;
   }

   *num_out = atoi( parser->token );

   if( MCSS_PROP_FLAG_AUTO == (MCSS_PROP_FLAG_AUTO & parser->prop_flags) ) {
      debug_printf( 1, "set %s: AUTO", prop_name );
   } else {
      debug_printf( 1, "set %s: " SIZE_T_FMT, prop_name, *num_out );
   }

   return retval;
}

MERROR_RETVAL mcss_push_prop_val( struct MCSS_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   #define MCSS_PROP_TABLE_PARSE( idx, prop_n, prop_t, prop_prse, def ) \
      case idx: \
         retval = prop_prse( \
            parser, #prop_n, \
            &(parser->styles[parser->styles_sz - 1].prop_n) ); \
         maug_cleanup_if_not_ok(); \
         parser->styles[parser->styles_sz - 1].prop_n ## _flags = \
            parser->prop_flags; \
         parser->styles[parser->styles_sz - 1].prop_n ## _flags |= \
            MCSS_PROP_FLAG_ACTIVE; \
         break;

   switch( parser->prop_key ) {
   MCSS_PROP_TABLE( MCSS_PROP_TABLE_PARSE )
   default:
      error_printf( "invalid property: %d", parser->prop_key );
      break;
   }

cleanup:

   parser->prop_flags = 0;

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
         retval = mcss_push_prop_val( parser );
         maug_cleanup_if_not_ok();
         mcss_parser_pstate_pop( parser );
         mcss_parser_reset_token( parser );

      } else if( MCSS_PSTATE_RULE == mcss_parser_pstate( parser ) ) {
         if(
            /* TODO: Break this out to make it more resilient. */
            NULL != strchr( parser->token, '!' ) &&
            0 == strncmp( "!important", strchr( parser->token, '!' ), 10 )
         ) {
            debug_printf( 1, "marking value !important..." );
            parser->prop_flags |= MCSS_PROP_FLAG_IMPORTANT;
         }
         retval = mcss_push_prop_val( parser );
         maug_cleanup_if_not_ok();
         mcss_parser_pstate_pop( parser ); /* Pop rule. */
         assert( MCSS_PSTATE_VALUE == mcss_parser_pstate( parser ) );
         mcss_parser_pstate_pop( parser );
         mcss_parser_reset_token( parser );

      } else {
         mcss_parser_invalid_c( parser, c, retval );
      }
      break;

   case '!':
      /* TODO: Handle !important. */
      if( MCSS_PSTATE_VALUE == mcss_parser_pstate( parser ) ) {
         mcss_parser_pstate_push( parser, MCSS_PSTATE_RULE );
         /* Append the ! here so the pusher can make it a \0 later. */
         mcss_parser_append_token( parser, c );
      }
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

MERROR_RETVAL mcss_style_init( struct MCSS_STYLE* style ) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( style, sizeof( struct MCSS_STYLE ) );

   #define MCSS_PROP_TABLE_DEFS( idx, prop_n, prop_t, prop_prse, def ) \
      style->prop_n = def;

   MCSS_PROP_TABLE( MCSS_PROP_TABLE_DEFS )

   return retval;
}

void mcss_parser_free( struct MCSS_PARSER* parser ) {

   debug_printf( 1, "freeing style parser..." );

   mcss_parser_unlock( parser );

   if( NULL != parser->styles_h ) {
      maug_mfree( parser->styles_h );
   }
}

MERROR_RETVAL mcss_parser_init( struct MCSS_PARSER* parser ) {
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

