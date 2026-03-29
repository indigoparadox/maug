
#ifndef RETPLTD_H
#define RETPLTD_H

/**
 * \file retpltd.h
 * \brief RetroFlat platform definition header.
 */

#include <psxetc.h>
#include <psxapi.h>
#include <psxgte.h>
#include <psxgpu.h>

#define RETROPLAT_PRESENT

#define RETROFLAT_NO_CLI_SZ

#define RETROFLAT_SOFT_VIEWPORT

#define RETROFLAT_LOAD_BITMAP_GENERIC

#define RETROFLAT_PSX_OT_LEN 1

#define RETROFLAT_PSX_FLAG_NOWAIT 0x01

#ifndef RETROFLAT_PSX_SCREEN_W
#  define RETROFLAT_PSX_SCREEN_W 320
#endif /* RETROFLAT_PSX_SCREEN_W */

#ifndef RETROFLAT_PSX_SCREEN_H
#  define RETROFLAT_PSX_SCREEN_H 240
#endif /* RETROFLAT_PSX_SCREEN_H */

#ifndef RETROFLAT_PSX_PRIM_BUF_SZ
#  define RETROFLAT_PSX_PRIM_BUF_SZ 16384
#endif /* !RETROFLAT_PSX_PRIM_BUF_SZ */

#ifndef RETROFLAT_PSX_CIRCLE_SEGMENTS
#  define RETROFLAT_PSX_CIRCLE_SEGMENTS 16
#endif /* !RETROFLAT_PSX_CIRCLE_SEGMENTS */

#ifndef RETROFLAT_PSX_OSB_PTS_CT_MAX
#  define RETROFLAT_PSX_OSB_PTS_CT_MAX 128
#endif /* RETROFLAT_PSX_OSB_PTS_CT_MAX */

#define RETROFLAT_PSX_VRAM_W 1024
#define RETROFLAT_PSX_VRAM_H 512

/* PlayStation VRAM is divided into 16x2 pages, but the leftmost 5 pages are
 * reserved for display buffers in our setup. We also reserve 4 pages between
 * the normal pages and screen buffers to merge into a "big page" for tilemaps
 * or similar.
 */
#define RETROFLAT_PSX_VRAM_PG_CT 18
#define RETROFLAT_PSX_VRAM_PG_CT_W 9
#define RETROFLAT_PSX_VRAM_PG_CT_H 2

#define RETROFLAT_PSX_VRAM_PG_BIG_IDX (RETROFLAT_PSX_VRAM_PG_CT)

/* Each VRAM page is 64x256. */
#define RETROFLAT_PSX_VRAM_PG_PX_W 64
#define RETROFLAT_PSX_VRAM_PG_PX_H 256

#define RETROFLAT_PSX_DRAW_STACK_CT_MAX 10

#define RETROSOFT_PRESENT

/* Force software drawing to rely on hardware lines. */
#define RETROSOFT_HARD_LINES

#define RETROGUI_NO_TEXTBOX

#  define RETROCON_DISABLE 1

#define RCntMdTARGET (1 << 3)
#define RCntMdDIV8 (1 << 6)

/**
 * \addtogroup maug_retroflt_bitmap
 * \{
 */

/**
 * \brief Platform-specific bitmap structure. retroflat_bitmap_ok() can be
 *        used on a pointer to it to determine if a valid bitmap is loaded.
 *
 * Please see the \ref maug_retroflt_bitmap for more information.
 */
struct RETROFLAT_BITMAP {
   /*! \brief Size of the bitmap structure, used to check VDP compatibility. */
   size_t sz;
   /*! \brief Platform-specific bitmap flags. */
   uint8_t flags;
   DRAWENV draw;
   /**
    * \brief Current drawing buffer idx.
    * \note This is only used for the RETROFLAT_FLAGS_SCREEN_BUFFER bitmap.
    *       It is the *drawing* index, which means it selects which buffer is
    *       being *drawn to*! (The other is being shown at this time!)
    */
   retroflat_pxxy_t w;
   retroflat_pxxy_t h;
   /*! \brief X coordinate of the VRAM page on which this is. */
   retroflat_pxxy_t vram_pg_x;
   /*! \brief Y coordinate of the VRAM page on which this is. */
   retroflat_pxxy_t vram_pg_y;
   /*! \brief X coordinate on the given VRAM page. */
   retroflat_pxxy_t vram_off_x;
   /*! \brief Y coordinate on the given VRAM page. */
   retroflat_pxxy_t vram_off_y;
   int page;
   uint32_t ot[RETROFLAT_PSX_OT_LEN];
   uint8_t prim_buff[RETROFLAT_PSX_PRIM_BUF_SZ];
   /**
    * \brief First primitive to draw on next draw_prims call.
    *
    * \note This is different from prim_buff because the GPU might still be
    *       drawing earlier primitives for this bitmap, so this is the first
    *       non-already-sent primitive.
    */
   uint8_t* first_prim;
   uint8_t* last_prim;
   uint8_t* next_prim;
   size_t used_prim;
   struct RETROFLAT_BITMAP* alt_page;
   uint8_t disp_idx;
};

/**
 * \relates RETROFLAT_BITMAP
 * \brief Check to see if a bitmap is loaded.
 */
#  define retroflat_bitmap_ok( bmp ) \
      ((bmp)->sz == sizeof( struct RETROFLAT_BITMAP ))

/**
 * \relates RETROFLAT_BITMAP
 * \brief Check to see if a bitmap is currently locked.
 */
#  define retroflat_bitmap_locked( bitmap ) (0)

/**
 * \relates RETROFLAT_BITMAP
 * \brief Get the width of this bitmap using underlying mechanisms.
 * \warn The bitmap must be valid!
 */
#  define retroflat_bitmap_w( bmp ) ((bmp)->w)

/**
 * \relates RETROFLAT_BITMAP
 * \brief Get the height of this bitmap using underlying mechanisms.
 * \warn The bitmap must be valid!
 */
#  define retroflat_bitmap_h( bmp ) ((bmp)->h)

/*! \} */ /* maug_retroflt_bitmap */

/*! \brief Get the current screen width in pixels. */
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)

/*! \brief Get the current screen height in pixels. */
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)

/*! \brief Get the direct screen buffer or the VDP buffer if a VDP is loaded. */
#  define retroflat_screen_buffer() (g_retroflat_psx_screen_buffer_ptr)

/*! \brief Lock a surface for pixel drawing if needed. */
#  define retroflat_px_lock( bmp )

/*! \brief Release a surface for pixel drawing if needed. */
#  define retroflat_px_release( bmp )

/**
 * \brief This should be called in order to quit a program using RetroFlat.
 * \param retval The return value to pass back to the operating system.
 */
#  define retroflat_quit( retval_in ) \
      debug_printf( 1, "quit called, retval: %d", retval_in ); \
      g_retroflat_state->retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING; \
      g_retroflat_state->retval = retval_in;

/*! \brief Defined for backward-compatibility with Allegro. */
#  define END_OF_MAIN()

/**
 * \addtogroup maug_retroflt_drawing
 * \{
 * 
 * \addtogroup maug_retroflt_color RetroFlat Colors
 * \brief Color definitions RetroFlat is aware of, for use with the
 *        \ref maug_retroflt_drawing.
 *
 * The precise type and values of these constants vary by platform.
 *
 * \{
 */

/*! \brief Example, should be replaced with platform-specific type. */
typedef struct {
   int r;
   int g;
   int b;
} RETROFLAT_COLOR_DEF;

/*! \} */ /* maug_retroflt_color */

/*! \} */ /* maug_retroflt_drawing */

struct RETROFLAT_PLATFORM_ARGS {
   /*! \brief Example field to prevent empty struct. */
   uint8_t flags;
};

struct RETROFLAT_PSX_OSB_PT {
   retroflat_pxxy_t x;
   retroflat_pxxy_t y;
};

struct RETROFLAT_PLATFORM {
   /*! \brief Example field to prevent empty struct. */
   uint8_t flags;
   DISPENV disp[2];
   /*! \brief LCG rand state. */
   uint32_t rand_state;
   /*! \brief Skyline points inventory.
    * \note Each sprite page in VRAM maintains its own skyline!
    */
   struct RETROFLAT_PSX_OSB_PT osb_pts
      [RETROFLAT_PSX_VRAM_PG_CT + 1][RETROFLAT_PSX_OSB_PTS_CT_MAX];
   size_t osb_pts_ct[RETROFLAT_PSX_VRAM_PG_CT + 1];
   /**
    * \brief Number of bitmaps in each page.
    *
    * \note When this hits zero, the page OSB points is reset.
    */
   size_t osb_bmps[RETROFLAT_PSX_VRAM_PG_CT + 1];
   DRAWENV* draw_stack[RETROFLAT_PSX_DRAW_STACK_CT_MAX];
   size_t draw_stack_ct;
   struct RETROFLAT_BITMAP buffer1;
   struct RETROFLAT_BITMAP buffer2;
};

#ifdef RETROFLT_C
struct RETROFLAT_BITMAP* g_retroflat_psx_screen_buffer_ptr = NULL;
#else
extern struct RETROFLAT_BITMAP* g_retroflat_psx_screen_buffer_ptr;
#endif /* RETROFLT_C */

#endif /* !RETPLTD_H */

