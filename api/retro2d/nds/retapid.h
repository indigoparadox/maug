
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#define RETROGUI_NO_TEXTBOX

#define RETROFLAT_NDS_FLAG_BMP_LOCKED 0x20

#define RETROFLAT_NDS_FLAG_CHANGE_BG   0x02

#define RETROFLAT_NO_CLI_SZ

/* == Nintendo DS == */

#  include <nds.h>

#  define RETROCON_DISABLE 1

#ifndef RETROFLAT_NDS_OAM_ACTIVE
/*! \brief Active sprite engine screen on Nintendo DS. */
#  define RETROFLAT_NDS_OAM_ACTIVE &oamMain
#endif /* !RETROFLAT_NDS_OAM_ACTIVE */

#ifndef RETROFLAT_NDS_SPRITES_ACTIVE
/*! \brief Maximum number of sprites active on-screen on Nintendo DS. */
#  define RETROFLAT_NDS_SPRITES_ACTIVE 200
#endif /* !RETROFLAT_NDS_SPRITES_ACTIVE */

#ifndef RETROFLAT_PLATFORM_TRACE_LVL
#  define RETROFLAT_PLATFORM_TRACE_LVL 0
#endif /* !RETROFLAT_PLATFORM_TRACE_LVL */

#ifndef RETROFLAT_NDS_VOLUME
#  define RETROFLAT_NDS_VOLUME 32
#endif /* !RETROFLAT_NDS_VOLUME */

#define RETROFLAT_NDS_BG_W_TILES 32

/* Determined by configurator:
 * https://mtheall.com/vram.html#T0=1&NT0=256&MB0=2&TB0=1&S0=0&T1=1&NT1=64&MB1=4&TB1=0&S1=0&T2=6&MB2=2&S2=2
 *
 * Should support <= 256 tiles on BG0 and <=64 tiles on BG1.
 */

#define RETROFLAT_NDS_BG0_MAP          2
#define RETROFLAT_NDS_BG0_MAP_BASE     (uint16_t*)0x06001000;
#define RETROFLAT_NDS_BG0_MAP_ALT      3
#define RETROFLAT_NDS_BG0_MAP_BASE_ALT (uint16_t*)0x06001800;
#define RETROFLAT_NDS_BG0_TILE         2
#define RETROFLAT_NDS_BG1_MAP          4
#define RETROFLAT_NDS_BG1_TILE         0
#define RETROFLAT_NDS_BG2_MAP          3
#define RETROFLAT_NDS_BG2_TILE         0

#  define RETROGLU_NO_TEXTURES
#  define RETROGLU_NO_ERRORS
#  define RETROGLU_NO_LISTS
#  define RETROGLU_NO_LIGHTING
#  define RETROGLU_NO_FOG

typedef int GLint;
#  define glPopMatrix() glPopMatrix( 1 )
#  define glFlush() glFlush( 0 )
#  define glPolygonMode( sides, mode )
#  define glClear( bits )
#  define glTexCoord2fv( arr )
#  define glTexCoord2i( x, y )
#  define glMaterialfv( side, light, rgb ) \
      glMaterialf( light, RGB15( (int)rgb[0], (int)rgb[1], (int)rgb[2] ) )
#  define glShininessf( side, light, f ) glMaterialf( light, f )
#  define glColor3fv( rgb ) glColor3f( rgb[0], rgb[1], rgb[2] )
#  define glVertex2fv( xy ) glVertex3f( xy[0], xy[1], 0 )
#  define glNormal3i( x, y, z )

/* NDS doesn't have primitives. */
#  ifndef RETROFLAT_SOFT_SHAPES
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

#  ifndef RETROFLAT_SOFT_LINES
#     define RETROFLAT_SOFT_LINES
#  endif /* !RETROFLAT_SOFT_LINES */

#  define RETROFLAT_NDS_BG_TILE_W_PX 8
#  define RETROFLAT_NDS_BG_TILE_H_PX 8
#  define RETROFLAT_NDS_BG_W_TILES 32

#define RETROFLAT_MS_FMT "%lu"

/* Inline pass to generic loop. */
#define retroflat_loop( frame_iter, loop_iter, data ) \
   retroflat_loop_generic( frame_iter, loop_iter, data )

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   uint16_t* b;
   unsigned int* tiles;
   unsigned short* pal;
   size_t tiles_len;
   size_t pal_len;
   size_t w;
   size_t h;
#  ifdef RETROFLAT_OPENGL
   struct RETROFLAT_GLTEX tex;
#  endif /* RETROFLAT_OPENGL */
};

typedef int RETROFLAT_COLOR_DEF;

/* TODO */
#  define retroflat_bitmap_locked( bmp ) \
   (RETROFLAT_NDS_FLAG_BMP_LOCKED == \
      ((bmp)->flags & RETROFLAT_NDS_FLAG_BMP_LOCKED))
#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )
#  ifdef RETROFLAT_VDP
#     define retroflat_vdp_lock( bmp )
#     define retroflat_vdp_release( bmp )
#  endif /* RETROFLAT_VDP */

#  define retroflat_screen_w() (256)
#  define retroflat_screen_h() (192)
#  define retroflat_screen_buffer() \
      (&(g_retroflat_state->platform.screen_buffer))
#  define retroflat_root_win() (NULL) /* TODO */

#  define END_OF_MAIN()

/* TODO? */
#  define retroflat_quit( retval_in )
#  define retroflat_bitmap_w( bmp ) ((bmp)->w)
#  define retroflat_bitmap_h( bmp ) ((bmp)->h)
#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->tiles)

#  define _retroflat_nds_platform_flag( flag ) \
   (RETROFLAT_NDS_FLAG_ ## flag == \
      (RETROFLAT_NDS_FLAG_ ## flag & g_retroflat_state->platform.flags))

struct RETROFLAT_PLATFORM_ARGS {
   uint8_t flags;
};

struct RETROFLAT_PLATFORM {
   uint8_t              flags;
   struct RETROFLAT_BITMAP screen_buffer;
   uint16_t*            sprite_frames[RETROFLAT_NDS_SPRITES_ACTIVE];
   struct RETROFLAT_BITMAP* oam_entries[RETROFLAT_NDS_SPRITES_ACTIVE];
   int16_t              oam_dx[RETROFLAT_NDS_SPRITES_ACTIVE];
   int16_t              oam_dy[RETROFLAT_NDS_SPRITES_ACTIVE];
   int16_t              oam_sx[RETROFLAT_NDS_SPRITES_ACTIVE];
   int16_t              oam_sy[RETROFLAT_NDS_SPRITES_ACTIVE];
   struct RETROFLAT_BITMAP* bg_bmp;
   int                  bg_id;
   int                  bg0_map_base;
   int                  window_id;
   int                  px_id;
   uint16_t             bg_map[1024];
   uint16_t             window_tiles[1024];
   int scroll_x;
   int scroll_y;
};

#  define retroflat_system_task() pmMainLoop();

#endif /* !RETPLTD_H */

