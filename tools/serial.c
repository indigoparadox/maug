
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
 *         in msercust.h.
 *
 *         Note that this is just used for emitting the serialize
 *         code on a high level. The engine should include functions to 
 *         serialize these named accordingly!
 *
 *         Values lower than 10 are special cases with specially defined calls:
 *         1 - struct MDATA_VECTOR
 */
#define parse_field_type_table_base( f ) \
   f( size_t,                  10 ) \
   f( ssize_t,                 11 ) \
   f( uint8_t,                 12 ) \
   f( int8_t,                  13 ) \
   f( uint16_t,                14 ) \
   f( int16_t,                 15 ) \
   f( uint32_t,                16 ) \
   f( int32_t,                 17 ) \
   f( char,                    18 ) \
   f( float,                   19 ) \
   f( mfix_t,                  20 ) \
   f( retroflat_asset_path,    21 ) \
   f( retrotile_coord_t,       22 ) \
   f( retroflat_dir4_t,        23 ) \
   f( retroflat_ms_t,          24 ) \
   f( struct RETROTILE_COORDS, 25 ) \
   f( struct MLISP_EXEC_STATE, 26 )

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
#define PARSE_MODE_COMMENT_VECTOR_TYPE 13

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
   /*! \brief Type of item stored if this is a struct MDATA_VECTOR. */
   char vector_type[PARSE_TOKEN_SZ + 1];
   /*! \brief Type of the field in parse_field_type_table(). */
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

/*! \brief Figure out what integer or defined constant a token could be. */
int eval_token(
   struct STRUCT_PARSER* parser, const char* token, int* p_result
) {
   int retval = 0;
   size_t i = 0;

   *p_result = 0;

   /* See if the value is an integer. */
   for( i = 0 ; strlen( token ) > i ; i++ ) {
      if( '9' < token[i] || '0' > token[i] ) {
         goto try_define;
      }
   }

   /* It's not not an integer, anyway! */
   *p_result = atoi( token );
   debug_str( "eval integer token", token );
   debug_int( "eval integer value", *p_result );

   goto cleanup;

try_define:

   /* If the value isn't an integer, search for a #define of it. */
   for( i = 0 ; parser->defines_ct > i ; i++ ) {
      if( 0 == strncmp( token, parser->defines[i].name, PARSE_TOKEN_SZ ) ) {
         debug_str( "eval define name", parser->defines[i].name );
         debug_int( "eval define value", parser->defines[i].value );
         /* Ship out the define value. */
         *p_result = parser->defines[i].value;
         goto cleanup;
      }
   }

   /* If we've reached this, we couldn't figure it out! */
   error_str( "could not find define: %s\n", token );
   retval = 1;

cleanup:

   return retval;
}

/*! \brief Evaluate a math operation on two values. */
int eval_op(
   struct STRUCT_PARSER* parser, const char* token, int* p_result, char op
) {
   int retval = 0;
   int last_val = 0;

   debug_str( "evaluating", token );

   /* Translate the ASCII op into math. */
   switch( op ) {
   case '+':
      if( !eval_token( parser, token, &last_val ) ) {
         *p_result += last_val;
      }
      break;
   case '-':
      if( !eval_token( parser, token, &last_val ) ) {
         *p_result -= last_val;
      }
      break;
   case '*':
      if( !eval_token( parser, token, &last_val ) ) {
         *p_result *= last_val;
      }
      break;
   case '/':
      if( !eval_token( parser, token, &last_val ) ) {
         *p_result /= last_val;
      }
      break;
   }

   return retval;
}

int eval_array_sz( struct STRUCT_PARSER* parser, const char* array_sz_str ) {
   int array_sz_out = 0;
   size_t i = 0,
      k = 0;
   char arr_def_token[PARSE_TOKEN_SZ + 1];
   int last_op = '+';
   int retval = 0;

   memset( arr_def_token, '\0', PARSE_TOKEN_SZ + 1 );

   debug_str( "eval sequence", array_sz_str );

   /* Parse defines out of array size string. */
   for( i = 0 ; strlen( array_sz_str ) > i ; i++ ) {
      switch( array_sz_str[i] ) {
      case '-':
      case '*':
      case '/':
      case '+':
         /* Apply the last op with this value. */
         if( eval_op( parser, arr_def_token, &array_sz_out, last_op ) ) {
            goto cleanup;
         }

         /* Reset the arr_def_token. */
         memset( arr_def_token, '\0', PARSE_TOKEN_SZ + 1 );
         k = 0;

         /* Store the next op from this char. */
         last_op = array_sz_str[i];
         break;

      default:
         /* Append to the token. */
         arr_def_token[k++] = array_sz_str[i];
         break;
      }
   }

   /* Evaluate the last symbol. */
   eval_op( parser, arr_def_token, &array_sz_out, last_op );

cleanup:

   return array_sz_out;
}

void parse_emit_struct( struct STRUCT_PARSED* parsed, int prototype ) {
   size_t i = 0,
      j = 0;
   char type_str[PARSE_TOKEN_SZ + 1];

#define parse_field_type_str( type_name, type_idx ) \
   case type_idx: \
      memset( type_str, '\0', PARSE_TOKEN_SZ + 1 ); \
      strncpy( type_str, #type_name, PARSE_TOKEN_SZ ); \
      for( j = 0 ; strlen( type_str ) > j ; j++ ) { \
         if( ' ' == type_str[j] ) { \
            type_str[j] = '_'; \
         } \
      } \
      break;

   printf( "MERROR_RETVAL mserialize_struct_%s( "
      "mfile_t* ser_f, size_t* p_sz, struct %s* p_ser_struct, size_t array )",
      parsed->name, parsed->name );

   if( prototype ) {
      printf( ";\n\n" );
      return;
   }

   printf( " {\n" );
   printf( "   MERROR_RETVAL retval = MERROR_OK;\n" );
   printf( "   size_t written = 0;\n" );
   printf( "   size_t i = 0;\n" );
   printf( "   off_t header = 0;\n" );

   printf(
      "   debug_printf( MSERIAL_TRACE_LVL, \"serializing struct %s...\" );\n",
      parsed->name );

   printf(
      "   header = mserialize_header( ser_f, MSERIALIZE_TYPE_OBJECT, 0 );\n" );

   /* Handle array value simply for complex objects. */
   printf( "   for( i = 0 ; array > i ; i++ ) {\n" );

   for( i = 0 ; parsed->fields_ct > i ; i++ ) {
      /* Get the type string for this field. */
      switch( parsed->fields[i].type ) {
      case 1: /* struct MDATA_VECTOR */
         memset( type_str, '\0', PARSE_TOKEN_SZ + 1 ); \
         strcpy( type_str, "struct_MDATA_VECTOR" );
         break;

      parse_field_type_table( parse_field_type_str )
      }

      if( 0 < strlen( parsed->fields[i].vector_type ) ) {
         /* If it's a vector, call the specialized vector serializer we emit
          * in the next function.
          */
         printf(
            "      retval = mserialize_vector_%s( "
            "ser_f, &written, &(p_ser_struct->%s) );\n",
            parsed->fields[i].vector_type, parsed->fields[i].name );

      } else if( 1 < parsed->fields[i].array ) {
         /* If it's an array, just pass the field directly and not a pointer
          * to it.
          */
         printf(
            "      retval = mserialize_%s( "
            "ser_f, &written, p_ser_struct->%s, %d );\n",
            type_str, parsed->fields[i].name, parsed->fields[i].array );
      } else {
         printf(
            "      retval = mserialize_%s( "
            "ser_f, &written, &(p_ser_struct->%s), %d );\n",
            type_str, parsed->fields[i].name, parsed->fields[i].array );
      }
      printf( "      maug_cleanup_if_not_ok();\n" );
      printf( "      *p_sz += written;\n" );
   }

   /* Handle array value simply for complex objects. */
   printf( "   }\n" );

   printf( "   retval = mserialize_footer( ser_f, header, 0 );\n" );

   printf(
      "   debug_printf( MSERIAL_TRACE_LVL, \"serialized struct %s.\" );\n",
      parsed->name );

   printf( "cleanup:\n   return retval;\n}\n\n" );


   /* TODO: Emit reader function with running size_t that stops parsing when
    *       the size_t matches the size of the object that was saved.
    */

}

void parse_emit_vector( struct STRUCT_PARSED* parsed, int prototype ) {
   size_t i = 0,
      j = 0;
   char type_str[PARSE_TOKEN_SZ + 1];

   printf( "MERROR_RETVAL mserialize_vector_struct_%s( "
      "mfile_t* ser_f, size_t* p_sz, struct MDATA_VECTOR* p_ser_vec )",
      parsed->name, parsed->name );

   if( prototype ) {
      printf( ";\n\n" );
      return;
   }

   printf( " {\n" );
   printf( "   MERROR_RETVAL retval = MERROR_OK;\n" );
   printf( "   size_t written = 0;\n" );
   printf( "   size_t i = 0;\n" );
   printf( "   off_t header = 0;\n" );
   printf( "   int autolock = 0;\n" );

   printf( "   debug_printf( "
      "MSERIAL_TRACE_LVL, \"serializing vector of %s...\" );\n",
      parsed->name );

   printf(
      "   header = mserialize_header( ser_f, MSERIALIZE_TYPE_ARRAY, 0 );\n" );

   /* Skip serializing if nothing to serialize. */
   printf( "   if( 0 == mdata_vector_ct( p_ser_vec ) ) {\n" );
   printf( "      retval = mserialize_footer( ser_f, header, 0 );\n" );
   printf( "      return retval;\n" );
   printf( "   }\n" );

   /* Emit autolock for serialized vector. */
   printf( "   if( !mdata_vector_is_locked( p_ser_vec ) ) {\n" );
   printf( "      mdata_vector_lock( p_ser_vec );\n" );
   printf( "      autolock = 1;\n" );
   printf( "   }\n" );

   printf( "   for( i = 0 ; mdata_vector_ct( p_ser_vec ) > i ; i++ ) {\n" );
   printf( "      retval = mserialize_struct_%s( "
      "ser_f, &written, mdata_vector_get( p_ser_vec, i, struct %s ), 1 );\n",
      parsed->name, parsed->name );
   printf( "      maug_cleanup_if_not_ok();\n" );
   printf( "      *p_sz += written;\n" );
   printf( "   }\n" );

   printf( "   retval = mserialize_footer( ser_f, header, 0 );\n" );

   printf(
      "   debug_printf( MSERIAL_TRACE_LVL, \"serialized vector of %s.\" );\n",
      parsed->name );

   /* Emit cleanup. */
   printf( "cleanup:\n" );
   printf( "   if( autolock ) {\n" );
   printf( "      mdata_vector_unlock( p_ser_vec );\n" );
   printf( "   }\n" );
   printf( "   return retval;\n}\n\n" );


   /* TODO: Emit reader function with running size_t that stops parsing when
    *       the size_t matches the size of the object that was saved.
    */


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

   /*
   if(
      0 == parser->parsed.fields_ct &&
      0 != strncmp( parser->token, "sz", PARSE_TOKEN_SZ )
   ) {
      error_str( "invalid first field", parser->token );
      parser->parsed.valid = 0;
   } else */ if(
      0 == strncmp( parser->token, "no_serial", PARSE_TOKEN_SZ )
   ) {
      /* FIELD_NAME */
      parse_pop_mode( parser );
      /* FIELD_TYPE */
      parse_pop_mode( parser );
      /* Found deactivation dummy field. Fields after this are runtime-only. */
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

int parse_c( struct STRUCT_PARSER* parser, char c, int prototypes ) {
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
         debug_str( "stored define name", 
            parser->defines[parser->defines_ct].name );
         debug_int( "stored define value", 
            parser->defines[parser->defines_ct].value );
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

         } else if(
            0 == strncmp( parser->token, "struct MDATA_VECTOR", PARSE_TOKEN_SZ )
         ) {
            debug_int( "type", 1 );
            parser->parsed.fields[parser->parsed.fields_ct].type = 1;
            parser->parsed.fields[parser->parsed.fields_ct].array = 1;

         } else if( 0 == strncmp( parser->token, "struct ", PARSE_TOKEN_SZ ) ) {
            /* Do nothing! Prepend this "struct" to type token! */
            goto cleanup;

         } else if( 0 == strncmp( parser->token, "struct", PARSE_TOKEN_SZ ) ) {
            /* Make sure a single space exists after "struct". */
            retval = parse_token_append( parser, ' ' );
            goto cleanup;

         /* Note the default assignment of array as "1", to specify 1 item.
          * This makes writing primative serializers a bit easer.
          */
#define parse_field_type_assign( type_name, type_idx ) \
   } else if( 0 == strncmp( parser->token, #type_name, PARSE_TOKEN_SZ ) ) { \
      debug_int( "type", type_idx ); \
      (parser)->parsed.fields[parser->parsed.fields_ct].type = type_idx; \
      (parser)->parsed.fields[parser->parsed.fields_ct].array = 1;

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

      case PARSE_MODE_COMMENT:
         debug_str( "comment", parser->token );
         if( 0 == strncmp( parser->token, "vector_type", PARSE_TOKEN_SZ ) ) {
            parse_push_mode( parser, PARSE_MODE_COMMENT_VECTOR_TYPE );
         }
         break;

      case PARSE_MODE_COMMENT_VECTOR_TYPE:
         strncpy(
            parser->parsed.fields[parser->parsed.fields_ct].vector_type,
            parser->token,
            PARSE_TOKEN_SZ );
         parse_pop_mode( parser );
         break;
      }

      if(
         /* Array sizes should be taken as one whole string, even with spaces.
          */
         PARSE_MODE_STRUCT_FIELD_ARRAY != parse_mode( parser )
      ) {
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
            parse_emit_struct( &(parser->parsed), prototypes );
            parse_emit_vector( &(parser->parsed), prototypes );
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

int parse_header( const char* header_path, int prototypes ) {
   int retval = 0;
   FILE* header_f = NULL;
   size_t read_sz = 0;
   char read_buf[READ_BUF_SZ + 1];
   struct STRUCT_PARSER parser;
   int i = 0;

   header_f = fopen( header_path, "r" );
   if( NULL == header_f ) {
      error_str( "coult not open", header_path );
      retval = 1;
      goto cleanup;
   }

   memset( &parser, '\0', sizeof( struct STRUCT_PARSER ) );
   do {
      memset( read_buf, '\0', READ_BUF_SZ + 1 );
      read_sz = fread( read_buf, 1, READ_BUF_SZ, header_f );
      for( i = 0 ; READ_BUF_SZ > i && '\0' != read_buf[i] ; i++ ) {
         retval = parse_c( &parser, read_buf[i], prototypes );
         assert( 0 == retval );
      }
   } while( read_sz == READ_BUF_SZ );

cleanup:

   if( NULL != header_f ) {
      fclose( header_f );
   }

   return retval;
}

int main( int argc, char* argv[] ) {
   int retval = 0;
   int i = 0;
   int prototypes = 0;

   if( 2 > argc ) {
      fprintf( stderr, "usage: %s <header.h>\n", argv[0] );
      retval = 1;
      goto cleanup;
   }

   for( i = 1 ; argc > i ; i++ ) {
      if( 0 == strcmp( "-p", argv[i] ) ) {
         prototypes = 1;
      } else {
         retval = parse_header( argv[i], prototypes );
      }
   }

cleanup:

   return retval;
}

