
#ifndef RETROGLU_H
#define RETROGLU_H

/* TODO */
#ifndef MAUG_CONST
#  define MAUG_CONST const
#endif /* !MAUG_CONST */

/**
 * \addtogroup maug_retroglu RetroGLU API
 * \brief Library of tools for working with RetroFlat and OpenGL.
 * \warning This API was a quick and dirty helper for OpenGL and is deprecated.
 * \{
 *
 * \file retroglu.h
 * \brief Library of tools for working with RetroFlat and OpenGL.
 */

#ifndef RETROGLU_TRACE_LVL
#  define RETROGLU_TRACE_LVL 0
#endif /* !RETROGLU_TRACE_LVL */

#ifndef RETROGLU_SPRITE_TEX_FRAMES_SZ
#  define RETROGLU_SPRITE_TEX_FRAMES_SZ 10
#endif /* !RETROGLU_SPRITE_TEX_FRAMES_SZ */

#ifndef MAUG_OS_NDS
#  define glShininessf( side, light, f ) glMaterialf( side, light, f )
#endif /* MAUG_OS_NDS */

typedef float RETROGLU_COLOR[4];

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
   int32_t lists[RETROGLU_SPRITE_LIST_SZ_MAX];
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

void retroglu_init_scene( uint8_t flags );
void retroglu_init_projection( struct RETROGLU_PROJ_ARGS* args );

MERROR_RETVAL retroglu_draw_poly( struct RETRO3DP_OBJ* obj );

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

void retroglu_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags );

#ifdef RETROGLU_C

#  define RETROFLAT_COLOR_TABLE_GL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   MAUG_CONST float RETROGLU_COLOR_ ## name_u[] = { \
      (float)((float)r * 1.0f / 255.0f), \
      (float)((float)g * 1.0f / 255.0f), \
      (float)((float)b * 1.0f / 255.0f) };

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_GL )

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

MERROR_RETVAL retroglu_draw_poly( struct RETRO3DP_OBJ* obj ) {
   MERROR_RETVAL retval = MERROR_OK;
   int i = 0;
   int j = 0;
   struct RETRO3DP_MATERIAL* m = NULL;
   struct RETRO3DP_FACE* f = NULL;
   struct RETRO3DP_VERTEX* v = NULL;

   debug_printf( RETROGLU_TRACE_LVL, "drawing poly..." );

   /* Compensate for parsed integer model. */
   glPushMatrix();
   glScalef( 0.001, 0.001, 0.001 );

   glBegin( GL_TRIANGLES );
   debug_printf( RETROGLU_TRACE_LVL, "locking faces..." );
   mdata_vector_lock( &(obj->faces) );
   debug_printf( RETROGLU_TRACE_LVL, "locking materials..." );
   mdata_vector_lock( &(obj->materials) );
   debug_printf( RETROGLU_TRACE_LVL, "locking vertices..." );
   mdata_vector_lock( &(obj->vertices) );
   debug_printf( RETROGLU_TRACE_LVL, "locking normals..." );
   mdata_vector_lock( &(obj->vnormals) );
   for( i = 0 ; mdata_vector_ct( &(obj->faces) ) > i ; i++ ) {
      debug_printf( RETROGLU_TRACE_LVL,
         "getting face %d of " SIZE_T_FMT "...",
         i, mdata_vector_ct( &(obj->faces) ) );
      f = mdata_vector_get( &(obj->faces), i, struct RETRO3DP_FACE );
      assert( NULL != f );
      if( 0 < mdata_vector_ct( &(obj->materials) ) ) {
         debug_printf(
            RETROGLU_TRACE_LVL, "getting material %d of " SIZE_T_FMT "...",
            f->material_idx, mdata_vector_ct( &(obj->materials) ) );
         m = mdata_vector_get(
            &(obj->materials), f->material_idx, struct RETRO3DP_MATERIAL );
         assert( NULL != m );
      
         /* TODO: Handle material on NDS. */
         glMaterialfv( GL_FRONT, GL_DIFFUSE, m->diffuse );
            /*
         glMaterialfv( GL_FRONT, GL_AMBIENT,
            obj->materials[faces[i].material_idx].ambient );
            */
         glMaterialfv( GL_FRONT, GL_SPECULAR, m->specular );
         glMaterialfv( GL_FRONT, GL_EMISSION, m->emissive );

         glColor3fv( m->diffuse );

         /* Use a specific macro here that can be overridden for e.g. the NDS.
          */
         glShininessf( GL_FRONT, GL_SHININESS, m->specular_exp );
      }

      for( j = 0 ; f->vertex_idxs_sz > j ; j++ ) {
         assert( 0 < f->vertex_idxs[j] );
         assert( 3 == f->vertex_idxs_sz );

         if( 0 < mdata_vector_ct( &(obj->vnormals) ) ) {
            debug_printf( RETROGLU_TRACE_LVL,
               "getting normal %d of " SIZE_T_FMT "...",
               j, mdata_vector_ct( &(obj->vnormals) ) );
            v = mdata_vector_get(
               &(obj->vnormals), f->vnormal_idxs[j] - 1,
               struct RETRO3DP_VERTEX );
            assert( NULL != v );

            glNormal3i( v->x, v->y, v->z );
         }

         debug_printf( RETROGLU_TRACE_LVL,
            "getting vertex %d of " SIZE_T_FMT "...",
            j, mdata_vector_ct( &(obj->vertices) ) );
         v = mdata_vector_get(
            &(obj->vertices), f->vertex_idxs[j] - 1, struct RETRO3DP_VERTEX );
         assert( NULL != v );
         glVertex3i( v->x, v->y, v->z );
      }

   }

   debug_printf( RETROGLU_TRACE_LVL, "drawing complete!" );

cleanup:

   mdata_vector_unlock( &(obj->vnormals) );
   mdata_vector_unlock( &(obj->faces) );
   mdata_vector_unlock( &(obj->materials) );
   mdata_vector_unlock( &(obj->vertices) );

   glEnd();

   glPopMatrix();

   return retval;
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

#else

#  define RETROFLAT_COLOR_TABLE_GL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   extern MAUG_CONST float RETROGLU_COLOR_ ## name_u[];

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_GL )

#endif /* RETROGLU_C */

/*! \} */ /* maug_retroglu */

#endif /* !RETROGLU_H */

