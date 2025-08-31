
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define READ_BUF_SZ 4096

#define PARSE_TOKEN_SZ 255
#define PARSE_STACK_DEPTH 128
#define PARSE_FIELDS_MAX 255

#define parse_field_type_table( f, parser ) \
   f( parser, size_t,                  1 ) \
   f( parser, ssize_t,                 2 ) \
   f( parser, uint8_t,                 3 ) \
   f( parser, int8_t,                  4 ) \
   f( parser, uint16_t,                5 ) \
   f( parser, int16_t,                 6 ) \
   f( parser, uint32_t,                7 ) \
   f( parser, int32_t,                 8 ) \
   f( parser, char,                    9 ) \
   f( parser, retroflat_asset_path,    10 ) \
   f( parser, mfix_t,                  11 ) \
   f( parser, retrotile_coord_t,       12 ) \
   f( parser, float,                   13 )

#define parse_field_type( parser, type_name, type_idx ) \
   } else if( 0 == strncmp( (parser)->token, #type_name, PARSE_TOKEN_SZ ) ) { \
      debug_int( "type", type_idx ); \
      (parser)->parsed.fields[(parser)->parsed.fields_ct].type = type_idx;

#define PARSE_MODE_NONE 0
#define PARSE_MODE_STRUCT_NAME 1
#define PARSE_MODE_STRUCT_BODY 2
#define PARSE_MODE_STRUCT_FIELD_TYPE 3
#define PARSE_MODE_STRUCT_FIELD_NAME 4
#define PARSE_MODE_COMMENT_SLASH 5
#define PARSE_MODE_COMMENT 6
#define PARSE_MODE_COMMENT_STAR 7
#define PARSE_MODE_NO_SERIAL 8
#define PARSE_MODE_STRUCT_FIELD_ARRAY 9

#ifdef DEBUG
#  define debug_int( name_in, int_in ) \
      printf( "%s: %d\n", name_in, int_in ); fflush( stdout );
#  define debug_str( name_in, str_in ) \
      printf( "%s: %s\n", name_in, str_in ); fflush( stdout );
#define error_str( msg_in, str_in ) \
      printf( "%s: %s\n", msg_in, str_in ); fflush( stdout );
#else
#  define debug_int( name_in, int_in )
#  define debug_str( name_in, str_in )
#  define error_str( msg_in, str_in ) \
      fprintf( stderr, "%s: %s\n", msg_in, str_in ); fflush( stderr );
#endif /* DEBUG */

#define parse_push_mode( parser, mode_in ) \
   debug_str( "push", #mode_in ); \
   debug_int( "push", mode_in ); \
   (parser)->mode[++((parser)->mode_cur)] = mode_in; \
   assert( (parser)->mode_cur < PARSE_STACK_DEPTH );

#define parse_pop_mode( parser ) \
   debug_int( "pop", parse_mode( parser ) ); \
   (parser)->mode_cur--;

#define parse_mode( parser ) \
   ((parser)->mode[parser->mode_cur])

struct STRUCT_PARSED_FIELD {
   char name[PARSE_TOKEN_SZ + 1];
   int type;
   int array;
};

struct STRUCT_PARSED {
   size_t sz;
   char name[PARSE_TOKEN_SZ + 1];
   size_t name_sz;
   size_t fields_ct;
   struct STRUCT_PARSED_FIELD fields[PARSE_FIELDS_MAX];
   int valid;
   int ready;
};

struct STRUCT_PARSER {
   int mode[PARSE_STACK_DEPTH];
   size_t mode_cur;
   char token[PARSE_TOKEN_SZ + 1];
   size_t token_sz;
   char array_token[PARSE_TOKEN_SZ + 1];
   size_t array_token_sz;
   int newline;
   struct STRUCT_PARSED parsed;
};

void parse_dump_struct( struct STRUCT_PARSED* parsed ) {
   size_t i = 0;
   printf( "> found valid struct: %s\n", parsed->name );
   for( i = 0 ; parsed->fields_ct > i ; i++ ) {
      printf( " - field: %s (type %d)\n",
         parsed->fields[i].name, parsed->fields[i].type );
   }
}

void parse_reset_token( struct STRUCT_PARSER* parser ) {
   memset( parser->token, '\0', PARSE_TOKEN_SZ );
   parser->token_sz = 0;
}

int parse_token_append( struct STRUCT_PARSER* parser, char c ) {
   int retval = 0;

   if( PARSE_MODE_STRUCT_FIELD_ARRAY == parse_mode( parser ) ) {
   } else {
      parser->token[parser->token_sz++] = c;
      assert( parser->token_sz <= PARSE_TOKEN_SZ );
   }
   return retval;
}

int parse_set_field_name( struct STRUCT_PARSER* parser ) {
   int retval = 0;

   if(
      0 == parser->parsed.fields_ct &&
      0 != strncmp( parser->token, "sz", PARSE_TOKEN_SZ )
   ) {
      error_str( "invalid first field", parser->token );
      parser->parsed.valid = 0;
   } else if(
      0 == strncmp( parser->token, "no_serial", PARSE_TOKEN_SZ )
   ) {
      /* FIELD_NAME */
      parse_pop_mode( parser );
      /* FIELD_TYPE */
      parse_pop_mode( parser );
      /* Found deactivation dummy field. Fields are this are runtime-only.
         */
      parse_push_mode( parser, PARSE_MODE_NO_SERIAL );

      /* Don't bother storing this field. */
      return retval;
   }
   strncpy(
      parser->parsed.fields[parser->parsed.fields_ct].name,
      parser->token,
      PARSE_TOKEN_SZ );
   debug_str( "field name",
      parser->parsed.fields[parser->parsed.fields_ct].name );
   parser->parsed.fields_ct++;
   debug_int( "fields", parser->parsed.fields_ct );
   /* FIELD_NAME */
   parse_pop_mode( parser );
   /* FIELD_TYPE */
   parse_pop_mode( parser );

   return retval;
}

int parse_c( struct STRUCT_PARSER* parser, char c ) {
   int retval = 0;

#ifdef DEBUG
   printf( "%d %d %c\n", parser->mode_cur, parse_mode( parser ), c );
#endif /* DEBUG */

   switch( c ) {
   /* Indicate newline on newline chars. */
   case '\n':
   case '\r':
      parser->newline = 1;
      debug_int( "newline", 1 );
      /* Fall through. */
   case ' ':
   case '\t':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_NONE:
         if(
            parser->newline &&
            0 == strncmp( parser->token, "struct", PARSE_TOKEN_SZ )
         ) {
            parse_push_mode( parser, PARSE_MODE_STRUCT_NAME );
         } else if( '\r' != c && '\n' != c ) {
            /* Don't immediately cancel out newline if this whitespace is the
             * newline, but if it's not then a token happened and it's no longer
             * a new line!
             */
            parser->newline = 0;
            debug_int( "newline", 0 );
         }
         break;

      case PARSE_MODE_STRUCT_NAME:
         if( !parser->newline || 0 == parser->token_sz ) {
            /* Skip struct references and extra whitespace after the name. */
            break;
         }
         /* Reset the parsed struct to prepare for population. */
         memset( &(parser->parsed), '\0', sizeof( struct STRUCT_PARSED ) );
         strncpy( parser->parsed.name, parser->token, PARSE_TOKEN_SZ );
         parser->parsed.name_sz = parser->token_sz;
         parser->parsed.ready = 0;
         parser->parsed.valid = 1;
         break;

      case PARSE_MODE_STRUCT_FIELD_TYPE:
         /* Figure out type from the token. */
         if( 0 == parser->token_sz ) {
            /* Do nothing! */
            break;
         parse_field_type_table( parse_field_type, parser )
         } else {
            error_str( "invalid field type", parser->token );
            parser->parsed.valid = 0;
         }
         parse_push_mode( parser, PARSE_MODE_STRUCT_FIELD_NAME );
         break;

      case PARSE_MODE_STRUCT_FIELD_NAME:
         /* Do nothing until ;. */
         break;
      }

      if(
         PARSE_MODE_COMMENT != parse_mode( parser ) &&
         PARSE_MODE_STRUCT_FIELD_ARRAY != parse_mode( parser )
      ) {
         /* Don't reset the token inside "meta" states that can interrupt
          * other states that will need the token later.
          */
         parse_reset_token( parser );
      }
      break;

   case ';':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_FIELD_NAME:
         retval = parse_set_field_name( parser );
         break;
      }
      break;

   case '{':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_NAME:
         parse_push_mode( parser, PARSE_MODE_STRUCT_BODY );
         debug_str( "struct", parser->parsed.name );
         break;
      }
      break;

   case '}':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_NO_SERIAL:
         /* NO_SERIAL */
         parse_pop_mode( parser );
      case PARSE_MODE_STRUCT_BODY:
         parser->parsed.ready = 1;
         /* BODY */
         parse_pop_mode( parser );
         assert( PARSE_MODE_STRUCT_NAME == parse_mode( parser ) );
         /* NAME */
         parse_pop_mode( parser );
         assert( 0 == parser->mode_cur );
         if( parser->parsed.valid ) {
            parse_dump_struct( &(parser->parsed) );
         }
         break;
      }
      break;

   case '[':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_FIELD_NAME:
         parse_push_mode( parser, PARSE_MODE_STRUCT_FIELD_ARRAY );
         break;
      }
      break;

   case ']':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_FIELD_ARRAY:
         /* Note the array size. */
         /* TODO: Handle preprocessor constants. */
         parser->parsed.fields[parser->parsed.fields_ct].array =
            atoi( parser->token );
         parse_pop_mode( parser );
         retval = parse_set_field_name( parser );
         break;
      }
      break;

   case '/':
      if( PARSE_MODE_COMMENT_STAR == parse_mode( parser ) ) {
         /* COMMENT_STAR */
         parse_pop_mode( parser );
         /* COMMENT */
         parse_pop_mode( parser );
         /* COMMENT_SLASH */
         parse_pop_mode( parser );
      } else {
         parse_push_mode( parser, PARSE_MODE_COMMENT_SLASH );
         if( PARSE_MODE_COMMENT != parse_mode( parser ) ) {
            parser->token[parser->token_sz++] = c;
            assert( parser->token_sz <= PARSE_TOKEN_SZ );
         }
      }
      break;

   case '*':
      if( PARSE_MODE_COMMENT_SLASH == parse_mode( parser ) ) {
         parse_push_mode( parser, PARSE_MODE_COMMENT );
      } else if( PARSE_MODE_COMMENT == parse_mode( parser ) ) {
         parse_push_mode( parser, PARSE_MODE_COMMENT_STAR );
      } else {
         retval = parse_token_append( parser, c );
      }
      break;

   case '(':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_NAME:
         /* We must've found a function that returns a struct. That doesn't
          * count!
          */
         parse_pop_mode( parser );
         break;

      case PARSE_MODE_NONE:
         parser->newline = 0;
         debug_int( "newline", 0 );
         break;
      }
      break;

   default:
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_COMMENT:
         /* Do nothing! */
         break;

      case PARSE_MODE_STRUCT_BODY:
         parse_push_mode( parser, PARSE_MODE_STRUCT_FIELD_TYPE );
         retval = parse_token_append( parser, c );
         break;

      case PARSE_MODE_COMMENT_SLASH:
         /* Not a *, so abandon this pre-comment state. */
         parse_pop_mode( parser );
         retval = parse_token_append( parser, c );
         break;

      default:
         retval = parse_token_append( parser, c );
         break;
      }
      break;
   }

cleanup:

   return retval;
}

int main( int argc, char* argv[] ) {
   int retval = 0;
   FILE* header_f = NULL;
   size_t read_sz = 0;
   char read_buf[READ_BUF_SZ + 1];
   struct STRUCT_PARSER parser;
   int i = 0;

   if( 2 > argc ) {
      fprintf( stderr, "usage: %s <header.h>\n", argv[0] );
      retval = 1;
      goto cleanup;
   }

   header_f = fopen( argv[1], "r" );
   assert( NULL != header_f );

   memset( &parser, '\0', sizeof( struct STRUCT_PARSER ) );
   do {
      memset( read_buf, '\0', READ_BUF_SZ + 1 );
      read_sz = fread( read_buf, 1, READ_BUF_SZ, header_f );
      for( i = 0 ; READ_BUF_SZ > i && '\0' != read_buf[i] ; i++ ) {
         retval = parse_c( &parser, read_buf[i] );
         assert( 0 == retval );
      }
   } while( read_sz == READ_BUF_SZ );

cleanup:

   return retval;
}

