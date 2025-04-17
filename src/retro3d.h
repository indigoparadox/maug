
#ifndef RETRO3D_H
#define RETRO3D_H

void retro3d_scene_init();

void retro3d_scene_complete();

void retro3d_scene_translate( int x, int y, int z );

void retro3d_scene_rotate( int x, int y, int z );

void retro3d_vx( int x, int y, int z, int s, int t );

void retro3d_tri_begin( uint8_t flags );

void retro3d_tri_end();

#define RETRO3D_TRI_FLAG_NORMAL_X 0x03

#define RETRO3D_TRI_FLAG_NORMAL_Y 0x02

#define RETRO3D_TRI_FLAG_NORMAL_Z 0x01

#define RETRO3D_TRI_FLAG_NORMAL_NEG 0x04

#ifdef RETRO3D_C

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

void retro3d_tri_begin( uint8_t flags ) {
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
   glBegin( GL_TRIANGLES );
}

/* === */

void retro3d_tri_end() {
   glEnd();
}

#endif /* RETRO3D_C */

#endif /* !RETRO3D_H */

