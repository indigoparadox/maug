
#ifndef RETROFLT_H
#define RETROFLT_H

#ifdef RETROFLAT_OPENGL
#  define RETROFLAT_BMP_TEX
#endif /* RETROFLAT_OPENGL */

/**
 * \addtogroup retrotile
 * \{
 */

/**
 * \brief Value for an individual tile in a ::RETROTILE_LAYER.
 * 
 * This is defined early on so that it can be used in some other modules.
 */
typedef int16_t retroflat_tile_t;

/*! \} */

/**
 * \addtogroup maug_retroflt RetroFlat API
 * \brief Abstraction layer header for retro systems.
 * \{
 *
 * \file retroflt.h
 * \brief Abstraction layer header for retro systems.
 *
 * RetroFlat is a compatibility layer for making graphical programs that work
 * on Win16 (32-bit via OpenWatcom's Win386), MS-DOS (32-bit via DOS/32a or
 * DOS4GW via Allegro), and possibly other platforms in the future.
 *
 * To use, define RETROFLT_C before including this header from your main.c.
 *
 * You may include this header in other .c files, as well, but RETROFLT_C
 * should \b ONLY be declared \b ONCE in the entire program.
 *
 * maug.h should also be included before this header.
 */

/**
 * \addtogroup maug_retroflt_example RetroFlat Example
 * \brief An extremely simple single-file annotated example program written
 *        with the RetroFlat API.
 * \{
 * \page maug_retroflt_example_page
 *
 * This is a brief example program written for the \ref maug_retroflt.
 *
 * This can be compiled using the Makefiles from \ref maug_retroflt_compiling.
 *
 *
 *
 *       / * RETROFLT_C must be defined before including retroflt.h ONLY
 *         * in one project file! It must be defined once, and only once,
 *         * project-wide! The C file where it is defined is where all the
 *         * RetroFlat functions will be placed.
 *         * /
 *       #define RETROFLT_C
 *       #include <retroflt.h>
 *       
 *       / * An example struct containing data fields to be retained between
 *         * loop iterations below.
 *         * /
 *       struct EXAMPLE_DATA {
 *          int example;
 *       };
 *       
 *       / * This is the loop iteration which will be repeatedly called
 *         * by retroflat_loop() below, until retroflat_quit() is called.
 *         * /
 *       void example_loop( struct EXAMPLE_DATA* data ) {
 *          struct RETROFLAT_INPUT input_evt;
 *          RETROFLAT_IN_KEY input = 0;
 *       
 *          / * Start loop. * /
 *          input = retroflat_poll_input( &input_evt );
 *       
 *          switch( input ) {
 *          case RETROFLAT_MOUSE_B_LEFT:
 *             / * Left mouse button was clicked, do something! * /
 *             break;
 *       
 *          case RETROFLAT_KEY_Q:
 *             / * Q key was pressed, so quit! * /
 *             retroflat_quit( 0 );
 *             break;
 *          }
 *       
 *          / *  === Drawing === * /
 *       
 *          / * Lock the screen (NULL) for drawing. * /
 *          retroflat_draw_lock( NULL );
 *       
 *          / * Clear the screen by drawing a big gray rectangle to NULL. * /
 *          retroflat_rect(
 *             NULL, RETROFLAT_COLOR_GRAY, 0, 0,
 *             retroflat_screen_w(), retroflat_screen_h(),
 *             RETROFLAT_FLAGS_FILL );
 *       
 *          / * Release the screen (NULL) from drawing. * /
 *          retroflat_draw_release( NULL );
 *       }
 *       
 *       int main( int argc, char* argv[] ) {
 *          int retval = 0;
 *          struct EXAMPLE_DATA data;
 *          struct RETROFLAT_ARGS args;
 *
 *          / * Startup logging before all else so we catch everything. * /
 *          logging_init();
 *
 *          / * Setup the args to retroflat_init() below to create a window 
 *            * titled "Example Program", and load bitmaps from the "assets"
 *            * subdirectory next to the executable.
 *            * /
 *          args.title = "Example Program";
 *          args.assets_path = "assets";
 *       
 *          / * Zero out the data holder. * /
 *          memset( &data, '\0', sizeof( struct EXAMPLE_DATA ) );
 *       
 *          / * === Setup === * /
 *          / * Call the init with the args struct created above. * /
 *          retval = retroflat_init( argc, argv, &args );
 *
 *          / * Make sure setup completed successfully! * /
 *          if( RETROFLAT_OK != retval ) {
 *             goto cleanup;
 *          }
 *       
 *          / * === Main Loop === * /
 *          / * Call example_loop( data ) repeatedly, until it calls
 *            * retroflat_quit(). Passing it as the first arg means it will
 *            * be capped by the framerate.
 *            * /
 *          retroflat_loop( (retroflat_loop_iter)example_loop, NULL, &data );
 *       
 *       cleanup:
 *       
 *       / * Don't run cleanup stuff under WASM. * /
 *       #ifndef RETROFLAT_OS_WASM
 *
 *          / * This must be called at the end of the program! * /
 *          retroflat_shutdown( retval );
 *
 *          / * Shutdown logging after all else so we catch everything. * /
 *          logging_shutdown();
 *
 *       #endif / * RETROFLAT_OS_WASM * /
 *       
 *          return retval;
 *       }
 *       / * This should ALWAYS be included after main(), no matter what
 *         * platform your program is intended for.
 *         * /
 *       END_OF_MAIN()
 *
 * \}
 */

/**
 * \addtogroup maug_retroflt_compiling RetroFlat Compilation
 * \brief Makefile examples and compiler considerations for using RetroFlat.
 * \{
 * \page maug_retroflt_cdefs_page RetroFlat Compiler Definitions
 *
 * Certain options can be configured when a RetroFlat program is compiled by
 * passing definitions with the compiler using the -D flag, or by #define-ing
 * them before retroflt.h is included in the source files.
 *
 * ## OS Selection Definitions
 *
 * These are mutually exclusive. It's best to specify them, in case RetroFlat
 * can't figure it out from the compiler options.
 *
 * | Define                | Description                                      |
 * | --------------------- | ------------------------------------------------ |
 * | RETROFLAT_OS_WIN      | Specify that the program will run on Windows.    |
 * | RETROFLAT_OS_DOS      | Specify that the program will run on DOS DPMI.   | 
 * | RETROFLAT_OS_DOS_REAL | Specify that the program will run DOS real mode. | 
 * | RETROFLAT_OS_UNIX     | Specify that the program will run on UNIX/Linux. |
 *
 * ## API/Library Selection Definitions
 *
 * These are mutually exclusive. \ref maug_retroglu is only supported on
 * some (32-bit and up) platforms.
 *
 * | Define                | Description        | Supports OpenGL |
 * | --------------------- | ------------------ | --------------- |
 * | RETROFLAT_API_ALLEGRO | Allegro            | No              |
 * | RETROFLAT_API_SDL1    | SDL 1.2            | Yes             |
 * | RETROFLAT_API_SDL2    | SDL 2              | No              |
 * | RETROFLAT_API_WIN16   | Windows 3.1x API   | No              |
 * | RETROFLAT_API_WIN32   | Win32/NT API       | Yes             |
 * | RETROFLAT_API_LIBNDS  | Nintendo DS        | Yes (Limited)   |
 * | RETROFLAT_API_PC_BIOS | MS-DOS w/ PC BIOS  | No              |
 * | RETROFLAT_API_GLUT    | GLUT (OpenGL-Only) | Yes             |
 *
 * ## Option Definitions
 *
 * These are \b NOT mutually exclusive.
 *
 * | Define                 | Description                                      |
 * | ---------------------- | -------------------------------------------------|
 * | RETROFLAT_MOUSE        | Force-enable mouse on broken APIs (DANGEROUS!)   |
 * | RETROFLAT_TXP_R        | Specify R component of bitmap transparent color. |
 * | RETROFLAT_TXP_G        | Specify G component of bitmap transparent color. |
 * | RETROFLAT_TXP_B        | Specify B component of bitmap transparent color. |
 * | RETROFLAT_BITMAP_EXT   | Specify file extension for bitmap assets.        |
 * | RETROFLAT_NO_RESIZABLE | Disallow resizing the RetroFlat window.          |
 * | RETROFLAT_NO_BLANK_INIT| Do not blank screen on retroflat_ini().          |
 *
 * \page maug_retroflt_makefile_page RetroFlat Project Makefiles
 *
 * ## Win16 (OpenWatcom)
 *
 * Compiling projects for Win16 requires the OpenWatcom compiler from
 * https://github.com/open-watcom/open-watcom-v2. Other compilers may work,
 * but this is the only one that has been tested. No other specific libraries
 * are required. The Windows API is used directly.
 *
 * The Watcom Makefile includes assume the following variable definitions
 * from the environment:
 *
 * - \b $WATCOM should be set to the path where the OpenWatcom compiler has
 *   been extracted, containing the binw, binl, h, and lib386 directories
 *   (among others).
 * - \b $PATH should contain $WATCOM/bin, so that the wcc386 and accompanying
 *   tools below can be executed.
 *
 * For an example of the main.c file, please see \ref maug_retroflt_example.
 *
 * ## Makefile
 *
 * An example to use the unified Makefile include follows. Please name it
 * "Makefile" in the root directory of the project and replace "exampl" with a
 * 6-character binary name for best results!
 *
 *       # A list of C files included in the project. These must be provided
 *       # by you. In this example, our program only has one file.
 *       C_FILES := main.c
 *
 *       # Project-specific defines to pass to EVERY target.
 *       GLOBAL_DEFINES := 
 *     
 *       # Include the template Makefile that will create targets for
 *       # platforms as specified below.
 *       include maug/Makefile.inc
 *
 *       # Build these targets by default.
 *       all: exampl.sdl examplb.exe examplnt.exe
 *       
 *       # Create an icon from exampl.bmp for SDL to use for its window.
 *       $(eval $(call TGTSDLICO,mdemo))
 *
 *       $(eval $(call TGTUNIXSDL,mdemo))
 *
 *       # Build a DOS real-mode target.
 *       $(eval $(call TGTDOSBIOS,exampl))
 *
 *       # Create an icon and resource file from exampl.bmp for Windows
 *       # targets to use.
 *       $(eval $(call TGTWINICO,exampl))
 *
 *       # Build a Windows NT 32-bit target.
 *       $(eval $(call TGTWINNT,exampl))
 *
 *       # Remove built files from previously defined targets.
 *       clean:
 *       	rm -rf $(CLEAN_TARGETS)
 * \}
 */

/* === Generic Includes and Defines === */

#ifndef RETROFLAT_BITMAP_TRACE_LVL
#  define RETROFLAT_BITMAP_TRACE_LVL 0
#endif /* !RETROFLAT_BITMAP_TRACE_LVL */

#ifndef RETROFLAT_KB_TRACE_LVL
#  define RETROFLAT_KB_TRACE_LVL    0
#endif /* !RETROFLAT_KB_TRACE_LVL */

#include <stdarg.h>

#include <marge.h>

/**
 * \addtogroup maug_retroflt_color
 * \{
 */

/**
 * \brief This macro defines all colors supported by RetroFlat for
 *        primative operations, particularly using retroflat_px().
 *
 * The fields are:
 * index, name (lowercase), name (uppercase), red, green, blue, cga on-color, cga off-color
 *
 * On-color and off-color are used for dithering... odd pixels get one and
 * even pixels get the other.
 */
#define RETROFLAT_COLOR_TABLE( f ) \
   f( 0, black,     BLACK,     0,   0,   0,   BLACK,   BLACK   ) \
   f( 1, darkblue,  DARKBLUE,  0,   0,   170, CYAN,    BLACK   ) \
   f( 2, darkgreen, DARKGREEN, 0,   170, 0,   CYAN,    BLACK   ) \
   f( 3, teal,      TEAL,      0,   170, 170, CYAN,    CYAN    ) \
   f( 4, darkred,   DARKRED,   170, 0,   0,   MAGENTA, BLACK   ) \
   f( 5, violet,    VIOLET,    170, 0,   170, MAGENTA, BLACK   ) \
   f( 6, brown,     BROWN,     170, 85,  0,   CYAN,    MAGENTA ) \
   f( 7, gray,      GRAY,      170, 170, 170, WHITE,   BLACK   ) \
   f( 8, darkgray,  DARKGRAY,  85,  85,  85,  WHITE,   BLACK   ) \
   f( 9, blue,      BLUE,      85,  85,  255, CYAN,    WHITE   ) \
   f( 10, green,    GREEN,     85,  255, 85,  CYAN,    CYAN    ) \
   f( 11, cyan,     CYAN,      85,  255, 255, CYAN,    CYAN    ) \
   f( 12, red,      RED,       255, 85,  85,  MAGENTA, WHITE   ) \
   f( 13, magenta,  MAGENTA,   255, 85,  255, MAGENTA, MAGENTA ) \
   f( 14, yellow,   YELLOW,    255, 255, 85,  CYAN,    MAGENTA ) \
   f( 15, white,    WHITE,     255, 255, 255, WHITE,   WHITE   )

/*! \brief Defines an index in the platform-specific color-table. */
typedef int8_t RETROFLAT_COLOR;

#  define RETROFLAT_COLOR_NULL (-1)

#  define RETROFLAT_COLORS_SZ 16

/*! \} */

/* TODO: Mouse is broken under DOS/Allegro. */
#if defined( RETROFLAT_OS_UNIX ) || defined( RETROFLAT_OS_WIN )
#define RETROFLAT_MOUSE
#endif /* RETROFLAT_OS_WIN || RETROFLAT_OS_WIN */

/**
 * \addtogroup maug_retroflt_retval RetroFlat API Return Values
 * \brief Standardized return values from RetroFlat API functions.
 * \{
 */

/*! \brief Certain functions return this when there was no problem. */
#define RETROFLAT_OK             0x00
#define RETROFLAT_ERROR_ENGINE   0x01
#define RETROFLAT_ERROR_GRAPHICS 0x02
#define RETROFLAT_ERROR_MOUSE    0x04

/**
 * \brief Returned if there is a problem loading or locking a
 *        ::RETROFLAT_BITMAP struct.
 */
#define RETROFLAT_ERROR_BITMAP   0x08
#define RETROFLAT_ERROR_TIMER    0x0f

/*! \} */ /* maug_retroflt_retval */

/**
 * \addtogroup maug_retroflt_drawing RetroFlat Drawing API
 * \brief Functions for drawing primitives on-screen.
 * \warning Drawing functions should not be used outside of the context of
 *          the main loop, as the platform may not have fully initialized
 *          the drawing window until the main loop is called for the first
 *          time!
 * \{
 */

/**
 * \brief Flag for retroflat_rect() or retroflat_ellipse(), indicating drawn
 *        shape should be filled.
 */
#define RETROFLAT_FLAGS_FILL     0x01

/**
 * \brief Flag for retroflat_create_bitmap() to create a bitmap without 
 *        transparency.
 */
#define RETROFLAT_FLAGS_OPAQUE   0x01

/**
 * \brief Flag for retroflat_string() and retroflat_string_sz() to print
 *        text in all capital letters. Non-letters are unmodified.
 * \todo This has not yet been implemented and is present for backward
 *       compatibility.
 */
#define RETROFLAT_FLAGS_ALL_CAPS 0x02

/**
 * \brief Flag for retroflat_load_bitmap() to not use assets path.
 */
#define RETROFLAT_FLAGS_LITERAL_PATH   0x02

/**
 * \brief Flag for retroflat_create_bitmap() to create a WinG-backed bitmap.
 *
 * Also may be present in RETROFLAT_BITMAP::flags to indicate that a bitmap
 * is screen-backed.
 */
#define RETROFLAT_FLAGS_SCREEN_BUFFER     0x80

/*! \} */ /* maug_retroflt_drawing */

/**
 * \addtogroup maug_retroflt_flags Global Flags
 * \brief Flags that may be present on RETROFLAT_STATE::retroflat_flags
 * \{
 */

/**
 * \brief Flag indicating that retroflat_loop() should continue executing.
 * \warning This flag is not used on all platforms! It should only be removed
 *          using retroflat_quit().
 */
#define RETROFLAT_FLAGS_RUNNING  0x01

/**
 * \brief Flag indicating FPS should not be capped.
 * \warning This flag should only be set inside retroflat!
 */
#define RETROFLAT_FLAGS_UNLOCK_FPS 0x02

/**
 * \brief Flag indicating keyboard repeat is enabled.
 * \warning This flag should only be set inside retroflat!
 */
#define RETROFLAT_FLAGS_KEY_REPEAT 0x04

/**
 * \brief Flag indicating the current application is running as a screensaver.
 * \warning This flag should only be set inside retroflat!
 */
#define RETROFLAT_FLAGS_SCREENSAVER 0x08

/**
 * \brief Do not execute any more inter-frame loops until next frame.
 */
#define RETROFLAT_FLAGS_WAIT_FOR_FPS   0x20

/*! \} */ /* maug_retroflt_flags */

/**
 * \addtogroup maug_retroflt_msg_api RetroFlat Message API
 * \brief These flags can be passed to retroflat_message() to indicate the
 *        type of message being conveyed.
 * \{
 */

/*! \brief This mask covers all possible icon/type flags. */
#define RETROFLAT_MSG_FLAG_TYPE_MASK 0x07

/**
 * \brief This icon/type flag indicates an error. It will try to display
 *        messages in an urgent way with a red icon, if possible.
 */
#define RETROFLAT_MSG_FLAG_ERROR 0x01

/**
 * \brief This icon/type flag indicates an informational notice. It will try 
 *        to display messages in a definite way, with an i or speech bubble
 *        icon, if possible.
 */
#define RETROFLAT_MSG_FLAG_INFO 0x02

/**
 * \brief This icon/type flag indicates a condition the user should be aware
 *        of. It will try to display messages in an urgent way with a yellow
 *        icon, if possible.
 */
#define RETROFLAT_MSG_FLAG_WARNING 0x04

/*! \} */ /* maug_retroflt_msg_flags */

struct RETROFLAT_STATE;

/**
 * \addtogroup maug_retroflt_vdp RetroFlat VDP API
 * \brief Video Display Processor tools for modifying display output.
 *
 * The VDP system allows for a plugin to post-process frames in programs that
 * use RetroFlat. The VDP is provided with a bitmap of every frame
 * in RETROFLAT_STATE::vdp_buffer which it processes and outputs to
 * RETROFLAT_STATE::buffer, which is then displayed on the screen.
 *
 * \{
 */

/**
 * \brief Flag for RETROFLAT_STATE::vdp_flags indicating the VDP requires
 *        RetroFlat to pixel-lock the frame before passing it (almost always
 *        true!)
 *
 * This should be set by the VDP during its initialization.
 */
#define RETROFLAT_VDP_FLAG_PXLOCK 0x01

/**
 * \brief VDP function called from the VDP library.
 *
 * The VDP plugin should export three functions with this signature:
 *
 * | Function Name          | Called From                                     |
 * |------------------------|-------------------------------------------------|
 * | retroflat_vdp_init()   | retroflat_init()                                |
 * | retroflat_vdp_flip()   | retroflat_draw_release() on screen buffer       |
 * | retroflat_vdp_shutdown | retroflat_shutdown()                            |
 */
typedef MERROR_RETVAL (*retroflat_vdp_proc_t)( struct RETROFLAT_STATE* );

/*! \} */ /* maug_retroflt_vdp */

typedef MERROR_RETVAL (*retroflat_proc_resize_t)(
   uint16_t new_w, uint16_t new_h, void* data );

/**
 * \relates RETROFLAT_ARGS
 * \{
 */

#define RETROSND_ARGS_FLAG_LIST_DEVS 0x01

/*! \} */

/**
 * \addtogroup maug_retroflt_bitmap RetroFlat Bitmap API
 * \brief Tools for loading bitmaps from disk and drawing them on-screen.
 *
 * Bitmaps handled by retroflat are subject to certain constraints, due to
 * the limitations of the underyling layers:
 *
 * - They must have a 16-color palette.
 * - The colors must be in the order specified in \ref maug_retroflt_color.
 * - The first color (black) is the transparency color.
 *
 * ::RETROFLAT_BITMAP structs loaded with retroflat_load_bitmap() should later
 * be freed using retroflat_destroy_bitmap().
 *
 * Locking from the \ref maug_retroflt_drawing also applies here. Please see
 * that page for more information on retroflat_draw_lock() and
 * retroflat_draw_release().
 *
 * \warning The screen \b MUST be locked by calling retroflat_draw_lock() with
 *          NULL before it is drawn to, and released with
 *          retroflat_draw_release() when drawing is finished before the
 *          retroflat_loop_iter() ends!
 *
 * \{
 */

/**
 * \relates RETROFLAT_BITMAP
 */
#define RETROFLAT_FLAGS_LOCK     0x01

#define RETROFLAT_FLAGS_SCREEN_LOCK     0x02

#define RETROFLAT_FLAGS_BITMAP_RO   0x04

/**
 * \relates retroflat_blit_bitmap
 * \brief Pass to retroflat_blit_bitmap() instance arg if this is not a sprite
 *        (i.e. if it is a background tile).
 */
#define RETROFLAT_INSTANCE_NULL (0)

/**
 * \relates retroflat_blit_bitmap
 * \brief Declare that a given instance ID is for a tile, rather than a sprite.
 */
#define retroflat_instance_tile( instance ) \
  (instance * -1)

/**
 * \brief The filename suffix to be appended with a "." to filenames passed to
 *        retroflat_load_bitmap(). Is a \ref maug_retroflt_cdefs_page.
 */
#ifndef RETROFLAT_BITMAP_EXT
#  define RETROFLAT_BITMAP_EXT "bmp"
#endif /* !RETROFLAT_BITMAP_EXT */

#ifndef RETROFLAT_OPENGL_BPP
#  define RETROFLAT_OPENGL_BPP 32
#endif /* !RETROFLAT_OPENGL_BPP */

#ifndef RETROFLAT_TILE_W
#  define RETROFLAT_TILE_W 16
#endif /* !RETROFLAT_TILE_W */

#ifndef RETROFLAT_TILE_W_BITS
#  define RETROFLAT_TILE_W_BITS 4
#endif /* !RETROFLAT_TILE_W_BITS */

#ifndef RETROFLAT_TILE_H
#  define RETROFLAT_TILE_H 16
#endif /* !RETROFLAT_TILE_H */

#ifndef RETROFLAT_TILE_H_BITS
#  define RETROFLAT_TILE_H_BITS 4
#endif /* !RETROFLAT_TILE_H_BITS */

/* Transparency background color: black by default, to match Allegro. */
#ifndef RETROFLAT_TXP_R
/**
 * \brief Compiler-define-overridable constant indicating the Red value of the
 *        transparency color on platforms that support it (mainly Win16/SDL).
 *        Is a \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_TXP_R 0x00
#endif /* !RETROFLAT_TXP_R */

#ifndef RETROFLAT_TXP_G
/**
 * \brief Compiler-define-overridable constant indicating the Green value of the
 *        transparency color on platforms that support it (mainly Win16/SDL).
 *        Is a \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_TXP_G 0x00
#endif /* !RETROFLAT_TXP_G */

#ifndef RETROFLAT_TXP_B
/**
 * \brief Compiler-define-overridable constant indicating the Blue value of the
 *        transparency color on platforms that support it (mainly Win16/SDL).
 *        Is a \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_TXP_B 0x00
#endif /* !RETROFLAT_TXP_B */

#ifndef RETROFLAT_TXP_PAL_IDX
#  define RETROFLAT_TXP_PAL_IDX 0
#endif /* !RETROFLAT_TXP_PAL_IDX */

/*! \} */ /* maug_retroflt_bitmap */

#ifndef RETROFLAT_DEFAULT_SCREEN_W
#  define RETROFLAT_DEFAULT_SCREEN_W 320
#endif /* RETROFLAT_DEFAULT_SCREEN_W */

#ifndef RETROFLAT_DEFAULT_SCREEN_H
#  define RETROFLAT_DEFAULT_SCREEN_H 200
#endif /* RETROFLAT_DEFAULT_SCREEN_H */

#define retroflat_on_resize( w, h ) \
   g_retroflat_state->screen_w = w; \
   g_retroflat_state->screen_h = h;

/**
 * \addtogroup maug_retroflt_drawing
 * \{
 */

#ifndef RETROFLAT_LINE_THICKNESS
/**
 * \brief Line drawing thickness (only works on some platforms).
 *        Is a \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_LINE_THICKNESS 1
#endif /* !RETROFLAT_LINE_THICKNESS */

#define RETROFLAT_PI 3.14159

/*! \} */ /* maug_retroflt_drawing */

/**
 * \addtogroup maug_retroflt_compiling
 * \{
 */

#ifndef RETROFLAT_FPS
/**
 * \brief Target Frames Per Second.
 * \todo FPS currently has no effect in Allegro.
 */
#  define RETROFLAT_FPS 30
#endif /* !RETROFLAT_FPS */

#define retroflat_fps_next() (1000 / RETROFLAT_FPS)

#ifndef RETROFLAT_WINDOW_CLASS
/**
 * \brief Unique window class to use on some platforms (e.g. Win32).
 *        Is a \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_WINDOW_CLASS "RetroFlatWindowClass"
#endif /* !RETROFLAT_WINDOW_CLASS */

#ifndef RETROFLAT_WIN_FRAME_TIMER_ID
/**
 * \brief Unique ID for the timer that execute frame draws in Win16/Win32.
 *        Is a \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_WIN_FRAME_TIMER_ID 6001
#endif /* !RETROFLAT_WIN_FRAME_TIMER_ID */

#ifndef RETROFLAT_WIN_LOOP_TIMER_ID
/**
 * \brief Unique ID for the timer that execute loop ticks in Win16/Win32.
 *        Is a \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_WIN_LOOP_TIMER_ID 6002
#endif /* !RETROFLAT_WIN_LOOP_TIMER_ID */

#ifndef RETROFLAT_MSG_MAX
/**
 * \brief Maximum number of characters possible in a message using
 *        retroflat_message(). Is a \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_MSG_MAX 4096
#endif /* !RETROFLAT_MSG_MAX */

#define RETROFLAT_PATH_MAX MAUG_PATH_MAX

#ifndef RETROFLAT_TITLE_MAX
#  define RETROFLAT_TITLE_MAX 255
#endif /* !RETROFLAT_TITLE_MAX */

#ifndef RETROFLAT_VDP_ARGS_SZ_MAX
/**
 * \warn Changing this may break binary compatibility!
 */
#  define RETROFLAT_VDP_ARGS_SZ_MAX 255
#endif /* !RETROFLAT_VDP_ARGS_SZ_MAX */

#if defined( RETROFLAT_API_SDL2 )
#  if !defined( NO_RETROFLAT_RESIZABLE )
#     define RETROFLAT_WIN_FLAGS SDL_WINDOW_RESIZABLE
#  else
#     define RETROFLAT_WIN_FLAGS 0
#  endif /* !NO_RETROFLAT_RESIZABLE */
#endif /* RETROFLAT_API_SDL2 */

#if defined( RETROFLAT_API_SDL1 )
#  define RETROFLAT_SDL_CC_FLAGS (SDL_RLEACCEL | SDL_SRCCOLORKEY)
#elif defined( RETROFLAT_API_SDL2 )
#  define RETROFLAT_SDL_CC_FLAGS (SDL_TRUE)
#endif /* RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 */

#ifdef RETROFLAT_OS_DOS
#  define RETROFLAT_PATH_SEP '\\'
#else
/*! \brief The valid path separator on the target platform. */
#  define RETROFLAT_PATH_SEP '/'
#endif /* RETROFLAT_OS_DOS */

/*! \brief Maximum size of the assets path, to allow room for appending. */
#define RETROFLAT_ASSETS_PATH_MAX (RETROFLAT_PATH_MAX >> 1)

#ifndef RETROFLAT_BMP_COLORS_SZ_MAX
#  define RETROFLAT_BMP_COLORS_SZ_MAX 256
#endif /* !RETROFLAT_BMP_COLORS_SZ_MAX */

/*! \} */ /* maug_retroflt_compiling */

#define retroflat_wait_for_frame() \
   (g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_WAIT_FOR_FPS)

#define retroflat_is_waiting_for_frame() \
   (RETROFLAT_FLAGS_WAIT_FOR_FPS == \
      (g_retroflat_state->retroflat_flags & RETROFLAT_FLAGS_WAIT_FOR_FPS))

/**
 * \addtogroup maug_retroflt_assets RetroFlat Assets API
 * \brief Functions and macros for handling graphical asset files.
 * \todo This is kind of a mess and needs better integration with the rest!
 * \{
 */

/**
 * \brief Path/name used to load an asset from disk.
 */
typedef char retroflat_asset_path[RETROFLAT_PATH_MAX + 1];

/**
 * \brief Compare two asset paths. Return 0 if they're the same.
 */
#define retroflat_cmp_asset_path( a, b ) strncmp( a, b, RETROFLAT_PATH_MAX )

#define retroflat_assign_asset_path( tgt, src ) \
   maug_strncpy( tgt, src, RETROFLAT_PATH_MAX )

#define retroflat_assign_asset_trim_ext( tgt, src ) \
   maug_snprintf( tgt, RETROFLAT_PATH_MAX, "%s", src ); \
   if( NULL != strrchr( tgt, '.' ) ) { \
      *(strrchr( tgt, '.' )) = '\0'; \
   }

/*! \} */ /* maug_retroflt_assets */

/**
 * \brief Prototype for the main loop function passed to retroflat_loop().
 */
typedef void (*retroflat_loop_iter)(void* data);

/**
 * \addtogroup maug_retroflt_input RetroFlat Input API
 * \brief Functions and constants for polling and interpreting user input.
 * \{
 */

/**
 * \brief Remove a character from a text buffer before cursor position.
 */
#define retroflat_buffer_bksp( buffer, buffer_cur, buffer_sz ) \
   if( 0 < buffer_cur ) { \
      if( buffer_cur < buffer_sz ) { \
         memmove( \
            &(buffer[(buffer_cur) - 1]), \
            &(buffer[buffer_cur]), \
            (buffer_sz) - (buffer_cur) ); \
      } \
      buffer_cur--; \
      buffer_sz--; \
      buffer[buffer_sz] = '\0'; \
   }

/**
 * \brief Insert a character into a text buffer at cursor position.
 */
#define retroflat_buffer_insert( c, buffer, buffer_cur, buffer_sz, buffer_mx ) \
   if( buffer_sz + 1 < buffer_mx ) { \
      if( buffer_cur < buffer_sz ) { \
         memmove( \
            &(buffer[(buffer_cur) + 1]), \
            &(buffer[buffer_cur]), \
            (buffer_sz) - (buffer_cur) ); \
      } \
      buffer[buffer_cur] = c; \
      buffer_cur++; \
      buffer_sz++; \
      buffer[buffer_sz] = '\0'; \
   }

#define RETROFLAT_INPUT_MOD_SHIFT   0x01

#define RETROFLAT_INPUT_MOD_ALT     0x02

#define RETROFLAT_INPUT_MOD_CTRL    0x04

#define RETROFLAT_INPUT_FORCE_UPPER    0x08

/*! \brief Struct passed to retroflat_poll_input() to hold return data. */
struct RETROFLAT_INPUT {
   /**
    * \brief X-coordinate of the mouse pointer in pixels if the returned
    *        event is a mouse click.
    */
   int mouse_x;
   /**
    * \brief Y-coordinate of the mouse pointer in pixels if the returned
    *        event is a mouse click.
    */
   int mouse_y;
   uint8_t key_flags;
};

/*! \} */ /* maug_retroflt_input */

/**
 * \addtogroup maug_retroflt_dir RetroFlat Direction API
 * \brief Macros and constants for definition cardinal directions in screen
 *        and world space.
 * \{
 */

typedef int8_t retroflat_dir4_t;

typedef int8_t retroflat_dir8_t;

#define RETROFLAT_DIR4_NONE  (-1)
#define RETROFLAT_DIR4_NORTH  0
#define RETROFLAT_DIR4_EAST   1
#define RETROFLAT_DIR4_SOUTH  2
#define RETROFLAT_DIR4_WEST   3

#define RETROFLAT_DIR8_NONE   (-1)
#define RETROFLAT_DIR8_NORTH  0
#define RETROFLAT_DIR8_EAST   2
#define RETROFLAT_DIR8_SOUTH  4
#define RETROFLAT_DIR8_WEST   6

#define retroflat_dir8_reverse( dir ) \
   ((dir + 4) % 8)

#define retroflat_dir8_bounce( dir ) \
   ((dir + 2) % 8)

/*! \} */ /* maug_retroflt_dir */

/**
 * \brief Type used for surface pixel coordinates.
 *
 * \todo Make this signed when most of the library uses it. Right now, it
 *       causes too many issues with passed references.
 */
typedef size_t retroflat_pxxy_t;

struct RETROFLAT_ARGS;

#ifndef RETRO2D_TRACE_LVL
#  define RETRO2D_TRACE_LVL 0
#endif /* !RETRO2D_TRACE_LVL */

#ifndef RETROFLAT_NO_SOUND

/**
 * \addtogroup maug_retrosnd RetroSound API
 * \brief Abstraction layer header for sound on retro systems.
 * \{
 *
 * \file retrosnd.h
 * \brief Abstraction layer header for sound on retro systems.
 *
 * RetroSound is a compatibility layer for making sound
 * on various platforms, including Windows, MS-DOS or Linux.
 *
 * To use, define RETROSND_C before including this header from your main.c.
 *
 * You may include this header in other .c files, as well, but RETROSND_C
 * should \b ONLY be declared \b ONCE in the entire program.
 *
 * It is *highly* advised to use this in conjunction with retroflt.h.
 *
 * maug.h should also be included before this header.
 *
 * Special thanks to: "Programming the Adlib/Sound Blaster FM Music Chips"
 * by Jeffrey S. Lee (https://bochs.sourceforge.io/techspec/adlib_sb.txt)
 * and "The Gravis Ultrasound" by neuraldk
 * (http://neuraldk.org/document.php?gus) for DOS platform-specific stuff.
 *
 * And the MPU-401 interface for being so darn simple!
 */

#ifndef RETROSND_TRACE_LVL
#  define RETROSND_TRACE_LVL 0
#endif /* !RETROSND_TRACE_LVL */

#ifndef RETROSND_REG_TRACE_LVL
#  define RETROSND_REG_TRACE_LVL 0
#endif /* !RETROSND_REG_TRACE_LVL */

/**
 * \addtogroup maug_retrosnd_flags RetroSound State Flags
 * \brief Flags indicating global state for the RETROSND_STATE::flags field.
 * \{
 */

/**
 * \brief Flag in RETROSND_STATE::flags indicating initialization was
 *        successful.
 */
#define RETROSND_FLAG_INIT 0x01

/*! \} */ /* maug_retrosnd_flags */

#define RETROSND_VOICE_BREATH       122

#define RETROSND_VOICE_SEASHORE     123

#define RETROSND_VOICE_BIRD_TWEET   124

#define RETROSND_VOICE_PHONE_RING   125

#define RETROSND_VOICE_HELICOPTER   126

#define RETROSND_VOICE_APPLAUSE     127

/**
 * \brief Parameter for retrosnd_midi_set_voice() indicating a gunshot
 *        sound effect.
 */
#define RETROSND_VOICE_GUNSHOT      128

#define RETROSND_CHANNEL_CT         8

/**
 * \brief Initialize retrosnd engine.
 * \param args A pointer to the RETROSND_ARGS struct initialized by
 *        the calling program.
 *
 * The RETROSND_ARGS::snd_io_base field must be initialized with the address
 * or other platform-specific indicator of the MIDI device to use.
 */
MERROR_RETVAL retrosnd_init( struct RETROFLAT_ARGS* args );

/**
 * \brief Set the name of the voice bank filename to use.
 */
void retrosnd_set_sf_bank( const char* filename_in );

void retrosnd_midi_set_voice( uint8_t channel, uint8_t voice );

void retrosnd_midi_set_control( uint8_t channel, uint8_t key, uint8_t val );

void retrosnd_midi_note_on( uint8_t channel, uint8_t pitch, uint8_t vel );

void retrosnd_midi_note_off( uint8_t channel, uint8_t pitch, uint8_t vel );

MERROR_RETVAL retrosnd_midi_play_smf( const char* filename );

uint8_t retrosnd_midi_is_playing_smf();

void retrosnd_shutdown();

/*! \} */ /* maug_retrosnd */

#endif /* !RETROFLAT_NO_SOUND */

/* === Platform-specific APIs === */

/* The first call to these headers should just establish definitions (macros, defines, prototypes,
 * typedefs, etc). The later call below should then define function bodies.
 */
#ifndef RETROFLAT_NO_SOUND
#  include <retapis.h>
#endif /* !RETROFLAT_NO_SOUND */
#include <retapii.h>

/* === End platform-specific APIs === */



/* === OS-Specific Includes and Defines === */

#if defined( RETROFLAT_OS_WIN ) && !defined( MAUG_WINDOWS_H )
#  include <windows.h>
#  define MAUG_WINDOWS_H
#endif /* !MAUG_WINDOWS_H */

#if defined( RETROFLAT_BMP_TEX ) || defined( DOCUMENTATION )

/**
 * \addtogroup maug_retro3d_util
 * \{
 */

struct RETROFLAT_3DTEX {
   uint8_t flags;
   MAUG_MHANDLE bytes_h;
   uint8_t* bytes;
   uint32_t bpp;
   uint32_t sz;
   uint8_t* px;
   uint32_t id;
   size_t w;
   size_t h;
};

#endif /* RETROFLAT_BMP_TEX */

/* TODO: Migrate all platform-specific parts below to retapid.h. */
#include <retapid.h>

typedef maug_ms_t retroflat_ms_t;

#include "retrom2d.h"

/* === Structures === */

/* TODO: Break the args into API-specific headers. */

/*! \brief Struct containing configuration values for a RetroFlat program. */
struct RETROFLAT_ARGS {
   uint8_t flags;
   /**
    * \brief Title to set for the main program Window if applicable on the
    *        target platform.
    */
   char* title;
   /*! \brief Relative path under which bitmap assets are stored. */
   char* assets_path;
   /*! \brief Relative path of local config file (if not using registry). */
   char* config_path;
#  if !defined( RETROFLAT_NO_CLI_SZ )
   int screen_w;
   /*! \brief Desired screen or window height in pixels. */
   int screen_h;
   /*! \brief Desired window X position in pixels. */
   int screen_x;
   /*! \brief Desired window Y position in pixels. */
   int screen_y;
#  endif /* RETROFLAT_NO_CLI_SZ */
   struct RETROFLAT_PLATFORM_ARGS platform;
#  ifndef RETROFLAT_NO_SOUND
   struct RETROFLAT_SOUND_ARGS sound;
#  endif /* !RETROFLAT_NO_SOUND */
};

/**
 * \addtogroup maug_retroflt_viewport RetroFlat Viewport API
 * \brief A flexible API to facilitate tile-based views using hardware
 *        acceleration where available.
 *
 * If no hardware acceleration is available on the platform, then the platform
 * API header should define RETROFLAT_SOFT_VIEWPORT to enable the _generic
 * functions and suffixes for this functionality.
 * \{
 */

/**
 * \brief The viewport data struct.
 * \warning Many of the fields in this struct are calculated based on each
 *          other, and so updates should only be made through
 *          retroflat_viewport_set_pos_size()!
 */
struct RETROFLAT_VIEWPORT {
   /**
    * \brief X position of the viewport in real screen memory in pixels.
    *        Should only be retrieved through retroflat_viewport_screen_x() and
    *        set through retroflat_viewport_set_pos_size().
    * */
   int16_t screen_x;
   /**
    * \brief Y position of the viewport in real screen memory in pixels.
    *        Should only be retrieved through retroflat_viewport_screen_y() and
    *        set through retroflat_viewport_set_pos_size()
    * */
   int16_t screen_y;
   /**
    * \brief The X offset, in pixels, of the viewport on the world tilemap.
    *        Should only be retrieved through retroflat_viewport_world_x() and
    *        set through retroflat_viewport_set_world_pos().
    */
   int16_t world_x;
   /**
    * \brief The Y offset, in pixels, of the viewport on the world tilemap.
    *        Should only be retrieved through retroflat_viewport_world_y() and
    *        set through retroflat_viewport_set_world_pos().
    */
   int16_t world_y;
   /**
    * \brief The width of the entire world tilemap in pixels.
    *        Should only be retrieved through retroflat_viewport_world_w() and
    *        set through retroflat_viewport_set_world().
    */
   int16_t world_w;
   /**
    * \brief The height of the entire world tilemap in pixels.
    *        Should only be retrieved through retroflat_viewport_world_h() and
    *        set through retroflat_viewport_set_world().
    */
   int16_t world_h;
   /**
    * \brief Viewport width in pixels.
    *        Should only be retrieved through retroflat_viewport_screen_w() and
    *        set through retroflat_viewport_set_pos_size().
    */
   uint16_t screen_w;
   /**
    * \brief Viewport height in pixels.
    *        Should only be retrieved through retroflat_viewport_screen_w() and
    *        set through retroflat_viewport_set_pos_size().
    */
   uint16_t screen_h;
   /**
    * \brief Difference between viewport width and screen width in pixels.
    *        Should only be retrieved through
    *        retroflat_viewport_screen_w_remainder() and
    *        calculated through retroflat_viewport_set_pos_size().
    */
   uint16_t screen_w_remainder;
   /**
    * \brief Difference between viewport height and screen height in pixels.
    *        Should only be retrieved through
    *        retroflat_viewport_screen_h_remainder() and
    *        calculated through retroflat_viewport_set_pos_size().
    */
   uint16_t screen_h_remainder;
   /**
    * \brief The number of tiles across that fit in the viewport.
    *        Should only be retrieved through
    *        retroflat_viewport_screen_tile_w() and
    *        calculated through retroflat_viewport_set_pos_size().
    */
   int16_t screen_tile_w;
   /**
    * \brief The number of tiles high that fit in the viewport.
    *        Should only be retrieved through
    *        retroflat_viewport_screen_tile_h() and
    *        calculated through retroflat_viewport_set_pos_size().
    */
   int16_t screen_tile_h;
   int16_t world_tile_x;
   int16_t world_tile_y;
#ifndef RETROFLAT_NO_VIEWPORT_REFRESH
   MAUG_MHANDLE refresh_grid_h;
   /**
    * \brief A grid of tile values representing the last-drawn values on-screen.
    *
    * If the value for a tile in this grid matches the value about to be drawn,
    * the draw will be skipped. This increases performance a LOT on systems with
    * slow video memory access.
    *
    * This functionality may be disabled by defining the macro
    * RETROFLAT_NO_VIEWPORT_REFRESH on build.
    */
   retroflat_tile_t* refresh_grid;
#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */
};

#  define retroflat_screen_colors() (g_retroflat_state->screen_colors)

#ifndef DOCUMENTATION

#  define retroflat_viewport_world_x_generic() \
   (g_retroflat_state->viewport.world_x)

#  define retroflat_viewport_world_y_generic() \
   (g_retroflat_state->viewport.world_y)

#  define retroflat_viewport_world_tile_x_generic() \
   (g_retroflat_state->viewport.world_tile_x)

#  define retroflat_viewport_world_tile_y_generic() \
   (g_retroflat_state->viewport.world_tile_y)

#  define retroflat_viewport_world_w_generic() \
   (g_retroflat_state->viewport.world_w)

#  define retroflat_viewport_world_h_generic() \
   (g_retroflat_state->viewport.world_h)

#  define retroflat_viewport_screen_tile_w_generic() \
   (g_retroflat_state->viewport.screen_tile_w)

#  define retroflat_viewport_screen_tile_h_generic() \
   (g_retroflat_state->viewport.screen_tile_h)

#  define retroflat_viewport_screen_w_generic() \
   (g_retroflat_state->viewport.screen_w)

#  define retroflat_viewport_screen_h_generic() \
   (g_retroflat_state->viewport.screen_h)

#  define retroflat_viewport_screen_w_remainder_generic() \
   (g_retroflat_state->viewport.screen_w_remainder)

#  define retroflat_viewport_screen_h_remainder_generic() \
   (g_retroflat_state->viewport.screen_h_remainder)

#  define retroflat_viewport_set_world_generic( w, h ) \
   debug_printf( 1, "setting viewport size to %d x %d...", \
      (int16_t)(w), (int16_t)(h) ); \
   (g_retroflat_state->viewport.world_w) = w; \
   (g_retroflat_state->viewport.world_h) = h;

#  define retroflat_viewport_set_world_pos_generic( x, y ) \
   debug_printf( 1, "setting viewport world pos to %d, %d...", x, y ); \
   (g_retroflat_state->viewport.world_x) = x; \
   (g_retroflat_state->viewport.world_y) = y; \
   (g_retroflat_state->viewport.world_tile_x) = (x) >> RETROFLAT_TILE_W_BITS; \
   (g_retroflat_state->viewport.world_tile_y) = (y) >> RETROFLAT_TILE_H_BITS;

#  define retroflat_viewport_set_pos_size_generic( x_px, y_px, w_px, h_px ) \
   g_retroflat_state->viewport.screen_x = (x_px); \
   g_retroflat_state->viewport.screen_y = (y_px); \
   g_retroflat_state->viewport.screen_tile_w = \
      ((w_px) / RETROFLAT_TILE_W); \
   g_retroflat_state->viewport.screen_tile_h = \
      ((h_px) / RETROFLAT_TILE_H); \
   /* We're not adding the extra room here since this won't be used for
   * indexing or allocation but rather pixel detection.
   */ \
   g_retroflat_state->viewport.screen_w = \
      ((w_px) / RETROFLAT_TILE_W) * RETROFLAT_TILE_W; \
   g_retroflat_state->viewport.screen_h = \
      ((h_px) / RETROFLAT_TILE_H) * RETROFLAT_TILE_H; \
   g_retroflat_state->viewport.screen_w_remainder = \
      (x_px) + (w_px) - g_retroflat_state->viewport.screen_w; \
   g_retroflat_state->viewport.screen_h_remainder = \
      (y_px) + (h_px) - g_retroflat_state->viewport.screen_h;

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH

#  define retroflat_viewport_lock_refresh_generic() \
   if( NULL == g_retroflat_state->viewport.refresh_grid ) { \
      maug_mlock( \
         g_retroflat_state->viewport.refresh_grid_h, \
         g_retroflat_state->viewport.refresh_grid ); \
      maug_cleanup_if_null_lock( retroflat_tile_t*, \
         g_retroflat_state->viewport.refresh_grid ); \
   }

#  define retroflat_viewport_unlock_refresh_generic() \
   if( NULL != g_retroflat_state->viewport.refresh_grid ) { \
      maug_munlock( \
         g_retroflat_state->viewport.refresh_grid_h, \
         g_retroflat_state->viewport.refresh_grid ); \
   }

#  define _retroflat_viewport_refresh_tile_x( x_px ) \
   (((x_px) + RETROFLAT_TILE_W) >> RETROFLAT_TILE_W_BITS)

#  define _retroflat_viewport_refresh_tile_y( y_px ) \
   (((y_px) + RETROFLAT_TILE_H) >> RETROFLAT_TILE_H_BITS)

#  define retroflat_viewport_set_refresh_generic( x_px, y_px, tid ) \
   assert( NULL != g_retroflat_state->viewport.refresh_grid ); \
   if( \
      /* Expand the range by -1 to account for just off-screen tile. */ \
      -(RETROFLAT_TILE_W) <= x_px && -(RETROFLAT_TILE_H) <= y_px && \
      retroflat_screen_w() > x_px && \
      retroflat_screen_h() > y_px \
   ) { \
      assert( 0 < g_retroflat_state->viewport.screen_tile_w ); \
      assert( 0 <= (((y_px) + RETROFLAT_TILE_H) >> RETROFLAT_TILE_H_BITS) ); \
      assert( 0 <= (((x_px) + RETROFLAT_TILE_W) >> RETROFLAT_TILE_W_BITS) ); \
      g_retroflat_state->viewport.refresh_grid[ \
         /* Add +1 tile to make off-screen "-1" tile positive. */ \
         ((_retroflat_viewport_refresh_tile_y( y_px ) + 1) * \
            (g_retroflat_state->viewport.screen_tile_w + 2)) + \
               (_retroflat_viewport_refresh_tile_x( x_px ) + 1)] = tid; \
   }

#  define retroflat_viewport_tile_is_stale( x_px, y_px, tile_id ) \
      ((tile_id) != \
      g_retroflat_state->viewport.refresh_grid[ \
         ((_retroflat_viewport_refresh_tile_y( y_px ) + 1) * \
            (g_retroflat_state->viewport.screen_tile_w + 2)) + \
               (_retroflat_viewport_refresh_tile_x( x_px ) + 1)])

#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

uint8_t retroflat_viewport_move_x_generic( int16_t x );

uint8_t retroflat_viewport_move_y_generic( int16_t y );

uint8_t retroflat_viewport_focus_generic(
   size_t x1, size_t y1, size_t range, size_t speed );

#  define retroflat_viewport_screen_x_generic( world_x ) \
   (g_retroflat_state->viewport.screen_x + \
      ((world_x) - retroflat_viewport_world_x()))

#  define retroflat_viewport_screen_y_generic( world_y ) \
   (g_retroflat_state->viewport.screen_y + \
      ((world_y) - retroflat_viewport_world_y()))

#endif /* !DOCUMENTATION */

#if defined( RETROFLAT_SOFT_VIEWPORT ) || defined( DOCUMENTATION )

#  ifndef RETROFLAT_NO_VIEWPORT_REFRESH
      /* These clamp world coordinates to tile borders to allow refresh grid to
       * function properly (smooth-scrolling tiles will always be in motion).
       */

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current viewport X position in the world in pixels.
 */
#     define retroflat_viewport_world_x() \
         ((retroflat_viewport_world_x_generic() \
            >> RETROFLAT_TILE_W_BITS) << RETROFLAT_TILE_W_BITS)

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current viewport Y position in the world in pixels.
 */
#     define retroflat_viewport_world_y() \
         ((retroflat_viewport_world_y_generic() \
            >> RETROFLAT_TILE_H_BITS) << RETROFLAT_TILE_H_BITS)
#  else
#     define retroflat_viewport_world_x() retroflat_viewport_world_x_generic()
#     define retroflat_viewport_world_y() retroflat_viewport_world_y_generic()
#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

#     define retroflat_viewport_world_tile_x() \
         retroflat_viewport_world_tile_x_generic()
#     define retroflat_viewport_world_tile_y() \
         retroflat_viewport_world_tile_y_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current width of the world in pixels.
 */
#  define retroflat_viewport_world_w() \
   retroflat_viewport_world_w_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current height of the world in pixels.
 */
#  define retroflat_viewport_world_h() \
   retroflat_viewport_world_h_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current width of the world in tiles.
 */
#  define retroflat_viewport_screen_tile_w() \
   retroflat_viewport_screen_tile_w_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the current height of the world in tiles.
 */
#  define retroflat_viewport_screen_tile_h() \
   retroflat_viewport_screen_tile_h_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the width of the viewport in pixels.
 */
#  define retroflat_viewport_screen_w() \
   retroflat_viewport_screen_w_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the height of the viewport in pixels.
 */
#  define retroflat_viewport_screen_h() \
   retroflat_viewport_screen_h_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the difference in pixels between the viewport X + width and the
 *        screen width.
 */
#  define retroflat_viewport_screen_w_remainder() \
   retroflat_viewport_screen_w_remainder_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Return the difference in pixels between the viewport Y + height and
 *        the screen height.
 */
#  define retroflat_viewport_screen_h_remainder() \
   retroflat_viewport_screen_h_remainder_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Set the pixel width and height of the world so the viewport knows
 *        how far it may scroll.
 * \param w The width of the world in pixels (tile_width * map_tile_width).
 * \param h The height of the world in pixels (tile_height * map_tile_height).
 */
#  define retroflat_viewport_set_world( w, h ) \
   retroflat_viewport_set_world_generic( w, h )

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Set the position of the viewport in the world in pixels.
 */
#  define retroflat_viewport_set_world_pos( x, y ) \
   retroflat_viewport_set_world_pos_generic( x, y )

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Set the pixel width and height of the viewport, as well as some other
 *        dependent values frequently used in screen updates.
 */
#  define retroflat_viewport_set_pos_size( x_px, y_px, w_px, h_px ) \
   retroflat_viewport_set_pos_size_generic( x_px, y_px, w_px, h_px )

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Lock access to RETROFLAT_VIEWPORT::refresh_grid in memory.
 *
 * This should be called before making references to the refresh grid e.g. with
 * retroflat_viewport_tile_is_stale().
 */
#  define retroflat_viewport_lock_refresh() \
   retroflat_viewport_lock_refresh_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Unlock access to RETROFLAT_VIEWPORT::refresh_grid in memory.
 *
 * This should be called when references to the refresh grid are no longer in
 * use and may be invalidated by the system.
 */
#  define retroflat_viewport_unlock_refresh() \
   retroflat_viewport_unlock_refresh_generic()

/**
 * \relates RETROFLAT_VIEWPORT
 * \brief Set the tile at the given *screen* pixel coordinates to the given
 *        tile ID.
 *
 * If these coordinates are from the world, they should subtract
 * retroflat_viewport_screen_x()/retroflat_viewport_screen_y() first!
 *
 * When the viewport is redrawn, e.g. with retrotile_topdown_draw(), it will
 * not redraw this tile if the tile ID is still the same.
 */
#  define retroflat_viewport_set_refresh( x, y, tid ) \
   retroflat_viewport_set_refresh_generic( x, y, tid )

#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

/**
 * \brief Move the viewport in a direction or combination thereof so that
 *        it's focusing the given x1/y1 within the given range.
 * \param x1 The X coordinate to focus on.
 * \param y1 The Y coordinate to focus on.
 * \param range The number of pixels from the center of the screen to keep
 *        the given X and Y inside.
 * \param speed The increment by which to move the viewport if the given X and
 *        Y are *not* in focus.
 * \warning The speed parameter should always divide evenly into the tile size,
 *          or problems may occur!
 */
#  define retroflat_viewport_focus( x1, y1, range, speed ) \
   retroflat_viewport_focus_generic( x1, y1, range, speed )

/**
 * \brief Return the screenspace X coordinate at which something at the given
 *        world coordinate should be drawn.
 */
#  define retroflat_viewport_screen_x( world_x ) \
   retroflat_viewport_screen_x_generic( world_x )

/**
 * \brief Return the screenspace Y coordinate at which something at the given
 *        world coordinate should be drawn.
 */
#  define retroflat_viewport_screen_y( world_y ) \
   retroflat_viewport_screen_y_generic( world_y )

#  ifndef RETROFLAT_VIEWPORT_OVERRIDE_MOVE
#     define retroflat_viewport_move_x( x ) \
         retroflat_viewport_move_x_generic( x )

#     define retroflat_viewport_move_y( y ) \
         retroflat_viewport_move_y_generic( y )
#  endif /* !RETROFLAT_VIEWPORT_OVERRIDE_MOVE */

#endif /* RETROFLAT_SOFT_VIEWPORT || DOCUMENTATION */

/*! \} */

/**
 * \relates RETROFLAT_STATE
 * \brief Set parameters for the RETROFLAT_STATE::heartbeat_frame.
 * \param len Number of ms between updates to RETROFLAT_STATE::heartbeat_frame.
 * \param max Value of RETROFLAT_STATE::heartbeat_frame on which to reset to 0.
 */
#define retroflat_heartbeat_set( len, max ) \
   g_retroflat_state->heartbeat_max = max; \
   g_retroflat_state->heartbeat_len = len;

/**
 * \relates RETROFLAT_STATE
 * \brief Get current value of RETROFLAT_STATE::heartbeat_frame
 */
#define retroflat_heartbeat() (g_retroflat_state->heartbeat_frame)

/**
 * \relates RETROFLAT_STATE
 * \brief Check and update RETROFLAT_STATE::heartbeat_frame. This should be
 *        called in the API HAL on every iteration of the main loop (this is
 *        done automatically in the generic main loop).
 */
#define retroflat_heartbeat_update() \
   /* Update the heartbeat animation frame. */ \
   if( g_retroflat_state->heartbeat_next <= retroflat_get_ms() ) { \
      g_retroflat_state->heartbeat_frame++; \
      if( \
         g_retroflat_state->heartbeat_frame >= \
            g_retroflat_state->heartbeat_max \
      ) { \
         g_retroflat_state->heartbeat_frame = 0; \
      } \
      g_retroflat_state->heartbeat_next = \
         retroflat_get_ms() + g_retroflat_state->heartbeat_len; \
   }

/**
 * \brief Global singleton containing state for the current platform.
 * 
 * The first few members are size and offset info that the VDP can use to work
 * with STATE structs from different kinds of binaries without crashing.
 */
struct RETROFLAT_STATE {
   /* TODO: Set this up in the initialization function! */
   /* TODO: We probably need more of these. */
   size_t                  sz;
   size_t                  offset_pal;
   size_t                  offset_tex_pal;

   void*                   loop_data;
   MERROR_RETVAL           retval;
   /*! \brief \ref maug_retroflt_flags indicating current system status. */
   uint8_t                 retroflat_flags;
   char                    config_path[RETROFLAT_PATH_MAX + 1];
   char                    assets_path[RETROFLAT_ASSETS_PATH_MAX + 1];
   /*! \brief Off-screen buffer bitmap. */
   struct RETROFLAT_BITMAP buffer;
   int scale;

#  if defined( RETROFLAT_VDP ) || defined( DOCUMENTATION ) || \
defined( RETROVDP_C )
   /**
    * \brief A buffer assembled and passed to the \ref maug_retroflt_vdp for
    *        it to modify, or NULL if no VDP is loaded.
    */
   struct RETROFLAT_BITMAP* vdp_buffer;
#     ifdef RETROFLAT_OS_WIN
   HMODULE vdp_exe;
#     else
   /*! \brief A handle for the loaded \ref maug_retroflt_vdp module. */
   void* vdp_exe;
#     endif /* RETROFLAT_OS_WIN */
   /**
    * \brief Pointer to data defined by the \ref maug_retroflt_vdp for its
    *        use.
    */
   void* vdp_data;
   /*! \brief CLI args passed with -vdp to the \ref maug_retroflt_vdp. */
   char vdp_args[RETROFLAT_VDP_ARGS_SZ_MAX];
   /*! \brief Flags set by the \ref maug_retroflt_vdp. */
   uint8_t vdp_flags;
#  endif /* RETROFLAT_VDP || DOCUMENTATION || RETROVDP_C */

   /* These are used by VDP so should be standardized/not put in plat-spec! */

   struct RETROFLAT_VIEWPORT viewport;

   /**
    * \brief The screen width as seen by our program, before scaling.
    *
    * This is the scale of the buffer, which the platform-specific code
    * should then scale to match screen_v_h on blit.
    */
   size_t               screen_v_w;
   /**
    * \brief The screen height as seen by our program, before scaling.
    *
    * This is the scale of the buffer, which the platform-specific code
    * should then scale to match screen_v_w on blit.
    */
   size_t               screen_v_h;
   /*! \brief The screen width as seen by the system, after scaling. */
   size_t               screen_w;
   /*! \brief The screen height as seen by the system, after scaling. */
   size_t               screen_h;
   /*! \brief The number of colors the screen is capable of displaying. */
   size_t               screen_colors;

   /* WARNING: The VDP requires the state specifier to be the same size
    *          as the one it was compiled for! Do not modify above here!
    */

   /* TODO: Put these in a platform-specific struct of some kind to maintain
    *       consistent state struct size for VDP?
    */

   retroflat_ms_t heartbeat_next;
   /**
    * \brief Number of ms to stay on current value of
    *        RETROFLAT_STATE::heartbeat_frame before incrementing. Modify with
    *        retroflat_heartbeat_set().
    */
   uint16_t heartbeat_len;
   /**
    * \brief Simple iteration loop that can be used to time e.g. perpetual
    *        sprite animations. Modify parameters with
    *        retroflat_heartbeat_set().
    */
   uint8_t heartbeat_frame;
   /**
    * \brief When RETROFLAT_STATE::heartbeat_frame reaches this value, it will
    *        reset to 0.
    */
   uint8_t heartbeat_max;

   retroflat_proc_resize_t on_resize;
   void* on_resize_data;

#ifndef RETROFLAT_OPENGL
   /*! \brief Index of available colors, initialized on platform init. */
   RETROFLAT_COLOR_DEF     palette[RETROFLAT_COLORS_SZ];
#endif /* !RETROFLAT_OPENGL */

   retroflat_loop_iter  loop_iter;
   retroflat_loop_iter  frame_iter;

   struct RETROFLAT_PLATFORM platform;

#  if defined( RETROFLAT_3D )
   /* This allows native colors to be used for things like glColor3fv while
    * these colors are used to manipulate textures passed through
    * retroflat_bitmap_*()
    */
   uint8_t tex_palette[RETROFLAT_COLORS_SZ][3];
#  endif /* RETROFLAT_OPENGL */

   struct RETROFLAT_INPUT_STATE input;

#  ifndef RETROFLAT_NO_SOUND
   struct RETROFLAT_SOUND_STATE sound;
#  endif /* !RETROFLAT_NO_SOUND */
};

/* === Translation Module === */

/* Declare the prototypes so that internal functions can call each other. */

#  ifdef retroflat_loop
MERROR_RETVAL retroflat_loop_generic(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data );
#  else
/**
 * \brief This should be called once in the main body of the program in order
 *        to enter the main loop. The main loop will continuously call
 *        loop_iter with data as an argument until retroflat_quit() is called.
 */
MERROR_RETVAL retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data );
#  endif /* retroflat_loop */

/**
 * \brief Display a message in a dialog box and/or on stderr.
 * \param title A string with the title to use for a dialog box.
 * \param format A format string to be passed to vsnprintf().
 * \todo  This should display a dialog box on every platform if possible.
 */
void retroflat_message(
   uint8_t flags, const char* title, const char* format, ... );

/**
 * \brief Initialize RetroFlat and its underlying layers. This should be
 *        called once at the beginning of the program and should quit if
 *        the return value indicates any failures.
 * \return ::RETROFLAT_OK if there were no problems or other
 *         \ref maug_retroflt_retval if there were.
 */
MERROR_RETVAL retroflat_init( int argc, char* argv[], struct RETROFLAT_ARGS* args );

/**
 * \brief Deinitialize RetroFlat and its underlying layers. This should be
 *        called once at the end of the program, after retroflat_loop().
 * \param retval Return value from retroflat_init(), so we know what layers
 *        are in what state.
 */
void retroflat_shutdown( int retval );

#  if defined( RETROFLAT_VDP ) || defined( DOCUMENTATION )
/**
 * \addtogroup maug_retroflt_vdp
 * \{
 */

/**
 * \brief Call a function from the retroflat VDP.
 */
MERROR_RETVAL retroflat_vdp_call( const char* proc_name );

/*! \} */ /* maug_retroflt_vdp */
#  endif /* RETROFLAT_VDP || DOCUMENTATION */

void retroflat_set_title( const char* format, ... );

retroflat_ms_t retroflat_get_ms();

uint32_t retroflat_get_rand();

#  if !defined( RETROFLAT_NO_KEYBOARD )
char retroflat_vk_to_ascii( RETROFLAT_IN_KEY k, uint8_t flags );
#  endif /* !RETROFLAT_NO_KEYBOARD */

/**
 * \addtogroup maug_retroflt_bitmap
 * \{
 */

/**
 * \brief Load a bitmap into the given ::RETROFLAT_BITMAP structure if
 *        it is available. Bitmaps are subject to the limitations enumerated in
 *        \ref maug_retroflt_bitmap.
 * \param filename Filename of the bitmap under RETROFLAT_ARGS::assets_path
 *                 with no separator, ".", or ::RETROFLAT_BITMAP_EXT.
 * \param bmp_out Pointer to a ::RETROFLAT_BITMAP to load the bitmap into.
 * \return ::RETROFLAT_OK if the bitmap was loaded or ::RETROFLAT_ERROR_TIMER if
 *         there was a problem (e.g. the bitmap was not found).
 */
MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags );

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags );

/**
 * \brief Unload a bitmap from a ::RETROFLAT_BITMAP struct. The struct, itself,
 *        is not freed (in case it is on the stack).
 * \param bitmap Pointer to the ::RETROFLAT_BITMAP to unload.
 */
void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bitmap );

/**
 * \brief Blit the contents of a ::RETROFLAT_BITMAP onto another
 *        ::RETROFLAT_BITMAP.
 * \param target Pointer to the ::RETROFLAT_BITMAP to blit src onto. This bitmap
 *               should be locked!
 * \param src Pointer to the ::RETROFLAT_BITMAP to blit onto target. This bitmap
 *            must *not* be locked.
 * \param s_x Left X coordinate to blit starting from on the source bitmap.
 * \param s_y Top Y coordinate to blit starting from on the source bitmap.
 * \param d_x Left X coordinate to blit to on the target bitmap.
 * \param d_y Top Y coordinate to blit to on the target bitmap.
 * \param w Pixel width of the region to blit.
 * \param h Pixel height of the region to blit.
 * \todo Currently s_x, s_y, w, and h are not functional on all platforms!
 */
MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance );

/**
 * \brief Ensure x and y (which must be unsigned!) are inside image boundaries.
 *
 * This also relies on the platform-specific retroflat_bitmap_w() and
 * retroflat_bitmap_h() macros being smart enough to measure the screen buffer
 * if bmp is NULL.
 */
#define retroflat_constrain_px( x, y, bmp, retact ) \
   if( \
      x >= retroflat_bitmap_w( bmp ) || y >= retroflat_bitmap_h( bmp ) \
   ) { retact; }

/*! \} */ /* maug_retroflt_bitmap */

/**
 * \addtogroup maug_retroflt_drawing
 * \{
 */

/**
 * \brief Lock a bitmap for drawing. This will be done automatically if
 *        necessary and not called explicitly, but performance should
 *        improve if done before a batch of drawing operations.
 * \param bmp Pointer to a ::RETROFLAT_BITMAP struct to lock. If this is NULL,
 *            try to lock the screen.
 * \return ::RETROFLAT_OK if lock was successful or \ref maug_retroflt_retval
 *         otherwise.
 */
MERROR_RETVAL retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp );

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp );

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   size_t x, size_t y, uint8_t flags );

#ifdef RETROFLAT_SOFT_SHAPES
#  ifdef RETROFLAT_OPENGL
/* Make sure we're not passing NULL to openGL texture drawers... they can't
 * handle that!
 */
#     define retroflat_rect( t, c, x, y, w, h, f ) \
      assert( NULL != t ); \
      retrosoft_rect( t, c, x, y, w, h, f );
#     define retroflat_ellipse( t, c, x, y, w, h, f ) \
      assert( NULL != t ); \
      retrosoft_ellipse( t, c, x, y, w, h, f )
#  else
#     define retroflat_rect( t, c, x, y, w, h, f ) \
      retrosoft_rect( t, c, x, y, w, h, f )
#     define retroflat_ellipse( t, c, x, y, w, h, f ) \
      retrosoft_ellipse( t, c, x, y, w, h, f )
#  endif /* RETROFLAT_3D */
#else

/**
 * \brief Draw a rectangle onto the target ::RETROFLAT_BITMAP.
 * \param target Pointer to the ::RETROFLAT_BITMAP to draw onto, or NULL to
 *        draw to the screen buffer.
 * \param color \ref maug_retroflt_color in which to draw.
 * \param x Left X coordinate in pixels at which to draw on the target bitmap.
 * \param y Top Y coordinate in pixels at which to draw on the target bitmap.
 * \param flags Flags to control drawing. The following flags apply:
 *        ::RETROFLAT_FLAGS_FILL
 */
void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags );

/**
 * \brief Draw an ellipse onto the target ::RETROFLAT_BITMAP.
 * \param target Pointer to the ::RETROFLAT_BITMAP to draw onto, or NULL to
 *        draw to the screen buffer.
 * \param color \ref maug_retroflt_color in which to draw.
 * \param x Left X coordinate in pixels at which to draw on the target bitmap.
 * \param y Top Y coordinate in pixels at which to draw on the target bitmap.
 * \param flags Flags to control drawing. The following flags apply:
 *        ::RETROFLAT_FLAGS_FILL
 */
void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags );

#endif /* RETROFLAT_SOFT_SHAPES */

#ifdef RETROFLAT_SOFT_LINES
#  define retroflat_line( t, c, x1, y1, x2, y2, f ) \
   retrosoft_line( t, c, x1, y1, x2, y2, f )
#else

/**
 * \brief Draw a straight line onto the target ::RETROFLAT_BITMAP.
 * \param target Pointer to the ::RETROFLAT_BITMAP to draw onto, or NULL to
 *        draw to the screen buffer.
 * \param x1 Left X coordinate of the line to draw on the target bitmap.
 * \param y1 Top Y coordinate of the line to draw on the target bitmap.
 * \param x2 Right X coordinate of the line to draw on the target bitmap.
 * \param y2 Bottom Y coordinate of the line to draw on the target bitmap.
 * \param color \ref maug_retroflt_color in which to draw.
 * \param flags Flags to control drawing. No flags currently apply.
 */
void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t flags );

#endif /* RETROFLAT_SOFT_LINES */

void retroflat_cursor( struct RETROFLAT_BITMAP* target, uint8_t flags );

/**
 * \brief Get the size in pixels of a text string when drawn with a given font
 *        by retroflat_string().
 * \warning Font specifications for font_str may vary by platform, so be sure
 *          to test with #ifdefs for each!
 * \param target Pointer to the ::RETROFLAT_BITMAP on which drawing is planned.
 * \param str The text string to get the size of.
 * \param str_sz Length of the string to get the size of in characters.
 * \param font_str Name of the font in which to size the string.
 * \param w_out Pointer to an int in which to store the string width in pixels.
 * \param h_out Pointer to an int in which to store the string height in pixels.
 */
void retroflat_string_sz(
   struct RETROFLAT_BITMAP* target, const char* str, size_t str_sz,
   const char* font_str, size_t* w_out, size_t* h_out, uint8_t flags );

/**
 * \brief Draw a text string at the specified location in the specified font
 *        and color on the target ::RETROFLAT_BITMAP.
 * \warning Font specifications for font_str may vary by platform, so be sure
 *          to test with #ifdefs for each!
 * \param target Pointer to the ::RETROFLAT_BITMAP to draw onto, or NULL to
 *        draw to the screen buffer.
 * \param color \ref maug_retroflt_color in which to draw.
 * \param str The text string to draw to the target bitmap.
 * \param str_sz Length of the string to draw in characters.
 * \param font_str Name of the font in which to draw the string.
 * \param x_orig Left X coordinate in pixels at which to draw on the target
 *               bitmap.
 * \param y_orig Top Y coordinate in pixels at which to draw on the target
 *               bitmap.
 * \param flags Flags to control drawing. No flags currently apply.
 */
void retroflat_string(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   const char* str, int str_sz, const char* font_str, int16_t x_orig, int16_t y_orig,
   uint8_t flags );

/* TODO: Documentation! */
void retroflat_get_palette( uint8_t idx, uint32_t* rgb );

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb );

/*! \} */ /* maug_retroflt_bitmap */

/**
 * \brief Set the procedure to call when the window is resized (on platforms
 *        that support resizing).
 * \param on_resize_in Procedure to call when window is resized.
 * \param data_in Data to pass to on_resize_in.
 *
 * If this is not set, no procedure is called. Some platforms may stretch the
 * "virtual" screen to fill the physical window.
 */
void retroflat_set_proc_resize(
   retroflat_proc_resize_t on_resize_in, void* data_in );

/**
 * \brief Platform-specific function to resize virtual screen to match
 *        physical window size.
 */
void retroflat_resize_v();

/**
 * \addtogroup maug_retroflt_input
 * \{
 */

/**
 * \brief Poll input devices (keyboard/mouse) and return the latest event.
 * \param input Pointer to a ::RETROFLAT_INPUT struct to store extended info.
 * \return A symbol from \ref maug_retroflt_keydefs.
 */
RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input );

/*! \} */ /* maug_retroflt_input */

#ifdef RETROFLT_C

MAUG_CONST int16_t SEG_MCONST gc_retroflat_offsets8_x[8] =
   {  0,  1, 1, 1, 0, -1, -1, -1 };
MAUG_CONST int16_t SEG_MCONST gc_retroflat_offsets8_y[8] =
   { -1, -1, 0, 1, 1,  1,  0, -1 };

MAUG_CONST int16_t SEG_MCONST gc_retroflat_offsets4_x[4] =
   {  0, 1, 0, -1 };
MAUG_CONST int16_t SEG_MCONST gc_retroflat_offsets4_y[4] =
   { -1, 0, 1,  0 };

MAUG_MHANDLE g_retroflat_state_h = (MAUG_MHANDLE)NULL;
struct RETROFLAT_STATE* g_retroflat_state = NULL;

#  define RETROFLAT_COLOR_TABLE_CONSTS( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      MAUG_CONST RETROFLAT_COLOR RETROFLAT_COLOR_ ## name_u = idx;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CONSTS )

#  define RETROFLAT_COLOR_TABLE_NAMES( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      #name_u,

MAUG_CONST char* SEG_MCONST gc_retroflat_color_names[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_NAMES )
};

/* Callback table is down below, after the statically-defined callbacks. */

/* === Function Definitions === */

MERROR_RETVAL retroflat_build_filename_path(
   const char* filename_in,
   char* buffer_out, size_t buffer_out_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( 1 < buffer_out_sz );

   /* Build the path to the bitmap. */
   memset( buffer_out, '\0', buffer_out_sz );
   if(
      RETROFLAT_FLAGS_LITERAL_PATH ==
      (RETROFLAT_FLAGS_LITERAL_PATH & flags)
   ) {
      /* TODO: Error checking. */
      maug_snprintf( buffer_out, buffer_out_sz - 1, "%s", filename_in );
   } else {
      /* TODO: Error checking. */
      maug_snprintf( buffer_out, buffer_out_sz - 1, "%s%c%s.%s",
         g_retroflat_state->assets_path, RETROFLAT_PATH_SEP,
         filename_in, RETROFLAT_BITMAP_EXT );
   }

   return retval;
}

/* === */

#  if (defined( RETROFLAT_SOFT_SHAPES ) || defined( RETROFLAT_SOFT_LINES ) || \
   defined( RETROFLAT_3D ))
/* RETROSOFT_PRESENT is different from RETROFLAT_SOFT_SHAPES in that it only
 * indicates that the retrosoft library is loaded, not that it is the default
 * for drawing primatives!
 */
#     define RETROSOFT_PRESENT
#  endif

#  if defined( RETROFLAT_3D )
#     if !defined( MAUG_NO_AUTO_C )
#        define RETRO3D_C
#        define RETRO3DP_C
#        define RETROFP_C
#     endif /* MAUG_NO_AUTO_C */
#     include <retro3dp.h>
#     include <retro3d.h>
#     include <retro3du.h>
#     include <retapi3.h>
#  endif /* RETROFLAT_3D */

#  ifdef RETROSOFT_PRESENT
#     if !defined( MAUG_NO_AUTO_C )
#        define RETROSFT_C
#     endif /* !MAUG_NO_AUTO_C */
#     define RETROSOFT_PRESENT
#     include <retrosft.h>
#  endif /* RETROFLAT_SOFT_SHAPES */

#  ifndef RETROFLAT_NO_SOUND
#  include <retapis.h>
#  endif /* !RETROFLAT_NO_SOUND */

#  include <retapii.h>

#  if defined( RETROFLAT_VDP ) && defined( RETROFLAT_OS_UNIX )
#     include <dlfcn.h>
#  endif

/* Still inside RETROFLT_C! */

/* === */

#ifndef RETROFLAT_NO_GENERIC_LOOP

MERROR_RETVAL retroflat_loop_generic(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;
   retroflat_ms_t next = 0,
      now = 0;

   g_retroflat_state->loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;
   g_retroflat_state->frame_iter = (retroflat_loop_iter)frame_iter;

   if(
      RETROFLAT_FLAGS_RUNNING ==
      (g_retroflat_state->retroflat_flags & RETROFLAT_FLAGS_RUNNING)
   ) {
      /* Main loop is already running, so we're just changing the iter call
       * and leaving!
       */
      debug_printf( 1, "main loop already running!" );
      goto cleanup;
   }

   g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_RUNNING;
   do {
      if(
         /* Not waiting for the next frame? */
         RETROFLAT_FLAGS_WAIT_FOR_FPS !=
         (RETROFLAT_FLAGS_WAIT_FOR_FPS & g_retroflat_state->retroflat_flags) &&
         /* Inter-frame loop present? */
         NULL != g_retroflat_state->loop_iter
      ) {
         /* Run the loop iter as many times as possible. */
         g_retroflat_state->loop_iter( g_retroflat_state->loop_data );
      }
      if(
         RETROFLAT_FLAGS_UNLOCK_FPS !=
         (RETROFLAT_FLAGS_UNLOCK_FPS & g_retroflat_state->retroflat_flags) &&
         retroflat_get_ms() < next
      ) {
         /* Sleep/low power for a bit. */
         continue;
      }

      retroflat_heartbeat_update();

      if( NULL != g_retroflat_state->frame_iter ) {
         /* Run the frame iterator once per FPS tick. */
         g_retroflat_state->frame_iter( g_retroflat_state->loop_data );
      }
      /* Reset wait-for-frame flag AFTER frame callback. */
      g_retroflat_state->retroflat_flags &= ~RETROFLAT_FLAGS_WAIT_FOR_FPS;
      now = retroflat_get_ms();
      if( now + retroflat_fps_next() > now ) {
         next = now + retroflat_fps_next();
      } else {
         /* Rollover protection. */
         /* TODO: Add difference from now/next to 0 here. */
         next = 0;
      }
   } while(
      RETROFLAT_FLAGS_RUNNING == 
         (RETROFLAT_FLAGS_RUNNING & g_retroflat_state->retroflat_flags)
   );
   retval = g_retroflat_state->retval;

cleanup:

   /* This should be set by retroflat_quit(). */
   return retval;
}

#endif /* !RETROFLAT_NO_GENERIC_LOOP */

/* === */

#  if !defined( RETROFLAT_NO_KEYBOARD )

char retroflat_vk_to_ascii( RETROFLAT_IN_KEY k, uint8_t flags ) {
   char c = 0;
   char offset_lower = 0;

   if( RETROFLAT_INPUT_MOD_SHIFT != (RETROFLAT_INPUT_MOD_SHIFT & flags) ) {
      /* Shift is *not* being held down. */

      if( RETROFLAT_KEY_A <= k && RETROFLAT_KEY_Z >= k ) {
         if( 
            RETROFLAT_INPUT_FORCE_UPPER !=
               (RETROFLAT_INPUT_FORCE_UPPER & flags)
         ) {
            /* Key is alphabetical and we're not forcing uppercase. */
            offset_lower = 0x20;
         }
      } else {
         offset_lower = 1;
      }
   }
   
   switch( k ) {
   case RETROFLAT_KEY_A: c = 0x41 + offset_lower; break;
   case RETROFLAT_KEY_B: c = 0x42 + offset_lower; break;
   case RETROFLAT_KEY_C: c = 0x43 + offset_lower; break;
   case RETROFLAT_KEY_D: c = 0x44 + offset_lower; break;
   case RETROFLAT_KEY_E: c = 0x45 + offset_lower; break;
   case RETROFLAT_KEY_F: c = 0x46 + offset_lower; break;
   case RETROFLAT_KEY_G: c = 0x47 + offset_lower; break;
   case RETROFLAT_KEY_H: c = 0x48 + offset_lower; break;
   case RETROFLAT_KEY_I: c = 0x49 + offset_lower; break;
   case RETROFLAT_KEY_J: c = 0x4a + offset_lower; break;
   case RETROFLAT_KEY_K: c = 0x4b + offset_lower; break;
   case RETROFLAT_KEY_L: c = 0x4c + offset_lower; break;
   case RETROFLAT_KEY_M: c = 0x4d + offset_lower; break;
   case RETROFLAT_KEY_N: c = 0x4e + offset_lower; break;
   case RETROFLAT_KEY_O: c = 0x4f + offset_lower; break;
   case RETROFLAT_KEY_P: c = 0x50 + offset_lower; break;
   case RETROFLAT_KEY_Q: c = 0x51 + offset_lower; break;
   case RETROFLAT_KEY_R: c = 0x52 + offset_lower; break;
   case RETROFLAT_KEY_S: c = 0x53 + offset_lower; break;
   case RETROFLAT_KEY_T: c = 0x54 + offset_lower; break;
   case RETROFLAT_KEY_U: c = 0x55 + offset_lower; break;
   case RETROFLAT_KEY_V: c = 0x56 + offset_lower; break;
   case RETROFLAT_KEY_W: c = 0x57 + offset_lower; break;
   case RETROFLAT_KEY_X: c = 0x58 + offset_lower; break;
   case RETROFLAT_KEY_Y: c = 0x59 + offset_lower; break;
   case RETROFLAT_KEY_Z: c = 0x60 + offset_lower; break;
   case RETROFLAT_KEY_0: c = offset_lower ? 0x30 : ')'; break;
   case RETROFLAT_KEY_1: c = offset_lower ? 0x31 : '!'; break;
   case RETROFLAT_KEY_2: c = offset_lower ? 0x32 : '@'; break;
   case RETROFLAT_KEY_3: c = offset_lower ? 0x33 : '#'; break;
   case RETROFLAT_KEY_4: c = offset_lower ? 0x34 : '$'; break;
   case RETROFLAT_KEY_5: c = offset_lower ? 0x35 : '%'; break;
   case RETROFLAT_KEY_6: c = offset_lower ? 0x36 : '^'; break;
   case RETROFLAT_KEY_7: c = offset_lower ? 0x37 : '&'; break;
   case RETROFLAT_KEY_8: c = offset_lower ? 0x38 : '*'; break;
   case RETROFLAT_KEY_9: c = offset_lower ? 0x39 : '('; break;
   case RETROFLAT_KEY_SPACE: c = ' '; break;
   case RETROFLAT_KEY_BKSP: c = 0x08; break;
   case RETROFLAT_KEY_ENTER: c = '\n'; break;
   case RETROFLAT_KEY_SEMICOLON: c = offset_lower ? ';' : ':'; break;
   case RETROFLAT_KEY_DASH: c = offset_lower ? '-' : '_'; break;
   case RETROFLAT_KEY_SLASH: c = offset_lower ? '/' : '?'; break;
   case RETROFLAT_KEY_PERIOD: c = offset_lower ? '.' : '>'; break;
   case RETROFLAT_KEY_COMMA: c = offset_lower ? ',' : '<'; break;
   case RETROFLAT_KEY_QUOTE: c = offset_lower ? '\'' : '"'; break;
   case RETROFLAT_KEY_EQUALS: c = offset_lower ? '=' : '+'; break;
   case RETROFLAT_KEY_BACKSLASH: c = offset_lower ? '\\' : '|'; break;
   case RETROFLAT_KEY_BRACKETL: c = offset_lower ? '[' : '{'; break;
   case RETROFLAT_KEY_BRACKETR: c = offset_lower ? ']' : '}'; break;
#ifndef RETROFLAT_API_PC_BIOS
   /* TODO: FIXME in DOS! */
   case RETROFLAT_KEY_GRAVE: c = offset_lower ? '`' : '~'; break;
#endif /* !RETROFLAT_API_PC_BIOS */
   }

   debug_printf( RETROFLAT_KB_TRACE_LVL, "0x%02x", c );

   return c;
}

#endif /* !RETROFLAT_NO_KEYBOARD */

/* === */

/* TODO: Migrate all platform-specific parts below to retapif.h. */
#include <retapif.h>

/* === */

#  ifndef RETROFLAT_NO_CLI

#  if !defined( RETROFLAT_NO_SOUND ) && defined( RETROSND_ARGS )

static MERROR_RETVAL retrosnd_cli_rsl(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if(
      0 <= arg_c &&
      0 == strncmp( MAUG_CLI_SIGIL "rsl", arg, MAUG_CLI_SIGIL_SZ + 4 )
   ) {
      args->sound.flags |= RETROSND_ARGS_FLAG_LIST_DEVS;
   }
   return MERROR_OK;
}

# endif /* !RETROFLAT_NO_SOUND && RETROSND_ARGS */

#  if !defined( RETROFLAT_API_PC_BIOS ) && !defined( RETROFLAT_NO_CLI_SZ )

static MERROR_RETVAL retroflat_cli_rfs(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 0 < arg_c ) {
      g_retroflat_state->scale = atoi( arg );
      debug_printf( 3, "screen scale set to: %d",
         g_retroflat_state->scale );
   }
   return MERROR_OK;
}

static MERROR_RETVAL retroflat_cli_rfx(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 0 > arg_c ) {
      if( 0 == args->screen_w ) {
         args->screen_x = 0;
      }
   } else if(
      0 == strncmp( MAUG_CLI_SIGIL "rfx", arg, MAUG_CLI_SIGIL_SZ + 4 )
   ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_x = atoi( arg );
   }
   return MERROR_OK;
}

static MERROR_RETVAL retroflat_cli_rfy(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 0 > arg_c ) {
      if( 0 == args->screen_h ) {
         args->screen_y = 0;
      }
   } else if(
      0 == strncmp( MAUG_CLI_SIGIL "rfy", arg, MAUG_CLI_SIGIL_SZ + 4 )
   ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_y = atoi( arg );
   }
   return MERROR_OK;
}

static MERROR_RETVAL retroflat_cli_rfw(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 0 > arg_c ) {
      if( 0 == args->screen_w ) {
         args->screen_w = RETROFLAT_DEFAULT_SCREEN_W;
      }
   } else if(
      0 == strncmp( MAUG_CLI_SIGIL "rfw", arg, MAUG_CLI_SIGIL_SZ + 4 )
   ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_w = atoi( arg );
   }
   return MERROR_OK;
}

static MERROR_RETVAL retroflat_cli_rfh(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 0 > arg_c ) {
      if( 0 == args->screen_h ) {
         args->screen_h = RETROFLAT_DEFAULT_SCREEN_H;
      }
   } else if(
      0 == strncmp( MAUG_CLI_SIGIL "rfh", arg, MAUG_CLI_SIGIL_SZ + 4 )
   ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_h = atoi( arg );
   }
   return MERROR_OK;
}

#  endif /* !RETROFLAT_API_PC_BIOS && !RETROFLAT_NO_CLI_SZ */

#  ifdef RETROFLAT_VDP
static MERROR_RETVAL retroflat_cli_vdp(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "vdp", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* Next arg is VDP args str. */
   } else {
      maug_strncpy( g_retroflat_state->vdp_args, arg, RETROFLAT_VDP_ARGS_SZ_MAX );
      debug_printf( 1, "VDP args: %s", g_retroflat_state->vdp_args );
   }
   return MERROR_OK;
}
#  endif /* RETROFLAT_VDP */

static MERROR_RETVAL retroflat_cli_u(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 0 > arg_c ) {
      args->flags &= ~RETROFLAT_FLAGS_UNLOCK_FPS;
   } else if(
      0 == strncmp( MAUG_CLI_SIGIL "rfu", arg, MAUG_CLI_SIGIL_SZ + 4 )
   ) {
      debug_printf( 1, "unlocking FPS..." );
      args->flags |= RETROFLAT_FLAGS_UNLOCK_FPS;
   }
   return MERROR_OK;
}

#endif /* !RETROFLAT_NO_CLI */

/* === */

/* Still inside RETROFLT_C! */

int retroflat_init( int argc, char* argv[], struct RETROFLAT_ARGS* args ) {

   /* = Declare Init Vars = */

   int retval = 0;

   /* = Begin Init Procedure = */

#  ifdef RETROFLAT_COMMIT_HASH
   debug_printf( 1, "retroflat commit: " RETROFLAT_COMMIT_HASH  );
#  endif /* RETROFLAT_COMMIT_HASH */

   debug_printf( 1, "retroflat: initializing..." );

   /* System sanity checks. */
   assert( 2 <= sizeof( MERROR_RETVAL ) );
   assert( 4 == sizeof( uint32_t ) );
   assert( 4 == sizeof( int32_t ) );
   assert( 2 == sizeof( uint16_t ) );
   assert( 2 == sizeof( int16_t ) );
   assert( 1 == sizeof( uint8_t ) );
   assert( 1 == sizeof( int8_t ) );
   assert( NULL != args );
   assert( 1 << RETROFLAT_TILE_W_BITS == RETROFLAT_TILE_W );
   assert( 1 << RETROFLAT_TILE_H_BITS == RETROFLAT_TILE_H );

   /* Initialize 2D callbacks depending on if we're in 2D or 3D mode.
    * Please see retrom2d.h for more information.
    */
#     if defined( RETROFLAT_BMP_TEX )
   retroflat_2d_px = (retroflat_px_cb)retro3d_texture_px;
   retroflat_2d_line = (retroflat_line_cb)retrosoft_line;
   retroflat_2d_rect = (retroflat_rect_cb)retrosoft_rect;
   retroflat_2d_blit_bitmap = (retroflat_blit_bitmap_cb)retro3d_texture_blit;
   retroflat_2d_load_bitmap =
      (retroflat_load_bitmap_cb)retro3d_texture_load_bitmap;
   retroflat_2d_create_bitmap =
      (retroflat_create_bitmap_cb)retro3d_texture_create;
#     else
   retroflat_2d_px = (retroflat_px_cb)retroflat_px;
#        ifdef RETROFLAT_SOFT_SHAPES
   /* TODO: Work retrosoft routines to use retroflat_blit_t */
   retroflat_2d_line = (retroflat_line_cb)retrosoft_line;
   retroflat_2d_rect = (retroflat_rect_cb)retrosoft_rect;
#        else
   retroflat_2d_line = (retroflat_line_cb)retroflat_line;
   retroflat_2d_rect = (retroflat_rect_cb)retroflat_rect;
#        endif /* RETROFLAT_SOFT_SHAPES */
   retroflat_2d_blit_bitmap = (retroflat_blit_bitmap_cb)retroflat_blit_bitmap;
   retroflat_2d_load_bitmap = (retroflat_load_bitmap_cb)retroflat_load_bitmap;
   retroflat_2d_create_bitmap =
      (retroflat_create_bitmap_cb)retroflat_create_bitmap;
#     endif /* RETROFLAT_BMP_TEX */

   debug_printf( 1, "retroflat: MFIX_PRECISION: %f", MFIX_PRECISION );

   debug_printf( 1, "retroflat: allocating state (" SIZE_T_FMT " bytes)...",
      sizeof( struct RETROFLAT_STATE ) );

   debug_printf( 1, "retroflat: size_t is (" SIZE_T_FMT " bytes)...",
      sizeof( size_t ) );

   debug_printf( 1, "retroflat: ssize_t is (" SIZE_T_FMT " bytes)...",
      sizeof( ssize_t ) );

   debug_printf( 1, "retroflat: off_t is (" SIZE_T_FMT " bytes)...",
      sizeof( off_t ) );

   g_retroflat_state_h = maug_malloc( 1, sizeof( struct RETROFLAT_STATE ) );
   if( (MAUG_MHANDLE)NULL == g_retroflat_state_h ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Could not allocate global state!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   maug_mlock( g_retroflat_state_h, g_retroflat_state );
   if( (MAUG_MHANDLE)NULL == g_retroflat_state ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Could not lock global state!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }
   maug_mzero( g_retroflat_state, sizeof( struct RETROFLAT_STATE ) );

   retroflat_heartbeat_set( 1000, 2 );

   /* Set default, so that this is never zero, to avoid division by zero. */
   g_retroflat_state->scale = 1;

#  ifndef RETROFLAT_NO_CLI

   debug_printf( 1, "retroflat: parsing args..." );

   /* All platforms: add command-line args based on compile definitons. */

#  if !defined( RETROFLAT_NO_SOUND ) && defined( RETROSND_ARGS )
	retval = maug_add_arg( MAUG_CLI_SIGIL "rsd", MAUG_CLI_SIGIL_SZ + 4,
      "Select MIDI device", 0, (maug_cli_cb)retrosnd_cli_rsd, args );
   maug_cleanup_if_not_ok();
	retval = maug_add_arg( MAUG_CLI_SIGIL "rsl", MAUG_CLI_SIGIL_SZ + 4,
      "List MIDI devices", 0, (maug_cli_cb)retrosnd_cli_rsl, args );
   maug_cleanup_if_not_ok();
# endif /* !RETROFLAT_NO_SOUND && RETROSND_ARGS */

#     ifdef RETROFLAT_SCREENSAVER
	retval = maug_add_arg( MAUG_CLI_SIGIL "p", MAUG_CLI_SIGIL_SZ + 2,
      "Preview screensaver", 0, (maug_cli_cb)retroflat_cli_p, args );
   maug_cleanup_if_not_ok();
	retval = maug_add_arg( MAUG_CLI_SIGIL "s", MAUG_CLI_SIGIL_SZ + 2,
      "Launch screensaver", 0, (maug_cli_cb)retroflat_cli_s, args );
   maug_cleanup_if_not_ok();
#     endif /* RETROFLAT_SCREENSAVER */

#     ifdef RETROFLAT_API_PC_BIOS
   retval = maug_add_arg( MAUG_CLI_SIGIL "rfm", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen mode.", 0,
      (maug_cli_cb)retroflat_cli_rfm, args );
   maug_cleanup_if_not_ok();
#     elif !defined( RETROFLAT_NO_CLI_SZ )
   retval = maug_add_arg( MAUG_CLI_SIGIL "rfs", MAUG_CLI_SIGIL_SZ + 4,
      "Set screen scale factor.", 0,
      (maug_cli_cb)retroflat_cli_rfs, args );
   maug_cleanup_if_not_ok();
   retval = maug_add_arg( MAUG_CLI_SIGIL "rfx", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen X position.", 0,
      (maug_cli_cb)retroflat_cli_rfx, args );
   maug_cleanup_if_not_ok();
   retval = maug_add_arg( MAUG_CLI_SIGIL "rfy", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen Y position.", 0,
      (maug_cli_cb)retroflat_cli_rfy, args );
   maug_cleanup_if_not_ok();
   retval = maug_add_arg( MAUG_CLI_SIGIL "rfw", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen width.", 0,
      (maug_cli_cb)retroflat_cli_rfw, args );
   maug_cleanup_if_not_ok();
   retval = maug_add_arg( MAUG_CLI_SIGIL "rfh", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen height.", 0,
      (maug_cli_cb)retroflat_cli_rfh, args );
   maug_cleanup_if_not_ok();
#     endif /* !RETROFLAT_NO_CLI_SZ */

#     ifdef RETROFLAT_VDP
   retval = maug_add_arg( MAUG_CLI_SIGIL "vdp", MAUG_CLI_SIGIL_SZ + 4,
      "Pass a string of args to the VDP.", 0,
      (maug_cli_cb)retroflat_cli_vdp, args );
   maug_cleanup_if_not_ok();
#     endif /* RETROFLAT_VDP */

   retval = maug_add_arg( MAUG_CLI_SIGIL "rfu", MAUG_CLI_SIGIL_SZ + 4,
      "Unlock FPS.", 0,
      (maug_cli_cb)retroflat_cli_u, args );
   maug_cleanup_if_not_ok();

   /* Parse command line args. */
   retval = maug_parse_args( argc, argv );
   maug_cleanup_if_not_ok();

#  else

   args->screen_w = RETROFLAT_DEFAULT_SCREEN_W;
   args->screen_h = RETROFLAT_DEFAULT_SCREEN_H;

#  endif /* !RETROFLAT_NO_CLI */

   if(
      RETROFLAT_FLAGS_UNLOCK_FPS == (RETROFLAT_FLAGS_UNLOCK_FPS & args->flags)
   ) {
      g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_UNLOCK_FPS;
   }

   debug_printf( 1, "retroflat: setting config..." );

   /* Set the assets path. */
   memset( g_retroflat_state->assets_path, '\0', RETROFLAT_ASSETS_PATH_MAX );
   if( NULL != args->assets_path ) {
      maug_strncpy( g_retroflat_state->assets_path,
         args->assets_path, RETROFLAT_ASSETS_PATH_MAX );
   }

#  if defined( RETROFLAT_SCREENSAVER )
   if(
      RETROFLAT_FLAGS_SCREENSAVER ==
      (RETROFLAT_FLAGS_SCREENSAVER & args->flags)
   ) {
      g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_SCREENSAVER;
   }
#  endif /* RETROFLAT_SCREENSAVER */

#  if !defined( RETROFLAT_NO_CLI_SZ )
   /* Setup intended screen size. */
   g_retroflat_state->screen_v_w = args->screen_w;
   g_retroflat_state->screen_v_h = args->screen_h;
   g_retroflat_state->screen_w = args->screen_w;
   g_retroflat_state->screen_h = args->screen_h;
#  endif /* RETROFLAT_NO_CLI_SZ */

   /* == Platform-Specific Init == */

   retval = retroflat_init_platform( argc, argv, args );
   maug_cleanup_if_not_ok();

   debug_printf( 3, "screen initialized with: " SIZE_T_FMT "x" SIZE_T_FMT
      " pixels with " SIZE_T_FMT " colors",
      retroflat_screen_w(), retroflat_screen_h(), retroflat_screen_colors() );

   /* Setup the refresh grid, if requested, only after screen space has been
    * determined by the platform!
    */
   maug_cleanup_if_eq(
      (size_t)0, retroflat_screen_w(), SIZE_T_FMT, MERROR_GUI );
   maug_cleanup_if_eq(
      (size_t)0, retroflat_screen_h(), SIZE_T_FMT, MERROR_GUI );
   maug_cleanup_if_eq(
      (size_t)0, retroflat_screen_colors(), SIZE_T_FMT, MERROR_GUI );

   /* This is intended as a default and can be modified by calling this macro
    * again later.
    */
   retroflat_viewport_set_pos_size(
      0, 0, retroflat_screen_w(), retroflat_screen_h() );

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH
   debug_printf( 1, "allocating refresh grid (%d tiles...)",
      g_retroflat_state->viewport.screen_tile_w *
      g_retroflat_state->viewport.screen_tile_h );
   g_retroflat_state->viewport.refresh_grid_h = maug_malloc(
      (g_retroflat_state->viewport.screen_tile_w + 2) *
      (g_retroflat_state->viewport.screen_tile_h + 2),
      sizeof( retroflat_tile_t ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE,
      g_retroflat_state->viewport.refresh_grid_h );
#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

#  ifdef RETROFLAT_VDP
#     if defined( RETROFLAT_OS_UNIX )
   g_retroflat_state->vdp_exe = dlopen(
      "./" RETROFLAT_VDP_LIB_NAME ".so", RTLD_LAZY );
#     elif defined( RETROFLAT_OS_WIN )
   g_retroflat_state->vdp_exe = LoadLibrary(
      "./" RETROFLAT_VDP_LIB_NAME ".dll" );
#     else
#        error "dlopen undefined!"
#     endif /* RETROFLAT_OS_UNIX */

   if( !(g_retroflat_state->vdp_exe) ) {
      error_printf( "not loading VDP" );
      /* Skip creating the buffer or trying to run the init proc. */
      goto skip_vdp;
   }

   /* Create intermediary screen buffer. */
   debug_printf( 1, "creating VDP buffer, " SIZE_T_FMT " x " SIZE_T_FMT,
      g_retroflat_state->screen_v_w, g_retroflat_state->screen_v_h );
   g_retroflat_state->vdp_buffer =
      calloc( 1, sizeof( struct RETROFLAT_BITMAP ) );
   maug_cleanup_if_null_alloc(
      struct RETROFLAT_BITMAP*, g_retroflat_state->vdp_buffer );
   retval = retroflat_create_bitmap(
      g_retroflat_state->screen_v_w, g_retroflat_state->screen_v_h,
      g_retroflat_state->vdp_buffer, RETROFLAT_FLAGS_OPAQUE );
   maug_cleanup_if_not_ok();

   debug_printf( 1, "initializing VDP..." );
   retval = retroflat_vdp_call( "retroflat_vdp_init" );

skip_vdp:

#  endif /* RETROFLAT_VDP */

#  ifdef RETROFLAT_3D
   retro3d_platform_init();
#  endif /* RETROFLAT_3D */

#  if !defined( RETROFLAT_NO_BLANK_INIT ) && !defined( RETROFLAT_3D )
   retroflat_draw_lock( NULL );
   retroflat_rect(
      NULL, RETROFLAT_COLOR_BLACK, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );
   retroflat_draw_release( NULL );
#  endif /* !RETROFLAT_NO_BLANK_INIT */

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown( int retval ) {

   debug_printf( 1, "retroflat shutdown called..." );

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH
   if( (MAUG_MHANDLE)NULL != g_retroflat_state->viewport.refresh_grid_h ) {
      maug_mfree( g_retroflat_state->viewport.refresh_grid_h );
   }
#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

#  if defined( RETROFLAT_VDP )
   if( NULL != g_retroflat_state->vdp_exe ) {
      retroflat_vdp_call( "retroflat_vdp_shutdown" );
#     ifdef RETROFLAT_OS_UNIX
      dlclose( g_retroflat_state->vdp_exe );
#     elif defined( RETROFLAT_OS_WIN )
      FreeLibrary( g_retroflat_state->vdp_exe );
#     else
#        error "dlclose undefined!"
#     endif /* RETROFLAT_OS_UNIX || RETROFLAT_OS_WIN */
   }

   if( NULL != g_retroflat_state->vdp_buffer ) {
      debug_printf( 1, "destroying VPD buffer..." );
      retroflat_destroy_bitmap( g_retroflat_state->vdp_buffer );
      free( g_retroflat_state->vdp_buffer );
   }
#  endif /* RETROFLAT_VDP */

   /* === Platform-Specific Shutdown === */

#ifdef RETROFLAT_3D
   retro3d_platform_shutdown();
#endif /* RETROFLAT_3D */

   retroflat_shutdown_platform( retval );

   maug_munlock( g_retroflat_state_h, g_retroflat_state );
   maug_mfree( g_retroflat_state );

}

/* === */

#  ifdef RETROFLAT_VDP

MERROR_RETVAL retroflat_vdp_call( const char* proc_name ) {
   MERROR_RETVAL retval = MERROR_OK;
   retroflat_vdp_proc_t vdp_proc = (retroflat_vdp_proc_t)NULL;
#     ifdef RETROFLAT_OS_WIN
   char proc_name_ex[256];
#     endif /* RETROFLAT_OS_WIN */

   if( NULL == g_retroflat_state->vdp_exe ) {
      goto cleanup;
   }

#     ifdef RETROFLAT_OS_UNIX
   vdp_proc = dlsym( g_retroflat_state->vdp_exe, proc_name );
#     elif defined( RETROFLAT_OS_WIN )
   /* Append a _ to the proc_name because Watcom? Windows? */
   maug_snprintf( proc_name_ex, 255, "%s_", proc_name );
   vdp_proc = (retroflat_vdp_proc_t)GetProcAddress(
      g_retroflat_state->vdp_exe, proc_name_ex );
#     else
#        error "dlsym undefined!"
#     endif
   if( (retroflat_vdp_proc_t)NULL == vdp_proc ) {
      goto cleanup;
   }

#     ifdef RETROFLAT_OS_WIN
   retroflat_draw_lock( g_retroflat_state->vdp_buffer );
#     endif /* RETROFLAT_OS_WIN */

   if(
      /* Don't pxlock before init can set the flag! */
      0 == strcmp( "retroflat_vdp_flip", proc_name ) &&
      RETROFLAT_VDP_FLAG_PXLOCK ==
         (RETROFLAT_VDP_FLAG_PXLOCK & g_retroflat_state->vdp_flags)
   ) {
      retroflat_vdp_lock( &(g_retroflat_state->buffer) );
      retroflat_vdp_lock( g_retroflat_state->vdp_buffer );
   }

   retval = vdp_proc( g_retroflat_state );

   if(
      0 == strcmp( "retroflat_vdp_flip", proc_name ) &&
      RETROFLAT_VDP_FLAG_PXLOCK ==
         (RETROFLAT_VDP_FLAG_PXLOCK & g_retroflat_state->vdp_flags)
   ) {
      retroflat_vdp_release( &(g_retroflat_state->buffer) );
      retroflat_vdp_release( g_retroflat_state->vdp_buffer );
   }

#     ifdef RETROFLAT_OS_WIN
   retroflat_draw_release( g_retroflat_state->vdp_buffer );
#     endif /* RETROFLAT_OS_WIN */

cleanup:
   return retval;
}

#  endif /* RETROFLAT_VDP */

/* === */

#if 0

void retroflat_cursor( struct RETROFLAT_BITMAP* target, uint8_t flags ) {
#if 0
   char mouse_str[11] = "";

   maug_snprintf(
      mouse_str, 10, "%02d, %02d", g_retroflat_state->last_mouse_x, g_retroflat_state->last_mouse_y );

   retroflat_string(
      target, RETROFLAT_COLOR_BLACK,
      mouse_str, 10, NULL, 0, 0, 0 );
   retroflat_rect(
      target, RETROFLAT_COLOR_BLACK,
      g_retroflat_state->last_mouse_x - 5, g_retroflat_state->last_mouse_y - 5, 10, 10, 0 );
#endif
}

#endif

/* === */

void retroflat_set_proc_resize(
   retroflat_proc_resize_t on_resize_in, void* data_in
) {
   g_retroflat_state->on_resize = on_resize_in;
   g_retroflat_state->on_resize_data = data_in;
}

/* === */

uint8_t retroflat_viewport_move_x_generic( int16_t x ) {
   int16_t new_world_x = g_retroflat_state->viewport.world_x + x;

   /* Keep the viewport in the world arena. */
   if(
      0 <= new_world_x &&
      g_retroflat_state->viewport.world_w >= new_world_x +
         g_retroflat_state->viewport.screen_w
   ) {
      g_retroflat_state->viewport.world_x += x;
      g_retroflat_state->viewport.world_tile_x += x >> RETROFLAT_TILE_W_BITS;
      return 1;
   }

   return 0;
}

/* === */

uint8_t retroflat_viewport_move_y_generic( int16_t y ) {
   int16_t new_world_y = g_retroflat_state->viewport.world_y + y;

   /* Keep the viewport in the world arena. */
   if(
      0 <= new_world_y &&
      g_retroflat_state->viewport.world_h >= new_world_y +
         g_retroflat_state->viewport.screen_h
   ) {
      g_retroflat_state->viewport.world_y += y;
      g_retroflat_state->viewport.world_tile_y += y >> RETROFLAT_TILE_H_BITS;
      return 1;
   }

   return 0;
}

/* === */

uint8_t retroflat_viewport_focus_generic(
   size_t x1, size_t y1, size_t range, size_t speed
) {
   uint8_t moved = 0,
      new_moved = 0;
   int16_t new_pt = 0;

#  define _retroflat_viewport_focus_dir( n, xy, wh, gl, pm, dir, range, speed ) \
      new_pt = n - retroflat_viewport_world_ ## xy(); \
      if( new_pt gl (retroflat_screen_ ## wh() >> 1) pm range ) { \
         new_moved = retroflat_viewport_move_ ## xy( \
            gc_retroflat_offsets8_ ## xy[RETROFLAT_DIR8_ ## dir] * speed ); \
         if( !moved && new_moved ) { \
            moved = new_moved; \
         } \
      }

   _retroflat_viewport_focus_dir( x1, x, w, <, -, WEST, range, speed );
   _retroflat_viewport_focus_dir( x1, x, w, >, +, EAST, range, speed );
   _retroflat_viewport_focus_dir( y1, y, h, <, -, NORTH, range, speed );
   _retroflat_viewport_focus_dir( y1, y, h, >, +, SOUTH, range, speed );

   return moved;
}

/* === */

#elif !defined( RETROVDP_C ) /* End of RETROFLT_C */

/**
 * \addtogroup maug_retroflt_dir
 * \{
 */

extern MAUG_CONST int16_t SEG_MCONST gc_retroflat_offsets8_x[8];
extern MAUG_CONST int16_t SEG_MCONST gc_retroflat_offsets8_y[8];
extern MAUG_CONST int16_t SEG_MCONST gc_retroflat_offsets4_x[4];
extern MAUG_CONST int16_t SEG_MCONST gc_retroflat_offsets4_y[4];

/*! \} */

#define RETROFLAT_COLOR_TABLE_CONSTS( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   extern MAUG_CONST RETROFLAT_COLOR RETROFLAT_COLOR_ ## name_u;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CONSTS )

extern MAUG_CONST char* SEG_MCONST gc_retroflat_color_names[];

   extern struct RETROFLAT_STATE* g_retroflat_state;
#     if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   extern HINSTANCE            g_retroflat_instance;
   extern int                  g_retroflat_cmd_show;
#     endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  if (defined( RETROFLAT_SOFT_SHAPES ) || defined( RETROFLAT_SOFT_LINES ) || \
   defined( RETROFLAT_3D ))
#     define RETROSOFT_PRESENT
#  endif

#  ifdef RETROFLAT_3D
#     include <retro3dp.h>
#     include <retro3d.h>
#     include <retro3du.h>
#  endif /* RETROFLAT_3D */

#  ifdef RETROSOFT_PRESENT
#     include <retrosft.h>
#  endif /* RETROFLAT_SOFT_SHAPES */

/* Second retapis.h include for function bodies not needed. */

/* Second retapii.h include for function bodies not needed. */

#endif /* RETROFLT_C */

#ifdef RETROFLAT_XPM
#include <retroxpm.h>
#endif /* RETROFLAT_XPM */

#ifdef RETROVDP_C

/* Declarations for VDP sources. */

#endif /* RETROVDP_C */

/*! \} */ /* maug_retroflt */

#endif /* RETROFLT_H */

