
#ifndef RETAPI3_H
#define RETAPI3_H

#  if defined( RETROFLAT_OPENGL )
#     include <GL/gl.h>
#     include <GL/glu.h>
#  endif /* RETROFLAT_OPENGL */

#define RETRO3D_COLOR_TABLE( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   { (float)((float)r * 1.0f / 255.0f), \
     (float)((float)g * 1.0f / 255.0f), \
     (float)((float)b * 1.0f / 255.0f) },

MAUG_CONST float gc_retro3d_color_table[][3] = { \
RETROFLAT_COLOR_TABLE( RETRO3D_COLOR_TABLE )
};

struct RETROFLAT_BITMAP g_bmp_wtf;

static int gs_tri_vxs_drawn = -1;

void retro3d_init_projection( struct RETRO3D_PROJ_ARGS* args ) {
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
   case RETRO3D_PROJ_FRUSTUM:
      /* This is really tweaky, and when it breaks, polygons seem to get drawn
         * out of order? Still experimenting/researching. */
      debug_printf( RETRO3D_TRACE_LVL, "aspect ratio: %f", aspect_ratio );
      glFrustum(
         /* The smaller these are, the closer it lets us get to the camera? */
         -1.0f * args->rzoom * aspect_ratio, args->rzoom * aspect_ratio,
         -1.0f * args->rzoom, args->rzoom,
         args->near_plane, args->far_plane );
      break;

   case RETRO3D_PROJ_ORTHO:
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

/* === */

MERROR_RETVAL retro3d_platform_init() {
   MERROR_RETVAL retval = MERROR_OK;

   /*
#     define RETROFLAT_COLOR_TABLE_OGL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
        g_retroflat_state->palette[idx] = RETRO3D_COLOR_ ## name_u;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_OGL )
   */

   debug_printf( 1, "setting up texture palette..." );
   /* Use the color indexes defined by retroflat for our internal OpenGL-only
    * color palette.
    */
#     define RETROFLAT_COLOR_TABLE_TEX( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         g_retroflat_state->tex_palette[idx][0] = r; \
         g_retroflat_state->tex_palette[idx][1] = g; \
         g_retroflat_state->tex_palette[idx][2] = b;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_TEX )

   /* TODO: Why do we need to do this or nothing shows up? */
   retroflat_create_bitmap( 16, 16, &g_bmp_wtf, 0 );
   retroflat_draw_lock( &g_bmp_wtf );
   retroflat_draw_release( &g_bmp_wtf );

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

   return retval;
}

/* === */

void retro3d_platform_shutdown() {
   /* Destroy weird texture we need to create so things show up. */
   retroflat_destroy_bitmap( &g_bmp_wtf );
}

/* === */

void retro3d_scene_init() {
   /* Scale down up so glVertex3i operates on 100-basis. */
   glPushMatrix();
   glScalef( MFIX_PRECISION_DIV, MFIX_PRECISION_DIV, MFIX_PRECISION_DIV );
   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

/* === */

void retro3d_scene_complete() {
   glPopMatrix();
   glFlush();
}

/* === */

void retro3d_scene_translate( mfix_t x, mfix_t y, mfix_t z ) {
   glTranslatef( ((float)x), ((float)y), ((float)z) );
}

/* === */

void retro3d_scene_scale( mfix_t x, mfix_t y, mfix_t z ) {
   glScalef( ((float)x), ((float)y), ((float)z) );
}

/* === */

void retro3d_scene_rotate( mfix_t x, mfix_t y, mfix_t z ) {
   if( 0 < x ) {
      glRotatef( x, 1.0f, 0.0f, 0.0f );
   }
   if( 0 < y ) {
      glRotatef( y, 0.0f, 1.0f, 0.0f );
   }
   if( 0 < z ) {
      glRotatef( z, 0.0f, 0.0f, 1.0f );
   }
}

/* === */

void retro3d_vx( mfix_t x, mfix_t y, mfix_t z, mfix_t s, mfix_t t ) {
   assert( 0 <= gs_tri_vxs_drawn );
   assert( 3 > gs_tri_vxs_drawn );
   debug_printf( RETRO3D_TRACE_LVL, "vertex: %d, %d, %d", x, y, z );
   glTexCoord2i( s, t );
   glVertex3i( x, y, z );
}

/* === */

void retro3d_tri_begin( RETROFLAT_COLOR color, uint8_t flags ) {
   const float* fcolor = NULL;

   if( RETROFLAT_COLOR_NULL != color ) {
      fcolor = gc_retro3d_color_table[color];
      retro3d_tri_begin_rgb( fcolor[0], fcolor[1], fcolor[2], flags );
   } else {
      retro3d_tri_begin_rgb( -1.0f, -1.0f, -1.0f, flags );
   }
}

/* === */

void retro3d_tri_begin_rgb( float r, float g, float b, uint8_t flags ) {
   int normal_val = 1;
   assert( 0 > gs_tri_vxs_drawn );
   gs_tri_vxs_drawn = 0;
   debug_printf( RETRO3D_TRACE_LVL, "triangle start!" );
   if( RETRO3D_TRI_FLAG_NORMAL_NEG == (RETRO3D_TRI_FLAG_NORMAL_NEG & flags) ) {
      normal_val *= -1;
   }
   if( RETRO3D_TRI_FLAG_NORMAL_X == (RETRO3D_TRI_FLAG_NORMAL_X & flags) ) {
      glNormal3i( normal_val, 0, 0 );
   } else if(
      RETRO3D_TRI_FLAG_NORMAL_Y == (RETRO3D_TRI_FLAG_NORMAL_Y & flags)
   ) {
      glNormal3i( 0, normal_val, 0 );
   } else if(
      RETRO3D_TRI_FLAG_NORMAL_Z == (RETRO3D_TRI_FLAG_NORMAL_Z & flags)
   ) {
      glNormal3i( 0, 0, normal_val );
   }
   if( 0 <= r ) {
      glColor3f( r, g, b );
   }
   glBegin( GL_TRIANGLES );
}

/* === */

void retro3d_tri_end() {
   assert( 0 <= gs_tri_vxs_drawn );
   assert( 3 > gs_tri_vxs_drawn );
   debug_printf( RETRO3D_TRACE_LVL, "triangle end!" );
   gs_tri_vxs_drawn = -1;
   glEnd();
}

/* === */

MERROR_RETVAL retro3d_texture_platform_refresh(
   struct RETROFLAT_BITMAP* bmp, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

#  ifndef RETRO3D_NO_TEXTURE_LISTS
   if( RETRO3D_TEX_FLAG_DESTROY == (RETRO3D_TEX_FLAG_DESTROY & flags) ) {
      glDeleteTextures( 1, (GLuint*)&(bmp->tex.id) );
   }

   /* Update stored texture if it exists. */
   if( RETRO3D_TEX_FLAG_GENERATE == (RETRO3D_TEX_FLAG_GENERATE & flags) ) {
      glGenTextures( 1, (GLuint*)&(bmp->tex.id) );
   }
   glBindTexture( GL_TEXTURE_2D, bmp->tex.id );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, bmp->tex.w, bmp->tex.h, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, bmp->tex.bytes ); 
   glBindTexture( GL_TEXTURE_2D, 0 );
#  endif /* !RETRO3D_NO_TEXTURE_LISTS */

   return retval;
}

/* === */

MERROR_RETVAL retro3d_check_errors( const char* desc ) {
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETRO3D_NO_ERRORS
   GLenum gl_retval;

   do {
      gl_retval = glGetError();
      if( GL_NO_ERROR != gl_retval ) {
         error_printf( "GL error on %s: 0x%x", desc, gl_retval );
         retval = MERROR_GUI;
      }
   } while( GL_NO_ERROR != gl_retval );
#endif /* !RETRO3D_NO_ERRORS */

   return retval;
}

#endif /* !RETAPI3_H */

