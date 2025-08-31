
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define READ_BUF_SZ 4096

#define PARSE_TOKEN_SZ 255
#define PARSE_STACK_DEPTH 128

#define PARSE_MODE_NONE 0
#define PARSE_MODE_STRUCT_NAME 1
#define PARSE_MODE_STRUCT_BODY 2
#define PARSE_MODE_STRUCT_FIELD_TYPE 3
#define PARSE_MODE_STRUCT_FIELD_NAME 4

#define parse_push_mode( parser, mode_in ) \
   printf( "push: %d\n", mode_in ); \
   (parser)->mode[++((parser)->mode_cur)] = mode_in; \
   assert( (parser)->mode_cur < PARSE_STACK_DEPTH );

#define parse_pop_mode( parser ) \
   printf( "pop: %d\n", parse_mode( parser ) ); \
   (parser)->mode_cur--;

#define parse_mode( parser ) \
   ((parser)->mode[parser->mode_cur])

struct STRUCT_PARSED {
   size_t sz;
   char name[PARSE_TOKEN_SZ + 1];
   size_t name_sz;
   size_t fields_ct;
};

struct STRUCT_PARSER {
   int mode[PARSE_STACK_DEPTH];
   size_t mode_cur;
   char token[PARSE_TOKEN_SZ + 1];
   size_t token_sz;
   int newline;
   struct STRUCT_PARSED parsed;
};

void parse_reset_token( struct STRUCT_PARSER* parser ) {
   memset( parser->token, '\0', PARSE_TOKEN_SZ );
   parser->token_sz = 0;
}

int parse_c( struct STRUCT_PARSER* parser, char c ) {
   int retval = 0;

   /*
   printf( "%d %d %c\n", parser->mode_cur, parse_mode( parser ), c );
   */

   switch( c ) {
   /* Indicate newline on newline chars. */
   case '\n':
   case '\r':
      parser->newline = 1;
      /* Fall through. */
   case ' ':
   case '\t':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_NONE:
         if( 0 == strncmp( parser->token, "struct", PARSE_TOKEN_SZ ) ) {
            parse_push_mode( parser, PARSE_MODE_STRUCT_NAME );
         } else if( '\r' != c && '\n' != c ) {
            /* Don't immediately cancel out newline if this whitespace is the
             * newline, but if it's not then a token happened and it's no longer
             * a new line!
             */
            parser->newline = 0;
         }
         break;

      case PARSE_MODE_STRUCT_NAME:
         if( !parser->newline || 0 == parser->token_sz ) {
            /* Skip struct references and extra whitespace after the name. */
            break;
         }
         memset( &(parser->parsed), '\0', sizeof( struct STRUCT_PARSED ) );
         strncpy( parser->parsed.name, parser->token, PARSE_TOKEN_SZ );
         parser->parsed.name_sz = parser->token_sz;
         printf( "struct: %s\n", parser->parsed.name );
         break;

      case PARSE_MODE_STRUCT_BODY:
         /* TODO: Note that whitespace must exist between { and first field
          *       type!
          */
         parse_push_mode( parser, PARSE_MODE_STRUCT_FIELD_TYPE );
         break;

      case PARSE_MODE_STRUCT_FIELD_TYPE:
         parse_push_mode( parser, PARSE_MODE_STRUCT_FIELD_NAME );
         break;

      case PARSE_MODE_STRUCT_FIELD_NAME:
         /* Do nothing until ;. */
         break;
      }
      parse_reset_token( parser );
      break;

   case ';':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_FIELD_NAME:
         printf( "field name: %s\n", parser->token );
         /* FIELD_NAME */
         parse_pop_mode( parser );
         /* FIELD_TYPE */
         parse_pop_mode( parser );
         break;
      }
      break;

   /* TODO: Handle comments. */

   case '{':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_NAME:
         parse_push_mode( parser, PARSE_MODE_STRUCT_BODY );
         break;
      }
      break;

   case '}':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_BODY:
         /* BODY */
         parse_pop_mode( parser );
         assert( PARSE_MODE_STRUCT_NAME == parse_mode( parser ) );
         /* NAME */
         parse_pop_mode( parser );
         assert( 0 == parser->mode_cur );
         break;
      }
      break;

   default:
      parser->token[parser->token_sz++] = c;
      assert( parser->token_sz <= PARSE_TOKEN_SZ );
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

