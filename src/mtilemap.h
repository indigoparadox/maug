
#ifndef MTILEMAP_H
#define MTILEMAP_H

#ifndef MTILEMAP_NAME_SZ_MAX
#  define MTILEMAP_NAME_SZ_MAX 10
#endif /* !MTILEMAP_NAME_SZ_MAX */

#ifndef MTILEMAP_PROPS_SZ_MAX
#  define MTILEMAP_PROPS_SZ_MAX 10
#endif /* !MTILEMAP_PROPS_SZ_MAX */

#ifndef MTILEMAP_W_MAX
#  define MTILEMAP_W_MAX 128
#endif /* !MTILEMAP_W_MAX */

#ifndef MTILEMAP_H_MAX
#  define MTILEMAP_H_MAX 128
#endif /* !MTILEMAP_H_MAX */

#ifndef MTILEMAP_D_MAX
#  define MTILEMAP_D_MAX 4
#endif /* !MTILEMAP_D_MAX */

#ifndef MTILEMAP_TOKEN_SZ_MAX
#  define MTILEMAP_TOKEN_SZ_MAX 127
#endif /* !MTILEMAP_TOKEN_SZ_MAX */

union MTILEMAP_CPROP_VAL {
   uint32_t u32;
   int32_t i32;
   char str[32];
   void* data;
};

struct MTILEMAP_CPROP {
   uint8_t type;
   char key[32];
   union MTILEMAP_CPROP_VAL value;
};

struct MTILEMAP {
   char name[MTILEMAP_NAME_SZ_MAX];
   struct MTILEMAP_PROP* cprops;
   uint8_t cprops_sz;
   uint8_t* tiles;
   uint8_t tiles_h;
   uint8_t tiles_w;
   uint8_t tiles_d;
};

struct MTILEMAP_PARSER {
   uint8_t state;
   size_t i;
   char* buffer;
   size_t buffer_sz;
   char token[MTILEMAP_TOKEN_SZ_MAX];
   size_t token_sz;
};

#define MTILEMAP_PARSER_STATE_TABLE( f ) \
   f( MTILEMAP_PSTATE_NONE, 0 ) \
   f( MTILEMAP_PSTATE_OBJECT_KEY, 1 ) \
   f( MTILEMAP_PSTATE_OBJECT_VAL, 2 ) \
   f( MTILEMAP_PSTATE_STRING_KEY, 3 ) \
   f( MTILEMAP_PSTATE_STRING_VAL, 4 )

MERROR_RETVAL
mtilemap_parse_json_c( struct MTILEMAP_PARSER* parser, char c );

MERROR_RETVAL
mtilemap_parse_json_file( const char* filename, struct MTILEMAP* m );

#ifdef MTILEMAP_C

#  include <stdio.h>

#define mtilemap_parser_state( parser, new_state ) \
   parser->state = new_state; \
   debug_printf( 1, "parser state: %s", gc_mtilemap_state_tokens[new_state] );

#  define MTILEMAP_PARSER_STATE_TABLE_CONST( name, idx ) \
      static MAUG_CONST uint8_t name = idx;

MTILEMAP_PARSER_STATE_TABLE( MTILEMAP_PARSER_STATE_TABLE_CONST )

#  define MTILEMAP_PARSER_STATE_TABLE_TOKEN( name, idx ) \
      #name,

static MAUG_CONST char* gc_mtilemap_state_tokens[] = {
   MTILEMAP_PARSER_STATE_TABLE( MTILEMAP_PARSER_STATE_TABLE_TOKEN )    
};

/* === */

static void
mtilemap_parse_append_token( struct MTILEMAP_PARSER* parser, char c ) {
   parser->token[parser->token_sz++] = c;
   parser->token[parser->token_sz] = '\0';
}

static void
mtilemap_parse_reset_token( struct MTILEMAP_PARSER* parser ) {
   parser->token_sz = 0;
   parser->token[parser->token_sz] = '\0';
   debug_printf( 1, "reset token" );
}

MERROR_RETVAL
mtilemap_parse_json_c( struct MTILEMAP_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1, "c: %c", c );

   switch( c ) {
   case '\r':
   case '\n':
   case '\t':
   case ' ':
      if(
         MTILEMAP_PSTATE_STRING_KEY == parser->state ||
         MTILEMAP_PSTATE_STRING_VAL == parser->state
      ) {
         mtilemap_parse_append_token( parser, c );
      }
      break;

   case '{':
      if( MTILEMAP_PSTATE_NONE == parser->state ) {
         mtilemap_parser_state( parser, MTILEMAP_PSTATE_OBJECT_KEY );
         mtilemap_parse_reset_token( parser );

      } else if(
         MTILEMAP_PSTATE_STRING_KEY == parser->state ||
         MTILEMAP_PSTATE_STRING_VAL == parser->state
      ) {
         mtilemap_parse_append_token( parser, c );
      } else {
         error_printf( "invalid { detected at char: " SIZE_T_FMT, parser->i );
         retval = 1;
      }
      break;

   case '"':
      if( MTILEMAP_PSTATE_OBJECT_KEY == parser->state ) {
         mtilemap_parser_state( parser, MTILEMAP_PSTATE_STRING_KEY );

      } else if( MTILEMAP_PSTATE_STRING_KEY == parser->state ) {
         debug_printf( 1, "found key: %s", parser->token );
         mtilemap_parser_state( parser, MTILEMAP_PSTATE_OBJECT_KEY );
         mtilemap_parse_reset_token( parser );

      } else if( MTILEMAP_PSTATE_STRING_VAL == parser->state ) {
         debug_printf( 1, "found value: %s", parser->token );
         mtilemap_parser_state( parser, MTILEMAP_PSTATE_OBJECT_VAL );
         mtilemap_parse_reset_token( parser );

      } else {
         error_printf( "invalid \" detected at char: " SIZE_T_FMT, parser->i );
         retval = 1;
      }
      break;

   case ',':
      if( MTILEMAP_PSTATE_OBJECT_VAL == parser->state ) {
         mtilemap_parser_state( parser, MTILEMAP_PSTATE_OBJECT_KEY );
         mtilemap_parse_reset_token( parser );
         /* TODO: Advance object. */

      } else if(
         MTILEMAP_PSTATE_STRING_KEY == parser->state ||
         MTILEMAP_PSTATE_STRING_VAL == parser->state
      ) {
         mtilemap_parse_append_token( parser, c );

      } else {
         error_printf( "invalid , detected at char: " SIZE_T_FMT, parser->i );
         retval = 1;
      }
      break;

   case ':':
      if( MTILEMAP_PSTATE_OBJECT_KEY == parser->state ) {
         mtilemap_parser_state( parser, MTILEMAP_PSTATE_OBJECT_VAL );
         mtilemap_parse_reset_token( parser );

      } else if(
         MTILEMAP_PSTATE_STRING_KEY == parser->state ||
         MTILEMAP_PSTATE_STRING_VAL == parser->state
      ) {
         mtilemap_parse_append_token( parser, c );
         /* TODO: Store key. */

      } else {
         error_printf( "invalid : detected at char: " SIZE_T_FMT, parser->i );
         retval = 1;
      }
      break;

   default:
      mtilemap_parse_append_token( parser, c );
      break;
   }

   return retval;
}

MERROR_RETVAL
mtilemap_parse_json_file( const char* filename, struct MTILEMAP* m ) {
   FILE* tilemap_file = NULL;
   size_t tilemap_file_sz = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct MTILEMAP_PARSER parser;

   memset( &parser, '\0', sizeof( struct MTILEMAP_PARSER ) );

   /* Open and get the file buffer size. */
   tilemap_file = fopen( filename, "r" );
   assert( NULL != tilemap_file );
   fseek( tilemap_file, 0, SEEK_END );
   tilemap_file_sz = ftell( tilemap_file );
   fseek( tilemap_file, 0, SEEK_SET );

   /* Load the file buffer into the parser. */
   parser.buffer = calloc( tilemap_file_sz, 1 );
   assert( NULL != parser.buffer );
   parser.buffer_sz = fread( parser.buffer, 1, tilemap_file_sz, tilemap_file );
   assert( parser.buffer_sz == tilemap_file_sz );
   fclose( tilemap_file );

   for( parser.i = 0 ; parser.buffer_sz > parser.i ; parser.i++ ) {
      retval = mtilemap_parse_json_c( &parser, parser.buffer[parser.i] );
      assert( 0 == retval );
   }

   return retval;
}

#endif /* MTILEMAP_C */

#endif /* !MTILEMAP_H */

