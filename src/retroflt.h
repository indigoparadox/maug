
#ifndef RETROFLT_H
#define RETROFLT_H

#include <mtypes.h>

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
 * | RETROFLAT_API_NDS     | Nintendo DS        | Yes (Limited)   |
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

#include <stdarg.h>

#include <marge.h>

#ifdef RETROFLAT_API_WINCE
/* WinCE is very similar to Win32, so we'll mostly use that with some exceptions below. */
#	define RETROFLAT_API_WIN32
#endif /* RETROFLAT_API_WINCE */

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

#define retroflat_read_lsbf_32( bytes, offset ) \
   ((bytes[offset]) | (bytes[offset + 1] << 8) | (bytes[offset + 2] << 16) \
   | (bytes[offset + 3] << 24))

#define retroflat_read_lsbf_16( bytes, offset ) \
   ((bytes[offset]) | (bytes[offset + 1] << 8))

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
 * \brief Flag for retroflat_string() and retroflat_string_sz() to print
 *        text as outline-only.
 * \todo This has not yet been implemented and is present for backward
 *       compatibility.
 */
#define RETROFLAT_FLAGS_OUTLINE  0x04

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
 * \brief Only supported on some platforms: Attempt to scale screen by 2X.
 */
#define RETROFLAT_FLAGS_SCALE2X 0x10

/*! \} */ /* maug_retroflt_flags */

/**
 * \addtogroup maug_retroflt_config
 * \{
 */

/**
 * \addtogroup maug_retroflt_config_btypes RetroFlat Config Buffer Types
 * \{
 */

#define RETROFLAT_BUFFER_INT 0x00

#define RETROFLAT_BUFFER_STRING 0x01

#define RETROFLAT_BUFFER_FLOAT 0x02

#define RETROFLAT_BUFFER_BOOL 0x04

/*! \} */ /* maug_retroflt_config_btypes */

/**
 * \addtogroup maug_retroflt_config_flags RetroFlat Config Flags
 * \brief Flags for the flags parameter of retroflat_config_open().
 * \{
 */

/**
 * \brief Flag indicating config object should be opened for WRITING only.
 *
 * If this is not specified, the buffer will only be opened for READING.
 *
 * These modes are mutually exclusive! A config object must be closed and
 * reopened to switch between READING and WRITING!
 */
#define RETROFLAT_CONFIG_FLAG_W     0x01

/**
 * \brief Flag indicating config object holds BINARY data.
 */
#define RETROFLAT_CONFIG_FLAG_BIN   0x02

/*! \} */ /* maug_retroflt_config_flags */

/*! \} */ /* maug_retroflt_config */

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

/*! \} */ /* maug_retroflt_bitmap */

#ifndef RETROFLAT_DEFAULT_SCREEN_W
#  define RETROFLAT_DEFAULT_SCREEN_W 320
#endif /* RETROFLAT_DEFAULT_SCREEN_W */

#ifndef RETROFLAT_DEFAULT_SCREEN_H
#  define RETROFLAT_DEFAULT_SCREEN_H 200
#endif /* RETROFLAT_DEFAULT_SCREEN_H */

#ifndef RETROFLAT_GL_Z
#  define RETROFLAT_GL_Z -0.001
#endif /* !RETROFLAT_GL_Z */

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

#ifndef RETROFLAT_PATH_MAX
/*! \brief Maximum size allocated for asset paths. */
#  define RETROFLAT_PATH_MAX 256
#endif /* !RETROFLAT_PATH_MAX */

#ifndef RETROFLAT_TITLE_MAX
#  define RETROFLAT_TITLE_MAX 255
#endif /* !RETROFLAT_TITLE_MAX */

#ifndef RETROFLAT_VDP_ARGS_SZ_MAX
/**
 * \warn Changing this may break binary compatibility!
 */
#  define RETROFLAT_VDP_ARGS_SZ_MAX 255
#endif /* !RETROFLAT_VDP_ARGS_SZ_MAX */

#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
#  if !defined( RETROFLAT_WIN_STYLE )
#     if defined( RETROFLAT_API_WINCE )
#        define RETROFLAT_WIN_STYLE (WS_VISIBLE)
#     else
#        define RETROFLAT_WIN_STYLE (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME)
#     endif /* RETROFLAT_API_WINCE */
#  endif /* !RETROFLAT_WIN_STYLE */
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

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

#ifndef NDS_OAM_ACTIVE
/*! \brief Active sprite engine screen on Nintendo DS. */
#  define NDS_OAM_ACTIVE &oamMain
#endif /* !NDS_OAM_ACTIVE */

#ifndef NDS_SPRITES_ACTIVE
/*! \brief Maximum number of sprites active on-screen on Nintendo DS. */
#  define NDS_SPRITES_ACTIVE 24
#endif /* !NDS_SPRITES_ACTIVE */

#ifndef RETROFLAT_CONFIG_LN_SZ_MAX
#  define RETROFLAT_CONFIG_LN_SZ_MAX 255
#endif /* !RETROFLAT_CONFIG_LN_SZ_MAX */

#ifndef RETROFLAT_CONFIG_EXT
#  define RETROFLAT_CONFIG_EXT ".ini"
#endif /* !RETROFLAT_CONFIG_EXT */

#ifndef RETROFLAT_BMP_COLORS_SZ_MAX
#  define RETROFLAT_BMP_COLORS_SZ_MAX 256
#endif /* !RETROFLAT_BMP_COLORS_SZ_MAX */

/*! \} */ /* maug_retroflt_compiling */

/**
 * \addtogroup maug_retroflt_assets RetroFlat Assets API
 * \brief Functions and macros for handling graphical asset files.
 * \todo This is kind of a mess and needs better integration with the rest!
 * \{
 */

/**
 * \brief Path/name used to load an asset from disk.
 */
typedef char retroflat_asset_path[RETROFLAT_PATH_MAX];

/**
 * \brief Compare two asset paths. Return 0 if they're the same.
 */
#define retroflat_cmp_asset_path( a, b ) strncmp( a, b, RETROFLAT_PATH_MAX )

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

#ifdef RETROFLAT_OPENGL
struct RETROFLAT_GLTEX {
   MAUG_MHANDLE bytes_h;
   uint8_t* bytes;
   uint32_t bpp;
   uint32_t sz;
   uint8_t* px;
   uint32_t id;
   size_t w;
   size_t h;
};
#endif /* RETROFLAT_OPENGL */

/* === OS-Specific Includes and Defines === */

#if defined( RETROFLAT_OS_WIN ) && !defined( MAUG_WINDOWS_H )
#  include <windows.h>
#  define MAUG_WINDOWS_H
#endif /* !MAUG_WINDOWS_H */

/* === API-Specific Includes and Defines === */

#if defined( RETROFLAT_API_ALLEGRO )

#  ifdef RETROFLAT_OPENGL
#     error "opengl support not implemented for allegro"
#  endif /* RETROFLAT_OPENGL */

/* == Allegro == */

#  include <allegro.h>

#  ifdef RETROFLAT_OS_DOS
#     include <dos.h>
#     include <conio.h>
#  endif /* RETROFLAT_OS_DOS */

#include <time.h> /* For srand() */

#  ifndef RETROFLAT_CONFIG_USE_FILE
#     define RETROFLAT_CONFIG_USE_FILE
#  endif /* !RETROFLAT_CONFIG_USE_FILE */

typedef FILE* RETROFLAT_CONFIG;
typedef int16_t RETROFLAT_IN_KEY;
typedef uint32_t retroflat_ms_t;

#define RETROFLAT_MS_FMT "%u"

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   BITMAP* b;
};

typedef int RETROFLAT_COLOR_DEF;

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) (0)
#  ifdef RETROFLAT_OPENGL
#     define retroflat_bitmap_w( bmp ) ((bmp)->tex.w)
#     define retroflat_bitmap_h( bmp ) ((bmp)->tex.h)
#  else
#     define retroflat_bitmap_w( bmp ) ((bmp)->w)
#     define retroflat_bitmap_h( bmp ) ((bmp)->h)
#  endif /* RETROFLAT_OPENGL */
#  define retroflat_screen_w() SCREEN_W
#  define retroflat_screen_h() SCREEN_H
#  define retroflat_screen_buffer() (&(g_retroflat_state->buffer))
#  define retroflat_root_win() (NULL) /* TODO */
#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )
#  ifdef RETROFLAT_VDP
#     define retroflat_vdp_lock( bmp )
#     define retroflat_vdp_release( bmp )
#  endif /* RETROFLAT_VDP */

#  define retroflat_quit( retval_in ) \
   g_retroflat_state->retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING; \
   g_retroflat_state->retval = retval_in;

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

#  define RETROFLAT_KEY_UP	KEY_UP
#  define RETROFLAT_KEY_DOWN	KEY_DOWN
#  define RETROFLAT_KEY_RIGHT	KEY_RIGHT
#  define RETROFLAT_KEY_LEFT	KEY_LEFT
#  define RETROFLAT_KEY_A	KEY_A
#  define RETROFLAT_KEY_B	KEY_B
#  define RETROFLAT_KEY_C	KEY_C
#  define RETROFLAT_KEY_D	KEY_D
#  define RETROFLAT_KEY_E	KEY_E
#  define RETROFLAT_KEY_F	KEY_F
#  define RETROFLAT_KEY_G	KEY_G
#  define RETROFLAT_KEY_H	KEY_H
#  define RETROFLAT_KEY_I	KEY_I
#  define RETROFLAT_KEY_J	KEY_J
#  define RETROFLAT_KEY_K	KEY_K
#  define RETROFLAT_KEY_L	KEY_L
#  define RETROFLAT_KEY_M	KEY_M
#  define RETROFLAT_KEY_N	KEY_N
#  define RETROFLAT_KEY_O	KEY_O
#  define RETROFLAT_KEY_P	KEY_P
#  define RETROFLAT_KEY_Q	KEY_Q
#  define RETROFLAT_KEY_R	KEY_R
#  define RETROFLAT_KEY_S	KEY_S
#  define RETROFLAT_KEY_T	KEY_T
#  define RETROFLAT_KEY_U	KEY_U
#  define RETROFLAT_KEY_V	KEY_V
#  define RETROFLAT_KEY_W	KEY_W
#  define RETROFLAT_KEY_X	KEY_X
#  define RETROFLAT_KEY_Y	KEY_Y
#  define RETROFLAT_KEY_Z	KEY_Z
#  define RETROFLAT_KEY_0  KEY_0
#  define RETROFLAT_KEY_1  KEY_1
#  define RETROFLAT_KEY_2  KEY_2
#  define RETROFLAT_KEY_3  KEY_3
#  define RETROFLAT_KEY_4  KEY_4
#  define RETROFLAT_KEY_5  KEY_5
#  define RETROFLAT_KEY_6  KEY_6
#  define RETROFLAT_KEY_7  KEY_7
#  define RETROFLAT_KEY_8  KEY_8
#  define RETROFLAT_KEY_9  KEY_9
#  define RETROFLAT_KEY_TAB	KEY_TAB
#  define RETROFLAT_KEY_SPACE	KEY_SPACE
#  define RETROFLAT_KEY_ESC	KEY_ESC
#  define RETROFLAT_KEY_ENTER	KEY_ENTER
#  define RETROFLAT_KEY_HOME	KEY_HOME
#  define RETROFLAT_KEY_END	KEY_END
#  define RETROFLAT_KEY_DELETE   KEY_DEL
#  define RETROFLAT_KEY_PGUP     KEY_PGUP
#  define RETROFLAT_KEY_PGDN     KEY_PGDN
#  define RETROFLAT_KEY_GRAVE    KEY_BACKQUOTE
#  define RETROFLAT_KEY_BKSP     KEY_BACKSPACE
#  define RETROFLAT_KEY_SLASH    KEY_SLASH
#  define RETROFLAT_KEY_SEMICOLON   KEY_SEMICOLON
#  define RETROFLAT_KEY_PERIOD   KEY_STOP
#  define RETROFLAT_KEY_COMMA    KEY_COMMA
#  define RETROFLAT_KEY_EQUALS   KEY_EQUALS
#  define RETROFLAT_KEY_DASH     KEY_MINUS

#elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

#  if defined( RETROFLAT_API_SDL2 ) && defined( RETROFLAT_OPENGL )
#     error "opengl support not implemented for SDL 2"
#  endif /* RETROFLAT_API_SDL2 && RETROFLAT_OPENGL */

#  include <time.h> /* For srand() */

#  if defined( RETROFLAT_OS_WASM )
#     include <emscripten.h>
#  endif /* RETROFLAT_OS_WASM */

#  ifndef RETROFLAT_CONFIG_USE_FILE
#     define RETROFLAT_CONFIG_USE_FILE
#  endif /* !RETROFLAT_CONFIG_USE_FILE */

#  include <SDL.h>
#  ifdef RETROFLAT_API_SDL1
#     include <SDL_getenv.h>
#  endif /* RETROFLAT_API_SDL1 */

#  if !defined( RETROFLAT_SOFT_SHAPES ) && !defined( RETROFLAT_OPENGL )
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

#  if !defined( RETROFLAT_SOFT_LINES ) && !defined( RETROFLAT_OPENGL )
/* TODO: Do we need soft lines for this? */
#     define RETROFLAT_SOFT_LINES
#  endif /* !RETROFLAT_SOFT_LINES */

typedef FILE* RETROFLAT_CONFIG;
#ifdef RETROFLAT_API_SDL2
typedef int32_t RETROFLAT_IN_KEY;
#else
typedef int16_t RETROFLAT_IN_KEY;
#endif /* RETROFLAT_API_SDL2 */
typedef uint32_t retroflat_ms_t;

#define RETROFLAT_MS_FMT "%u"

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   SDL_Surface* surface;
#  ifdef RETROFLAT_API_SDL1
   /* SDL1 autolock counter. */
   ssize_t autolock_refs;
#  else
   /* SDL2 texture pointers. */
   SDL_Texture* texture;
   SDL_Renderer* renderer;
#  endif /* RETROFLAT_API_SDL1 */
#  ifdef RETROFLAT_OPENGL
   struct RETROFLAT_GLTEX tex;
#  endif /* RETROFLAT_OPENGL */
};

#  define RETROFLAT_KEY_UP	SDLK_UP
#  define RETROFLAT_KEY_DOWN  SDLK_DOWN
#  define RETROFLAT_KEY_RIGHT	SDLK_RIGHT
#  define RETROFLAT_KEY_LEFT	SDLK_LEFT
#  define RETROFLAT_KEY_A	   SDLK_a
#  define RETROFLAT_KEY_B	   SDLK_b
#  define RETROFLAT_KEY_C	   SDLK_c
#  define RETROFLAT_KEY_D	   SDLK_d
#  define RETROFLAT_KEY_E	   SDLK_e
#  define RETROFLAT_KEY_F	   SDLK_f
#  define RETROFLAT_KEY_G	   SDLK_g
#  define RETROFLAT_KEY_H	   SDLK_h
#  define RETROFLAT_KEY_I	   SDLK_i
#  define RETROFLAT_KEY_J	   SDLK_j
#  define RETROFLAT_KEY_K	   SDLK_k
#  define RETROFLAT_KEY_L	   SDLK_l
#  define RETROFLAT_KEY_M	   SDLK_m
#  define RETROFLAT_KEY_N	   SDLK_n
#  define RETROFLAT_KEY_O	   SDLK_o
#  define RETROFLAT_KEY_P	   SDLK_p
#  define RETROFLAT_KEY_Q	   SDLK_q
#  define RETROFLAT_KEY_R	   SDLK_r
#  define RETROFLAT_KEY_S	   SDLK_s
#  define RETROFLAT_KEY_T	   SDLK_t
#  define RETROFLAT_KEY_U	   SDLK_u
#  define RETROFLAT_KEY_V	   SDLK_v
#  define RETROFLAT_KEY_W	   SDLK_w
#  define RETROFLAT_KEY_X	   SDLK_x
#  define RETROFLAT_KEY_Y	   SDLK_y
#  define RETROFLAT_KEY_Z	   SDLK_z
#  define RETROFLAT_KEY_0     SDLK_0
#  define RETROFLAT_KEY_1     SDLK_1
#  define RETROFLAT_KEY_2     SDLK_2
#  define RETROFLAT_KEY_3     SDLK_3
#  define RETROFLAT_KEY_4     SDLK_4
#  define RETROFLAT_KEY_5     SDLK_5
#  define RETROFLAT_KEY_6     SDLK_6
#  define RETROFLAT_KEY_7     SDLK_7
#  define RETROFLAT_KEY_8     SDLK_8
#  define RETROFLAT_KEY_9     SDLK_9
#  define RETROFLAT_KEY_TAB	SDLK_TAB
#  define RETROFLAT_KEY_SPACE	SDLK_SPACE
#  define RETROFLAT_KEY_ESC	SDLK_ESCAPE
#  define RETROFLAT_KEY_ENTER	SDLK_RETURN
#  define RETROFLAT_KEY_HOME	SDLK_HOME
#  define RETROFLAT_KEY_END	SDLK_END
#  define RETROFLAT_KEY_DELETE   SDLK_DELETE
#  define RETROFLAT_KEY_PGUP     SDLK_PAGEUP
#  define RETROFLAT_KEY_PGDN     SDLK_PAGEDOWN
#  define RETROFLAT_KEY_GRAVE SDLK_BACKQUOTE
#  define RETROFLAT_KEY_SLASH SDLK_SLASH
#  define RETROFLAT_KEY_BKSP  SDLK_BACKSPACE
#  define RETROFLAT_KEY_SEMICOLON   SDLK_SEMICOLON
#  define RETROFLAT_KEY_PERIOD   SDLK_PERIOD
#  define RETROFLAT_KEY_COMMA    SDLK_COMMA
#  define RETROFLAT_KEY_EQUALS   SDLK_EQUALS
#  define RETROFLAT_KEY_DASH     SDLK_MINUS

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->surface)
#  ifdef RETROFLAT_OPENGL
#     define retroflat_bitmap_w( bmp ) ((bmp)->tex.w)
#     define retroflat_bitmap_h( bmp ) ((bmp)->tex.h)
#  else
#     define retroflat_bitmap_w( bmp ) ((bmp)->surface->w)
#     define retroflat_bitmap_h( bmp ) ((bmp)->surface->h)
#  endif /* RETROFLAT_OPENGL */
#  ifdef RETROFLAT_API_SDL1
#     define retroflat_bitmap_locked( bmp ) \
         (RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & (bmp)->flags))
#  else
#     define retroflat_bitmap_locked( bmp ) (NULL != (bmp)->renderer)
#  endif
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)

#  ifdef RETROFLAT_VDP
#     define retroflat_screen_buffer() \
         (NULL == g_retroflat_state->vdp_buffer ? \
         &(g_retroflat_state->buffer) : g_retroflat_state->vdp_buffer)
#  else
#     define retroflat_screen_buffer() (&(g_retroflat_state->buffer))
#  endif /* RETROFLAT_VDP */
#  define retroflat_root_win() (NULL) /* TODO */

#  if defined( RETROFLAT_API_SDL1 ) && !defined( RETROFLAT_OPENGL )
/* Special pixel lock JUST for SDL1 surfaces. */
#     define retroflat_px_lock( bmp ) \
         if( NULL != bmp ) { \
            (bmp)->autolock_refs++; \
            SDL_LockSurface( (bmp)->surface ); \
         }
#     define retroflat_px_release( bmp ) \
         if( NULL != bmp ) { \
            assert( 0 < (bmp)->autolock_refs ); \
            (bmp)->autolock_refs--; \
            SDL_UnlockSurface( (bmp)->surface ); \
         }
#     ifdef RETROFLAT_VDP
#        define retroflat_vdp_lock( bmp ) retroflat_px_lock( bmp )
#        define retroflat_vdp_release( bmp ) retroflat_px_release( bmp )
#     endif /* RETROFLAT_VDP */
#  else
/* Pixel lock above does not apply to SDL2 surfaces or bitmap textures. */
#     define retroflat_px_lock( bmp )
#     define retroflat_px_release( bmp )
#  endif

#  define retroflat_quit( retval_in ) \
      g_retroflat_state->retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING; \
      g_retroflat_state->retval = retval_in;

#  define END_OF_MAIN()

   /* SDL Icon */
#  if defined( RETROFLT_C ) && defined( RETROFLAT_SDL_ICO )
#     include <sdl_ico.h>
#  endif /* RETROFLT_C && RETROFLAT_SDL_ICO */

   /* SDL Colors */
#  ifdef RETROFLAT_OPENGL
typedef float MAUG_CONST* RETROFLAT_COLOR_DEF;
#else
typedef SDL_Color RETROFLAT_COLOR_DEF;
#  endif /* RETROFLAT_OPENGL */

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

/* == Win16/Win32 == */

typedef int16_t RETROFLAT_IN_KEY;
typedef uint32_t retroflat_ms_t;

#  define RETROFLAT_MS_FMT "%lu"

#  if defined( RETROFLAT_API_WIN16 ) && defined( RETROFLAT_OPENGL )
#     error "opengl support not implemented for win16"
#  endif /* RETROFLAT_API_SDL2 && RETROFLAT_OPENGL */

#  include <mmsystem.h>

#  ifndef RETROFLAT_API_WINCE
#     include <time.h> /* For srand() */
#  endif /* !RETROFLAT_API_WINCE */

#  ifdef RETROFLAT_WING

#     if defined( RETROFLAT_API_WIN32 )
#        define WINGAPI WINAPI
#     else
#        define WINGAPI WINAPI _loadds
#     endif

#     define RETROFLAT_WING_LLTABLE( f ) \
   f( HDC, WinGCreateDC, 1001 ) \
   f( BOOL, WinGRecommendDIBFormat, 1002 ) \
   f( HBITMAP, WinGCreateBitmap, 1003 ) \
   f( BOOL, WinGStretchBlt, 1009 )

typedef int16_t RETROFLAT_IN_KEY;
typedef uint32_t retroflat_ms_t;
typedef uint32_t retroflat_ms_t;

#     define RETROFLAT_MS_FMT "%lu"

typedef HDC (WINGAPI *WinGCreateDC_t)();
typedef BOOL (WINGAPI *WinGRecommendDIBFormat_t)( BITMAPINFO FAR* );
typedef HBITMAP (WINGAPI *WinGCreateBitmap_t)(
   HDC, BITMAPINFO const FAR*, void FAR* FAR* );
typedef BOOL (WINGAPI *WinGStretchBlt_t)(
   HDC, int, int, int, int, HDC, int, int, int, int );

#     define RETROFLAT_WING_LLTABLE_STRUCT_MEMBERS( retval, proc, ord ) \
   proc ## _t proc;

struct RETROFLAT_WING_MODULE {
   HMODULE module;
   uint8_t success;
   RETROFLAT_WING_LLTABLE( RETROFLAT_WING_LLTABLE_STRUCT_MEMBERS )
};
#  endif /* RETROFLAT_WING */

struct RETROFLAT_BMI {
   BITMAPINFOHEADER header;
   RGBQUAD colors[RETROFLAT_BMP_COLORS_SZ_MAX];
};

#  ifdef RETROFLAT_API_WIN32
typedef HKEY RETROFLAT_CONFIG;
#  else
typedef int RETROFLAT_CONFIG;
#  endif /* RETROFLAT_API_WIN32 */

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   HBITMAP b;
   HBITMAP mask;
   HDC hdc_b;
   HDC hdc_mask;
   HBITMAP old_hbm_b;
   HBITMAP old_hbm_mask;
#  ifdef RETROFLAT_API_WIN16
   uint8_t far* bits;
#  else
	uint8_t* bits;
#  endif /* RETROFLAT_API_WIN16 */
   ssize_t autolock_refs;
#  ifdef RETROFLAT_OPENGL
   struct RETROFLAT_GLTEX tex;
#  endif /* RETROFLAT_OPENGL */
   struct RETROFLAT_BMI bmi;
};

LPSTR* retroflat_win_cli( LPSTR cmd_line, int* argc_out );

#  ifdef RETROFLAT_OPENGL

typedef float MAUG_CONST* RETROFLAT_COLOR_DEF;

#  else

/* Use Windoes API and generate brushes/pens for GDI. */

typedef COLORREF RETROFLAT_COLOR_DEF;

/* === Setup Brush Cache === */

/* This will be initialized in setup, so just preserve the number. */
#     define RETROFLAT_COLOR_TABLE_WIN_BRUSH( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         (HBRUSH)NULL,

#     define RETROFLAT_COLOR_TABLE_WIN_BRSET( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         gc_retroflat_win_brushes[idx] = CreateSolidBrush( RGB( r, g, b ) );

#     define RETROFLAT_COLOR_TABLE_WIN_BRRM( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   if( (HBRUSH)NULL != gc_retroflat_win_brushes[idx] ) { \
      DeleteObject( gc_retroflat_win_brushes[idx] ); \
      gc_retroflat_win_brushes[idx] = (HBRUSH)NULL; \
   }

/* === End Setup Brush Cache === */

/* === Setup Pen Cache === */

#     define RETROFLAT_COLOR_TABLE_WIN_PENS( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   (HPEN)NULL,

#     define RETROFLAT_COLOR_TABLE_WIN_PNSET( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         gc_retroflat_win_pens[idx] = CreatePen( \
            PS_SOLID, RETROFLAT_LINE_THICKNESS, RGB( r, g, b ) );

#     define RETROFLAT_COLOR_TABLE_WIN_PENRM( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         if( (HPEN)NULL != gc_retroflat_win_pens[idx] ) { \
            DeleteObject( gc_retroflat_win_pens[idx] ); \
            gc_retroflat_win_pens[idx] = (HPEN)NULL; \
         }

/* === End Setup Pen Cache === */

#     ifdef RETROFLT_C

HBRUSH gc_retroflat_win_brushes[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRUSH )
};

static HPEN gc_retroflat_win_pens[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PENS )
};

#     else

extern HBRUSH gc_retroflat_win_brushes[];

#     endif /* RETROFLT_C */

/* Create a brush and set it to the target HDC. */
#     define retroflat_win_setup_brush( old_brush, target, color, flags ) \
         if( RETROFLAT_FLAGS_FILL != (RETROFLAT_FLAGS_FILL & flags) ) { \
            old_brush = \
               SelectObject( target->hdc_b, GetStockObject( NULL_BRUSH ) ); \
         } else { \
            old_brush = SelectObject( \
               target->hdc_b, gc_retroflat_win_brushes[color] ); \
         }

/* Create a pen and set it to the target HDC. */
#     define retroflat_win_setup_pen( old_pen, target, color, flags ) \
         old_pen = \
            SelectObject( target->hdc_b, gc_retroflat_win_pens[color] );

#     define retroflat_win_cleanup_brush( old_brush, target ) \
         if( (HBRUSH)NULL != old_brush ) { \
            SelectObject( target->hdc_b, old_brush ); \
         }

#     define retroflat_win_cleanup_pen( old_pen, target ) \
         if( (HPEN)NULL != old_pen ) { \
            SelectObject( target->hdc_b, old_pen ); \
         }

#  endif /* RETROFLAT_OPENGL */

/* TODO: This is a parallel bitmap system... maybe move OPENGL stuff into its
 *       own header that takes over graphics stuff in OPENGL mode? */
#  ifdef RETROFLAT_OPENGL
#     define retroflat_bitmap_w( bmp ) ((bmp)->tex.w)
#     define retroflat_bitmap_h( bmp ) ((bmp)->tex.h)
#     define retroflat_bitmap_locked( bmp ) (NULL != (bmp)->tex.bytes)
#  else
#     define retroflat_bitmap_w( bmp ) ((bmp)->bmi.header.biWidth)
#     define retroflat_bitmap_h( bmp ) ((bmp)->bmi.header.biHeight)
#     define retroflat_bitmap_locked( bmp ) ((HDC)NULL != (bmp)->hdc_b)
#  endif /* RETROFLAT_OPENGL */
/* TODO: Adapt this for the OPENGL test above? */
#  define retroflat_bitmap_ok( bitmap ) ((HBITMAP)NULL != (bitmap)->b)

#  ifdef RETROFLAT_VDP

#     ifdef RETROFLAT_API_WIN16
#        error "VDP not supported in Win16!"
#     endif /* RETROFLAT_API_WIN16 */

#     define retroflat_vdp_lock( bmp ) \
   assert( NULL != (bmp)->hdc_b ); \
   /* Confirm header info. */ \
   (bmp)->autolock_refs++; \
   if( \
      1 == (bmp)->autolock_refs && \
      RETROFLAT_FLAGS_SCREEN_BUFFER != \
         (RETROFLAT_FLAGS_SCREEN_BUFFER & (bmp)->flags) \
   ) { \
      assert( NULL == (bmp)->bits ); \
      assert( (bmp)->bmi.header.biBitCount == 32 ); \
      assert( (bmp)->bmi.header.biWidth > 0 ); \
      assert( (bmp)->bmi.header.biHeight > 0 ); \
      assert( (bmp)->bmi.header.biSizeImage == \
         (bmp)->bmi.header.biWidth * (bmp)->bmi.header.biHeight * 4 ); \
      (bmp)->bits = VirtualAlloc( \
         0, (bmp)->bmi.header.biSizeImage, \
         MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ); \
      /* TODO: Check alloc without assert! */ \
      assert( NULL != (bmp)->bits ); \
      /* Get bitmap bits from bitmap HDC into bitmap->bits so that the bitmap
       * can be altered byte-by-byte.
       */ \
      GetDIBits( (bmp)->hdc_b, (bmp)->b, 0, (bmp)->bmi.header.biHeight, \
         (bmp)->bits, (BITMAPINFO*)&((bmp)->bmi), DIB_RGB_COLORS ); \
   }

#     define retroflat_vdp_release( bmp ) \
   assert( 0 < (bmp)->autolock_refs ); \
   (bmp)->autolock_refs--; \
   if( \
      0 == (bmp)->autolock_refs && \
      RETROFLAT_FLAGS_SCREEN_BUFFER != \
         (RETROFLAT_FLAGS_SCREEN_BUFFER & (bmp)->flags) \
   ) { \
      /* TODO: Causes alpha blending in mdemos? */ \
      if( \
         /* Set bitmap bits from bitmap->bits into HDC. */ \
         SetDIBits( g_retroflat_state->hdc_win, (bmp)->b, 0, \
            (bmp)->bmi.header.biHeight, (bmp)->bits, \
            (BITMAPINFO*)&((bmp)->bmi), DIB_RGB_COLORS ) < \
               (bmp)->bmi.header.biHeight \
      ) { \
         error_printf( "SetDIBits failed!" ); \
      } \
      VirtualFree( (bmp)->bits, 0, MEM_RELEASE ); \
      (bmp)->bits = NULL; \
   }

#     define retroflat_screen_buffer() \
         (NULL == g_retroflat_state->vdp_buffer ? \
         &(g_retroflat_state->buffer) : g_retroflat_state->vdp_buffer)

#  else

#     define retroflat_screen_buffer() (&(g_retroflat_state->buffer))

#  endif /* RETROFLAT_VDP */

#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)
#  define retroflat_root_win() (g_retroflat_state->window)
#  define retroflat_quit( retval_in ) PostQuitMessage( retval_in );

#  define retroflat_bmp_int( type, buf, offset ) *((type*)&(buf[offset]))

#  ifndef VK_OEM_2
#     define VK_OEM_2 0xbf
#  endif /* !VK_OEM_2 */

#  ifndef VK_OEM_3
#     define VK_OEM_3 0xc0
#  endif /* !VK_OEM_3 */

#  define RETROFLAT_KEY_GRAVE VK_OEM_3
#  define RETROFLAT_KEY_SLASH VK_OEM_2
#  define RETROFLAT_KEY_UP	   VK_UP
#  define RETROFLAT_KEY_DOWN	VK_DOWN
#  define RETROFLAT_KEY_RIGHT	VK_RIGHT
#  define RETROFLAT_KEY_LEFT	VK_LEFT
#  define RETROFLAT_KEY_BKSP  VK_BACK
#  define RETROFLAT_KEY_A	   0x41
#  define RETROFLAT_KEY_B	   0x42
#  define RETROFLAT_KEY_C	   0x43
#  define RETROFLAT_KEY_D	   0x44
#  define RETROFLAT_KEY_E	   0x45
#  define RETROFLAT_KEY_F	   0x46
#  define RETROFLAT_KEY_G	   0x47
#  define RETROFLAT_KEY_H	   0x48
#  define RETROFLAT_KEY_I	   0x49
#  define RETROFLAT_KEY_J	   0x4a
#  define RETROFLAT_KEY_K	   0x4b
#  define RETROFLAT_KEY_L	   0x4c
#  define RETROFLAT_KEY_M	   0x4d
#  define RETROFLAT_KEY_N	   0x4e
#  define RETROFLAT_KEY_O	   0x4f
#  define RETROFLAT_KEY_P	   0x50
#  define RETROFLAT_KEY_Q	   0x51
#  define RETROFLAT_KEY_R	   0x52
#  define RETROFLAT_KEY_S	   0x53
#  define RETROFLAT_KEY_T	   0x54
#  define RETROFLAT_KEY_U	   0x55
#  define RETROFLAT_KEY_V	   0x56
#  define RETROFLAT_KEY_W	   0x57
#  define RETROFLAT_KEY_X	   0x58
#  define RETROFLAT_KEY_Y	   0x59
#  define RETROFLAT_KEY_Z	   0x60
#  define RETROFLAT_KEY_0     0x30
#  define RETROFLAT_KEY_1     0x31
#  define RETROFLAT_KEY_2     0x32
#  define RETROFLAT_KEY_3     0x33
#  define RETROFLAT_KEY_4     0x34
#  define RETROFLAT_KEY_5     0x35
#  define RETROFLAT_KEY_6     0x36
#  define RETROFLAT_KEY_7     0x37
#  define RETROFLAT_KEY_8     0x38
#  define RETROFLAT_KEY_9     0x39
#  define RETROFLAT_KEY_TAB	VK_TAB
#  define RETROFLAT_KEY_SPACE	VK_SPACE
#  define RETROFLAT_KEY_ESC	VK_ESCAPE
#  define RETROFLAT_KEY_ENTER	VK_RETURN
#  define RETROFLAT_KEY_HOME	VK_HOME
#  define RETROFLAT_KEY_END	VK_END
#  define RETROFLAT_KEY_DELETE   VK_DELETE
#  define RETROFLAT_KEY_PGUP     VK_PRIOR
#  define RETROFLAT_KEY_PGDN     VK_NEXT
#  define RETROFLAT_KEY_SEMICOLON   ';'
#  define RETROFLAT_KEY_PERIOD   '.'
#  define RETROFLAT_KEY_COMMA    ','
#  define RETROFLAT_KEY_EQUALS   '='
#  define RETROFLAT_KEY_DASH     '-'

#  define RETROFLAT_MOUSE_B_LEFT    VK_LBUTTON
#  define RETROFLAT_MOUSE_B_RIGHT   VK_RBUTTON

/* Set the calling convention for WinMain, depending on Win16/Win32. */
#  if defined( RETROFLAT_API_WIN16 )
#     define WINXAPI PASCAL
#  elif defined( RETROFLAT_API_WIN32 )
#     define WINXAPI WINAPI
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* TODO: Verify this on multi-monitor Win32 systems. */
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#define main( argc, argv ) retroflat_main( argc, argv )

/* Improvise a rough WinMain to call main(). */
#define END_OF_MAIN() \
   int WINXAPI WinMain( \
      HINSTANCE hInstance, HINSTANCE hPrevInstance, \
      LPSTR lpCmdLine, int nCmdShow \
   ) { \
      LPSTR* rf_argv = NULL; \
      int rf_argc = 0; \
      int retval = 0; \
      g_retroflat_instance = hInstance; \
      g_retroflat_cmd_show = nCmdShow; \
      rf_argv = retroflat_win_cli( lpCmdLine, &rf_argc ); \
      retval = retroflat_main( rf_argc, rf_argv ); \
      free( rf_argv ); \
      return retval; \
   }

/* Convenience macro for auto-locking inside of draw functions. */
/* TODO: Use maug_cleanup and assume retval. */
#define retroflat_internal_autolock_bitmap( bmp, lock_auto ) \
   if( !retroflat_bitmap_locked( bmp ) ) { \
      retval = retroflat_draw_lock( bmp ); \
      maug_cleanup_if_not_ok(); \
      lock_auto = 1; \
   }

#elif defined( RETROFLAT_API_LIBNDS )

/* == Nintendo DS == */

#  include <nds.h>

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
typedef void* RETROFLAT_CONFIG;

#define RETROFLAT_MS_FMT "%lu"

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

#elif defined( RETROFLAT_API_GLUT )

#  ifndef RETROFLAT_OPENGL
#     error "RETROFLAT_API_GLUT specified without RETROFLAT_OPENGL!"
#     define RETROFLAT_OPENGL
#  endif /* !RETROFLAT_OPENGL */

#  ifdef __WATCOMC__
#     define GLUT_DISABLE_ATEXIT_HACK
#  endif

#include <GL/glut.h>

#  ifndef RETROFLAT_CONFIG_USE_FILE
#     define RETROFLAT_CONFIG_USE_FILE
#  endif /* !RETROFLAT_CONFIG_USE_FILE */

typedef int16_t RETROFLAT_IN_KEY;
typedef uint32_t retroflat_ms_t;

#define RETROFLAT_MS_FMT "%lu"

typedef FILE* RETROFLAT_CONFIG;

typedef float MAUG_CONST* RETROFLAT_COLOR_DEF;

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   struct RETROFLAT_GLTEX tex;
   ssize_t w;
   ssize_t h;
};

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) \
      (RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & (bmp)->flags))
/* TODO */
#  define retroflat_bitmap_w( bmp ) (0)
#  define retroflat_bitmap_h( bmp ) (0)

/*! \brief Special lock used before per-pixel operations because of SDL1. */
#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )
#  ifdef RETROFLAT_VDP
#     define retroflat_vdp_lock( bmp )
#     define retroflat_vdp_release( bmp )
#  endif /* RETROFLAT_VDP */
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)
#  define retroflat_screen_buffer() (&(g_retroflat_state->buffer))
#  define retroflat_root_win() (NULL) /* TODO */
#  define retroflat_quit( retval_in ) glutDestroyWindow( glutGetWindow() )
#  define END_OF_MAIN()

#  define GLUT_SPECIAL_KEY_OFFSET 0x80

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

#  define RETROFLAT_KEY_UP	      (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_UP)
#  define RETROFLAT_KEY_DOWN     (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_DOWN)
#  define RETROFLAT_KEY_RIGHT	   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_RIGHT)
#  define RETROFLAT_KEY_LEFT	   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_LEFT)
#  define RETROFLAT_KEY_HOME	   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_HOME)
#  define RETROFLAT_KEY_END	   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_END)
#  define RETROFLAT_KEY_PGUP     (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_PAGE_UP)
#  define RETROFLAT_KEY_PGDN     \
   (GLUT_SPECIAL_KEY_OFFSET + GLUT_KEY_PAGE_DOWN)
#  define RETROFLAT_KEY_DELETE   0x7f
#  define RETROFLAT_KEY_ESC      0x1b
#  define RETROFLAT_KEY_ENTER    0x0d
#  define RETROFLAT_KEY_TAB	   '\t'
#  define RETROFLAT_KEY_SPACE	   ' '
#  define RETROFLAT_KEY_GRAVE    '`'
#  define RETROFLAT_KEY_SLASH    '/'
#  define RETROFLAT_KEY_BKSP     0x08
#  define RETROFLAT_KEY_SEMICOLON   ';'
#  define RETROFLAT_KEY_PERIOD   '.'
#  define RETROFLAT_KEY_COMMA    ','
#  define RETROFLAT_KEY_EQUALS   '='
#  define RETROFLAT_KEY_DASH     '-'
#  define RETROFLAT_KEY_A		   'a'
#  define RETROFLAT_KEY_B		   'b'
#  define RETROFLAT_KEY_C		   'c'
#  define RETROFLAT_KEY_D		   'd'
#  define RETROFLAT_KEY_E		   'e'
#  define RETROFLAT_KEY_F		   'f'
#  define RETROFLAT_KEY_G		   'g'
#  define RETROFLAT_KEY_H		   'h'
#  define RETROFLAT_KEY_I		   'i'
#  define RETROFLAT_KEY_J		   'j'
#  define RETROFLAT_KEY_K		   'k'
#  define RETROFLAT_KEY_L		   'l'
#  define RETROFLAT_KEY_M		   'm'
#  define RETROFLAT_KEY_N		   'n'
#  define RETROFLAT_KEY_O		   'o'
#  define RETROFLAT_KEY_P		   'p'
#  define RETROFLAT_KEY_Q		   'q'
#  define RETROFLAT_KEY_R		   'r'
#  define RETROFLAT_KEY_S		   's'
#  define RETROFLAT_KEY_T		   't'
#  define RETROFLAT_KEY_U		   'u'
#  define RETROFLAT_KEY_V		   'v'
#  define RETROFLAT_KEY_W		   'w'
#  define RETROFLAT_KEY_X		   'x'
#  define RETROFLAT_KEY_Y		   'y'
#  define RETROFLAT_KEY_Z		   'z'
#  define RETROFLAT_KEY_0		   '0'
#  define RETROFLAT_KEY_1		   '1'
#  define RETROFLAT_KEY_2		   '2'
#  define RETROFLAT_KEY_3		   '3'
#  define RETROFLAT_KEY_4		   '4'
#  define RETROFLAT_KEY_5		   '5'
#  define RETROFLAT_KEY_6		   '6'
#  define RETROFLAT_KEY_7		   '7'
#  define RETROFLAT_KEY_8		   '8'
#  define RETROFLAT_KEY_9		   '9'

#elif defined( RETROFLAT_API_PC_BIOS )

#  include <time.h> /* For srand() */

#if defined( MAUG_OS_DOS_REAL ) && \
   defined( MAUG_DOS_MEM_L ) && \
   defined( __WATCOMC__ )
#     define SEG_RETROBMP __based( __segname( "RETROBMP" ) )
#  else
#     define SEG_RETROBMP
#  endif /* __WATCOMC__ */

typedef int16_t RETROFLAT_IN_KEY;
typedef uint16_t retroflat_ms_t;

#  define RETROFLAT_MS_FMT "%u"

/* Explicity screen sizes aren't supported, only screen modes handled in
 * special cases during init.
 */
#  define RETROFLAT_NO_CLI_SZ

#  define RETROFLAT_SCREEN_MODE_CGA       0x04
#  define RETROFLAT_SCREEN_MODE_VGA       0x13

#  define RETROFLAT_CGA_COLOR_BLACK        0
#  define RETROFLAT_CGA_COLOR_CYAN         1
#  define RETROFLAT_CGA_COLOR_MAGENTA      2
#  define RETROFLAT_CGA_COLOR_WHITE        3

#  ifndef RETROFLAT_SOFT_SHAPES
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

#  ifndef RETROFLAT_DOS_TIMER_DIV
/* #define RETROFLAT_DOS_TIMER_DIV 1103 */
#     define RETROFLAT_DOS_TIMER_DIV 100
#  endif /* !RETROFLAT_DOS_TIMER_DIV */

#  define retroflat_px_lock( bmp )
#  define retroflat_px_release( bmp )

#  ifdef RETROFLAT_VDP
#     define retroflat_vdp_lock( bmp )
#     define retroflat_vdp_release( bmp )
#  endif /* RETROFLAT_VDP */

#  ifndef NO_I86
#     include <i86.h>
#  endif /* NO_I86 */
#  include <dos.h>
#  include <conio.h>
#  include <malloc.h>

#  define END_OF_MAIN()

#  ifndef RETROFLAT_CONFIG_USE_FILE
#     define RETROFLAT_CONFIG_USE_FILE
#  endif /* !RETROFLAT_CONFIG_USE_FILE */

typedef FILE* RETROFLAT_CONFIG;

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->px)

#  define retroflat_quit( retval_in ) \
   g_retroflat_state->retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING; \
   g_retroflat_state->retval = retval_in;

typedef uint8_t RETROFLAT_COLOR_DEF;

struct RETROFLAT_BITMAP {
   size_t sz;
   uint8_t flags;
   int16_t w;
   int16_t h;
   uint8_t SEG_FAR* px;
   uint8_t SEG_FAR* mask;
};

#  define retroflat_screen_buffer() (&(g_retroflat_state->buffer))
/* TODO: Vary based on current screen mode! */
#  define retroflat_screen_w() 320
#  define retroflat_screen_h() 200

/* TODO: DOS Keycodes */

#  define RETROFLAT_KEY_BKSP  0x08
#  define RETROFLAT_KEY_GRAVE 0x60
#  define RETROFLAT_KEY_DASH  '-'
#  define RETROFLAT_KEY_SLASH '/'
#  define RETROFLAT_KEY_PERIOD '.'
#  define RETROFLAT_KEY_COMMA ','
#  define RETROFLAT_KEY_SEMICOLON ';'
#  define RETROFLAT_KEY_A	   0x41
#  define RETROFLAT_KEY_B	   0x42
#  define RETROFLAT_KEY_C	   0x43
#  define RETROFLAT_KEY_D	   0x44
#  define RETROFLAT_KEY_E	   0x45
#  define RETROFLAT_KEY_F	   0x46
#  define RETROFLAT_KEY_G	   0x47
#  define RETROFLAT_KEY_H	   0x48
#  define RETROFLAT_KEY_I	   0x49
#  define RETROFLAT_KEY_J	   0x4a
#  define RETROFLAT_KEY_K	   0x4b
#  define RETROFLAT_KEY_L	   0x4c
#  define RETROFLAT_KEY_M	   0x4d
#  define RETROFLAT_KEY_N	   0x4e
#  define RETROFLAT_KEY_O	   0x4f
#  define RETROFLAT_KEY_P	   0x50
#  define RETROFLAT_KEY_Q	   0x51
#  define RETROFLAT_KEY_R	   0x52
#  define RETROFLAT_KEY_S	   0x53
#  define RETROFLAT_KEY_T	   0x54
#  define RETROFLAT_KEY_U	   0x55
#  define RETROFLAT_KEY_V	   0x56
#  define RETROFLAT_KEY_W	   0x57
#  define RETROFLAT_KEY_X	   0x58
#  define RETROFLAT_KEY_Y	   0x59
#  define RETROFLAT_KEY_Z	   0x60
#  define RETROFLAT_KEY_0     0x30
#  define RETROFLAT_KEY_1     0x31
#  define RETROFLAT_KEY_2     0x32
#  define RETROFLAT_KEY_3     0x33
#  define RETROFLAT_KEY_4     0x34
#  define RETROFLAT_KEY_5     0x35
#  define RETROFLAT_KEY_6     0x36
#  define RETROFLAT_KEY_7     0x37
#  define RETROFLAT_KEY_8     0x38
#  define RETROFLAT_KEY_9     0x39
#  define RETROFLAT_KEY_TAB	0
#  define RETROFLAT_KEY_SPACE	0x20
#  define RETROFLAT_KEY_ESC	0x1b
#  define RETROFLAT_KEY_ENTER	0x0d

/* Handle keys that send a double-code. */
#  define RETROFLAT_KEY_UP	      -3
#  define RETROFLAT_KEY_DOWN	   -4
#  define RETROFLAT_KEY_RIGHT	   -5
#  define RETROFLAT_KEY_LEFT	   -6
#  define RETROFLAT_KEY_HOME	   -7
#  define RETROFLAT_KEY_END	   -8
#  define RETROFLAT_KEY_PGUP     -9
#  define RETROFLAT_KEY_PGDN     -10
#  define RETROFLAT_KEY_DELETE   -11

#  define RETROFLAT_MOUSE_B_LEFT    -100
#  define RETROFLAT_MOUSE_B_RIGHT   -200

typedef void (__interrupt __far* retroflat_intfunc)( void );

#else
#  pragma message( "warning: not implemented" )

/**
 * \addtogroup maug_retroflt_config
 * \{
 */

/**
 * \brief A configuration object to use with the \ref maug_retroflt_config.
 *
 * This is a file by default, but may be different things (e.g. a registry key
 * or battery-backed SRAM block) on different platforms.
 */
typedef FILE* RETROFLAT_CONFIG;

/*! \} */ /* maug_retroflt_config */

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
   struct RETROFLAT_GLTEX tex;
};

/*! \brief Check to see if a bitmap is loaded. */
#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)

/*! \} */ /* maug_retroflt_bitmap */

/*! \brief Get the current screen width in pixels. */
#  define retroflat_screen_w()

/*! \brief Get the current screen height in pixels. */
#  define retroflat_screen_h()

/*! \brief Get the direct screen buffer or the VDP buffer if a VDP is loaded. */
#  define retroflat_screen_buffer() (&(g_retroflat_state->buffer))

/**
 * \brief This should be called in order to quit a program using RetroFlat.
 * \param retval The return value to pass back to the operating system.
 */
#  define retroflat_quit( retval_in )

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

typedef int RETROFLAT_COLOR_DEF;

#  define RETROFLAT_COLOR_BLACK        0
#  define RETROFLAT_COLOR_DARKBLUE     1
#  define RETROFLAT_COLOR_DARKGREEN    2
#  define RETROFLAT_COLOR_TEAL         3
#  define RETROFLAT_COLOR_DARKRED      4
#  define RETROFLAT_COLOR_VIOLET       5
#  define RETROFLAT_COLOR_BROWN        6
#  define RETROFLAT_COLOR_GRAY         7
#  define RETROFLAT_COLOR_DARKGRAY     8
#  define RETROFLAT_COLOR_BLUE         9
#  define RETROFLAT_COLOR_GREEN        10
#  define RETROFLAT_COLOR_CYAN         11
#  define RETROFLAT_COLOR_RED          12
#  define RETROFLAT_COLOR_MAGENTA      13
#  define RETROFLAT_COLOR_YELLOW       14
#  define RETROFLAT_COLOR_WHITE        15

/*! \} */ /* maug_retroflt_color */

/*! \} */ /* maug_retroflt_drawing */

/**
 * \addtogroup maug_retroflt_input
 * \{
 *
 * \addtogroup maug_retroflt_keydefs RetroFlat Key Definitions
 * \brief Keyboard and mouse controls RetroFlat is aware of, for use within the
 *        \ref maug_retroflt_input.
 *
 * The precise type and values of these constants vary by platform.
 *
 * \{
 */

#  define RETROFLAT_KEY_UP	   0
#  define RETROFLAT_KEY_DOWN	0
#  define RETROFLAT_KEY_RIGHT	0
#  define RETROFLAT_KEY_LEFT	0
#  define RETROFLAT_KEY_A	   0x41
#  define RETROFLAT_KEY_B	   0x42
#  define RETROFLAT_KEY_C	   0x43
#  define RETROFLAT_KEY_D	   0x44
#  define RETROFLAT_KEY_E	   0x45
#  define RETROFLAT_KEY_F	   0x46
#  define RETROFLAT_KEY_G	   0x47
#  define RETROFLAT_KEY_H	   0x48
#  define RETROFLAT_KEY_I	   0x49
#  define RETROFLAT_KEY_J	   0x4a
#  define RETROFLAT_KEY_K	   0x4b
#  define RETROFLAT_KEY_L	   0x4c
#  define RETROFLAT_KEY_M	   0x4d
#  define RETROFLAT_KEY_N	   0x4e
#  define RETROFLAT_KEY_O	   0x4f
#  define RETROFLAT_KEY_P	   0x50
#  define RETROFLAT_KEY_Q	   0x51
#  define RETROFLAT_KEY_R	   0x52
#  define RETROFLAT_KEY_S	   0x53
#  define RETROFLAT_KEY_T	   0x54
#  define RETROFLAT_KEY_U	   0x55
#  define RETROFLAT_KEY_V	   0x56
#  define RETROFLAT_KEY_W	   0x57
#  define RETROFLAT_KEY_X	   0x58
#  define RETROFLAT_KEY_Y	   0x59
#  define RETROFLAT_KEY_Z	   0x60
#  define RETROFLAT_KEY_0     0x30
#  define RETROFLAT_KEY_1     0x31
#  define RETROFLAT_KEY_2     0x32
#  define RETROFLAT_KEY_3     0x33
#  define RETROFLAT_KEY_4     0x34
#  define RETROFLAT_KEY_5     0x35
#  define RETROFLAT_KEY_6     0x36
#  define RETROFLAT_KEY_7     0x37
#  define RETROFLAT_KEY_8     0x38
#  define RETROFLAT_KEY_9     0x39
#  define RETROFLAT_KEY_TAB	0
#  define RETROFLAT_KEY_SPACE	0
#  define RETROFLAT_KEY_ESC	0
#  define RETROFLAT_KEY_ENTER	0
#  define RETROFLAT_KEY_HOME	0
#  define RETROFLAT_KEY_END	0

#  define RETROFLAT_MOUSE_B_LEFT    0
#  define RETROFLAT_MOUSE_B_RIGHT   0

/*! \} */ /* maug_retroflt_keydefs */

/*! \} */ /* maug_retroflt_input */

#endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* OpenGL can be called from several different APIs. */
#  if defined( RETROFLAT_OPENGL ) && !defined( RETROFLAT_API_LIBNDS )
#     include <GL/gl.h>
#     include <GL/glu.h>
#  endif /* RETROFLAT_OPENGL */

/* === Structures === */

/*! \brief Struct containing configuration values for a RetroFlat program. */
struct RETROFLAT_ARGS {
   /**
    * \brief Title to set for the main program Window if applicable on the
    *        target platform.
    */
   char* title;
   /*! \brief Relative path under which bitmap assets are stored. */
   char* assets_path;
   uint8_t flags;
   /*! \brief Relative path of local config file (if not using registry). */
   char* config_path;
#  ifdef RETROFLAT_API_PC_BIOS
   /*! \brief Desired screen or window width in pixels. */
   uint8_t screen_mode;
#  elif !defined( RETROFLAT_NO_CLI_SZ )
   int screen_w;
   /*! \brief Desired screen or window height in pixels. */
   int screen_h;
   /*! \brief Desired window X position in pixels. */
   int screen_x;
   /*! \brief Desired window Y position in pixels. */
   int screen_y;
#  endif /* RETROFLAT_API_PC_BIOS */
   uint8_t snd_flags;
#  if defined( RETROSND_API_WINMM )
   UINT snd_dev_id;
#  elif defined( RETROSND_API_PC_BIOS )
   uint16_t snd_io_base;
   uint8_t snd_driver;
#  elif defined( RETROSND_API_ALSA )
   uint8_t snd_client;
   uint8_t snd_port;
#  else
#     pragma message( "warning: sound args not specified" )
#  endif /* RETROSND_API_WINMM */
};

/*! \brief Global singleton containing state for the current platform. */
struct RETROFLAT_STATE {
   void*                   loop_data;
   MERROR_RETVAL           retval;
   /*! \brief \ref maug_retroflt_flags indicating current system status. */
   uint8_t                 retroflat_flags;
   char                    config_path[RETROFLAT_PATH_MAX + 1];
   char                    assets_path[RETROFLAT_ASSETS_PATH_MAX + 1];
   /*! \brief Index of available colors, initialized on platform init. */
   RETROFLAT_COLOR_DEF     palette[RETROFLAT_COLORS_SZ];
   /*! \brief Off-screen buffer bitmap. */
   struct RETROFLAT_BITMAP buffer;

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

   /**
    * \brief The screen width as seen by our program, before scaling.
    *
    * This is the scale of the buffer, which the platform-specific code
    * should then scale to match screen_v_h on blit.
    */
   int                  screen_v_w;
   /**
    * \brief The screen height as seen by our program, before scaling.
    *
    * This is the scale of the buffer, which the platform-specific code
    * should then scale to match screen_v_w on blit.
    */
   int                  screen_v_h;
   /*! \brief The screen width as seen by the system, after scaling. */
   int                  screen_w;
   /*! \brief The screen height as seen by the system, after scaling. */
   int                  screen_h;

   /* WARNING: The VDP requires the state specifier to be the same size
    *          as the one it was compiled for! Do not modify above here!
    */

   /* TODO: Put these in a platform-specific struct of some kind to maintain
    *       consistent state struct size for VDP?
    */

   retroflat_proc_resize_t on_resize;
   void* on_resize_data;

#  if defined( RETROFLAT_OPENGL )
   uint8_t tex_palette[RETROFLAT_COLORS_SZ][3];
#  endif /* RETROFLAT_OPENGL */

#if defined( RETROFLAT_API_ALLEGRO )

#  ifdef RETROFLAT_OS_DOS
   unsigned int         last_mouse;
   unsigned int         last_mouse_x;
   unsigned int         last_mouse_y;
#  endif /* RETROFLAT_OS_DOS */
   unsigned int         close_button;

#elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

#  ifndef RETROFLAT_API_SDL1
   SDL_Window*          window;
#  endif /* !RETROFLAT_API_SDL1 */
   int                  mouse_state;

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   HWND                 window;
   int16_t              last_idc; /* Last clicked button. */
#  ifdef RETROFLAT_SCREENSAVER
   HWND                 parent;
#  endif /* RETROFLAT_SCREENSAVER */
   HDC                  hdc_win;
#  ifdef RETROFLAT_OPENGL
   HGLRC                hrc_win;
#  endif /* RETROFLAT_OPENGL */
   uint8_t              last_key;
   uint8_t              vk_mods;
   unsigned int         last_mouse;
   unsigned int         last_mouse_x;
   unsigned int         last_mouse_y;
   retroflat_loop_iter  loop_iter;
   retroflat_loop_iter  frame_iter;

#  elif defined( RETROFLAT_API_LIBNDS )

   uint16_t*            sprite_frames[NDS_SPRITES_ACTIVE];
   int                  bg_id;
   uint8_t              bg_bmp_changed;
   uint8_t              window_bmp_changed;
   int                  window_id;
   int                  px_id;
   uint16_t             bg_tiles[1024];
   uint16_t             window_tiles[1024];

#  elif defined( RETROFLAT_API_GLUT )

   size_t               retroflat_next;
   retroflat_loop_iter  loop_iter;
   retroflat_loop_iter  frame_iter;
   int16_t              retroflat_last_key;

#  elif defined( RETROFLAT_API_PC_BIOS )

   retroflat_intfunc old_timer_interrupt;
   uint8_t old_video_mode;
   uint8_t cga_color_table[16];
   uint8_t cga_dither_table[16];
   uint8_t screen_mode;

#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

};

/* === Translation Module === */

/* Declare the prototypes so that internal functions can call each other. */

/**
 * \brief This should be called once in the main body of the program in order
 *        to enter the main loop. The main loop will continuously call
 *        loop_iter with data as an argument until retroflat_quit() is called.
 */
MERROR_RETVAL retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data );

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
 * \param target Pointer to the ::RETROFLAT_BITMAP to blit src onto.
 * \param src Pointer to the ::RETROFLAT_BITMAP to blit onto target.
 * \param s_x Left X coordinate to blit starting from on the source bitmap.
 * \param s_y Top Y coordinate to blit starting from on the source bitmap.
 * \param d_x Left X coordinate to blit to on the target bitmap.
 * \param d_y Top Y coordinate to blit to on the target bitmap.
 * \param w Pixel width of the region to blit.
 * \param h Pixel height of the region to blit.
 * \todo Currently s_x, s_y, w, and h are not functional on all platforms!
 */
void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int16_t w, int16_t h );

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
   int16_t x, int16_t y, uint8_t flags );

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

void retroflat_set_proc_resize(
   retroflat_proc_resize_t on_resize_in, void* data_in );

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

/**
 * \addtogroup maug_retroflt_config RetroFlat Config API
 * \brief Tools for loading and saving config data between sessions.
 * \{
 */

/**
 * \brief Open a configuration file/registry/SRAM/etc handle.
 * \param flags \ref maug_retroflt_config_flags to open config with.
 */
MERROR_RETVAL retroflat_config_open( RETROFLAT_CONFIG* config, uint8_t flags );

void retroflat_config_close( RETROFLAT_CONFIG* config );

/**
 * \brief Write the contents of a buffer to a config object.
 * \warning The config object must have been opened with the
 *          ::RETROFLAT_CONFIG_FLAG_W flag enabled!
 * \param buffer_type \ref maug_retroflt_config_btypes of the buffer provided.
 * \param buffer_sz_max Size of the buffer provided in bytes.
 */
size_t retroflat_config_write(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer, size_t buffer_sz_max );

/**
 * \brief Write the contents of a buffer to a config object.
 * \warning The config object must have been opened with the
 *          ::RETROFLAT_CONFIG_FLAG_W flag disabled!
 * \param buffer_type \ref maug_retroflt_config_btypes of the buffer provided.
 * \param buffer_out_sz_max Size of the buffer provided in bytes.
 */
size_t retroflat_config_read(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer_out, size_t buffer_out_sz_max,
   const void* default_out, size_t default_out_sz );

/*! \} */ /* maug_retroflt_config */

#ifdef RETROFLT_C

#  if defined( RETROFLAT_API_ALLEGRO ) || defined( RETROFLAT_API_PC_BIOS )
static volatile retroflat_ms_t g_ms = 0;
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_PC_BIOS */
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

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
/* For now, these are set by WinMain(), so they need to be outside of the
 * state that's zeroed on init()!
 */
HINSTANCE            g_retroflat_instance;
int                  g_retroflat_cmd_show;

#     ifdef RETROFLAT_WING
struct RETROFLAT_WING_MODULE g_w;
#     endif /* RETROFLAT_WING */
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>

/* Callback table is down below, after the statically-defined callbacks. */

/* === Function Definitions === */

#  if (defined( RETROFLAT_SOFT_SHAPES ) || defined( RETROFLAT_SOFT_LINES )) \
   && !defined( MAUG_NO_AUTO_C )
#     define RETROFP_C
#     include <retrofp.h>
#     define RETROSFT_C
#     include <retrosft.h>
#  endif /* RETROFLAT_SOFT_SHAPES */

#  if defined( RETROFLAT_OPENGL ) && !defined( MAUG_NO_AUTO_C )
#     define RETROGLU_C
#     include <retroglu.h>
#     define RETROFP_C
#     include <retrofp.h>
#     define RETROSFT_C
#     include <retrosft.h>
#  endif /* RETROFLAT_OPENGL */

#  if defined( RETROFLAT_VDP ) && defined( RETROFLAT_OS_UNIX )
#     include <dlfcn.h>
#  endif

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

static LRESULT CALLBACK WndProc(
   HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
) {
#     ifndef RETROFLAT_OPENGL
   PAINTSTRUCT ps;
   HDC hdc_paint = (HDC)NULL;
#     endif /* !RETROFLAT_OPENGL */
#     if defined( RETROFLAT_OPENGL )
   int pixel_fmt_int = 0;
   static PIXELFORMATDESCRIPTOR pixel_fmt = {
      sizeof( PIXELFORMATDESCRIPTOR ),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,
      RETROFLAT_OPENGL_BPP,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      16,
      0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
   };
#     endif /* RETROFLAT_OPENGL */

   switch( message ) {
      case WM_CREATE:

         g_retroflat_state->hdc_win = GetDC( hWnd );

#     if defined( RETROFLAT_OPENGL )
         
         pixel_fmt_int =
            ChoosePixelFormat( g_retroflat_state->hdc_win, &pixel_fmt );
         SetPixelFormat(
            g_retroflat_state->hdc_win, pixel_fmt_int, &pixel_fmt );

         debug_printf( 1, "setting up OpenGL context..." );

         g_retroflat_state->hrc_win =
            wglCreateContext( g_retroflat_state->hdc_win );
         if(
            FALSE == wglMakeCurrent( g_retroflat_state->hdc_win,
               g_retroflat_state->hrc_win )
         ) {
            retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
               "Error", "Error creating OpenGL context: %d",
               GetLastError() );
         }

#     else

         /* Setup the screen buffer. */
         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            debug_printf( 1, "retroflat: creating window buffer (%d x %d)...",
               g_retroflat_state->screen_w, g_retroflat_state->screen_h );
            /* Do this in its own function so a one-time setup isn't using stack
            * in our WndProc!
            */
            retroflat_create_bitmap(
               g_retroflat_state->screen_v_w,
               g_retroflat_state->screen_v_h,
               &(g_retroflat_state->buffer),
               RETROFLAT_FLAGS_SCREEN_BUFFER | RETROFLAT_FLAGS_OPAQUE );
            if( (HDC)NULL == g_retroflat_state->buffer.hdc_b ) {
               retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
                  "Error", "Could not determine buffer device context!" );
               g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
               retroflat_quit( g_retroflat_state->retval );
               break;
            }

         }
         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
               "Error", "Could not create screen buffer!" );
            g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retroflat_state->retval );
            break;
         }

#     endif /* RETROFLAT_OPENGL */
         break;

      case WM_CLOSE:
#     if defined( RETROFLAT_OPENGL )
         wglMakeCurrent( g_retroflat_state->hdc_win, NULL );
         wglDeleteContext( g_retroflat_state->hrc_win );
#     endif /* RETROFLAT_OPENGL */

         /* Quit on window close. */
         retroflat_quit( 0 );
         break;

#     if !defined( RETROFLAT_OPENGL )
      case WM_PAINT:

         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            error_printf( "screen buffer not ready!" );
            break;
         }

         /* Create HDC for window to blit to. */
         /* maug_mzero( &ps, sizeof( PAINTSTRUCT ) ); */
         hdc_paint = BeginPaint( hWnd, &ps );
         if( (HDC)NULL == hdc_paint ) {
            retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
               "Error", "Could not determine window device context!" );
            g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retroflat_state->retval );
            break;
         }

#        if defined( RETROFLAT_VDP )
         retroflat_vdp_call( "retroflat_vdp_flip" );
#        endif /* RETROFLAT_VDP */

#        ifdef RETROFLAT_WING
         if( (WinGStretchBlt_t)NULL != g_w.WinGStretchBlt ) {
            g_w.WinGStretchBlt(
               hdc_paint,
               0, 0,
               g_retroflat_state->screen_w, g_retroflat_state->screen_h,
               g_retroflat_state->buffer.hdc_b,
               0, 0,
               g_retroflat_state->screen_w,
               g_retroflat_state->screen_h
            );
#           ifdef RETROFLAT_API_WIN32
            GdiFlush();
#           endif /* RETROFLAT_API_WIN32 */
         } else {
#        endif /* RETROFLAT_WING */
         StretchBlt(
            hdc_paint,
            0, 0,
            g_retroflat_state->screen_w, g_retroflat_state->screen_h,
            g_retroflat_state->buffer.hdc_b,
            0, 0,
            g_retroflat_state->screen_v_w,
            g_retroflat_state->screen_v_h,
            SRCCOPY
         );
#        ifdef RETROFLAT_WING
         }
#        endif /* RETROFLAT_WING */

         DeleteDC( hdc_paint );
         EndPaint( hWnd, &ps );
         break;

#     endif /* !RETROFLAT_OPENGL */

      case WM_ERASEBKGND:
         return 1;

      case WM_KEYDOWN:
         switch( wParam ) {
         case VK_SHIFT:
            g_retroflat_state->vk_mods |= RETROFLAT_INPUT_MOD_SHIFT;
            break;

         case VK_CONTROL:
            g_retroflat_state->vk_mods |= RETROFLAT_INPUT_MOD_CTRL;
            break;

         /* TODO: Alt? */

         default:
            g_retroflat_state->last_key = wParam;
            break;
         }
         break;

      case WM_KEYUP:
         switch( wParam ) {
         case VK_SHIFT:
            g_retroflat_state->vk_mods &= ~RETROFLAT_INPUT_MOD_SHIFT;
            break;

         case VK_CONTROL:
            g_retroflat_state->vk_mods |= RETROFLAT_INPUT_MOD_CTRL;
            break;

         /* TODO: Alt? */

         }
         break;

      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
         g_retroflat_state->last_mouse = wParam;
         g_retroflat_state->last_mouse_x = GET_X_LPARAM( lParam );
         g_retroflat_state->last_mouse_y = GET_Y_LPARAM( lParam );
         break;

      /* TODO: Handle resize message. */

      case WM_DESTROY:
         if( retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            DeleteObject( g_retroflat_state->buffer.b );
         }
         PostQuitMessage( 0 );
         break;

      case WM_SIZE:
         retroflat_on_resize( LOWORD( lParam ), HIWORD( lParam ) );
         if( NULL != g_retroflat_state->on_resize ) {
            g_retroflat_state->on_resize(
               LOWORD( lParam ), HIWORD( lParam ),
               g_retroflat_state->on_resize_data );
         }
         break;

      case WM_TIMER:
         if(
#     ifdef RETROFLAT_OPENGL
            (HGLRC)NULL == g_retroflat_state->hrc_win ||
#     else
            !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ||
#     endif /* !RETROFLAT_OPENGL */
            hWnd != g_retroflat_state->window
         ) {
            /* Timer message was called prematurely. */
            break;
         }

         if( RETROFLAT_WIN_FRAME_TIMER_ID == wParam ) {
            /* Frame timer has expired. */
            assert( NULL != g_retroflat_state->frame_iter );
            g_retroflat_state->frame_iter( g_retroflat_state->loop_data );
         } else if( RETROFLAT_WIN_LOOP_TIMER_ID == wParam ) {
            /* Loop/tick timer has expired. */
            assert( NULL != g_retroflat_state->loop_iter );
            g_retroflat_state->loop_iter( g_retroflat_state->loop_data );
         }
         break;

      case WM_COMMAND:
         g_retroflat_state->last_idc = LOWORD( wParam );
         break;

      default:
         return DefWindowProc( hWnd, message, wParam, lParam );
   }

   return 0;
}

LPSTR* retroflat_win_cli( LPSTR cmd_line, int* argc_out ) {
   LPSTR* argv_out = NULL;
   int i = 0,
      arg_iter = 0,
      arg_start = 0,
      arg_idx = 0,
      arg_longest = 10; /* Program name. */
   MERROR_RETVAL retval = MERROR_OK;

   /* This uses calloc() to simplify things, since this works on Windows, the
    * only platform where this routine is used, anyway. */

   debug_printf( 1, "retroflat: win cli: %s", cmd_line );

   /* Get the number of args. */
   *argc_out = 1; /* Program name. */
   for( i = 0 ; '\0' != cmd_line[i - 1] ; i++ ) {
      if( ' ' != cmd_line[i] && '\0' != cmd_line[i] ) {
         arg_iter++;
      } else if( 0 < i ) {
         (*argc_out)++;
         if( arg_iter > arg_longest ) {
            /* This is the new longest arg. */
            arg_longest = arg_iter;
         }
         arg_iter = 0;
      }
   }

   argv_out = calloc( *argc_out, sizeof( char* ) );
   maug_cleanup_if_null_alloc( char**, argv_out );

   /* NULL program name. */
   argv_out[0] = calloc( 1, sizeof( char ) );
   maug_cleanup_if_null_alloc( char*, argv_out[0] );

   /* Copy args into array. */
   arg_idx = 1;
   for( i = 0 ; '\0' != cmd_line[i - 1] ; i++ ) {
      if( ' ' != cmd_line[i] && '\0' != cmd_line[i] ) {
         /* If this isn't a WS char, it's an arg. */
         if( 0 < i && ' ' == cmd_line[i - 1] ) {
            /* If this is first non-WS char, it's start of a new arg. */
            arg_start = i;
            arg_iter = 0;
         }
         arg_iter++;
         continue;
      }

      if( 0 < i && ' ' != cmd_line[i - 1] ) {
         /* If this is first WS char, it's the end of an arg. */
         assert( NULL == argv_out[arg_idx] );
         argv_out[arg_idx] = calloc( arg_iter + 1, sizeof( char ) );
         maug_cleanup_if_null_alloc( char*, argv_out[arg_idx] );
         strncpy( argv_out[arg_idx], &(cmd_line[arg_start]), arg_iter );
         arg_idx++; /* Start next arg. */
         arg_iter = 0; /* New arg is 0 long. */
         arg_start = i; /* New arg starts here (maybe). */
      }
   }

cleanup:

   if( MERROR_OK != retval && NULL != argv_out ) {
      for( i = 0 ; *argc_out > i ; i++ ) {
         free( argv_out[i] );
         argv_out[i] = NULL;
      }
      free( argv_out );
      argv_out = NULL;
   }

   return argv_out;
}

#  elif defined( RETROFLAT_API_GLUT )

#ifdef RETROFLAT_OS_OS2
void APIENTRY
#else
void
#endif /* RETROFLAT_OS_OS2 */
retroflat_glut_display( void ) {
   /* TODO: Work in frame_iter if provided. */
   if( NULL != g_retroflat_state->loop_iter ) {
      g_retroflat_state->loop_iter( g_retroflat_state->loop_data );
   }
}

#ifdef RETROFLAT_OS_OS2
void APIENTRY
#else
void
#endif /* RETROFLAT_OS_OS2 */
retroflat_glut_idle( void ) {
   uint32_t now = 0;

   now = retroflat_get_ms();
   if(
      RETROFLAT_FLAGS_UNLOCK_FPS !=
      (RETROFLAT_FLAGS_UNLOCK_FPS & g_retroflat_state->retroflat_flags) &&
      now < g_retroflat_state->retroflat_next
   ) {
      return;
   }
   
   glutPostRedisplay();

   if( now + retroflat_fps_next() > now ) {
      g_retroflat_state->retroflat_next = now + retroflat_fps_next();
   } else {
      /* Rollover protection. */
      g_retroflat_state->retroflat_next = 0;
   }
}

#     ifdef RETROFLAT_OS_OS2
void APIENTRY
#     elif defined( RETROFLAT_OS_WIN )
void 
#     else
void
#     endif /* RETROFLAT_OS_OS2 */
retroflat_glut_key( unsigned char key, int x, int y ) {
#     ifdef RETROFLAT_OS_WIN
      /* key -= 109; */
#     endif /* RETROFLAT_OS_WIN */
   debug_printf( 0, "key: %c (0x%02x)", key, key );
   g_retroflat_state->retroflat_last_key = key;
}

#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* Still inside RETROFLT_C! */

/* === */

int retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data
) {
   int retval = 0;

#  if defined( RETROFLAT_OS_WASM )

   /* TODO: Work in frame_iter if provided. */
   emscripten_cancel_main_loop();
   emscripten_set_main_loop_arg( loop_iter, data, 0, 0 );

#  elif defined( RETROFLAT_API_ALLEGRO ) || \
   defined( RETROFLAT_API_SDL1 ) || \
   defined( RETROFLAT_API_SDL2 ) || \
   defined( RETROFLAT_API_LIBNDS ) || \
   defined( RETROFLAT_API_PC_BIOS )

   retroflat_ms_t next = 0,
      now = 0;

   g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_RUNNING;
   do {
      if(
         RETROFLAT_FLAGS_UNLOCK_FPS !=
         (RETROFLAT_FLAGS_UNLOCK_FPS & g_retroflat_state->retroflat_flags) &&
         retroflat_get_ms() < next
      ) {
         /* Sleep/low power for a bit. */
#     ifdef RETROFLAT_API_LIBNDS
         swiWaitForVBlank();
#     endif /* RETROFLAT_API_LIBNDS */
         if( NULL != loop_iter ) {
            /* Run the loop iter as many times as possible. */
            loop_iter( data );
         }
         continue;
      }
      if( NULL != frame_iter ) {
         /* Run the frame iterator once per FPS tick. */
         frame_iter( data );
      }
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

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int msg_retval = 0;
   MSG msg;

   /* Set these to be called from WndProc later. */
   g_retroflat_state->loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;
   g_retroflat_state->frame_iter = (retroflat_loop_iter)frame_iter;

   if( NULL != frame_iter ) {
      debug_printf( 3, "setting up frame timer %u every %d ms...",
         RETROFLAT_WIN_FRAME_TIMER_ID, (int)(1000 / RETROFLAT_FPS) );
      if( !SetTimer(
         g_retroflat_state->window, RETROFLAT_WIN_FRAME_TIMER_ID,
         (int)(1000 / RETROFLAT_FPS), NULL )
      ) {
         retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
            "Error", "Could not create frame timer!" );
         retval = RETROFLAT_ERROR_TIMER;
         goto cleanup;
      }
   }

   if( NULL != loop_iter ) {
      debug_printf( 3, "setting up loop timer %u every 1 ms...",
         RETROFLAT_WIN_LOOP_TIMER_ID );
      if( !SetTimer(
         g_retroflat_state->window, RETROFLAT_WIN_LOOP_TIMER_ID, 1, NULL )
      ) {
         retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
            "Error", "Could not create loop timer!" );
         retval = RETROFLAT_ERROR_TIMER;
         goto cleanup;
      }
   }

   /* TODO: loop_iter is artificially slow on Windows! */

   /* Handle Windows messages until quit. */
   do {
      msg_retval = GetMessage( &msg, 0, 0, 0 );
      TranslateMessage( &msg );
      DispatchMessage( &msg );
      if( WM_QUIT == msg.message ) {
         /* Get retval from PostQuitMessage(). */
         retval = msg.wParam;
      }
   } while( WM_QUIT != msg.message && 0 < msg_retval );

cleanup:
#  elif defined( RETROFLAT_API_GLUT )

   g_retroflat_state->loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;
   g_retroflat_state->frame_iter = (retroflat_loop_iter)frame_iter;
   glutMainLoop();
   retval = g_retroflat_state->retval;

#  else
#     pragma message( "warning: loop not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   /* This should be set by retroflat_quit(). */
   return retval;
}

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
   case RETROFLAT_KEY_0: c = 0x30 + offset_lower; break;
   case RETROFLAT_KEY_1: c = offset_lower ? 0x31 : '!'; break;
   case RETROFLAT_KEY_2: c = 0x32; break;
   case RETROFLAT_KEY_3: c = 0x33; break;
   case RETROFLAT_KEY_4: c = 0x34; break;
   case RETROFLAT_KEY_5: c = 0x35; break;
   case RETROFLAT_KEY_6: c = 0x36; break;
   case RETROFLAT_KEY_7: c = 0x37; break;
   case RETROFLAT_KEY_8: c = 0x38; break;
   case RETROFLAT_KEY_9: c = 0x39; break;
   case RETROFLAT_KEY_SPACE: c = ' '; break;
   case RETROFLAT_KEY_BKSP: c = 0x08; break;
   case RETROFLAT_KEY_ENTER: c = '\n'; break;
   case RETROFLAT_KEY_SEMICOLON: c = offset_lower ? ';' : ':'; break;
   case RETROFLAT_KEY_DASH: c = offset_lower ? '-' : '_'; break;
   case RETROFLAT_KEY_SLASH: c = offset_lower ? '/' : '?'; break;
   case RETROFLAT_KEY_PERIOD: c = offset_lower ? '.' : '>'; break;
   case RETROFLAT_KEY_COMMA: c = offset_lower ? ',' : '<'; break;
   }

   return c;
}

#endif /* !RETROFLAT_NO_KEYBOARD */

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;
#  ifdef RETROFLAT_API_SDL2
   uint32_t sdl_msg_flags = 0;
#  elif (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_WIN )) || \
   (defined( RETROFLAT_API_GLUT) && defined( RETROFLAT_OS_WIN )) || \
   defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   uint32_t win_msg_flags = 0;
#  endif

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

#  if defined( RETROFLAT_API_ALLEGRO )
   allegro_message( "%s", msg_out );
#  elif defined( RETROFLAT_API_SDL2 )
   switch( (flags & RETROFLAT_MSG_FLAG_TYPE_MASK) ) {
   case RETROFLAT_MSG_FLAG_ERROR:
      sdl_msg_flags = SDL_MESSAGEBOX_ERROR;
      break;

   case RETROFLAT_MSG_FLAG_INFO:
      sdl_msg_flags = SDL_MESSAGEBOX_INFORMATION;
      break;

   case RETROFLAT_MSG_FLAG_WARNING:
      sdl_msg_flags = SDL_MESSAGEBOX_WARNING;
      break;
   }

   SDL_ShowSimpleMessageBox(
      sdl_msg_flags, title, msg_out, g_retroflat_state->window );
#  elif (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_WIN )) || \
   (defined( RETROFLAT_API_GLUT) && defined( RETROFLAT_OS_WIN )) || \
   defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   switch( (flags & RETROFLAT_MSG_FLAG_TYPE_MASK) ) {
   case RETROFLAT_MSG_FLAG_ERROR:
      win_msg_flags |= MB_ICONSTOP;
      break;

   case RETROFLAT_MSG_FLAG_INFO:
      win_msg_flags |= MB_ICONINFORMATION;
      break;

   case RETROFLAT_MSG_FLAG_WARNING:
      win_msg_flags |= MB_ICONEXCLAMATION;
      break;
   }

   MessageBox( retroflat_root_win(), msg_out, title, win_msg_flags );
#  elif (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_UNIX )) || \
   (defined( RETROFLAT_API_GLUT) && defined( RETROFLAT_OS_UNIX ))

   /* TODO */
   error_printf( "%s", msg_out );

#  elif defined( RETROFLAT_API_PC_BIOS )

   /* TODO: Display error somehow. */
   error_printf( "%s", msg_out );

#  else
#     pragma message( "warning: not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   va_end( vargs );
}

#  ifdef RETROSND_ARGS

static int retrosnd_cli_rsl( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rsl", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      args->snd_flags |= RETROSND_ARGS_FLAG_LIST_DEVS;
   }
   return RETROFLAT_OK;
}

static int retrosnd_cli_rsd( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rsd", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
#     ifdef RETROSND_API_PC_BIOS
      /* TODO: Parse device. */
#     elif defined( RETROSND_API_ALSA )
      /* TODO: Parse device. */
#     elif defined( RETROSND_API_WINMM )
      debug_printf( 3, "setting MIDI device to rsd arg: %s", arg );
      args->snd_dev_id = atoi( arg );
#     endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */
   }
   return RETROFLAT_OK;
}

static int retrosnd_cli_rsd_def(
   const char* arg, struct RETROFLAT_ARGS* args
) {
   char* env_var = NULL;
   MERROR_RETVAL retval = MERROR_OK;
#     if defined( RETROSND_API_PC_BIOS ) || defined( RETROSND_API_ALSA )
   size_t i = 0;
#     endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA */

#     ifdef RETROSND_API_PC_BIOS
   if( NULL != env_var ) {
      env_var = getenv( "MAUG_MIDI_DOS" );

      /* Return MERROR_OK since this isn't fatal and will just cause sound
         * init to fail later.
         */
      maug_cleanup_if_null_msg(
         char*, env_var, MERROR_OK, "MAUG_MIDI_DOS variable not found!" );

      debug_printf( 2, "env: MAUG_MIDI_DOS: %s", env_var );

      /* Turn comma separator into NULL split. */
      for( i = 0 ; strlen( env_var ) > i ; i++ ) {
         if( ',' == env_var[i] ) {
            /* Split into two null-terminated strings. */
            env_var[i] = '\0';
         }
      }

      if( 0 == strcmp( env_var, "mpu" ) ) {
         debug_printf( 3, "selecting MIDI driver: mpu" );
         args->snd_driver = 2;
      } else if( 0 == strcmp( env_var, "gus" ) ) {
         debug_printf( 3, "selecting MIDI driver: gus" );
         args->snd_driver = 4;
      } else if( 0 == strcmp( env_var, "adlib" ) ) {
         debug_printf( 3, "selecting MIDI driver: adlib" );
         args->snd_driver = 8;
      }
      args->snd_io_base = strtoul( &(env_var[i]), NULL, 16 );
      debug_printf( 3, "setting MIDI I/O base: %u", args->snd_io_base );
   } else {
      /* default */
      debug_printf( 3, "default MIDI driver: adlib" );
      args->snd_driver = 8;
      args->snd_io_base = 0x388;
   }
#     elif defined( RETROSND_API_ALSA )
   if( 0 == args->snd_client ) {
      env_var = getenv( "MAUG_MIDI_ALSA" );

      /* Return MERROR_OK since this isn't fatal and will just cause sound
         * init to fail later.
         */
      maug_cleanup_if_null_msg(
         char*, env_var, MERROR_OK, "MAUG_MIDI_ALSA variable not found!" );

      debug_printf( 2, "env: MAUG_MIDI_ALSA: %s", env_var );

      for( i = 0 ; strlen( env_var ) > i ; i++ ) {
         if( ':' == env_var[i] ) {
            /* Split into two null-terminated strings. */
            env_var[i] = '\0';
         }
      }

      args->snd_client = atoi( env_var );
      args->snd_port = atoi( &(env_var[i]) );
      debug_printf( 3, "setting MIDI device to: %u:%u",
         args->snd_client, args->snd_port );
   }

#     elif defined( RETROSND_API_WINMM )
   env_var = getenv( "MAUG_MIDI_WIN" );

   /* Return MERROR_OK since this isn't fatal and will just cause sound
      * init to fail later.
      */
   maug_cleanup_if_null_msg(
      char*, env_var, MERROR_OK, "MAUG_MIDI_WIN variable not found!" );

   debug_printf( 2, "env: MAUG_MIDI_WIN: %s", env_var );

   if( NULL != env_var ) {
      args->snd_dev_id = atoi( env_var );
   } else {
      args->snd_dev_id = 0;
   }
   debug_printf( 3, "setting MIDI device to: %u", args->snd_dev_id );
#     endif /* RETROSND_API_PC_BIOS || RETROSND_API_ALSA || RETROSND_API_WINMM */

cleanup:
   return retval;
}

#  endif /* RETROSND_ARGS */

#  ifdef RETROFLAT_SCREENSAVER

/* Windows screensaver (.scr) command-line arguments. */

static int retroflat_cli_p( const char* arg, struct RETROFLAT_ARGS* args ) {
#ifdef __WIN64__
   /* 64-bit Windows has 64-bit pointers! */
   intptr_t hwnd_long = 0;
#else
   long hwnd_long = 0;
#endif /* __GNUC__ */
   if( 0 == strncmp( MAUG_CLI_SIGIL "p", arg, MAUG_CLI_SIGIL_SZ + 2 ) ) {
      /* The next arg must be the new var. */
   } else {
#ifdef __WIN64__
      hwnd_long = atoll( arg );
#else
      hwnd_long = atol( arg );
#endif /* __GNUC__ */
      g_retroflat_state->parent = (HWND)hwnd_long;
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_s( const char* arg, struct RETROFLAT_ARGS* args ) {
   debug_printf( 3, "using screensaver mode..." );
   args->flags |= RETROFLAT_FLAGS_SCREENSAVER;
   return RETROFLAT_OK;
}

#  endif /* RETROFLAT_SCREENSAVER */

#  ifdef RETROFLAT_API_PC_BIOS

static int retroflat_cli_rfm( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfm", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_mode = atoi( arg );
      debug_printf( 3, "choosing screen mode: %u", args->screen_mode );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_rfm_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == args->screen_mode ) {
      /* TODO: Autodetect best available? */
      args->screen_mode = RETROFLAT_SCREEN_MODE_VGA;
   }
   return RETROFLAT_OK;
}

#  elif !defined( RETROFLAT_NO_CLI_SZ )

static int retroflat_cli_rfx( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfx", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_x = atoi( arg );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_rfx_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == args->screen_w ) {
      args->screen_x = 0;
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_rfy( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfy", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_y = atoi( arg );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_rfy_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == args->screen_h ) {
      args->screen_y = 0;
   }
   return RETROFLAT_OK;
}

static void retroflat_cli_apply_scale( struct RETROFLAT_ARGS* args ) {
#if defined( RETROFLAT_API_SDL2 )
   /*
   if(
      RETROFLAT_FLAGS_SCALE2X == (RETROFLAT_FLAGS_SCALE2X & args->flags)
   ) {
      args->screen_w *= 2;
      debug_printf( 1, "doubling screen_w to: %d", args->screen_w );
      args->screen_h *= 2;
      debug_printf( 1, "doubling screen_h to: %d", args->screen_h );
   }
   */
#endif /* RETROFLAT_API_SDL2 */
}

static int retroflat_cli_rfw( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfw", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_w = atoi( arg );
      retroflat_cli_apply_scale( args );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_rfw_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == args->screen_w ) {
      args->screen_w = RETROFLAT_DEFAULT_SCREEN_W;
   }
   retroflat_cli_apply_scale( args );
   return RETROFLAT_OK;
}

static int retroflat_cli_rfh( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfh", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_h = atoi( arg );
      retroflat_cli_apply_scale( args );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_rfh_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == args->screen_h ) {
      args->screen_h = RETROFLAT_DEFAULT_SCREEN_H;
   }
   retroflat_cli_apply_scale( args );
   return RETROFLAT_OK;
}

#  endif /* RETROFLAT_API_PC_BIOS || !RETROFLAT_NO_CLI_SZ */

#  ifndef MAUG_NO_CONFIG

static int retroflat_cli_c( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfc", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      debug_printf( 1, "setting config path to: %s", arg );
      strncpy( g_retroflat_state->config_path, arg, RETROFLAT_PATH_MAX );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_c_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   memset( g_retroflat_state->config_path, '\0', RETROFLAT_PATH_MAX + 1 );

   if( NULL != args->config_path ) {
      debug_printf( 1, "setting config path to: %s", args->config_path );
      strncpy(
         g_retroflat_state->config_path,
         args->config_path, RETROFLAT_PATH_MAX );
   } else {
      debug_printf( 1, "setting config path to: %s%s",
         args->title, RETROFLAT_CONFIG_EXT );
      strncpy(
         g_retroflat_state->config_path, args->title, RETROFLAT_PATH_MAX );
      strncat(
         g_retroflat_state->config_path,
         RETROFLAT_CONFIG_EXT, RETROFLAT_PATH_MAX );
   }

   return RETROFLAT_OK;
}

#  endif /* !MAUG_CLI_SIGIL_SZ */

#  ifdef RETROFLAT_VDP
static int retroflat_cli_vdp( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "vdp", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* Next arg is VDP args str. */
   } else {
      strncpy( g_retroflat_state->vdp_args, arg, RETROFLAT_VDP_ARGS_SZ_MAX );
      debug_printf( 1, "VDP args: %s", g_retroflat_state->vdp_args );
   }
   return RETROFLAT_OK;
}
#  endif /* RETROFLAT_VDP */

static int retroflat_cli_u( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfu", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      debug_printf( 1, "unlocking FPS..." );
      args->flags |= RETROFLAT_FLAGS_UNLOCK_FPS;
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_u_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   args->flags &= ~RETROFLAT_FLAGS_UNLOCK_FPS;
   return RETROFLAT_OK;
}

/* === */

#  ifdef RETROFLAT_API_ALLEGRO

/* Allegro-specific callbacks for init, below. */

void retroflat_on_ms_tick() {
   if( NULL == g_retroflat_state ) {
      debug_printf( 1, "no state!" );
   } else {
      g_ms++;
   }
}

void retroflat_on_close_button() {
   g_retroflat_state->close_button = 1;
}
END_OF_FUNCTION( retroflat_on_close_button )

#  endif /* RETROFLAT_API_ALLEGRO */

/* === */

#  ifdef RETROFLAT_API_PC_BIOS

void __interrupt __far retroflat_timer_handler() {
   static unsigned long count = 0;

   ++g_ms;
   count += RETROFLAT_DOS_TIMER_DIV; /* Original DOS timer in parallel. */
   if( 65536 <= count ) {
      /* Call the original handler. */
      count -= 65536;
      _chain_intr( g_retroflat_state->old_timer_interrupt );
   } else {
      /* Acknowledge interrupt */
      outp( 0x20, 0x20 );
   }
}

#  endif /* RETROFLAT_API_PC_BIOS */

/* === */

/* Still inside RETROFLT_C! */

int retroflat_init( int argc, char* argv[], struct RETROFLAT_ARGS* args ) {

   /* = Declare Init Vars = */

   int retval = 0;
#  if defined( RETROFLAT_API_ALLEGRO ) && defined( RETROFLAT_OS_DOS )
#     if 0
   union REGS regs;
   struct SREGS sregs;
#     endif
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   WNDCLASS wc;
   RECT wr = { 0, 0, 0, 0 };
   DWORD window_style = RETROFLAT_WIN_STYLE;
   DWORD window_style_ex = 0;
   int ww = 0,
      wh = 0;
#  elif defined( RETROFLAT_API_SDL1 )
   const SDL_VideoInfo* info = NULL;
   char sdl_video_parms[256] = "";
#     if defined( RETROFLAT_OPENGL )
   int gl_retval = 0,
      gl_depth = 16;
#     endif /* RETROFLAT_OPENGL */
#  elif defined( RETROFLAT_API_LIBNDS )
   int i = 0;
#  elif defined( RETROFLAT_API_GLUT )
   unsigned int glut_init_flags = 0;
#  elif defined( RETROFLAT_API_PC_BIOS )
   union REGS r;
   struct SREGS s;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   /* = Begin Init Procedure = */

#  if defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
#     if defined( RETROFLAT_SDL_ICO )
   SDL_Surface* icon = NULL;
#     endif /* RETROFLAT_SDL_ICO */
#  endif /* RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 */

#  ifdef RETROFLAT_COMMIT_HASH
   debug_printf( 1, "retroflat commit: " RETROFLAT_COMMIT_HASH  );
#  endif /* RETROFLAT_COMMIT_HASH */

   debug_printf( 1, "retroflat: initializing..." );

   /* System sanity checks. */
   assert( 4 == sizeof( uint32_t ) );
   assert( 4 == sizeof( int32_t ) );
   assert( 2 == sizeof( uint16_t ) );
   assert( 2 == sizeof( int16_t ) );
   assert( 1 == sizeof( uint8_t ) );
   assert( 1 == sizeof( int8_t ) );
   assert( NULL != args );

   debug_printf( 1, "retroflat: allocating state (" SIZE_T_FMT " bytes)...",
      sizeof( struct RETROFLAT_STATE ) );

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

   debug_printf( 1, "retroflat: parsing args..." );

   /* All platforms: add command-line args based on compile definitons. */

#  ifdef RETROSND_ARGS
	maug_add_arg( MAUG_CLI_SIGIL "rsd", MAUG_CLI_SIGIL_SZ + 4,
      "Select MIDI device", 0, (maug_cli_cb)retrosnd_cli_rsd,
         (maug_cli_cb)retrosnd_cli_rsd_def, args );
	maug_add_arg( MAUG_CLI_SIGIL "rsl", MAUG_CLI_SIGIL_SZ + 4,
      "List MIDI devices", 0, (maug_cli_cb)retrosnd_cli_rsl, NULL, args );
#  endif /* RETROSND_ARGS */

#  ifdef RETROFLAT_SCREENSAVER
	maug_add_arg( MAUG_CLI_SIGIL "p", MAUG_CLI_SIGIL_SZ + 2,
      "Preview screensaver", 0, (maug_cli_cb)retroflat_cli_p, NULL, args );
	maug_add_arg( MAUG_CLI_SIGIL "s", MAUG_CLI_SIGIL_SZ + 2,
      "Launch screensaver", 0, (maug_cli_cb)retroflat_cli_s, NULL, args );
#  endif /* RETROFLAT_SCREENSAVER */

#  ifdef RETROFLAT_API_PC_BIOS
   maug_add_arg( MAUG_CLI_SIGIL "rfm", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen mode.", 0,
      (maug_cli_cb)retroflat_cli_rfm,
      (maug_cli_cb)retroflat_cli_rfm_def, args );
#  elif !defined( RETROFLAT_NO_CLI_SZ )
   maug_add_arg( MAUG_CLI_SIGIL "rfx", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen X position.", 0,
      (maug_cli_cb)retroflat_cli_rfx,
      (maug_cli_cb)retroflat_cli_rfx_def, args );
   maug_add_arg( MAUG_CLI_SIGIL "rfy", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen Y position.", 0,
      (maug_cli_cb)retroflat_cli_rfy,
      (maug_cli_cb)retroflat_cli_rfy_def, args );
   maug_add_arg( MAUG_CLI_SIGIL "rfw", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen width.", 0,
      (maug_cli_cb)retroflat_cli_rfw,
      (maug_cli_cb)retroflat_cli_rfw_def, args );
   maug_add_arg( MAUG_CLI_SIGIL "rfh", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen height.", 0,
      (maug_cli_cb)retroflat_cli_rfh,
      (maug_cli_cb)retroflat_cli_rfh_def, args );
#  endif /* !RETROFLAT_NO_CLI_SZ */

#  ifdef RETROFLAT_VDP
   maug_add_arg( MAUG_CLI_SIGIL "vdp", MAUG_CLI_SIGIL_SZ + 4,
      "Pass a string of args to the VDP.", 0,
      (maug_cli_cb)retroflat_cli_vdp, NULL, args );
#  endif /* RETROFLAT_VDP */

#  ifndef MAUG_NO_CONFIG
   maug_add_arg( MAUG_CLI_SIGIL "rfc", MAUG_CLI_SIGIL_SZ + 4,
      "Set the config path.", 0,
      (maug_cli_cb)retroflat_cli_c, (maug_cli_cb)retroflat_cli_c_def, args );
#  endif /* !MAUG_NO_CONFIG */

   maug_add_arg( MAUG_CLI_SIGIL "rfu", MAUG_CLI_SIGIL_SZ + 4,
      "Unlock FPS.", 0,
      (maug_cli_cb)retroflat_cli_u, (maug_cli_cb)retroflat_cli_u_def, args );

   /* Parse command line args. */
   retval = maug_parse_args( argc, argv );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

   if(
      RETROFLAT_FLAGS_UNLOCK_FPS == (RETROFLAT_FLAGS_UNLOCK_FPS & args->flags)
   ) {
      g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_UNLOCK_FPS;
   }

   debug_printf( 1, "retroflat: setting config..." );

   /* Set the assets path. */
   memset( g_retroflat_state->assets_path, '\0', RETROFLAT_ASSETS_PATH_MAX );
   if( NULL != args->assets_path ) {
      strncpy( g_retroflat_state->assets_path,
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
   /* TODO: Handle window resizing someday! */
   g_retroflat_state->screen_v_w = args->screen_w;
   g_retroflat_state->screen_v_h = args->screen_h;
   if(
      RETROFLAT_FLAGS_SCALE2X == (RETROFLAT_FLAGS_SCALE2X & args->flags)
   ) {
      debug_printf( 1, "setting SDL window scale to 2x..." );
      g_retroflat_state->screen_w = args->screen_w * 2;
      g_retroflat_state->screen_h = args->screen_h * 2;
   } else {
      g_retroflat_state->screen_w = args->screen_w;
      g_retroflat_state->screen_h = args->screen_h;
   }
#  endif /* RETROFLAT_NO_CLI_SZ */

#  ifdef RETROFLAT_OPENGL
   debug_printf( 1, "setting up texture palette..." );
#     define RETROFLAT_COLOR_TABLE_TEX( idx, name_l, name_u, r, g, b, cgac, cgad ) \
         g_retroflat_state->tex_palette[idx][0] = r; \
         g_retroflat_state->tex_palette[idx][1] = g; \
         g_retroflat_state->tex_palette[idx][2] = b;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_TEX )
#  endif /* RETROFLAT_OPENGL */

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

   srand( time( NULL ) );

   if( allegro_init() ) {
      allegro_message( "could not setup allegro!" );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   install_keyboard();
#     if !defined( RETROFLAT_OS_DOS )
   /* XXX: Broken in DOS. */
   install_timer();
   install_int( retroflat_on_ms_tick, 1 );
#     endif /* RETROFLAT_OS_DOS */

#     ifdef RETROFLAT_OS_DOS
   /* Don't try windowed mode in DOS. */
   if(
      set_gfx_mode( GFX_AUTODETECT, args->screen_w, args->screen_h, 0, 0 )
   ) {
#     else
   if( 
      /* TODO: Set window position. */
      set_gfx_mode(
         GFX_AUTODETECT_WINDOWED, args->screen_w, args->screen_h, 0, 0 )
   ) {
#     endif /* RETROFLAT_OS_DOS */

      allegro_message( "could not setup graphics!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#     define RETROFLAT_COLOR_TABLE_ALLEGRO_INIT( i, name_l, name_u, r, g, b, cgac, cgad ) \
         g_retroflat_state->palette[i] = makecol( r, g, b );

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_ALLEGRO_INIT )

   LOCK_FUNCTION( retroflat_on_close_button );
   set_close_button_callback( retroflat_on_close_button );

#     ifndef RETROFLAT_OS_DOS
   if( NULL != args->title ) {
      retroflat_set_title( args->title );
   }

   /* XXX: Broken in DOS. */
   if( 0 > install_mouse() ) {
      allegro_message( "could not setup mouse!" );
      retval = RETROFLAT_ERROR_MOUSE;
      goto cleanup;
   }
#     endif /* !RETROFLAT_OS_DOS */

#     ifdef RETROFLAT_OS_DOS
#        if 0
   regs.w.ax = 0x9;
   regs.w.bx = 0x0;
   regs.w.cx = 0x0;
   regs.x.edx = FP_OFF( g_retroflat_state->mouse_cursor );
   sregs.es = FP_SEG( g_retroflat_state->mouse_cursor );
   int386x( 0x33, &regs, &regs, &sregs );
#        endif
#     endif /* RETROFLAT_OS_DOS */

   g_retroflat_state->buffer.b = 
      create_bitmap( args->screen_w, args->screen_h );
   maug_cleanup_if_null(
      BITMAP*, g_retroflat_state->buffer.b, RETROFLAT_ERROR_GRAPHICS );

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   /* Random seed. */
   srand( time( NULL ) );

   /* Startup SDL. */
   if( SDL_Init( SDL_INIT_VIDEO ) ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Error initializing SDL: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   /* Get info on best available video mode. */
   info = SDL_GetVideoInfo();
   maug_cleanup_if_null_alloc( SDL_VideoInfo*, info );

   debug_printf( 3, "maximum window size: %ux%u",
      info->current_w, info->current_h );

#     ifndef RETROFLAT_OS_WIN

   /* TODO: Maximum screen size detection returns bogus values in Windows! */

   /* Setup default screen position. */
   if( 0 == args->screen_x ) {
      /* Get screen width so we can center! */
      args->screen_x = (info->current_w / 2) -
         (g_retroflat_state->screen_w / 2);
   }

   if( 0 == args->screen_y ) {
      /* Get screen height so we can center! */
      args->screen_y = (info->current_h / 2) -
         (g_retroflat_state->screen_h / 2);
   }

   maug_snprintf( sdl_video_parms, 255, "SDL_VIDEO_WINDOW_POS=%d,%d",
       args->screen_x, args->screen_y );
   putenv( sdl_video_parms );

#     endif /* !RETROFLAT_OS_WIN */

   /* Setup color palettes. */
#     ifdef RETROFLAT_OPENGL
#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
            g_retroflat_state->palette[idx] = RETROGLU_COLOR_ ## name_u;
#     else
#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
            g_retroflat_state->palette[idx].r = rd; \
            g_retroflat_state->palette[idx].g = gd; \
            g_retroflat_state->palette[idx].b = bd;
#     endif /* RETROFLAT_OPENGL */
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL )

#     ifdef RETROFLAT_OPENGL
   if(
      RETROFLAT_FLAGS_UNLOCK_FPS == (RETROFLAT_FLAGS_UNLOCK_FPS & args->flags)
   ) {
      SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );
   }
   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
   do {
      /* Retry with smaller depth buffers if this fails. */
      gl_retval = SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, gl_depth );
      if( gl_retval ) {
         error_printf( "unable to set depth buffer to %d!", gl_depth );
         gl_depth -= 4;
      }
   } while( gl_retval );
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
#     endif /* RETROFLAT_OPENGL */

   if( NULL != args->title ) {
      retroflat_set_title( args->title );
   }

#     ifdef RETROFLAT_SDL_ICO
   debug_printf( 1, "setting SDL window icon..." );
   icon = SDL_LoadBMP_RW(
      SDL_RWFromConstMem( obj_ico_sdl_ico_bmp,
      obj_ico_sdl_ico_bmp_len ), 1 );
   maug_cleanup_if_null( SDL_Surface*, icon, RETROFLAT_ERROR_BITMAP );
   SDL_WM_SetIcon( icon, 0 ); /* TODO: Constant mask. */
#     endif /* RETROFLAT_SDL_ICO */

   g_retroflat_state->buffer.surface = SDL_SetVideoMode(
      g_retroflat_state->screen_w, g_retroflat_state->screen_h,
      info->vfmt->BitsPerPixel,
      SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_ANYFORMAT
#     ifdef RETROFLAT_OPENGL
      | SDL_OPENGL
#     endif /* RETROFLAT_OPENGL */
   );
   maug_cleanup_if_null(
      SDL_Surface*, g_retroflat_state->buffer.surface,
      RETROFLAT_ERROR_GRAPHICS );

   /* Setup key repeat. */
   if(
      RETROFLAT_FLAGS_KEY_REPEAT == (RETROFLAT_FLAGS_KEY_REPEAT & args->flags)
   ) {
      if( 0 != SDL_EnableKeyRepeat(
         1, SDL_DEFAULT_REPEAT_INTERVAL
      ) ) {
         error_printf( "could not enable key repeat!" );
      } else {
         debug_printf( 3, "key repeat enabled" );
      }
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   srand( time( NULL ) );

   if( SDL_Init( SDL_INIT_VIDEO ) ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Error initializing SDL: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   /* Setup color palettes. */
#     ifdef RETROFLAT_OPENGL
#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
            g_retroflat_state->palette[idx] = RETROGLU_COLOR_ ## name_u;
#     else
#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
            g_retroflat_state->palette[idx].r = rd; \
            g_retroflat_state->palette[idx].g = gd; \
            g_retroflat_state->palette[idx].b = bd;
#     endif /* RETROFLAT_OPENGL */
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL )

   /* Create the main window. */
   g_retroflat_state->window = SDL_CreateWindow( args->title,
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      args->screen_w, args->screen_h, RETROFLAT_WIN_FLAGS );
   maug_cleanup_if_null( SDL_Window*, g_retroflat_state->window, RETROFLAT_ERROR_GRAPHICS );

   /* Create the main renderer. */
   g_retroflat_state->buffer.renderer = SDL_CreateRenderer(
      g_retroflat_state->window, -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE );
   maug_cleanup_if_null(
      SDL_Renderer*, g_retroflat_state->buffer.renderer,
      RETROFLAT_ERROR_GRAPHICS );

   /* Create the buffer texture. */
   g_retroflat_state->buffer.texture =
      SDL_CreateTexture( g_retroflat_state->buffer.renderer,
      SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      g_retroflat_state->screen_w, g_retroflat_state->screen_h );

   /* TODO: This doesn't seem to do anything. */
   if(
      RETROFLAT_FLAGS_SCALE2X == (RETROFLAT_FLAGS_SCALE2X & args->flags)
   ) {
      debug_printf( 1, "setting SDL window scale to 2x..." );
      SDL_RenderSetScale( g_retroflat_state->buffer.renderer, 2.0f, 2.0f );
   }

#     ifdef RETROFLAT_SDL_ICO
   debug_printf( 1, "setting SDL window icon..." );
   icon = SDL_LoadBMP_RW(
      SDL_RWFromConstMem( obj_ico_sdl_ico_bmp,
      obj_ico_sdl_ico_bmp_len ), 1 );
   maug_cleanup_if_null( SDL_Surface*, icon, RETROFLAT_ERROR_BITMAP );
   SDL_SetWindowIcon( g_retroflat_state->window, icon );
#     endif /* RETROFLAT_SDL_ICO */

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

#     ifdef RETROFLAT_API_WINCE
   srand( GetTickCount() );
#     else
   srand( time( NULL ) );
#     endif /* RETROFLAT_API_WINCE */

   /* Setup color palettes. */
   /* TODO: For WinG, try to make the indexes match system palette? */
#     ifdef RETROFLAT_OPENGL
#        define RETROFLAT_COLOR_TABLE_WIN( idx, name_l, name_u, r, g, b, cgac, cgad ) \
            g_retroflat_state->palette[idx] = RETROGLU_COLOR_ ## name_u;
#     else
#        define RETROFLAT_COLOR_TABLE_WIN( idx, name_l, name_u, r, g, b, cgac, cgad ) \
            g_retroflat_state->palette[idx] = RGB( r, g, b );
#     endif /* RETROFLAT_OPENGL */

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN )

#     ifdef RETROFLAT_WING
   debug_printf( 3, "attempting to link WinG..." );
   /* Dynamically load the WinG procedures. */
#        ifdef RETROFLAT_API_WIN32
   g_w.module = LoadLibrary( "wing32.dll" );
   if( (HMODULE)NULL == g_w.module ) {
#        elif defined( RETROFLAT_API_WIN16 )
   g_w.module = LoadLibrary( "wing.dll" );
   if( HINSTANCE_ERROR == g_w.module ) {
#        endif
      g_w.success = 0;
   } else {
      g_w.success = 1;

#        ifdef RETROFLAT_API_WIN32
#           define RETROFLAT_WING_LLTABLE_LOAD_PROC( retval, proc, ord ) \
               g_w.proc = (proc ## _t)GetProcAddress( g_w.module, #proc ); \
               if( (proc ## _t)NULL == g_w.proc ) { \
                  g_w.success = 0; \
               }
#        elif defined( RETROFLAT_API_WIN16 )
#           define RETROFLAT_WING_LLTABLE_LOAD_PROC( retval, proc, ord ) \
               g_w.proc = (proc ## _t)GetProcAddress( \
                  g_w.module, MAKEINTRESOURCE( ord ) ); \
               if( (proc ## _t)NULL == g_w.proc ) { \
                  retroflat_message( \
                     RETROFLAT_MSG_FLAG_ERROR, "Error", \
                     "Unable to link WinG proc: %s", #proc ); \
                  g_w.success = 0; \
               }
#        endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

      RETROFLAT_WING_LLTABLE( RETROFLAT_WING_LLTABLE_LOAD_PROC )

   }

   if( !g_w.success ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Unable to link WinG!" );
   }
#     endif /* RETROFLAT_WING */

   /* Get the *real* size of the window, including titlebar. */
   wr.right = g_retroflat_state->screen_w;
   wr.bottom = g_retroflat_state->screen_h;
#     ifndef RETROFLAT_API_WINCE
   AdjustWindowRect( &wr, RETROFLAT_WIN_STYLE, FALSE );
#     endif /* !RETROFLAT_API_WINCE */

   memset(
      &(g_retroflat_state->buffer), '\0', sizeof( struct RETROFLAT_BITMAP ) );

   debug_printf( 1, "retroflat: creating window class..." );

   memset( &wc, '\0', sizeof( WNDCLASS ) );

   wc.lpfnWndProc   = (WNDPROC)&WndProc;
   wc.hInstance     = g_retroflat_instance;
#     ifdef RETROFLAT_ICO_RES_ID
   wc.hIcon         = LoadIcon(
      g_retroflat_instance, MAKEINTRESOURCE( RETROFLAT_ICO_RES_ID ) );
#     endif /* RETROFLAT_ICO_RES_ID */
   wc.hCursor       = LoadCursor( 0, IDC_ARROW );
   wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );
   /* wc.lpszMenuName  = MAKEINTRESOURCE( IDR_MAINMENU ); */
   wc.lpszClassName = RETROFLAT_WINDOW_CLASS;

   if( !RegisterClass( &wc ) ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Could not register window class!" );
      goto cleanup;
   }

   debug_printf( 1, "retroflat: creating window..." );
   
#     ifdef RETROFLAT_SCREENSAVER
   if( (HWND)0 != g_retroflat_state->parent ) {
      /* Shrink the child window into the parent. */
      debug_printf( 1, "retroflat: using window parent: %p",
         g_retroflat_state->parent );
      window_style = WS_CHILD;
      GetClientRect( g_retroflat_state->parent, &wr );
   } else if(
      RETROFLAT_FLAGS_SCREENSAVER ==
      (RETROFLAT_FLAGS_SCREENSAVER & g_retroflat_state->retroflat_flags)
   ) {
      /* Make window fullscreen and on top. */
      window_style_ex = WS_EX_TOPMOST;
      window_style = WS_POPUP | WS_VISIBLE;
      /* X/Y are hardcoded to zero below, so just get desired window size. */
      wr.left = 0;
      wr.top = 0;
      wr.right = GetSystemMetrics( SM_CXSCREEN );
      wr.bottom = GetSystemMetrics( SM_CYSCREEN );
   } else {
#     endif /* RETROFLAT_SCREENSAVER */
#     ifndef RETROFLAT_API_WINCE
   /* Open in a centered window. */
   ww = wr.right - wr.left;
   wh = wr.bottom - wr.top;
   if( 0 == args->screen_x ) {
      args->screen_x = (GetSystemMetrics( SM_CXSCREEN ) / 2) - (ww / 2);
   }
   if( 0 == args->screen_y ) {
      args->screen_y = (GetSystemMetrics( SM_CYSCREEN ) / 2) - (wh / 2);
   }
#     endif /* !RETROFLAT_API_WINCE */
#     ifdef RETROFLAT_SCREENSAVER
   }
#     endif /* RETROFLAT_SCREENSAVER */

   g_retroflat_state->window = CreateWindowEx(
      window_style_ex, RETROFLAT_WINDOW_CLASS, args->title,
      window_style,
#     ifdef RETROFLAT_API_WINCE
      0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
#     else
      args->screen_x, args->screen_y, ww, wh,
#     endif /* RETROFLAT_API_WINCE */
#     ifdef RETROFLAT_SCREENSAVER
      g_retroflat_state->parent
#     else
      0
#     endif /* RETROFLAT_SCREENSAVER */
      , 0, g_retroflat_instance, 0
   );

#     ifdef RETROFLAT_API_WINCE
   /* Force screen size. */
   GetClientRect( g_retroflat_state->window, &wr );
   g_retroflat_state->screen_w = wr.right - wr.left;
   g_retroflat_state->screen_h = wr.bottom - wr.top;
#     endif /* RETROFLAT_API_WINCE */

   if( !g_retroflat_state->window ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Could not create window!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

   maug_cleanup_if_null_alloc( HWND, g_retroflat_state->window );

#ifndef RETROFLAT_OPENGL
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRSET )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PNSET )
#endif /* !RETROFLAT_OPENGL */

   ShowWindow( g_retroflat_state->window, g_retroflat_cmd_show );

#  elif defined( RETROFLAT_API_LIBNDS )

   /* == Nintendo DS == */

   /* Setup color constants. */
#  define RETROFLAT_COLOR_TABLE_NDS_RGBS_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->palette[idx] = ARGB16( 1, r, g, b );
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_NDS_RGBS_INIT )

   /* Force screen size. */
   args->screen_w = 256;
   args->screen_h = 192;

   powerOn( POWER_ALL );

#     ifdef RETROFLAT_OPENGL

   debug_printf( 3, "setting up GL subsystem..." );

   videoSetMode( MODE_0_3D );

   vramSetBankA( VRAM_A_TEXTURE );

   glInit();
   
   /* TODO: Setup NDS 3D engine! */

#     else
   videoSetMode( MODE_5_2D );
	videoSetModeSub( MODE_0_2D );

   /* Setup the upper screen for background and sprites. */
	vramSetBankA( VRAM_A_MAIN_BG );
	vramSetBankB( VRAM_B_MAIN_SPRITE );

   /* Setup the lower screen for background and sprites. */
	vramSetBankC( VRAM_C_MAIN_BG );
	vramSetBankD( VRAM_D_SUB_SPRITE );

   bgExtPaletteEnable();

   /* Setup the background engine. */

   /* Put map at base 2, but stow tiles up after the bitmap BG at base 7. */
   g_retroflat_state->bg_id =
      bgInit( 0, BgType_Text8bpp, BgSize_T_256x256, 2, 7 );
   dmaFillWords( 0, g_retroflat_state->bg_tiles,
      sizeof( g_retroflat_state->bg_tiles ) );
   bgSetPriority( g_retroflat_state->bg_id, 2 );

   /* Put map at base 3, and tiles at base 0. */
   g_retroflat_state->window_id =
      bgInit( 1, BgType_Text8bpp, BgSize_T_256x256, 3, 0 );
   dmaFillWords( 0, g_retroflat_state->window_tiles,
      sizeof( g_retroflat_state->window_tiles ) );
   bgSetPriority( g_retroflat_state->window_id, 1 );

   /* Put bitmap BG at base 1, leaving map-addressable space at base 0. */
   g_retroflat_state->px_id = bgInit( 2, BgType_Bmp16, BgSize_B16_256x256, 1, 0 );
   bgSetPriority( g_retroflat_state->px_id, 0 );

   /* Setup the sprite engines. */
	oamInit( NDS_OAM_ACTIVE, SpriteMapping_1D_128, 0 );

   /* Allocate sprite frame memory. */
   for( i = 0 ; NDS_SPRITES_ACTIVE > i ; i++ ) {
      g_retroflat_state->sprite_frames[i] = oamAllocateGfx(
         NDS_OAM_ACTIVE, SpriteSize_16x16, SpriteColorFormat_256Color );
   }

#     endif /* RETROFLAT_OPENGL */

   /* Setup the timer. */
   TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024;
   TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;

#  elif defined( RETROFLAT_API_GLUT )

   /* == GLUT == */

#     define RETROFLAT_COLOR_TABLE_GLUT( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
         g_retroflat_state->palette[idx] = RETROGLU_COLOR_ ## name_u;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_GLUT )

   glutInit( &argc, argv );
   glut_init_flags = GLUT_DEPTH | GLUT_RGBA;
   if(
      RETROFLAT_FLAGS_UNLOCK_FPS != (RETROFLAT_FLAGS_UNLOCK_FPS & args->flags)
   ) {
      glut_init_flags |= GLUT_DOUBLE;
   }
   glutInitDisplayMode( glut_init_flags );
   if( 0 < args->screen_x || 0 < args->screen_y ) {
      glutInitWindowPosition( args->screen_x, args->screen_y );
   }
   /* TODO: Handle screen scaling? */
   glutInitWindowSize(
      g_retroflat_state->screen_w, g_retroflat_state->screen_h );
   glutCreateWindow( args->title );
   glutIdleFunc( retroflat_glut_idle );
   glutDisplayFunc( retroflat_glut_display );
   glutKeyboardFunc( retroflat_glut_key );

#  elif defined( RETROFLAT_API_PC_BIOS )

   /* == DOS PC_BIOS == */

   srand( time( NULL ) );

   debug_printf( 3, "memory available before growth: %u", _memavl() );
#     ifdef MAUG_DOS_MEM_L
   /* TODO: Should this check/init be in mmem.h instead? */
   _fheapgrow();
#  else
   _nheapgrow();
#  endif /* MAUG_DOS_MEM_L */
   debug_printf( 3, "memory available after growth: %u", _memavl() );

   /* Setup timer handler. */

   _disable();

   /* Backup original handler for later. */
   segread( &s );
   r.h.al = 0x08;
   r.h.ah = 0x35;
   int86x( 0x21, &r, &r, &s );
   g_retroflat_state->old_timer_interrupt =
      (retroflat_intfunc)MK_FP( s.es, r.x.bx );

   /* Install new interrupt handler. */
   g_ms = 0;
   r.h.al = 0x08;
   r.h.ah = 0x25;
   s.ds = FP_SEG( retroflat_timer_handler );
   r.x.dx = FP_OFF( retroflat_timer_handler );
   int86x( 0x21, &r, &r, &s );

   /* Set resolution of timer chip to 1ms. */
   outp( 0x43, 0x36 );
   outp( 0x40, (uint8_t)(RETROFLAT_DOS_TIMER_DIV & 0xff) );
   outp( 0x40, (uint8_t)((RETROFLAT_DOS_TIMER_DIV >> 8) & 0xff) );

   _enable();

   debug_printf( 3, "timers initialized..." );

   /* Setup graphics. */

   memset( &r, 0, sizeof( r ) );
   r.x.ax = 0x0f00; /* Service: Get video mode. */
	int86( 0x10, &r, &r ); /* Call video interrupt. */
   g_retroflat_state->old_video_mode = r.h.al;
   debug_printf( 2, "saved old video mode: 0x%02x",
      g_retroflat_state->old_video_mode );

   /* TODO: Put all screen mode dimensions in a LUT. */
   g_retroflat_state->screen_mode = args->screen_mode;
   switch( args->screen_mode ) {
   case RETROFLAT_SCREEN_MODE_CGA:
      debug_printf( 3, "using CGA 320x200x4 colors" );
      g_retroflat_state->screen_v_w = 320;
      g_retroflat_state->screen_v_h = 200;
      g_retroflat_state->screen_w = 320;
      g_retroflat_state->screen_h = 200;
      g_retroflat_state->buffer.px = (uint8_t SEG_FAR*)0xB8000000L;
      g_retroflat_state->buffer.w = 320;
      g_retroflat_state->buffer.h = 200;
      break;

   case RETROFLAT_SCREEN_MODE_VGA:
      debug_printf( 3, "using VGA 320x200x16 colors" );
      g_retroflat_state->screen_v_w = 320;
      g_retroflat_state->screen_v_h = 200;
      g_retroflat_state->screen_w = 320;
      g_retroflat_state->screen_h = 200;
      g_retroflat_state->buffer.px = (uint8_t SEG_FAR*)0xA0000000L;
      g_retroflat_state->buffer.w = 320;
      g_retroflat_state->buffer.h = 200;
      g_retroflat_state->buffer.sz = 320 * 200;
      break;

   default:
      error_printf( "unsupported video mode: %d", args->screen_mode );
      retval = MERROR_GUI;
      goto cleanup;
   }

   memset( &r, 0, sizeof( r ) );
   r.h.al = args->screen_mode;
   int86( 0x10, &r, &r ); /* Call video interrupt. */

   debug_printf(
      3, "graphics initialized (mode 0x%02x)...", args->screen_mode );

   /* Initialize color table. */
#     define RETROFLAT_COLOR_TABLE_CGA_COLORS_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->cga_color_table[idx] = RETROFLAT_CGA_COLOR_ ## cgac;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CGA_COLORS_INIT )

   /* Initialize dither table. */
#     define RETROFLAT_COLOR_TABLE_CGA_DITHER_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->cga_dither_table[idx] = RETROFLAT_CGA_COLOR_ ## cgad;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CGA_DITHER_INIT )

#  else
#     pragma message( "warning: init not implemented" )
#  endif  /* RETROFLAT_API_ALLEGRO */

#  if defined( RETROFLAT_SOFT_SHAPES ) || defined( RETROFLAT_SOFT_LINES )
   retval = retrosoft_init();
   maug_cleanup_if_not_ok();
#  endif /* RETROFLAT_SOFT_SHAPES || RETROFLAT_SOFT_LINES */

#  if defined( RETROFLAT_OPENGL )
   retval = retrosoft_init();
   maug_cleanup_if_not_ok();
   retval = retroglu_init_glyph_tex();
   maug_cleanup_if_not_ok();
#  endif /* RETROFLAT_OPENGL */

#  ifdef RETROFLAT_VDP
#     if defined( RETROFLAT_OS_UNIX )
   g_retroflat_state->vdp_exe = dlopen(
#        ifdef RETROFLAT_API_SDL1
      "./rvdpsdl1.so",
#        elif defined( RETROFLAT_API_SDL2 )
      "./rvdpsdl2.so",
#        else
#           error "rvdp .so undefined!"
#        endif
      RTLD_LAZY );
#     elif defined( RETROFLAT_OS_WIN )
   g_retroflat_state->vdp_exe = LoadLibrary(
#        ifdef RETROFLAT_API_SDL1
      "./rvdpsdl1.dll"
#        elif defined( RETROFLAT_API_SDL2 )
      "./rvdpsdl2.dll"
#        elif defined( RETROFLAT_API_WIN32 )
      "./rvdpnt.dll"
#        else
#           error "rvdp .so undefined!"
#        endif
      );
#     else
#        error "dlopen undefined!"
#     endif /* RETROFLAT_OS_UNIX */

   if( !(g_retroflat_state->vdp_exe) ) {
      error_printf( "not loading VDP" );
      /* Skip creating the buffer or trying to run the init proc. */
      goto skip_vdp;
   }

   /* Create intermediary screen buffer. */
   debug_printf( 1, "creating VDP buffer, %d x %d",
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

#  ifndef RETROFLAT_NO_BLANK_INIT
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

#  if defined( RETROFLAT_API_PC_BIOS )
   union REGS r;
   struct SREGS s;
#  endif /* RETROFLAT_API_PC_BIOS */

   debug_printf( 1, "retroflat shutdown called..." );

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

#  if defined( RETROFLAT_SOFT_SHAPES ) || defined( RETROFLAT_SOFT_LINES ) || \
defined( RETROFLAT_OPENGL )
   debug_printf( 1, "calling retrosoft shutdown..." );
   retrosoft_shutdown();
#  endif /* RETROFLAT_SOFT_SHAPES */

#  ifdef RETROFLAT_OPENGL
   debug_printf( 1, "destroying GL glyphs..." );
   retroglu_destroy_glyph_tex();
#  endif /* RETROFLAT_OPENGL */

   /* === Platform-Specific Shutdown === */

#  if defined( RETROFLAT_OS_WASM )
   /* Do nothing, start the main loop later. */
   return;

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( RETROFLAT_ERROR_ENGINE != retval ) {
      clear_keybuf();
   }

   retroflat_destroy_bitmap( &(g_retroflat_state->buffer) );

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

#     ifndef RETROFLAT_API_SDL1
   SDL_DestroyWindow( g_retroflat_state->window );
#     endif /* !RETROFLAT_API_SDL1 */

   SDL_Quit();

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* TODO: Windows shutdown? */

   /* Stop frame timer if available. */
   if( NULL != g_retroflat_state->frame_iter ) {
      KillTimer( g_retroflat_state->window, RETROFLAT_WIN_FRAME_TIMER_ID );
   }

   /* Stop loop timer if available. */
   if( NULL != g_retroflat_state->loop_iter ) {
      KillTimer( g_retroflat_state->window, RETROFLAT_WIN_LOOP_TIMER_ID );
   }

   if( (HDC)NULL != g_retroflat_state->buffer.hdc_b ) {
      /* Return the default object into the HDC. */
      SelectObject(
         g_retroflat_state->buffer.hdc_b,
         g_retroflat_state->buffer.old_hbm_b );
      DeleteDC( g_retroflat_state->buffer.hdc_b );
      g_retroflat_state->buffer.hdc_b = (HDC)NULL;

      /* Destroy buffer bitmap! */
      retroflat_destroy_bitmap( &(g_retroflat_state->buffer) );
   }

#     ifndef RETROFLAT_OPENGL
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRRM )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PENRM )
#     endif /* !RETROFLAT_OPENGL */

#     ifdef RETROFLAT_WING
   if( (HMODULE)NULL != g_w.module ) {
      FreeLibrary( g_w.module );
   }
#     endif /* RETROFLAT_WING */

   if( (HDC)NULL != g_retroflat_state->hdc_win ) {
      ReleaseDC( g_retroflat_state->window, g_retroflat_state->hdc_win );
   }

#  elif defined( RETROFLAT_API_GLUT )

   /* TODO */

#  elif defined( RETROFLAT_API_PC_BIOS )

   if( 0 != g_retroflat_state->old_video_mode ) {
      /* Restore old video mode. */
      debug_printf( 3, "restoring video mode 0x%02x...",
         g_retroflat_state->old_video_mode );

      memset( &r, 0, sizeof( r ) );
      r.x.ax = g_retroflat_state->old_video_mode;
      int86( 0x10, &r, &r ); /* Call video interrupt. */
   }

   if( NULL != g_retroflat_state->old_timer_interrupt ) {
      /* Re-install original interrupt handler. */
      debug_printf( 3, "restoring timer interrupt..." );
      _disable();
      segread( &s );
      r.h.al = 0x08;
      r.h.ah = 0x25;
      s.ds = FP_SEG( g_retroflat_state->old_timer_interrupt );
      r.x.dx = FP_OFF( g_retroflat_state->old_timer_interrupt );
      int86x( 0x21, &r, &r, &s );
   }

   /* Reset timer chip resolution to 18.2...ms. */
   outp( 0x43,0x36 );
   outp( 0x40,0x00 );
   outp( 0x40,0x00 );

   _enable();

#  else
#     pragma message( "warning: shutdown not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 */

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

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Build the title. */
   va_start( vargs, format );
   memset( title, '\0', RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

#if defined( RETROFLAT_API_ALLEGRO )
   set_window_title( title );
#elif defined( RETROFLAT_API_SDL1 )
   SDL_WM_SetCaption( title, NULL );
#elif defined( RETROFLAT_API_SDL2 )
   SDL_SetWindowTitle( g_retroflat_state->window, title );
#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   SetWindowText( g_retroflat_state->window, title );
#elif defined( RETROFLAT_API_LIBNDS )
   /* Do nothing. */
#elif defined( RETROFLAT_API_GLUT )
   glutSetWindowTitle( title );
#else
#     pragma message( "warning: set title implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL */

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   return g_ms;

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   
   /* == SDL == */

   return SDL_GetTicks();

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   return timeGetTime();

#  elif defined( RETROFLAT_API_LIBNDS )

   /* == libNDS == */

   return ((TIMER1_DATA * (1 << 16)) + TIMER0_DATA) / 32;

#  elif defined( RETROFLAT_API_GLUT )

   /* == GLUT == */

   return glutGet( GLUT_ELAPSED_TIME );

#  elif defined( RETROFLAT_API_PC_BIOS )

   return /**((uint16_t far*)0x046c) >> 4;*/ g_ms;

#  else
#     pragma message( "warning: get_ms not implemented" )
#  endif /* RETROFLAT_API_* */
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

#  if defined( RETROFLAT_OPENGL )

   if(
      NULL != bmp &&
      &(g_retroflat_state->buffer) != bmp &&
      (MAUG_MHANDLE)NULL != bmp->tex.bytes_h
   ) {
      bmp->flags |= RETROFLAT_FLAGS_LOCK;
      maug_mlock( bmp->tex.bytes_h, bmp->tex.bytes );
   }

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( NULL != bmp ) {
      /* Normal bitmaps don't need to be locked in allegro. */
      goto cleanup;
   }

   /* Lock screen for drawing if bmp is NULL. */

#     ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
   show_mouse( NULL ); /* Disable mouse before drawing. */
#     endif
   acquire_screen();

cleanup:

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   /* SDL locking semantics are the opposite of every other platform. See
    * retroflat_px_lock() for a proxy to SDL_LockSurface().
    */

   if( NULL == bmp || &(g_retroflat_state->buffer) == bmp ) {
      /* Special case: Attempting to lock the screen. */
      bmp = &(g_retroflat_state->buffer);

      if(
         RETROFLAT_FLAGS_SCREEN_LOCK !=
         (RETROFLAT_FLAGS_SCREEN_LOCK & bmp->flags)
      ) {
         /* Do a perfunctory "screen lock" since programs are supposed to
          * lock the screen before doing any drawing.
          */
         bmp->flags |= RETROFLAT_FLAGS_SCREEN_LOCK;

      } else {
         /* We actually want to lock the buffer for pixel manipulation. */
         assert( 0 == (RETROFLAT_FLAGS_LOCK & bmp->flags) );
         bmp->flags |= RETROFLAT_FLAGS_LOCK;
      }

   } else {
      /* Locking a bitmap for pixel drawing. */
      assert( 0 == (RETROFLAT_FLAGS_LOCK & bmp->flags) );
      bmp->flags |= RETROFLAT_FLAGS_LOCK;
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if(
      NULL == bmp
#     ifdef RETROFLAT_VDP
      && NULL == g_retroflat_state->vdp_buffer
#     endif /* RETROFLAT_VDP */
   ) {

      /* Target is the screen buffer. */
      SDL_SetRenderTarget(
         g_retroflat_state->buffer.renderer,
         g_retroflat_state->buffer.texture );

      goto cleanup;

#     ifdef RETROFLAT_VDP
   } else if( NULL == bmp && NULL != g_retroflat_state->vdp_buffer ) {
      /* Lock the VDP buffer for drawing. */
      bmp = g_retroflat_state->vdp_buffer;
#     endif /* RETROFLAT_VDP */
   }

   assert( NULL == bmp->renderer );
   assert( NULL != bmp->surface );
   bmp->renderer = SDL_CreateSoftwareRenderer( bmp->surface );

cleanup:

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   if( NULL == bmp ) {
#     ifdef RETROFLAT_VDP
      if( NULL != g_retroflat_state->vdp_buffer ) {
         bmp = g_retroflat_state->vdp_buffer;
      } else {
#     endif

      /* TODO: Reconcile with VDP! */
      /* The HDC should be created already by WndProc. */
      assert( (HDC)NULL != g_retroflat_state->buffer.hdc_b );
      goto cleanup;

#     ifdef RETROFLAT_VDP
      }
#     endif /* RETROFLAT_VDP */
   }

   /* Sanity check. */
   assert( (HBITMAP)NULL != bmp->b );
   assert( (HDC)NULL == bmp->hdc_b );

   /* Create HDC for source bitmap compatible with the buffer. */
   bmp->hdc_b = CreateCompatibleDC( (HDC)NULL );
   maug_cleanup_if_null( HDC, bmp->hdc_b, RETROFLAT_ERROR_BITMAP );

   if( (HBITMAP)NULL != bmp->mask ) {
      /* Create HDC for source mask compatible with the buffer. */
      bmp->hdc_mask = CreateCompatibleDC( (HDC)NULL );
      maug_cleanup_if_null( HDC, bmp->hdc_mask, RETROFLAT_ERROR_BITMAP );
   }

   /* Select bitmaps into their HDCs. */
   bmp->old_hbm_b = SelectObject( bmp->hdc_b, bmp->b );
   if( (HBITMAP)NULL != bmp->mask ) {
      bmp->old_hbm_mask = SelectObject( bmp->hdc_mask, bmp->mask );
   }

cleanup:

#  else
#     pragma message( "warning: draw lock not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

#  ifdef RETROFLAT_OPENGL
   if( NULL == bmp || &(g_retroflat_state->buffer) == bmp ) {
      /* Flush GL buffer and swap screen buffers. */
      glFlush();

#     if defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
      SDL_GL_SwapBuffers();
#     elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
      SwapBuffers( g_retroflat_state->hdc_win );
#     elif defined( RETROFLAT_API_GLUT )
      glutSwapBuffers();
#     endif
   } else if( retroflat_bitmap_locked( bmp ) ) {
      bmp->flags &= ~RETROFLAT_FLAGS_LOCK;
#ifndef RETROGLU_NO_TEXTURES
      assert( 0 < bmp->tex.id );
      assert( NULL != bmp->tex.bytes );

      /* Update stored texture if it exists. */
      glBindTexture( GL_TEXTURE_2D, bmp->tex.id );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, bmp->tex.w, bmp->tex.h, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, bmp->tex.bytes ); 
      glBindTexture( GL_TEXTURE_2D, 0 );
#endif /* !RETROGLU_NO_TEXTURES */

      /* Unlock texture bitmap. */
      maug_munlock( bmp->tex.bytes_h, bmp->tex.bytes );
   }

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( NULL != bmp ) {
      /* Don't need to lock bitmaps in Allegro. */
      goto cleanup;
   }

   /* Flip the buffer. */
   blit( g_retroflat_state->buffer.b, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

   /* Release the screen. */
   release_screen();
#     ifdef RETROFLAT_MOUSE
      /* XXX: Broken in DOS. */
   show_mouse( screen ); /* Enable mouse after drawing. */
#     endif /* RETROFLAT_MOUSE */
   vsync();

cleanup:
#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   if( NULL == bmp || &(g_retroflat_state->buffer) == bmp ) {
      /* Special case: Attempting to release the (real, non-VDP) screen. */
      bmp = &(g_retroflat_state->buffer);

      if(
         RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & bmp->flags)
      ) {
         /* The screen was locked for pixel manipulation. */
         bmp->flags &= ~RETROFLAT_FLAGS_LOCK;
         SDL_UnlockSurface( bmp->surface );

      } else {
         assert( 
            RETROFLAT_FLAGS_SCREEN_LOCK ==
            (RETROFLAT_FLAGS_SCREEN_LOCK & bmp->flags) );
         bmp->flags &= ~RETROFLAT_FLAGS_SCREEN_LOCK;

#     if defined( RETROFLAT_VDP )
         retroflat_vdp_call( "retroflat_vdp_flip" );
#     endif /* RETROFLAT_VDP */

         SDL_Flip( bmp->surface );
      }

   } else {
      /* Releasing a bitmap. */
      assert( RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & bmp->flags) );
      bmp->flags &= ~RETROFLAT_FLAGS_LOCK;
      SDL_UnlockSurface( bmp->surface );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if( 
      NULL == bmp
#     ifdef RETROFLAT_VDP
      && NULL == g_retroflat_state->vdp_buffer
#     endif /* RETROFLAT_VDP */
   ) {
      /* Flip the screen. */
      SDL_SetRenderTarget( g_retroflat_state->buffer.renderer, NULL );
      SDL_RenderCopyEx(
         g_retroflat_state->buffer.renderer,
         g_retroflat_state->buffer.texture, NULL, NULL, 0, NULL, 0 );
      SDL_RenderPresent( g_retroflat_state->buffer.renderer );

      goto cleanup;

#     ifdef RETROFLAT_VDP
   } else if( NULL == bmp && NULL != g_retroflat_state->vdp_buffer ) {
      bmp = g_retroflat_state->vdp_buffer;
#     endif /* RETROFLAT_VDP */
   }

   /* It's a bitmap. */

   /* Scrap the software renderer. */
   SDL_RenderPresent( bmp->renderer );
   SDL_DestroyRenderer( bmp->renderer );
   bmp->renderer = NULL;

   /* Scrap the old texture and recreate it from the updated surface. */
   /* The renderer should be a software renderer pointing to the surface,
      * created in retroflat_lock() above.
      */
   assert( NULL != bmp->texture );
   SDL_DestroyTexture( bmp->texture );
   bmp->texture = SDL_CreateTextureFromSurface(
      g_retroflat_state->buffer.renderer, bmp->surface );
   maug_cleanup_if_null(
      SDL_Texture*, bmp->texture, RETROFLAT_ERROR_BITMAP );

cleanup:
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   if( NULL == bmp ) {
      /* Trigger a screen refresh if this was a screen lock. */
      if( (HWND)NULL != g_retroflat_state->window ) {
         InvalidateRect( g_retroflat_state->window, 0, TRUE );
      }

#     ifdef RETROFLAT_VDP
      if( NULL != g_retroflat_state->vdp_buffer ) {
         bmp = g_retroflat_state->vdp_buffer;
      } else {
#     endif

      /* TODO: Reconcile with VDP! */
      goto cleanup;

#     ifdef RETROFLAT_VDP
      }
#     endif
   }

   /* Unlock the bitmap. */
   if( (HDC)NULL != bmp->hdc_b ) {
      SelectObject( bmp->hdc_b, bmp->old_hbm_b );
      DeleteDC( bmp->hdc_b );
      bmp->hdc_b = (HDC)NULL;
      bmp->old_hbm_b = (HBITMAP)NULL;
   }

   /* Unlock the mask. */
   if( (HDC)NULL != bmp->hdc_mask ) {
      SelectObject( bmp->hdc_mask, bmp->old_hbm_mask );
      DeleteDC( bmp->hdc_mask );
      bmp->hdc_mask = (HDC)NULL;
      bmp->old_hbm_mask = (HBITMAP)NULL;
   }

cleanup:

#  else
#     pragma message( "warning: draw release not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

/* === */

#if (defined( RETROFLAT_API_WIN16 ) || defined (RETROFLAT_API_WIN32 )) && \
!defined( RETROFLAT_OPENGL )

static int retroflat_bitmap_win_transparency(
   struct RETROFLAT_BITMAP* bmp_out, int16_t w, int16_t h  
) {
   int retval = RETROFLAT_OK;
   unsigned long txp_color = 0;

   /* Setup bitmap transparency mask. */
   bmp_out->mask = CreateBitmap( w, h, 1, 1, NULL );
   maug_cleanup_if_null( HBITMAP, bmp_out->mask, RETROFLAT_ERROR_BITMAP );

   retval = retroflat_draw_lock( bmp_out );
   maug_cleanup_if_not_ok();

   /* Convert the color key into bitmap format. */
   txp_color |= (RETROFLAT_TXP_B & 0xff);
   txp_color <<= 8;
   txp_color |= (RETROFLAT_TXP_G & 0xff);
   txp_color <<= 8;
   txp_color |= (RETROFLAT_TXP_R & 0xff);
   SetBkColor( bmp_out->hdc_b, txp_color );

   /* Create the mask from the color key. */
   BitBlt(
      bmp_out->hdc_mask, 0, 0, w, h, bmp_out->hdc_b, 0, 0, SRCCOPY );
   BitBlt(
      bmp_out->hdc_b, 0, 0, w, h, bmp_out->hdc_mask, 0, 0, SRCINVERT );

cleanup:

   if( RETROFLAT_OK == retval ) {
      retroflat_draw_release( bmp_out );
   }

   return retval;
}

#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* TODO: Use mfile API and create an mformat lib to handle different
 *       formats using perpix loader code.
 */
MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   int retval = MERROR_OK;
#  if defined( RETROFLAT_OPENGL )
   mfile_t bmp_file;
   struct MFMT_STRUCT_BMPFILE header_bmp;
   MAUG_MHANDLE bmp_palette_h = (MAUG_MHANDLE)NULL;
   uint32_t bmp_color = 0;
   uint32_t* bmp_palette = NULL;
   MAUG_MHANDLE bmp_px_h = (MAUG_MHANDLE)NULL;
   uint8_t* bmp_px = NULL;
   size_t bmp_px_sz = 0;
   uint8_t bmp_r = 0,
      bmp_g = 0,
      bmp_b = 0,
      bmp_color_idx = 0,
      bmp_flags = 0;
   size_t i = 0;
#  elif defined( RETROFLAT_API_SDL1 )
   SDL_Surface* tmp_surface = NULL;
#  elif defined( RETROFLAT_API_WIN16 ) || defined (RETROFLAT_API_WIN32 )
#     if defined( RETROFLAT_API_WIN16 )
   char* buf = NULL;
   FILE* bmp_file = NULL;
   long int i, x, y, w, h, colors, offset, sz, read;
#     elif defined( RETROFLAT_API_WIN32 )
   BITMAP bm;
#     endif /* RETROFLAT_API_WIN32 */
#  elif defined( RETROFLAT_API_PC_BIOS )
   mfile_t bmp_file;
   struct MFMT_STRUCT_BMPFILE header_bmp;
   MAUG_MHANDLE bmp_palette_h = (MAUG_MHANDLE)NULL;
   uint32_t bmp_color = 0;
   uint8_t bmp_flags = 0;
   size_t bmp_px_sz = 0;
   size_t i = 0;

#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   assert( NULL != bmp_out );

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   /* Build the path to the bitmap. */
   memset( filename_path, '\0', RETROFLAT_PATH_MAX + 1 );
   maug_snprintf( filename_path, RETROFLAT_PATH_MAX, "%s%c%s.%s",
      g_retroflat_state->assets_path, RETROFLAT_PATH_SEP,
      filename, RETROFLAT_BITMAP_EXT );

   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

#  ifdef RETROFLAT_OPENGL

   retval = mfile_open_read( filename_path, &bmp_file );
   maug_cleanup_if_not_ok();

   /* TODO: mfmt file detection system. */
   header_bmp.magic[0] = 'B';
   header_bmp.magic[1] = 'M';
   header_bmp.info.sz = 40;

   retval = mfmt_read_bmp_header(
      (struct MFMT_STRUCT*)&header_bmp,
      &bmp_file, 0, mfile_get_sz( &bmp_file ), &bmp_flags );
   maug_cleanup_if_not_ok();

   /* Setup bitmap options from header. */
   bmp_out->tex.w = header_bmp.info.width;
   bmp_out->tex.h = header_bmp.info.height;
   bmp_out->tex.sz = bmp_out->tex.w * bmp_out->tex.h * 4;
   bmp_out->tex.bpp = 24;

   /* Allocate a space for the bitmap palette. */
   bmp_palette_h = maug_malloc( 4, header_bmp.info.palette_ncolors );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_palette_h );

   maug_mlock( bmp_palette_h, bmp_palette );
   maug_cleanup_if_null_alloc( uint32_t*, bmp_palette );

   retval = mfmt_read_bmp_palette( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_palette, 4 * header_bmp.info.palette_ncolors,
      &bmp_file, 54 /* TODO */, mfile_get_sz( &bmp_file ) - 54, bmp_flags );
   maug_cleanup_if_not_ok();

   /* Allocate a space for the bitmap pixels. */
   bmp_px_sz = header_bmp.info.width * header_bmp.info.height;
   bmp_px_h = maug_malloc( 1, bmp_px_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_px_h );

   maug_mlock( bmp_px_h, bmp_px );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_px );

   retval = mfmt_read_bmp_px( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_px, bmp_px_sz,
      &bmp_file, header_bmp.px_offset,
      mfile_get_sz( &bmp_file ) - header_bmp.px_offset, bmp_flags );
   maug_cleanup_if_not_ok();

   /* Allocate buffer for unpacking. */
   debug_printf( 0, "creating bitmap: " SIZE_T_FMT " x " SIZE_T_FMT,
      bmp_out->tex.w, bmp_out->tex.h );
   bmp_out->tex.bytes_h = maug_malloc( bmp_out->tex.w * bmp_out->tex.h, 4 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_out->tex.bytes_h );

   maug_mlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_out->tex.bytes );

   /* Unpack palletized bitmap into BGRA with color key. */
   for( i = 0 ; bmp_px_sz > i ; i++ ) {
      /* Grab the color from the palette by index. */
      bmp_color_idx = bmp_px[bmp_px_sz - i - 1]; /* Reverse image. */
      if( bmp_color_idx >= header_bmp.info.palette_ncolors ) {
         error_printf(
            "invalid color at px " SIZE_T_FMT ": " UPRINTF_X32_FMT,
            bmp_px_sz - i - 1, bmp_color_idx );
         continue;
      }
      bmp_color = bmp_palette[bmp_color_idx];
      bmp_r = (bmp_color >> 16) & 0xff;
      bmp_g = (bmp_color >> 8) & 0xff;
      bmp_b = bmp_color & 0xff;

      bmp_out->tex.bytes[i * 4] = bmp_r;
      bmp_out->tex.bytes[(i * 4) + 1] = bmp_g;
      bmp_out->tex.bytes[(i * 4) + 2] = bmp_b;
      if(
         RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) &&
         RETROFLAT_TXP_R == bmp_r &&
         RETROFLAT_TXP_G == bmp_g &&
         RETROFLAT_TXP_B == bmp_b
      ) {
         /* Transparent pixel found. */
         bmp_out->tex.bytes[(i * 4) + 3] = 0x00;
      } else {
         bmp_out->tex.bytes[(i * 4) + 3] = 0xff;
      }
   }

#ifndef RETROGLU_NO_TEXTURES
   glGenTextures( 1, (GLuint*)&(bmp_out->tex.id) );
   glBindTexture( GL_TEXTURE_2D, bmp_out->tex.id );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, bmp_out->tex.w, bmp_out->tex.h, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, bmp_out->tex.bytes ); 
   glBindTexture( GL_TEXTURE_2D, 0 );
#endif /* !RETROGLU_NO_TEXTURES */

cleanup:

   if( NULL != bmp_out->tex.bytes ) {
      maug_munlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   }

   if( NULL != bmp_px ) {
      maug_munlock( bmp_px_h, bmp_px );
   }

   if( NULL != bmp_px_h ) {
      maug_mfree( bmp_px_h );
   }

   if( NULL != bmp_palette ) {
      maug_munlock( bmp_palette_h, bmp_palette );
   }

   if( NULL != bmp_palette_h ) {
      maug_mfree( bmp_palette_h );
   }

   mfile_close( &bmp_file );

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   bmp_out->b = load_bitmap( filename_path, NULL );

   /* TODO: maug_cleanup_if_null()? */
   if( NULL == bmp_out->b ) {
      allegro_message( "unable to load %s", filename_path );
      retval = RETROFLAT_ERROR_BITMAP;
   }

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "loading bitmap: %s", filename_path );

   tmp_surface = SDL_LoadBMP( filename_path ); /* Free stream on close. */
   /* TODO: maug_cleanup_if_null()? */
   if( NULL == tmp_surface ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = 0;
      goto cleanup;
   }

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "loaded bitmap: %d x %d", tmp_surface->w, tmp_surface->h );

   bmp_out->surface = SDL_DisplayFormat( tmp_surface );
   if( NULL == bmp_out->surface ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL, "converted bitmap: %d x %d",
      bmp_out->surface->w, bmp_out->surface->h );

   SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
      SDL_MapRGB( bmp_out->surface->format,
         RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );

cleanup:

   if( NULL != tmp_surface ) {
      SDL_FreeSurface( tmp_surface );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "loading bitmap: %s", filename_path );

   bmp_out->renderer = NULL;
   
   bmp_out->surface = SDL_LoadBMP( filename_path );

   /* TODO: maug_cleanup_if_null()? */
   if( NULL == bmp_out->surface ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
         SDL_MapRGB( bmp_out->surface->format,
            RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );
   }

   bmp_out->texture = SDL_CreateTextureFromSurface(
      g_retroflat_state->buffer.renderer, bmp_out->surface );
   /* TODO: maug_cleanup_if_null()? */
   if( NULL == bmp_out->texture ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "SDL unable to create texture: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      if( NULL != bmp_out->surface ) {
         SDL_FreeSurface( bmp_out->surface );
         bmp_out->surface = NULL;
      }
      goto cleanup;
   }

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "successfully loaded bitmap: %s", filename_path );

cleanup:

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#     if defined( RETROFLAT_API_WIN16 )

   /* Win16 has a bunch of extra involved steps for getting a bitmap from
    * disk. These cause a crash in Win32.
    */

   /* == Win16 == */

   /* Load the bitmap file from disk. */
   bmp_file = fopen( filename_path, "rb" );
   maug_cleanup_if_null_file( bmp_file );
   fseek( bmp_file, 0, SEEK_END );
   sz = ftell( bmp_file );
   fseek( bmp_file, 0, SEEK_SET );

   buf = calloc( sz, 1 );
   maug_cleanup_if_null_alloc( char*, buf );

   read = fread( buf, 1, sz, bmp_file );
   assert( read == sz );

   offset = retroflat_bmp_int( unsigned long, buf, 10 );
   colors = retroflat_bmp_int( int, buf, 46 );

   /* Avoid a color overflow. */
   if(
      sizeof( BITMAPFILEHEADER ) +
      sizeof( BITMAPINFOHEADER ) +
      (colors * sizeof( RGBQUAD )) > sz
   ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error",
         "Attempted to load bitmap with too many colors!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

   memcpy( &(bmp_out->bmi),
      &(buf[sizeof( BITMAPFILEHEADER )]),
      /* SetDIBits needs the color palette! */
      /* TODO: Sync with state palette? */
      sizeof( BITMAPINFOHEADER ) + (colors * sizeof( RGBQUAD )) );

   /* This never gets the height right? */
   debug_printf( 1, "bitmap w: %08x, h: %08x, colors: %d",
      bmp_out->bmi.header.biWidth, bmp_out->bmi.header.biHeight, colors );

   assert( 0 < bmp_out->bmi.header.biWidth );
   assert( 0 < bmp_out->bmi.header.biHeight );
   assert( 0 == bmp_out->bmi.header.biWidth % 8 );
   assert( 0 == bmp_out->bmi.header.biHeight % 8 );

   bmp_out->b = CreateCompatibleBitmap( g_retroflat_state->hdc_win,
      bmp_out->bmi.header.biWidth, bmp_out->bmi.header.biHeight );
   maug_cleanup_if_null( HBITMAP, bmp_out->b, RETROFLAT_ERROR_BITMAP );

   /* Turn the bits into a bitmap. */
   SetDIBits( g_retroflat_state->hdc_win, bmp_out->b, 0,
      bmp_out->bmi.header.biHeight, &(buf[offset]),
      (BITMAPINFO*)&(bmp_out->bmi),
      DIB_RGB_COLORS );

   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      retval = retroflat_bitmap_win_transparency( bmp_out,
         bmp_out->bmi.header.biWidth, bmp_out->bmi.header.biHeight );
   }

#     else

   /* Win32 greatly simplifies the loading portion. */

   /* == Win32 == */

#        ifdef RETROFLAT_API_WINCE
   bmp_out->b = SHLoadDIBitmap( filename_path );
#        else
   bmp_out->b = LoadImage(
      NULL, filename_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
#        endif /* RETROFLAT_API_WINCE */
   maug_cleanup_if_null_msg(
      HBITMAP, bmp_out->b, MERROR_FILE, "failed to open FILE!" )

   GetObject( bmp_out->b, sizeof( BITMAP ), &bm );

   bmp_out->bmi.header.biSize = sizeof( BITMAPINFOHEADER );
   bmp_out->bmi.header.biCompression = BI_RGB;
   bmp_out->bmi.header.biWidth = bm.bmWidth;
   bmp_out->bmi.header.biHeight = bm.bmHeight;
   bmp_out->bmi.header.biPlanes = bm.bmPlanes;
   bmp_out->bmi.header.biBitCount = bm.bmBitsPixel;
   bmp_out->bmi.header.biSizeImage =
      bmp_out->bmi.header.biWidth *
      bmp_out->bmi.header.biHeight *
      (bm.bmBitsPixel / sizeof( uint8_t ));

   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      retval = retroflat_bitmap_win_transparency(
         bmp_out, bm.bmWidth, bm.bmHeight );
   }

#     endif /* RETROFLAT_API_WIN16 */

   /* The transparency portion is the same for Win32 and Win16. */

cleanup:

#     ifdef RETROFLAT_API_WIN16
   if( NULL != buf ) {
      free( buf );
   }

   if( NULL != bmp_file ) {
      fclose( bmp_file );
   }

#     endif /* RETROFLAT_API_WIN16 */

#  elif defined( RETROFLAT_API_PC_BIOS )

   /* TODO: When loading a bitmap, zero out the color key color so XOR 
    *       works? */

   assert( NULL == bmp_out->px );

   retval = mfile_open_read( filename_path, &bmp_file );
   maug_cleanup_if_not_ok();

   /* TODO: mfmt file detection system. */
   header_bmp.magic[0] = 'B';
   header_bmp.magic[1] = 'M';
   header_bmp.info.sz = 40;

   retval = mfmt_read_bmp_header(
      (struct MFMT_STRUCT*)&header_bmp,
      &bmp_file, 0, mfile_get_sz( &bmp_file ), &bmp_flags );
   maug_cleanup_if_not_ok();

   /* Setup bitmap options from header. */
   bmp_out->w = header_bmp.info.width;
   bmp_out->h = header_bmp.info.height;

#if 0
   retval = mfmt_read_bmp_palette( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_palette, 4 * header_bmp.info.palette_ncolors,
      &bmp_file, 54 /* TODO */, mfile_get_sz( &bmp_file ) - 54, bmp_flags );
   maug_cleanup_if_not_ok();
#endif

   bmp_out->flags = flags;

   /* Allocate a space for the bitmap pixels. */
   bmp_out->sz = header_bmp.info.width * header_bmp.info.height;
   /* We're on PC BIOS... we don't need to lock pointers in this
    * platform-specific code!
    */
   bmp_out->px = _fcalloc( header_bmp.info.height, header_bmp.info.width );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_out->px );

   retval = mfmt_read_bmp_px( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_out->px, bmp_out->sz,
      &bmp_file, header_bmp.px_offset,
      mfile_get_sz( &bmp_file ) - header_bmp.px_offset, bmp_flags );
   maug_cleanup_if_not_ok();

   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      /* Create a transparency mask based on palette 0. */
      /* TODO: Use palette index from  global ifdef. */
      bmp_out->mask = _fcalloc(
         header_bmp.info.height, header_bmp.info.width );
      for( i = 0 ; bmp_out->sz > i ; i++ ) {
         if( 0 == bmp_out->px[i] ) {
            bmp_out->mask[i] = 0xff;
         } else {
            bmp_out->mask[i] = 0x00;
         }
      }
   }

   /* Allocate buffer for unpacking. */

cleanup:

#  else
#     pragma message( "warning: load bitmap not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_OPENGL )
#     ifndef RETROGLU_NO_TEXTURES
   GLenum error = GL_NO_ERROR;
#     endif /* !RETROGLU_NO_TEXTURES */
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int i = 0;
   PALETTEENTRY palette[RETROFLAT_BMP_COLORS_SZ_MAX];
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

#  if defined( RETROFLAT_OPENGL )

   if( w > 256 ) {
      error_printf( "warning! attempting to create texture with w > 256 ("
         SIZE_T_FMT "). This may not work on Win32!", w );
   }

   if( h > 256 ) {
      error_printf( "warning! attempting to create texture with h > 256 ("
         SIZE_T_FMT "). This may not work on Win32!", h );
   }

   bmp_out->tex.w = w;
   bmp_out->tex.h = h;
   /* TODO: Overflow checking. */
   debug_printf( 0, "creating bitmap: " SIZE_T_FMT " x " SIZE_T_FMT,
      bmp_out->tex.w, bmp_out->tex.h );
   bmp_out->tex.bytes_h = maug_malloc( bmp_out->tex.w * bmp_out->tex.h, 4 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_out->tex.bytes_h );

   maug_mlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_out->tex.bytes );

   /* TODO: Overflow checking. */
   maug_mzero(
      bmp_out->tex.bytes,
      bmp_out->tex.w * bmp_out->tex.h * sizeof( uint32_t ) );

#     ifndef RETROGLU_NO_TEXTURES
   glGenTextures( 1, (GLuint*)&(bmp_out->tex.id) );
   debug_printf( 0, "assigned bitmap texture: %u", bmp_out->tex.id );
   error = glGetError();
   if( GL_NO_ERROR != error ) {
      error_printf( "error generating texture: %u", error );
   }
#     endif /* !RETROGLU_NO_TEXTURES */

cleanup:
   if( NULL != bmp_out->tex.bytes ) {
      maug_munlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   }
#  elif defined( RETROFLAT_API_ALLEGRO )
   
   /* == Allegro == */

   bmp_out->b = create_bitmap( w, h );
   maug_cleanup_if_null( BITMAP*, bmp_out->b, RETROFLAT_ERROR_BITMAP );
   clear_bitmap( bmp_out->b );

cleanup:
#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   bmp_out->surface = SDL_CreateRGBSurface( 0, w, h,
      32, 0, 0, 0, 0 );
   maug_cleanup_if_null(
      SDL_Surface*, bmp_out->surface, RETROFLAT_ERROR_BITMAP );
   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
         SDL_MapRGB( bmp_out->surface->format,
            RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );
   }

cleanup:
#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   /* TODO: Handle opaque flag. */

   /* Create surface. */
   bmp_out->surface = SDL_CreateRGBSurface( 0, w, h,
      /* TODO: Are these masks right? */
      32, 0, 0, 0, 0 );
   maug_cleanup_if_null(
      SDL_Surface*, bmp_out->surface, RETROFLAT_ERROR_BITMAP );
   SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
      SDL_MapRGB( bmp_out->surface->format,
         RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );

   /* Convert new surface to texture. */
   bmp_out->texture = SDL_CreateTextureFromSurface(
      g_retroflat_state->buffer.renderer, bmp_out->surface );
   maug_cleanup_if_null(
      SDL_Texture*, bmp_out->texture, RETROFLAT_ERROR_BITMAP );
      
cleanup:
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16 / Win32 == */

   /* TODO: Handle opaque flag. */
   bmp_out->mask = (HBITMAP)NULL;

#     ifdef RETROFLAT_WING
   /* Put this first because WinGRecommendDIBFormat sets some header props. */
   if(
      RETROFLAT_FLAGS_SCREEN_BUFFER == 
         (RETROFLAT_FLAGS_SCREEN_BUFFER & flags) &&
      (WinGCreateDC_t)NULL != g_w.WinGCreateDC &&
      (WinGRecommendDIBFormat_t)NULL != g_w.WinGRecommendDIBFormat
   ) {
      bmp_out->hdc_b = g_w.WinGCreateDC();

      if( !g_w.WinGRecommendDIBFormat( (BITMAPINFO far*)&(bmp_out->bmi) ) ) {
         retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
            "Error", "Could not determine recommended format!" );
         retval = RETROFLAT_ERROR_GRAPHICS;
         goto cleanup;
      }
   }
#     endif /* RETROFLAT_WING */

   debug_printf( 0, "creating bitmap..." );

   bmp_out->bmi.header.biSize = sizeof( BITMAPINFOHEADER );
   bmp_out->bmi.header.biPlanes = 1;
   bmp_out->bmi.header.biCompression = BI_RGB;
   bmp_out->bmi.header.biWidth = w;
#     ifdef RETROFLAT_WING
   bmp_out->bmi.header.biHeight *= h;
#     else
   bmp_out->bmi.header.biHeight = h;
#     endif /* RETROFLAT_WING */
   bmp_out->bmi.header.biBitCount = 32;
   bmp_out->bmi.header.biSizeImage = w * h * 4;

   GetSystemPaletteEntries(
      g_retroflat_state->hdc_win, 0, RETROFLAT_BMP_COLORS_SZ_MAX, palette );
   for( i = 0 ; RETROFLAT_BMP_COLORS_SZ_MAX > i ; i++ ) {
      bmp_out->bmi.colors[i].rgbRed = palette[i].peRed;
      bmp_out->bmi.colors[i].rgbGreen = palette[i].peGreen;
      bmp_out->bmi.colors[i].rgbBlue = palette[i].peBlue;
      bmp_out->bmi.colors[i].rgbReserved = 0;
   }

#     ifdef RETROFLAT_WING
   /* Now try to create the WinG bitmap using the header we've built. */
   if(
      RETROFLAT_FLAGS_SCREEN_BUFFER == 
         (RETROFLAT_FLAGS_SCREEN_BUFFER & flags) &&
      (WinGCreateBitmap_t)NULL != g_w.WinGCreateBitmap
   ) {
      /* Setup an optimal WinG hardware screen buffer bitmap. */
      debug_printf( 1, "creating WinG-backed bitmap..." );

      bmp_out->flags |= RETROFLAT_FLAGS_SCREEN_BUFFER;
      bmp_out->b = g_w.WinGCreateBitmap(
         bmp_out->hdc_b,
         (BITMAPINFO far*)(&bmp_out->bmi),
         (void far*)&(bmp_out->bits) );

      debug_printf( 1, "WinG bitmap bits: %p", bmp_out->bits );

   } else {
#     endif /* RETROFLAT_WING */

   bmp_out->b = CreateCompatibleBitmap( g_retroflat_state->hdc_win, w, h );
   maug_cleanup_if_null( HBITMAP, bmp_out->b, RETROFLAT_ERROR_BITMAP );

   if(
      RETROFLAT_FLAGS_SCREEN_BUFFER == (RETROFLAT_FLAGS_SCREEN_BUFFER & flags)
   ) {
      debug_printf( 1, "creating screen device context..." );
      bmp_out->hdc_b = CreateCompatibleDC( g_retroflat_state->hdc_win );
      bmp_out->old_hbm_b = SelectObject( bmp_out->hdc_b, bmp_out->b );
   }

#     ifdef RETROFLAT_WING
   }
#     endif /* RETROFLAT_WING */

cleanup:

#  elif defined( RETROFLAT_API_PC_BIOS )

   assert( NULL == bmp_out->px );

   bmp_out->w = w;
   bmp_out->h = h;
   bmp_out->flags = flags;

   /* Allocate a space for the bitmap pixels. */
   bmp_out->sz = w * h;
   /* We're on PC BIOS... we don't need to lock pointers in this
    * platform-specific code!
    */
   bmp_out->px = _fcalloc( w, h );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_out->px );

cleanup:
#  else
#     pragma message( "warning: create bitmap not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

#  if defined( RETROFLAT_OPENGL )

   if( NULL != bmp->tex.bytes_h ) {
      if( NULL != bmp->tex.bytes ) {
         maug_munlock( bmp->tex.bytes_h, bmp->tex.bytes );
      }
      
      maug_mfree( bmp->tex.bytes_h );
   }

   if( 0 < bmp->tex.id ) {
      debug_printf( 0, "destroying bitmap texture: %u", bmp->tex.id );
      glDeleteTextures( 1, (GLuint*)&(bmp->tex.id) );
   }

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( NULL == bmp->b ) {
      return;
   }

   destroy_bitmap( bmp->b );
   bmp->b = NULL;

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   assert( NULL != bmp );
   assert( NULL != bmp->surface );

#     ifndef RETROFLAT_API_SDL1
   assert( NULL != bmp->texture );

   SDL_DestroyTexture( bmp->texture );
   bmp->texture = NULL;
#     endif /* !RETROFLAT_API_SDL1 */

   SDL_FreeSurface( bmp->surface );
   bmp->surface = NULL;

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16 == */

   if( NULL != bmp->old_hbm_b ) {
      SelectObject( bmp->hdc_b, bmp->old_hbm_b );
      bmp->old_hbm_b = (HBITMAP)NULL;
      bmp->old_hbm_b = (HBITMAP)NULL;
   }

   if( (HBITMAP)NULL != bmp->b ) {
      DeleteObject( bmp->b );
      bmp->b = (HBITMAP)NULL;
   }

   if( (HBITMAP)NULL != bmp->mask ) {
      DeleteObject( bmp->mask );
      bmp->mask = (HBITMAP)NULL;
   }

#  elif defined( RETROFLAT_API_PC_BIOS )

   if( NULL != bmp->px ) {
      _ffree( bmp->px );
      bmp->px = NULL;
   }

   if( NULL != bmp->mask ) {
      _ffree( bmp->mask );
      bmp->mask = NULL;
   }

#  else
#     pragma message( "warning: destroy bitmap not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int16_t w, int16_t h
) {
#  if defined( RETROFLAT_OPENGL )
   int y_iter = 0;
#  elif defined( RETROFLAT_API_SDL1 ) && !defined( RETROFLAT_OPENGL )
   MERROR_RETVAL retval = MERROR_OK;
   SDL_Rect src_rect;
   SDL_Rect dest_rect;
#  elif defined( RETROFLAT_API_SDL2 )
   MERROR_RETVAL retval = MERROR_OK;
   SDL_Rect src_rect = { s_x, s_y, w, h };
   SDL_Rect dest_rect = { d_x, d_y, w, h };
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   MERROR_RETVAL retval = MERROR_OK;
   int locked_src_internal = 0;
#  elif defined( RETROFLAT_API_PC_BIOS )
   int16_t y_iter = 0,
      x_iter = 0;
   uint16_t target_line_offset = 0;
   int16_t src_line_offset = 0;
   MERROR_RETVAL retval = MERROR_OK;
#  endif /* RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  ifndef RETROFLAT_OPENGL
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#  endif /* RETROFLAT_OPENGL */

   assert( NULL != src );

#  if defined( RETROFLAT_OPENGL )

   if( NULL == target || retroflat_screen_buffer() == target ) {
      /* TODO: Create ortho sprite on screen. */

   } else {
      /* Blit to texture. */

      assert( NULL != target->tex.bytes );

      /* TODO: Some kind of source-autolock? */
      assert( !retroflat_bitmap_locked( src ) );
      maug_mlock( src->tex.bytes_h, src->tex.bytes );
      for( y_iter = 0 ; h > y_iter ; y_iter++ ) {
         /* TODO: Handle transparency! */
         memcpy(
            &(target->tex.bytes[(((y_iter * target->tex.w) + d_x) * 4)]),
            &(src->tex.bytes[(((y_iter * src->tex.w) + s_x) * 4)]),
            w * 4 );
      }
      maug_munlock( src->tex.bytes_h, src->tex.bytes );

   }

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );
   assert( NULL != src->b );

   if(
      0 == s_x && 0 == s_y &&
      ((-1 == w && -1 == h ) || (src->b->w == w && src->b->h == h))
   ) {
      draw_sprite( target->b, src->b, d_x, d_y );
   } else {
      /* Handle partial blit. */
      blit( src->b, target->b, s_x, s_y, d_x, d_y, w, h );
   }

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

   src_rect.x = s_x;
   src_rect.y = s_y;
   src_rect.w = w;
   src_rect.h = h;
   dest_rect.x = d_x;
   dest_rect.y = d_y;
   dest_rect.w = w;
   dest_rect.h = h;

#     ifdef RETROFLAT_API_SDL1
   assert( 0 == src->autolock_refs );
   assert( 0 == target->autolock_refs );
   retval = 
      SDL_BlitSurface( src->surface, &src_rect, target->surface, &dest_rect );
   if( 0 != retval ) {
      error_printf( "could not blit surface: %s", SDL_GetError() );
   }
#     else

   assert( retroflat_bitmap_locked( target ) );
   retval = SDL_RenderCopy(
      target->renderer, src->texture, &src_rect, &dest_rect );
   if( 0 != retval ) {
      error_printf( "could not blit surface: %s", SDL_GetError() );
   }

#     endif /* !RETROFLAT_API_SDL1 */

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( (HBITMAP)NULL != target->b );
   assert( (HBITMAP)NULL != src->b );

   retroflat_internal_autolock_bitmap( src, locked_src_internal );
   assert( retroflat_bitmap_locked( target ) );

   if( (HBITMAP)NULL != src->mask ) {
      /* Use mask to blit transparency. */
      BitBlt(
         target->hdc_b, d_x, d_y, w, h, src->hdc_mask, s_x, s_y, SRCAND );
   }

   /* Do actual blit. */
   BitBlt(
      target->hdc_b, d_x, d_y, w, h, src->hdc_b, s_x, s_y, SRCPAINT );

cleanup:

   if( locked_src_internal ) {
      retroflat_draw_release( src );
   }

#  elif defined( RETROFLAT_API_PC_BIOS )

   if( NULL == target ) {
      target = &(g_retroflat_state->buffer);
   }

   switch( g_retroflat_state->screen_mode ) {
   case RETROFLAT_SCREEN_MODE_CGA:
      /* TODO: CGA bitmap blitting. */
      break;

   case RETROFLAT_SCREEN_MODE_VGA:
      for( y_iter = 0 ; h > y_iter ; y_iter++ ) {
         target_line_offset = ((d_y + y_iter) * (target->w)) + d_x;
         src_line_offset = ((s_y + y_iter) * src->w) + s_x;
         if( target->sz <= target_line_offset + w ) {
            continue;
         }
         /* Blit the line. */
         if(
            RETROFLAT_FLAGS_OPAQUE ==
            (RETROFLAT_FLAGS_OPAQUE & src->flags)
         ) {
            /* Copy line-by-line for speed. */
            _fmemcpy(
               &(target->px[target_line_offset]),
               &(src->px[src_line_offset]), w );
         } else {
            for( x_iter = 0 ; w > x_iter ; x_iter++ ) {
               /* AND with mask for transparency cutout. */
               target->px[target_line_offset + x_iter] &=
                  src->mask[src_line_offset + x_iter];
               /* Draw into cutout with OR. */
               target->px[target_line_offset + x_iter] |=
                  src->px[src_line_offset + x_iter];
            }
         }
      }
      break;
   }

#  else
#     pragma message( "warning: blit bitmap not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO */
   return;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_API_SDL1 )
   int offset = 0;
   uint8_t* px_1 = NULL;
   uint16_t* px_2 = NULL;
   uint32_t* px_4 = NULL;
   RETROFLAT_COLOR_DEF* color = &(g_retroflat_state->palette[color_idx]);
#  elif defined( RETROFLAT_API_SDL2 )
   RETROFLAT_COLOR_DEF* color = &(g_retroflat_state->palette[color_idx]);
#  elif defined( RETROFLAT_API_PC_BIOS )
   uint16_t screen_byte_offset = 0,
      screen_bit_offset = 0;
   uint8_t color = 0;
#  endif /* RETROFLAT_API_SDL1 */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      x < 0 || x >= retroflat_screen_w() ||
      y < 0 || y >= retroflat_screen_h()
   ) {
      return;
   }

#  if defined( RETROFLAT_OPENGL )

   assert( NULL != target->tex.bytes );
   assert( retroflat_bitmap_locked( target ) );

#     if !defined( RETROFLAT_NO_BOUNDSC )
   if( (size_t)x >= target->tex.w ) {
      return;
   }

   if( (size_t)y >= target->tex.h ) {
      return;
   }
#     endif /* !RETROFLAT_NO_BOUNDSC */

   /* Draw pixel colors from texture palette. */
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 0] =
      g_retroflat_state->tex_palette[color_idx][0];
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 1] =
      g_retroflat_state->tex_palette[color_idx][1];
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 2] =
      g_retroflat_state->tex_palette[color_idx][2];

   /* Set pixel as opaque. */
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 3] = 0xff;

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   putpixel( target->b, x, y, g_retroflat_state->palette[color_idx] );

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   retroflat_px_lock( target );

   assert( 0 < target->autolock_refs );

   offset = (y * target->surface->pitch) +
      (x * target->surface->format->BytesPerPixel);

   switch( target->surface->format->BytesPerPixel ) {
   case 4:
      px_4 = (uint32_t*)&(((uint8_t*)(target->surface->pixels))[offset]);
      *px_4 =
         SDL_MapRGB( target->surface->format, color->r, color->g, color->b );
      break;

   case 2:
      px_2 = (uint16_t*)&(((uint8_t*)(target->surface->pixels))[offset]);
      *px_2 =
         SDL_MapRGB( target->surface->format, color->r, color->g, color->b );
      break;

   case 1:
      px_1 = (uint8_t*)&(((uint8_t*)(target->surface->pixels))[offset]);
      *px_1 =
         SDL_MapRGB( target->surface->format, color->r, color->g, color->b );
      break;
   }

   retroflat_px_release( target );

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   assert( retroflat_bitmap_locked( target ) );

   SDL_SetRenderDrawColor(
      target->renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderDrawPoint( target->renderer, x, y );

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( retroflat_bitmap_locked( target ) );

#  ifdef RETROFLAT_WING
   if( NULL != target->bits ) {
      /* Modify target bits directly (faster) if available! */
      /* WinG bitmaps are 8-bit palettized, so use the index directly. */
      if( 0 > target->h ) {
         target->bits[((target->h - 1 - y) * target->tex.w) + x] =
            color_idx;
      } else {
         target->bits[(y * target->tex.w) + x] =
            color_idx;
      }
   } else {
      /* Use slow Windows GDI. */
      SetPixel( target->hdc_b, x, y,
         g_retroflat_state->palette[color_idx] );
   }
#  else
   SetPixel( target->hdc_b, x, y,
      g_retroflat_state->palette[color_idx] );
#  endif /* RETROFLAT_WING */

#  elif defined( RETROFLAT_API_LIBNDS )

   /* == Nintendo DS == */

   uint16_t* px_ptr = NULL;

   px_ptr = bgGetGfxPtr( g_retroflat_state->px_id );
   px_ptr[(y * 256) + x] = g_retroflat_state->palette[color_idx];

#  elif defined( RETROFLAT_API_PC_BIOS )

   /* == DOS PC_BIOS == */

   /* TODO: Determine if we're drawing on-screen or on a bitmap. */
   if( NULL == target ) {
      target = &(g_retroflat_state->buffer);
   }

   switch( g_retroflat_state->screen_mode ) {
   case RETROFLAT_SCREEN_MODE_CGA:
      /* Divide y by 2 since both planes are SCREEN_H / 2 high. */
      /* Divide result by 4 since it's 2 bits per pixel. */
      screen_byte_offset = (((y >> 1) * target->w) + x) >> 2;
      /* Shift the bits over by the remainder. */
      /* TODO: Factor out this modulo to shift/and. */
      screen_bit_offset = 6 - (((((y >> 1) * target->w) + x) % 4) << 1);

      /* Dither colors on odd/even squares. */
      if( (x & 0x01 && y & 0x01) || (!(x & 0x01) && !(y & 0x01)) ) {
         color = g_retroflat_state->cga_color_table[color_idx];
      } else {
         color = g_retroflat_state->cga_dither_table[color_idx];
      }

      if( target != &(g_retroflat_state->buffer) ) {
         /* TODO: Memory bitmap. */

      } else if( y & 0x01 ) {
         /* 0x2000 = difference between even/odd CGA planes. */
         g_retroflat_state->buffer.px[0x2000 + screen_byte_offset] &=
            /* 0x03 = 2-bit pixel mask. */
            ~(0x03 << screen_bit_offset);
         g_retroflat_state->buffer.px[0x2000 + screen_byte_offset] |= 
            ((color & 0x03) << screen_bit_offset);
      } else {
         /* 0x03 = 2-bit pixel mask. */
         g_retroflat_state->buffer.px[screen_byte_offset] &= 
            ~(0x03 << screen_bit_offset);
         g_retroflat_state->buffer.px[screen_byte_offset] |=
            /* 0x03 = 2-bit pixel mask. */
            ((color & 0x03) << screen_bit_offset);
      }
      break;

   case RETROFLAT_SCREEN_MODE_VGA:
      screen_byte_offset = ((y * target->w) + x);
      if( target->sz <= screen_byte_offset ) {
         break;
      }
      target->px[screen_byte_offset] = color_idx;
      break;
   }

#  else
#     pragma message( "warning: px not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
   float aspect_ratio = 0,
      screen_x = 0,
      screen_y = 0,
      screen_w = 0,
      screen_h = 0;
#  elif defined( RETROFLAT_SOFT_SHAPES )
#  elif defined( RETROFLAT_API_SDL2 )
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HBRUSH old_brush = (HBRUSH)NULL;
   HPEN old_pen = (HPEN)NULL;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

#  ifndef RETROFLAT_OPENGL
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#  endif /* !RETROFLAT_OPENGL */

#  if defined( RETROFLAT_OPENGL )

   if( NULL == target || retroflat_screen_buffer() == target ) {

      /* Push new overlay projection parms before we create a new overlay. */
      retroglu_push_overlay( x, y, screen_x, screen_y, aspect_ratio );
      retroglu_whf( w, h, screen_w, screen_h, aspect_ratio );

      /* Create the overlay poly with a solid color. */
      glBegin( GL_TRIANGLES );
      glColor3fv( g_retroflat_state->palette[color_idx] );
      glVertex3f( screen_x,            screen_y,            RETROFLAT_GL_Z );
      glVertex3f( screen_x,            screen_y - screen_h, RETROFLAT_GL_Z );
      glVertex3f( screen_x + screen_w, screen_y - screen_h, RETROFLAT_GL_Z );

      glVertex3f( screen_x + screen_w, screen_y - screen_h, RETROFLAT_GL_Z );
      glVertex3f( screen_x + screen_w, screen_y,            RETROFLAT_GL_Z );
      glVertex3f( screen_x,            screen_y,            RETROFLAT_GL_Z );
      glEnd();
      
      retroglu_pop_overlay();
   } else {
      /* Draw the rect onto the given 2D texture. */
      retrosoft_rect( target, color_idx, x, y, w, h, flags );
   }

#  elif defined( RETROFLAT_SOFT_SHAPES )

   retrosoft_rect( target, color_idx, x, y, w, h, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );
   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      rectfill( target->b, x, y, x + w, y + h,
         g_retroflat_state->palette[color_idx] );
   } else {
      rect( target->b, x, y, x + w, y + h,
         g_retroflat_state->palette[color_idx] );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   assert( retroflat_bitmap_locked( target ) );

   area.x = x;
   area.y = y;
   area.w = w;
   area.h = h;

   SDL_SetRenderDrawColor(
      target->renderer, color->r, color->g, color->b, 255 );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      SDL_RenderFillRect( target->renderer, &area );
   } else {
      SDL_RenderDrawRect( target->renderer, &area );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( (HBITMAP)NULL != target->b );

   assert( retroflat_bitmap_locked( target ) );

   retroflat_win_setup_brush( old_brush, target, color_idx, flags );
   retroflat_win_setup_pen( old_pen, target, color_idx, flags );

   Rectangle( target->hdc_b, x, y, x + w, y + h );

/* cleanup: */

   retroflat_win_cleanup_brush( old_brush, target )
   retroflat_win_cleanup_pen( old_pen, target )

#  else
#     pragma message( "warning: rect not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_SOFT_LINES )
#  elif defined( RETROFLAT_API_SDL2 )
   MERROR_RETVAL retval = MERROR_OK;
   RETROFLAT_COLOR_DEF color = g_retroflat_state->palette[color_idx];
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HPEN pen = (HPEN)NULL;
   HPEN old_pen = (HPEN)NULL;
   POINT points[2];
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

#  ifndef RETROFLAT_OPENGL
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#  endif /* !RETROFLAT_OPENGL */

#  if defined( RETROFLAT_OPENGL )

   if( NULL == target || retroflat_screen_buffer() == target ) {
      /* TODO: Draw line in ortho. */
   } else {
      retrosoft_line( target, color_idx, x1, y1, x2, y2, flags );
   }

#  elif defined( RETROFLAT_SOFT_LINES )

   retrosoft_line( target, color_idx, x1, y1, x2, y2, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );
   line( target->b, x1, y1, x2, y2, color_idx );

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   assert( retroflat_bitmap_locked( target ) );

   SDL_SetRenderDrawColor(
      target->renderer, color->r, color->g, color->b, 255 );
   SDL_RenderDrawLine( target->renderer, x1, y1, x2, y2 );
 
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );

   assert( retroflat_bitmap_locked( target ) );

   retroflat_win_setup_pen( old_pen, target, color_idx, flags );

   /* Create the line points. */
   points[0].x = x1;
   points[0].y = y1;
   points[1].x = x2;
   points[1].y = y2;

   Polyline( target->hdc_b, points, 2 );

/* cleanup: */

   if( (HPEN)NULL != pen ) {
      SelectObject( target->hdc_b, old_pen );
      DeleteObject( pen );
   }

#  elif defined( RETROFLAT_API_PC_BIOS )

   /* == DOS PC_BIOS == */

   /* TODO: Determine if we're drawing on-screen or on a bitmap. */

   switch( g_retroflat_state->screen_mode ) {
   case RETROFLAT_SCREEN_MODE_CGA:
      retrosoft_line( target, color_idx, x1, y1, x2, y2, flags );
      break;

   case RETROFLAT_SCREEN_MODE_VGA:
      /* TODO: Try accelerated 2D. */
      retrosoft_line( target, color_idx, x1, y1, x2, y2, flags );
      break;
   }

#  else
#     pragma message( "warning: line not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_SOFT_SHAPES )
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HPEN old_pen = (HPEN)NULL;
   HBRUSH old_brush = (HBRUSH)NULL;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( RETROFLAT_COLOR_NULL == color ) {
      return;
   }

#  ifndef RETROFLAT_OPENGL
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#  endif /* !RETROFLAT_OPENGL */

#  if defined( RETROFLAT_OPENGL )

   if( NULL == target || retroflat_screen_buffer() == target ) {
      /* TODO: Draw ellipse in ortho. */
   } else {
      retrosoft_ellipse( target, color, x, y, w, h, flags );
   }

#  elif defined( RETROFLAT_SOFT_SHAPES )

   retrosoft_ellipse( target, color, x, y, w, h, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      /* >> 1 performs better than / 2 on lousy old DOS compilers. */
      ellipsefill(
         target->b, x + (w >> 1), y + (h >> 1), w >> 1, h >> 1, color );
   } else {
      /* >> 1 performs better than / 2 on lousy old DOS compilers. */
      ellipse( target->b, x + (w >> 1), y + (h >> 1), w >> 1, h >> 1, color );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );
   assert( retroflat_bitmap_locked( target ) );

   retroflat_win_setup_brush( old_brush, target, color, flags );
   retroflat_win_setup_pen( old_pen, target, color, flags );

   Ellipse( target->hdc_b, x, y, x + w, y + h );

/* cleanup: */

   retroflat_win_cleanup_brush( old_brush, target )
   retroflat_win_cleanup_pen( old_pen, target )

#  else
#     pragma message( "warning: ellipse not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

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

/* === */

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#     define retroflat_win_create_font( flags, font_str ) \
         CreateFont( 10, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, \
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, \
            DEFAULT_QUALITY, DEFAULT_PITCH, \
            (NULL == font_str || '\0' == font_str[0] ? "Arial" : font_str) );

#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

void retroflat_string_sz(
   struct RETROFLAT_BITMAP* target, const char* str, size_t str_sz,
   const char* font_str, size_t* w_out, size_t* h_out, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_SOFT_SHAPES )
#  elif defined( RETROFLAT_API_ALLEGRO )
   FONT* font_data = NULL;
   int font_loaded = 0;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   SIZE sz;
   HFONT font;
   HFONT old_font;
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

#  if defined( RETROFLAT_OPENGL )

   retrosoft_string_sz( target, str, str_sz, font_str, w_out, h_out, flags );

#  elif defined( RETROFLAT_SOFT_SHAPES )

   retrosoft_string_sz( target, str, str_sz, font_str, w_out, h_out, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( NULL == font_str || '\0' == font_str[0] ) {
      font_data = font;
   } else {
      /* TODO: Cache loaded fonts for later use. */
      font_data = load_font( font_str, NULL, NULL );
   }
   if( NULL == font_data ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Unable to load font: %s", font_str );
      goto cleanup;
   }

   *w_out = text_length( font_data, str );
   *h_out = text_height( font_data );
   
cleanup:

   if( font_loaded && NULL != font_data ) {
      destroy_font( font_data );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );
   assert( retroflat_bitmap_locked( target ) );

   font = retroflat_win_create_font( flags, font_str );
   old_font = SelectObject( target->hdc_b, font );

   GetTextExtentPoint( target->hdc_b, str, str_sz, &sz );
   *w_out = sz.cx;
   *h_out = sz.cy;

/* cleanup: */

   SelectObject( target->hdc_b, old_font );

#  else
#     pragma message( "warning: string sz not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_string(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   const char* str, int str_sz, const char* font_str, int16_t x_orig, int16_t y_orig,
   uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
   float aspect_ratio = 0,
      screen_x = 0,
      screen_y = 0;
#  elif defined( RETROFLAT_SOFT_SHAPES )
#  elif defined( RETROFLAT_API_ALLEGRO )
   FONT* font_data = NULL;
   int font_loaded = 0;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   RECT rect;
   SIZE sz;
   HFONT font;
   HFONT old_font;
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( RETROFLAT_COLOR_NULL == color ) {
      return;
   }

#  if !defined( RETROFLAT_OPENGL )
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#  endif /* !RETROFLAT_OPENGL */

   if( 0 == str_sz ) {
      str_sz = strlen( str );
   }

#  if defined( RETROFLAT_OPENGL )

   if( NULL == target || retroflat_screen_buffer() == target ) {
      /* Push new overlay projection parms before we create a new overlay. */
      retroglu_push_overlay( x_orig, y_orig, screen_x, screen_y, aspect_ratio );

      retroglu_string(
         screen_x, screen_y, 0, 
         g_retroflat_state->palette[color], str, str_sz, font_str, flags );

      retroglu_pop_overlay();
   } else {
      /* Assume drawing surface is already configured inside a push_overlay()
       * call and draw to its texture. */
      retrosoft_string(
         target, color, str, str_sz, font_str, x_orig, y_orig, flags );
   }

#  elif defined( RETROFLAT_SOFT_SHAPES )

   retrosoft_string(
      target, color, str, str_sz, font_str, x_orig, y_orig, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( NULL == font_str || '\0' == font_str[0] ) {
      font_data = font;
   } else {
      /* TODO: Cache loaded fonts for later use. */
      font_data = load_font( font_str, NULL, NULL );
   }
   if( NULL == font_data ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Unable to load font: %s", font_str );
      goto cleanup;
   }

   textout_ex( target->b, font_data, str, x_orig, y_orig, color, -1 );

cleanup:
   if( font_loaded && NULL != font_data ) {
      destroy_font( font_data );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );

   assert( retroflat_bitmap_locked( target ) );

   /* DrawText will draw gibberish even if the string is null-terminated! */
   str_sz = strlen( str );

   memset( &sz, '\0', sizeof( SIZE ) );

   font = retroflat_win_create_font( flags, font_str );
   old_font = SelectObject( target->hdc_b, font );

   GetTextExtentPoint( target->hdc_b, str, str_sz, &sz );
   rect.left = x_orig;
   rect.top = y_orig;
   rect.right = (x_orig + sz.cx);
   rect.bottom = (y_orig + sz.cy);

   SetTextColor( target->hdc_b, g_retroflat_state->palette[color] );
   SetBkMode( target->hdc_b, TRANSPARENT );

   DrawText( target->hdc_b, str, str_sz, &rect, 0 );

/* cleanup: */

   SelectObject( target->hdc_b, old_font );

   SetBkMode( target->hdc_b, OPAQUE );
   SetTextColor( target->hdc_b,
      g_retroflat_state->palette[RETROFLAT_COLOR_BLACK] );

#  else
#     pragma message( "warning: string not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_get_palette( uint8_t idx, uint32_t* p_rgb ) {

#  ifdef RETROFLAT_OPENGL

   *p_rgb = 0;
   *p_rgb |= g_retroflat_state->tex_palette[idx][0] & 0xff;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][1] & 0xff) << 8;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][2] & 0xff) << 16;

#  elif defined( RETROFLAT_API_SDL2 )

   *p_rgb = 0;
   *p_rgb |= g_retroflat_state->palette[idx].b & 0xff;
   *p_rgb |= ((g_retroflat_state->palette[idx].g & 0xff) << 8);
   *p_rgb |= ((g_retroflat_state->palette[idx].r & 0xff) << 16);

#  else
#     pragma message( "warning: get palette not implemented" )
#  endif

}

/* === */

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 3,
      "setting texture palette #%u to " UPRINTF_X32_FMT "...",
      idx, rgb );

#  ifdef RETROFLAT_OPENGL

   g_retroflat_state->tex_palette[idx][0] = rgb & 0xff;
   g_retroflat_state->tex_palette[idx][1] = (rgb & 0xff00) >> 8;
   g_retroflat_state->tex_palette[idx][2] = (rgb & 0xff0000) >> 16;

#  elif defined( RETROFLAT_API_SDL2 )

   g_retroflat_state->palette[idx].b = rgb & 0xff;
   g_retroflat_state->palette[idx].g = (rgb & 0xff00) >> 8;
   g_retroflat_state->palette[idx].r = (rgb & 0xff0000) >> 16;

#  else
#     pragma message( "warning: set palette not implemented" )
#  endif

   return retval;
}

/* === */

void retroflat_set_proc_resize(
   retroflat_proc_resize_t on_resize_in, void* data_in
) {
   g_retroflat_state->on_resize = on_resize_in;
   g_retroflat_state->on_resize_data = data_in;
}

/* === */

void retroflat_resize_v() {
#  if defined( RETROFLAT_API_SDL2 )

   g_retroflat_state->screen_v_w = g_retroflat_state->screen_w;
   g_retroflat_state->screen_v_h = g_retroflat_state->screen_h;

   assert( NULL != g_retroflat_state->buffer.texture );
   SDL_DestroyTexture( g_retroflat_state->buffer.texture );

   /* Create the buffer texture. */
   g_retroflat_state->buffer.texture =
      SDL_CreateTexture( g_retroflat_state->buffer.renderer,
      SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      g_retroflat_state->screen_w, g_retroflat_state->screen_h );

#  endif /* RETROFLAT_API_SDL2 */
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
#  if defined( RETROFLAT_OS_DOS ) || defined( RETROFLAT_OS_DOS_REAL )
   union REGS inregs;
   union REGS outregs;
#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   int eres = 0;
   SDL_Event event;
#  endif /* RETROFLAT_API_ALLEGRO && RETROFLAT_OS_DOS */
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( g_retroflat_state->close_button ) {
      retroflat_quit( 0 );
      return 0;
   }

#     ifdef RETROFLAT_OS_DOS

   /* Poll the mouse. */
   inregs.w.ax = 3;
   int386( 0x33, &inregs, &outregs );

   if(
      1 == outregs.x.ebx && /* Left button clicked. */
      outregs.w.cx != g_retroflat_state->last_mouse_x &&
      outregs.w.dx != g_retroflat_state->last_mouse_y
   ) { 
      input->mouse_x = outregs.w.cx;
      input->mouse_y = outregs.w.dx;

      /* Prevent repeated clicks. */
      g_retroflat_state->last_mouse_x = input->mouse_x;
      g_retroflat_state->last_mouse_y = input->mouse_y;

      return RETROFLAT_MOUSE_B_LEFT;
   } else {
      g_retroflat_state->last_mouse_x = outregs.w.cx;
      g_retroflat_state->last_mouse_y = outregs.w.dx;
   }

#     else
   /* Allegro mouse is broken in DOS. */
   poll_mouse();
   if( mouse_b & 0x01 ) {
      input->mouse_x = mouse_x;
      input->mouse_y = mouse_y;
      return RETROFLAT_MOUSE_B_LEFT;
   } else if( mouse_b & 0x02 ) {
      input->mouse_x = mouse_x;
      input->mouse_y = mouse_y;
      return RETROFLAT_MOUSE_B_RIGHT;
   }
#     endif /* RETROFLAT_OS_DOS */

   poll_keyboard();
   if( keypressed() ) {
      /* TODO: ??? */
      if( KB_SHIFT_FLAG == (KB_SHIFT_FLAG & key_shifts) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_SHIFT;
      }

      if( KB_CTRL_FLAG == (KB_CTRL_FLAG & key_shifts) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_CTRL;
      }

      if( KB_ALT_FLAG == (KB_ALT_FLAG & key_shifts) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_ALT;
      }

      return (readkey() >> 8);
   }

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

   SDL_PollEvent( &event );

   switch( event.type ) {
   case SDL_QUIT:
      /* Handle SDL window close. */
      retroflat_quit( 0 );
      break;

   case SDL_KEYDOWN:
      key_out = event.key.keysym.sym;

      if(
         KMOD_RSHIFT == (KMOD_RSHIFT & event.key.keysym.mod) ||
         KMOD_LSHIFT == (KMOD_LSHIFT & event.key.keysym.mod) 
      ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_SHIFT;
      }

      if( KMOD_CTRL == (KMOD_CTRL & event.key.keysym.mod) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_CTRL;
      }

      if( KMOD_ALT == (KMOD_ALT & event.key.keysym.mod) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_ALT;
      }

      /* Flush key buffer to improve responsiveness. */
      if(
         RETROFLAT_FLAGS_KEY_REPEAT !=
         (RETROFLAT_FLAGS_KEY_REPEAT & g_retroflat_state->retroflat_flags)
      ) {
         while( (eres = SDL_PollEvent( &event )) );
      }
      break;

   case SDL_MOUSEBUTTONUP:
      /* Stop dragging. */
      g_retroflat_state->mouse_state = 0;
      break;

   case SDL_MOUSEBUTTONDOWN:

      /* Begin dragging. */

      input->mouse_x = event.button.x;  
      input->mouse_y = event.button.y;  

      /* Differentiate which button was clicked. */
      if( SDL_BUTTON_LEFT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_LEFT;
         g_retroflat_state->mouse_state = RETROFLAT_MOUSE_B_LEFT;
      } else if( SDL_BUTTON_RIGHT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_RIGHT;
         g_retroflat_state->mouse_state = RETROFLAT_MOUSE_B_RIGHT;
      }

      /* Flush key buffer to improve responsiveness. */
      /*while( (eres = SDL_PollEvent( &event )) );*/
      break;

#  if !defined( RETROFLAT_API_SDL1 )
   case SDL_WINDOWEVENT:
      switch( event.window.event ) {
      case SDL_WINDOWEVENT_RESIZED:
         retroflat_on_resize( event.window.data1, event.window.data2 );
         if( NULL != g_retroflat_state->on_resize ) {
            g_retroflat_state->on_resize(
               event.window.data1, event.window.data2,
               g_retroflat_state->on_resize_data );
         }
         break;
      }
      break;
#  endif /* !RETROFLAT_API_SDL1 */

   default:
      /* Check for mouse dragging if mouse was previously held down. */
      if( 0 != g_retroflat_state->mouse_state ) {
         /* Update coordinates and keep dragging. */
         SDL_GetMouseState( &(input->mouse_x), &(input->mouse_y) );
         key_out = g_retroflat_state->mouse_state;
      }
      break;
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   
   /* == Win16/Win32 == */

   if( g_retroflat_state->last_key ) {
      /* Return g_retroflat_state->last_key, which is set in WndProc when a keypress msg is
      * received.
      */
      key_out = g_retroflat_state->last_key;
      input->key_flags = g_retroflat_state->vk_mods;

      /* Reset pressed key. */
      g_retroflat_state->last_key = 0;

   } else if( g_retroflat_state->last_mouse ) {
      if( MK_LBUTTON == (MK_LBUTTON & g_retroflat_state->last_mouse) ) {
         input->mouse_x = g_retroflat_state->last_mouse_x;
         input->mouse_y = g_retroflat_state->last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_LEFT;
      } else if( MK_RBUTTON == (MK_RBUTTON & g_retroflat_state->last_mouse) ) {
         input->mouse_x = g_retroflat_state->last_mouse_x;
         input->mouse_y = g_retroflat_state->last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_RIGHT;
      }
      g_retroflat_state->last_mouse = 0;
      g_retroflat_state->last_mouse_x = 0;
      g_retroflat_state->last_mouse_y = 0;
   }

#     ifdef RETROFLAT_SCREENSAVER
   if( 
      (RETROFLAT_FLAGS_SCREENSAVER ==
      (RETROFLAT_FLAGS_SCREENSAVER & g_retroflat_state->retroflat_flags))
      && 0 != key_out
   ) {
      /* retroflat_quit( 0 ); */
   }
#     endif /* RETROFLAT_SCREENSAVER */

#  elif defined( RETROFLAT_API_GLUT )

   /* TODO: Implement RETROFLAT_MOD_SHIFT. */

   key_out = g_retroflat_state->retroflat_last_key;
   g_retroflat_state->retroflat_last_key = 0;

   /* TODO: Handle mouse. */

#  elif defined( RETROFLAT_API_PC_BIOS )

   /* TODO: Poll the mouse. */

   /* TODO: Implement RETROFLAT_MOD_SHIFT. */

   if( kbhit() ) {
      /* Poll the keyboard. */
      key_out = getch();
      debug_printf( 2, "key: 0x%02x", key_out );
      if( 0 == key_out ) {
         /* Special key was pressed that returns two scan codes. */
         key_out = getch();
         switch( key_out ) {
         case 0x48: key_out = RETROFLAT_KEY_UP; break;
         case 0x4b: key_out = RETROFLAT_KEY_LEFT; break;
         case 0x4d: key_out = RETROFLAT_KEY_RIGHT; break;
         case 0x50: key_out = RETROFLAT_KEY_DOWN; break;
         case 0x4f: key_out = RETROFLAT_KEY_HOME; break;
         case 0x47: key_out = RETROFLAT_KEY_END; break;
         case 0x51: key_out = RETROFLAT_KEY_PGDN; break;
         case 0x49: key_out = RETROFLAT_KEY_PGUP; break;
         case 0x53: key_out = RETROFLAT_KEY_DELETE; break;
         }
      }
      if(
         RETROFLAT_FLAGS_KEY_REPEAT !=
         (RETROFLAT_FLAGS_KEY_REPEAT & g_retroflat_state->retroflat_flags)
      ) {
         while( kbhit() ) {
            getch();
         }
      }
   }

#  else
#     pragma message( "warning: poll input not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   /* TODO: Handle NDS input! */

   return key_out;
}

/* === */

MERROR_RETVAL retroflat_config_open( RETROFLAT_CONFIG* config, uint8_t flags ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_CONFIG_USE_FILE )
   char flag_buffer[3] = { '\0', '\0', '\0' };

   if( RETROFLAT_CONFIG_FLAG_W == (RETROFLAT_CONFIG_FLAG_W & flags) ) {
      flag_buffer[0] = 'w';
   } else {
      flag_buffer[0] = 'r';
   }

   if( RETROFLAT_CONFIG_FLAG_BIN == (RETROFLAT_CONFIG_FLAG_BIN & flags) ) {
      flag_buffer[1] = 'b';
   }

   debug_printf( 1, "opening config file %s with mode [%s]...",
      g_retroflat_state->config_path, flag_buffer );

   *config = fopen( g_retroflat_state->config_path, flag_buffer );
   maug_cleanup_if_null( RETROFLAT_CONFIG, *config, MERROR_FILE );

cleanup:
#  elif defined( RETROFLAT_API_WIN16 )

   /* == Win16 (.ini file) == */

   /* TODO */

#  elif defined( RETROFLAT_API_WIN32 )

   /* == Win32 (Registry) == */

   char key_path[RETROFLAT_PATH_MAX + 1] = "SOFTWARE\\";

   /* TODO */
   strncat( key_path, "RetroFlat", RETROFLAT_PATH_MAX );

   /* TODO */
   if( ERROR_SUCCESS != RegOpenKey(
      HKEY_CURRENT_USER, 
      key_path,
      (HKEY*)config
   ) ) {
      retval = MERROR_FILE;
   }

#  else
#     pragma message( "warning: config close not implemented" )
#  endif

   return retval;
}

/* === */

void retroflat_config_close( RETROFLAT_CONFIG* config ) {

#  if defined( RETROFLAT_CONFIG_USE_FILE )

   debug_printf( 1, "closing config file..." );
   fclose( *config );
   *config = NULL;

#  elif defined( RETROFLAT_API_WIN16 )

   /* TODO */

#  elif defined( RETROFLAT_API_WIN32 )

   /* == Win32 (Registry) == */

   /* TODO */

#  else
#     pragma message( "warning: config close not implemented" )
#  endif

}

/* === */

#ifdef RETROFLAT_CONFIG_USE_FILE

/**
 * \param sect_match Pointer to size_t to hold strncmp() result.
 * \return Unmodified line if *any* section is found, with brackets stripped
 *         if sect_name is found.
 */
static char* retroflat_config_tok_sect(
   char* line, size_t line_sz, const char* sect_name, size_t* sect_match
) {

   /* Section check. */
   if( '[' == line[0] ) {
      *sect_match = strncmp( sect_name, &(line[1]), line_sz - 2 );
      if( 0 == *sect_match ) {
         /* TODO: Tokenize at closing bracket. */
         line[line_sz - 1] = '\0';
         debug_printf( 1, "found section: %s", &(line[1]) );
      }
      return line;
   }

   return NULL;
}  

#endif /* RETROFLAT_CONFIG_USE_FILE */

/* === */

size_t retroflat_config_write(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer, size_t buffer_sz_max
) {
   size_t retval = 0;


   return retval;
}

/* === */

size_t retroflat_config_read(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer_out, size_t buffer_out_sz_max,
   const void* default_out, size_t default_out_sz
) {
   size_t retval = 0;
#  if defined( RETROFLAT_CONFIG_USE_FILE )
   char line[RETROFLAT_CONFIG_LN_SZ_MAX + 1];
   char* line_val = NULL;
   size_t line_sz = 0;
   size_t sect_match = 1;
#  endif /* RETROFLAT_API_WIN32 */

#  if defined( RETROFLAT_CONFIG_USE_FILE )

   /* == SDL / Allegro == */

   while( fgets( line, RETROFLAT_CONFIG_LN_SZ_MAX, *config ) ) {
      /* Size check. */
      line_sz = strlen( line );
      if( 1 >= line_sz || RETROFLAT_CONFIG_LN_SZ_MAX <= line_sz ) {
         error_printf( "invalid line sz: " SIZE_T_FMT, line_sz );
         continue;
      }
      
      /* Strip off trailing newline. */
      if( '\n' == line[line_sz - 1] || '\r' == line[line_sz - 1] ) {
         debug_printf( 1, "stripping newline!" );
         line_sz--;
         line[line_sz] = '\0'; /* NULL goes after strlen() finishes! */
      }

      /* Section check. */
      if(
         retroflat_config_tok_sect( line, line_sz, sect_name, &sect_match )
      ) {
         continue;
      }
      
      /* Split up key/value pair. */
      line_val = strchr( line, '=' );
      if( NULL == line_val || line_val == line ) {
         error_printf( "invalid line: %s", line );
         continue;
      }

      /* Terminate key. */
      line_val[0] = '\0';
      line_val++;

      if( 0 == sect_match && 0 == strcmp( key_name, line ) ) {
         debug_printf( 1, "found %s: %s", line, line_val );

         switch( buffer_type ) {
         case RETROFLAT_BUFFER_INT:
            *((int*)buffer_out) = atoi( line_val );
            goto cleanup;

         case RETROFLAT_BUFFER_FLOAT:
            *((float*)buffer_out) = atof( line_val );
            goto cleanup;

         case RETROFLAT_BUFFER_STRING:
            strncpy( (char*)buffer_out, line_val, buffer_out_sz_max );
            goto cleanup;

         case RETROFLAT_BUFFER_BOOL:
            /* TODO */
            goto cleanup;
         }
      }
   }

cleanup:

   fseek( *config, 0, SEEK_SET );

#  elif defined( RETROFLAT_API_WIN16 )

   /* == Win16 (.ini file) == */

/* XXX
   retval = GetPrivateProfileString(
      sect_name, key_name, default_out, buffer_out, buffer_out_sz_max,
      g_retroflat_state->config_path ); */

#  elif defined( RETROFLAT_API_WIN32 )

   /* == Win32 (Registry) == */

   /* TODO */

#  else
#     pragma message( "warning: config read not implemented" )
#  endif

   return retval;
}

#elif !defined( RETROVDP_C ) /* End of RETROFLT_C */

#define RETROFLAT_COLOR_TABLE_CONSTS( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   extern MAUG_CONST RETROFLAT_COLOR RETROFLAT_COLOR_ ## name_u;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CONSTS )

extern MAUG_CONST char* SEG_MCONST gc_retroflat_color_names[];

   extern struct RETROFLAT_STATE* g_retroflat_state;
#     if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   extern HINSTANCE            g_retroflat_instance;
   extern int                  g_retroflat_cmd_show;
#     endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  include <uprintf.h>

#  if (defined( RETROFLAT_SOFT_SHAPES ) || defined( RETROFLAT_SOFT_LINES)) \
   && !defined( MAUG_NO_AUTO_C )
#     include <retrofp.h>
#     include <retrosft.h>
#  endif /* RETROFLAT_SOFT_SHAPES || RETROFLAT_SOFT_LINES */

#  if defined( RETROFLAT_OPENGL ) && !defined( MAUG_NO_AUTO_C )
#     include <retroglu.h>
#     include <retrofp.h>
#     include <retrosft.h>
#  endif /* RETROFLAT_OPENGL */

#endif /* RETROFLT_C */

#ifdef RETROFLAT_XPM
#include <retroxpm.h>
#endif /* RETROFLAT_XPM */

#ifdef RETROVDP_C

/* Declarations for VDP sources. */

#endif /* RETROVDP_C */

/*! \} */ /* maug_retroflt */

#endif /* RETROFLT_H */

