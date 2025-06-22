
/**
 * \addtogroup maug_retroflt_dir_2d RetroFlat 2D Wrapper API
 * \brief Wrappers to call appropriate 2D surface functions for bitmaps or
 *        textures, depending on if the engine is in 2D or 3D mode.
 *
 * This wrapper system is necessary so that high-level internal library 
 * functions (e.g. retrosoft, retrofont, or retroani) are able to continue
 * functioning on textures in 3D mode.
 *
 * Formerly, this was accomplished by shunts inside of the lower-level
 * \ref maug_retroflt_drawing, but these shunts were removed in order to
 * facilitate platform-agnostic software-only 3D (and they were messy).
 *
 * \note This API has been specifically redone to eliminate variadic macros.
 *       These do not work in older compilers, such as those for Windows CE!
 * \{
 */

/*! \} */ /* maug_retro3d_util */

#if defined( RETROFLAT_BMP_TEX )
typedef struct RETROFLAT_3DTEX retroflat_blit_t;
#else
typedef struct RETROFLAT_BITMAP retroflat_blit_t;
#endif

#ifdef RETROFLT_C
#  define RETROFLT_CB_EXTERN
#  define RETROFLT_CB_INIT = NULL
#else
#  define RETROFLT_CB_EXTERN extern
#  define RETROFLT_CB_INIT
#endif /* RETROFLT_C */

/**
 * \brief Type of callback function used to produce pixels on a surface.
 *
 * This is switched between ::RETROFLAT_3DTEX and ::RETROFLAT_BITMAP, depending
 * on whether this is a 3D or 2D engine.
 */
typedef void (*retroflat_px_cb)(
   retroflat_blit_t* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags );

typedef void (*retroflat_line_cb)(
   retroflat_blit_t* target, const RETROFLAT_COLOR color,
   int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t flags );

typedef void (*retroflat_rect_cb)(
   retroflat_blit_t* target, const RETROFLAT_COLOR color,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags );

typedef MERROR_RETVAL (*retroflat_blit_bitmap_cb)(
   retroflat_blit_t* target, retroflat_blit_t* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance );

typedef MERROR_RETVAL (*retroflat_load_bitmap_cb)(
   const char* filename, retroflat_blit_t* bmp_out, uint8_t flags );

typedef MERROR_RETVAL (*retroflat_create_bitmap_cb)(
   size_t w, size_t h, retroflat_blit_t* bmp_out, uint8_t flags );

/**
 * \brief Directly addressable callback to produce pixels on a surface.
 *
 * This is assigned in retroflat_init(), as that is when all of the _px()
 * callback functions are defined and available.
 */
RETROFLT_CB_EXTERN retroflat_px_cb retroflat_2d_px RETROFLT_CB_INIT;

RETROFLT_CB_EXTERN retroflat_line_cb retroflat_2d_line RETROFLT_CB_INIT;

RETROFLT_CB_EXTERN retroflat_rect_cb retroflat_2d_rect RETROFLT_CB_INIT;

RETROFLT_CB_EXTERN
retroflat_blit_bitmap_cb retroflat_2d_blit_bitmap RETROFLT_CB_INIT;

RETROFLT_CB_EXTERN
retroflat_load_bitmap_cb retroflat_2d_load_bitmap RETROFLT_CB_INIT;

RETROFLT_CB_EXTERN
retroflat_create_bitmap_cb retroflat_2d_create_bitmap RETROFLT_CB_INIT;

#if defined( RETROFLAT_BMP_TEX ) || defined( DOCUMENTATION )

/**
 * \addtogroup maug_retroflt_bitmap
 * \{
 */
#  define retroflat_bitmap_has_flags( bmp, f ) \
      (NULL != (bmp) && (f) == ((f) & (bmp)->tex.flags))

/*! \} */ /* maug_retroflt_bitmap */

#  define retroflat_2d_bitmap_ok( b ) retro3d_texture_ok( b )

#  define retroflat_2d_bitmap_w( b ) retro3d_texture_w( b )

#  define retroflat_2d_bitmap_h( b ) retro3d_texture_h( b )

#  define retroflat_2d_lock_bitmap( b ) retro3d_texture_lock( b )

#  define retroflat_2d_release_bitmap( b ) retro3d_texture_release( b )

#  define retroflat_2d_destroy_bitmap( b ) retro3d_texture_destroy( b )

#  define retroflat_2d_blit_win( src, d_x, d_y ) \
      retro3d_draw_window( src, d_x, d_y )

#else

#  define retroflat_bitmap_has_flags( bmp, f ) \
      (NULL != (bmp) && (f) == ((f) & (bmp)->flags))

#  define retroflat_2d_bitmap_ok( b ) retroflat_bitmap_ok( b )

#  define retroflat_2d_bitmap_w( b ) retroflat_bitmap_w( b )

#  define retroflat_2d_bitmap_h( b ) retroflat_bitmap_h( b )

#  define retroflat_2d_lock_bitmap( b ) retroflat_draw_lock( b )

#  define retroflat_2d_release_bitmap( b ) retroflat_draw_release( b )

#  define retroflat_2d_destroy_bitmap( b ) retroflat_destroy_bitmap( b )

#  define retroflat_2d_blit_win( src, d_x, d_y ) \
      retroflat_blit_bitmap( NULL, (src), 0, 0, d_x, d_y, \
         (win)->gui->w, (win)->gui->h, 0 )

#endif /* RETROFLAT_BMP_TEX || DOCUMENTATION */

