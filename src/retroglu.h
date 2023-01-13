
#ifndef RETROGLU_H
#define RETROGLU_H

#define RETROGLU_PARSER_ERROR -1

#ifndef RETROGLU_PARSER_TOKEN_SZ_MAX
#  define RETROGLU_PARSER_TOKEN_SZ_MAX 32
#endif /* !RETROGLU_PARSER_TOKEN_SZ_MAX */

#define RETROGLU_PARSER_STATE_NONE 0
#define RETROGLU_PARSER_STATE_VERTEX_X 1
#define RETROGLU_PARSER_STATE_VERTEX_Y 2
#define RETROGLU_PARSER_STATE_VERTEX_Z 3
#define RETROGLU_PARSER_STATE_COMMENT 4
#define RETROGLU_PARSER_STATE_FACE_TOKEN 5
#define RETROGLU_PARSER_STATE_FACE_VERTICES 6
#define RETROGLU_PARSER_STATE_FACE_TEXTURES 7
#define RETROGLU_PARSER_STATE_FACE_NORMALS 8

#ifndef RETROGLU_FACE_VERTICES_SZ_MAX
#  define RETROGLU_FACE_VERTICES_SZ_MAX 3
#endif /* !RETROGLU_FACE_VERTICES_SZ_MAX */

struct RETROGLU_VERTEX {
   float x;
   float y;
   float z;
};

struct RETROGLU_FACE {
   /**
    * \brief List of vertex indices from the associated
    *        RETROGLU_PARSER::vertices.
    *
    * The size of this array is fixed to simplify allocation of arrays.
    */
   int vertice_idxs[RETROGLU_FACE_VERTICES_SZ_MAX];
   int vertice_idxs_sz;
};

struct RETROGLU_PARSER {
   unsigned char* obj_buf;
   long int obj_buf_sz;
   struct RETROGLU_VERTEX* vertices;
   int vertices_sz_max;
   int vertices_sz;
   /**
    * \brief List of faces from an OBJ file. Faces comprise a list of polygons
    *        denoted by index of the vertices in RETROGLU_PARSER::vertices.
    */
   struct RETROGLU_FACE* faces;
   int faces_sz_max;
   int faces_sz;
   int state;
   char token[RETROGLU_PARSER_TOKEN_SZ_MAX];
   int token_sz;
};

typedef int (*retroglu_token_cb)( struct RETROGLU_PARSER* parser );

void retroglu_parse_init(
   struct RETROGLU_PARSER* parser, 
   struct RETROGLU_VERTEX* vertices, int vertices_sz_max,
   struct RETROGLU_FACE* faces, int faces_sz_max );

#define retroglu_parser_state( parser, new_state ) \
   printf( "changing parser to state: %d\n", new_state ); \
   (parser)->state = new_state;

int retroglu_parse_obj_c( struct RETROGLU_PARSER* parser, unsigned char c );

#define RETROGLU_OBJ_TOKENS( f ) f( "v", retroglu_token_vertice ) f( "f", retroglu_token_face )

#ifdef RETROGLU_C

#  define RETROGLU_OBJ_TOKEN_STRINGS( token, callback ) token,

int retroglu_token_vertice( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_VERTEX_X );
   return RETROFLAT_OK;
}

int retroglu_token_face( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_VERTICES );
   parser->faces[parser->faces_sz].vertice_idxs_sz = 0;
   return RETROFLAT_OK;
}

char* g_retroglu_token_strings[] = {
   RETROGLU_OBJ_TOKENS( RETROGLU_OBJ_TOKEN_STRINGS )  
   ""
};

#  define RETROGLU_OBJ_TOKEN_CALLBACKS( token, callback ) callback,

retroglu_token_cb g_retroglu_token_callbacks[] = {
   RETROGLU_OBJ_TOKENS( RETROGLU_OBJ_TOKEN_CALLBACKS )  
   NULL
};

void retroglu_parse_init(
   struct RETROGLU_PARSER* parser, 
   struct RETROGLU_VERTEX* vertices, int vertices_sz_max,
   struct RETROGLU_FACE* faces, int faces_sz_max
) {
   parser->vertices = vertices;
   parser->vertices_sz = 0;
   parser->vertices_sz_max = vertices_sz_max;
   parser->faces = faces;
   parser->faces_sz = 0;
   parser->faces_sz_max = faces_sz_max;
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );
   parser->token_sz = 0;
}

void retroglu_null_e( struct RETROGLU_PARSER* parser ) {
   int i = 0;

   /* Look for an 'E-nnn' in a floating point number and chop it off. */
   for( i = 0 ; parser->token_sz > i ; i++ ) {
      if( 'E' == parser->token[i] ) {
         parser->token[i] = '\0';
         break;
      }
   }
}

int retroglu_parse_token( struct RETROGLU_PARSER* parser ) {
   int i = 0;
   int retval = RETROFLAT_OK;

   if( 0 == parser->token_sz ) {
      /* Empty token. */
      goto cleanup;
   }

   /* NULL-terminate token. */
   parser->token[parser->token_sz] = '\0';

   printf( "token: %s\n", parser->token );

   if( RETROGLU_PARSER_STATE_VERTEX_X == parser->state ) {
      /* First number after "v", probably the X. */
      /* TODO: Validation. */
      retroglu_null_e( parser );
      parser->vertices[parser->vertices_sz].x = atof( parser->token );
      printf( "vertice %d X: %f\n",
         parser->vertices_sz, parser->vertices[parser->vertices_sz].x );
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_VERTEX_Y );

   } else if( RETROGLU_PARSER_STATE_VERTEX_Y == parser->state ) {
      /* Number after X, probably Y. */
      /* TODO: Validation. */
      retroglu_null_e( parser );
      parser->vertices[parser->vertices_sz].y = atof( parser->token );
      printf( "vertice %d Y: %f\n",
         parser->vertices_sz, parser->vertices[parser->vertices_sz].y );
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_VERTEX_Z );

   } else if( RETROGLU_PARSER_STATE_VERTEX_Z == parser->state ) {
      /* Number after Y, probably Z. */
      /* TODO: Validation. */
      retroglu_null_e( parser );
      parser->vertices[parser->vertices_sz].z = atof( parser->token );
      printf( "vertice %d Z: %f\n",
         parser->vertices_sz, parser->vertices[parser->vertices_sz].z );
      parser->vertices_sz++;
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );

      /* TODO: Handle W. */

   } else if( RETROGLU_PARSER_STATE_FACE_VERTICES == parser->state ) {
      /* Parsing face vertex indices. */
      parser->faces[parser->faces_sz].vertice_idxs[
         parser->faces[parser->faces_sz].vertice_idxs_sz] =
            atoi( parser->token );

      printf( "face %d, vertice %d: %d\n",
         parser->faces_sz, parser->faces[parser->faces_sz].vertice_idxs_sz,
         parser->faces[parser->faces_sz].vertice_idxs[
            parser->faces[parser->faces_sz].vertice_idxs_sz] );

      parser->faces[parser->faces_sz].vertice_idxs_sz++;

   } else {
      /* Check against generic tokens. */
      while( '\0' != g_retroglu_token_strings[i][0] ) {
         if(
            parser->token_sz == strlen( g_retroglu_token_strings[i] ) &&
            0 == strncmp(
               parser->token, g_retroglu_token_strings[i], parser->token_sz )
         ) {
            retval = g_retroglu_token_callbacks[i]( parser );
            goto cleanup;
         }
         i++;
      }
   }

cleanup:

   /* Reset token. */
   parser->token_sz = 0;

   return retval;
}

int retroglu_append_token( struct RETROGLU_PARSER* parser, unsigned char c ) {
   parser->token[parser->token_sz++] = c;

   /* Protect against token overflow. */
   if( parser->token_sz >= RETROGLU_PARSER_TOKEN_SZ_MAX ) {
      printf( "token out of bounds!\n" );
      return RETROGLU_PARSER_ERROR;
   }

   return RETROFLAT_OK;
}

int retroglu_parse_obj_c( struct RETROGLU_PARSER* parser, unsigned char c ) {
   int retval = RETROFLAT_OK;

   if(
      RETROGLU_PARSER_STATE_COMMENT == parser->state && '\r' != c && '\n' != c
   ) {
      /* We're inside of a comment. */
      return RETROFLAT_OK;
   }

   switch( c ) {
   case '#':
      /* Start of a comment. */
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_COMMENT );
      break;

   case '\r':
   case '\n':
      /* New line! End of a comment or token. */
      if( RETROGLU_PARSER_STATE_COMMENT == parser->state ) {
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );
         return RETROFLAT_OK;

      } else if(
         RETROGLU_PARSER_STATE_FACE_VERTICES == parser->state
      ) {
         retval = retroglu_parse_token( parser );
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );
         parser->faces_sz++; /* Newline means this face is done. */
         return retval;
         
      } else if(
         RETROGLU_PARSER_STATE_FACE_TEXTURES == parser->state ||
         RETROGLU_PARSER_STATE_FACE_NORMALS == parser->state
      ) {
         /* TODO: These are not handled yet, otherwise they could go above. */
         parser->token_sz = 0;
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );
         parser->faces_sz++; /* Newline means this face is done. */
         return RETROFLAT_OK;

      } else {
         return retroglu_parse_token( parser );
      }

   case '\t':
   case ' ':
      /* Whitespace. Inside of a comment or time for a new token. */
      if( RETROGLU_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing on spaces in comments. */
         return RETROFLAT_OK;

      } else if( RETROGLU_PARSER_STATE_FACE_TOKEN == parser->state ) {
         /* Just advance to parsing vertices. */
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_VERTICES );
         return RETROFLAT_OK;
      
      } else if( RETROGLU_PARSER_STATE_FACE_VERTICES == parser->state ) {
         retval = retroglu_parse_token( parser );
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_TEXTURES );
         return retval;

      } else if( RETROGLU_PARSER_STATE_FACE_TEXTURES == parser->state ) {
         /* TODO: Handle texture tokens. */
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_NORMALS );
         parser->token_sz = 0;
         return RETROFLAT_OK;

      } else if( RETROGLU_PARSER_STATE_FACE_NORMALS == parser->state ) {
         /* TODO: Handle normal tokens. */
         parser->token_sz = 0;
         return RETROFLAT_OK;

      } else {
         return retroglu_parse_token( parser );
      }

   case '/':
      if(
         RETROGLU_PARSER_STATE_FACE_VERTICES == parser->state ||
         RETROGLU_PARSER_STATE_FACE_TEXTURES == parser->state ||
         RETROGLU_PARSER_STATE_FACE_NORMALS == parser->state
      ) {
         /* Move to next number inside of face v/t/n. */
         return retroglu_parse_token( parser );
      }
      /* If not part of a face, fall through to default append. */
      return retroglu_append_token( parser, c );

   default:
      return retroglu_append_token( parser, c );
   }

   return RETROFLAT_OK;
}

#endif /* RETROGLU_C */

#endif /* !RETROGLU_H */

