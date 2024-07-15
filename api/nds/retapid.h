
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

#define RETROFLAT_SOFT_VIEWPORT

/* == Nintendo DS == */

#  include <nds.h>

#  define RETROCON_DISABLE 1

#define retroflat_wait_for_vblank() swiWaitForVBlank();

#ifndef RETROFLAT_NDS_OAM_ACTIVE
/*! \brief Active sprite engine screen on Nintendo DS. */
#  define RETROFLAT_NDS_OAM_ACTIVE &oamMain
#endif /* !RETROFLAT_NDS_OAM_ACTIVE */

#ifndef RETROFLAT_NDS_SPRITES_ACTIVE
/*! \brief Maximum number of sprites active on-screen on Nintendo DS. */
#  define RETROFLAT_NDS_SPRITES_ACTIVE 24
#endif /* !RETROFLAT_NDS_SPRITES_ACTIVE */

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

typedef int16_t RETROFLAT_IN_KEY;

#define RETROFLAT_MS_FMT "%lu"

/* Inline pass to generic loop. */
#define retroflat_loop( frame_iter, loop_iter, data ) \
   retroflat_loop_generic( frame_iter, loop_iter, data )

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   uint16_t* b;
   const unsigned int* tiles;
   const unsigned short* pal;
   size_t tiles_len;
   size_t pal_len;
#  ifdef RETROFLAT_OPENGL
   struct RETROFLAT_GLTEX tex;
   ssize_t w;
   ssize_t h;
#  endif /* RETROFLAT_OPENGL */
};

typedef int RETROFLAT_COLOR_DEF;

#  ifdef RETROFLAT_NDS_WASD
#     define RETROFLAT_KEY_A           KEY_LEFT
#     define RETROFLAT_KEY_D           KEY_RIGHT
#     define RETROFLAT_KEY_W           KEY_UP
#     define RETROFLAT_KEY_S           KEY_DOWN
#     define RETROFLAT_KEY_Q           KEY_L
#     define RETROFLAT_KEY_E           KEY_R
#  else
#     define RETROFLAT_KEY_LEFT        KEY_LEFT
#     define RETROFLAT_KEY_RIGHT       KEY_RIGHT
#     define RETROFLAT_KEY_UP          KEY_UP
#     define RETROFLAT_KEY_DOWN        KEY_DOWN
#     define RETROFLAT_KEY_INSERT      KEY_L
#     define RETROFLAT_KEY_DELETE      KEY_R
#  endif /* RETROFLAT_NDS_WASD */
#  define RETROFLAT_KEY_ENTER       KEY_START
#  define RETROFLAT_KEY_SPACE       KEY_A
#  define RETROFLAT_KEY_ESC         KEY_B
#  define RETROFLAT_MOUSE_B_LEFT    (-1)
#  define RETROFLAT_MOUSE_B_RIGHT   (-2)

#define retroflat_nds_buttons( f ) \
   f( RETROFLAT_KEY_LEFT ) \
   f( RETROFLAT_KEY_RIGHT ) \
   f( RETROFLAT_KEY_UP ) \
   f( RETROFLAT_KEY_DOWN ) \
   f( RETROFLAT_KEY_ENTER ) \
   f( RETROFLAT_KEY_SPACE ) \
   f( RETROFLAT_KEY_ESC )

/* TODO */
#  define retroflat_bitmap_locked( bmp ) (0)
#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )
#  ifdef RETROFLAT_VDP
#     define retroflat_vdp_lock( bmp )
#     define retroflat_vdp_release( bmp )
#  endif /* RETROFLAT_VDP */

#  define retroflat_screen_w() (256)
#  define retroflat_screen_h() (192)
#  define retroflat_screen_buffer() (&(g_retroflat_state->buffer))
#  define retroflat_root_win() (NULL) /* TODO */

#  define END_OF_MAIN()

/* TODO? */
#  define retroflat_quit( retval_in )
#  define retroflat_bitmap_w( bmp ) (0)
#  define retroflat_bitmap_h( bmp ) (0)
#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->tiles)

struct RETROFLAT_PLATFORM {
   uint16_t*            sprite_frames[RETROFLAT_NDS_SPRITES_ACTIVE];
   struct RETROFLAT_BITMAP* oam_entries[RETROFLAT_NDS_SPRITES_ACTIVE];
   int16_t              oam_dx[RETROFLAT_NDS_SPRITES_ACTIVE];
   int16_t              oam_dy[RETROFLAT_NDS_SPRITES_ACTIVE];
   int16_t              oam_sx[RETROFLAT_NDS_SPRITES_ACTIVE];
   int16_t              oam_sy[RETROFLAT_NDS_SPRITES_ACTIVE];
   int                  bg_id;
   uint8_t              bg_bmp_changed;
   uint8_t              window_bmp_changed;
   int                  window_id;
   int                  px_id;
   uint16_t             bg_tiles[1024];
   uint16_t             window_tiles[1024];
};

#endif /* !RETPLTD_H */

