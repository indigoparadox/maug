
#ifndef RETROGLU_H
#define RETROGLU_H

#define RETROGLU_PARSER_ERROR -1

#define RETROGLU_PARSER_TOKEN_SZ_MAX 32

#define RETROGLU_PARSER_STATE_NONE 0
#define RETROGLU_PARSER_STATE_VERTEX_X 1
#define RETROGLU_PARSER_STATE_VERTEX_Y 2
#define RETROGLU_PARSER_STATE_VERTEX_Z 3
#define RETROGLU_PARSER_STATE_COMMENT 4

struct RETROGLU_VERTEX {
   float x;
   float y;
   float z;
};

struct RETROGLU_PARSER {
   unsigned char* obj_buf;
   long int obj_buf_sz;
   struct RETROGLU_VERTEX* vertices;
   int vertices_max;
   int vertices_sz;
   int state;
   char token[RETROGLU_PARSER_TOKEN_SZ_MAX];
   int token_sz;
};

typedef int (*retroglu_token_cb)( struct RETROGLU_PARSER* parser );

void retroglu_parse_init( struct RETROGLU_PARSER* parser );
int retroglu_parse_obj_c( struct RETROGLU_PARSER* parser, unsigned char c );

#define RETROGLU_OBJ_TOKENS( f ) f( "v", retroglu_token_vertice )

#ifdef RETROGLU_C

#  define RETROGLU_OBJ_TOKEN_STRINGS( token, callback ) token,

int retroglu_token_vertice( struct RETROGLU_PARSER* parser ) {
   parser->state = RETROGLU_PARSER_STATE_VERTEX_X;
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

void retroglu_parse_init( struct RETROGLU_PARSER* parser ) {
   parser->vertices_sz = 0;
   parser->state = RETROGLU_PARSER_STATE_NONE;
   parser->token_sz = 0;
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
      parser->vertices[parser->vertices_sz].x = atof( parser->token );
      printf( "vertice %d X: %f\n",
         parser->vertices_sz, parser->vertices[parser->vertices_sz].x );
      parser->state = RETROGLU_PARSER_STATE_VERTEX_Y;

   } else if( RETROGLU_PARSER_STATE_VERTEX_Y == parser->state ) {
      parser->vertices[parser->vertices_sz].y = atof( parser->token );
      printf( "vertice %d Y: %f\n",
         parser->vertices_sz, parser->vertices[parser->vertices_sz].y );
      parser->state = RETROGLU_PARSER_STATE_VERTEX_Z;

   } else if( RETROGLU_PARSER_STATE_VERTEX_Z == parser->state ) {
      parser->vertices[parser->vertices_sz].z = atof( parser->token );
      printf( "vertice %d Z: %f\n",
         parser->vertices_sz, parser->vertices[parser->vertices_sz].z );
      parser->vertices_sz++;
      parser->state = RETROGLU_PARSER_STATE_NONE;

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

int retroglu_parse_obj_c( struct RETROGLU_PARSER* parser, unsigned char c ) {

   if(
      RETROGLU_PARSER_STATE_COMMENT == parser->state && '\r' != c && '\n' != c
   ) {
      /* We're inside of a comment. */
      return RETROFLAT_OK;
   }

   switch( c ) {
   case '#':
      /* Start of a comment. */
      parser->state = RETROGLU_PARSER_STATE_COMMENT;
      break;

   case '\r':
   case '\n':
      /* New line! End of a comment or token. */
      if( RETROGLU_PARSER_STATE_COMMENT == parser->state ) {
         parser->state = RETROGLU_PARSER_STATE_NONE;
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

      } else {
         return retroglu_parse_token( parser );
      }

   default:
      parser->token[parser->token_sz++] = c;

      /* Protect against token overflow. */
      if( parser->token_sz >= RETROGLU_PARSER_TOKEN_SZ_MAX ) {
         printf( "token out of bounds!\n" );
         return RETROGLU_PARSER_ERROR;
      }
      break;
   }

   return RETROFLAT_OK;
}

#endif /* RETROGLU_C */

#endif /* !RETROGLU_H */

