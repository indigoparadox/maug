
#ifndef RETRO3D_H
#define RETRO3D_H

/**
 * \addtogroup maug_retro3d Retro3D API
 */

#ifndef RETRO3D_TRACE_LVL
#  define RETRO3D_TRACE_LVL 0
#endif /* RETRO3D_TRACE_LVL */

struct RETRO3D_PROJ_ARGS {
   uint8_t proj;
   float rzoom;
   float near_plane;
   float far_plane;
   size_t screen_px_w;
   size_t screen_px_h;
};

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

#define RETRO3D_TEX_FLAG_DEACTIVATE 0x04

#define RETRO3D_PROJ_ORTHO 0

#define RETRO3D_PROJ_FRUSTUM 1

void retro3d_init_projection( struct RETRO3D_PROJ_ARGS* args );

void retro3d_scene_init_bg(
   RETROFLAT_COLOR color, mfix_t fog_draw_dist, mfix_t fog_density );

MERROR_RETVAL retro3d_platform_init();

void retro3d_platform_shutdown();

void retro3d_scene_init();

void retro3d_scene_complete();

void retro3d_scene_open_node();

void retro3d_scene_close_node();

void retro3d_scene_translate( mfix_t x, mfix_t y, mfix_t z );

void retro3d_scene_scale( mfix_t x, mfix_t y, mfix_t z );

void retro3d_scene_rotate( mfix_t x, mfix_t y, mfix_t z );

/**
 * \brief Create a vertex at the designated location.
 */
void retro3d_vx( mfix_t x, mfix_t y, mfix_t z, mfix_t s, mfix_t t );

void retro3d_tri_begin( RETROFLAT_COLOR color, uint8_t flags );

void retro3d_tri_begin_rgb( float r, float g, float b, uint8_t flags );

void retro3d_tri_end();

/**
 * \brief Draw a bitmap 2D "window" in ortho view proportionally on screen.
 *        (e.g. for displaying a UI.)
 */
MERROR_RETVAL retro3d_draw_window(
   retroflat_blit_t* win, retroflat_pxxy_t x, retroflat_pxxy_t y );

MERROR_RETVAL retro3d_texture_activate( retroflat_blit_t* tex, uint8_t flags );

/**
 * \brief Perform engine-specific refresh actions on the texture.
 * \warning This should be called by the low-level Retro3D API and shouldn't
 *          need to be called directly by programs using this library!
 */
MERROR_RETVAL retro3d_texture_platform_refresh(
   retroflat_blit_t* tex, uint8_t flags );

MERROR_RETVAL retro3d_check_errors( const char* desc );

#define RETRO3D_TRI_FLAG_NORMAL_X 0x03

#define RETRO3D_TRI_FLAG_NORMAL_Y 0x02

#define RETRO3D_TRI_FLAG_NORMAL_Z 0x01

#define RETRO3D_TRI_FLAG_NORMAL_NEG 0x04

/*! \} */ /* maug_retro3d */

#endif /* !RETRO3D_H */

