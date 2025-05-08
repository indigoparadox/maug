
#ifndef RETAPI3_H
#define RETAPI3_H

#include <ddraw.h>
#include <d3d.h>

LPDIRECT3D3 lpD3D = NULL;
LPDIRECT3DDEVICE3 lpD3DDevice = NULL;
LPDIRECT3DVIEWPORT3 lpViewport = NULL;

void retro3d_init_projection( struct RETRO3D_PROJ_ARGS* args ) {
}

/* === */

void retro3d_scene_init_bg(
   RETROFLAT_COLOR color, mfix_t fog_draw_dist, mfix_t fog_density
) {
}

/* === */

MERROR_RETVAL retro3d_platform_init() {
   MERROR_RETVAL retval = MERROR_OK;

   return retval;
}

/* === */

void retro3d_platform_shutdown() {
}

/* === */

void retro3d_scene_init() {
}

/* === */

void retro3d_scene_complete() {
}

/* === */

void retro3d_scene_open_node() {
}

/* === */

void retro3d_scene_close_node() {
}

/* === */

void retro3d_scene_translate( mfix_t x, mfix_t y, mfix_t z ) {
}

/* === */

void retro3d_scene_scale( mfix_t x, mfix_t y, mfix_t z ) {
}

/* === */

void retro3d_scene_rotate( mfix_t x, mfix_t y, mfix_t z ) {
}

/* === */

void retro3d_vx( mfix_t x, mfix_t y, mfix_t z, mfix_t s, mfix_t t ) {
}

/* === */

void retro3d_tri_begin( RETROFLAT_COLOR color, uint8_t flags ) {
}

/* === */

void retro3d_tri_begin_rgb( float r, float g, float b, uint8_t flags ) {
}

/* === */

void retro3d_tri_end() {
}

/* === */

MERROR_RETVAL retro3d_draw_window(
   retroflat_blit_t* win, retroflat_pxxy_t x_px, retroflat_pxxy_t y_px
) {
   MERROR_RETVAL retval = MERROR_OK;

   return retval;
}

/* === */

MERROR_RETVAL retro3d_texture_activate(
   retroflat_blit_t* tex, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   return retval;
}

/* === */

MERROR_RETVAL retro3d_texture_platform_refresh(
   retroflat_blit_t* tex, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   return retval;
}

/* === */

MERROR_RETVAL retro3d_check_errors( const char* desc ) {
   MERROR_RETVAL retval = MERROR_OK;

   return retval;
}


#endif /* !RETAPI3_H */

