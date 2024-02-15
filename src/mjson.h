
#ifndef MJSON_H
#define MJSON_H

#include <mparser.h>

#ifndef MJSON_TOKEN_SZ_MAX
#  define MJSON_TOKEN_SZ_MAX 4096
#endif /* !MJSON_TOKEN_SZ_MAX */

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
   uint8_t pstate[MPARSER_STACK_SZ_MAX];
   uint8_t pstate_popped;
   size_t pstate_sz;
   char token[MJSON_TOKEN_SZ_MAX];
   size_t token_sz;
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
   size_t i;
   char last_c;
};

#define mjson_parser_pstate( parser ) \
   ((parser)->pstate_sz > 0 ? \
      (parser)->pstate[(parser)->pstate_sz - 1] : MJSON_PSTATE_NONE)

#define mjson_parser_pstate_push( parser, new_pstate ) \
   mparser_pstate_push( mjson, parser, new_pstate )

#define mjson_parser_pstate_pop( parser ) \
   (parser)->pstate_popped = (parser)->pstate_sz - 1; \
   mparser_pstate_pop( mjson, parser )

#define mjson_parser_invalid_c( parser, c, retval ) \
   mparser_invalid_c( mjson, parser, c, retval )

#define mjson_parser_reset_token( parser ) \
   mparser_reset_token( mjson, parser )

#define mjson_parser_append_token( parser, c ) \
   mparser_append_token( mjson, parser, c, MJSON_TOKEN_SZ_MAX )

#define mjson_parser_parse_token( parser ) \
   parser->token_parser( \
      (parser)->token, (parser)->token_sz, (parser)->token_parser_arg )

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
         mjson_parser_append_token( parser, c );
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
         mjson_parser_pstate_push( parser, MJSON_PSTATE_OBJECT_KEY );
         mjson_parser_reset_token( parser );

         if( NULL != parser->open_obj ) {
            parser->open_obj( parser->open_obj_arg );
         }

      } else if( MJSON_PSTATE_STRING == mjson_parser_pstate( parser ) ) {
         mjson_parser_append_token( parser, c );

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
         mjson_parser_append_token( parser, c );

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
         mjson_parser_pstate_push( parser, MJSON_PSTATE_LIST );
         mjson_parser_reset_token( parser );

      } else if(
         MJSON_PSTATE_STRING == mjson_parser_pstate( parser )
      ) {
         mjson_parser_append_token( parser, c );

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
         mjson_parser_append_token( parser, c );

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
         mjson_parser_pstate_push( parser, MJSON_PSTATE_STRING );

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
         mjson_parser_pstate_push( parser, MJSON_PSTATE_OBJECT_KEY );
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
         mjson_parser_append_token( parser, c );

      } else {
         mjson_parser_invalid_c( parser, c, retval );
      }
      break;

   case ':':
      if( MJSON_PSTATE_OBJECT_KEY == mjson_parser_pstate( parser ) ) {
         retval = mjson_parser_parse_token( parser );
         maug_cleanup_if_not_ok();
         mjson_parser_pstate_push( parser, MJSON_PSTATE_OBJECT_VAL );
         mjson_parser_reset_token( parser );

      } else if(
         MJSON_PSTATE_STRING == mjson_parser_pstate( parser )
      ) {
         mjson_parser_append_token( parser, c );

      } else {
         mjson_parser_invalid_c( parser, c, retval );
      }
      break;

   default:
      mjson_parser_append_token( parser, c );
      break;
   }

cleanup:

   parser->last_c = c;

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

