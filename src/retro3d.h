
#ifndef RETRO3D_H
#define RETRO3D_H

void retro3d_init();

void retro3d_vx( int x, int y, int z, int s, int t );

#ifdef RETRO3D_C

void retro3d_init() {
   /* Scale down up so glVertex3i operates on 1000-basis. */
   glScalef( 0.001, 0.001, 0.001 );

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

/* === */

void retro3d_vx( int x, int y, int z, int s, int t ) {
   glTexCoord2i( s, t );
   glVertex3i( x, y, z );
}

#endif /* RETRO3D_C */

#endif /* !RETRO3D_H */

