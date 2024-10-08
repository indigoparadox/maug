
#ifndef RETROGLU_H
#define RETROGLU_H

/* TODO */
#ifndef MAUG_CONST
#  define MAUG_CONST const
#endif /* !MAUG_CONST */

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

#ifndef RETROGLU_TRACE_LVL
#  define RETROGLU_TRACE_LVL 0
#endif /* !RETROGLU_TRACE_LVL */

#ifndef RETROGLU_SPRITE_TEX_FRAMES_SZ
#  define RETROGLU_SPRITE_TEX_FRAMES_SZ 10
#endif /* !RETROGLU_SPRITE_TEX_FRAMES_SZ */

#ifndef MAUG_OS_NDS
#  define glShininessf( side, light, f ) glMaterialf( side, light, f )
#endif /* MAUG_OS_NDS */

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

typedef float RETROGLU_COLOR[4];

struct RETROGLU_VERTEX {
   float x;
   float y;
   float z;
};

struct RETROGLU_VTEXTURE {
   float u;
   float v;
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
   uint16_t vertex_idxs[RETROGLU_FACE_VERTICES_SZ_MAX];
   uint16_t vnormal_idxs[RETROGLU_FACE_VERTICES_SZ_MAX];
   uint16_t vtexture_idxs[RETROGLU_FACE_VERTICES_SZ_MAX];
   uint16_t vertex_idxs_sz;
   uint16_t material_idx;
};

#define RETROGLU_VERTICES_SZ_MAX 1024
#define RETROGLU_FACES_SZ_MAX 1024
#define RETROGLU_MATERIALS_SZ_MAX 1024

struct RETROGLU_OBJ {
   uint8_t flags;
   struct RETROGLU_VERTEX vertices[RETROGLU_VERTICES_SZ_MAX];
   uint16_t vertices_sz;
   struct RETROGLU_VERTEX vnormals[RETROGLU_VERTICES_SZ_MAX];
   uint16_t vnormals_sz;
   struct RETROGLU_VTEXTURE vtextures[RETROGLU_VERTICES_SZ_MAX];
   uint16_t vtextures_sz;
   /**
    * \brief List of faces from an OBJ file. Faces comprise a list of polygons
    *        denoted by index of the vertices in RETROGLU_PARSER::vertices.
    */
   struct RETROGLU_FACE faces[RETROGLU_FACES_SZ_MAX];
   uint16_t faces_sz;
   struct RETROGLU_MATERIAL materials[RETROGLU_MATERIALS_SZ_MAX];
   uint16_t materials_sz;
};

/**
 * \addtogroup maug_retroglu_sprite RetroGLU Sprites
 * \{
 */

#define RETROGLU_SPRITE_X 0
#define RETROGLU_SPRITE_Y 1

#ifndef RETROGLU_SPRITE_LIST_SZ_MAX
#  define RETROGLU_SPRITE_LIST_SZ_MAX 10
#endif /* !RETROGLU_SPRITE_LIST_SZ_MAX */

/**
 * \brief If draw lists are disabled, this struct holds a list of params for
 *        retroglu_set_sprite_clip() so that the sprite can be drawn
 *        without a list.
 */
struct RETROGLU_SPRITE_PARMS {
   uint32_t front_px;
   uint32_t front_py;
   uint32_t back_px;
   uint32_t back_py;
   uint32_t pw;
   uint32_t ph;
   uint8_t flags;
};

struct RETROGLU_SPRITE {
   float vertices_front[6][2];
   float vtexture_front[6][2];
   float vertices_back[6][2];
   float vtexture_back[6][2];
   float translate_x;
   float translate_y;
   int rotate_y;
   RETROGLU_COLOR color;
   struct RETROFLAT_BITMAP texture;
#ifdef RETROGLU_NO_LISTS
   struct RETROGLU_SPRITE_PARMS parms[RETROGLU_SPRITE_LIST_SZ_MAX];
#else
   GLint lists[RETROGLU_SPRITE_LIST_SZ_MAX];
#endif /* RETROGLU_NO_LISTS */
};

struct RETROGLU_TILE {
   float vertices[6][2];
   float vtexture[6][2];
   int rotate_x;
   struct RETROFLAT_BITMAP texture;
};


/*! \} */ /* maug_retroglu_sprite */

#define RETROGLU_PROJ_ORTHO 0
#define RETROGLU_PROJ_FRUSTUM 1

struct RETROGLU_PROJ_ARGS {
   uint8_t proj;
   float rzoom;
   float near_plane;
   float far_plane;
   size_t screen_px_w;
   size_t screen_px_h;
};

#ifdef RETROFLAT_API_LIBNDS
#  define retroglu_enable_lightning()
#  define retroglu_disable_lightning()
#else
#  define retroglu_enable_lightning() glEnable( GL_LIGHTING )
#  define retroglu_disable_lightning() glDisable( GL_LIGHTING )
#endif /* RETROFLAT_API_NDS */

/**
 * \addtogroup maug_retroglu_overlay RetroGLU Overlay API
 * \brief Convenience macros for drawing a 2D overlay using transparent
 *        textures.
 * \{
 */

/**
 * \brief Push current projection modelview and prepare useful parameters for
 *        drawing a textured overlay.
 */
#define retroglu_push_overlay( x, y, x_f, y_f, aspect_f ) \
   /* Switch to projection setup. */ \
   glMatrixMode( GL_PROJECTION ); \
   glPushMatrix(); \
   /* Lighting makes overlay text hard to see. */ \
   retroglu_disable_lightning(); \
   /* Use ortho for overlay. */ \
   glLoadIdentity(); \
   aspect_f = (float)retroflat_screen_w() / (float)retroflat_screen_h(); \
   /* Switch to ortho projection proportional to screen size. */ \
   glOrtho( -1.0f * aspect_f, aspect_f, -1.0f, 1.0f, 0, 10.0f ); \
   /* -1 to 1 is 2! */ \
   aspect_f *= 2.0f; \
   /* Assuming width > height for aspect ratio. */ \
   x_f = ((x) * aspect_f / retroflat_screen_w()) - (aspect_f / 2); \
   /* Vertical coords also need to be inverted because OpenGL. */ \
   y_f = 1.0f - ((y) * 2.0f / retroflat_screen_h()); \

/**
 * \brief Convert width/height in pixels to proportional float values for
 *        OpenGL.
 * \param w Width of an overlay in pixels (read-only int).
 * \param h Height of an overlay in pixels (read-only int).
 * \param w_f Float to assign proportional width of the overlay.
 * \param h_f Float to assign proportional height of the overlay.
 */
#define retroglu_whf( w, h, w_f, h_f, aspect_f ) \
   w_f = ((w) * aspect_f / retroflat_screen_w()); \
   h_f = ((h) * 2.0f / retroflat_screen_h());

/**
 * \brief Restore projection modelview previously pushed using retroglu_push().
 */
#define retroglu_pop_overlay() \
   glPopMatrix(); \
   glMatrixMode( GL_MODELVIEW );

/*! \} */ /* maug_retroglu_overlay */

#define retroglu_tex_px_x_to_f( px, sprite ) \
   ((px) * 1.0 / sprite->texture.tex.w)
#define retroglu_tex_px_y_to_f( px, sprite ) \
   ((px) * 1.0 / sprite->texture.tex.h)

#define retroglu_scr_px_x_to_f( px ) \
   (float)(((px) * 1.0 / (retroflat_screen_w() / 2)) - 1.0)
#define retroglu_scr_px_y_to_f( py ) \
   (float)(((py) * 1.0 / (retroflat_screen_h() / 2)) - 1.0)

#define retroglu_set_sprite_tex( sprite, texture_id, bmp_w, bmp_h ) \
   sprite->texture_id = texture_id; \
   sprite->texture_w = bmp_w; \
   sprite->texture_h = bmp_h;

#define retroglu_set_sprite_color( sprite, color_in ) \
   memcpy( (sprite)->color, (color_in), 3 * sizeof( float ) )

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
   debug_printf( \
      RETROGLU_TRACE_LVL, "changing parser to state: %d", new_state ); \
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
   struct RETROGLU_OBJ* obj;
   int state;
   int material_idx;
   char token[RETROGLU_PARSER_TOKEN_SZ_MAX];
   size_t token_sz;
   retroglu_mtl_cb load_mtl;
   void* load_mtl_data;
};

typedef int (*retroglu_token_cb)( struct RETROGLU_PARSER* parser );

void retroglu_init_scene( uint8_t flags );
void retroglu_init_projection( struct RETROGLU_PROJ_ARGS* args );

/**
 * \related RETROGLU_PARSER
 * \brief Initialize a ::RETROGLU_PARSER.
 * \warning This should be run before retroglu_parse_obj_c() is called on the
 *          parser!
 */
void retroglu_parse_init(
   struct RETROGLU_PARSER* parser, struct RETROGLU_OBJ* obj,
   retroglu_mtl_cb load_mtl, void* load_mtl_data
);

/**
 * \related RETROGLU_PARSER
 * \brief Parse OBJ data into a parser, one character at a time.
 *
 * Generally, this should loop over a character array loaded from an OBJ file.
 */
MERROR_RETVAL
retroglu_parse_obj_c( struct RETROGLU_PARSER* parser, unsigned char c );

MERROR_RETVAL retroglu_parse_obj_file(
   const char* filename, struct RETROGLU_PARSER* parser,
   struct RETROGLU_OBJ* obj );

/*! \} */ /* maug_retroglu_obj_fsm */

void retroglu_draw_poly( struct RETROGLU_OBJ* obj );

void retroglu_set_tile_clip(
   struct RETROGLU_TILE* tile,
   uint32_t px, uint32_t py, uint32_t pw, uint32_t ph, uint8_t flags );

void retroglu_set_sprite_clip(
   struct RETROGLU_SPRITE* sprite,
   uint32_t front_px, uint32_t front_py, uint32_t back_px, uint32_t back_py,
   uint32_t pw, uint32_t ph, uint8_t flags );
 
/**
 * \brief Setup the sprite vertices for the poly the sprite will be drawn on.
 *        This should be called once when the sprite is initialized, but
 *        calling it again later shouldn't hurt.
 */
void retroglu_init_sprite_vertices( struct RETROGLU_SPRITE* sprite );

void retroglu_init_sprite_vertices_scale(
   struct RETROGLU_SPRITE* sprite, float scale );

void retroglu_set_sprite_pos(
   struct RETROGLU_SPRITE* sprite, uint32_t px, uint32_t py );

void retroglu_tsrot_sprite( struct RETROGLU_SPRITE* sprite );

/**
 * \brief Draw the given sprite. This function never uses a list, and can
 *        therefore be used to create a draw list.
 */
void retroglu_draw_sprite( struct RETROGLU_SPRITE* sprite );

/**
 * \brief If lists are enabled, prerender the sprite to a list using the
 *        given params to retroglu_set_sprite_clip().
 *
 * \note If lists are not enabled, this call does nothing.
 */
void retroglu_prerender_sprite(
   struct RETROGLU_SPRITE* sprite, int list_idx,
   uint32_t front_px, uint32_t front_py, uint32_t back_px, uint32_t back_py,
   uint32_t pw, uint32_t ph, uint8_t flags );

/**
 * \brief If lists are enabled, render the sprite list at list_idx. Otherwise,
 *        draw the sprite using retroglu_draw_sprite().
 */
void retroglu_jitrender_sprite( struct RETROGLU_SPRITE* sprite, int list_idx );

void retroglu_free_sprite( struct RETROGLU_SPRITE* sprite );

MERROR_RETVAL retroglu_init_glyph_tex();

void retroglu_destroy_glyph_tex();

void retroglu_string(
   float x, float y, float z, const RETROGLU_COLOR color,
   const char* str, size_t str_sz, const char* font_str, uint8_t flags );

MERROR_RETVAL retroglu_check_errors( const char* desc );

/* int retroglu_draw_lock( struct RETROFLAT_BITMAP* bmp ); */

int retroglu_draw_release( struct RETROFLAT_BITMAP* bmp );

MERROR_RETVAL retroglu_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, size_t d_x, size_t d_y, size_t w, size_t h,
   int16_t instance );

#ifdef RETROGLU_C

#  define RETROFLAT_COLOR_TABLE_GL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   MAUG_CONST float RETROGLU_COLOR_ ## name_u[] = { \
      (float)((float)r * 1.0f / 255.0f), \
      (float)((float)g * 1.0f / 255.0f), \
      (float)((float)b * 1.0f / 255.0f) };

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_GL )

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
   parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz = 0;
   return RETROFLAT_OK;
}

int retroglu_token_usemtl( struct RETROGLU_PARSER* parser ) {
   retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_MATERIAL );
   return RETROFLAT_OK;
}

int retroglu_token_newmtl( struct RETROGLU_PARSER* parser ) {
   /* Set default lighting alpha to non-transparent. */
   parser->obj->materials[parser->obj->materials_sz].ambient[3] = 1.0f;
   parser->obj->materials[parser->obj->materials_sz].diffuse[3] = 1.0f;
   parser->obj->materials[parser->obj->materials_sz].specular[3] = 1.0f;
   parser->obj->materials[parser->obj->materials_sz].emissive[3] = 1.0f;
   parser->obj->materials[parser->obj->materials_sz].specular_exp = 0;
   parser->obj->materials_sz++;
   assert( parser->obj->materials_sz <= RETROGLU_MATERIALS_SZ_MAX );
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

void retroglu_init_scene( uint8_t flags ) {
   debug_printf( RETROGLU_TRACE_LVL, "initializing..." );

#ifdef MAUG_OS_NDS
   glMaterialShinyness();
   glPolyFmt( POLY_ALPHA( 15 ) | POLY_CULL_BACK  | POLY_FORMAT_LIGHT0 );
#else
   glEnable( GL_CULL_FACE );
   glShadeModel( GL_SMOOTH );

   /* Setup texture transparency. */
   glEnable( GL_TEXTURE_2D );
   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

   /* Setup depth buffer so triangles in back are hidden. */
   glEnable( GL_DEPTH_TEST );
   glDepthMask( GL_TRUE );
   glDepthFunc( GL_LESS );
   glDepthRange( 0.0f, 1.0f );
#endif /* !MAUG_OS_NDS */
}

void retroglu_init_projection( struct RETROGLU_PROJ_ARGS* args ) {
   float aspect_ratio = 0;

   if( 0 == args->screen_px_w ) {
      debug_printf( 1,
         "using assumed screen width: " SIZE_T_FMT, retroflat_screen_w() );
      args->screen_px_w = retroflat_screen_w();
   } else {
      debug_printf( 1,
         "using specified screen width: " SIZE_T_FMT, retroflat_screen_w() );
   }
   if( 0 == args->screen_px_h ) {
      debug_printf( 1,
         "using assumed screen height: " SIZE_T_FMT, retroflat_screen_h() );
      args->screen_px_h = retroflat_screen_h();
   } else {
      debug_printf( 1,
         "using specified screen height: " SIZE_T_FMT, retroflat_screen_h() );
   }

   /* Setup projection. */
#ifdef MAUG_OS_NDS
   glViewport( 0, 0, 255, 191 );
#else
   glViewport(
      0, 0, (uint32_t)(args->screen_px_w), (uint32_t)(args->screen_px_h) );
#endif
   aspect_ratio = (float)(args->screen_px_w) / (float)(args->screen_px_h);

   /* Switch to projection matrix for setup. */
   glMatrixMode( GL_PROJECTION );

   /* Zero everything out. */
   glLoadIdentity();

   /* Near plane can't be zero for frustum! */
   assert( 0 != args->near_plane );

   switch( args->proj ) {
   case RETROGLU_PROJ_FRUSTUM:
      /* This is really tweaky, and when it breaks, polygons seem to get drawn
         * out of order? Still experimenting/researching. */
      debug_printf( RETROGLU_TRACE_LVL, "aspect ratio: %f", aspect_ratio );
      glFrustum(
         /* The smaller these are, the closer it lets us get to the camera? */
         -1.0f * args->rzoom * aspect_ratio, args->rzoom * aspect_ratio,
         -1.0f * args->rzoom, args->rzoom,
         args->near_plane, args->far_plane );
      break;

   case RETROGLU_PROJ_ORTHO:
      /* This is much simpler/more forgiving than frustum. */
      glOrtho(
         -1.0f * args->rzoom * aspect_ratio,
         args->rzoom * aspect_ratio,
         -1.0f * args->rzoom, args->rzoom,
         args->near_plane, args->far_plane );
      break;
   }

   /* Revert to model matrix for later instructions (out of this scope). */
   glMatrixMode( GL_MODELVIEW );
}

void retroglu_parse_init(
   struct RETROGLU_PARSER* parser, struct RETROGLU_OBJ* obj,
   retroglu_mtl_cb load_mtl, void* load_mtl_data
) {
   parser->load_mtl = load_mtl;
   parser->load_mtl_data = load_mtl_data;
   parser->obj = obj;
   assert( NULL != parser->obj );
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
      /* TODO: Maug replacement for C99 crutch. */ \
      parser->obj->array[parser->obj->sz].val = strtod( parser->token, NULL ); \
      debug_printf( RETROGLU_TRACE_LVL, "vertex %d " desc ": %f", \
         parser->obj->sz, parser->obj->array[parser->obj->sz].val ); \
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_ ## state_next );

MERROR_RETVAL
retroglu_parse_token( struct RETROGLU_PARSER* parser ) {
   int i = 0;
   MERROR_RETVAL retval = RETROFLAT_OK;

   if( 0 == parser->token_sz ) {
      /* Empty token. */
      goto cleanup;
   }

   /* NULL-terminate token. */
   parser->token[parser->token_sz] = '\0';

   debug_printf( RETROGLU_TRACE_LVL, "token: %s", parser->token );

   if( RETROGLU_PARSER_STATE_MATERIAL_LIB == parser->state ) {

      debug_printf(
         RETROGLU_TRACE_LVL, "parsing material lib: %s", parser->token );
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );
      assert( NULL != parser->load_mtl );
      return parser->load_mtl( parser->token, parser, parser->load_mtl_data );

   RETROGLU_TOKENS_VF( RETROGLU_TOKEN_PARSE_VF )

      /* TODO: Handle W. */

   } else if( RETROGLU_PARSER_STATE_FACE_VERTEX == parser->state ) {
      /* Parsing face vertex index. */
      parser->obj->faces[parser->obj->faces_sz].vertex_idxs[
         parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz] =
            atoi( parser->token );

      debug_printf( RETROGLU_TRACE_LVL, "face %d, vertex %d: %d",
         parser->obj->faces_sz, parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz,
         parser->obj->faces[parser->obj->faces_sz].vertex_idxs[
            parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz] );

      /* The new state is set in the parser below, as it could become
       * RETROGLU_PARSER_STATE_FACE_NORMAL or RETROGLU_PARSER_STATE_NONE,
       * depending on whether it's a whitespace/newline/slash after.
       * Same for index incr.
       */

   } else if( RETROGLU_PARSER_STATE_FACE_NORMAL == parser->state ) {

      /* Parsing face normal index. */
      parser->obj->faces[parser->obj->faces_sz].vnormal_idxs[
         parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz] =
            atoi( parser->token );

      debug_printf( RETROGLU_TRACE_LVL, "face %d, normal %d: %d",
         parser->obj->faces_sz, parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz,
         parser->obj->faces[parser->obj->faces_sz].vnormal_idxs[
            parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz] );

      /* The new state is set in the parser below, as it could become
       * RETROGLU_PARSER_STATE_FACE_NORMAL or RETROGLU_PARSER_STATE_NONE,
       * depending on whether it's a whitespace/newline/slash after.
       * Same for index incr.
       */

   } else if( RETROGLU_PARSER_STATE_FACE_TEXTURE == parser->state ) {

      /* Parsing face texture index. */
      parser->obj->faces[parser->obj->faces_sz].vtexture_idxs[
         parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz] =
            atoi( parser->token );

      debug_printf( RETROGLU_TRACE_LVL, "face %d, texture %d: %d",
         parser->obj->faces_sz, parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz,
         parser->obj->faces[parser->obj->faces_sz].vtexture_idxs[
            parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz] );

      /* The new state is set in the parser below, as it could become
       * RETROGLU_PARSER_STATE_FACE_NORMAL or RETROGLU_PARSER_STATE_NONE,
       * depending on whether it's a whitespace/newline/slash after.
       * Same for index incr.
       */

 
   } else if( RETROGLU_PARSER_STATE_FACE_MATERIAL == parser->state ) {

      /* Find the material index and assign it to the parser. */
      for( i = 0 ; parser->obj->materials_sz > i ; i++ ) {
         debug_printf(
            RETROGLU_TRACE_LVL,
            "%s vs %s", parser->obj->materials[i].name, parser->token );
         if( 0 == strncmp(
            parser->obj->materials[i].name, parser->token,
            RETROGLU_MATERIAL_NAME_SZ_MAX
         ) ) {
            debug_printf( RETROGLU_TRACE_LVL, "using material: \"%s\" (%d)",
               parser->obj->materials[i].name, i );
            parser->material_idx = i;
            break;
         }
      }
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );

   } else if( RETROGLU_PARSER_STATE_MATERIAL_NAME == parser->state ) {

      debug_printf(
         RETROGLU_TRACE_LVL, "adding material: \"%s\" at idx: %d",
         parser->token, parser->obj->materials_sz - 1 );
      maug_strncpy(
         parser->obj->materials[parser->obj->materials_sz - 1].name,
         parser->token,
         RETROGLU_MATERIAL_NAME_SZ_MAX );
      retroglu_parser_state( parser, RETROGLU_PARSER_STATE_NONE );

   } else {
      /* Check against generic tokens. */
      while( '\0' != g_retroglu_token_strings[i][0] ) {
         if(
            parser->token_sz == maug_strlen( g_retroglu_token_strings[i] ) &&
            0 == strncmp(
               parser->token, g_retroglu_token_strings[i],
               parser->token_sz )
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

MERROR_RETVAL 
retroglu_append_token( struct RETROGLU_PARSER* parser, unsigned char c ) {
   parser->token[parser->token_sz++] = c;

   /* Protect against token overflow. */
   if( parser->token_sz >= RETROGLU_PARSER_TOKEN_SZ_MAX ) {
      debug_printf( RETROGLU_TRACE_LVL, "token out of bounds!" );
      return RETROGLU_PARSER_ERROR;
   }

   return RETROFLAT_OK;
}

MERROR_RETVAL
retroglu_parse_obj_c( struct RETROGLU_PARSER* parser, unsigned char c ) {
   MERROR_RETVAL retval = RETROFLAT_OK;

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
         parser->obj->faces[parser->obj->faces_sz].material_idx = 
            parser->material_idx;

         /* Move to next face. */
         parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz++;
         parser->obj->faces_sz++; /* Newline means this face is done. */
         assert( parser->obj->faces_sz <= RETROGLU_FACES_SZ_MAX );
         return retval;

      } else if( RETROGLU_PARSER_STATE_VNORMAL_Z == parser->state ) {

         retval = retroglu_parse_token( parser );
         /* End of vertex. */
         parser->obj->vnormals_sz++;
         assert( parser->obj->vnormals_sz <= RETROGLU_VERTICES_SZ_MAX );
         return retval;

      } else if( RETROGLU_PARSER_STATE_VERTEX_Z == parser->state ) {

         retval = retroglu_parse_token( parser );
         /* End of vertex. */
         parser->obj->vertices_sz++;
         assert( parser->obj->vertices_sz <= RETROGLU_VERTICES_SZ_MAX );
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
         parser->obj->faces[parser->obj->faces_sz].vertex_idxs_sz++;
         retroglu_parser_state( parser, RETROGLU_PARSER_STATE_FACE_VERTEX );
         return retval;

      } else if( RETROGLU_PARSER_STATE_VNORMAL_Z == parser->state ) {

         retval = retroglu_parse_token( parser );
         /* End of vertex. */
         parser->obj->vnormals_sz++;
         assert( parser->obj->vnormals_sz <= RETROGLU_VERTICES_SZ_MAX );
         return retval;

      } else if( RETROGLU_PARSER_STATE_VERTEX_Z == parser->state ) {

         retval = retroglu_parse_token( parser );
         /* End of vertex. */
         parser->obj->vertices_sz++;
         assert( parser->obj->vertices_sz <= RETROGLU_VERTICES_SZ_MAX );
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

MERROR_RETVAL retroglu_parse_obj_file(
   const char* filename, struct RETROGLU_PARSER* parser,
   struct RETROGLU_OBJ* obj
) {
   int auto_parser = 0; /* Did we provision parser? */
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t obj_file;
   char c;

   if( NULL == parser ) {
      parser = calloc( 1, sizeof( struct RETROGLU_PARSER ) );
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

   retroglu_parse_init( 
      parser, obj, (retroglu_mtl_cb)retroglu_parse_obj_file, obj );

   /* Parse the obj, byte by byte. */
   while( mfile_has_bytes( &obj_file ) ) {
      retval = obj_file.read_int( &obj_file, (uint8_t*)&c, 1, 0 );
      maug_cleanup_if_not_ok();
      retval = retroglu_parse_obj_c( parser, c );
      maug_cleanup_if_not_ok();
   }

   if( auto_parser ) {
      free( parser );
      parser = NULL;
   }

   debug_printf(
      RETROGLU_TRACE_LVL,
      "parsed %s, %u vertices, %u materials",
      filename_path, obj->vertices_sz, obj->materials_sz );

cleanup:

   return retval;
}

void retroglu_draw_poly( struct RETROGLU_OBJ* obj ) {
   int i = 0;
   int j = 0;

   glBegin( GL_TRIANGLES );
   for( i = 0 ; obj->faces_sz > i ; i++ ) {
   
      /* TODO: Handle material on NDS. */
      glMaterialfv( GL_FRONT, GL_DIFFUSE,
         obj->materials[obj->faces[i].material_idx].diffuse );
         /*
      glMaterialfv( GL_FRONT, GL_AMBIENT,
         obj->materials[faces[i].material_idx].ambient );
         */
      glMaterialfv( GL_FRONT, GL_SPECULAR,
         obj->materials[obj->faces[i].material_idx].specular );
      glMaterialfv( GL_FRONT, GL_EMISSION,
         obj->materials[obj->faces[i].material_idx].emissive );

      glColor3fv( obj->materials[obj->faces[i].material_idx].diffuse );

      /* Use a specific macro here that can be overridden for e.g. the NDS. */
      glShininessf( GL_FRONT, GL_SHININESS, 
         obj->materials[obj->faces[i].material_idx].specular_exp );

      for( j = 0 ; obj->faces[i].vertex_idxs_sz > j ; j++ ) {
         assert( 0 < obj->faces[i].vertex_idxs[j] );
         assert( 3 == obj->faces[i].vertex_idxs_sz );

         glNormal3f(
            obj->vnormals[obj->faces[i].vnormal_idxs[j] - 1].x,
            obj->vnormals[obj->faces[i].vnormal_idxs[j] - 1].y,
            obj->vnormals[obj->faces[i].vnormal_idxs[j] - 1].z );

         glVertex3f(
            obj->vertices[obj->faces[i].vertex_idxs[j] - 1].x,
            obj->vertices[obj->faces[i].vertex_idxs[j] - 1].y,
            obj->vertices[obj->faces[i].vertex_idxs[j] - 1].z );
      }

   }
   glEnd();
}

void retroglu_set_tile_clip(
   struct RETROGLU_TILE* tile,
   uint32_t px, uint32_t py, uint32_t pw, uint32_t ph, uint8_t flags
) {
   /* Set vertices in terms of half the clip size so that rotation is around
    * the midpoint of the sprite, not the side!
    */
   float clip_tex_x = 0, /* Front tex X */
      clip_tex_y = 0, /* Front tex Y */
      clip_tex_w = 0,
      clip_tex_h = 0;

   /* Setup texture tilesheet. */

   clip_tex_x = retroglu_tex_px_x_to_f( px, tile ); 
   clip_tex_y = retroglu_tex_px_y_to_f( py, tile ); 
   clip_tex_w = retroglu_tex_px_x_to_f( pw, tile );
   clip_tex_h = retroglu_tex_px_y_to_f( ph, tile );

   /* == Front Face Textures == */

   /* Lower Left */
   tile->vtexture[0][RETROGLU_SPRITE_X] = clip_tex_x;
   tile->vtexture[0][RETROGLU_SPRITE_Y] = clip_tex_y;

   /* Lower Right */
   tile->vtexture[1][RETROGLU_SPRITE_X] = clip_tex_x + clip_tex_w;
   tile->vtexture[1][RETROGLU_SPRITE_Y] = clip_tex_y;

   /* Upper Right */
   tile->vtexture[2][RETROGLU_SPRITE_X] = clip_tex_x + clip_tex_w;
   tile->vtexture[2][RETROGLU_SPRITE_Y] = clip_tex_y + clip_tex_h;

   /* Upper Right */
   tile->vtexture[3][RETROGLU_SPRITE_X] = clip_tex_x + clip_tex_w;
   tile->vtexture[3][RETROGLU_SPRITE_Y] = clip_tex_y + clip_tex_h;

   /* Upper Left */
   tile->vtexture[4][RETROGLU_SPRITE_X] = clip_tex_x;
   tile->vtexture[4][RETROGLU_SPRITE_Y] = clip_tex_y + clip_tex_h;

   /* Lower Left */
   tile->vtexture[5][RETROGLU_SPRITE_X] = clip_tex_x;
   tile->vtexture[5][RETROGLU_SPRITE_Y] = clip_tex_y;
}

void retroglu_init_tile_vertices( struct RETROGLU_TILE* tile ) {

   /* == Front Face Vertices == */

   /* Lower-Left */
   tile->vertices[0][RETROGLU_SPRITE_X] = -1;
   tile->vertices[0][RETROGLU_SPRITE_Y] = -1;
   
   /* Lower-Right */
   tile->vertices[1][RETROGLU_SPRITE_X] = 1;
   tile->vertices[1][RETROGLU_SPRITE_Y] = -1;
   
   /* Upper-Right */
   tile->vertices[2][RETROGLU_SPRITE_X] = 1;
   tile->vertices[2][RETROGLU_SPRITE_Y] = 1;

   /* Upper-Right */
   tile->vertices[3][RETROGLU_SPRITE_X] = 1;
   tile->vertices[3][RETROGLU_SPRITE_Y] = 1;

   /* Upper-Left */
   tile->vertices[4][RETROGLU_SPRITE_X] = -1;
   tile->vertices[4][RETROGLU_SPRITE_Y] = 1;

   /* Lower-Left */
   tile->vertices[5][RETROGLU_SPRITE_X] = -1;
   tile->vertices[5][RETROGLU_SPRITE_Y] = -1;
}

void retroglu_set_sprite_clip(
   struct RETROGLU_SPRITE* sprite,
   uint32_t front_px, uint32_t front_py, uint32_t back_px, uint32_t back_py,
   uint32_t pw, uint32_t ph, uint8_t flags
) {
   /* Set vertices in terms of half the clip size so that rotation is around
    * the midpoint of the sprite, not the side!
    */
   float clip_tex_fx = 0, /* Front tex X */
      clip_tex_fy = 0, /* Front tex Y */
      clip_tex_bx = 0, /* Back tex X */
      clip_tex_by = 0, /* Back tex Y */
      clip_tex_w = 0,
      clip_tex_h = 0;

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
}

/* === */

void retroglu_init_sprite_vertices( struct RETROGLU_SPRITE* sprite ) {
   retroglu_init_sprite_vertices_scale( sprite, 1.0f );
}

/* === */

void retroglu_init_sprite_vertices_scale(
   struct RETROGLU_SPRITE* sprite, float scale
) {

   /* == Front Face Vertices == */

   /* Lower-Left */
   sprite->vertices_front[0][RETROGLU_SPRITE_X] = -1.0f * scale;
   sprite->vertices_front[0][RETROGLU_SPRITE_Y] = -1.0f * scale;
   
   /* Lower-Right */
   sprite->vertices_front[1][RETROGLU_SPRITE_X] = scale;
   sprite->vertices_front[1][RETROGLU_SPRITE_Y] = -1.0f * scale;
   
   /* Upper-Right */
   sprite->vertices_front[2][RETROGLU_SPRITE_X] = scale;
   sprite->vertices_front[2][RETROGLU_SPRITE_Y] = scale;

   /* Upper-Right */
   sprite->vertices_front[3][RETROGLU_SPRITE_X] = scale;
   sprite->vertices_front[3][RETROGLU_SPRITE_Y] = scale;

   /* Upper-Left */
   sprite->vertices_front[4][RETROGLU_SPRITE_X] = -1.0f * scale;
   sprite->vertices_front[4][RETROGLU_SPRITE_Y] = scale;

   /* Lower-Left */
   sprite->vertices_front[5][RETROGLU_SPRITE_X] = -1.0f * scale;
   sprite->vertices_front[5][RETROGLU_SPRITE_Y] = -1.0f * scale;

   /* == Back Face Vertices == */

   /* Lower-Right */
   sprite->vertices_back[0][RETROGLU_SPRITE_X] = scale;
   sprite->vertices_back[0][RETROGLU_SPRITE_Y] = -1.0f * scale;

   /* Lower-Left */
   sprite->vertices_back[1][RETROGLU_SPRITE_X] = -1.0f * scale;
   sprite->vertices_back[1][RETROGLU_SPRITE_Y] = -1.0f * scale;

   /* Upper-Left */
   sprite->vertices_back[2][RETROGLU_SPRITE_X] = -1.0f * scale;
   sprite->vertices_back[2][RETROGLU_SPRITE_Y] = scale;

   /* Upper-Left */
   sprite->vertices_back[3][RETROGLU_SPRITE_X] = -1.0f * scale;
   sprite->vertices_back[3][RETROGLU_SPRITE_Y] = scale;

   /* Upper-Right */
   sprite->vertices_back[4][RETROGLU_SPRITE_X] = scale;
   sprite->vertices_back[4][RETROGLU_SPRITE_Y] = scale;

   /* Lower-Right */
   sprite->vertices_back[5][RETROGLU_SPRITE_X] = scale;
   sprite->vertices_back[5][RETROGLU_SPRITE_Y] = -1.0f * scale;
}

/* === */

void retroglu_set_sprite_pos(
   struct RETROGLU_SPRITE* sprite, uint32_t px, uint32_t py
) {
   sprite->translate_x = retroglu_scr_px_x_to_f( px );
   sprite->translate_y = retroglu_scr_px_y_to_f( py );
}

/* === */

void retroglu_tsrot_sprite( struct RETROGLU_SPRITE* sprite ) {
   /* Set the matrix to translate/rotate/scale based on sprite props. */
   glTranslatef( sprite->translate_x, sprite->translate_y, 0 );
   glRotatef( sprite->rotate_y, 0.0f, 1.0f, 0.0f );
}

/* === */

void retroglu_draw_sprite( struct RETROGLU_SPRITE* sprite ) {
   int i = 0;
#ifndef RETROGLU_NO_TEXTURES
#  ifdef RETROGLU_NO_TEXTURE_LISTS
   MERROR_RETVAL retval = MERROR_OK;
#  endif /* RETROGLU_NO_TEXTURES */

   glColor3fv( sprite->color );
   
#  ifndef RETROGLU_NO_TEXTURE_LISTS
   glBindTexture( GL_TEXTURE_2D, sprite->texture.tex.id );
#  else
   maug_mlock( sprite->texture.tex.bytes_h, sprite->texture.tex.bytes );
   maug_cleanup_if_null_alloc( uint8_t*, sprite->texture.tex.bytes );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
      sprite->texture.tex.w, sprite->texture.tex.h, 0,
      GL_RGBA, GL_UNSIGNED_BYTE,
      sprite->texture.tex.bytes ); 
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */
   glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

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

#  ifndef RETROGLU_NO_TEXTURE_LISTS
   glBindTexture( GL_TEXTURE_2D, 0 );
#  else
cleanup:
   if( NULL != sprite->texture.tex.bytes ) {
      maug_munlock( sprite->texture.tex.bytes_h, sprite->texture.tex.bytes );
   }
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */
#endif /* !RETROGLU_NO_TEXTURES */
}

/* === */

void retroglu_prerender_sprite(
   struct RETROGLU_SPRITE* sprite, int list_idx,
   uint32_t front_px, uint32_t front_py, uint32_t back_px, uint32_t back_py,
   uint32_t pw, uint32_t ph, uint8_t flags
) {
#ifdef RETROGLU_NO_LISTS
   sprite->parms[list_idx].front_px = front_px;
   sprite->parms[list_idx].front_py = front_py;
   sprite->parms[list_idx].back_px = back_px;
   sprite->parms[list_idx].back_py = back_py;
   sprite->parms[list_idx].pw = pw;
   sprite->parms[list_idx].ph = ph;
   sprite->parms[list_idx].flags = flags;
#else
   /* Prerender the sprite to a GL list to call later. */
   sprite->lists[list_idx] = glGenLists( 1 );
   retroglu_set_sprite_clip(
      sprite, front_px, front_py, back_px, back_py, pw, ph, flags );
   glNewList( sprite->lists[list_idx], GL_COMPILE );
   retroglu_draw_sprite( sprite );
   glEndList();
#endif /* RETROGLU_NO_LISTS */
}

/* === */

void retroglu_jitrender_sprite( struct RETROGLU_SPRITE* sprite, int list_idx ) {
#ifdef RETROGLU_NO_LISTS
   /* Prerender the sprite to a GL list to call later. */
   retroglu_set_sprite_clip(
      sprite,
      sprite->parms[list_idx].front_px,
      sprite->parms[list_idx].front_py,
      sprite->parms[list_idx].back_px,
      sprite->parms[list_idx].back_py,
      sprite->parms[list_idx].pw,
      sprite->parms[list_idx].ph,
      sprite->parms[list_idx].flags );
   retroglu_draw_sprite( sprite );
#else
   glCallList( sprite->lists[list_idx] );
#endif /* RETROGLU_NO_LISTS */
}

/* === */

void retroglu_free_sprite( struct RETROGLU_SPRITE* sprite ) {
#ifndef RETROGLU_NO_TEXTURES
   if( NULL != sprite->texture.tex.bytes_h ) {
      if( NULL != sprite->texture.tex.bytes ) {
         maug_munlock( sprite->texture.tex.bytes_h, sprite->texture.tex.bytes );
      }
      
      maug_mfree( sprite->texture.tex.bytes_h );
   }

#  ifndef RETROGLU_NO_TEXTURE_LISTS
   if( 0 < sprite->texture.tex.id ) {
      glDeleteTextures( 1, (GLuint*)&(sprite->texture.tex.id) );
   }
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */
#endif /* !RETROGLU_NO_TEXTURES */
}

/* === */

#ifndef RETROFLAT_NO_STRING

#  include "mfont8x8.h"

static
uint32_t g_retroglu_font_tex[RETROSOFT_SETS_COUNT][RETROSOFT_GLYPHS_COUNT];

MERROR_RETVAL retroglu_load_glyph( size_t set_idx, size_t glyph_idx ) {
   MERROR_RETVAL retval = MERROR_OK;

#ifndef RETROGLU_NO_TEXTURES

   uint8_t* bmp_px = NULL;
   int16_t i = 0,
      x = 0,
      y = 0;
   const char* glyph_dots = gc_font8x8[set_idx][glyph_idx];

   /* Allocate temporary buffer for drawing. */
   bmp_px = calloc( RETROSOFT_GLYPH_W_SZ * RETROSOFT_GLYPH_H_SZ, 4 );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_px );
   assert( NULL != bmp_px );

   /* Draw font to texture. */
   for( y = 0 ; RETROSOFT_GLYPH_H_SZ > y ; y++ ) {
      for( x = 0 ; RETROSOFT_GLYPH_W_SZ > x ; x++ ) {
         i = ((RETROSOFT_GLYPH_H_SZ - y - 1) * RETROSOFT_GLYPH_W_SZ) + x;
         assert( i < RETROSOFT_GLYPH_W_SZ * RETROSOFT_GLYPH_H_SZ * 4 );

         if( 1 == ((glyph_dots[y] >> x) & 0x01) ) {
            bmp_px[i * 4] = 0xff;
            bmp_px[(i * 4) + 1] = 0xff;
            bmp_px[(i * 4) + 2] = 0xff;
            bmp_px[(i * 4) + 3] = 0xff;
         }
      }
   }

#  ifndef RETROGLU_NO_TEXTURE_LISTS
   assert( 0 == g_retroglu_font_tex[set_idx][glyph_idx] );
   glGenTextures( 1, (GLuint*)&(g_retroglu_font_tex[set_idx][glyph_idx]) );
   assert( 0 < g_retroglu_font_tex[set_idx][glyph_idx] );
   glBindTexture( GL_TEXTURE_2D, g_retroglu_font_tex[set_idx][glyph_idx] );
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */
   /* glPixelStorei( GL_UNPACK_ALIGNMENT, 4 ); */
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
      RETROSOFT_GLYPH_W_SZ, RETROSOFT_GLYPH_H_SZ, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, bmp_px ); 

cleanup:

   if( NULL != bmp_px ) {
      free( bmp_px );
   }

#endif /* RETROGLU_NO_TEXTURES */

   return retval;
}

/* === */

MERROR_RETVAL retroglu_init_glyph_tex() {
   MERROR_RETVAL retval = MERROR_OK;

#ifndef RETROGLU_NO_TEXTURES
   size_t i = 0,
      j = 0;

   /* Only precache textures we we support glBindTexture(). Otherwise, we'll
    * have to forcibly load them as we use them later on. */

   debug_printf( RETROGLU_TRACE_LVL, "loading glyph textures..." );

   for( i = 0 ; RETROSOFT_SETS_COUNT > i ; i++ ) {
      for( j = 0 ; RETROSOFT_GLYPHS_COUNT > j ; j++ ) {
         retval = retroglu_load_glyph( i, j );
         maug_cleanup_if_not_ok();
      }
   }
#endif /* !RETROGLU_NO_TEXTURES */

cleanup:

   /* TODO: Destroy loaded textures if failure. */

   return retval;
}

/* === */

void retroglu_destroy_glyph_tex() {

#ifndef RETROGLU_NO_TEXTURE_LISTS
   size_t i = 0,
      j = 0;

   debug_printf( RETROGLU_TRACE_LVL, "destroying glyph textures..." );

   for( i = 0 ; RETROSOFT_SETS_COUNT > i ; i++ ) {
      for( j = 0 ; RETROSOFT_GLYPHS_COUNT > j ; j++ ) {
         glDeleteTextures( 1, (GLuint*)&(g_retroglu_font_tex[i][j]) );
      }
   }
#endif /* !RETROGLU_NO_TEXTURE_LISTS */

}

/* === */

#define RETROGLU_FONT_W 0.05f

void retroglu_string(
   float x, float y, float z, const RETROGLU_COLOR color,
   const char* str, size_t str_sz, const char* font_str, uint8_t flags
) {
#ifndef RETROGLU_NO_TEXTURES
   size_t i = 0;
#  ifdef RETROGLU_NO_TEXTURE_LISTS
   MERROR_RETVAL retval = MERROR_OK;
#  endif /* RETROGLU_NO_TEXTURE_LISTS */

   if( str_sz == 0 ) {
      str_sz = maug_strlen( str );
   }

   for( i = 0 ; str_sz > i ; i++ ) {
      /* Stop drawing on NULL. */
      if( '\0' == str[i] ) {
         break;
      }

      glColor3fv( color );

#ifdef RETROGLU_NO_TEXTURE_LISTS
      retval = retroglu_load_glyph( 0, str[i] - ' ' );
      maug_cleanup_if_not_ok();
#else
      glBindTexture( GL_TEXTURE_2D, g_retroglu_font_tex[0][str[i] - ' '] );
#endif /* RETROGLU_NO_TEXTURE_LISTS */
      glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      
      glBegin( GL_TRIANGLES );

      glTexCoord2f( 0, 0 );
      glVertex3f( x + (RETROGLU_FONT_W * i), y, z );
      glTexCoord2f( 1, 0 );
      glVertex3f( x + (RETROGLU_FONT_W * i) + RETROGLU_FONT_W, y, z );
      glTexCoord2f( 1, 1 );
      glVertex3f(
         x + (RETROGLU_FONT_W * i) + RETROGLU_FONT_W,
         y + RETROGLU_FONT_W, z );

      glTexCoord2f( 1, 1 );
      glVertex3f(
         x + (RETROGLU_FONT_W * i) + RETROGLU_FONT_W,
         y + RETROGLU_FONT_W, z );
      glTexCoord2f( 0, 1 );
      glVertex3f( x + (RETROGLU_FONT_W * i), y + RETROGLU_FONT_W, z );
      glTexCoord2f( 0, 0 );
      glVertex3f( x + (RETROGLU_FONT_W * i), y, z );
      
      glEnd();

#  ifdef RETROGLU_NO_TEXTURE_LISTS
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
         0, 0, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, NULL ); 
#  else
      glBindTexture( GL_TEXTURE_2D, 0 );
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */
   }

#  ifdef RETROGLU_NO_TEXTURE_LISTS
cleanup:
   return;
#  endif /* RETROGLU_NO_TEXTURE_LISTS */
#endif /* !RETROGLU_NO_TEXTURES */
}

#endif /* !RETROFLAT_NO_STRING */

/* === */

MERROR_RETVAL retroglu_check_errors( const char* desc ) {
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETROGLU_NO_ERRORS
   GLenum gl_retval;

   do {
      gl_retval = glGetError();
      if( GL_NO_ERROR != gl_retval ) {
         error_printf( "GL error on %s: 0x%x", desc, gl_retval );
         retval = MERROR_GUI;
      }
   } while( GL_NO_ERROR != gl_retval );
#endif /* !RETROGLU_NO_ERRORS */

   return retval;
}

/* === */

MERROR_RETVAL retroglu_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = RETROFLAT_OK;

   /* TODO: Move this to a common OpenGL header. */

   if(
      NULL != bmp &&
      &(g_retroflat_state->buffer) != bmp &&
      (MAUG_MHANDLE)NULL != bmp->tex.bytes_h
   ) {
      bmp->flags |= RETROFLAT_FLAGS_LOCK;
      maug_mlock( bmp->tex.bytes_h, bmp->tex.bytes );
   }

   return retval;
}

/* === */

MERROR_RETVAL retroglu_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO: Move this to a common OpenGL header. */

   if( NULL == bmp || &(g_retroflat_state->buffer) == bmp ) {
      /* Flush GL buffer and swap screen buffers. */
      glFlush();

#     if defined( RETROFLAT_API_SDL1 )
      SDL_GL_SwapBuffers();
#     elif defined( RETROFLAT_API_SDL2 )
      SDL_GL_SwapWindow( g_retroflat_state->platform.window );
#     elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
      SwapBuffers( g_retroflat_state->platform.hdc_win );
#     elif defined( RETROFLAT_API_GLUT )
      glutSwapBuffers();
#     endif
   } else if( retroflat_bitmap_locked( bmp ) ) {
#ifndef RETROGLU_NO_TEXTURES
      bmp->flags &= ~RETROFLAT_FLAGS_LOCK;
#  ifndef RETROGLU_NO_TEXTURE_LISTS
      assert( 0 < bmp->tex.id );
      assert( NULL != bmp->tex.bytes );

      /* Update stored texture if it exists. */
      glBindTexture( GL_TEXTURE_2D, bmp->tex.id );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, bmp->tex.w, bmp->tex.h, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, bmp->tex.bytes ); 
      glBindTexture( GL_TEXTURE_2D, 0 );
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */

      /* Unlock texture bitmap. */
      maug_munlock( bmp->tex.bytes_h, bmp->tex.bytes );
#endif /* !RETROGLU_NO_TEXTURES */
   }

   return retval;
}

/* === */

MERROR_RETVAL retroglu_load_bitmap(
   const char* filename_path, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETROGLU_NO_TEXTURES
   mfile_t bmp_file;
   struct MFMT_STRUCT_BMPFILE header_bmp;
   MAUG_MHANDLE bmp_palette_h = (MAUG_MHANDLE)NULL;
   uint32_t bmp_color = 0;
   uint32_t* bmp_palette = NULL;
   MAUG_MHANDLE bmp_px_h = (MAUG_MHANDLE)NULL;
   uint8_t* bmp_px = NULL;
   off_t bmp_px_sz = 0;
   uint8_t bmp_r = 0,
      bmp_g = 0,
      bmp_b = 0,
      bmp_color_idx = 0,
      bmp_flags = 0;
   off_t i = 0;

   retval = mfile_open_read( filename_path, &bmp_file );
   maug_cleanup_if_not_ok();

   /* TODO: mfmt file detection system. */
   header_bmp.magic[0] = 'B';
   header_bmp.magic[1] = 'M';
   header_bmp.info.sz = 40;

   retval = mfmt_read_bmp_header(
      (struct MFMT_STRUCT*)&header_bmp,
      &bmp_file, 0, mfile_get_sz( &bmp_file ), &bmp_flags );
   maug_cleanup_if_not_ok();

   assert( 0 < mfile_get_sz( &bmp_file ) );

   /* Setup bitmap options from header. */
   bmp_out->tex.w = header_bmp.info.width;
   bmp_out->tex.h = header_bmp.info.height;
   bmp_out->tex.sz = bmp_out->tex.w * bmp_out->tex.h * 4;
   bmp_out->tex.bpp = 24;

   /* Allocate a space for the bitmap palette. */
   bmp_palette_h = maug_malloc( 4, header_bmp.info.palette_ncolors );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_palette_h );

   maug_mlock( bmp_palette_h, bmp_palette );
   maug_cleanup_if_null_alloc( uint32_t*, bmp_palette );

   retval = mfmt_read_bmp_palette( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_palette, 4 * header_bmp.info.palette_ncolors,
      &bmp_file, 54 /* TODO */, mfile_get_sz( &bmp_file ) - 54, bmp_flags );
   maug_cleanup_if_not_ok();

   /* Allocate a space for the bitmap pixels. */
   bmp_px_sz = header_bmp.info.width * header_bmp.info.height;
   bmp_px_h = maug_malloc( 1, bmp_px_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_px_h );

   maug_mlock( bmp_px_h, bmp_px );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_px );

   retval = mfmt_read_bmp_px( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_px, bmp_px_sz,
      &bmp_file, header_bmp.px_offset,
      mfile_get_sz( &bmp_file ) - header_bmp.px_offset, bmp_flags );
   maug_cleanup_if_not_ok();

   /* Allocate buffer for unpacking. */
   debug_printf( 0, "creating bitmap: " SIZE_T_FMT " x " SIZE_T_FMT,
      bmp_out->tex.w, bmp_out->tex.h );
   bmp_out->tex.bytes_h = maug_malloc( bmp_out->tex.w * bmp_out->tex.h, 4 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_out->tex.bytes_h );

   maug_mlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_out->tex.bytes );

   /* Unpack palletized bitmap into BGRA with color key. */
   for( i = 0 ; bmp_px_sz > i ; i++ ) {
      if( bmp_px_sz - i - 1 > bmp_px_sz ) {
         error_printf(
            "pixel overflow! (" OFF_T_FMT " of " OFF_T_FMT " bytes!)",
            bmp_px_sz - i - 1, bmp_px_sz );
         retval = MERROR_OVERFLOW;
         goto cleanup;
      }

      /* Grab the color from the palette by index. */
      bmp_color_idx = bmp_px[bmp_px_sz - i - 1]; /* Reverse image. */
      if( bmp_color_idx >= header_bmp.info.palette_ncolors ) {
         error_printf(
            "invalid color at px " OFF_T_FMT ": %02x",
            bmp_px_sz - i - 1, bmp_color_idx );
         continue;
      }
      bmp_color = bmp_palette[bmp_color_idx];
      bmp_r = (bmp_color >> 16) & 0xff;
      bmp_g = (bmp_color >> 8) & 0xff;
      bmp_b = bmp_color & 0xff;

      bmp_out->tex.bytes[i * 4] = bmp_r;
      bmp_out->tex.bytes[(i * 4) + 1] = bmp_g;
      bmp_out->tex.bytes[(i * 4) + 2] = bmp_b;
      if(
         RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) &&
         RETROFLAT_TXP_R == bmp_r &&
         RETROFLAT_TXP_G == bmp_g &&
         RETROFLAT_TXP_B == bmp_b
      ) {
         /* Transparent pixel found. */
         bmp_out->tex.bytes[(i * 4) + 3] = 0x00;
      } else {
         bmp_out->tex.bytes[(i * 4) + 3] = 0xff;
      }
   }

#  ifndef RETROGLU_NO_TEXTURE_LISTS
   glGenTextures( 1, (GLuint*)&(bmp_out->tex.id) );
   glBindTexture( GL_TEXTURE_2D, bmp_out->tex.id );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, bmp_out->tex.w, bmp_out->tex.h, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, bmp_out->tex.bytes ); 
   glBindTexture( GL_TEXTURE_2D, 0 );
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */

cleanup:

   if( NULL != bmp_out->tex.bytes ) {
      maug_munlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   }

   if( NULL != bmp_px ) {
      maug_munlock( bmp_px_h, bmp_px );
   }

   if( NULL != bmp_px_h ) {
      maug_mfree( bmp_px_h );
   }

   if( NULL != bmp_palette ) {
      maug_munlock( bmp_palette_h, bmp_palette );
   }

   if( NULL != bmp_palette_h ) {
      maug_mfree( bmp_palette_h );
   }

   mfile_close( &bmp_file );

#endif /* !RETROGLU_NO_TEXTURES */

   return retval;
}

/* === */

MERROR_RETVAL retroglu_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

#ifdef RETROGLU_NO_TEXTURES
   error_printf( "textures not enabled!" );
   retval = MERROR_GUI;
#else

   if( w > 256 ) {
      error_printf( "warning! attempting to create texture with w > 256 ("
         SIZE_T_FMT "). This may not work on Win32!", w );
   }

   if( h > 256 ) {
      error_printf( "warning! attempting to create texture with h > 256 ("
         SIZE_T_FMT "). This may not work on Win32!", h );
   }

   bmp_out->tex.w = w;
   bmp_out->tex.h = h;
   /* TODO: Overflow checking. */
   debug_printf( 0, "creating bitmap: " SIZE_T_FMT " x " SIZE_T_FMT,
      bmp_out->tex.w, bmp_out->tex.h );
   bmp_out->tex.bytes_h =
      maug_malloc( bmp_out->tex.w * bmp_out->tex.h, 4 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_out->tex.bytes_h );

   maug_mlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   maug_cleanup_if_null_lock( uint8_t*, bmp_out->tex.bytes );

   /* TODO: Overflow checking. */
   maug_mzero(
      bmp_out->tex.bytes,
      bmp_out->tex.w * bmp_out->tex.h * sizeof( uint32_t ) );

#     ifndef RETROGLU_NO_TEXTURE_LISTS
   glGenTextures( 1, (GLuint*)&(bmp_out->tex.id) );
   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "assigned bitmap texture: " UPRINTF_U32_FMT, bmp_out->tex.id );
   retval = retroglu_check_errors( "gentextures" );
   maug_cleanup_if_not_ok();
#     endif /* !RETROGLU_NO_TEXTURE_LISTS */

cleanup:
   if( NULL != bmp_out->tex.bytes ) {
      maug_munlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   }

#endif /* !RETROGLU_NO_TEXTURES */

   return retval;
}

/* === */

void retroglu_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {
#ifndef RETROGLU_NO_TEXTURES
   debug_printf( 1, "destroying bitmap..." );

   if( NULL != bmp->tex.bytes ) {
      maug_munlock( bmp->tex.bytes_h, bmp->tex.bytes );
   }
   
   if( NULL != bmp->tex.bytes_h ) {
      maug_mfree( bmp->tex.bytes_h );
   }

#  ifndef RETROGLU_NO_TEXTURE_LISTS
   if( 0 < bmp->tex.id ) {
      debug_printf( 0, 
         "destroying bitmap texture: " UPRINTF_U32_FMT, bmp->tex.id );
      glDeleteTextures( 1, (GLuint*)&(bmp->tex.id) );
   }
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */
#endif /* !RETROGLU_NO_TEXTURES */
}

/* === */

MERROR_RETVAL retroglu_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, size_t d_x, size_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;
   int y_iter = 0;

   /* TODO: Check memcpy, etc? */

   if( NULL == target || retroflat_screen_buffer() == target ) {
      /* TODO: Create ortho sprite on screen. */

   } else {
      /* Blit to texture. */

      assert( NULL != target->tex.bytes );

      /* TODO: Some kind of source-autolock? */
      assert( !retroflat_bitmap_locked( src ) );
      maug_mlock( src->tex.bytes_h, src->tex.bytes );
      for( y_iter = 0 ; h > y_iter ; y_iter++ ) {
         /* TODO: Handle transparency! */
         memcpy(
            &(target->tex.bytes[(((y_iter * target->tex.w) + d_x) * 4)]),
            &(src->tex.bytes[(((y_iter * src->tex.w) + s_x) * 4)]),
            w * 4 );
      }
      maug_munlock( src->tex.bytes_h, src->tex.bytes );

   }

   return retval;
}

/* === */

void retroglu_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags
) {

   if(
      RETROFLAT_FLAGS_BITMAP_RO ==
         (RETROFLAT_FLAGS_BITMAP_RO & target->flags) ||
      target->tex.w <= x ||
      target->tex.h <= y
   ) {
      return;
   }

   assert( NULL != target->tex.bytes );
   /* assert( retroflat_bitmap_locked( target ) ); */

   /* Draw pixel colors from texture palette. */
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 0] =
      g_retroflat_state->tex_palette[color_idx][0];
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 1] =
      g_retroflat_state->tex_palette[color_idx][1];
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 2] =
      g_retroflat_state->tex_palette[color_idx][2];

   /* Set pixel as opaque. */
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 3] = 0xff;
}

#else

#  define RETROFLAT_COLOR_TABLE_GL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   extern MAUG_CONST float RETROGLU_COLOR_ ## name_u[];

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_GL )

#endif /* RETROGLU_C */

/*! \} */ /* maug_retroglu */

#endif /* !RETROGLU_H */

