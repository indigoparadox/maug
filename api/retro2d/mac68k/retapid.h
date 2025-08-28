
#ifndef RETPLTD_H
#define RETPLTD_H

/**
 * \file retpltd.h
 * \brief RetroFlat platform definition header.
 */

#ifdef RETROFLAT_API_CARBON
#  include <Carbon.h>
#else
#  include <Quickdraw.h>
#  include <Dialogs.h>
#  include <Fonts.h>
#  include <Gestalt.h>
#  include <Traps.h>
#endif /* RETROFLAT_API_CARBON */

#define RETROPLAT_PRESENT 1

/* Number of previous graphics ports we can stack. This can theoretically be as
 * low as 2 since we won't be drawing more than 2 bitmaps at once, but you never
 * know.
 */
#define RETROFLAT_M68K_PORT_STACK_MAX_CT 4

/* TODO: Implement Quickdraw viewport. */
#define RETROFLAT_SOFT_VIEWPORT

#define RETROFLAT_LOAD_BITMAP_GENERIC 1

#  ifdef RETROFLAT_OPENGL
#     error "opengl support not implemented for PC BIOS"
#  endif /* RETROFLAT_OPENGL */

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
   Handle bits_h;
   Ptr bits;
   BitMap bitmap;
   GrafPort port;
   GWorldPtr gworld;
};

/*! \brief Check to see if a bitmap is loaded. */
#  define retroflat_bitmap_ok( bmp ) (nil != (bmp)->bits_h)

#  define retroflat_bitmap_locked( bmp ) (nul != (bmp)->bitmap.baseAddr)

#  define retroflat_bitmap_w( bmp ) ((bmp)->bitmap.bounds.right)

#  define retroflat_bitmap_h( bmp ) ((bmp)->bitmap.bounds.bottom)

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
typedef RGBColor RETROFLAT_COLOR_DEF;

/*! \} */ /* maug_retroflt_color */

/*! \} */ /* maug_retroflt_drawing */

struct RETROFLAT_PLATFORM_ARGS {
   uint8_t flags;
};

struct RETROFLAT_PLATFORM {
   uint8_t flags;
   WindowPtr win;
   GrafPort* port_stack[RETROFLAT_M68K_PORT_STACK_MAX_CT];
   size_t port_stack_ct;
   GWorldPtr gworld_stack[RETROFLAT_M68K_PORT_STACK_MAX_CT];
   GDHandle gdhandle_stack[RETROFLAT_M68K_PORT_STACK_MAX_CT];
};

#endif /* !RETPLTD_H */

