
#ifndef RETRO3D_H
#define RETRO3D_H

MERROR_RETVAL retro3d_platform_init();

void retro3d_platform_shutdown();

void retro3d_scene_init();

void retro3d_scene_complete();

void retro3d_scene_translate( int x, int y, int z );

void retro3d_scene_rotate( int x, int y, int z );

void retro3d_vx( int x, int y, int z, int s, int t );

void retro3d_tri_begin( RETROFLAT_COLOR color, uint8_t flags );

void retro3d_tri_end();

MERROR_RETVAL retro3d_check_errors( const char* desc );

MERROR_RETVAL retro3d_texture_lock( struct RETROFLAT_BITMAP* bmp );

MERROR_RETVAL retro3d_texture_release( struct RETROFLAT_BITMAP* bmp );

#define RETRO3D_TRI_FLAG_NORMAL_X 0x03

#define RETRO3D_TRI_FLAG_NORMAL_Y 0x02

#define RETRO3D_TRI_FLAG_NORMAL_Z 0x01

#define RETRO3D_TRI_FLAG_NORMAL_NEG 0x04

#ifdef RETRO3D_C

#endif /* RETRO3D_C */

#endif /* !RETRO3D_H */

