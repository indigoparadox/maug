
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

MERROR_RETVAL retro3d_platform_init() {
   MERROR_RETVAL retval = MERROR_OK;

   /*
#     define RETROFLAT_COLOR_TABLE_OGL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
        g_retroflat_state->palette[idx] = RETROGLU_COLOR_ ## name_u;
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
   glScalef( 0.01, 0.01, 0.01 );
   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

/* === */

void retro3d_scene_complete() {
   glPopMatrix();
   glFlush();
}

/* === */

void retro3d_scene_translate( int x, int y, int z ) {
   glTranslatef( ((float)x), ((float)y), ((float)z) );
}

/* === */

void retro3d_scene_rotate( int x, int y, int z ) {
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

void retro3d_vx( int x, int y, int z, int s, int t ) {
   glTexCoord2i( s, t );
   glVertex3i( x, y, z );
}

/* === */

void retro3d_tri_begin( RETROFLAT_COLOR color, uint8_t flags ) {
   int normal_val = 1;
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
   if( RETROFLAT_COLOR_NULL != color ) {
      glColor3fv( gc_retro3d_color_table[color] );
   }
   glBegin( GL_TRIANGLES );
}

/* === */

void retro3d_tri_end() {
   glEnd();
}

/* === */

MERROR_RETVAL retro3d_texture_platform_refresh(
   struct RETROFLAT_BITMAP* bmp, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

#  ifndef RETROGLU_NO_TEXTURE_LISTS
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
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */

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

