
#ifndef RETPLTD_H
#define RETPLTD_H

#define RETROPLAT_PRESENT 1

/* == Nintendo DS == */

#  include <nds.h>

#  define RETROCON_DISABLE 1

#define retroflat_wait_for_vblank() swiWaitForVBlank();

/* NDS doesn't have primitives. */
#  ifndef RETROFLAT_SOFT_SHAPES
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

#  ifndef RETROFLAT_SOFT_LINES
#     define RETROFLAT_SOFT_LINES
#  endif /* !RETROFLAT_SOFT_LINES */

#  define BG_TILE_W_PX 8
#  define BG_TILE_H_PX 8
#  define BG_W_TILES 32

typedef int16_t RETROFLAT_IN_KEY;
typedef uint32_t retroflat_ms_t;

#define RETROFLAT_MS_FMT "%lu"

/* Inline pass to generic loop. */
#define retroflat_loop( frame_iter, loop_iter, data ) \
   retroflat_loop_generic( frame_iter, loop_iter, data )

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   uint16_t* b;
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
#  endif /* RETROFLAT_NDS_WASD */
#  define RETROFLAT_KEY_LEFT        KEY_LEFT
#  define RETROFLAT_KEY_RIGHT       KEY_RIGHT
#  define RETROFLAT_KEY_UP          KEY_UP
#  define RETROFLAT_KEY_DOWN        KEY_DOWN
#  define RETROFLAT_KEY_ENTER       KEY_START
#  define RETROFLAT_KEY_SPACE       KEY_A
#  define RETROFLAT_KEY_ESC         KEY_B
#  define RETROFLAT_MOUSE_B_LEFT    (-1)
#  define RETROFLAT_MOUSE_B_RIGHT   (-2)

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
#  define retroflat_bitmap_ok( bitmap ) (0)

struct RETROFLAT_PLATFORM {
   uint16_t*            sprite_frames[NDS_SPRITES_ACTIVE];
   int                  bg_id;
   uint8_t              bg_bmp_changed;
   uint8_t              window_bmp_changed;
   int                  window_id;
   int                  px_id;
   uint16_t             bg_tiles[1024];
   uint16_t             window_tiles[1024];
};

#endif /* !RETPLTD_H */

