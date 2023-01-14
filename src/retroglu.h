
#ifndef RETROGLU_H
#define RETROGLU_H

/**
 * \addtogroup maug_retroglu RetroGLU API
 * \brief Library of tools for working with RetroFlat and OpenGL.
 * \{
 *
 * \file retroglu.h
 * \brief Library of tools for working with RetroFlat and OpenGL.
 */

#define RETROGLU_PARSER_ERROR -1

#ifndef RETROGLU_PARSER_TOKEN_SZ_MAX
#  define RETROGLU_PARSER_TOKEN_SZ_MAX 32
#endif /* !RETROGLU_PARSER_TOKEN_SZ_MAX */

/**
 * \addtogroup maug_retroglu_obj_fsm RetroGLU OBJ Parser
 * \{
 */

/**
 * \addtogroup maug_retroglu_obj_fsm_states RetroGLU OBJ Parser States
 * \{
 */

#define RETROGLU_PARSER_STATE_NONE 0
#define RETROGLU_PARSER_STATE_VERTEX_X 1
#define RETROGLU_PARSER_STATE_VERTEX_Y 2
#define RETROGLU_PARSER_STATE_VERTEX_Z 3
#define RETROGLU_PARSER_STATE_COMMENT 4
#define RETROGLU_PARSER_STATE_FACE_TOKEN 5
#define RETROGLU_PARSER_STATE_FACE_VERTEX 6
#define RETROGLU_PARSER_STATE_FACE_TEXTURE 7
#define RETROGLU_PARSER_STATE_FACE_NORMAL 8
#define RETROGLU_PARSER_STATE_FACE_MATERIAL 9
#define RETROGLU_PARSER_STATE_MATERIAL_NAME 10
#define RETROGLU_PARSER_STATE_MATERIAL_AMB 11
#define RETROGLU_PARSER_STATE_MATERIAL_DIF 12
#define RETROGLU_PARSER_STATE_MATERIAL_SPEC 13
#define RETROGLU_PARSER_STATE_MATERIAL_LIB 14
#define RETROGLU_PARSER_STATE_MTL_KD_R 15
#define RETROGLU_PARSER_STATE_MTL_KD_G 16
#define RETROGLU_PARSER_STATE_MTL_KD_B 17
#define RETROGLU_PARSER_STATE_VNORMAL_X 18
#define RETROGLU_PARSER_STATE_VNORMAL_Y 19
#define RETROGLU_PARSER_STATE_VNORMAL_Z 20
#define RETROGLU_PARSER_STATE_VTEXTURE_X 21
#define RETROGLU_PARSER_STATE_VTEXTURE_Y 22
#define RETROGLU_PARSER_STATE_VTEXTURE_Z 23

/*! \} */ /* maug_retroglu_obj_fsm_states */

/*! \} */ /* maug_retroglu_obj_fsm */

#ifndef RETROGLU_FACE_VERTICES_SZ_MAX
#  define RETROGLU_FACE_VERTICES_SZ_MAX 3
#endif /* !RETROGLU_FACE_VERTICES_SZ_MAX */

#ifndef RETROGLU_MATERIAL_NAME_SZ_MAX
#  define RETROGLU_MATERIAL_NAME_SZ_MAX 32
#endif /* !RETROGLU_MATERIAL_NAME_SZ_MAX */

#ifndef RETROGLU_MATERIAL_LIB_SZ_MAX
#  define RETROGLU_MATERIAL_LIB_SZ_MAX 32
#endif /* !RETROGLU_MATERIAL_LIB_SZ_MAX */

struct RETROGLU_VERTEX {
   float x;
   float y;
   float z;
};

struct RETROGLU_MATERIAL {
   float ambient[4];
   float diffuse[4];
   float specular[4];
   char name[RETROGLU_MATERIAL_NAME_SZ_MAX];
};

struct RETROGLU_FACE {
   /**
    * \brief List of vertex indices from the associated
    *        RETROGLU_PARSER::vertices.
    *
    * The size of this array is fixed to simplify allocation of arrays.
    */
   int vertex_idxs[RETROGLU_FACE_VERTICES_SZ_MAX];
   int vnormal_idxs[RETROGLU_FACE_VERTICES_SZ_MAX];
   int vtexture_idxs[RETROGLU_FACE_VERTICES_SZ_MAX];
   int vertex_idxs_sz;
   int material_idx;
};

/**
 * \addtogroup maug_retroglu_obj_fsm
 * \{
 */

/**
 * \brief Change the parser state.
 * \param Pointer to the ::RETROGLU_PARSER to modify.
 * \param new_state \ref maug_retroglu_obj_fsm_states to set the parser to.
 */
#define retroglu_parser_state( parser, new_state ) \
   printf( "changing parser to state: %d\n", new_state ); \
   (parser)->state = new_state;

/**
 * \brief Table of OBJ file tokens understood by the parser.
 */
#define RETROGLU_OBJ_TOKENS( f ) \
   f( "v", retroglu_token_vertice ) \
   f( "vn", retroglu_token_vnormal ) \
   f( "f", retroglu_token_face ) \
   f( "usemtl", retroglu_token_usemtl ) \
   f( "newmtl", retroglu_token_newmtl ) \
   f( "mtllib", retroglu_token_mtllib ) \
   f( "Kd", retroglu_token_kd )

struct RETROGLU_PARSER;

/**
 * \brief Callback to execute when its associate in ::RETROGLU_OBJ_TOKENS is
 *        found in an OBJ file.
 */
typedef int (*retroglu_mtl_cb)(
   const char* filename, struct RETROGLU_PARSER* parser, void* data );

/**
 * \brief As retroglu_parse_obj_c() parses OBJ data, it populates this struct
 *        with object information.
 */
struct RETROGLU_PARSER {
   struct RETROGLU_VERTEX* vertices;
   int vertices_sz_max;
   int vertices_sz;
   struct RETROGLU_VERTEX* vnormals;
   int vnormals_sz;
   int vnormals_sz_max;
   struct RETROGLU_VERTEX* vtextures;
   int vtextures_sz;
   int vtextures_sz_max;
   /**
    * \brief List of faces from an OBJ file. Faces comprise a list of polygons
    *        denoted by index of the vertices in RETROGLU_PARSER::vertices.
    */
   struct RETROGLU_FACE* faces;
   int faces_sz_max;
   int faces_sz;
   int state;
   int material_idx;
   char token[RETROGLU_PARSER_TOKEN_SZ_MAX];
   int token_sz;
   struct RETROGLU_MATERIAL* materials;
   int materials_sz;
   int materials_sz_max;
   retroglu_mtl_cb load_mtl;
   void* load_mtl_data;
};

typedef int (*retroglu_token_cb)( struct RETROGLU_PARSER* parser );

/**
 * \related RETROGLU_PARSER
 * \brief Initialize a ::RETROGLU_PARSER.
 * \warning This should be run before retroglu_parse_obj_c() is called on the
 *          parser!
 */
void retroglu_parse_init(
   struct RETROGLU_PARSER* parser, 
   struct RETROGLU_VERTEX* vertices, int vertices_sz, int vertices_sz_max,
   struct RETROGLU_VERTEX* vnormals, int vnormals_sz, int vnormals_sz_max,
   struct RETROGLU_VERTEX* vtextures, int vtextures_sz, int vtextures_sz_max,
   struct RETROGLU_FACE* faces, int faces_sz, int faces_sz_max,
   struct RETROGLU_MATERIAL* materials, int materials_sz, int materials_sz_max,
   retroglu_mtl_cb load_mtl, void* load_mtl_data
);

/**
 * \related RETROGLU_PARSER
 * \brief Parse OBJ data into a parser, one character at a time.
 *
 * Generally, this should loop over a character array loaded from an OBJ file.
 */
int retroglu_parse_obj_c( struct RETROGLU_PARSER* parser, unsigned char c );

/*! \} */ /* maug_retroglu_obj_fsm */

void retroglu_draw_poly(
   struct RETROGLU_VERTEX* vertices, int vertices_sz,
   struct RETROGLU_VERTEX* vnormals, int vnormals_sz,
   struct RETROGLU_VERTEX* vtextures, int vtextures_sz,
   struct RETROGLU_FACE* faces, int faces_sz,
   struct RETROGLU_MATERIAL* materials, int materials_sz );

#ifdef RETROGLU_C

#  define RETROGLU_OBJ_TOKEN_STRINGS( token, callback ) token,

int retroglu_token_vertice( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_VERTEX_X );
   return RETROFLAT_OK;
}

int retroglu_token_vnormal( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_VNORMAL_X );
   return RETROFLAT_OK;
}

int retroglu_token_face( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_VERTEX );
   parser->faces[parser->faces_sz].vertex_idxs_sz = 0;
   return RETROFLAT_OK;
}

int retroglu_token_usemtl( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_MATERIAL );
   return RETROFLAT_OK;
}

int retroglu_token_newmtl( struct RETROGLU_PARSER* parser ) {
   parser->materials_sz++;
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_MATERIAL_NAME );
   return RETROFLAT_OK;
}

int retroglu_token_mtllib( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_MATERIAL_LIB );
   return RETROFLAT_OK;
}

int retroglu_token_kd( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_MTL_KD_R );
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
   struct RETROGLU_VERTEX* vertices, int vertices_sz, int vertices_sz_max,
   struct RETROGLU_VERTEX* vnormals, int vnormals_sz, int vnormals_sz_max,
   struct RETROGLU_VERTEX* vtextures, int vtextures_sz, int vtextures_sz_max,
   struct RETROGLU_FACE* faces, int faces_sz, int faces_sz_max,
   struct RETROGLU_MATERIAL* materials, int materials_sz, int materials_sz_max,
   retroglu_mtl_cb load_mtl, void* load_mtl_data
) {
   parser->vertices = vertices;
   parser->vertices_sz = vertices_sz;
   parser->vertices_sz_max = vertices_sz_max;
   parser->vnormals = vnormals;
   parser->vnormals_sz = vnormals_sz;
   parser->vnormals_sz_max = vnormals_sz_max;
   parser->vtextures = vtextures;
   parser->vtextures_sz = vtextures_sz;
   parser->vtextures_sz_max = vtextures_sz_max;
   parser->faces = faces;
   parser->faces_sz = faces_sz;
   parser->faces_sz_max = faces_sz_max;
   parser->materials = materials;
   parser->materials_sz = materials_sz;
   parser->materials_sz_max = materials_sz_max;
   parser->load_mtl = load_mtl;
   parser->load_mtl_data = load_mtl_data;
   assert( NULL != parser->vertices );
   assert( NULL != parser->faces );
   assert( NULL != parser->materials );
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );
   parser->token_sz = 0;
}

#define RETROGLU_TOKENS_VF( f ) \
   f( "X", VERTEX_X, vertices, vertices_sz, x, VERTEX_Y ) \
   f( "Y", VERTEX_Y, vertices, vertices_sz, y, VERTEX_Z ) \
   f( "Z", VERTEX_Z, vertices, vertices_sz, z, NONE ) \
   f( "normal X", VNORMAL_X, vnormals, vnormals_sz, x, VNORMAL_Y ) \
   f( "normal Y", VNORMAL_Y, vnormals, vnormals_sz, y, VNORMAL_Z ) \
   f( "normal Z", VNORMAL_Z, vnormals, vnormals_sz, z, NONE ) \
   f( "mtl Kd R", MTL_KD_R, materials, materials_sz-1, diffuse[0], MTL_KD_G ) \
   f( "mtl Kd G", MTL_KD_G, materials, materials_sz-1, diffuse[1], MTL_KD_B ) \
   f( "mtl Kd B", MTL_KD_B, materials, materials_sz-1, diffuse[2], NONE )

#define RETROGLU_TOKEN_PARSE_VF( desc, cond, array, sz, val, state_next ) \
   } else if( RETROGLU_PARSER_STATE_ ## cond == parser->state ) { \
      parser->array[parser->sz].val = strtod( parser->token, NULL ); \
      printf( "vertex %d " desc ": %f\n", \
         parser->sz, parser->array[parser->sz].val ); \
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_ ## state_next );

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

   if( RETROGLU_PARSER_STATE_MATERIAL_LIB == parser->state ) {

      printf( "parsing material lib: %s\n", parser->token );
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );
      assert( NULL != parser->load_mtl );
      return parser->load_mtl( parser->token, parser, parser->load_mtl_data );

   RETROGLU_TOKENS_VF( RETROGLU_TOKEN_PARSE_VF )

      /* TODO: Handle W. */
      /* TODO: Append 1.0 to materials. */

   } else if( RETROGLU_PARSER_STATE_FACE_VERTEX == parser->state ) {
      /* Parsing face vertex index. */
      parser->faces[parser->faces_sz].vertex_idxs[
         parser->faces[parser->faces_sz].vertex_idxs_sz] =
            atoi( parser->token );

      printf( "face %d, vertex %d: %d\n",
         parser->faces_sz, parser->faces[parser->faces_sz].vertex_idxs_sz,
         parser->faces[parser->faces_sz].vertex_idxs[
            parser->faces[parser->faces_sz].vertex_idxs_sz] );

      /* The new state is set in the parser below, as it could become
       * RETROGLU_PARSER_STATE_FACE_NORMAL or RETROGLU_PARSER_STATE_NONE,
       * depending on whether it's a whitespace/newline/slash after.
       * Same for index incr.
       */

   } else if( RETROGLU_PARSER_STATE_FACE_NORMAL == parser->state ) {

      /* Parsing face normal index. */
      parser->faces[parser->faces_sz].vnormal_idxs[
         parser->faces[parser->faces_sz].vertex_idxs_sz] =
            atoi( parser->token );

      printf( "face %d, normal %d: %d\n",
         parser->faces_sz, parser->faces[parser->faces_sz].vertex_idxs_sz,
         parser->faces[parser->faces_sz].vnormal_idxs[
            parser->faces[parser->faces_sz].vertex_idxs_sz] );

      /* The new state is set in the parser below, as it could become
       * RETROGLU_PARSER_STATE_FACE_NORMAL or RETROGLU_PARSER_STATE_NONE,
       * depending on whether it's a whitespace/newline/slash after.
       * Same for index incr.
       */

   } else if( RETROGLU_PARSER_STATE_FACE_TEXTURE == parser->state ) {

      /* Parsing face texture index. */
      parser->faces[parser->faces_sz].vtexture_idxs[
         parser->faces[parser->faces_sz].vertex_idxs_sz] =
            atoi( parser->token );

      printf( "face %d, texture %d: %d\n",
         parser->faces_sz, parser->faces[parser->faces_sz].vertex_idxs_sz,
         parser->faces[parser->faces_sz].vtexture_idxs[
            parser->faces[parser->faces_sz].vertex_idxs_sz] );

      /* The new state is set in the parser below, as it could become
       * RETROGLU_PARSER_STATE_FACE_NORMAL or RETROGLU_PARSER_STATE_NONE,
       * depending on whether it's a whitespace/newline/slash after.
       * Same for index incr.
       */

 
   } else if( RETROGLU_PARSER_STATE_FACE_MATERIAL == parser->state ) {

      /* Find the material index and assign it to the parser. */
      for( i = 0 ; parser->materials_sz > i ; i++ ) {
         printf( "%s vs %s\n", parser->materials[i].name, parser->token );
         if( 0 == strncmp(
            parser->materials[i].name, parser->token,
            RETROGLU_MATERIAL_NAME_SZ_MAX
         ) ) {
            printf( "using material: \"%s\" (%d)\n",
               parser->materials[i].name, i );
            parser->material_idx = i;
            break;
         }
      }
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );

   } else if( RETROGLU_PARSER_STATE_MATERIAL_NAME == parser->state ) {

      printf( "adding material: \"%s\" at idx: %d\n",
         parser->token, parser->materials_sz - 1 );
      strncpy(
         parser->materials[parser->materials_sz - 1].name,
         parser->token,
         RETROGLU_MATERIAL_NAME_SZ_MAX );
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );

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
         RETROGLU_PARSER_STATE_FACE_VERTEX == parser->state ||
         RETROGLU_PARSER_STATE_FACE_TEXTURE == parser->state ||
         RETROGLU_PARSER_STATE_FACE_NORMAL == parser->state
      ) {
         /* End of face. */
         retval = retroglu_parse_token( parser );
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );

         /* Use current parser material. */
         parser->faces[parser->faces_sz].material_idx = parser->material_idx;

         /* Move to next face. */
         parser->faces[parser->faces_sz].vertex_idxs_sz++;
         parser->faces_sz++; /* Newline means this face is done. */
         return retval;

      } else if( RETROGLU_PARSER_STATE_VNORMAL_Z == parser->state ) {

         retval = retroglu_parse_token( parser );
         /* End of vertex. */
         parser->vnormals_sz++;
         return retval;

      } else if( RETROGLU_PARSER_STATE_VERTEX_Z == parser->state ) {

         retval = retroglu_parse_token( parser );
         /* End of vertex. */
         parser->vertices_sz++;
         return retval;
         
      } else {
         return retroglu_parse_token( parser );
      }

   case '\t':
   case ' ':
      /* Whitespace. Inside of a comment or time for a new token. */
      if( RETROGLU_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing on spaces in comments. */
         return RETROFLAT_OK;

      } else if(
         RETROGLU_PARSER_STATE_FACE_VERTEX == parser->state ||
         RETROGLU_PARSER_STATE_FACE_TEXTURE == parser->state ||
         RETROGLU_PARSER_STATE_FACE_NORMAL == parser->state
      ) {
         /* A space means we're moving on to the next vertex! */
         retval = retroglu_parse_token( parser );
         parser->faces[parser->faces_sz].vertex_idxs_sz++;
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_VERTEX );
         return retval;

      } else if( RETROGLU_PARSER_STATE_VNORMAL_Z == parser->state ) {

         retval = retroglu_parse_token( parser );
         /* End of vertex. */
         parser->vnormals_sz++;
         return retval;

      } else if( RETROGLU_PARSER_STATE_VERTEX_Z == parser->state ) {

         retval = retroglu_parse_token( parser );
         /* End of vertex. */
         parser->vertices_sz++;
         return retval;

      } else {
         return retroglu_parse_token( parser );
      }

   case '/':
      if( RETROGLU_PARSER_STATE_FACE_VERTEX == parser->state ) {
         retval = retroglu_parse_token( parser );
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_TEXTURE );
         return retval;

      } else if( RETROGLU_PARSER_STATE_FACE_TEXTURE == parser->state ) {
         retval = retroglu_parse_token( parser );
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_NORMAL );
         return retval;
      }
      
      /* v/vt/vn/??? */
      assert( RETROGLU_PARSER_STATE_FACE_NORMAL != parser->state );

      /* If not part of a face, fall through to default append. */
      return retroglu_append_token( parser, c );

   default:
      return retroglu_append_token( parser, c );
   }

   return RETROFLAT_OK;
}

void retroglu_draw_poly(
   struct RETROGLU_VERTEX* vertices, int vertices_sz,
   struct RETROGLU_VERTEX* vnormals, int vnormals_sz,
   struct RETROGLU_VERTEX* vtextures, int vtextures_sz,
   struct RETROGLU_FACE* faces, int faces_sz,
   struct RETROGLU_MATERIAL* materials, int materials_sz
) {
   int i = 0;
   int j = 0;

   glBegin( GL_TRIANGLES );
   for( i = 0 ; faces_sz > i ; i++ ) {
   
      glMaterialfv( GL_FRONT, GL_DIFFUSE,
         materials[faces[i].material_idx].diffuse );
      glMaterialf( GL_FRONT, GL_SHININESS, 100.0f );
   
      for( j = 0 ; faces[i].vertex_idxs_sz > j ; j++ ) {
         assert( 0 < faces[i].vertex_idxs[j] );
         assert( 3 == faces[i].vertex_idxs_sz );

         glNormal3f(
            vnormals[faces[i].vnormal_idxs[j] - 1].x,
            vnormals[faces[i].vnormal_idxs[j] - 1].y,
            vnormals[faces[i].vnormal_idxs[j] - 1].z );

         glVertex3f(
            vertices[faces[i].vertex_idxs[j] - 1].x,
            vertices[faces[i].vertex_idxs[j] - 1].y,
            vertices[faces[i].vertex_idxs[j] - 1].z );
      }

   }
   glEnd();
}

#endif /* RETROGLU_C */

/*! \} */ /* maug_retroglu */

#endif /* !RETROGLU_H */

