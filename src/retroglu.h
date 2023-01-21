
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
#define RETROGLU_PARSER_STATE_MTL_KA_R 24
#define RETROGLU_PARSER_STATE_MTL_KA_G 25
#define RETROGLU_PARSER_STATE_MTL_KA_B 26
#define RETROGLU_PARSER_STATE_MTL_KS_R 27
#define RETROGLU_PARSER_STATE_MTL_KS_G 28
#define RETROGLU_PARSER_STATE_MTL_KS_B 29
#define RETROGLU_PARSER_STATE_MTL_KE_R 30
#define RETROGLU_PARSER_STATE_MTL_KE_G 31
#define RETROGLU_PARSER_STATE_MTL_KE_B 32
#define RETROGLU_PARSER_STATE_MTL_NS 33

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
   float emissive[4];
   float specular_exp;
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
 * \addtogroup maug_retroglu_sprite RetroGLU Sprites
 * \{
 */

#define RETROGLU_SPRITE_X 0
#define RETROGLU_SPRITE_Y 1

struct RETROGLU_SPRITE {
   float vertices_front[6][2];
   float vtexture_front[6][2];
   float vertices_back[6][2];
   float vtexture_back[6][2];
   uint32_t texture_id;
   int texture_w;
   int texture_h;
   /*! \brief Width of clipped sprite on screen. */
   float screen_clip_wf;
   /*! \brief Height of clipped sprite on screen. */
   float screen_clip_hf;
   float translate_x;
   float translate_y;
   float scale_x;
   float scale_y;
   int rotate_y;
};

/*! \} */ /* maug_retroglu_sprite */

#define retroglu_tex_px_x_to_f( px, sprite ) ((px) * 1.0 / sprite->texture_w)
#define retroglu_tex_px_y_to_f( px, sprite ) ((px) * 1.0 / sprite->texture_h)

#define retroglu_scr_px_x_to_f( px ) \
   (float)(((px) * 1.0 / (retroflat_screen_w() / 2)) - 1.0)
#define retroglu_scr_px_y_to_f( py ) \
   (float)(((py) * 1.0 / (retroflat_screen_h() / 2)) - 1.0)

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
   debug_printf( 0, "changing parser to state: %d\n", new_state ); \
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
   f( "Kd", retroglu_token_kd ) \
   f( "Ka", retroglu_token_ka ) \
   f( "Ks", retroglu_token_ks ) \
   f( "Ke", retroglu_token_ks ) \
   f( "Ns", retroglu_token_ns )

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

int retroglu_load_tex_bmp(
   const uint8_t* bmp_buf, uint32_t bmp_buf_sz, uint32_t* p_texture_id,
   uint32_t* p_bmp_w, uint32_t* p_bmp_h );

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
   /* Set default lighting alpha to non-transparent. */
   parser->materials[parser->materials_sz].ambient[3] = 1.0f;
   parser->materials[parser->materials_sz].diffuse[3] = 1.0f;
   parser->materials[parser->materials_sz].specular[3] = 1.0f;
   parser->materials[parser->materials_sz].emissive[3] = 1.0f;
   parser->materials[parser->materials_sz].specular_exp = 0;
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

int retroglu_token_ka( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_MTL_KA_R );
   return RETROFLAT_OK;
}

int retroglu_token_ks( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_MTL_KS_R );
   return RETROFLAT_OK;
}

int retroglu_token_ke( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_MTL_KE_R );
   return RETROFLAT_OK;
}

int retroglu_token_ns( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_MTL_NS );
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
   f( "mtl Kd B", MTL_KD_B, materials, materials_sz-1, diffuse[2], NONE ) \
   f( "mtl Ka R", MTL_KA_R, materials, materials_sz-1, ambient[0], MTL_KA_G ) \
   f( "mtl Ka G", MTL_KA_G, materials, materials_sz-1, ambient[1], MTL_KA_B ) \
   f( "mtl Ka B", MTL_KA_B, materials, materials_sz-1, ambient[2], NONE ) \
   f( "mtl Ks R", MTL_KS_R, materials, materials_sz-1, specular[0], MTL_KS_G ) \
   f( "mtl Ks G", MTL_KS_G, materials, materials_sz-1, specular[1], MTL_KS_B ) \
   f( "mtl Ks B", MTL_KS_B, materials, materials_sz-1, specular[2], NONE ) \
   f( "mtl Ke R", MTL_KE_R, materials, materials_sz-1, emissive[0], MTL_KE_G ) \
   f( "mtl Ke G", MTL_KE_G, materials, materials_sz-1, emissive[1], MTL_KE_B ) \
   f( "mtl Ke B", MTL_KE_B, materials, materials_sz-1, emissive[2], NONE ) \
   f( "mtl Ns", MTL_NS, materials, materials_sz-1, specular_exp, NONE )

#define RETROGLU_TOKEN_PARSE_VF( desc, cond, array, sz, val, state_next ) \
   } else if( RETROGLU_PARSER_STATE_ ## cond == parser->state ) { \
      parser->array[parser->sz].val = strtod( parser->token, NULL ); \
      debug_printf( 0, "vertex %d " desc ": %f\n", \
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

   debug_printf( 0, "token: %s\n", parser->token );

   if( RETROGLU_PARSER_STATE_MATERIAL_LIB == parser->state ) {

      debug_printf( 0, "parsing material lib: %s\n", parser->token );
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );
      assert( NULL != parser->load_mtl );
      return parser->load_mtl( parser->token, parser, parser->load_mtl_data );

   RETROGLU_TOKENS_VF( RETROGLU_TOKEN_PARSE_VF )

      /* TODO: Handle W. */

   } else if( RETROGLU_PARSER_STATE_FACE_VERTEX == parser->state ) {
      /* Parsing face vertex index. */
      parser->faces[parser->faces_sz].vertex_idxs[
         parser->faces[parser->faces_sz].vertex_idxs_sz] =
            atoi( parser->token );

      debug_printf( 0, "face %d, vertex %d: %d\n",
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

      debug_printf( 0, "face %d, normal %d: %d\n",
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

      debug_printf( 0, "face %d, texture %d: %d\n",
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
         debug_printf(
            0, "%s vs %s\n", parser->materials[i].name, parser->token );
         if( 0 == strncmp(
            parser->materials[i].name, parser->token,
            RETROGLU_MATERIAL_NAME_SZ_MAX
         ) ) {
            debug_printf( 0, "using material: \"%s\" (%d)\n",
               parser->materials[i].name, i );
            parser->material_idx = i;
            break;
         }
      }
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );

   } else if( RETROGLU_PARSER_STATE_MATERIAL_NAME == parser->state ) {

      debug_printf( 0, "adding material: \"%s\" at idx: %d\n",
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
      debug_printf( 0, "token out of bounds!\n" );
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

      } else if( RETROGLU_PARSER_STATE_MTL_KD_B == parser->state ) {
         retval = retroglu_parse_token( parser );
         /* This tuple has a space after blue, so maybe alpha? */
         /* TODO: Set alpha state. */
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

int retroglu_load_tex_bmp(
   const uint8_t* bmp_buf, uint32_t bmp_buf_sz, uint32_t* p_texture_id,
   uint32_t* p_bmp_w, uint32_t* p_bmp_h
) {
   uint32_t bmp_offset = 0;
   uint32_t bmp_bpp = 0;
   uint32_t bmp_px_sz = 0;
   uint8_t* bmp_px = NULL;
   int16_t i = 0;

   /* Offsets hardcoded based on windows bitmap. */
   /* TODO: More flexibility. */

   if( 40 != bmp_read_uint32( &(bmp_buf[0x0e]) ) ) {
      error_printf( "unable to determine texture bitmap format" );
      return RETROFLAT_ERROR_BITMAP;
   }

   bmp_offset = bmp_read_uint32( &(bmp_buf[0x0a]) );
   bmp_px_sz = bmp_buf_sz - bmp_offset;
   *p_bmp_w = bmp_read_uint32( &(bmp_buf[0x12]) );
   *p_bmp_h = bmp_read_uint32( &(bmp_buf[0x16]) );
   bmp_bpp = bmp_read_uint32( &(bmp_buf[0x1c]) );
   assert( 24 == bmp_bpp );

   debug_printf( 1,
      "bitmap " UPRINTF_U32 " x " UPRINTF_U32 " x " UPRINTF_U32
      " starting at " UPRINTF_U32 " bytes",
      *p_bmp_w, *p_bmp_h, bmp_bpp, bmp_offset );

   /* Allocate temporary buffer for unpacking. */
   bmp_px = calloc( *p_bmp_w * *p_bmp_h, 4 );
   assert( NULL != bmp_px );

   /* Unpack bitmap BGR into BGRA with color key. */
   for( i = 0 ; bmp_px_sz / 3 > i ; i++ ) {
      bmp_px[i * 4] = bmp_buf[bmp_offset + (i * 3) + 2];
      bmp_px[(i * 4) + 1] = bmp_buf[bmp_offset + (i * 3) + 1];
      bmp_px[(i * 4) + 2] = bmp_buf[bmp_offset + (i * 3)];
      if(
         RETROFLAT_TXP_R == bmp_buf[bmp_offset + (i * 3) + 2] &&
         RETROFLAT_TXP_G == bmp_buf[bmp_offset + (i * 3) + 1] &&
         RETROFLAT_TXP_B == bmp_buf[bmp_offset + (i * 3)]
      ) {
         /* Transparent pixel found. */
         bmp_px[(i * 4) + 3] = 0x00;
      } else {
         bmp_px[(i * 4) + 3] = 0xff;
      }
   }

   glGenTextures( 1, p_texture_id );
   glBindTexture( GL_TEXTURE_2D, *p_texture_id );
   /* glPixelStorei( GL_UNPACK_ALIGNMENT, 4 ); */
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, *p_bmp_w, *p_bmp_h, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, bmp_px ); 

   free( bmp_px );

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
         /*
      glMaterialfv( GL_FRONT, GL_AMBIENT,
         materials[faces[i].material_idx].ambient );
         */
      glMaterialfv( GL_FRONT, GL_SPECULAR,
         materials[faces[i].material_idx].specular );
      glMaterialfv( GL_FRONT, GL_EMISSION,
         materials[faces[i].material_idx].emissive );
      glMaterialf( GL_FRONT, GL_SHININESS, 
         materials[faces[i].material_idx].specular_exp );
   
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

int16_t retroglu_set_sprite_tex(
   struct RETROGLU_SPRITE* sprite, uint8_t* bmp_buf, uint32_t bmp_buf_sz
) {
   uint32_t bmp_w = 0,
      bmp_h = 0;
   uint32_t texture_id = 0;
   int16_t retval = RETROFLAT_OK;

   retval = retroglu_load_tex_bmp(
      bmp_buf, bmp_buf_sz, &texture_id, &bmp_w, &bmp_h );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

   /* Set sprite properties based on texture. */
   sprite->texture_id = texture_id;
   sprite->texture_w = bmp_w;
   sprite->texture_h = bmp_h;
   sprite->scale_x = 1.0f;
   sprite->scale_y = 1.0f;

cleanup:

   return retval;
}

void retroglu_set_sprite_clip(
   struct RETROGLU_SPRITE* sprite,
   uint32_t front_px, uint32_t front_py, uint32_t back_px, uint32_t back_py,
   uint32_t pw, uint32_t ph
) {
   /* Set vertices in terms of half the clip size so that rotation is around
    * the midpoint of the sprite, not the side!
    */
   float clip_half_x = 0,
      clip_half_y = 0;
   float clip_tex_fx = 0, /* Front tex X */
      clip_tex_fy = 0, /* Front tex Y */
      clip_tex_bx = 0, /* Back tex X */
      clip_tex_by = 0, /* Back tex Y */
      clip_tex_w = 0,
      clip_tex_h = 0;

   sprite->screen_clip_wf = (pw) * 1.0 / retroflat_screen_w();
   sprite->screen_clip_hf = (ph) * 1.0 / retroflat_screen_h();
   clip_half_x = sprite->screen_clip_wf / 2;
   clip_half_y = sprite->screen_clip_hf / 2;

   /* Setup texture spritesheet. */

   clip_tex_fx = retroglu_tex_px_x_to_f( front_px, sprite ); 
   clip_tex_fy = retroglu_tex_px_y_to_f( front_py, sprite ); 
   clip_tex_bx = retroglu_tex_px_x_to_f( back_px, sprite ); 
   clip_tex_by = retroglu_tex_px_y_to_f( back_py, sprite ); 
   clip_tex_w = retroglu_tex_px_x_to_f( pw, sprite );
   clip_tex_h = retroglu_tex_px_y_to_f( ph, sprite );

   /* == Front Face Textures == */

   /* Lower Left */
   sprite->vtexture_front[0][RETROGLU_SPRITE_X] = clip_tex_fx;
   sprite->vtexture_front[0][RETROGLU_SPRITE_Y] = clip_tex_fy;

   /* Lower Right */
   sprite->vtexture_front[1][RETROGLU_SPRITE_X] = clip_tex_fx + clip_tex_w;
   sprite->vtexture_front[1][RETROGLU_SPRITE_Y] = clip_tex_fy;

   /* Upper Right */
   sprite->vtexture_front[2][RETROGLU_SPRITE_X] = clip_tex_fx + clip_tex_w;
   sprite->vtexture_front[2][RETROGLU_SPRITE_Y] = clip_tex_fy + clip_tex_h;

   /* Upper Right */
   sprite->vtexture_front[3][RETROGLU_SPRITE_X] = clip_tex_fx + clip_tex_w;
   sprite->vtexture_front[3][RETROGLU_SPRITE_Y] = clip_tex_fy + clip_tex_h;

   /* Upper Left */
   sprite->vtexture_front[4][RETROGLU_SPRITE_X] = clip_tex_fx;
   sprite->vtexture_front[4][RETROGLU_SPRITE_Y] = clip_tex_fy + clip_tex_h;

   /* Lower Left */
   sprite->vtexture_front[5][RETROGLU_SPRITE_X] = clip_tex_fx;
   sprite->vtexture_front[5][RETROGLU_SPRITE_Y] = clip_tex_fy;

   /* == Back face Textures == */

   /* Lower Left */
   sprite->vtexture_back[0][RETROGLU_SPRITE_X] = clip_tex_bx;
   sprite->vtexture_back[0][RETROGLU_SPRITE_Y] = clip_tex_by;

   /* Lower Right */
   sprite->vtexture_back[1][RETROGLU_SPRITE_X] = clip_tex_bx + clip_tex_w;
   sprite->vtexture_back[1][RETROGLU_SPRITE_Y] = clip_tex_by;

   /* Upper Right */
   sprite->vtexture_back[2][RETROGLU_SPRITE_X] = clip_tex_bx + clip_tex_w;
   sprite->vtexture_back[2][RETROGLU_SPRITE_Y] = clip_tex_by + clip_tex_h;

   /* Upper Right */
   sprite->vtexture_back[3][RETROGLU_SPRITE_X] = clip_tex_bx + clip_tex_w;
   sprite->vtexture_back[3][RETROGLU_SPRITE_Y] = clip_tex_by + clip_tex_h;

   /* Upper Left */
   sprite->vtexture_back[4][RETROGLU_SPRITE_X] = clip_tex_bx;
   sprite->vtexture_back[4][RETROGLU_SPRITE_Y] = clip_tex_by + clip_tex_h;

   /* Lower Left */
   sprite->vtexture_back[5][RETROGLU_SPRITE_X] = clip_tex_bx;
   sprite->vtexture_back[5][RETROGLU_SPRITE_Y] = clip_tex_by;

   /* == Front Face Vertices == */

   /* Lower-Left */
   sprite->vertices_front[0][RETROGLU_SPRITE_X] = -1 * clip_half_x;
   sprite->vertices_front[0][RETROGLU_SPRITE_Y] = -1 * clip_half_y;
   
   /* Lower-Right */
   sprite->vertices_front[1][RETROGLU_SPRITE_X] = clip_half_x;
   sprite->vertices_front[1][RETROGLU_SPRITE_Y] = -1 * clip_half_y;
   
   /* Upper-Right */
   sprite->vertices_front[2][RETROGLU_SPRITE_X] = clip_half_x;
   sprite->vertices_front[2][RETROGLU_SPRITE_Y] = clip_half_y;

   /* Upper-Right */
   sprite->vertices_front[3][RETROGLU_SPRITE_X] = clip_half_x;
   sprite->vertices_front[3][RETROGLU_SPRITE_Y] = clip_half_y;

   /* Upper-Left */
   sprite->vertices_front[4][RETROGLU_SPRITE_X] = -1 * clip_half_x;
   sprite->vertices_front[4][RETROGLU_SPRITE_Y] = clip_half_y;

   /* Lower-Left */
   sprite->vertices_front[5][RETROGLU_SPRITE_X] = -1 * clip_half_x;
   sprite->vertices_front[5][RETROGLU_SPRITE_Y] = -1 * clip_half_y;

   /* == Back Face Vertices == */

   /* Lower-Right */
   sprite->vertices_back[0][RETROGLU_SPRITE_X] = clip_half_x;
   sprite->vertices_back[0][RETROGLU_SPRITE_Y] = -1 * clip_half_y;

   /* Lower-Left */
   sprite->vertices_back[1][RETROGLU_SPRITE_X] = -1 * clip_half_x;
   sprite->vertices_back[1][RETROGLU_SPRITE_Y] = -1 * clip_half_y;

   /* Upper-Left */
   sprite->vertices_back[2][RETROGLU_SPRITE_X] = -1 * clip_half_x;
   sprite->vertices_back[2][RETROGLU_SPRITE_Y] = clip_half_y;

   /* Upper-Left */
   sprite->vertices_back[3][RETROGLU_SPRITE_X] = -1 * clip_half_x;
   sprite->vertices_back[3][RETROGLU_SPRITE_Y] = clip_half_y;

   /* Upper-Right */
   sprite->vertices_back[4][RETROGLU_SPRITE_X] = clip_half_x;
   sprite->vertices_back[4][RETROGLU_SPRITE_Y] = clip_half_y;

   /* Lower-Right */
   sprite->vertices_back[5][RETROGLU_SPRITE_X] = clip_half_x;
   sprite->vertices_back[5][RETROGLU_SPRITE_Y] = -1 * clip_half_y;
}

void retroglu_set_sprite_pos(
   struct RETROGLU_SPRITE* sprite, uint32_t px, uint32_t py
) {
   sprite->translate_x = retroglu_scr_px_x_to_f( px );
   sprite->translate_y = retroglu_scr_px_y_to_f( py );
}

void retroglu_draw_sprite( struct RETROGLU_SPRITE* sprite ) {
   int i = 0;
   
   glBindTexture( GL_TEXTURE_2D, sprite->texture_id );
   glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

   /* Save the current (presumably unmodified model view matrix. */
   glPushMatrix();

   /* Set the matrix to translate/rotate/scale based on sprite props. */
   glTranslatef( sprite->translate_x, sprite->translate_y, 0 );
   glScalef( sprite->scale_x, sprite->scale_y, 1.0f );
   glRotatef( sprite->rotate_y, 0.0f, 1.0f, 0.0f );

   glBegin( GL_TRIANGLES );

   for( i = 0 ; 6 > i ; i++ ) {
      glTexCoord2fv( sprite->vtexture_front[i] );
      glVertex2fv( sprite->vertices_front[i] );
   }

   for( i = 0 ; 6 > i ; i++ ) {
      glTexCoord2fv( sprite->vtexture_back[i] );
      glVertex2fv( sprite->vertices_back[i] );
   }

   glEnd();

   glPopMatrix();
}

#endif /* RETROGLU_C */

/*! \} */ /* maug_retroglu */

#endif /* !RETROGLU_H */

