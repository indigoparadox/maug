
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
 * \relates retroflat_blit_bitmap
 * \brief Pass to retroflat_blit_bitmap() instance arg if this is not a sprite
 *        (i.e. if it is a background tile).
 */
#define RETROFLAT_INSTANCE_NULL (-1)

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

#if defined( RETROFLAT_OS_WIN ) && !defined( MAUG_WINDOWS_H )
#  include <windows.h>
#  define MAUG_WINDOWS_H
#endif /* !MAUG_WINDOWS_H */

/* TODO: Migrate all platform-specific parts below to retapid.h. */
#include <retapid.h>

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
   /* TODO: Make indirect palette optional per-platform. */
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
   size_t s_x, size_t s_y, size_t d_x, size_t d_y, size_t w, size_t h,
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
#  endif /* RETROFLAT_OPENGL */
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

   g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_RUNNING;
   do {
      if(
         RETROFLAT_FLAGS_UNLOCK_FPS !=
         (RETROFLAT_FLAGS_UNLOCK_FPS & g_retroflat_state->retroflat_flags) &&
         retroflat_get_ms() < next
      ) {
         /* Sleep/low power for a bit. */
#     ifdef retroflat_wait_for_vblank
         retroflat_wait_for_vblank();
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

#  if !defined( RETROFLAT_API_PC_BIOS ) && !defined( RETROFLAT_NO_CLI_SZ )

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

#  endif /* !RETROFLAT_API_PC_BIOS && !RETROFLAT_NO_CLI_SZ */

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

   /* = Begin Init Procedure = */

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
      debug_printf( 1, "setting window scale to 2x..." );
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

   /* == Platform-Specific Init == */

   retval = retroflat_init_platform( argc, argv, args );
   maug_cleanup_if_not_ok();

#  if defined( RETROFLAT_SOFT_SHAPES ) || defined( RETROFLAT_SOFT_LINES )
   retval = retrosoft_init();
   maug_cleanup_if_not_ok();
#  endif /* RETROFLAT_SOFT_SHAPES || RETROFLAT_SOFT_LINES */

#  if defined( RETROFLAT_OPENGL )
   retval = retrosoft_init();
   maug_cleanup_if_not_ok();
#     ifndef RETROFLAT_NO_STRING
   retval = retroglu_init_glyph_tex();
   maug_cleanup_if_not_ok();
#     endif /* !RETROFLAT_NO_STRING */
#  endif /* RETROFLAT_OPENGL */

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

#  if !defined( RETROFLAT_NO_BLANK_INIT ) && !defined( RETROFLAT_OPENGL )
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

#  if defined( RETROFLAT_OPENGL ) && !defined( RETROFLAT_NO_STRING )
   debug_printf( 1, "destroying GL glyphs..." );
   retroglu_destroy_glyph_tex();
#  endif /* RETROFLAT_OPENGL */

   /* === Platform-Specific Shutdown === */

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

void retroflat_set_proc_resize(
   retroflat_proc_resize_t on_resize_in, void* data_in
) {
   g_retroflat_state->on_resize = on_resize_in;
   g_retroflat_state->on_resize_data = data_in;
}

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

