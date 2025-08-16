
#ifndef RETRO3D_H
#define RETRO3D_H

/**
 * \addtogroup maug_retroflt
 * \{
 * \addtogroup maug_retro3d Retro3D API
 * \{
 */

#ifndef RETRO3D_TRACE_LVL
#  define RETRO3D_TRACE_LVL 0
#endif /* RETRO3D_TRACE_LVL */

/**
 * \file retro3d.h
 * \brief Lower-level retargetable 3D engine wrapper.
 */

struct RETRO3D_PROJ_ARGS {
   /**
    * \brief The desired projection method. Can be RETRO3D_PROJ_ORTHO or
    *        RETRO3D_PROJ_FRUSTUM.
    */
   uint8_t proj;
   float rzoom;
   float near_plane;
   float far_plane;
   size_t screen_px_w;
   size_t screen_px_h;
};

/**
 * \addtogroup maug_retro3d_tex
 * \{
 */

/**
 * \brief Flag for retro3d_texture_platform_refresh() indicating the platform
 *        should generate a new texture, rather than updating the existing.
 */
#define RETRO3D_TEX_FLAG_GENERATE 0x01

/**
 * \brief Flag for retro3d_texture_platform_refresh() indicating the platform
 *        should destroy a texture, rather than updating the existing.
 */
#define RETRO3D_TEX_FLAG_DESTROY 0x02

/**
 * \brief Flag for retro3d_texture_activate() indicating the platform should
 *        deactivate the current texture, rather than activating a new one.
 */
#define RETRO3D_TEX_FLAG_DEACTIVATE 0x04

/*! \} */ /* maug_retro3d_tex */

/**
 * \relates RETRO3D_PROJ_ARGS
 * \brief When provided to RETRO3D_PROJ_ARGS::proj, specifies orthographic
 *        projection.
 */
#define RETRO3D_PROJ_ORTHO 0

/**
 * \relates RETRO3D_PROJ_ARGS
 * \brief When provided to RETRO3D_PROJ_ARGS::proj, specifies frustum
 *        projection.
 */
#define RETRO3D_PROJ_FRUSTUM 1

/**
 * \brief Setup projection properties to subsequently drawn scenes/frames.
 */
void retro3d_init_projection( struct RETRO3D_PROJ_ARGS* args );

/**
 * \brief Setup background and fog (when supported) for subsequently drawn 
 *        scenes.
 *
 * \note This does NOT clear the current scene!
 */
void retro3d_init_bg(
   RETROFLAT_COLOR color, mfix_t fog_draw_dist, mfix_t fog_density );

MERROR_RETVAL retro3d_platform_init();

void retro3d_platform_shutdown();

/**
 * \addtogroup maug_retro3d_scene Retro3D Scene Graph
 * \brief Basic 3D scene graph API.
 * \{
 */

/**
 * \brief Commence drawing a new scene/frame.
 */
void retro3d_scene_init();

/**
 * \brief Finish drawing the current scene/frame and display it on-screen.
 * \return The number of polys drawn for this scene/frame.
 */
size_t retro3d_scene_complete();

/**
 * \brief Denote the beginning of a new scene node, or group of polygons inside
 *        the current scene/frame.
 *
 * This can be useful for e.g. applying \ref maug_retro3d_trans to a group
 * of polygons, like a spinning waterwheel or a character's moving arm.
 */
void retro3d_scene_open_node();

/**
 * \brief Denote the end of the current scene node opened with
 *        retro3d_scene_open_node().
 */
void retro3d_scene_close_node();

/*! \} */

/**
 * \addtogroup maug_retro3d_trans Retro3D Transformations
 * \brief Functions for translating/transforming the current scene node.
 * \{
 */

void retro3d_scene_translate( mfix_t x, mfix_t y, mfix_t z );

void retro3d_scene_scale( mfix_t x, mfix_t y, mfix_t z );

/**
 * \brief Rotate the current scene node the given number of degrees on the X,
 *        Y, or Z axis.
 */
void retro3d_scene_rotate( mfix_t x, mfix_t y, mfix_t z );

/*! \} */ /* maug_retro3d_trans */

/**
 * \addtogroup maug_retro3d_poly Retro3D Polygons
 * \brief Functions for drawing polygons inside of the current scene.
 * \{
 */

/**
 * \brief Create a vertex at the designated location. Must be called inside
 *        of retro3d_tri_begin() and retro3d_tri_end().
 */
void retro3d_vx( mfix_t x, mfix_t y, mfix_t z, mfix_t s, mfix_t t );

/**
 * \brief Begins drawing a polygon/triangle using predefined
 *        \ref maug_retroflt_color. Follow with three calls to retro3d_vx()
 *        and a call to retro3d_tri_end().
 */
void retro3d_tri_begin( RETROFLAT_COLOR color, uint8_t flags );

/**
 * \brief Begins drawing a polygon/triangle with the given custom RGB color.
 *        Follow with three calls to retro3d_vx() and a call to
 *        retro3d_tri_end().
 */
void retro3d_tri_begin_rgb( float r, float g, float b, uint8_t flags );

/**
 * \brief Complete a triangle opened with retro3d_tri_begin(). This MUST be
 *        called after drawing three vertices.
 */
void retro3d_tri_end();

/*! \} */ /* maug_retro3d_poly */

/**
 * \addtogroup maug_retro3d_tex Retro3D Textures and Sprites
 * \brief Functions for drawing textured polygons and billboarded sprites.
 * \{
 */

/**
 * \brief Draw a bitmap 2D "window" in ortho view proportionally on screen.
 *        (e.g. for displaying a UI.)
 */
MERROR_RETVAL retro3d_draw_window(
   retroflat_blit_t* win, retroflat_pxxy_t x, retroflat_pxxy_t y );

/**
 * \brief Activates the given texture so the next polygon will be drawn with it.
 * \param flags Optionally accepts RETRO3D_TEX_FLAG_DEACTIVATE.
 */
MERROR_RETVAL retro3d_texture_activate( retroflat_blit_t* tex, uint8_t flags );

/**
 * \brief Perform engine-specific refresh actions on the texture.
 * \param flags Optionally accepts RETRO3D_TEX_FLAG_GENERATE or
 *              RETRO3D_TEX_FLAG_DESTROY.
 * \warning This should be called by the low-level Retro3D API and shouldn't
 *          need to be called directly by programs using this library!
 */
MERROR_RETVAL retro3d_texture_platform_refresh(
   retroflat_blit_t* tex, uint8_t flags );

/*! \} */ /* maug_retro3d_tex */

MERROR_RETVAL retro3d_check_errors( const char* desc );

#define RETRO3D_TRI_FLAG_NORMAL_X 0x03

#define RETRO3D_TRI_FLAG_NORMAL_Y 0x02

#define RETRO3D_TRI_FLAG_NORMAL_Z 0x01

#define RETRO3D_TRI_FLAG_NORMAL_NEG 0x04

/*! \} */ /* maug_retro3d */

/*! \} */ /* maug_retroflt */

#endif /* !RETRO3D_H */

