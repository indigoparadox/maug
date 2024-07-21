
#ifndef MJSON_H
#define MJSON_H

#define MJSON_PARSER_PSTATE_TABLE( f ) \
   f( MJSON_PSTATE_NONE, 0 ) \
   f( MJSON_PSTATE_OBJECT_KEY, 1 ) \
   f( MJSON_PSTATE_OBJECT_VAL, 2 ) \
   f( MJSON_PSTATE_STRING, 3 ) \
   f( MJSON_PSTATE_LIST, 4 )

typedef MERROR_RETVAL
(*mjson_parse_token_cb)( const char* token, size_t token_sz, void* arg );

typedef MERROR_RETVAL (*mjson_parse_close_cb)( void* arg );

typedef MERROR_RETVAL (*mjson_parse_c_cb)( void* arg );

struct MJSON_PARSER {
   struct MPARSER base;
   mjson_parse_token_cb token_parser;
   void* token_parser_arg;
   mjson_parse_close_cb close_list;
   void* close_list_arg;
   mjson_parse_close_cb open_obj;
   void* open_obj_arg;
   mjson_parse_close_cb close_obj;
   void* close_obj_arg;
   mjson_parse_close_cb close_val;
   void* close_val_arg;
};

#define mjson_parser_pstate( parser ) \
   ((&((parser)->base))->pstate_sz > 0 ? \
      (&((parser)->base))->pstate[(&((parser)->base))->pstate_sz - 1] : MJSON_PSTATE_NONE)

#define mjson_parser_pstate_push( parser, new_pstate ) \
   mparser_pstate_push( "mjson", &((parser)->base), new_pstate )

#define mjson_parser_pstate_pop( parser ) \
   mparser_pstate_pop( "mjson", &(parser->base) )

#define mjson_parser_invalid_c( parser, c, retval ) \
   mparser_invalid_c( mjson, &((parser)->base), c, retval )

#define mjson_parser_reset_token( parser ) \
   mparser_reset_token( "mjson", &((parser)->base) )

#define mjson_parser_append_token( parser, c ) \
   mparser_append_token( "mjson", &((parser)->base), c )

#define mjson_parser_parse_token( parser ) \
   parser->token_parser( \
      (&((parser)->base))->token, (&((parser)->base))->token_sz, (parser)->token_parser_arg )

MERROR_RETVAL mjson_parse_c( struct MJSON_PARSER* parser, char c );

#ifdef MJSON_C

MJSON_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_CONST )

MPARSER_PSTATE_NAMES( MJSON_PARSER_PSTATE_TABLE, mjson )

MERROR_RETVAL mjson_parse_c( struct MJSON_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;

   switch( c ) {
   case '\r':
   case '\n':
   case '\t':
   case ' ':
      if( MJSON_PSTATE_STRING == mjson_parser_pstate( parser ) ) {
         retval = mjson_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();
      }
      break;

   case '{':
      if(
         MJSON_PSTATE_NONE == mjson_parser_pstate( parser ) ||
         MJSON_PSTATE_LIST == mjson_parser_pstate( parser ) ||
         MJSON_PSTATE_OBJECT_VAL == mjson_parser_pstate( parser )
      ) {
         /* Starting to read a new object... it's gonna start with a key
          * for its first child!
          */
         retval = mjson_parser_pstate_push( parser, MJSON_PSTATE_OBJECT_KEY );
         maug_cleanup_if_not_ok();
         mjson_parser_reset_token( parser );

         if( NULL != parser->open_obj ) {
            parser->open_obj( parser->open_obj_arg );
         }

      } else if( MJSON_PSTATE_STRING == mjson_parser_pstate( parser ) ) {
         retval = mjson_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mjson_parser_invalid_c( parser, c, retval );
      }
      break;

   case '}':
      if( MJSON_PSTATE_OBJECT_VAL == mjson_parser_pstate( parser ) ) {
         retval = mjson_parser_parse_token( parser );
         maug_cleanup_if_not_ok();
         mjson_parser_pstate_pop( parser );
         mjson_parser_pstate_pop( parser ); /* Pop key */
         mjson_parser_reset_token( parser );

         if( NULL != parser->close_obj ) {
            parser->close_obj( parser->close_obj_arg );
         }

      } else if(
         MJSON_PSTATE_STRING == mjson_parser_pstate( parser )
      ) {
         retval = mjson_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mjson_parser_invalid_c( parser, c, retval );
      }
      break;

   case '[':
      if(
         MJSON_PSTATE_NONE == mjson_parser_pstate( parser ) ||
         MJSON_PSTATE_OBJECT_VAL == mjson_parser_pstate( parser ) ||
         MJSON_PSTATE_LIST == mjson_parser_pstate( parser )
      ) {
         retval = mjson_parser_pstate_push( parser, MJSON_PSTATE_LIST );
         maug_cleanup_if_not_ok();
         mjson_parser_reset_token( parser );

      } else if(
         MJSON_PSTATE_STRING == mjson_parser_pstate( parser )
      ) {
         retval = mjson_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mjson_parser_invalid_c( parser, c, retval );
      }
      break;

   case ']':
      if( MJSON_PSTATE_LIST == mjson_parser_pstate( parser ) ) {
         retval = mjson_parser_parse_token( parser );
         maug_cleanup_if_not_ok();
         mjson_parser_pstate_pop( parser );
         mjson_parser_reset_token( parser );

         if( NULL != parser->close_list ) {
            parser->close_list( parser->close_list_arg );
         }

      } else if(
         MJSON_PSTATE_STRING == mjson_parser_pstate( parser )
      ) {
         retval = mjson_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mjson_parser_invalid_c( parser, c, retval );
      }
      break;

   case '"':
      if(
         MJSON_PSTATE_OBJECT_KEY == mjson_parser_pstate( parser ) ||
         MJSON_PSTATE_OBJECT_VAL == mjson_parser_pstate( parser ) ||
         MJSON_PSTATE_LIST == mjson_parser_pstate( parser )
      ) {
         retval = mjson_parser_pstate_push( parser, MJSON_PSTATE_STRING );
         maug_cleanup_if_not_ok();

      } else if( MJSON_PSTATE_STRING == mjson_parser_pstate( parser ) ) {
         mjson_parser_pstate_pop( parser );

      } else {
         mjson_parser_invalid_c( parser, c, retval );
      }
      break;

   case ',':
      if( MJSON_PSTATE_OBJECT_VAL == mjson_parser_pstate( parser ) ) {
         retval = mjson_parser_parse_token( parser );
         maug_cleanup_if_not_ok();
         mjson_parser_pstate_pop( parser );
         mjson_parser_pstate_pop( parser ); /* Pop key */
         retval = mjson_parser_pstate_push( parser, MJSON_PSTATE_OBJECT_KEY );
         maug_cleanup_if_not_ok();
         mjson_parser_reset_token( parser );

         if( NULL != parser->close_val ) {
            parser->close_val( parser->close_val_arg );
         }

      } else if( MJSON_PSTATE_LIST == mjson_parser_pstate( parser ) ) {
         retval = mjson_parser_parse_token( parser );
         maug_cleanup_if_not_ok();
         mjson_parser_reset_token( parser );

      } else if(
         MJSON_PSTATE_STRING == mjson_parser_pstate( parser )
      ) {
         retval = mjson_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mjson_parser_invalid_c( parser, c, retval );
      }
      break;

   case ':':
      if( MJSON_PSTATE_OBJECT_KEY == mjson_parser_pstate( parser ) ) {
         retval = mjson_parser_parse_token( parser );
         maug_cleanup_if_not_ok();
         retval = mjson_parser_pstate_push( parser, MJSON_PSTATE_OBJECT_VAL );
         maug_cleanup_if_not_ok();
         mjson_parser_reset_token( parser );

      } else if(
         MJSON_PSTATE_STRING == mjson_parser_pstate( parser )
      ) {
         retval = mjson_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mjson_parser_invalid_c( parser, c, retval );
      }
      break;

   default:
      retval = mjson_parser_append_token( parser, c );
      maug_cleanup_if_not_ok();
      break;
   }

   mparser_wait( &(parser->base) );

cleanup:

   parser->base.last_c = c;

   return retval;
}

#else

#  define MJSON_PSTATE_TABLE_CONST( name, idx ) \
      extern MAUG_CONST uint8_t SEG_MCONST name;

MJSON_PARSER_PSTATE_TABLE( MJSON_PSTATE_TABLE_CONST )

#ifdef MPARSER_TRACE_NAMES
extern MAUG_CONST char* SEG_MCONST gc_mjson_pstate_names[];
#endif /* MPARSER_TRACE_NAMES */

#endif /* MJSON_C */

#endif /* !MJSON_H */

