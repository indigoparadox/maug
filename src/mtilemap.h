
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
#  define MTILEMAP_TOKEN_SZ_MAX 4096
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

typedef MERROR_RETVAL
(*mtilemap_tj_parse_cb)( const char* filename, struct MTILEMAP* t );

struct MTILEMAP_PARSER {
   uint8_t pstate;
   uint8_t pstate_prev;
   uint8_t mstate;
   uint8_t mstate_prev;
   size_t i;
   char* buffer;
   size_t buffer_sz;
   char token[MTILEMAP_TOKEN_SZ_MAX];
   size_t token_sz;
   struct MTILEMAP* t;
   mtilemap_tj_parse_cb tj_parse_cb;
};

#define MTILEMAP_PARSER_PSTATE_TABLE( f ) \
   f( MTILEMAP_PSTATE_NONE, 0 ) \
   f( MTILEMAP_PSTATE_OBJECT_KEY, 1 ) \
   f( MTILEMAP_PSTATE_OBJECT_VAL, 2 ) \
   f( MTILEMAP_PSTATE_STRING_KEY, 3 ) \
   f( MTILEMAP_PSTATE_STRING_VAL, 4 ) \
   f( MTILEMAP_PSTATE_LIST, 5 ) \
   f( MTILEMAP_PSTATE_OBJECT_LIST_VAL, 6 )

#define MTILEMAP_PARSER_MSTATE_TABLE( f ) \
   f( MTILEMAP_MSTATE_NONE,         0, "", 0 ) \
   f( MTILEMAP_MSTATE_HEIGHT,       1, "height", 0 ) \
   f( MTILEMAP_MSTATE_WIDTH,        2, "width", 0 ) \
   f( MTILEMAP_MSTATE_LAYERS,       3, "layers", 0 ) \
   f( MTILEMAP_MSTATE_LAYER_DATA,   4, "data", MTILEMAP_MSTATE_LAYERS ) \
   f( MTILEMAP_MSTATE_LAYER_NAME,   5, "name", MTILEMAP_MSTATE_LAYERS ) \
   f( MTILEMAP_MSTATE_TILES,        6, "tiles", 0 ) \
   f( MTILEMAP_MSTATE_TILES_ID,     7, "id", MTILEMAP_MSTATE_TILES ) \
   f( MTILEMAP_MSTATE_TILES_IMAGE,  8, "image", MTILEMAP_MSTATE_TILES ) \
   f( MTILEMAP_MSTATE_TILESETS,     9, "tilesets", 0 ) \
   f( MTILEMAP_MSTATE_TILESETS_SRC, 10, "source", MTILEMAP_MSTATE_TILESETS )


MERROR_RETVAL
mtilemap_parse_json_c( struct MTILEMAP_PARSER* parser, char c );

MERROR_RETVAL
mtilemap_parse_json_file( const char* filename, struct MTILEMAP* m );

#ifdef MTILEMAP_C

#  include <stdio.h>

#define mtilemap_parser_pstate( parser, new_pstate ) \
   parser->pstate_prev = parser->pstate; \
   parser->pstate = new_pstate; \
   debug_printf( \
      1, "parser pstate: %s (prev: %s)", \
         gc_mtilemap_pstate_names[parser->pstate], \
         gc_mtilemap_pstate_names[parser->pstate_prev] );


#  define mtilemap_parser_invalid_c( parser, c, retval ) \
   error_printf( "invalid %c detected at char: " SIZE_T_FMT, c, parser->i ); \
   error_printf( "pstate: %s, pstate_prev: %s, mstate: %s, mstate_prev: %s", \
         gc_mtilemap_pstate_names[parser->pstate], \
         gc_mtilemap_pstate_names[parser->pstate_prev], \
         gc_mtilemap_mstate_names[parser->mstate], \
         gc_mtilemap_mstate_names[parser->mstate_prev] ); \
   retval = 1;

#  define MTILEMAP_PARSER_PSTATE_TABLE_CONST( name, idx ) \
      static MAUG_CONST uint8_t name = idx;

MTILEMAP_PARSER_PSTATE_TABLE( MTILEMAP_PARSER_PSTATE_TABLE_CONST )

#  define MTILEMAP_PARSER_PSTATE_TABLE_NAME( name, idx ) \
      #name,

static MAUG_CONST char* gc_mtilemap_pstate_names[] = {
   MTILEMAP_PARSER_PSTATE_TABLE( MTILEMAP_PARSER_PSTATE_TABLE_NAME )
   ""
};

/* TODO: Better swap logic. */
#define mtilemap_parser_mstate( parser, new_mstate ) \
   if( new_mstate != parser->mstate_prev ) { \
      parser->mstate_prev = parser->mstate; \
   } \
   parser->mstate = new_mstate; \
   debug_printf( \
      1, "parser mstate: %s (prev: %s)", \
         gc_mtilemap_mstate_names[parser->mstate], \
         gc_mtilemap_mstate_names[parser->mstate_prev] );

#  define MTILEMAP_PARSER_MSTATE_TABLE_CONST( name, idx, token, parent ) \
      static MAUG_CONST uint8_t name = idx;

MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_CONST )

#  define MTILEMAP_PARSER_MSTATE_TABLE_NAME( name, idx, token, parent ) \
      #name,

static MAUG_CONST char* gc_mtilemap_mstate_names[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_NAME )
   ""
};

#  define MTILEMAP_PARSER_MSTATE_TABLE_TOKEN( name, idx, token, parent ) \
      token,

static MAUG_CONST char* gc_mtilemap_mstate_tokens[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_TOKEN )
   ""
};

#  define MTILEMAP_PARSER_MSTATE_TABLE_PARENT( name, idx, token, parent ) \
      parent,

static MAUG_CONST uint8_t gc_mtilemap_mstate_parents[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_PARENT )
   0
};

/* === */

static void
mtilemap_parse_append_token( struct MTILEMAP_PARSER* parser, char c ) {
   parser->token[parser->token_sz++] = c;
   parser->token[parser->token_sz] = '\0';
   assert( parser->token_sz <= MTILEMAP_TOKEN_SZ_MAX );
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
   size_t j = 0;

   debug_printf( 1, "c: %c", c );

   switch( c ) {
   case '\r':
   case '\n':
   case '\t':
   case ' ':
      if(
         MTILEMAP_PSTATE_STRING_KEY == parser->pstate ||
         MTILEMAP_PSTATE_STRING_VAL == parser->pstate
      ) {
         mtilemap_parse_append_token( parser, c );
      }
      break;

   case '{':
      if(
         MTILEMAP_PSTATE_NONE == parser->pstate ||
         MTILEMAP_PSTATE_LIST == parser->pstate ||
         MTILEMAP_PSTATE_OBJECT_VAL == parser->pstate
      ) {
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_OBJECT_KEY );
         mtilemap_parse_reset_token( parser );

      } else if(
         MTILEMAP_PSTATE_STRING_KEY == parser->pstate ||
         MTILEMAP_PSTATE_STRING_VAL == parser->pstate
      ) {
         mtilemap_parse_append_token( parser, c );

      } else {
         mtilemap_parser_invalid_c( parser, c, retval );
      }
      break;

   case '}':
      if( MTILEMAP_PSTATE_OBJECT_VAL == parser->pstate ) {
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_NONE );
         mtilemap_parse_reset_token( parser );

      } else if(
         MTILEMAP_PSTATE_STRING_KEY == parser->pstate ||
         MTILEMAP_PSTATE_STRING_VAL == parser->pstate
      ) {
         mtilemap_parse_append_token( parser, c );

      } else {
         mtilemap_parser_invalid_c( parser, c, retval );
      }
      break;

   case '[':
      if(
         MTILEMAP_PSTATE_NONE == parser->pstate ||
         MTILEMAP_PSTATE_OBJECT_VAL == parser->pstate
      ) {
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_LIST );
         mtilemap_parse_reset_token( parser );

      } else if(
         MTILEMAP_PSTATE_STRING_KEY == parser->pstate ||
         MTILEMAP_PSTATE_STRING_VAL == parser->pstate
      ) {
         mtilemap_parse_append_token( parser, c );

      } else {
         mtilemap_parser_invalid_c( parser, c, retval );
      }
      break;

   case ']':
      if( MTILEMAP_PSTATE_LIST == parser->pstate ) {
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_OBJECT_VAL );
         mtilemap_parse_reset_token( parser );

         if( MTILEMAP_MSTATE_LAYER_DATA == parser->mstate ) {
            mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_LAYERS );
         }

      } else if( MTILEMAP_PSTATE_NONE == parser->pstate ) {
         /* TODO */
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_OBJECT_VAL );

      } else if(
         MTILEMAP_PSTATE_STRING_KEY == parser->pstate ||
         MTILEMAP_PSTATE_STRING_VAL == parser->pstate
      ) {
         mtilemap_parse_append_token( parser, c );

      } else {
         mtilemap_parser_invalid_c( parser, c, retval );
      }
      break;

   case '"':
      if( MTILEMAP_PSTATE_OBJECT_KEY == parser->pstate ) {
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_STRING_KEY );

      } else if( MTILEMAP_PSTATE_OBJECT_VAL == parser->pstate ) {
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_STRING_VAL );

      } else if( MTILEMAP_PSTATE_STRING_KEY == parser->pstate ) {
         debug_printf( 1, "found key: %s", parser->token );
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_OBJECT_KEY );

         /* Figure out what the key is for. */
         j = 1;
         while( '\0' != gc_mtilemap_mstate_tokens[j][0] ) {
            if(
               /* If a non-zero parent is specified, this state can only be
                * reached THROUGH that parent state. This allows us to have
                * keys with the same name but different parents!
                */
               (0 == gc_mtilemap_mstate_parents[j] ||
                  parser->mstate == gc_mtilemap_mstate_parents[j]) &&
               /* Make sure tokens match. */
               strlen( gc_mtilemap_mstate_tokens[j] ) == parser->token_sz &&
               0 == strncmp(
                  parser->token, gc_mtilemap_mstate_tokens[j], parser->token_sz
               )
            ) {
               /* Found it! */
               mtilemap_parser_mstate( parser, j );
               break;
            }
            j++;
         }

         mtilemap_parse_reset_token( parser );

      } else if( MTILEMAP_PSTATE_STRING_VAL == parser->pstate ) {
         debug_printf( 1, "found value: %s", parser->token );
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_OBJECT_VAL );

         if( MTILEMAP_MSTATE_TILESETS_SRC == parser->mstate ) {
            debug_printf( 1, "parsing %s...", parser->token );
            parser->tj_parse_cb( parser->token, parser->t );
         }

         mtilemap_parse_reset_token( parser );

      } else {
         mtilemap_parser_invalid_c( parser, c, retval );
      }
      break;

   case ',':
      if( MTILEMAP_PSTATE_OBJECT_VAL == parser->pstate ) {
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_OBJECT_KEY );
         mtilemap_parse_reset_token( parser );
         /* TODO: Advance object. */

         /* Go back to parent if we have one. */
         if( MTILEMAP_MSTATE_NONE != parser->mstate ) {
            /* XXX */
            /* mtilemap_parser_mstate( parser, parser->mstate_prev ); */
         }

      } else if( MTILEMAP_PSTATE_NONE == parser->pstate ) {
         /* TODO */
         mtilemap_parser_mstate( parser, MTILEMAP_PSTATE_OBJECT_LIST_VAL );

      } else if( MTILEMAP_PSTATE_LIST == parser->pstate ) {
         /* TODO */

      } else if(
         MTILEMAP_PSTATE_STRING_KEY == parser->pstate ||
         MTILEMAP_PSTATE_STRING_VAL == parser->pstate
      ) {
         mtilemap_parse_append_token( parser, c );

      } else {
         mtilemap_parser_invalid_c( parser, c, retval );
      }
      break;

   case ':':
      if( MTILEMAP_PSTATE_OBJECT_KEY == parser->pstate ) {
         mtilemap_parser_pstate( parser, MTILEMAP_PSTATE_OBJECT_VAL );
         mtilemap_parse_reset_token( parser );

      } else if(
         MTILEMAP_PSTATE_STRING_KEY == parser->pstate ||
         MTILEMAP_PSTATE_STRING_VAL == parser->pstate
      ) {
         mtilemap_parse_append_token( parser, c );
         /* TODO: Store key. */

      } else {
         mtilemap_parser_invalid_c( parser, c, retval );
      }
      break;

   default:
      mtilemap_parse_append_token( parser, c );
      break;
   }

   return retval;
}

MERROR_RETVAL
mtilemap_parse_json_file( const char* filename, struct MTILEMAP* t ) {
   FILE* tilemap_file = NULL;
   size_t tilemap_file_sz = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct MTILEMAP_PARSER parser;
   char filename_path[RETROFLAT_PATH_MAX];

   memset( &parser, '\0', sizeof( struct MTILEMAP_PARSER ) );
   parser.t = t;
   parser.tj_parse_cb = mtilemap_parse_json_file;

   memset( filename_path, '\0', RETROFLAT_PATH_MAX );
   /* TODO: Configurable path. */
   snprintf( filename_path, RETROFLAT_PATH_MAX, "mapsrc/%s", filename );

   /* Open and get the file buffer size. */
   debug_printf( 2, "opening tilemap file %s...", filename_path );
   tilemap_file = fopen( filename_path, "r" );
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

