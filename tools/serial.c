
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define READ_BUF_SZ 4096

#define PARSE_TOKEN_SZ 255
#define PARSE_STACK_DEPTH 128
#define PARSE_FIELDS_MAX 255
#define PARSE_DEFINES_MAX 255

/* !\brief These are the types we can handle by default. These can be expanded
 *         in msercust.h. Note that this is just used for emitting the serialize
 *         code on a high level. The engine should include functions to 
 *         serialize these named accordingly!
 */
#define parse_field_type_table_base( f ) \
   f( size_t,                  1 ) \
   f( ssize_t,                 2 ) \
   f( uint8_t,                 3 ) \
   f( int8_t,                  4 ) \
   f( uint16_t,                5 ) \
   f( int16_t,                 6 ) \
   f( uint32_t,                7 ) \
   f( int32_t,                 8 ) \
   f( char,                    9 ) \
   f( retroflat_asset_path,    10 ) \
   f( mfix_t,                  11 ) \
   f( retrotile_coord_t,       12 ) \
   f( float,                   13 )

#include <msercust.h>

#ifndef parse_field_type_table
/**
 * \brief Definition for the type table if not overridden in msercust.h.
 */
#  define parse_field_type_table( f ) \
      parse_field_type_table_base( f )
#endif /* !parse_field_type_table */

#define PARSE_MODE_NONE 0
#define PARSE_MODE_STRUCT_NAME 1
#define PARSE_MODE_STRUCT_BODY 2
#define PARSE_MODE_STRUCT_FIELD_TYPE 3
#define PARSE_MODE_STRUCT_FIELD_NAME 4
#define PARSE_MODE_COMMENT_SLASH 5
#define PARSE_MODE_COMMENT 6
#define PARSE_MODE_COMMENT_STAR 7
#define PARSE_MODE_NO_SERIAL 8
/*! \brief Inside of [array size indicator] for a struct field. */
#define PARSE_MODE_STRUCT_FIELD_ARRAY 9
/*! \brief Preprocessor directive starting. */
#define PARSE_MODE_HASH 10
/*! \brief Inside of a preprocessor define. */
#define PARSE_MODE_DEFINE_NAME 11
#define PARSE_MODE_DEFINE_VALUE 12

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

struct PARSE_DEFINE {
   char name[PARSE_TOKEN_SZ + 1];
   int value;
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
   struct PARSE_DEFINE defines[PARSE_DEFINES_MAX];
   size_t defines_ct;
};

void parse_dump_struct( struct STRUCT_PARSED* parsed ) {
   size_t i = 0;
   printf( "> found valid struct: %s\n", parsed->name );
   for( i = 0 ; parsed->fields_ct > i ; i++ ) {
      printf( " - field: %s (type %d)\n",
         parsed->fields[i].name, parsed->fields[i].type );
   }
}

int eval_array_sz( struct STRUCT_PARSER* parser, const char* array_sz_str ) {
   int array_sz_out = 0;
   size_t i = 0,
      j = 0,
      k = 0;
   char arr_def_token[PARSE_TOKEN_SZ + 1];
   int last_op = '+';

   memset( arr_def_token, '\0', PARSE_TOKEN_SZ + 1 );

   printf( "eval: %s\n", array_sz_str );

   /* Parse defines out of array size string. */
   for( i = 0 ; strlen( array_sz_str ) > i ; i++ ) {
      switch( array_sz_str[i] ) {
      case '+':
         /* TODO: Make this a reusable function. */
         /* Find the current define. */
         /* TODO: Handle this being a literal number. */
         for( j = 0 ; parser->defines_ct > j ; j++ ) {
            if( 0 == strncmp(
               arr_def_token, parser->defines[j].name, PARSE_TOKEN_SZ
            ) ) {
               printf( "found: %s, %d\n",
                  parser->defines[j].name,
                  parser->defines[j].value );
               break;
            }
         }

         if( j >= parser->defines_ct ) {
            error_str( "could not find define: %s\n", arr_def_token );
            goto cleanup;
         }

         /* Apply the last op with this value. */
         switch( last_op ) {
         case '+':
            array_sz_out += parser->defines[j].value;
            break;
         }

         /* Reset the arr_def_token. */
         memset( arr_def_token, '\0', PARSE_TOKEN_SZ + 1 );
         k = 0;

         /* Store this op for the next last op. */
         last_op = '+';
         break;

      default:
         arr_def_token[k++] = array_sz_str[i];
         break;
      }
   }

cleanup:

   return array_sz_out;
}

void parse_emit_struct( struct STRUCT_PARSED* parsed ) {
   size_t i = 0;
   char type_str[PARSE_TOKEN_SZ + 1];

#define parse_field_type_str( type_name, type_idx ) \
   case type_idx: \
      memset( type_str, '\0', PARSE_TOKEN_SZ + 1 ); \
      strncpy( type_str, #type_name, PARSE_TOKEN_SZ ); \
      break;

   printf( "MERROR_RETVAL mserialize_struct_%s( struct %s ser_struct ) {\n",
      parsed->name, parsed->name );
   for( i = 0 ; parsed->fields_ct > i ; i++ ) {
      /* Get the type string for this field. */
      switch( parsed->fields[i].type ) {
      parse_field_type_table( parse_field_type_str )
      }

      printf( "   mserialize_field_%s( ser_struct->%s, %d );\n",
         type_str, parsed->fields[i].name, parsed->fields[i].array );
   }
   printf( "}\n" );

}

void parse_reset_token( struct STRUCT_PARSER* parser ) {
   if( PARSE_MODE_STRUCT_FIELD_ARRAY == parse_mode( parser ) ) {
      memset( parser->array_token, '\0', PARSE_TOKEN_SZ );
      parser->array_token_sz = 0;
   } else {
      memset( parser->token, '\0', PARSE_TOKEN_SZ );
      parser->token_sz = 0;
   }
}

int parse_token_append( struct STRUCT_PARSER* parser, char c ) {
   int retval = 0;

   if( PARSE_MODE_STRUCT_FIELD_ARRAY == parse_mode( parser ) ) {
      parser->array_token[parser->array_token_sz++] = c;
      assert( parser->array_token_sz <= PARSE_TOKEN_SZ );
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
      if( PARSE_MODE_DEFINE_VALUE == parse_mode( parser ) ) {
         /* Store the defined value. */
         parser->defines[parser->defines_ct].value = atoi( parser->token );
         parser->defines_ct++;
         /* DEFINE_VALUE */
         parse_pop_mode( parser );
         /* DEFINE_NAME */
         parse_pop_mode( parser );
         /* HASH */
         parse_pop_mode( parser );
         parse_reset_token( parser );
         goto cleanup;

      } else {
         /* Hash modes end uneventfully on newline. */
         if( PARSE_MODE_DEFINE_NAME == parse_mode( parser ) ) {
            parse_pop_mode( parser );
         }
         if( PARSE_MODE_HASH == parse_mode( parser ) ) {
            parse_pop_mode( parser );
         }
         /* TODO: Handle backslash escapes? */
         parse_reset_token( parser );
         goto cleanup;
      }
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

         } else if( 0 == strncmp( parser->token, "struct ", PARSE_TOKEN_SZ ) ) {
            /* Do nothing! Prepend this "struct" to type token! */
            goto cleanup;

         } else if( 0 == strncmp( parser->token, "struct", PARSE_TOKEN_SZ ) ) {
            /* Make sure a single space exists after "struct". */
            retval = parse_token_append( parser, ' ' );
            goto cleanup;

#define parse_field_type_assign( type_name, type_idx ) \
   } else if( 0 == strncmp( parser->token, #type_name, PARSE_TOKEN_SZ ) ) { \
      debug_int( "type", type_idx ); \
      (parser)->parsed.fields[parser->parsed.fields_ct].type = type_idx;

         parse_field_type_table( parse_field_type_assign )
         } else {
            error_str( "invalid field type", parser->token );
            parser->parsed.valid = 0;
         }
         parse_push_mode( parser, PARSE_MODE_STRUCT_FIELD_NAME );
         break;

      case PARSE_MODE_STRUCT_FIELD_NAME:
         /* Do nothing until ;. */
         break;

      case PARSE_MODE_HASH:
         if( 0 == strncmp( parser->token, "define", PARSE_TOKEN_SZ ) ) {
            parse_push_mode( parser, PARSE_MODE_DEFINE_NAME );
         }
         break;

      case PARSE_MODE_DEFINE_NAME:
         /* Note the name for this define. */
         memset(
            parser->defines[parser->defines_ct].name,
            '\0', PARSE_TOKEN_SZ + 1 );
         strncpy(
            parser->defines[parser->defines_ct].name, parser->token,
            PARSE_TOKEN_SZ );
         debug_str( "define", parser->token );
         parse_push_mode( parser, PARSE_MODE_DEFINE_VALUE );
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
#ifdef DEBUG
            parse_dump_struct( &(parser->parsed) );
#else
            parse_emit_struct( &(parser->parsed) );
#endif /* DEBUG */
         }
         break;
      }
      break;

   case '[':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_FIELD_NAME:
         parse_push_mode( parser, PARSE_MODE_STRUCT_FIELD_ARRAY );
         parse_reset_token( parser );
         break;
      }
      break;

   case ']':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_STRUCT_FIELD_ARRAY:
         /* Note the array size. */
         parser->parsed.fields[parser->parsed.fields_ct].array =
            eval_array_sz( parser, parser->array_token );
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

   case '#':
      switch( parse_mode( parser ) ) {
      case PARSE_MODE_NONE:
         parse_push_mode( parser, PARSE_MODE_HASH );
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

