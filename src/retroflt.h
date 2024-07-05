
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

#ifndef RETROFLAT_KB_TRACE_LVL
#  define RETROFLAT_KB_TRACE_LVL    0
#endif /* !RETROFLAT_KB_TRACE_LVL */

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

/* TODO: Migrate all platform-specific parts below to retapid.h. */
#include <retapid.h>

#if defined( RETROFLAT_OS_WIN ) && !defined( MAUG_WINDOWS_H )
#  include <windows.h>
#  define MAUG_WINDOWS_H
#endif /* !MAUG_WINDOWS_H */

/* === API-Specific Includes and Defines === */

#if defined( RETROFLAT_API_ALLEGRO )

/* See retapid.h for Allegro. */

#elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

/* See retapid.h for SDL. */

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

/* See retapid.h for Win. */


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

/* See retapid.h for GLUT. */

#elif defined( RETROFLAT_API_PC_BIOS )

/* See retapid.h for DOS BIOS. */

#else
#  pragma message( "warning: not implemented" )

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

   struct RETROFLAT_PLATFORM platform;

#if defined( RETROFLAT_API_ALLEGRO )

#elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

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

#  elif defined( RETROFLAT_API_PC_BIOS )

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

#ifdef RETROFLT_C

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

/* TODO: Migrate all platform-specific parts below to retapid.h. */
#include <retapif.h>

#  if defined( RETROFLAT_VDP ) && defined( RETROFLAT_OS_UNIX )
#     include <dlfcn.h>
#  endif

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

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
   emscripten_set_main_loop_arg( frame_iter, data, 0, 0 );

   /* TODO: Replace this with standard loop that retapif.h can define a
    *       an optional replacement for.
    */
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
   g_retroflat_state->platform.loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;
   g_retroflat_state->platform.frame_iter = (retroflat_loop_iter)frame_iter;

   if( NULL != frame_iter ) {
      debug_printf( 3, "setting up frame timer %u every %d ms...",
         RETROFLAT_WIN_FRAME_TIMER_ID, (int)(1000 / RETROFLAT_FPS) );
      if( !SetTimer(
         g_retroflat_state->platform.window, RETROFLAT_WIN_FRAME_TIMER_ID,
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
         g_retroflat_state->platform.window,
         RETROFLAT_WIN_LOOP_TIMER_ID, 1, NULL )
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

   g_retroflat_state->platform.loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;
   g_retroflat_state->platform.frame_iter = (retroflat_loop_iter)frame_iter;
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

#ifndef RETROPLAT_PRESENT

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

#  elif defined( RETROFLAT_API_SDL2 )

#  elif (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_WIN )) || \
   (defined( RETROFLAT_API_GLUT) && defined( RETROFLAT_OS_WIN )) || \
   defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#  elif (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_UNIX )) || \
   (defined( RETROFLAT_API_GLUT) && defined( RETROFLAT_OS_UNIX ))

#  elif defined( RETROFLAT_API_PC_BIOS )

#  else
#     pragma message( "warning: not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   va_end( vargs );
}

#endif /* !RETROPLAT_PRESENT */

#  ifndef RETROFLAT_NO_CLI

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
      /* TODO: Maug replacement for C99 crutch. */
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

#  ifdef RETROFLAT_API_PC_BIOS

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

#endif /* !RETROFLAT_NO_CLI */

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

#  ifndef RETROFLAT_NO_CLI

   debug_printf( 1, "retroflat: parsing args..." );

   /* All platforms: add command-line args based on compile definitons. */

#     ifdef RETROSND_ARGS
	maug_add_arg( MAUG_CLI_SIGIL "rsd", MAUG_CLI_SIGIL_SZ + 4,
      "Select MIDI device", 0, (maug_cli_cb)retrosnd_cli_rsd,
         (maug_cli_cb)retrosnd_cli_rsd_def, args );
	maug_add_arg( MAUG_CLI_SIGIL "rsl", MAUG_CLI_SIGIL_SZ + 4,
      "List MIDI devices", 0, (maug_cli_cb)retrosnd_cli_rsl, NULL, args );
#     endif /* RETROSND_ARGS */

#     ifdef RETROFLAT_SCREENSAVER
	maug_add_arg( MAUG_CLI_SIGIL "p", MAUG_CLI_SIGIL_SZ + 2,
      "Preview screensaver", 0, (maug_cli_cb)retroflat_cli_p, NULL, args );
	maug_add_arg( MAUG_CLI_SIGIL "s", MAUG_CLI_SIGIL_SZ + 2,
      "Launch screensaver", 0, (maug_cli_cb)retroflat_cli_s, NULL, args );
#     endif /* RETROFLAT_SCREENSAVER */

#     ifdef RETROFLAT_API_PC_BIOS
   maug_add_arg( MAUG_CLI_SIGIL "rfm", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen mode.", 0,
      (maug_cli_cb)retroflat_cli_rfm,
      (maug_cli_cb)retroflat_cli_rfm_def, args );
#     elif !defined( RETROFLAT_NO_CLI_SZ )
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
#     endif /* !RETROFLAT_NO_CLI_SZ */

#     ifdef RETROFLAT_VDP
   maug_add_arg( MAUG_CLI_SIGIL "vdp", MAUG_CLI_SIGIL_SZ + 4,
      "Pass a string of args to the VDP.", 0,
      (maug_cli_cb)retroflat_cli_vdp, NULL, args );
#     endif /* RETROFLAT_VDP */

   maug_add_arg( MAUG_CLI_SIGIL "rfu", MAUG_CLI_SIGIL_SZ + 4,
      "Unlock FPS.", 0,
      (maug_cli_cb)retroflat_cli_u, (maug_cli_cb)retroflat_cli_u_def, args );

   /* Parse command line args. */
   retval = maug_parse_args( argc, argv );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

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
#     if !defined( RETROFLAT_OS_DOS ) || !defined( __WATCOMC__ )
   /* XXX: Broken in DOS on watcom. */
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
   g_retroflat_state->platform.window = SDL_CreateWindow( args->title,
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      args->screen_w, args->screen_h, RETROFLAT_WIN_FLAGS );
   maug_cleanup_if_null(
      SDL_Window*, g_retroflat_state->platform.window,
      RETROFLAT_ERROR_GRAPHICS );

   /* Create the main renderer. */
   g_retroflat_state->buffer.renderer = SDL_CreateRenderer(
      g_retroflat_state->platform.window, -1,
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
   SDL_SetWindowIcon( g_retroflat_state->platform.window, icon );
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
   if( (HWND)0 != g_retroflat_state->platform.parent ) {
      /* Shrink the child window into the parent. */
      debug_printf( 1, "retroflat: using window parent: %p",
         g_retroflat_state->platform.parent );
      window_style = WS_CHILD;
      GetClientRect( g_retroflat_state->platform.parent, &wr );
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

   g_retroflat_state->platform.window = CreateWindowEx(
      window_style_ex, RETROFLAT_WINDOW_CLASS, args->title,
      window_style,
#     ifdef RETROFLAT_API_WINCE
      0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
#     else
      args->screen_x, args->screen_y, ww, wh,
#     endif /* RETROFLAT_API_WINCE */
#     ifdef RETROFLAT_SCREENSAVER
      g_retroflat_state->platform.parent
#     else
      0
#     endif /* RETROFLAT_SCREENSAVER */
      , 0, g_retroflat_instance, 0
   );

#     ifdef RETROFLAT_API_WINCE
   /* Force screen size. */
   GetClientRect( g_retroflat_state->platform.window, &wr );
   g_retroflat_state->screen_w = wr.right - wr.left;
   g_retroflat_state->screen_h = wr.bottom - wr.top;
#     endif /* RETROFLAT_API_WINCE */

   if( !g_retroflat_state->platform.window ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Could not create window!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

   maug_cleanup_if_null_alloc( HWND, g_retroflat_state->platform.window );

#ifndef RETROFLAT_OPENGL
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRSET )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PNSET )
#endif /* !RETROFLAT_OPENGL */

   ShowWindow( g_retroflat_state->platform.window, g_retroflat_cmd_show );

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

   /* TODO: Handle mouse input in GLUT. */

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
   g_retroflat_state->platform.old_timer_interrupt =
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
   g_retroflat_state->platform.old_video_mode = r.h.al;
   debug_printf( 2, "saved old video mode: 0x%02x",
      g_retroflat_state->platform.old_video_mode );

   /* TODO: Put all screen mode dimensions in a LUT. */
   g_retroflat_state->platform.screen_mode = args->screen_mode;
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
      g_retroflat_state->platform.cga_color_table[idx] = RETROFLAT_CGA_COLOR_ ## cgac;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_CGA_COLORS_INIT )

   /* Initialize dither table. */
#     define RETROFLAT_COLOR_TABLE_CGA_DITHER_INIT( idx, name_l, name_u, r, g, b, cgac, cgad ) \
      g_retroflat_state->platform.cga_dither_table[idx] = RETROFLAT_CGA_COLOR_ ## cgad;
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
   if( NULL != g_retroflat_state->platform.frame_iter ) {
      KillTimer(
         g_retroflat_state->platform.window, RETROFLAT_WIN_FRAME_TIMER_ID );
   }

   /* Stop loop timer if available. */
   if( NULL != g_retroflat_state->platform.loop_iter ) {
      KillTimer(
         g_retroflat_state->platform.window, RETROFLAT_WIN_LOOP_TIMER_ID );
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

   if( (HDC)NULL != g_retroflat_state->platform.hdc_win ) {
      ReleaseDC(
         g_retroflat_state->platform.window,
         g_retroflat_state->platform.hdc_win );
   }

#  elif defined( RETROFLAT_API_GLUT )

   /* TODO */

#  elif defined( RETROFLAT_API_PC_BIOS )

   if( 0 != g_retroflat_state->platform.old_video_mode ) {
      /* Restore old video mode. */
      debug_printf( 3, "restoring video mode 0x%02x...",
         g_retroflat_state->platform.old_video_mode );

      memset( &r, 0, sizeof( r ) );
      r.x.ax = g_retroflat_state->platform.old_video_mode;
      int86( 0x10, &r, &r ); /* Call video interrupt. */
   }

   if( NULL != g_retroflat_state->platform.old_timer_interrupt ) {
      /* Re-install original interrupt handler. */
      debug_printf( 3, "restoring timer interrupt..." );
      _disable();
      segread( &s );
      r.h.al = 0x08;
      r.h.ah = 0x25;
      s.ds = FP_SEG( g_retroflat_state->platform.old_timer_interrupt );
      r.x.dx = FP_OFF( g_retroflat_state->platform.old_timer_interrupt );
      int86x( 0x21, &r, &r, &s );
   }

   /* Reset timer chip resolution to 18.2...ms. */
   outp( 0x43, 0x36 );
   outp( 0x40, 0x00 );
   outp( 0x40, 0x00 );

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

#ifndef RETROPLAT_PRESENT

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Build the title. */
   va_start( vargs, format );
   memset( title, '\0', RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

#if defined( RETROFLAT_API_ALLEGRO )

#elif defined( RETROFLAT_API_SDL1 )

#elif defined( RETROFLAT_API_SDL2 )

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#elif defined( RETROFLAT_API_LIBNDS )
   /* Do nothing. */
#elif defined( RETROFLAT_API_GLUT )

#else
#     pragma message( "warning: set title implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL */

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
#  if defined( RETROFLAT_API_ALLEGRO )

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#  elif defined( RETROFLAT_API_LIBNDS )

   /* == libNDS == */

   return ((TIMER1_DATA * (1 << 16)) + TIMER0_DATA) / 32;

#  elif defined( RETROFLAT_API_GLUT )

#  elif defined( RETROFLAT_API_PC_BIOS )

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

   retval = retroglu_draw_lock( bmp );

#  elif defined( RETROFLAT_API_ALLEGRO )

#  elif defined( RETROFLAT_API_SDL1 )

#  elif defined( RETROFLAT_API_SDL2 )

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#  else
#     pragma message( "warning: draw lock not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

#  ifdef RETROFLAT_OPENGL

   retval = retroglu_draw_release( bmp );

#  elif defined( RETROFLAT_API_ALLEGRO )

#  elif defined( RETROFLAT_API_SDL1 )

#  elif defined( RETROFLAT_API_SDL2 )

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#  else
#     pragma message( "warning: draw release not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, RETROFLAT_PATH_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

#  ifdef RETROFLAT_OPENGL

   retval = retroglu_load_bitmap( filename_path, bmp_out, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

#  elif defined( RETROFLAT_API_SDL1 )

#  elif defined( RETROFLAT_API_SDL2 )

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#  elif defined( RETROFLAT_API_PC_BIOS )

#  else
#     pragma message( "warning: load bitmap not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO */

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int i = 0;
   PALETTEENTRY palette[RETROFLAT_BMP_COLORS_SZ_MAX];
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

#  if defined( RETROFLAT_OPENGL )

   retval = retroglu_create_bitmap( w, h, bmp_out, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )
   
#  elif defined( RETROFLAT_API_SDL1 )

#  elif defined( RETROFLAT_API_SDL2 )

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#  elif defined( RETROFLAT_API_PC_BIOS )

#  else
#     pragma message( "warning: create bitmap not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

#  if defined( RETROFLAT_OPENGL )

   retroglu_destroy_bitmap( bmp );

#  elif defined( RETROFLAT_API_ALLEGRO )

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#  elif defined( RETROFLAT_API_PC_BIOS )

#  else
#     pragma message( "warning: destroy bitmap not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int16_t w, int16_t h
) {
#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   MERROR_RETVAL retval = MERROR_OK;
   int locked_src_internal = 0;
#  endif /* RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  ifndef RETROFLAT_OPENGL
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#  endif /* RETROFLAT_OPENGL */

   assert( NULL != src );

#  if defined( RETROFLAT_OPENGL )

   retroglu_blit_bitmap( target, src, s_x, s_y, d_x, d_y, w, h );

#  elif defined( RETROFLAT_API_ALLEGRO )

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#  elif defined( RETROFLAT_API_PC_BIOS )

#  else
#     pragma message( "warning: blit bitmap not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO */
   return;
}

#endif /* !RETROPLAT_PRESENT */

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

   switch( g_retroflat_state->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_CGA:
      /* Divide y by 2 since both planes are SCREEN_H / 2 high. */
      /* Divide result by 4 since it's 2 bits per pixel. */
      screen_byte_offset = (((y >> 1) * target->w) + x) >> 2;
      /* Shift the bits over by the remainder. */
      /* TODO: Factor out this modulo to shift/and. */
      screen_bit_offset = 6 - (((((y >> 1) * target->w) + x) % 4) << 1);

      /* Dither colors on odd/even squares. */
      if( (x & 0x01 && y & 0x01) || (!(x & 0x01) && !(y & 0x01)) ) {
         color = g_retroflat_state->platform.cga_color_table[color_idx];
      } else {
         color = g_retroflat_state->platform.cga_dither_table[color_idx];
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

   default:
      error_printf( "pixel blit unsupported in video mode: %d",
         g_retroflat_state->platform.screen_mode );
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

   switch( g_retroflat_state->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_VGA:
      /* TODO: Try accelerated 2D. */
      retrosoft_line( target, color_idx, x1, y1, x2, y2, flags );
      break;

   default:
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

#ifndef RETROFLAT_NO_STRING

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

#endif /* !RETROFLAT_NO_STRING */

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

#  elif defined( RETROFLAT_API_PC_BIOS )

   /* Set VGA mask register. */
   switch( g_retroflat_state->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_VGA:
      outp( 0x3c6, 0xff );
      outp( 0x3c7, idx );
      *p_rgb = 0;
      *p_rgb |= ((uint32_t)(inp( 0x3c9 ) & 0xff) << 16);
      *p_rgb |= ((uint32_t)(inp( 0x3c9 ) & 0xff) << 8);
      *p_rgb |= (inp( 0x3c9 ) & 0xff);
      break;

   default:
      error_printf( "could not set palette index %d in screen mode %d!",
         idx, g_retroflat_state->platform.screen_mode );
      break;
   }

#  else
#     pragma message( "warning: get palette not implemented" )
#  endif

}

/* === */

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb ) {
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_API_PC_BIOS )
   uint8_t byte_buffer = 0;
#  endif /* RETROFLAT_API_PC_BIOS */

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

#  elif defined( RETROFLAT_API_PC_BIOS )

   /* Set VGA mask register. */
   switch( g_retroflat_state->platform.screen_mode ) {
   case RETROFLAT_SCREEN_MODE_VGA:
      /* TODO: This doesn't seem to be working in DOSBox? */
      outp( 0x3c6, 0xff );
      outp( 0x3c8, idx );

      byte_buffer = (rgb >> 16) & 0x3f;
      debug_printf( 1, "r: %u", byte_buffer );
      outp( 0x3c9, byte_buffer );

      byte_buffer = (rgb >> 8) & 0x3f;
      debug_printf( 1, "g: %u", byte_buffer );
      outp( 0x3c9, byte_buffer );

      byte_buffer = rgb & 0x3f;
      debug_printf( 1, "b: %u", byte_buffer );
      outp( 0x3c9, byte_buffer );
      break;

   default:
      error_printf( "could not set palette index %d in screen mode %d!",
         idx, g_retroflat_state->platform.screen_mode );
      break;
   }

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

#ifndef RETROPLAT_PRESENT

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

#  if defined( RETROFLAT_API_ALLEGRO )

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   
#  elif defined( RETROFLAT_API_GLUT )

#  elif defined( RETROFLAT_API_PC_BIOS )

#  else
#     pragma message( "warning: poll input not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   /* TODO: Handle NDS input! */

   return key_out;
}

#endif /* !RETROPLAT_PRESENT */

/* === */

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

