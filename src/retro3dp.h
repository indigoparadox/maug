
#ifndef RETRO3DP_H
#define RETRO3DP_H

/**
 * \addtogroup maug_retro3dp_obj_fsm RetroGLU OBJ Parser
 * \{
 */

#ifndef RETRO3DP_TRACE_LVL
#  define RETRO3DP_TRACE_LVL 0
#endif /* !RETRO3DP_TRACE_LVL */

#ifndef RETRO3DP_PARSER_TOKEN_SZ_MAX
#  define RETRO3DP_PARSER_TOKEN_SZ_MAX 32
#endif /* !RETRO3DP_PARSER_TOKEN_SZ_MAX */

#define RETRO3DP_PARSER_ERROR -1

/**
 * \addtogroup maug_retro3dp_obj_fsm_states RetroGLU OBJ Parser States
 * \{
 */

#define RETRO3DP_PARSER_STATE_NONE 0
#define RETRO3DP_PARSER_STATE_VERTEX_X 1
#define RETRO3DP_PARSER_STATE_VERTEX_Y 2
#define RETRO3DP_PARSER_STATE_VERTEX_Z 3
#define RETRO3DP_PARSER_STATE_COMMENT 4
#define RETRO3DP_PARSER_STATE_FACE_TOKEN 5
#define RETRO3DP_PARSER_STATE_FACE_VERTEX 6
#define RETRO3DP_PARSER_STATE_FACE_TEXTURE 7
#define RETRO3DP_PARSER_STATE_FACE_NORMAL 8
#define RETRO3DP_PARSER_STATE_FACE_MATERIAL 9
#define RETRO3DP_PARSER_STATE_MATERIAL_NAME 10
#define RETRO3DP_PARSER_STATE_MATERIAL_AMB 11
#define RETRO3DP_PARSER_STATE_MATERIAL_DIF 12
#define RETRO3DP_PARSER_STATE_MATERIAL_SPEC 13
#define RETRO3DP_PARSER_STATE_MATERIAL_LIB 14
#define RETRO3DP_PARSER_STATE_MTL_KD_R 15
#define RETRO3DP_PARSER_STATE_MTL_KD_G 16
#define RETRO3DP_PARSER_STATE_MTL_KD_B 17
#define RETRO3DP_PARSER_STATE_VNORMAL_X 18
#define RETRO3DP_PARSER_STATE_VNORMAL_Y 19
#define RETRO3DP_PARSER_STATE_VNORMAL_Z 20
#define RETRO3DP_PARSER_STATE_VTEXTURE_X 21
#define RETRO3DP_PARSER_STATE_VTEXTURE_Y 22
#define RETRO3DP_PARSER_STATE_VTEXTURE_Z 23
#define RETRO3DP_PARSER_STATE_MTL_KA_R 24
#define RETRO3DP_PARSER_STATE_MTL_KA_G 25
#define RETRO3DP_PARSER_STATE_MTL_KA_B 26
#define RETRO3DP_PARSER_STATE_MTL_KS_R 27
#define RETRO3DP_PARSER_STATE_MTL_KS_G 28
#define RETRO3DP_PARSER_STATE_MTL_KS_B 29
#define RETRO3DP_PARSER_STATE_MTL_KE_R 30
#define RETRO3DP_PARSER_STATE_MTL_KE_G 31
#define RETRO3DP_PARSER_STATE_MTL_KE_B 32
#define RETRO3DP_PARSER_STATE_MTL_NS 33

/*! \} */ /* maug_retro3dp_obj_fsm_states */

#ifndef RETRO3DP_FACE_VERTICES_SZ_MAX
#  define RETRO3DP_FACE_VERTICES_SZ_MAX 3
#endif /* !RETRO3DP_FACE_VERTICES_SZ_MAX */

#ifndef RETRO3DP_MATERIAL_NAME_SZ_MAX
#  define RETRO3DP_MATERIAL_NAME_SZ_MAX 32
#endif /* !RETRO3DP_MATERIAL_NAME_SZ_MAX */

#ifndef RETRO3DP_MATERIAL_LIB_SZ_MAX
#  define RETRO3DP_MATERIAL_LIB_SZ_MAX 32
#endif /* !RETRO3DP_MATERIAL_LIB_SZ_MAX */

struct RETRO3DP_MATERIAL {
   float ambient[4];
   float diffuse[4];
   float specular[4];
   float emissive[4];
   float specular_exp;
   char name[RETRO3DP_MATERIAL_NAME_SZ_MAX];
};

struct RETRO3DP_FACE {
   /**
    * \brief List of vertex indices from the associated
    *        RETRO3DP_PARSER::vertices.
    *
    * The size of this array is fixed to simplify allocation of arrays.
    */
   uint16_t vertex_idxs[RETRO3DP_FACE_VERTICES_SZ_MAX];
   uint16_t vnormal_idxs[RETRO3DP_FACE_VERTICES_SZ_MAX];
   uint16_t vtexture_idxs[RETRO3DP_FACE_VERTICES_SZ_MAX];
   uint16_t vertex_idxs_sz;
   uint16_t material_idx;
};

struct RETRO3DP_VERTEX {
   float x;
   float y;
   float z;
};

struct RETRO3DP_VTEXTURE {
   float u;
   float v;
};

#define RETRO3DP_VERTICES_SZ_MAX 1024
#define RETRO3DP_FACES_SZ_MAX 1024
#define RETRO3DP_MATERIALS_SZ_MAX 1024

/* TODO: Switch these to mdata_vector structs. */
struct RETRO3DP_OBJ {
   uint8_t flags;
   struct MDATA_VECTOR vertices;
   struct MDATA_VECTOR vnormals;
   struct MDATA_VECTOR vtextures;
   /**
    * \brief List of faces from an OBJ file. Faces comprise a list of polygons
    *        denoted by index of the vertices in RETRO3DP_PARSER::vertices.
    */
   struct MDATA_VECTOR faces;
   struct MDATA_VECTOR materials;
};

#define retro3dp_append_token( parser, c ) \
   mparser_append_token( "retro3dp", &((parser)->base), c )

#ifdef MPARSER_TRACE_NAMES
#  define retro3dp_pstate_push( parser, new_pstate ) \
      if( 0 < (parser)->base.pstate_sz ) { \
         mparser_pstate_pop( "retro3dp", &((parser)->base), NULL ); \
      } \
      mparser_pstate_push( "retro3dp", &((parser)->base), new_pstate, NULL );
#else
#  define retro3dp_pstate_push( parser, new_pstate ) \
      if( 0 < (parser)->base.pstate_sz ) { \
         mparser_pstate_pop( "retro3dp", &((parser)->base) ); \
      } \
      mparser_pstate_push( "retro3dp", &((parser)->base), new_pstate );
#endif /* MPARSER_TRACE_NAMES */

#define retro3dp_pstate( parser ) \
   mparser_pstate( &((parser)->base) )

#define retro3dp_reset_token( parser ) \
   mparser_reset_token( "retro3dp", &((parser)->base) )

/**
 * \brief Table of OBJ file tokens understood by the parser.
 */
#define RETRO3DP_OBJ_TOKENS( f ) \
   f( "v", retro3dp_token_vertice ) \
   f( "vn", retro3dp_token_vnormal ) \
   f( "f", retro3dp_token_face ) \
   f( "usemtl", retro3dp_token_usemtl ) \
   f( "newmtl", retro3dp_token_newmtl ) \
   f( "mtllib", retro3dp_token_mtllib ) \
   f( "Kd", retro3dp_token_kd ) \
   f( "Ka", retro3dp_token_ka ) \
   f( "Ks", retro3dp_token_ks ) \
   f( "Ke", retro3dp_token_ks ) \
   f( "Ns", retro3dp_token_ns )

struct RETRO3DP_PARSER;

/**
 * \brief Callback to execute when its associate in ::RETRO3DP_OBJ_TOKENS is
 *        found in an OBJ file.
 */
typedef int (*retro3dp_mtl_cb)(
   const char* filename, struct RETRO3DP_PARSER* parser, void* data );

/**
 * \brief As retro3dp_parse_obj_c() parses OBJ data, it populates this struct
 *        with object information.
 */
struct RETRO3DP_PARSER {
   struct MPARSER base;
   struct RETRO3DP_OBJ* obj;
   int material_idx;
   retro3dp_mtl_cb load_mtl;
   void* load_mtl_data;
};

typedef int (*retro3dp_token_cb)( struct RETRO3DP_PARSER* parser );

/**
 * \related RETRO3DP_PARSER
 * \brief Initialize a ::RETRO3DP_PARSER.
 * \warning This should be run before retro3dp_parse_obj_c() is called on the
 *          parser!
 */
void retro3dp_parse_init(
   struct RETRO3DP_PARSER* parser, struct RETRO3DP_OBJ* obj,
   retro3dp_mtl_cb load_mtl, void* load_mtl_data
);

/**
 * \related RETRO3DP_PARSER
 * \brief Parse OBJ data into a parser, one character at a time.
 *
 * Generally, this should loop over a character array loaded from an OBJ file.
 */
MERROR_RETVAL
retro3dp_parse_obj_c( struct RETRO3DP_PARSER* parser, unsigned char c );

MERROR_RETVAL retro3dp_parse_obj_file(
   const char* filename, struct RETRO3DP_PARSER* parser,
   struct RETRO3DP_OBJ* obj );

void retro3dp_destroy_obj( struct RETRO3DP_OBJ* obj );

/*! \} */ /* maug_retro3dp_obj_fsm */

#ifdef RETRO3DP_C

int retro3dp_token_vertice( struct RETRO3DP_PARSER* parser ) {
   ssize_t append_retval = 0;
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_VERTEX_X );
   append_retval = mdata_vector_append(
      &(parser->obj->vertices), NULL, sizeof( struct RETRO3DP_VERTEX ) );
   return 0 <= append_retval ? RETROFLAT_OK : mdata_retval( append_retval );
}

int retro3dp_token_vnormal( struct RETRO3DP_PARSER* parser ) {
   ssize_t append_retval = 0;
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_VNORMAL_X );
   append_retval = mdata_vector_append(
      &(parser->obj->vnormals), NULL, sizeof( struct RETRO3DP_VERTEX ) );
   return 0 <= append_retval ? RETROFLAT_OK : mdata_retval( append_retval );
}

int retro3dp_token_face( struct RETRO3DP_PARSER* parser ) {
   ssize_t append_retval = 0;
   struct RETRO3DP_FACE f;
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_FACE_VERTEX );
   f.vertex_idxs_sz = 0;
   append_retval = mdata_vector_append(
      &(parser->obj->faces), &f, sizeof( struct RETRO3DP_FACE ) );
   return 0 <= append_retval ? RETROFLAT_OK : mdata_retval( append_retval );
}

int retro3dp_token_usemtl( struct RETRO3DP_PARSER* parser ) {
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_FACE_MATERIAL );
   return RETROFLAT_OK;
}

int retro3dp_token_newmtl( struct RETRO3DP_PARSER* parser ) {
   ssize_t append_retval = 0;
   struct RETRO3DP_MATERIAL m;
   /* Set default lighting alpha to non-transparent. */
   m.ambient[3] = 1.0f;
   m.diffuse[3] = 1.0f;
   m.specular[3] = 1.0f;
   m.emissive[3] = 1.0f;
   m.specular_exp = 0;
   append_retval = mdata_vector_append(
      &(parser->obj->materials), &m, sizeof( struct RETRO3DP_MATERIAL ) );
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_MATERIAL_NAME );
   return 0 <= append_retval ? RETROFLAT_OK : mdata_retval( append_retval );
}

int retro3dp_token_mtllib( struct RETRO3DP_PARSER* parser ) {
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_MATERIAL_LIB );
   return RETROFLAT_OK;
}

int retro3dp_token_kd( struct RETRO3DP_PARSER* parser ) {
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_MTL_KD_R );
   return RETROFLAT_OK;
}

int retro3dp_token_ka( struct RETRO3DP_PARSER* parser ) {
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_MTL_KA_R );
   return RETROFLAT_OK;
}

int retro3dp_token_ks( struct RETRO3DP_PARSER* parser ) {
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_MTL_KS_R );
   return RETROFLAT_OK;
}

int retro3dp_token_ke( struct RETRO3DP_PARSER* parser ) {
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_MTL_KE_R );
   return RETROFLAT_OK;
}

int retro3dp_token_ns( struct RETRO3DP_PARSER* parser ) {
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_MTL_NS );
   return RETROFLAT_OK;
}

#  define RETRO3DP_OBJ_TOKEN_STRINGS( token, callback ) token,

char* g_retro3dp_token_strings[] = {
   RETRO3DP_OBJ_TOKENS( RETRO3DP_OBJ_TOKEN_STRINGS )  
   ""
};

#  define RETRO3DP_OBJ_TOKEN_CALLBACKS( token, callback ) callback,

retro3dp_token_cb g_retro3dp_token_callbacks[] = {
   RETRO3DP_OBJ_TOKENS( RETRO3DP_OBJ_TOKEN_CALLBACKS )  
   NULL
};

void retro3dp_parse_init(
   struct RETRO3DP_PARSER* parser, struct RETRO3DP_OBJ* obj,
   retro3dp_mtl_cb load_mtl, void* load_mtl_data
) {
   parser->load_mtl = load_mtl;
   parser->load_mtl_data = load_mtl_data;
   parser->obj = obj;
   assert( NULL != parser->obj );
   retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_NONE );
   retro3dp_reset_token( parser );
}

/* TODO: Get rid of sz_diff. */
#define RETRO3DP_TOKENS_VF( f ) \
   f( "X",        VERTEX_X,   VERTEX,   vertices, 0, x, VERTEX_Y ) \
   f( "Y",        VERTEX_Y,   VERTEX,   vertices, 0, y, VERTEX_Z ) \
   f( "Z",        VERTEX_Z,   VERTEX,   vertices, 0, z, NONE ) \
   f( "normal X", VNORMAL_X,  VERTEX,   vnormals, 0, x, VNORMAL_Y ) \
   f( "normal Y", VNORMAL_Y,  VERTEX,   vnormals, 0, y, VNORMAL_Z ) \
   f( "normal Z", VNORMAL_Z,  VERTEX,   vnormals, 0, z, NONE ) \
   f( "mtl Kd R", MTL_KD_R,   MATERIAL, materials, 1, diffuse[0], MTL_KD_G ) \
   f( "mtl Kd G", MTL_KD_G,   MATERIAL, materials, 1, diffuse[1], MTL_KD_B ) \
   f( "mtl Kd B", MTL_KD_B,   MATERIAL, materials, 1, diffuse[2], NONE ) \
   f( "mtl Ka R", MTL_KA_R,   MATERIAL, materials, 1, ambient[0], MTL_KA_G ) \
   f( "mtl Ka G", MTL_KA_G,   MATERIAL, materials, 1, ambient[1], MTL_KA_B ) \
   f( "mtl Ka B", MTL_KA_B,   MATERIAL, materials, 1, ambient[2], NONE ) \
   f( "mtl Ks R", MTL_KS_R,   MATERIAL, materials, 1, specular[0], MTL_KS_G ) \
   f( "mtl Ks G", MTL_KS_G,   MATERIAL, materials, 1, specular[1], MTL_KS_B ) \
   f( "mtl Ks B", MTL_KS_B,   MATERIAL, materials, 1, specular[2], NONE ) \
   f( "mtl Ke R", MTL_KE_R,   MATERIAL, materials, 1, emissive[0], MTL_KE_G ) \
   f( "mtl Ke G", MTL_KE_G,   MATERIAL, materials, 1, emissive[1], MTL_KE_B ) \
   f( "mtl Ke B", MTL_KE_B,   MATERIAL, materials, 1, emissive[2], NONE ) \
   f( "mtl Ns",   MTL_NS,     MATERIAL, materials, 1, specular_exp, NONE )

#define RETRO3DP_TOKEN_PARSE_VF( desc, cond, vt, array, sz_diff, val, next ) \
   } else if( RETRO3DP_PARSER_STATE_ ## cond == retro3dp_pstate( parser ) ) { \
      idx = mdata_vector_ct( &(parser->obj->array) ) - 1 /* - (sz_diff) */; \
      mdata_vector_lock( &(parser->obj->array) ); \
      /* TODO: Maug replacement for C99 crutch. */ \
      vt = mdata_vector_get( \
         &(parser->obj->array), idx, struct RETRO3DP_ ## vt ); \
      assert( NULL != vt ); \
      vt->val = strtod( parser->base.token, NULL ); \
      debug_printf( RETRO3DP_TRACE_LVL, #vt " " SSIZE_T_FMT " " desc ": %f", \
         idx, vt->val ); \
      mdata_vector_unlock( &(parser->obj->array) ); \
      retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_ ## next );

MERROR_RETVAL
retro3dp_parse_obj_token( struct RETRO3DP_PARSER* parser ) {
   int i = 0;
   MERROR_RETVAL retval = RETROFLAT_OK;
   struct RETRO3DP_FACE* face = NULL;
   ssize_t idx = 0;
   /* These are in all-caps to match the macro above and save some typing. */
   struct RETRO3DP_VERTEX* VERTEX = NULL;
   struct RETRO3DP_MATERIAL* MATERIAL = NULL;

   if( 0 == parser->base.token_sz ) {
      /* Empty token. */
      goto cleanup;
   }

   /* NULL-terminate token. */
   /* parser->base.token[parser->token_sz] = '\0'; */

   debug_printf( RETRO3DP_TRACE_LVL, "token: %s", parser->base.token );

   if( RETRO3DP_PARSER_STATE_MATERIAL_LIB == retro3dp_pstate( parser ) ) {

      debug_printf(
         RETRO3DP_TRACE_LVL, "parsing material lib: %s", parser->base.token );
      retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_NONE );
      assert( NULL != parser->load_mtl );
      return parser->load_mtl(
         parser->base.token, parser, parser->load_mtl_data );

   RETRO3DP_TOKENS_VF( RETRO3DP_TOKEN_PARSE_VF )

      /* TODO: Handle W. */

   } else if( RETRO3DP_PARSER_STATE_FACE_VERTEX == retro3dp_pstate( parser ) ) {
      /* Parsing face vertex index. */

      mdata_vector_lock( &(parser->obj->faces) );
      face = mdata_vector_get_last(
         &(parser->obj->faces), struct RETRO3DP_FACE );
      assert( NULL != face );
      face->vertex_idxs[face->vertex_idxs_sz] =
         maug_atos32( parser->base.token, parser->base.token_sz );
      mdata_vector_unlock( &(parser->obj->faces) );

      /*
      debug_printf( RETRO3DP_TRACE_LVL, "face %d, vertex %d: %d",
         parser->obj->faces_sz, parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz,
         parser->obj->faces[parser->obj->faces_sz].vertex_idxs[
            parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz] );
      */

      /* The new state is set in the parser below, as it could become
       * RETRO3DP_PARSER_STATE_FACE_NORMAL or RETRO3DP_PARSER_STATE_NONE,
       * depending on whether it's a whitespace/newline/slash after.
       * Same for index incr.
       */

   } else if( RETRO3DP_PARSER_STATE_FACE_NORMAL == retro3dp_pstate( parser ) ) {

      /* Parsing face normal index. */
      mdata_vector_lock( &(parser->obj->faces) );
      face = mdata_vector_get_last(
         &(parser->obj->faces), struct RETRO3DP_FACE );
      assert( NULL != face );
      face->vnormal_idxs[face->vertex_idxs_sz] =
         maug_atos32( parser->base.token, parser->base.token_sz );
      mdata_vector_unlock( &(parser->obj->faces) );

      /*
      debug_printf( RETRO3DP_TRACE_LVL, "face %d, normal %d: %d",
         parser->obj->faces_sz, parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz,
         parser->obj->faces[parser->obj->faces_sz].vnormal_idxs[
            parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz] );
      */

      /* The new state is set in the parser below, as it could become
       * RETRO3DP_PARSER_STATE_FACE_NORMAL or RETRO3DP_PARSER_STATE_NONE,
       * depending on whether it's a whitespace/newline/slash after.
       * Same for index incr.
       */

   } else if(
      RETRO3DP_PARSER_STATE_FACE_TEXTURE == retro3dp_pstate( parser )
   ) {
      /* Parsing face texture index. */
      mdata_vector_lock( &(parser->obj->faces) );
      face = mdata_vector_get_last(
         &(parser->obj->faces), struct RETRO3DP_FACE );
      assert( NULL != face );
      face->vtexture_idxs[face->vertex_idxs_sz] =
         maug_atos32( parser->base.token, parser->base.token_sz );
      mdata_vector_unlock( &(parser->obj->faces) );

      /*
      debug_printf( RETRO3DP_TRACE_LVL, "face %d, texture %d: %d",
         parser->obj->faces_sz, parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz,
         parser->obj->faces[parser->obj->faces_sz].vtexture_idxs[
            parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz] );
      */

      /* The new state is set in the parser below, as it could become
       * RETRO3DP_PARSER_STATE_FACE_NORMAL or RETRO3DP_PARSER_STATE_NONE,
       * depending on whether it's a whitespace/newline/slash after.
       * Same for index incr.
       */

 
   } else if(
      RETRO3DP_PARSER_STATE_FACE_MATERIAL == retro3dp_pstate( parser )
   ) {

      /* Find the material index and assign it to the parser. */
      mdata_vector_lock( &(parser->obj->materials) );
      for( i = 0 ; mdata_vector_ct( &(parser->obj->materials) ) > i ; i++ ) {
         MATERIAL = mdata_vector_get(
            &(parser->obj->materials), i, struct RETRO3DP_MATERIAL );
         assert( NULL != MATERIAL );
         debug_printf( RETRO3DP_TRACE_LVL,
            "%s vs %s", MATERIAL->name, parser->base.token );
         if( 0 == strncmp(
            MATERIAL->name, parser->base.token, RETRO3DP_MATERIAL_NAME_SZ_MAX
         ) ) {
            debug_printf( RETRO3DP_TRACE_LVL, "using material: \"%s\" (%d)",
               MATERIAL->name, i );
            parser->material_idx = i;
            break;
         }
      }
      mdata_vector_unlock( &(parser->obj->materials) );
      retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_NONE );

   } else if(
      RETRO3DP_PARSER_STATE_MATERIAL_NAME == retro3dp_pstate( parser )
   ) {

      mdata_vector_lock( &(parser->obj->materials) );
      debug_printf(
         RETRO3DP_TRACE_LVL, "adding material: \"%s\" at idx: " SIZE_T_FMT,
         parser->base.token, mdata_vector_ct( &(parser->obj->materials) ) - 1 );
      MATERIAL = mdata_vector_get_last(
         &(parser->obj->materials), struct RETRO3DP_MATERIAL );
      assert( NULL != MATERIAL );
      maug_strncpy(
         MATERIAL->name, parser->base.token, RETRO3DP_MATERIAL_NAME_SZ_MAX );
      mdata_vector_unlock( &(parser->obj->materials) );
      retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_NONE );

   } else {
      /* Check against generic tokens. */
      while( '\0' != g_retro3dp_token_strings[i][0] ) {
         if(
            0 == strncmp(
               parser->base.token, g_retro3dp_token_strings[i],
               strlen( g_retro3dp_token_strings[i] ) + 1 )
         ) {
            retval = g_retro3dp_token_callbacks[i]( parser );
            goto cleanup;
         }
         i++;
      }
   }

cleanup:

   /* Reset token. */
   retro3dp_reset_token( parser );

   return retval;
}

MERROR_RETVAL
retro3dp_parse_obj_c( struct RETRO3DP_PARSER* parser, unsigned char c ) {
   MERROR_RETVAL retval = RETROFLAT_OK;
   struct RETRO3DP_FACE* face = NULL;

   if(
      RETRO3DP_PARSER_STATE_COMMENT == retro3dp_pstate( parser ) && '\r' != c && '\n' != c
   ) {
      /* We're inside of a comment. */
      return RETROFLAT_OK;
   }

   switch( c ) {
   case '#':
      /* Start of a comment. */
      retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_COMMENT );
      break;

   case '\r':
   case '\n':
      /* New line! End of a comment or token. */
      if( RETRO3DP_PARSER_STATE_COMMENT == retro3dp_pstate( parser ) ) {
         retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_NONE );
         return RETROFLAT_OK;

      } else if(
         RETRO3DP_PARSER_STATE_FACE_VERTEX == retro3dp_pstate( parser ) ||
         RETRO3DP_PARSER_STATE_FACE_TEXTURE == retro3dp_pstate( parser ) ||
         RETRO3DP_PARSER_STATE_FACE_NORMAL == retro3dp_pstate( parser )
      ) {
         /* End of face. */
         retval = retro3dp_parse_obj_token( parser );
         retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_NONE );

         mdata_vector_lock( &(parser->obj->faces) );
         face = mdata_vector_get_last(
            &(parser->obj->faces), struct RETRO3DP_FACE );
         assert( NULL != face );

         /* Use current parser material and register the last vertex. */
         face->material_idx = parser->material_idx;
         face->vertex_idxs_sz++;

         mdata_vector_unlock( &(parser->obj->faces) );
         return retval;

      } else if(
         RETRO3DP_PARSER_STATE_VNORMAL_Z == retro3dp_pstate( parser )
      ) {
         /* End of vertex. */
         retval = retro3dp_parse_obj_token( parser );
         return retval;

      } else if( RETRO3DP_PARSER_STATE_VERTEX_Z == retro3dp_pstate( parser ) ) {

         /* End of vertex. */
         retval = retro3dp_parse_obj_token( parser );
         return retval;

      } else {
         return retro3dp_parse_obj_token( parser );
      }

   case '\t':
   case ' ':
      /* Whitespace. Inside of a comment or time for a new token. */
      if( RETRO3DP_PARSER_STATE_COMMENT == retro3dp_pstate( parser ) ) {
         /* Do nothing on spaces in comments. */
         return RETROFLAT_OK;

      } else if(
         RETRO3DP_PARSER_STATE_FACE_VERTEX == retro3dp_pstate( parser ) ||
         RETRO3DP_PARSER_STATE_FACE_TEXTURE == retro3dp_pstate( parser ) ||
         RETRO3DP_PARSER_STATE_FACE_NORMAL == retro3dp_pstate( parser )
      ) {
         /* End of vertex of face. */
         retval = retro3dp_parse_obj_token( parser );

         /* A space means we're moving on to the next vertex in this face! */
         mdata_vector_lock( &(parser->obj->faces) );
         face = mdata_vector_get_last(
            &(parser->obj->faces), struct RETRO3DP_FACE );
         assert( NULL != face );
         face->vertex_idxs_sz++;
         mdata_vector_unlock( &(parser->obj->faces) );
         retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_FACE_VERTEX );
         return retval;

      } else if(
         RETRO3DP_PARSER_STATE_VNORMAL_Z == retro3dp_pstate( parser )
      ) {
         /* End of vnormal. */
         retval = retro3dp_parse_obj_token( parser );
         return retval;

      } else if( RETRO3DP_PARSER_STATE_VERTEX_Z == retro3dp_pstate( parser ) ) {
         /* End of vertex. */
         retval = retro3dp_parse_obj_token( parser );
         return retval;

      } else if( RETRO3DP_PARSER_STATE_MTL_KD_B == retro3dp_pstate( parser ) ) {
         retval = retro3dp_parse_obj_token( parser );
         /* This tuple has a space after blue, so maybe alpha? */
         /* TODO: Set alpha state. */
         return retval;
         
      } else {
         return retro3dp_parse_obj_token( parser );
      }

   case '/':
      if( RETRO3DP_PARSER_STATE_FACE_VERTEX == retro3dp_pstate( parser ) ) {
         retval = retro3dp_parse_obj_token( parser );
         retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_FACE_TEXTURE );
         return retval;

      } else if(
         RETRO3DP_PARSER_STATE_FACE_TEXTURE == retro3dp_pstate( parser )
      ) {
         retval = retro3dp_parse_obj_token( parser );
         retro3dp_pstate_push( parser, RETRO3DP_PARSER_STATE_FACE_NORMAL );
         return retval;
      }
      
      /* v/vt/vn/??? */
      assert( RETRO3DP_PARSER_STATE_FACE_NORMAL != retro3dp_pstate( parser ) );

      /* If not part of a face, fall through to default append. */
      return retro3dp_append_token( parser, c );

   default:
      return retro3dp_append_token( parser, c );
   }

cleanup:

   return retval;
}

MERROR_RETVAL retro3dp_parse_obj_file(
   const char* filename, struct RETRO3DP_PARSER* parser,
   struct RETRO3DP_OBJ* obj
) {
   int auto_parser = 0; /* Did we provision parser? */
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t obj_file;
   char c;

   if( NULL == parser ) {
      parser = calloc( 1, sizeof( struct RETRO3DP_PARSER ) );
      assert( NULL != parser );
      auto_parser = 1;
   }

   /* Build the path to the obj. */
   memset( filename_path, '\0', RETROFLAT_PATH_MAX + 1 );
   maug_snprintf( filename_path, RETROFLAT_PATH_MAX, "%s%c%s",
      g_retroflat_state->assets_path, RETROFLAT_PATH_SEP, filename );

   /* Open the file and allocate the buffer. */
   retval = mfile_open_read( filename_path, &obj_file );
   maug_cleanup_if_not_ok();

   retro3dp_parse_init( 
      parser, obj, (retro3dp_mtl_cb)retro3dp_parse_obj_file, obj );

   /* Parse the obj, byte by byte. */
   while( mfile_has_bytes( &obj_file ) ) {
      retval = obj_file.read_int( &obj_file, (uint8_t*)&c, 1, 0 );
      maug_cleanup_if_not_ok();
      retval = retro3dp_parse_obj_c( parser, c );
      maug_cleanup_if_not_ok();
   }

   if( auto_parser ) {
      free( parser );
      parser = NULL;
   }

   assert( !mdata_vector_is_locked( &(obj->vertices) ) );
   assert( !mdata_vector_is_locked( &(obj->vnormals) ) );
   assert( !mdata_vector_is_locked( &(obj->materials) ) );
   assert( !mdata_vector_is_locked( &(obj->faces) ) );

   debug_printf(
      RETRO3DP_TRACE_LVL,
      "parsed %s, " SIZE_T_FMT " vertices, " SIZE_T_FMT " normals, "
         SIZE_T_FMT " faces, " SIZE_T_FMT " texture vertices, "
         SIZE_T_FMT " materials",
      filename_path,
      mdata_vector_ct( &(obj->vertices) ),
      mdata_vector_ct( &(obj->vnormals) ),
      mdata_vector_ct( &(obj->faces) ),
      mdata_vector_ct( &(obj->vtextures) ),
      mdata_vector_ct( &(obj->materials) ) );

cleanup:

   return retval;
}

/* === */

void retro3dp_destroy_obj( struct RETRO3DP_OBJ* obj ) {
   debug_printf( RETRO3DP_TRACE_LVL, "destroying object..." );
   mdata_vector_free( &(obj->vertices) );
   mdata_vector_free( &(obj->vtextures) );
   mdata_vector_free( &(obj->vnormals) );
   mdata_vector_free( &(obj->faces) );
   mdata_vector_free( &(obj->materials) );
}

#endif /* RETRO3DP_C */

#endif /* !RETRO3DP_H */

