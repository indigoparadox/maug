
#ifndef RETROFLT_H
#define RETROFLT_H

/**
 * \addtogroup maug_retroflt RetroFlat API
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
 *          int input = 0;
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
 *          / * Setup the args to retroflat_init() below to create a window 
 *            * titled "Example Program", 320x200 pixels large, and load
 *            * bitmaps from the "assets" subdirectory next to the executable.
 *            * /
 *          args.screen_w = 320;
 *          args.screen_h = 200;
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
 *            * retroflat_quit().
 *            * /
 *          retroflat_loop( (retroflat_loop_iter)example_loop, &data );
 *       
 *       cleanup:
 *       
 *          / * This must be called at the end of the program! * /
 *          retroflat_shutdown( retval );
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
 * | RETROFLAT_OS_DOS      | Specify that the program will run on DOS.        | 
 * | RETROFLAT_OS_UNIX     | Specify that the program will run on UNIX/Linux. |
 *
 * ## API/Library Selection Definitions
 *
 * These are mutually exclusive.
 *
 * | Define                | Description                                      |
 * | --------------------- | ------------------------------------------------ |
 * | RETROFLAT_API_ALLEGRO | Specify that the program will link Allegro.      |
 * | RETROFLAT_API_SDL1    | Specify that the program will link SDL 1.2.      |
 * | RETROFLAT_API_SDL2    | Specify that the program will link SDL 2.        |
 * | RETROFLAT_API_WIN16   | Specify that the program will use Win 3.1x API.  |
 * | RETROFLAT_API_WIN32   | Specify that the program will use Win32/NT API.  |
 *
 * ## Option Definitions
 *
 * These are \b NOT mutually exclusive.
 *
 * | Define                | Description                                       |
 * | --------------------- | ------------------------------------------------- |
 * | ::RETROFLAT_CLI_SIGIL | Specify the sigil prepended to command-line args. |
 * | RETROFLAT_NO_CLI_SZ   | Disable ::RETROFLAT_CLI_SZ command-line args.     |
 * | RETROFLAT_MOUSE       | Force-enable mouse on broken APIs (DANGEROUS!)    |
 * | RETROFLAT_TXP_R       | Specify R component of bitmap transparent color.  |
 * | RETROFLAT_TXP_G       | Specify G component of bitmap transparent color.  |
 * | RETROFLAT_TXP_B       | Specify B component of bitmap transparent color.  |
 * | RETROFLAT_BITMAP_EXT  | Specify file extension for bitmap assets.         |
 * | RETROFLAT_RESIZABLE   | Allow resizing the RetroFlat window.              |
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
 * Outlined below is a basic GNU Makefile for using OpenWatcom to cross-compile
 * a Win16 application under GNU/Linux. This Makefile assumes the following
 * variable definitions from the environment:
 *
 * - \b $WATCOM should be set to the path where the OpenWatcom compiler has
 *   been extracted, containing the binw, binl, h, and lib386 directories
 *   (among others).
 * - \b $PATH should contain $WATCOM/bin, so that the wcc386 and accompanying
 *   tools below can be executed.
 *
 * For an example of the main.c file, please see \ref maug_retroflt_example.
 *
 * The Makefile is as follows:
 *
 *       # A list of C files included in the project. These must be provided
 *       # by you. In this example, our program only has one file.
 *       EXAMPLE_C_FILES := main.c
 *
 *       # A basic set of defines to instruct RetroFlat to use The Win16 API on
 *       # the (target) Windows OS.
 *       DEFINES := -DRETROFLAT_OS_WIN -DRETROFLAT_API_WIN16
 *     
 *       # Including the defines above, these flags will be passed to the C
 *       # compiler in the rules, below. The -i flags specify that we want
 *       # to include the Windows headers from OpenWatcom, as well as the
 *       # RetroFlat header directory located in ./maug/src. The
 *       # -bt=windows flag specifies that we want to compile code for Win16.
 *       CFLAGS := $(DEFINES) -bt=windows -i$(WATCOM)/h/win -imaug/src
 *
 *       # The only mandatory linker flag for our example is -l=win386, which
 *       # instructs the Watcom linker to target the win386 32-bit Windows
 *       # extender, which we will include with our executable below. This
 *       # allows our program to run on Windows 3.x without us having to
 *       # observe 16-bit memory constraints or deal with LocalHeaps or
 *       # handle locking.
 *       LDFLAGS := -l=win386
 *      
 *       # This rule handles compiling all object files requested by the next
 *       # rule from their respective C code files.
 *       %.o: %.c
 *     	   wcc386 $(CFLAGS) -fo=$@ $(<:%.c=%)
 *
 *       # This rule builds the intermediate LE executable to which the win386
 *       # extender will be prepended. The -fe flag specifies the name of the
 *       # executable to create.
 *       examplew.rex: $(subst .c,.o,$(EXAMPLE_C_FILES))
 *      	   wcl386 $(LDFLAGS) -fe=$@ $^
 *      
 *       # This rule builds the final executable, by prepending the win386
 *       # extender located in the OpenWatcom installation directory to the
 *       # Intermediate file created in the previous rule.
 *       examplew.exe: examplew.rex
 *      	   wbind $< -s $(WATCOM)/binw/win386.ext -R $@
 *
 * \}
 */

#  include <stdarg.h>

/* === Generic Includes and Defines === */

#define RETROFLAT_COLOR_TABLE( f ) \
   f( 0, black, BLACK, 0, 0, 0 ) \
   f( 1, darkblue, DARKBLUE, 0, 0, 170 ) \
   f( 2, darkgreen, DARKGREEN, 0, 170, 0 ) \
   f( 3, teal, TEAL, 0, 170, 170 ) \
   f( 4, darkred, DARKRED, 170, 0, 0 ) \
   f( 5, violet, VIOLET, 170, 0, 170 ) \
   f( 6, brown, BROWN, 170, 85, 0 ) \
   f( 7, gray, GRAY, 170, 170, 170 ) \
   f( 8, darkgray, DARKGRAY, 85, 85, 85 ) \
   f( 9, blue, BLUE, 85, 85, 255 ) \
   f( 10, green, GREEN, 85, 255, 85 ) \
   f( 11, cyan, CYAN, 85, 255, 255 ) \
   f( 12, red, RED, 255, 85, 85 ) \
   f( 13, magenta, MAGENTA, 255, 85, 255 ) \
   f( 14, yellow, YELLOW, 255, 255, 85 ) \
   f( 15, white, WHITE, 255, 255, 255 )

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
 * \{
 */

/**
 * \brief Flag for retroflat_rect() or retroflat_ellipse(), indicating drawn
 *        shape should be filled.
 */
#define RETROFLAT_FLAGS_FILL     0x01

/*! \} */ /* maug_retroflt_drawing */

/**
 * \brief Flag for g_retroflat_flags indicating that retroflat_loop() should
 *        continue executing.
 * \warning This flag is not used on all platforms! It should only be removed
 *          using retroflat_quit().
 */
#define RETROFLAT_FLAGS_RUNNING  0x01

/**
 * \addtogroup maug_retroflt_cli RetroFlat Command Line API
 * \brief Tools for parsing command line arguments in RetroFlat programs.
 * \{
 */

#ifdef RETROFLAT_NO_CLI_SZ
#  define RETROFLAT_CLI_SZ( f )
#else
/**
 * \brief CLI arguments accepted if RETROFLAT_NO_CLI_SZ is not passed as a
 *        \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_CLI_SZ( f ) \
   f( RETROFLAT_CLI_SIGIL "x", 3, "Set the screen width.", retroflat_cli_x ) \
   f( RETROFLAT_CLI_SIGIL "y", 3, "Set the screen height.", retroflat_cli_y )
#endif /* !RETROFLAT_NO_CLI_SZ */

/*! \brief Default CLI arguments for all RetroFlat programs. */
#define RETROFLAT_CLI( f ) \
   f( RETROFLAT_CLI_SIGIL "h", 3, "Display help and exit.", retroflat_cli_h ) \
   RETROFLAT_CLI_SZ( f )

#ifndef RETROFLAT_CLI_ARG_LIST_SZ_MAX
#  define RETROFLAT_CLI_ARG_LIST_SZ_MAX 50
#endif /* !RETROFLAT_CLI_ARG_LIST_SZ_MAX */

#ifndef RETROFLAT_CLI_ARG_HELP_SZ_MAX
#  define RETROFLAT_CLI_ARG_HELP_SZ_MAX 255
#endif /* !RETROFLAT_CLI_ARG_HELP_SZ_MAX */

#ifndef RETROFLAT_CLI_ARG_SZ_MAX
#  define RETROFLAT_CLI_ARG_SZ_MAX 50
#endif /* !RETROFLAT_CLI_ARG_SZ_MAX */

/*! \} */ /* maug_retroflt_cdefs_page */

#if defined( DEBUG )
#include <assert.h>
#elif !defined( DOCUMENTATION )
#define assert( x )
#endif /* DEBUG */

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
 * \brief The filename suffix to be appended with a "." to filenames passed to
 *        retroflat_load_bitmap(). Is a \ref maug_retroflt_cdefs_page.
 */
#ifndef RETROFLAT_BITMAP_EXT
#  define RETROFLAT_BITMAP_EXT "bmp"
#endif /* !RETROFLAT_BITMAP_EXT */

/**
 * \brief The default font to load if none is specified to retroflat_string().
 *        Currently this only has an effect in SDL.
 */
#ifndef RETROFLAT_DEFAULT_FONT
#  define RETROFLAT_DEFAULT_FONT "./Sans.ttf"
#endif /* RETROFLAT_DEFAULT_FONT */

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

/* Convenience macro for auto-locking inside of draw functions. */
#define retroflat_internal_autolock_bitmap( bmp, lock_ret, lock_auto ) \
   if( !retroflat_bitmap_locked( bmp ) ) { \
      lock_ret = retroflat_draw_lock( bmp ); \
      if( RETROFLAT_OK != lock_ret ) { \
         goto cleanup; \
      } \
      lock_auto = 1; \
   }

/*! \} */ /* maug_retroflt_bitmap */

#define retroflat_on_resize( w, h ) \
   g_screen_w = w; \
   g_screen_h = h;

/**
 * \brief Prototype for the main loop function passed to retroflat_loop().
 */
typedef void (*retroflat_loop_iter)(void* data);

/**
 * \addtogroup maug_retroflt_input RetroFlat Input API
 * \brief Functions and constants for polling and interpreting user input.
 * \{
 */

/*! \brief Struct passed to retroflat_poll_input() to hold return data. */
struct RETROFLAT_INPUT {
   int allow_repeat;
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
};

/*! \} */ /* maug_retroflt_input */

union RETROFLAT_FMT_SPEC {
   long int d;
   long unsigned int u;
   char c;
   void* p;
   char* s;
};

/**
 * \addtogroup maug_retroflt_cli
 * \{
 */

/*! \brief Struct containing configuration values for a RetroFlat program. */
struct RETROFLAT_ARGS {
   /**
    * \brief Title to set for the main program Window if applicable on the
    *        target platform.
    */
   char* title;
   /*! \brief Desired screen or window width in pixels. */
   int screen_w;
   /*! \brief Desired screen or window height in pixels. */
   int screen_h;
   /*! \brief Relative path under which bitmap assets are stored. */
   char* assets_path;
};

typedef int (*retroflat_cli_cb)( const char* arg, struct RETROFLAT_ARGS* data );

/*! \} */ /* maug_retroflt_cli */

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

#ifndef RETROFLAT_WIN_GFX_TIMER_ID
/**
 * \brief Unique ID for the timer that execute graphics ticks in Win16/Win32.
 *        Is a \ref maug_retroflt_cdefs_page.
 */
#  define RETROFLAT_WIN_GFX_TIMER_ID 6001
#endif /* !RETROFLAT_WIN_GFX_TIMER_ID */

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

#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
#  if !defined( RETROFLAT_WIN_STYLE )
#     define RETROFLAT_WIN_STYLE (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME)
#  endif /* !RETROFLAT_WIN_STYLE */
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#if defined( RETROFLAT_API_SDL2 )
#  if defined( RETROFLAT_RESIZABLE )
#     define RETROFLAT_WIN_FLAGS SDL_WINDOW_RESIZABLE
#  else
#     define RETROFLAT_WIN_FLAGS 0
#  endif /* RETROFLAT_RESIZABLE */
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
#define RETROFLAT_ASSETS_PATH_MAX (RETROFLAT_PATH_MAX / 2)

/*! \} */ /* maug_retroflt_compiling */

/**
 * \addtogroup maug_retroflt_cli
 * \{
 */

#define RETROFLAT_CLI_SIGIL_SZ 1

#if !defined( RETROFLAT_CLI_SIGIL ) && defined( RETROFLAT_OS_WIN )
#  define RETROFLAT_CLI_SIGIL "/"
#elif !defined( RETROFLAT_CLI_SIGIL ) && defined( RETROFLAT_OS_DOS )
#  define RETROFLAT_CLI_SIGIL "/"
#elif !defined( RETROFLAT_CLI_SIGIL )
/**
 * \brief Default flag to prepend to CLI arguments. Is "/" on Windows/DOS and
 *        "-" on other platforms. Can be overridden with a -D flag or define.
 */
#  define RETROFLAT_CLI_SIGIL "-"
#endif /* !RETROFLAT_CLI_SIGIL */

/*! \} */ /* maug_retroflt_cli */

/* === Platform-Specific Includes and Defines === */

#if defined( RETROFLAT_API_ALLEGRO )

#  ifdef RETROFLAT_OPENGL
#     warning "opengl support not implemented for allegro"
#  endif /* RETROFLAT_OPENGL */

/* == Allegro == */

#  include <allegro.h>

#  ifdef RETROFLAT_OS_DOS
#     include <dos.h>
#     include <conio.h>
#  endif /* RETROFLAT_OS_DOS */

struct RETROFLAT_BITMAP {
   unsigned char flags;
   BITMAP* b;
};

typedef int RETROFLAT_COLOR;

#  define RETROFLAT_COLOR_BLACK        makecol(0,   0,   0)
#  define RETROFLAT_COLOR_DARKBLUE     makecol(0, 0, 170)
#  define RETROFLAT_COLOR_DARKGREEN    makecol(0, 170, 0)
#  define RETROFLAT_COLOR_TEAL         makecol(0, 170, 170)
#  define RETROFLAT_COLOR_DARKRED      makecol(170, 0, 0)
#  define RETROFLAT_COLOR_VIOLET       makecol(170, 0, 170)
#  define RETROFLAT_COLOR_BROWN        makecol(170, 85, 0)
#  define RETROFLAT_COLOR_GRAY         makecol(170, 170, 170)
#  define RETROFLAT_COLOR_DARKGRAY     makecol(85, 85, 85)
#  define RETROFLAT_COLOR_BLUE         makecol(85, 85, 255)
#  define RETROFLAT_COLOR_GREEN        makecol(85, 255, 85)
#  define RETROFLAT_COLOR_CYAN         makecol(85, 255, 255)
#  define RETROFLAT_COLOR_RED          makecol(255, 85, 85)
#  define RETROFLAT_COLOR_MAGENTA      makecol(255, 85, 255)
#  define RETROFLAT_COLOR_YELLOW       makecol(255, 255, 85)
#  define RETROFLAT_COLOR_WHITE        makecol(255, 255, 255)

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) (0)
#  define retroflat_screen_w() SCREEN_W
#  define retroflat_screen_h() SCREEN_H

#  define retroflat_quit( retval ) \
   g_retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING; \
   g_retval = retval;

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

#elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

#  if defined( RETROFLAT_API_SDL2 ) && defined( RETROFLAT_OPENGL )
#     warning "opengl support not implemented for SDL 2"
#  endif /* RETROFLAT_API_SDL2 && RETROFLAT_OPENGL */

#  include <SDL.h>
#  include <SDL_ttf.h>

struct RETROFLAT_BITMAP {
   unsigned char flags;
   SDL_Surface* surface;
#ifndef RETROFLAT_API_SDL1
   SDL_Texture* texture;
   SDL_Renderer* renderer;
#endif /* !RETROFLAT_API_SDL1 */
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
#  define RETROFLAT_KEY_ENTER	SDLK_ENTER
#  define RETROFLAT_KEY_HOME	SDLK_HOME
#  define RETROFLAT_KEY_END	SDLK_END

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->surface)
#  define retroflat_bitmap_locked( bmp ) (NULL != (bmp)->renderer)
#  define retroflat_screen_w() g_screen_v_w
#  define retroflat_screen_h() g_screen_v_h

#  define retroflat_quit( retval ) \
      g_retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING; \
      g_retval = retval;

#  define END_OF_MAIN()

#  define RETROFLAT_COLOR const SDL_Color*

#  ifdef RETROFLT_C

const SDL_Color gc_black =       {0,   0,   0};
const SDL_Color gc_darkblue =    {0, 0, 170};
const SDL_Color gc_darkgreen =   {0, 170, 0};
const SDL_Color gc_teal =        {0, 170, 170};
const SDL_Color gc_darkred =     {170, 0, 0};
const SDL_Color gc_violet =      {170, 0, 170};
const SDL_Color gc_brown =       {170, 85, 0};
const SDL_Color gc_gray =        {170, 170, 170};
const SDL_Color gc_darkgray =    {85, 85, 85};
const SDL_Color gc_blue =        {85, 85, 255};
const SDL_Color gc_green =       {85, 255, 85};
const SDL_Color gc_cyan =        {85, 255, 255};
const SDL_Color gc_red =         {255, 85, 85};
const SDL_Color gc_magenta =     {255, 85, 255};
const SDL_Color gc_yellow =      {255, 255, 85};
const SDL_Color gc_white =       {255, 255, 255};

#  else

extern const SDL_Color gc_black;
extern const SDL_Color gc_darkblue;
extern const SDL_Color gc_darkgreen;
extern const SDL_Color gc_teal;
extern const SDL_Color gc_darkred;
extern const SDL_Color gc_violet;
extern const SDL_Color gc_brown;
extern const SDL_Color gc_gray;
extern const SDL_Color gc_darkgray;
extern const SDL_Color gc_blue;
extern const SDL_Color gc_green;
extern const SDL_Color gc_cyan;
extern const SDL_Color gc_red;
extern const SDL_Color gc_magenta;
extern const SDL_Color gc_yellow;
extern const SDL_Color gc_white;

#  endif /* RETROFLT_C */

#define RETROFLAT_COLOR_BLACK       (&gc_black)
#define RETROFLAT_COLOR_DARKBLUE    (&gc_darkblue)
#define RETROFLAT_COLOR_DARKGREEN   (&gc_darkgreen)
#define RETROFLAT_COLOR_TEAL        (&gc_teal)
#define RETROFLAT_COLOR_DARKRED     (&gc_darkred)
#define RETROFLAT_COLOR_VIOLET      (&gc_violet)
#define RETROFLAT_COLOR_BROWN       (&gc_brown)
#define RETROFLAT_COLOR_GRAY        (&gc_gray)
#define RETROFLAT_COLOR_DARKGRAY    (&gc_darkgray)
#define RETROFLAT_COLOR_BLUE        (&gc_blue)
#define RETROFLAT_COLOR_GREEN       (&gc_green)
#define RETROFLAT_COLOR_CYAN        (&gc_cyan)
#define RETROFLAT_COLOR_RED         (&gc_red)
#define RETROFLAT_COLOR_MAGENTA     (&gc_magenta)
#define RETROFLAT_COLOR_YELLOW      (&gc_yellow)
#define RETROFLAT_COLOR_WHITE       (&gc_white)

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

/* == Win16/Win32 == */

#  if defined( RETROFLAT_API_WIN16 ) && defined( RETROFLAT_OPENGL )
#     warning "opengl support not implemented for win16"
#  endif /* RETROFLAT_API_SDL2 && RETROFLAT_OPENGL */

#  include <windows.h>

#  ifdef RETROFLAT_WING
#     include <wing.h>
#  endif /* RETROFLAT_WING */

struct RETROFLAT_BITMAP {
   unsigned char flags;
   HBITMAP b;
   HBITMAP mask;
   HDC hdc_b;
   HDC hdc_mask;
   HBITMAP old_hbm_b;
   HBITMAP old_hbm_mask;
};

typedef COLORREF RETROFLAT_COLOR;


#ifdef RETROFLT_C

#  define RETROFLAT_COLOR_TABLE_WIN( idx, name_l, name_u, r, g, b ) \
const int RETROFLAT_COLOR_ ## name_u = idx;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN )

#else

#  define RETROFLAT_COLOR_TABLE_WIN_EXT( idx, name_l, name_u, r, g, b ) \
extern const int RETROFLAT_COLOR_ ## name_u;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_EXT )

#endif /* RETROFLT_C */

/* === Setup Brush Cache === */

/* This will be initialized in setup, so just preserve the number. */
#  define RETROFLAT_COLOR_TABLE_WIN_BRUSHES( idx, name_l, name_u, r, g, b ) \
   (HBRUSH)NULL,

#  define RETROFLAT_COLOR_TABLE_WIN_BRSETUP( idx, name_l, name_u, r, g, b ) \
   gc_retroflat_win_brushes[idx] =  CreateSolidBrush( RGB( r, g, b ) );

#  define RETROFLAT_COLOR_TABLE_WIN_BRCLEANUP( idx, name_l, name_u, r, g, b ) \
   if( (HBRUSH)NULL != gc_retroflat_win_brushes[idx] ) { \
      DeleteObject( gc_retroflat_win_brushes[idx] ); \
      gc_retroflat_win_brushes[idx] = (HBRUSH)NULL; \
   }

#ifdef RETROFLT_C

HBRUSH gc_retroflat_win_brushes[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRUSHES )
};

#else

extern HBRUSH gc_retroflat_win_brushes[];

#endif /* RETROFLT_C */

/* === End Setup Brush Cache === */

/* === Setup Pen Cache === */

#  define RETROFLAT_COLOR_TABLE_WIN_PENS( idx, name_l, name_u, r, g, b ) \
   (HPEN)NULL,

#  define RETROFLAT_COLOR_TABLE_WIN_PENSETUP( idx, name_l, name_u, r, g, b ) \
   gc_retroflat_win_pens[idx] = CreatePen( \
      PS_SOLID, RETROFLAT_LINE_THICKNESS, RGB( r, g, b ) );

#  define RETROFLAT_COLOR_TABLE_WIN_PENCLEANUP( idx, name_l, name_u, r, g, b ) \
   if( (HPEN)NULL != gc_retroflat_win_pens[idx] ) { \
      DeleteObject( gc_retroflat_win_pens[idx] ); \
      gc_retroflat_win_pens[idx] = (HPEN)NULL; \
   }

#ifdef RETROFLT_C

HPEN gc_retroflat_win_pens[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PENS )
};

#else

extern gc_retroflat_win_pens[];

#endif /* RETROFLT_C */

/* === End Setup Pen Cache === */

#  define RETROFLAT_COLOR_TABLE_WIN_RGBS_INIT( idx, name_l, name_u, r, g, b ) \
   0,

#  define RETROFLAT_COLOR_TABLE_WIN_RGBS( idx, name_l, name_u, r, g, b ) \
   gc_retroflat_win_rgbs[idx] = RGB( r, g, b );

#ifdef RETROFLT_C

int gc_retroflat_win_rgbs[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_RGBS_INIT )
};

#else

extern int gc_retroflat_win_rgbs[];

#endif /* RETROFLT_C */

/* Create a brush and set it to the target HDC. */
#  define retroflat_win_setup_brush( old_brush, target, color, flags ) \
   if( RETROFLAT_FLAGS_FILL != (RETROFLAT_FLAGS_FILL & flags) ) { \
      old_brush = SelectObject( target->hdc_b, GetStockObject( NULL_BRUSH ) ); \
   } else { \
      old_brush = SelectObject( \
         target->hdc_b, gc_retroflat_win_brushes[color] ); \
   }

/* Create a pen and set it to the target HDC. */
#  define retroflat_win_setup_pen( old_pen, target, color, flags ) \
   old_pen = SelectObject( target->hdc_b, gc_retroflat_win_pens[color] );

#  define retroflat_win_cleanup_brush( old_brush, target ) \
   if( (HBRUSH)NULL != old_brush ) { \
      SelectObject( target->hdc_b, old_brush ); \
   }

#  define retroflat_win_cleanup_pen( old_pen, target ) \
   if( (HPEN)NULL != old_pen ) { \
      SelectObject( target->hdc_b, old_pen ); \
   }

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) ((HDC)NULL != (bmp)->hdc_b)
#  define retroflat_screen_w() g_screen_v_w
#  define retroflat_screen_h() g_screen_v_h
#  define retroflat_quit( retval ) PostQuitMessage( retval );

#  define retroflat_bmp_int( type, buf, offset ) *((type*)&(buf[offset]))

#  define RETROFLAT_KEY_UP	   VK_UP
#  define RETROFLAT_KEY_DOWN	VK_DOWN
#  define RETROFLAT_KEY_RIGHT	VK_RIGHT
#  define RETROFLAT_KEY_LEFT	VK_LEFT
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
#  define RETROFLAT_KEY_ENTER	VK_ENTER
#  define RETROFLAT_KEY_HOME	VK_HOME
#  define RETROFLAT_KEY_END	VK_END

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

/* Improvise a rough WinMain to call main(). */
#define END_OF_MAIN() \
   int WINXAPI WinMain( \
      HINSTANCE hInstance, HINSTANCE hPrevInstance, \
      LPSTR lpCmdLine, int nCmdShow \
   ) { \
      char** rf_argv = NULL; \
      int rf_argc = 0; \
      int retval = 0; \
      g_instance = hInstance; \
      g_cmd_show = nCmdShow; \
      rf_argv = retroflat_win_cli( lpCmdLine, &rf_argc ); \
      retval = main( rf_argc, rf_argv ); \
      free( rf_argv ); \
      return retval; \
   }

#else
#  warning "not implemented"

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
   unsigned char flags;
};

/*! \brief Check to see if a bitmap is loaded. */
#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)

/*! \} */ /* maug_retroflt_bitmap */

/*! \brief Get the current screen width in pixels. */
#  define retroflat_screen_w()

/*! \brief Get the current screen height in pixels. */
#  define retroflat_screen_h()

/**
 * \brief This should be called in order to quit a program using RetroFlat.
 * \param retval The return value to pass back to the operating system.
 */
#  define retroflat_quit( retval )

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

/**
 * \brief Functions that accept parameters of this type will accept the colors
 *        on this page.
 */
typedef int RETROFLAT_COLOR;

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
#  ifdef RETROFLAT_OPENGL
#     include <GL/gl.h>
#     include <GL/glu.h>
#  endif /* RETROFLAT_OPENGL */

/* === Translation Module === */

/* Declare the prototypes so that internal functions can call each other. */

/**
 * \brief This should be called once in the main body of the program in order
 *        to enter the main loop. The main loop will continuously call
 *        loop_iter with data as an argument until retroflat_quit() is called.
 */
int retroflat_loop( retroflat_loop_iter iter, void* data );

#define retroflat_bufcat( c, buf, buf_idx, buf_sz, cleanup ) \
   buf[buf_idx++] = c; \
   if( buf_idx >= buf_sz ) { goto cleanup; }

#define retroflat_bufpad( pad_c, pad_sz, i, buf, buf_idx, buf_sz, cleanup ) \
   i = 0; \
   while( i < pad_sz ) { \
      retroflat_bufcat( pad_c, buffer, buffer_idx, buffer_sz, cleanup ); \
      i++; \
   }

int retroflat_digits( long int num, int base );

#define retroflat_xtoa( num, base, rem, dest, dest_idx, digits, digits_done, pad ) \
   dest_idx += digits; \
   while( 0 != num ) { \
      /* Get the 1's place. */ \
      rem = num % base; \
      dest[--dest_idx] = (9 < rem) ? \
         /* > 10, so it's a letter. */ \
         (rem - 10) + 'a' : \
         /* < 10, so it's a number. */ \
         rem + '0'; \
      /* Move the next place value into range. */ \
      num /= base; \
      digits_done++; \
   } \
   while( digits_done < digits ) { \
      dest[--dest_idx] = '0'; \
      digits_done++; \
   }

int retroflat_itoa( long int num, char* dest, int dest_sz, int base );

int retroflat_utoa( long unsigned int num, char* dest, int dest_sz, int base );

void retroflat_vsnprintf(
   char* buffer, int buffer_sz, const char* fmt, va_list vargs );

void retroflat_snprintf( char* buffer, int buffer_sz, const char* fmt, ... );

/**
 * \brief Display a message in a dialog box and/or on stderr.
 * \param title A string with the title to use for a dialog box.
 * \param format A format string to be passed to vsnprintf().
 * \todo  This should display a dialog box on every platform if possible.
 */
void retroflat_message( const char* title, const char* format, ... );

/**
 * \addtogroup maug_retroflt_cli RetroFlat Command Line API
 * \{
 */

/**
 * \brief Add a command-line argument to the built-in parser.
 * \param arg String containing the argument to look for.
 * \param arg_sz Length of arg in chars or 0 to autodetect.
 * \param help Help text for the arg when -h is invoked.
 * \param help_sz Length of help in chars or 0 to autodetect.
 * \param cb ::retroflat_cli_cb to invoke when arg is found.
 */
void retroflat_add_arg(
   const char* arg, int arg_sz, const char* help, int help_sz,
   retroflat_cli_cb cb );

/*! \} */ /* maug_retroflt_cli */

/**
 * \brief Initialize RetroFlat and its underlying layers. This should be
 *        called once at the beginning of the program and should quit if
 *        the return value indicates any failures.
 * \return ::RETROFLAT_OK if there were no problems or other
 *         \ref maug_retroflt_retval if there were.
 */
int retroflat_init( int argc, char* argv[], struct RETROFLAT_ARGS* args );

/**
 * \brief Deinitialize RetroFlat and its underlying layers. This should be
 *        called once at the end of the program, after retroflat_loop().
 * \param retval Return value from retroflat_init(), so we know what layers
 *        are in what state.
 */
void retroflat_shutdown( int retval );

unsigned long int retroflat_get_ms();

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
int retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out );

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
   int s_x, int s_y, int d_x, int d_y, int w, int h );

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
int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp );

void retroflat_draw_release( struct RETROFLAT_BITMAP* bmp );

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
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags );

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
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags );

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
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, unsigned char flags );

void retroflat_cursor( struct RETROFLAT_BITMAP* target, unsigned char flags );

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
   struct RETROFLAT_BITMAP* target, const char* str, int str_sz,
   const char* font_str, int* w_out, int* h_out );

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
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   const char* str, int str_sz, const char* font_str, int x_orig, int y_orig,
   unsigned char flags );

/*! \} */ /* maug_retroflt_bitmap */

/**
 * \addtogroup maug_retroflt_input
 * \{
 */

/**
 * \brief Poll input devices (keyboard/mouse) and return the latest event.
 * \param input Pointer to a ::RETROFLAT_INPUT struct to store extended info.
 * \return A symbol from \ref maug_retroflt_keydefs.
 */
int retroflat_poll_input( struct RETROFLAT_INPUT* input );

/*! \} */ /* maug_retroflt_input */

#ifdef RETROFLT_C

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>

/* TODO: Declare externs in ifelse section for multi-file projects. */

void* g_loop_data = NULL;
int g_retval = 0;
int g_screen_w = 0;
int g_screen_h = 0;

#  if defined( RETROFLAT_API_ALLEGRO )

unsigned int g_last_mouse = 0;
unsigned int g_last_mouse_x = 0;
unsigned int g_last_mouse_y = 0;
volatile unsigned long g_ms;

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

#     ifndef RETROFLAT_API_SDL1
SDL_Window* g_window = NULL;
#     endif /* !RETROFLAT_API_SDL1 */
int g_screen_v_w = 0;
int g_screen_v_h = 0;
int g_mouse_state = 0;

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

/* Windows-specific global handles for the window/instance. */
HINSTANCE g_instance;
HWND g_window;
MSG g_msg;
HDC g_hdc_win = (HDC)NULL;
int g_msg_retval = 0;
int g_screen_v_w = 0;
int g_screen_v_h = 0;
int g_cmd_show = 0;
const long gc_ms_target = 1000 / RETROFLAT_FPS;
static unsigned long g_ms_start = 0;
volatile unsigned long g_ms;
unsigned char g_last_key = 0;
unsigned int g_last_mouse = 0;
unsigned int g_last_mouse_x = 0;
unsigned int g_last_mouse_y = 0;
unsigned char g_running;
retroflat_loop_iter g_loop_iter = NULL;

#  ifdef RETROFLAT_WING
struct RETROFLAT_BMI {
   BITMAPINFOHEADER header;
   RGBQUAD colors[256];
};

struct RETROFLAT_BMI g_buffer_bmi;
void far* g_buffer_bits = NULL;
#  endif /* RETROFLAT_WING */

#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* === Globals === */

char g_retroflat_assets_path[RETROFLAT_ASSETS_PATH_MAX + 1];
struct RETROFLAT_BITMAP g_buffer;
unsigned char g_retroflat_flags = 0;
unsigned char g_retroflat_cli_flags = 0;

/* TODO: Don't make these arrays fully const; add a callback to add custom
 *       command line args.
 */

#define RETROFLAT_CLI_ARG_ARG( arg, arg_sz, help, callback ) \
   arg,

char g_retroflat_cli_args[RETROFLAT_CLI_ARG_LIST_SZ_MAX][RETROFLAT_CLI_ARG_SZ_MAX] = {
   RETROFLAT_CLI( RETROFLAT_CLI_ARG_ARG )
   ""
};

#define RETROFLAT_CLI_ARG_SZ( arg, arg_sz, help, callback ) \
   arg_sz,

int g_retroflat_cli_arg_sz[RETROFLAT_CLI_ARG_LIST_SZ_MAX] = {
   RETROFLAT_CLI( RETROFLAT_CLI_ARG_SZ )
   0
};

#define RETROFLAT_CLI_ARG_HELP( arg, arg_sz, help, callback ) \
   help,

char g_retroflat_cli_arg_help[RETROFLAT_CLI_ARG_LIST_SZ_MAX][RETROFLAT_CLI_ARG_HELP_SZ_MAX] = {
   RETROFLAT_CLI( RETROFLAT_CLI_ARG_HELP )
   ""
};

/* Callback table is down below, after the statically-defined callbacks. */

/* === Function Definitions === */

#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

static LRESULT CALLBACK WndProc(
   HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
) {
   PAINTSTRUCT ps;
#ifndef RETROFLAT_OPENGL
   HDC hdc_win = (HDC)NULL;
#endif /* RETROFLAT_OPENGL */
   BITMAP srcBitmap;
   int screen_initialized = 0;
#  if defined( RETROFLAT_OPENGL )
   int pixel_fmt_int = 0;
   static HGLRC hrc_win = NULL;
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
#  endif /* RETROFLAT_OPENGL */
   static unsigned long int next = 0;

   switch( message ) {
      case WM_CREATE:
#  if defined( RETROFLAT_OPENGL )
         g_hdc_win = GetDC( hWnd );

         pixel_fmt_int = ChoosePixelFormat( g_hdc_win, &pixel_fmt );
         SetPixelFormat( g_hdc_win, pixel_fmt_int, &pixel_fmt );

         hrc_win = wglCreateContext( g_hdc_win );
         wglMakeCurrent( g_hdc_win, hrc_win );
#  endif /* RETROFLAT_OPENGL */
         break;

      case WM_CLOSE:
#  if defined( RETROFLAT_OPENGL )
         wglMakeCurrent( g_hdc_win, NULL );
         wglDeleteContext( hrc_win );
#  endif /* RETROFLAT_OPENGL */

         /* Quit on window close. */
         PostQuitMessage( 0 );
         break;

#  if !defined( RETROFLAT_OPENGL )
      case WM_PAINT:

         /* Create HDC for window to blit to. */
#  if defined( RETROFLAT_WING )
         hdc_win = GetDC( hWnd );
#  else
         hdc_win = BeginPaint( hWnd, &ps );
#  endif /* RETROFLAT_WING */
         if( (HDC)NULL == hdc_win ) {
            retroflat_message(
               "Error", "Could not determine window device context!" );
            g_retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retval );
            break;
         }

         /* === First-Time Setup === */

         /* Setup the buffer HDC from which to blit to the window. */
         if( (HDC)NULL == g_buffer.hdc_b ) {
#  ifdef RETROFLAT_WING
            g_buffer.hdc_b = WinGCreateDC();
#  else
            g_buffer.hdc_b = CreateCompatibleDC( hdc_win );
#  endif /* RETROFLAT_WING */
         }
         if( (HDC)NULL == g_buffer.hdc_b ) {
            retroflat_message(
               "Error", "Could not determine buffer device context!" );
            g_retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retval );
            break;
         }

         /* Setup the screen buffer. */
         if( !retroflat_bitmap_ok( &g_buffer ) ) {
#  ifdef RETROFLAT_WING
            /* Setup an optimal WinG hardware screen buffer bitmap. */
            if(
               !WinGRecommendDIBFormat( (BITMAPINFO far*)(&g_buffer_bmi) )
            ) {
               retroflat_message(
                  "Error", "Could not determine recommended format!" );
               g_retval = RETROFLAT_ERROR_GRAPHICS;
               retroflat_quit( g_retval );
               break;
            }
            g_buffer_bmi.header.biSize = sizeof( BITMAPINFOHEADER );
            g_buffer_bmi.header.biWidth = g_screen_w;
            g_buffer_bmi.header.biHeight *= g_screen_h;
            g_buffer.b = WinGCreateBitmap(
               g_buffer.hdc_b,
               (BITMAPINFO far*)(&g_buffer_bmi), &g_buffer_bits );
#  else
            g_buffer.b = CreateCompatibleBitmap( hdc_win,
               g_screen_v_w, g_screen_v_h );
#  endif /* RETROFLAT_WING */
            screen_initialized = 1;
         }
         if( (HBITMAP)NULL == g_buffer.b ) {
            retroflat_message( "Error", "Could not create screen buffer!" );
            g_retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retval );
            break;
         }

         if( screen_initialized ) {
            /* Create a new HDC for buffer and select buffer into it. */
            g_buffer.old_hbm_b = SelectObject( g_buffer.hdc_b, g_buffer.b );
         }

         /* === End First-Time Setup === */

#  ifdef RETROFLAT_WING
         WinGStretchBlt(
            hdc_win,
            0, 0,
            g_screen_w, g_screen_h,
            g_buffer.hdc_b,
            0, 0,
            srcBitmap.bmWidth,
            srcBitmap.bmHeight
         );
         GdiFlush();
#  else
         /* Load parameters of the buffer into info object (srcBitmap). */
         GetObject( g_buffer.b, sizeof( BITMAP ), &srcBitmap );

         StretchBlt(
            hdc_win,
            0, 0,
            g_screen_w, g_screen_h,
            g_buffer.hdc_b,
            0, 0,
            srcBitmap.bmWidth,
            srcBitmap.bmHeight,
            SRCCOPY
         );
#  endif /* RETROFLAT_WING */

#  ifdef RETROFLAT_WING
         ReleaseDC( g_window, hdc_win );
#  else
         DeleteDC( hdc_win );
         EndPaint( hWnd, &ps );
         hdc_win = (HDC)NULL;
#  endif /* RETROFLAT_WING */
         break;

#  endif /* !RETROFLAT_OPENGL */

      case WM_ERASEBKGND:
         return 1;

      case WM_KEYDOWN:
         g_last_key = wParam;
         break;

      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
         g_last_mouse = wParam;
         g_last_mouse_x = GET_X_LPARAM( lParam );
         g_last_mouse_y = GET_Y_LPARAM( lParam );
         break;

      case WM_DESTROY:
         if( retroflat_bitmap_ok( &g_buffer ) ) {
            DeleteObject( g_buffer.b );
         }
         PostQuitMessage( 0 );
         break;

      case WM_SIZE:
         retroflat_on_resize( LOWORD( lParam ), HIWORD( lParam ) );
         break;

      case WM_TIMER:
         if( next <= retroflat_get_ms() ) {
            g_loop_iter( g_loop_data );
            next = retroflat_get_ms() + retroflat_fps_next();
         }

         /* Kind of a hack so that we can have a cheap timer. */
         g_ms += 100;
         break;

      default:
         return DefWindowProc( hWnd, message, wParam, lParam );
   }

   return 0;
}

char** retroflat_win_cli( char* cmd_line, int* argc_out ) {
   char** argv_out = NULL;
   int i = 0,
      arg_iter = 0,
      arg_start = 0,
      arg_idx = 0,
      arg_longest = 10; /* Program name. */

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
   assert( NULL != argv_out );

   /* NULL program name. */
   argv_out[0] = calloc( 1, sizeof( char ) );

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
         assert( NULL != argv_out[arg_idx] );
         strncpy( argv_out[arg_idx], &(cmd_line[arg_start]), arg_iter );
         arg_idx++; /* Start next arg. */
         arg_iter = 0; /* New arg is 0 long. */
         arg_start = i; /* New arg starts here (maybe). */
      }
   }

   return argv_out;
}

#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* === */

int retroflat_loop( retroflat_loop_iter loop_iter, void* data ) {

#  if defined( RETROFLAT_API_ALLEGRO ) || defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   unsigned long int next = 0;

   g_retroflat_flags |= RETROFLAT_FLAGS_RUNNING;
   do {
      if( retroflat_get_ms() < next ) {
         continue;
      }
      loop_iter( data );
      next = retroflat_get_ms() + retroflat_fps_next();
   } while(
      RETROFLAT_FLAGS_RUNNING == (RETROFLAT_FLAGS_RUNNING & g_retroflat_flags)
   );

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* Set these to be called from WndProc later. */
   g_loop_iter = (retroflat_loop_iter)loop_iter;
   g_loop_data = (void*)data;

   /* Handle Windows messages until quit. */
   do {
      g_msg_retval = GetMessage( &g_msg, 0, 0, 0 );
      TranslateMessage( &g_msg );
      DispatchMessage( &g_msg );
   } while( 0 < g_msg_retval );

#  else
#     warning "loop not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   /* This should be set by retroflat_quit(). */
   return g_retval;
}

/* === */

int retroflat_digits( long int num, int base ) {
   int digits = 0;
   int negative = 0;

   if( 0 > num ) {
      num *= -1;
      negative = 1;
   }

   while( 0 < num ) {
      num /= base;
      digits++;
   }

   if( 0 == digits ) {
      digits = 1; /* 0 */
   }

   if( negative ) {
      digits++; /* - symbol */
   }

   return digits;
}

/* === */

int retroflat_itoa( long int num, char* dest, int dest_sz, int base ) {
   long int rem;
   int digits;
   int digits_done = 0;
   int dest_idx = 0;
   int negative = 0;

   digits = retroflat_digits( num, base );
   assert( (0 == num && 1 == digits) || (0 != num && 0 < digits) );
   assert( digits < dest_sz );

   /* Handle 0 explicitly, otherwise empty string is printed for 0. */
   if( 0 == num ) {
      dest[0] = '0';
      digits_done++;
   } else if( 0 > num ) {
      num *= -1;
      negative = 1;
   }

   retroflat_xtoa( num, base, rem, dest, dest_idx, digits, digits_done, pad );

   if( negative ) {
      dest[dest_idx] = '-';
   }
   dest[digits] = '\0';

   return digits;
}

/* === */

/* TODO: Seems to force 16-bit in Borland... why? */
int retroflat_utoa( unsigned long int num, char* dest, int dest_sz, int base ) {
   unsigned long int rem;
   int digits;
   int digits_done = 0;
   int dest_idx = 0;

   digits = retroflat_digits( num, base );
   assert( (0 == num && 1 == digits) || (0 != num && 0 < digits) );
   assert( digits < dest_sz );

   /* Handle 0 explicitly, otherwise empty string is printed for 0. */
   if( 0 == num ) {
      dest[0] = '0';
      digits_done++;
   }

   retroflat_xtoa( num, base, rem, dest, dest_idx, digits, digits_done, pad );
   dest[digits] = '\0';

   return digits;
}

/* === */

void retroflat_vsnprintf(
   char* buffer, int buffer_sz, const char* fmt, va_list vargs
) {
   int i = 0, j = 0;
   char last = '\0';
   union RETROFLAT_FMT_SPEC spec;
   int pad_len = 0;
   char c;
   char pad_char = ' ';
   int buffer_idx = 0;
   int spec_is_long = 0;

   for( i = 0 ; '\0' != fmt[i] ; i++ ) {
      c = fmt[i]; /* Separate so we can play tricks below. */
 
      if( '%' == last ) {
         /* Conversion specifier encountered. */
         memset( &spec, '\0', sizeof( union RETROFLAT_FMT_SPEC ) );
         switch( fmt[i] ) {
            case 'l':
               spec_is_long = 1;
               /* Skip resetting the last char. */
               continue;

            case 's':
               spec.s = va_arg( vargs, char* );

               /* Print string. */
               j = 0;
               while( '\0' != spec.s[j] ) {
                  retroflat_bufcat( spec.s[j++], buffer, buffer_idx,
                     buffer_sz, cleanup );
               }
               break;

            case 'u':
               if( spec_is_long ) {
                  spec.u = va_arg( vargs, long unsigned int );
               } else {
                  spec.u = va_arg( vargs, unsigned int );
               }
               
               /* Print padding. */
               pad_len -= retroflat_digits( spec.d, 10 );
               retroflat_bufpad( pad_char, pad_len, j,
                  buffer, buffer_idx, buffer_sz, cleanup );

               /* Print number. */
               buffer_idx += retroflat_utoa(
                  spec.d, &(buffer[buffer_idx]), buffer_sz - buffer_idx, 10 );
               break;

            case 'd':
               if( spec_is_long ) {
                  spec.d = va_arg( vargs, long int );
               } else {
                  spec.d = va_arg( vargs, int );
               }
               
               /* Print padding. */
               pad_len -= retroflat_digits( spec.d, 10 );
               retroflat_bufpad( pad_char, pad_len, j,
                  buffer, buffer_idx, buffer_sz, cleanup );

               /* Print number. */
               buffer_idx += retroflat_itoa(
                  spec.d, &(buffer[buffer_idx]), buffer_sz - buffer_idx, 10 );
               break;

            case 'x':
               spec.d = va_arg( vargs, int );

               /* Print padding. */
               pad_len -= retroflat_digits( spec.d, 16 );
               retroflat_bufpad( pad_char, pad_len, j,
                  buffer, buffer_idx, buffer_sz, cleanup );

               /* Print number. */
               buffer_idx += retroflat_utoa(
                  spec.d, &(buffer[buffer_idx]), buffer_sz - buffer_idx, 16 );
               break;

            case 'c':
               spec.c = va_arg( vargs, int );

               /* Print padding. */
               pad_len -= 1;
               retroflat_bufpad( pad_char, pad_len, j,
                  buffer, buffer_idx, buffer_sz, cleanup );

               /* Print char. */
               retroflat_bufcat( spec.c, buffer, buffer_idx,
                  buffer_sz, cleanup );
               break;

            case '%':
               /* Literal % */
               last = '\0';
               retroflat_bufcat( '%', buffer, buffer_idx, buffer_sz, cleanup );
               break;

            case '0':
               /* If we haven't started counting padding with a non-zero number,
                * this must be a 0-padding signifier.
                */
               if( 0 >= pad_len ) {
                  pad_char = '0';
                  c = '%';
                  break;
               }
               /* If we've already started parsing padding count digits, then
                * fall through below as a regular number.
                */
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               /* Handle multi-digit qty padding. */
               pad_len *= 10;
               pad_len += (c - '0'); /* Convert from char to int. */
               c = '%';
               break;
         }
      } else if( '%' != c ) {
         pad_char = ' '; /* Reset padding. */
         pad_len = 0; /* Reset padding. */
         spec_is_long = 0;

         /* Print non-escape characters verbatim. */
         retroflat_bufcat( c, buffer, buffer_idx,
            buffer_sz, cleanup );
      }

      last = c;
   }

cleanup:
   return;
}

/* === */

void retroflat_snprintf( char* buffer, int buffer_sz, const char* fmt, ... ) {
   va_list vargs;

   va_start( vargs, fmt );
   retroflat_vsnprintf( buffer, buffer_sz, fmt, vargs );
   va_end( vargs );
}

/* === */

void retroflat_message( const char* title, const char* format, ... ) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   retroflat_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

#  if defined( RETROFLAT_API_ALLEGRO )
   allegro_message( "%s", msg_out );
#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   /* TODO: Use a dialog box? */
   fprintf( stderr, "%s\n", msg_out );
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   /* TODO: Dialog box crashes! */
   /* MessageBox( NULL, msg_out, title, 0 ); */
   printf( "%s\n", msg_out );
#  else
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   va_end( vargs );
}

/* === */

static int retroflat_cli_h( const char* arg, struct RETROFLAT_ARGS* args ) {
   int i = 0;

   fprintf( stderr, "%s usage:\n\n", args->title );

   /* Display help for all available options. */
   while( '\0' != g_retroflat_cli_args[i][0] ) {
      fprintf( stderr, "\t%s\t%s\n", g_retroflat_cli_args[i],
         g_retroflat_cli_arg_help[i] );
      i++;
   }

   fprintf( stderr, "\n" );

   return RETROFLAT_ERROR_ENGINE;
}

#ifndef RETROFLAT_NO_CLI_SZ

static int retroflat_cli_x( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( RETROFLAT_CLI_SIGIL "x", arg, 3 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_w = atoi( arg );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_y( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( RETROFLAT_CLI_SIGIL "y", arg, 3 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_h = atoi( arg );
   }
   return RETROFLAT_OK;
}

#endif /* !RETROFLAT_NO_CLI_SZ */

#define RETROFLAT_CLI_ARG_CB( arg, arg_sz, help, callback ) \
   callback,

retroflat_cli_cb g_retroflat_cli_callbacks[RETROFLAT_CLI_ARG_LIST_SZ_MAX] = {
   RETROFLAT_CLI( RETROFLAT_CLI_ARG_CB )
   NULL
};

static int retroflat_parse_args(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   int arg_i = 0,
      const_i = 0,
      last_i = 0,
      retval = 0;

   for( arg_i = 1 ; argc > arg_i ; arg_i++ ) {
      const_i = 0;
      while( '\0' != g_retroflat_cli_args[const_i][0] ) {
         if( 0 == strncmp(
            g_retroflat_cli_args[const_i],
            argv[arg_i],
            g_retroflat_cli_arg_sz[const_i]
         ) ) {
            /* Save this matched index for the next pass. */
            last_i = const_i;
            retval = g_retroflat_cli_callbacks[const_i]( argv[arg_i], args );
            if( RETROFLAT_OK != retval ) {
               goto cleanup;
            }

            /* We found a match, so go to the next arg. */
            break;
         }
         const_i++;
      }

      if( '\0' == g_retroflat_cli_args[const_i][0] ) {
         /* No valid arg was found, so we must be passing data to the last one!
          */
         retval = g_retroflat_cli_callbacks[last_i]( argv[arg_i], args );
         if( RETROFLAT_OK != retval ) {
            goto cleanup;
         }
      }
   }

cleanup:

   return retval;
}

/* === */

void retroflat_add_arg(
   const char* arg, int arg_sz, const char* help, int help_sz,
   retroflat_cli_cb cb
) {
   int slot_idx = 0;

   /* Find empty arg slot. */
   while(
      '\0' != g_retroflat_cli_args[slot_idx][0] &&
      RETROFLAT_CLI_ARG_LIST_SZ_MAX > slot_idx
   ) {
      slot_idx++;
   }

   /* Sanity checking and sizing. */
   if( RETROFLAT_CLI_ARG_LIST_SZ_MAX <= slot_idx ) {
      retroflat_message( "Error", "Too many command line args requested!" );
      return;
   }

   if( 0 >= arg_sz ) {
      arg_sz = strlen( arg );
   }
   assert( arg_sz < RETROFLAT_CLI_ARG_SZ_MAX );

   if( 0 >= help_sz ) {
      help_sz = strlen( help );
   }
   assert( help_sz < RETROFLAT_CLI_ARG_HELP_SZ_MAX );

   /* Add arg to arrays. */

   strncpy( g_retroflat_cli_args[slot_idx], arg, arg_sz );
   g_retroflat_cli_args[slot_idx + 1][0] = '\0';
   
   strncpy( g_retroflat_cli_arg_help[slot_idx], help, help_sz );
   g_retroflat_cli_arg_help[slot_idx + 1][0] = '\0';
   
   g_retroflat_cli_arg_sz[slot_idx] = arg_sz;
   g_retroflat_cli_arg_sz[slot_idx + 1] = 0;
   
   g_retroflat_cli_callbacks[slot_idx] = cb;
   g_retroflat_cli_callbacks[slot_idx + 1] = NULL;
}

/* === */

#ifdef RETROFLAT_API_ALLEGRO

void retroflat_ms_tick() {
   g_ms++;
}

#endif /* RETROFLAT_API_ALLEGRO */

/* === */

int retroflat_init( int argc, char* argv[], struct RETROFLAT_ARGS* args ) {
   int retval = 0;
#  if defined( RETROFLAT_API_ALLEGRO ) && defined( RETROFLAT_OS_DOS )
# if 0
   union REGS regs;
   struct SREGS sregs;
#endif
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   WNDCLASS wc = { 0 };
   RECT wr = { 0, 0, 0, 0 };
#  elif defined( RETROFLAT_API_SDL1 )
#  if defined( RETROFLAT_OPENGL )
   const SDL_VideoInfo* info = NULL;
#  endif /* RETROFLAT_OPENGL */
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   /* Parse command line args. */
   retval = retroflat_parse_args( argc, argv, args );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

   /* Set the assets path. */
   memset( g_retroflat_assets_path, '\0', RETROFLAT_ASSETS_PATH_MAX );
   if( NULL != args->assets_path ) {
      strncpy( g_retroflat_assets_path,
         args->assets_path, RETROFLAT_ASSETS_PATH_MAX );
   }

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

   if( allegro_init() ) {
      allegro_message( "could not setup allegro!" );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   install_keyboard();
#     if !defined( RETROFLAT_OS_DOS )
   /* XXX: Broken in DOS. */
   install_timer();
   install_int( retroflat_ms_tick, 1 );
#     endif /* RETROFLAT_OS_DOS */

#     ifdef RETROFLAT_OS_DOS
   /* Don't try windowed mode in DOS. */
   if( set_gfx_mode( GFX_AUTODETECT, args->screen_w, args->screen_h, 0, 0 ) ) {
#     else
   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, args->screen_w, args->screen_h, 0, 0 ) ) {
#     endif /* RETROFLAT_OS_DOS */

      allegro_message( "could not setup graphics!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#     ifndef RETROFLAT_OS_DOS
   if( NULL != args->title ) {
      set_window_title( args->title );
   }

   /* XXX: Broken in DOS. */
   if( 0 > install_mouse() ) {
      allegro_message( "could not setup mouse!" );
      retval = RETROFLAT_ERROR_MOUSE;
      goto cleanup;
   }
#     endif /* !RETROFLAT_OS_DOS */

#     ifdef RETROFLAT_OS_DOS
#if 0
   regs.w.ax = 0x9;
   regs.w.bx = 0x0;
   regs.w.cx = 0x0;
   regs.x.edx = FP_OFF( g_mouse_cursor );
   sregs.es = FP_SEG( g_mouse_cursor );
   int386x( 0x33, &regs, &regs, &sregs );
#endif
#     endif /* RETROFLAT_OS_DOS */

   g_buffer.b = create_bitmap( args->screen_w, args->screen_h );
   if( NULL == g_buffer.b ) {
      allegro_message( "unable to allocate screen buffer!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   if( SDL_Init( SDL_INIT_EVERYTHING ) ) {
      retroflat_message(
         "Error", "Error initializing SDL: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   info = SDL_GetVideoInfo();
   assert( NULL != info );

#     ifdef RETROFLAT_OPENGL
   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
#     endif /* RETROFLAT_OPENGL */

   g_buffer.surface = SDL_SetVideoMode(
      args->screen_w, args->screen_h, info->vfmt->BitsPerPixel,
      SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_ANYFORMAT
#     ifdef RETROFLAT_OPENGL
      | SDL_OPENGL
#     endif /* RETROFLAT_OPENGL */
   );
   if( NULL == g_buffer.surface ) {
      retroflat_message(
         "Error", "unable to allocate screen buffer: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if( SDL_Init( SDL_INIT_EVERYTHING ) ) {
      retroflat_message(
         "Error", "Error initializing SDL: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   if( TTF_Init() ) {
      retroflat_message(
         "Error", "Error initializing SDL_TTF: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

   g_screen_v_w = args->screen_w;
   g_screen_v_h = args->screen_h;
   g_screen_w = args->screen_w;
   g_screen_h = args->screen_h;

   /* Create the main window. */
   g_window = SDL_CreateWindow( args->title,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      args->screen_w, args->screen_h, RETROFLAT_WIN_FLAGS );
   if( NULL == g_window ) {
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

   /* Create the main renderer. */
   g_buffer.renderer = SDL_CreateRenderer(
      g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE );
   if( NULL == g_buffer.renderer ) {
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

   /* Create the buffer texture. */
   g_buffer.texture = SDL_CreateTexture( g_buffer.renderer,
      SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      g_screen_w, g_screen_h );

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   /* Get the *real* size of the window, including titlebar. */
   wr.right = args->screen_w;
   wr.bottom = args->screen_h;
   AdjustWindowRect( &wr, RETROFLAT_WIN_STYLE, FALSE );

   g_screen_w = args->screen_w;
   g_screen_h = args->screen_h;
   g_screen_v_w = args->screen_w;
   g_screen_v_h = args->screen_h;

   memset( &g_buffer, '\0', sizeof( struct RETROFLAT_BITMAP ) );
   memset( &wc, '\0', sizeof( WNDCLASS ) );

   wc.lpfnWndProc   = (WNDPROC)&WndProc;
   wc.hInstance     = g_instance;
   /* TODO: Window icon. */
   /*
   wc.hIcon         = LoadIcon( g_instance, MAKEINTRESOURCE(
      g_graphics_args.icon_res ) );
   */
   wc.hCursor       = LoadCursor( 0, IDC_ARROW );
   wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );
   /* wc.lpszMenuName  = MAKEINTRESOURCE( IDR_MAINMENU ); */
   wc.lpszClassName = RETROFLAT_WINDOW_CLASS;

   if( !RegisterClass( &wc ) ) {
      retroflat_message( "Error", "Could not register window class!" );
      goto cleanup;
   }

   g_window = CreateWindowEx(
      0, RETROFLAT_WINDOW_CLASS, args->title,
      RETROFLAT_WIN_STYLE,
      CW_USEDEFAULT, CW_USEDEFAULT,
      wr.right - wr.left, wr.bottom - wr.top, 0, 0, g_instance, 0
   );

   if( !g_window ) {
      retroflat_message( "Error", "Could not create window!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

   assert( (HWND)NULL != g_window );

   if( !SetTimer(
      g_window, RETROFLAT_WIN_GFX_TIMER_ID, (int)(1000 / RETROFLAT_FPS), NULL )
   ) {
      retroflat_message( "Error", "Could not create graphics timer!" );
      retval = RETROFLAT_ERROR_TIMER;
      goto cleanup;
   }

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_RGBS )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRSETUP )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PENSETUP )

   ShowWindow( g_window, g_cmd_show );

#  else
#     warning "init not implemented"
#  endif  /* RETROFLAT_API_ALLEGRO */

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown( int retval ) {

#if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( RETROFLAT_ERROR_ENGINE != retval ) {
      clear_keybuf();
   }

   retroflat_destroy_bitmap( &g_buffer );

#elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

#  ifndef RETROFLAT_API_SDL1
   SDL_DestroyWindow( g_window );
#  endif /* !RETROFLAT_API_SDL1 */

   TTF_Quit();

   SDL_Quit();

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* TODO: Windows shutdown? */

   if( (HDC)NULL != g_buffer.hdc_b ) {
      /* Return the default object into the HDC. */
      SelectObject( g_buffer.hdc_b, g_buffer.old_hbm_b );
      DeleteDC( g_buffer.hdc_b );
      g_buffer.hdc_b = (HDC)NULL;

      /* TODO: Destroy buffer bitmap! */
   }

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRCLEANUP )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PENCLEANUP )

#else
#  warning "shutdown not implemented"
#endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 */

}

/* === */

unsigned long int retroflat_get_ms() {
#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   return g_ms;

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   
   /* == SDL == */

   return SDL_GetTicks();

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/32 == */

   return g_ms;

#  else
#  warning "get_ms not implemented"
#  endif /* RETROFLAT_API_* */
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

#  if defined( RETROFLAT_API_ALLEGRO )

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

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if( NULL != bmp && (&g_buffer) != bmp ) {
      assert( NULL == bmp->renderer );
      bmp->renderer = SDL_CreateSoftwareRenderer( bmp->surface );
   } else {
      /* Target is the screen buffer. */
      SDL_SetRenderTarget( g_buffer.renderer, g_buffer.texture );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   if( NULL == bmp ) {
      /* Do nothing. */
      /* TODO: Handle screen locking? */
      goto cleanup;
   }

   /* Sanity check. */
   assert( (HBITMAP)NULL != bmp->b );
   assert( (HBITMAP)NULL != bmp->mask );
   assert( (HDC)NULL == bmp->hdc_b );
   assert( (HDC)NULL == bmp->hdc_mask );

   /* Create HDC for source bitmap compatible with the buffer. */
   bmp->hdc_b = CreateCompatibleDC( (HDC)NULL );
   if( (HDC)NULL == bmp->hdc_b ) {
      retroflat_message( "Error", "Error locking bitmap!" );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   /* Create HDC for source mask compatible with the buffer. */
   bmp->hdc_mask = CreateCompatibleDC( (HDC)NULL );
   if( (HDC)NULL == bmp->hdc_mask ) {
      retroflat_message( "Error", "Error locking bitmap mask!" );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   bmp->old_hbm_b = SelectObject( bmp->hdc_b, bmp->b );
   bmp->old_hbm_mask = SelectObject( bmp->hdc_mask, bmp->mask );

cleanup:
#  else
#     warning "draw lock not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

/* === */

void retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( NULL != bmp ) {
      /* Don't need to lock bitmaps in Allegro. */
      return;
   }

   /* Flip the buffer. */
   blit( g_buffer.b, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

   /* Release the screen. */
   release_screen();
#     ifdef RETROFLAT_MOUSE
      /* XXX: Broken in DOS. */
   show_mouse( screen ); /* Enable mouse after drawing. */
#     endif /* RETROFLAT_MOUSE */
   vsync();

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   if( NULL == bmp ) {
      bmp = (&g_buffer);
   }

   if( (&g_buffer) == bmp ) {
#     ifdef RETROFLAT_OPENGL
      SDL_GL_SwapBuffers();
#     else
      SDL_Flip( bmp->surface );
#     endif /* RETROFLAT_OPENGL */
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if( NULL == bmp || (&g_buffer) == bmp ) {
      /* Flip the screen. */
      SDL_SetRenderTarget( g_buffer.renderer, NULL );
      SDL_RenderCopyEx(
         g_buffer.renderer, g_buffer.texture, NULL, NULL, 0, NULL, 0 );
      SDL_RenderPresent( g_buffer.renderer );
   } else {
      /* It's a bitmap. */

      /* Scrap the old texture and recreate it from the updated surface. */
      assert( NULL != bmp->texture );
      SDL_DestroyTexture( bmp->texture );
      bmp->texture = SDL_CreateTextureFromSurface(
         bmp->renderer, bmp->surface );
      assert( NULL != bmp->texture );

      /* Scrap the renderer. */
      SDL_DestroyRenderer( bmp->renderer );
      bmp->renderer = NULL;
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   if( NULL == bmp  ) {
      /* Trigger a screen refresh if this was a screen lock. */
      if( (HWND)NULL != g_window ) {
#     ifdef RETROFLAT_OPENGL
         SwapBuffers( g_hdc_win );
#     else
         InvalidateRect( g_window, 0, TRUE );
#     endif /* RETROFLAT_OPENGL */
      }
      return;
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

#  else
#     warning "draw release not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */
}

/* === */

#if defined( RETROFLAT_API_WIN16 ) || defined (RETROFLAT_API_WIN32 )

static int retroflat_bitmap_win_transparency(
   struct RETROFLAT_BITMAP* bmp_out, int w, int h  
) {
   int retval = RETROFLAT_OK,
      lock_ret = 0;
   unsigned long txp_color = 0;

   /* Setup bitmap transparency mask. */
   bmp_out->mask = CreateBitmap( w, h, 1, 1, NULL );

   lock_ret = retroflat_draw_lock( bmp_out );
   if( RETROFLAT_OK != lock_ret ) {
      goto cleanup;
   }

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

   if( RETROFLAT_OK == lock_ret ) {
      retroflat_draw_release( bmp_out );
   }

   return retval;
}

#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

int retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   int retval = 0;
#if defined( RETROFLAT_API_SDL1 )
   SDL_Surface* tmp_surface = NULL;
#elif defined( RETROFLAT_API_WIN16 ) || defined (RETROFLAT_API_WIN32 )
   HDC hdc_win = (HDC)NULL;
#  if defined( RETROFLAT_API_WIN16 )
   char* buf = NULL;
   BITMAPINFO* bmi = NULL;
   FILE* bmp_file = NULL;
   long int i, x, y, w, h, bpp, offset, sz, read;
#  elif defined( RETROFLAT_API_WIN32 )
   BITMAP bm;
#  endif /* RETROFLAT_API_WIN32 */
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   assert( NULL != bmp_out );

   /* Build the path to the bitmap. */
   memset( filename_path, '\0', RETROFLAT_PATH_MAX + 1 );
   retroflat_snprintf( filename_path, RETROFLAT_PATH_MAX, "%s%c%s.%s",
      g_retroflat_assets_path, RETROFLAT_PATH_SEP,
      filename, RETROFLAT_BITMAP_EXT );

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

   bmp_out->b = load_bitmap( filename_path, NULL );

   if( NULL != bmp_out->b ) {
      retval = RETROFLAT_OK;
   } else {
      allegro_message( "unable to load %s", filename_path );
      retval = RETROFLAT_ERROR_BITMAP;
   }

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

#     ifdef RETROFLAT_API_SDL1
   tmp_surface = SDL_LoadBMP( filename_path ); /* Free stream on close. */
   if( NULL == tmp_surface ) {
      retroflat_message(
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = 0;
      goto cleanup;
   }

   bmp_out->surface = SDL_DisplayFormat( tmp_surface );
#     else
   bmp_out->renderer = NULL;
   
   bmp_out->surface = SDL_LoadBMP( filename_path );
#     endif /* RETROFLAT_API_SDL1 */
   if( NULL == bmp_out->surface ) {
      retroflat_message(
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
      SDL_MapRGB( bmp_out->surface->format,
         RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );

#     ifndef RETROFLAT_API_SDL1
   bmp_out->texture =
      SDL_CreateTextureFromSurface( g_buffer.renderer, bmp_out->surface );
   if( NULL == bmp_out->texture ) {
      retroflat_message(
         "Error", "SDL unable to create texture: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      if( NULL != bmp_out->surface ) {
         SDL_FreeSurface( bmp_out->surface );
         bmp_out->surface = NULL;
      }
      goto cleanup;
   }
#     endif /* !RETROFLAT_API_SDL1 */

cleanup:

#     ifdef RETROFLAT_API_SDL1
   if( NULL != tmp_surface ) {
      SDL_FreeSurface( tmp_surface );
   }
#     endif /* RETROFLAT_API_SDL1 */

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#     if defined( RETROFLAT_API_WIN16 )

   /* Win16 has a bunch of extra involved steps for getting a bitmap from
    * disk. These cause a crash in Win32.
    */

   /* == Win16 == */

   /* Load the bitmap file from disk. */
   bmp_file = fopen( filename_path, "rb" );
   if( NULL == bmp_file ) {
      retroflat_message( "Error", "Could not load bitmap: %s", filename_path );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }
   fseek( bmp_file, 0, SEEK_END );
   sz = ftell( bmp_file );
   fseek( bmp_file, 0, SEEK_SET );
   read = fread( buf, sz, 1, bmp_file );
   assert( read == sz );

   bmi = (BITMAPINFO*)&(buf[sizeof( BITMAPFILEHEADER )]);

   bpp = retroflat_bmp_int( unsigned short, buf, 28 );
   offset = retroflat_bmp_int( unsigned long, buf, 10 );

   assert( 0 < bmi->bmiHeader.biWidth );
   assert( 0 < bmi->bmiHeader.biHeight );
   assert( 0 == bmi->bmiHeader.biWidth % 8 );
   assert( 0 == bmi->bmiHeader.biHeight % 8 );

   hdc_win = GetDC( g_window );
   bmp_out->b = CreateCompatibleBitmap( hdc_win,
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );
   if( NULL == bmp_out->b ) {
      retroflat_message( "Error", "Error loading bitmap!" );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   SetDIBits( hdc_win, bmp_out->b, 0,
      bmi->bmiHeader.biHeight, &(buf[offset]), bmi,
      DIB_RGB_COLORS );

   retval = retroflat_bitmap_win_transparency( bmp_out,
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );

#     else

   /* Win32 greatly simplifies the loading portion. */

   /* == Win32 == */

   bmp_out->b = LoadImage(
      NULL, filename_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
   if( NULL == bmp_out->b ) {
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   GetObject( bmp_out->b, sizeof( BITMAP ), &bm );

   retval = retroflat_bitmap_win_transparency(
      bmp_out, bm.bmWidth, bm.bmHeight );

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

   if( (HDC)NULL != hdc_win ) {
      ReleaseDC( g_window, hdc_win );
   }
#     endif /* RETROFLAT_API_WIN16 */

#  else
#     warning "load bitmap not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bitmap ) {
#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( NULL == bitmap->b ) {
      return;
   }

   destroy_bitmap( bitmap->b );
   bitmap->b = NULL;

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   assert( NULL != bitmap );
   assert( NULL != bitmap->surface );

#     ifndef RETROFLAT_API_SDL1
   assert( NULL != bitmap->texture );

   SDL_DestroyTexture( bitmap->texture );
   bitmap->texture = NULL;
#     endif /* !RETROFLAT_API_SDL1 */

   SDL_FreeSurface( bitmap->surface );
   bitmap->surface = NULL;

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16 == */

   if( (HBITMAP)NULL != bitmap->b ) {
      DeleteObject( bitmap->b );
      bitmap->b = (HBITMAP)NULL;
   }

   if( (HBITMAP)NULL != bitmap->mask ) {
      DeleteObject( bitmap->mask );
      bitmap->mask = (HBITMAP)NULL;
   }

#  else
#     warning "destroy bitmap not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int w, int h
) {
#  if defined( RETROFLAT_API_SDL2 )
   int lock_ret = 0;
   int locked_target_internal = 0;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int lock_ret = 0;
   int locked_src_internal = 0;
   int locked_target_internal = 0;
#  endif /* RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_buffer);
   }
   assert( NULL != src );

#  if defined( RETROFLAT_API_ALLEGRO )

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

   SDL_Rect dest_rect = {
      d_x, 
      d_y,
      w, 
      h};
   SDL_Rect src_rect = {
      s_x, s_y, w, h };

#     ifdef RETROFLAT_API_SDL1
   SDL_BlitSurface( src->surface, &src_rect, target->surface, &dest_rect );
#     else
   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   SDL_RenderCopy( target->renderer, src->texture, &src_rect, &dest_rect );
#     endif /* RETROFLAT_API_SDL1 */

#     ifndef RETROFLAT_API_SDL1
cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }
#     endif /* !RETROFLAT_API_SDL1 */

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );
   assert( NULL != src->b );

   retroflat_internal_autolock_bitmap(
      src, lock_ret, locked_src_internal );
   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   /* Use mask to blit transparency. */
   BitBlt(
      target->hdc_b, d_x, d_y, w, h, src->hdc_mask, s_x, s_y, SRCAND );

   /* Do actual blit. */
   BitBlt(
      target->hdc_b, d_x, d_y, w, h, src->hdc_b, s_x, s_y, SRCPAINT );

cleanup:

   if( locked_src_internal ) {
      retroflat_draw_release( src );
   }

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  else
#     warning "blit bitmap not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */
   return;
}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags
) {
#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HBRUSH old_brush = (HBRUSH)NULL;
   int lock_ret = 0,
      locked_target_internal = 0;
   HPEN old_pen = (HPEN)NULL;
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_buffer);
   }

#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );
   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      rectfill( target->b, x, y, x + w, y + h, color );
   } else {
      rect( target->b, x, y, x + w, y + h, color );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   SDL_Rect area;
   int locked_target_internal = 0;
   int lock_ret = 0;

   area.x = x;
   area.y = y;
   area.w = w;
   area.h = h;

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   SDL_SetRenderDrawColor(
      target->renderer, color->r, color->g, color->b, 255 );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      SDL_RenderFillRect( target->renderer, &area );
   } else {
      SDL_RenderDrawRect( target->renderer, &area );
   }

cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   retroflat_win_setup_brush( old_brush, target, color, flags );
   retroflat_win_setup_pen( old_pen, target, color, flags );

   Rectangle( target->hdc_b, x, y, x + w, y + h );

cleanup:

   retroflat_win_cleanup_brush( old_brush, target )
   retroflat_win_cleanup_pen( old_pen, target )

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  else
#     warning "rect not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, unsigned char flags
) {
#  if defined( RETROFLAT_API_SDL2 )
   int lock_ret = 0,
      locked_target_internal = 0;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HPEN pen = (HPEN)NULL;
   HPEN old_pen = (HPEN)NULL;
   POINT points[2];
   int lock_ret = 0,
      locked_target_internal = 0;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_buffer);
   }

#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );
   line( target->b, x1, y1, x2, y2, color );

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   SDL_SetRenderDrawColor(
      target->renderer, color->r, color->g, color->b, 255 );
   SDL_RenderDrawLine( target->renderer, x1, y1, x2, y2 );
 
cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }  

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   retroflat_win_setup_pen( old_pen, target, color, flags );

   /* Create the line points. */
   points[0].x = x1;
   points[0].y = y1;
   points[1].x = x2;
   points[1].y = y2;

   Polyline( target->hdc_b, points, 2 );

cleanup:

   if( (HPEN)NULL != pen ) {
      SelectObject( target->hdc_b, old_pen );
      DeleteObject( pen );
   }

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  else
#     warning "line not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags
) {
#  if defined( RETROFLAT_API_SDL2 )
   int lock_ret = 0,
      locked_target_internal = 0;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HPEN old_pen = (HPEN)NULL;
   HBRUSH old_brush = (HBRUSH)NULL;
   int lock_ret = 0,
      locked_target_internal = 0;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_buffer);
   }

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

   assert( NULL != target->b );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      ellipsefill( target->b, x + (w / 2), y + (h / 2), w / 2, h / 2, color );
   } else {
      ellipse( target->b, x + (w / 2), y + (h / 2), w / 2, h / 2, color );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   SDL_SetRenderDrawColor(
      target->renderer, color->r, color->g, color->b, 255 );

   /* TODO: How to do an ellipse in SDL? */
 
cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }  

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   retroflat_win_setup_brush( old_brush, target, color, flags );
   retroflat_win_setup_pen( old_pen, target, color, flags );

   Ellipse( target->hdc_b, x, y, x + w, y + h );

cleanup:

   retroflat_win_cleanup_brush( old_brush, target )
   retroflat_win_cleanup_pen( old_pen, target )

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  else
#     warning "ellipse not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_cursor( struct RETROFLAT_BITMAP* target, unsigned char flags ) {
#if 0
   char mouse_str[11] = "";

   retroflat_snprintf(
      mouse_str, 10, "%02d, %02d", g_last_mouse_x, g_last_mouse_y );

   retroflat_string(
      target, RETROFLAT_COLOR_BLACK,
      mouse_str, 10, NULL, 0, 0, 0 );
   retroflat_rect(
      target, RETROFLAT_COLOR_BLACK,
      g_last_mouse_x - 5, g_last_mouse_y - 5, 10, 10, 0 );
#endif
}

/* === */

void retroflat_string_sz(
   struct RETROFLAT_BITMAP* target, const char* str, int str_sz,
   const char* font_str, int* w_out, int* h_out
) {
#  if defined( RETROFLAT_API_ALLEGRO )
   FONT* font_data = NULL;
   int font_loaded = 0;
#  elif defined( RETROFLAT_API_SDL2 )
   TTF_Font* font_data = NULL;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int lock_ret = 0,
      locked_target_internal = 0;
   SIZE sz;
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_buffer);
   }

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

   if( NULL == font_str ) {
      font_data = font;
   } else {
      /* TODO: Cache loaded fonts for later use. */
      font_data = load_font( font_str, NULL, NULL );
   }
   if( NULL == font_data ) {
      retroflat_message( "Error", "Unable to load font: %s", font_str );
      goto cleanup;
   }

   *w_out = text_length( font_data, str );
   *h_out = text_height( font_data );
   
cleanup:

   if( font_loaded && NULL != font_data ) {
      destroy_font( font_data );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

   if( NULL == font_str ) {
      font_str = RETROFLAT_DEFAULT_FONT;
   }

   font_data = TTF_OpenFont( font_str, 12 );
   if( NULL == font_data ) {
      retroflat_message( "Error", "Unable to load font: %s", font_str );
      goto cleanup;
   }

   TTF_SizeText( font_data, str, w_out, h_out );

cleanup:

   if( NULL != font_data ) {
      /* TODO: Cache loaded fonts for later use. */
      TTF_CloseFont( font_data );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   /* TODO: Set specified font. */

   GetTextExtentPoint( target->hdc_b, str, str_sz, &sz );
   *w_out = sz.cx;
   *h_out = sz.cy;

cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  else
#     warning "string sz not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_string(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   const char* str, int str_sz, const char* font_str, int x_orig, int y_orig,
   unsigned char flags
) {
#  if defined( RETROFLAT_API_ALLEGRO )
   FONT* font_data = NULL;
   int font_loaded = 0;
#  elif defined( RETROFLAT_API_SDL2 )
   TTF_Font* font_data = NULL;
   SDL_Surface* str_surface = NULL;
   SDL_Texture* str_texture = NULL;
   SDL_Rect str_rect;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int lock_ret = 0,
      locked_target_internal = 0;
   RECT rect;
   SIZE sz;
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_buffer);
   }

   if( 0 == str_sz ) {
      str_sz = strlen( str );
   }

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

   if( NULL == font_str ) {
      font_data = font;
   } else {
      /* TODO: Cache loaded fonts for later use. */
      font_data = load_font( font_str, NULL, NULL );
   }
   if( NULL == font_data ) {
      retroflat_message( "Error", "Unable to load font: %s", font_str );
      goto cleanup;
   }

   textout_ex( target->b, font_data, str, x_orig, y_orig, color, -1 );

cleanup:
   if( font_loaded && NULL != font_data ) {
      destroy_font( font_data );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

   if( NULL == font_str ) {
      font_str = RETROFLAT_DEFAULT_FONT;
   }

   font_data = TTF_OpenFont( font_str, 12 );
   if( NULL == font_data ) {
      retroflat_message( "Error", "Unable to load font: %s", font_str );
      goto cleanup;
   }

   str_surface = TTF_RenderText_Solid( font_data, str, *color );
   if( NULL == str_surface ) {
      retroflat_message( "Error", "Unable to render string surface." );
      goto cleanup;
   }

   str_texture = SDL_CreateTextureFromSurface( target->renderer, str_surface );
   if( NULL == str_surface ) {
      retroflat_message( "Error", "Unable to render string texture." );
      goto cleanup;
   }

   str_rect.x = x_orig;
   str_rect.y = y_orig;
   str_rect.w = str_surface->w;
   str_rect.h = str_surface->h;

   SDL_RenderCopy( target->renderer, str_texture, NULL, &str_rect );

cleanup:

   if( NULL != str_surface ) {
      SDL_FreeSurface( str_surface );
   }

   if( NULL != str_texture ) {
      SDL_DestroyTexture( str_texture );
   }

   if( NULL != font_data ) {
      /* TODO: Cache loaded fonts for later use. */
      TTF_CloseFont( font_data );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( NULL != target->b );

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   /* TODO: Set specified font. */

   memset( &sz, '\0', sizeof( SIZE ) );

   GetTextExtentPoint( target->hdc_b, str, str_sz, &sz );
   rect.left = x_orig;
   rect.top = y_orig;
   rect.right = (x_orig + sz.cx);
   rect.bottom = (y_orig + sz.cy);

   SetTextColor( target->hdc_b, gc_retroflat_win_rgbs[color] );
   SetBkMode( target->hdc_b, TRANSPARENT );

   DrawText( target->hdc_b, str, str_sz, &rect, 0 );

cleanup:

   SetBkMode( target->hdc_b, OPAQUE );
   SetTextColor( target->hdc_b, gc_retroflat_win_rgbs[RETROFLAT_COLOR_BLACK] );

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  else
#     warning "string not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

int retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
#  if defined( RETROFLAT_API_ALLEGRO ) && defined( RETROFLAT_OS_DOS )
   union REGS inregs;
   union REGS outregs;
#  endif /* RETROFLAT_API_ALLEGRO && RETROFLAT_OS_DOS */
   int key_out = 0;

   assert( NULL != input );

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

#     ifdef RETROFLAT_OS_DOS

   /* Poll the mouse. */
   inregs.w.ax = 3;
   int386( 0x33, &inregs, &outregs );

   if(
      1 == outregs.x.ebx && /* Left button clicked. */
      outregs.w.cx != g_last_mouse_x &&
      outregs.w.dx != g_last_mouse_y
   ) { 
      input->mouse_x = outregs.w.cx;
      input->mouse_y = outregs.w.dx;

      /* Prevent repeated clicks. */
      g_last_mouse_x = input->mouse_x;
      g_last_mouse_y = input->mouse_y;

      return RETROFLAT_MOUSE_B_LEFT;
   } else {
      g_last_mouse_x = outregs.w.cx;
      g_last_mouse_y = outregs.w.dx;
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
      return (readkey() >> 8);
   }

#  elif defined( RETROFLAT_API_SDL2 ) || defined( RETROFLAT_API_SDL1 )

   /* == SDL == */

   int eres = 0;
   SDL_Event event;

   SDL_PollEvent( &event );

   /* TODO: Handle SDL window close. */

   if( SDL_KEYDOWN == event.type ) {
      key_out = event.key.keysym.sym;

      /* Flush key buffer to improve responsiveness. */
      if( !input->allow_repeat ) {
         while( (eres = SDL_PollEvent( &event )) );
      }

   } else if( SDL_MOUSEBUTTONUP == event.type ) {
      /* Stop dragging. */
      g_mouse_state = 0;

   } else if( 0 != g_mouse_state ) {

      /* Update coordinates and keep dragging. */

      SDL_GetMouseState( &(input->mouse_x), &(input->mouse_y) );
      key_out = g_mouse_state;

   } else if( SDL_MOUSEBUTTONDOWN == event.type ) {

      /* Begin dragging. */

      input->mouse_x = event.button.x;  
      input->mouse_y = event.button.y;  

      /* Differentiate which button was clicked. */
      if( SDL_BUTTON_LEFT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_LEFT;
         g_mouse_state = RETROFLAT_MOUSE_B_LEFT;
      } else if( SDL_BUTTON_RIGHT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_RIGHT;
         g_mouse_state = RETROFLAT_MOUSE_B_RIGHT;
      }

      /* Flush key buffer to improve responsiveness. */
      while( (eres = SDL_PollEvent( &event )) );

#  if !defined( RETROFLAT_API_SDL1 )
   } else if( SDL_WINDOWEVENT == event.type ) {
      switch( event.window.event ) {
      case SDL_WINDOWEVENT_RESIZED:
         retroflat_on_resize( event.window.data1, event.window.data2 );
      }
#  endif /* !RETROFLAT_API_SDL1 */
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   
   /* == Win16/Win32 == */

   if( g_last_key ) {
      /* Return g_last_key, which is set in WndProc when a keypress msg is
      * received.
      */
      key_out = g_last_key;

      /* Reset pressed key. */
      g_last_key = 0;

   } else if( g_last_mouse ) {
      if( MK_LBUTTON == (MK_LBUTTON & g_last_mouse) ) {
         input->mouse_x = g_last_mouse_x;
         input->mouse_y = g_last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_LEFT;
      } else if( MK_RBUTTON == (MK_RBUTTON & g_last_mouse) ) {
         input->mouse_x = g_last_mouse_x;
         input->mouse_y = g_last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_RIGHT;
      }
      g_last_mouse = 0;
      g_last_mouse_x = 0;
      g_last_mouse_y = 0;
   }

#  else
#     warning "poll input not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   return key_out;
}

#else

extern int g_retval;
extern int g_screen_w;
extern int g_screen_h;
extern int g_screen_v_w;
extern int g_screen_v_h;
extern unsigned char g_retroflat_flags;

#endif /* RETROFLT_C */

/*! \} */ /* maug_retroflt */

#endif /* RETROFLT_H */

