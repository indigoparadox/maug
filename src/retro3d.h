
#ifndef RETRO3D_H
#define RETRO3D_H

/**
 * \addtogroup maug_retro3d Retro3D API
 */

/**
 * \file retro3d.h
 * \brief Lower-level retargetable 3D engine wrapper.
 */

/**
 * \brief Flag for retro3d_texture_platform_refresh() indicating the platform
 *        should generate a new texture, rather than updating the existing.
 */
#define RETRO3D_TEX_FLAG_GENERATE 0x01

#define RETRO3D_TEX_FLAG_DESTROY 0x02

MERROR_RETVAL retro3d_platform_init();

void retro3d_platform_shutdown();

void retro3d_scene_init();

void retro3d_scene_complete();

void retro3d_scene_translate( int x, int y, int z );

void retro3d_scene_rotate( int x, int y, int z );

/**
 * \brief Create a vertex at the designated location.
 */
void retro3d_vx( int x, int y, int z, int s, int t );

void retro3d_tri_begin( RETROFLAT_COLOR color, uint8_t flags );

void retro3d_tri_end();

/**
 * \brief Perform engine-specific refresh actions on the texture.
 * \warning This should be called by the low-level Retro3D API and shouldn't
 *          need to be called directly by programs using this library!
 */
MERROR_RETVAL retro3d_texture_platform_refresh(
   struct RETROFLAT_BITMAP* bmp, uint8_t flags );

MERROR_RETVAL retro3d_check_errors( const char* desc );

#define RETRO3D_TRI_FLAG_NORMAL_X 0x03

#define RETRO3D_TRI_FLAG_NORMAL_Y 0x02

#define RETRO3D_TRI_FLAG_NORMAL_Z 0x01

#define RETRO3D_TRI_FLAG_NORMAL_NEG 0x04

/*! \} */ /* maug_retro3d */

#endif /* !RETRO3D_H */

