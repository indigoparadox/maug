
#ifndef RETPLTD_H
#define RETPLTD_H

/**
 * \file retpltd.h
 * \brief RetroFlat platform definition header.
 */

#define RETROPLAT_PRESENT 1

#define RETROFLAT_SOFT_VIEWPORT

#define RETROFLAT_SOFT_SHAPES

#define RETROFLAT_SOFT_LINES

#define RETROFLAT_LOAD_BITMAP_GENERIC 1

#include <time.h>
#include <X11/Xutil.h>

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
   XImage* img;
   char* bits;
   int bits_sz;
};

/**
 * \relates RETROFLAT_BITMAP
 * \brief Check to see if a bitmap is loaded.
 */
#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->img)

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
#  define retroflat_bitmap_w( bitmap ) ((bitmap)->img->width)

/**
 * \relates RETROFLAT_BITMAP
 * \brief Get the height of this bitmap using underlying mechanisms.
 * \warn The bitmap must be valid!
 */
#  define retroflat_bitmap_h( bitmap ) ((bitmap)->img->height)

/*! \} */ /* maug_retroflt_bitmap */

/*! \brief Get the current screen width in pixels. */
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)

/*! \brief Get the current screen height in pixels. */
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)

/*! \brief Get the direct screen buffer or the VDP buffer if a VDP is loaded. */
#  define retroflat_screen_buffer() (&(g_retroflat_state->buffer))

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
typedef uint32_t RETROFLAT_COLOR_DEF;

/*! \} */ /* maug_retroflt_color */

/*! \} */ /* maug_retroflt_drawing */

struct RETROFLAT_PLATFORM_ARGS {
   /*! \brief Example field to prevent empty struct. */
   uint8_t flags;
};

struct RETROFLAT_PLATFORM {
   /*! \brief Example field to prevent empty struct. */
   uint8_t flags;
   int screen;
   Window window;
   Display* display;
   Visual* visual;
   uint32_t s_launch;
};

#endif /* !RETPLTD_H */

