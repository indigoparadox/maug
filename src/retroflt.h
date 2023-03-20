
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

/* === Generic Includes and Defines === */

#include <stdarg.h>

#include <marge.h>

#ifdef RETROFLAT_API_WINCE
/* WinCE is very similar to Win32, so we'll mostly use that with some exceptions below. */
#	define RETROFLAT_API_WIN32
#endif /* RETROFLAT_API_WINCE */

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

#define bmp_read_uint32( b ) (*((uint32_t*)(b)))

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

/*! \} */ /* maug_retroflt_drawing */

/**
 * \brief Flag for g_retroflat_flags indicating that retroflat_loop() should
 *        continue executing.
 * \warning This flag is not used on all platforms! It should only be removed
 *          using retroflat_quit().
 */
#define RETROFLAT_FLAGS_RUNNING  0x01

#define RETROFLAT_FLAGS_UNLOCK_FPS 0x02

#define RETROFLAT_FLAGS_KEY_REPEAT 0x04

#define RETROFLAT_FLAGS_SCREENSAVER 0x08

#define RETROFLAT_BUFFER_INT 0x00

#define RETROFLAT_BUFFER_STRING 0x01

#define RETROFLAT_BUFFER_FLOAT 0x02

#define RETROFLAT_BUFFER_BOOL 0x04

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

#ifndef RETROFLAT_TITLE_MAX
#  define RETROFLAT_TITLE_MAX 255
#endif /* !RETROFLAT_TITLE_MAX */

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

/*! \} */ /* maug_retroflt_compiling */

/**
 * \addtogroup maug_retroflt_cli
 * \{
 */

/*! \} */ /* maug_retroflt_cli */

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

/* === Platform-Specific Includes and Defines === */

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

struct RETROFLAT_BITMAP {
   uint8_t flags;
   BITMAP* b;
};

typedef int RETROFLAT_COLOR;

#  ifdef RETROFLT_C

#  define RETROFLAT_COLOR_TABLE_ALLEGRO( idx, name_l, name_u, r, g, b ) \
      int RETROFLAT_COLOR_ ## name_u;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_ALLEGRO )

#  else

#  define RETROFLAT_COLOR_TABLE_ALLEGRO_EXT( idx, name_l, name_u, r, g, b ) \
      extern int RETROFLAT_COLOR_ ## name_u;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_ALLEGRO_EXT )

#  endif /* RETROFLT_C */

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) (0)
#  define retroflat_screen_w() SCREEN_W
#  define retroflat_screen_h() SCREEN_H

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

#  if !defined( RETROFLAT_SOFT_SHAPES ) && !defined( RETROFLAT_OPENGL )
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

typedef FILE* RETROFLAT_CONFIG;

struct RETROFLAT_BITMAP {
   uint8_t flags;
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
#  define RETROFLAT_KEY_ENTER	SDLK_RETURN
#  define RETROFLAT_KEY_HOME	SDLK_HOME
#  define RETROFLAT_KEY_END	SDLK_END
#  define RETROFLAT_KEY_DELETE   SDLK_DELETE
#  define RETROFLAT_KEY_PGUP     SDLK_PAGEUP
#  define RETROFLAT_KEY_PGDN     SDLK_PAGEDOWN
#  define RETROFLAT_KEY_GRAVE SDLK_BACKQUOTE
#  define RETROFLAT_KEY_SLASH SDLK_SLASH
#  define RETROFLAT_KEY_BKSP  SDLK_BACKSPACE

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->surface)
#  ifdef RETROFLAT_API_SDL1
#     define retroflat_bitmap_locked( bmp ) \
         (RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & (bmp)->flags))
#  else
#     define retroflat_bitmap_locked( bmp ) (NULL != (bmp)->renderer)
#  endif
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)

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

typedef float MAUG_CONST* RETROFLAT_COLOR;

/* TODO: Generate with table macro. */
#     define RETROFLAT_COLOR_BLACK       RETROGLU_COLOR_BLACK       
#     define RETROFLAT_COLOR_DARKBLUE    RETROGLU_COLOR_DARKBLUE    
#     define RETROFLAT_COLOR_DARKGREEN   RETROGLU_COLOR_DARKGREEN   
#     define RETROFLAT_COLOR_TEAL        RETROGLU_COLOR_TEAL        
#     define RETROFLAT_COLOR_DARKRED     RETROGLU_COLOR_DARKRED     
#     define RETROFLAT_COLOR_VIOLET      RETROGLU_COLOR_VIOLET      
#     define RETROFLAT_COLOR_BROWN       RETROGLU_COLOR_BROWN       
#     define RETROFLAT_COLOR_GRAY        RETROGLU_COLOR_GRAY        
#     define RETROFLAT_COLOR_DARKGRAY    RETROGLU_COLOR_DARKGRAY    
#     define RETROFLAT_COLOR_BLUE        RETROGLU_COLOR_BLUE        
#     define RETROFLAT_COLOR_GREEN       RETROGLU_COLOR_GREEN       
#     define RETROFLAT_COLOR_CYAN        RETROGLU_COLOR_CYAN        
#     define RETROFLAT_COLOR_RED         RETROGLU_COLOR_RED         
#     define RETROFLAT_COLOR_MAGENTA     RETROGLU_COLOR_MAGENTA     
#     define RETROFLAT_COLOR_YELLOW      RETROGLU_COLOR_YELLOW      
#     define RETROFLAT_COLOR_WHITE       RETROGLU_COLOR_WHITE       

#else

typedef SDL_Color MAUG_CONST* RETROFLAT_COLOR;

#     ifdef RETROFLT_C

#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, r, g, b ) \
            MAUG_CONST SDL_Color gc_retroflat_color_ ## name_l = {r, g, b};

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL )

#        define RETROFLAT_COLOR_TABLE_SDL_P( idx, name_l, name_u, r, g, b ) \
            MAUG_CONST SDL_Color* RETROFLAT_COLOR_ ## name_u = \
               &gc_retroflat_color_ ## name_l;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL_P )

#     else

#        define RETROFLAT_COLOR_TABLE_SDL_EXT( idx, n_l, n_u, r, g, b ) \
               extern MAUG_CONST SDL_Color gc_retroflat_color_ ## n_l;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL_EXT )

#        define RETROFLAT_COLOR_TABLE_SDL_P_EXT( idx, n_l, n_u, r, g, b ) \
            extern MAUG_CONST SDL_Color* RETROFLAT_COLOR_ ## n_u;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL_P_EXT )

#     endif /* RETROFLT_C */

#  endif /* RETROFLAT_OPENGL */

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

/* == Win16/Win32 == */

#  if defined( RETROFLAT_API_WIN16 ) && defined( RETROFLAT_OPENGL )
#     error "opengl support not implemented for win16"
#  endif /* RETROFLAT_API_SDL2 && RETROFLAT_OPENGL */

#ifndef MAUG_WINDOWS_H
#  include <windows.h>
#  define MAUG_WINDOWS_H
#endif /* !MAUG_WINDOWS_H */

#  include <mmsystem.h>

#  ifndef RETROFLAT_API_WINCE
#     include <time.h> /* For srand() */
#  endif /* !RETROFLAT_API_WINCE */

#  ifdef RETROFLAT_WING
#     include <wing.h>

struct RETROFLAT_BMI {
   BITMAPINFOHEADER header;
   RGBQUAD colors[256];
};
#  endif /* RETROFLAT_WING */

#  ifdef RETROFLAT_API_WIN32
typedef HKEY RETROFLAT_CONFIG;
#  else
typedef int RETROFLAT_CONFIG;
#  endif /* RETROFLAT_API_WIN32 */

struct RETROFLAT_BITMAP {
   uint8_t flags;
   HBITMAP b;
   HBITMAP mask;
   HDC hdc_b;
   HDC hdc_mask;
   HBITMAP old_hbm_b;
   HBITMAP old_hbm_mask;
};

/* TODO: Remove this in favor of mmem.h. */
#  if defined( RETROFLAT_C ) && defined( RETROFLAT_API_WINCE )
void* calloc( size_t n, size_t s ) {
   void* out = NULL;

   out = malloc( n * s );
   memset( out, '\0', n * s );

   return out;
}
#  endif /* RETROFLAT_API_WINCE */

#  ifdef RETROFLAT_OPENGL

typedef float MAUG_CONST* RETROFLAT_COLOR;

/* TODO: Generate with table macro. */
#     define RETROFLAT_COLOR_BLACK       RETROGLU_COLOR_BLACK       
#     define RETROFLAT_COLOR_DARKBLUE    RETROGLU_COLOR_DARKBLUE    
#     define RETROFLAT_COLOR_DARKGREEN   RETROGLU_COLOR_DARKGREEN   
#     define RETROFLAT_COLOR_TEAL        RETROGLU_COLOR_TEAL        
#     define RETROFLAT_COLOR_DARKRED     RETROGLU_COLOR_DARKRED     
#     define RETROFLAT_COLOR_VIOLET      RETROGLU_COLOR_VIOLET      
#     define RETROFLAT_COLOR_BROWN       RETROGLU_COLOR_BROWN       
#     define RETROFLAT_COLOR_GRAY        RETROGLU_COLOR_GRAY        
#     define RETROFLAT_COLOR_DARKGRAY    RETROGLU_COLOR_DARKGRAY    
#     define RETROFLAT_COLOR_BLUE        RETROGLU_COLOR_BLUE        
#     define RETROFLAT_COLOR_GREEN       RETROGLU_COLOR_GREEN       
#     define RETROFLAT_COLOR_CYAN        RETROGLU_COLOR_CYAN        
#     define RETROFLAT_COLOR_RED         RETROGLU_COLOR_RED         
#     define RETROFLAT_COLOR_MAGENTA     RETROGLU_COLOR_MAGENTA     
#     define RETROFLAT_COLOR_YELLOW      RETROGLU_COLOR_YELLOW      
#     define RETROFLAT_COLOR_WHITE       RETROGLU_COLOR_WHITE       

#else

/* Use Windoes API and generate brushes/pens for GDI. */

typedef COLORREF RETROFLAT_COLOR;

#     ifdef RETROFLT_C

#        define RETROFLAT_COLOR_TABLE_WIN( idx, name_l, name_u, r, g, b ) \
            const uint32_t RETROFLAT_COLOR_ ## name_u = idx;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN )

#     else

#        define RETROFLAT_COLOR_TABLE_W_EXT( idx, name_l, name_u, r, g, b ) \
            extern const uint32_t RETROFLAT_COLOR_ ## name_u;

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_W_EXT )

#     endif /* RETROFLT_C */

/* === Setup Brush Cache === */

/* This will be initialized in setup, so just preserve the number. */
#     define RETROFLAT_COLOR_TABLE_WIN_BRUSH( idx, name_l, name_u, r, g, b ) \
         (HBRUSH)NULL,

#     define RETROFLAT_COLOR_TABLE_WIN_BRSET( idx, name_l, name_u, r, g, b ) \
         gc_retroflat_win_brushes[idx] =  CreateSolidBrush( RGB( r, g, b ) );

#     define RETROFLAT_COLOR_TABLE_WIN_BRRM( idx, name_l, name_u, r, g, b ) \
   if( (HBRUSH)NULL != gc_retroflat_win_brushes[idx] ) { \
      DeleteObject( gc_retroflat_win_brushes[idx] ); \
      gc_retroflat_win_brushes[idx] = (HBRUSH)NULL; \
   }

#ifdef RETROFLT_C

#else


#endif /* RETROFLT_C */

/* === End Setup Brush Cache === */

/* === Setup Pen Cache === */

#  define RETROFLAT_COLOR_TABLE_WIN_PENS( idx, name_l, name_u, r, g, b ) \
   (HPEN)NULL,

#     define RETROFLAT_COLOR_TABLE_WIN_PNSET( idx, name_l, name_u, r, g, b ) \
         gc_retroflat_win_pens[idx] = CreatePen( \
            PS_SOLID, RETROFLAT_LINE_THICKNESS, RGB( r, g, b ) );

#     define RETROFLAT_COLOR_TABLE_WIN_PENRM( idx, name_l, name_u, r, g, b ) \
         if( (HPEN)NULL != gc_retroflat_win_pens[idx] ) { \
            DeleteObject( gc_retroflat_win_pens[idx] ); \
            gc_retroflat_win_pens[idx] = (HPEN)NULL; \
         }

/* === End Setup Pen Cache === */

#     define RETROFLAT_COLOR_TABLE_WIN_RGBS_INIT( idx, name_l, name_u, r, g, b ) \
         0,

#     define RETROFLAT_COLOR_TABLE_WIN_RGBS( idx, name_l, name_u, r, g, b ) \
         gc_retroflat_win_rgbs[idx] = RGB( r, g, b );

#     ifdef RETROFLT_C

HBRUSH gc_retroflat_win_brushes[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRUSH )
};

static HPEN gc_retroflat_win_pens[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PENS )
};

static uint32_t gc_retroflat_win_rgbs[] = {
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_RGBS_INIT )
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
#        define retroflat_win_setup_pen( old_pen, target, color, flags ) \
            old_pen = \
               SelectObject( target->hdc_b, gc_retroflat_win_pens[color] );

#        define retroflat_win_cleanup_brush( old_brush, target ) \
            if( (HBRUSH)NULL != old_brush ) { \
               SelectObject( target->hdc_b, old_brush ); \
            }

#  define retroflat_win_cleanup_pen( old_pen, target ) \
            if( (HPEN)NULL != old_pen ) { \
               SelectObject( target->hdc_b, old_pen ); \
            }

#endif /* RETROFLAT_OPENGL */

#  define retroflat_bitmap_ok( bitmap ) ((HBITMAP)NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) ((HDC)NULL != (bmp)->hdc_b)
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)
#  define retroflat_quit( retval_in ) PostQuitMessage( retval_in );

#  define retroflat_bmp_int( type, buf, offset ) *((type*)&(buf[offset]))

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

#elif defined( RETROFLAT_API_LIBNDS )

/* == Nintendo DS == */

#  include <nds.h>

/* NDS doesn't have primitives. */
#  ifndef RETROFLAT_SOFT_SHAPES
#     define RETROFLAT_SOFT_SHAPES
#  endif /* !RETROFLAT_SOFT_SHAPES */

#  define BG_TILE_W_PX 8
#  define BG_TILE_H_PX 8
#  define BG_W_TILES 32

struct RETROFLAT_BITMAP {
   uint16_t* b;
};

typedef int RETROFLAT_COLOR;

#  ifdef RETROFLT_C
#     define RETROFLAT_COLOR_TABLE_NDS_RGBS( idx, name_l, name_u, r, g, b ) \
         RETROFLAT_COLOR RETROFLAT_COLOR_ ## name_u = 0;
#  else
#     define RETROFLAT_COLOR_TABLE_NDS_RGBS( idx, name_l, name_u, r, g, b ) \
         extern RETROFLAT_COLOR RETROFLAT_COLOR_ ## name_u;
#  endif /* RETROFLT_C */

RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_NDS_RGBS )

#  ifdef RETROFLAT_NDS_WASD
#     define RETROFLAT_KEY_A           KEY_LEFT
#     define RETROFLAT_KEY_D           KEY_RIGHT
#     define RETROFLAT_KEY_W           KEY_UP
#     define RETROFLAT_KEY_S           KEY_DOWN
#  else
#     define RETROFLAT_KEY_LEFT        KEY_LEFT
#     define RETROFLAT_KEY_RIGHT       KEY_RIGHT
#     define RETROFLAT_KEY_UP          KEY_UP
#     define RETROFLAT_KEY_DOWN        KEY_DOWN
#  endif /* RETROFLAT_NDS_WASD */
#  define RETROFLAT_KEY_ENTER       KEY_START
#  define RETROFLAT_KEY_SPACE       KEY_A
#  define RETROFLAT_KEY_ESC         KEY_B
#  define RETROFLAT_MOUSE_B_LEFT    (-1)
#  define RETROFLAT_MOUSE_B_RIGHT   (-2)

/* TODO */
#  define retroflat_bitmap_locked( bmp ) (0)

#  define retroflat_screen_w() (256)
#  define retroflat_screen_h() (192)

#  define END_OF_MAIN()

/* TODO? */
#  define retroflat_quit( retval_in )

#elif defined( RETROFLAT_API_GLUT )

#ifndef RETROFLAT_OPENGL
#  error "RETROFLAT_API_GLUT specified without RETROFLAT_OPENGL!"
#  define RETROFLAT_OPENGL
#endif /* !RETROFLAT_OPENGL */

#include <GL/glut.h>

#  ifndef RETROFLAT_CONFIG_USE_FILE
#     define RETROFLAT_CONFIG_USE_FILE
#  endif /* !RETROFLAT_CONFIG_USE_FILE */

typedef FILE* RETROFLAT_CONFIG;

typedef float MAUG_CONST* RETROFLAT_COLOR;

/* TODO: Generate with table macro. */
#     define RETROFLAT_COLOR_BLACK       RETROGLU_COLOR_BLACK       
#     define RETROFLAT_COLOR_DARKBLUE    RETROGLU_COLOR_DARKBLUE    
#     define RETROFLAT_COLOR_DARKGREEN   RETROGLU_COLOR_DARKGREEN   
#     define RETROFLAT_COLOR_TEAL        RETROGLU_COLOR_TEAL        
#     define RETROFLAT_COLOR_DARKRED     RETROGLU_COLOR_DARKRED     
#     define RETROFLAT_COLOR_VIOLET      RETROGLU_COLOR_VIOLET      
#     define RETROFLAT_COLOR_BROWN       RETROGLU_COLOR_BROWN       
#     define RETROFLAT_COLOR_GRAY        RETROGLU_COLOR_GRAY        
#     define RETROFLAT_COLOR_DARKGRAY    RETROGLU_COLOR_DARKGRAY    
#     define RETROFLAT_COLOR_BLUE        RETROGLU_COLOR_BLUE        
#     define RETROFLAT_COLOR_GREEN       RETROGLU_COLOR_GREEN       
#     define RETROFLAT_COLOR_CYAN        RETROGLU_COLOR_CYAN        
#     define RETROFLAT_COLOR_RED         RETROGLU_COLOR_RED         
#     define RETROFLAT_COLOR_MAGENTA     RETROGLU_COLOR_MAGENTA     
#     define RETROFLAT_COLOR_YELLOW      RETROGLU_COLOR_YELLOW      
#     define RETROFLAT_COLOR_WHITE       RETROGLU_COLOR_WHITE       

struct RETROFLAT_BITMAP {
   uint8_t flags;
};

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) 0
#  define retroflat_screen_w() (g_retroflat_state->screen_v_w)
#  define retroflat_screen_h() (g_retroflat_state->screen_v_h)
#  define retroflat_quit( retval_in ) glutDestroyWindow( glutGetWindow() )
#  define END_OF_MAIN()

#  define GLUT_SPECIAL_KEY_OFFSET 0x80

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
   uint8_t flags;
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
   /*! \brief Desired screen or window width in pixels. */
   int screen_w;
   /*! \brief Desired screen or window height in pixels. */
   int screen_h;
   /*! \brief Relative path under which bitmap assets are stored. */
   char* assets_path;
   uint8_t flags;
   /*! \brief Relative path of local config file (if not using registry). */
   char* config_path;
};

struct RETROFLAT_STATE {
   void*                   loop_data;
   MERROR_RETVAL           retval;
   uint8_t                 retroflat_flags;
   char                    config_path[RETROFLAT_PATH_MAX + 1];
   char                    assets_path[RETROFLAT_ASSETS_PATH_MAX + 1];
   struct RETROFLAT_BITMAP buffer;

#if defined( RETROFLAT_API_ALLEGRO )

#  ifdef RETROFLAT_OS_DOS
   unsigned int         last_mouse;
   unsigned int         last_mouse_x;
   unsigned int         last_mouse_y;
#endif /* RETROFLAT_OS_DOS */
   unsigned int         close_button;
   unsigned long        ms;

#elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

#  ifndef RETROFLAT_API_SDL1
   SDL_Window*          window;
   int                  screen_w;
   int                  screen_h;
#  endif /* !RETROFLAT_API_SDL1 */
   int                  screen_v_w;
   int                  screen_v_h;
   int                  mouse_state;

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   HWND                 window;
   int16_t              last_idc; /* Last clicked button. */
#  ifdef RETROFLAT_SCREENSAVER
   HWND                 parent;
#  endif /* RETROFLAT_SCREENSAVER */
   MSG                  msg;
   HDC                  hdc_win;
   int                  msg_retval;
   int                  screen_v_w;
   int                  screen_v_h;
   int                  screen_w;
   int                  screen_h;
   uint8_t              last_key;
   unsigned int         last_mouse;
   unsigned int         last_mouse_x;
   unsigned int         last_mouse_y;
   retroflat_loop_iter  loop_iter;
#  ifdef RETROFLAT_WING
   struct RETROFLAT_BMI buffer_bmi;
   void far*            buffer_bits;
#  endif /* RETROFLAT_WING */

#elif defined( RETROFLAT_API_LIBNDS )

   uint16_t*            sprite_frames[NDS_SPRITES_ACTIVE];
   int                  bg_id;
   uint8_t              bg_bmp_changed;
   uint8_t              window_bmp_changed;
   int                  window_id;
   int                  px_id;
   uint16_t             bg_tiles[1024];
   uint16_t             window_tiles[1024];

#  elif defined( RETROFLAT_API_GLUT )

   int                  screen_v_w;
   int                  screen_v_h;
   size_t               retroflat_next;
   retroflat_loop_iter  loop_iter;
   int16_t              retroflat_last_key;

#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

};

/* === Translation Module === */

/* Declare the prototypes so that internal functions can call each other. */

/**
 * \brief This should be called once in the main body of the program in order
 *        to enter the main loop. The main loop will continuously call
 *        loop_iter with data as an argument until retroflat_quit() is called.
 */
MERROR_RETVAL retroflat_loop( retroflat_loop_iter iter, void* data );

/**
 * \brief Display a message in a dialog box and/or on stderr.
 * \param title A string with the title to use for a dialog box.
 * \param format A format string to be passed to vsnprintf().
 * \todo  This should display a dialog box on every platform if possible.
 */
void retroflat_message( const char* title, const char* format, ... );

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

void retroflat_set_title( const char* format, ... );

uint32_t retroflat_get_ms();

uint32_t retroflat_get_rand();

#define retroflat_opengl_push( x, y, x_f, y_f, aspect_f ) \
   /* Switch to projection setup. */ \
   glMatrixMode( GL_PROJECTION ); \
   glPushMatrix(); \
   /* Lighting makes overlay text hard to see. */ \
   glDisable( GL_LIGHTING ); \
   /* Use ortho for overlay. */ \
   glLoadIdentity(); \
   aspect_f = (float)retroflat_screen_w() / (float)retroflat_screen_h(); \
   glOrtho( -1.0f * aspect_f, aspect_f, -1.0f, 1.0f, 0, 10.0f ); \
   /* -1 to 1 is 2! */ \
   aspect_f *= 2.0f; \
   /* Assuming width > height for aspect ratio. */ \
   x_f = ((x) * aspect_f / retroflat_screen_w()) - (aspect_f / 2); \
   /* Vertical coords also need to be inverted because OpenGL. */ \
   y_f = 1.0f - ((y) * 2.0f / retroflat_screen_h()); \

#define retroflat_opengl_whf( w, h, w_f, h_f, aspect_f ) \
   w_f = ((w) * aspect_f / retroflat_screen_w()); \
   h_f = ((h) * 2.0f / retroflat_screen_h());

#define retroflat_opengl_pop() \
   /* Restore modelview. */ \
   glPopMatrix(); \
   glMatrixMode( GL_MODELVIEW );

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
   const char* filename, struct RETROFLAT_BITMAP* bmp_out );

MERROR_RETVAL retroflat_create_bitmap(
   int w, int h, struct RETROFLAT_BITMAP* bmp_out );

#if defined( RETROFLAT_XPM ) || defined( DOCUMENTATION )

/**
 * \brief Load a compiled-in XPM image into an API-specific bitmap context.
 * \warn The XPM must have been generated from a bitmap using the rather
 *       restrictive \ref maug_retroflt_bitmap rules.
 */
MERROR_RETVAL retroflat_load_xpm(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out );

#endif /* RETROFLAT_XPM || defined( DOCUMENTATION ) */

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
MERROR_RETVAL retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp );

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp );

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int x, int y, uint8_t flags );

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
   int x, int y, int w, int h, uint8_t flags );

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
   int x, int y, int w, int h, uint8_t flags );

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
   int x1, int y1, int x2, int y2, uint8_t flags );

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
   struct RETROFLAT_BITMAP* target, const char* str, int str_sz,
   const char* font_str, int* w_out, int* h_out, uint8_t flags );

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
   const char* str, int str_sz, const char* font_str, int x_orig, int y_orig,
   uint8_t flags );

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

MERROR_RETVAL retroflat_config_open( RETROFLAT_CONFIG* config );

void retroflat_config_close( RETROFLAT_CONFIG* config );

size_t retroflat_config_write(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer, size_t buffer_sz_max );

size_t retroflat_config_read(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer_out, size_t buffer_out_sz_max,
   const void* default_out, size_t default_out_sz );

#ifdef RETROFLT_C

MAUG_MHANDLE g_retroflat_state_h = (MAUG_MHANDLE)NULL;
struct RETROFLAT_STATE* g_retroflat_state = NULL;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
/* For now, these are set by WinMain(), so they need to be outside of the
 * state that's zeroed on init()!
 */
HINSTANCE            g_retroflat_instance;
int                  g_retroflat_cmd_show;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>

/* Callback table is down below, after the statically-defined callbacks. */

/* === Function Definitions === */

#  if defined( RETROFLAT_SOFT_SHAPES ) && !defined( MAUG_NO_AUTO_C )
/* TODO: Define _C here? */
#     define RETROFP_C
#     include <retrofp.h>
#     define RETROSFT_C
#     include <retrosft.h>
#  endif /* RETROFLAT_SOFT_SHAPES */

#  if defined( RETROFLAT_OPENGL ) && !defined( MAUG_NO_AUTO_C )
#     define RETROGLU_C
#     include <retroglu.h>
#  endif /* RETROFLAT_OPENGL */

#  ifdef RETROFLAT_WING

static HBITMAP retroflat_wing_buffer_setup( HDC hdc_win ) {
   PALETTEENTRY palette[256];
   int i = 0;

   /* Setup an optimal WinG hardware screen buffer bitmap. */
   debug_printf( 1, "retroflat: creating WinG buffer..." );
   if(
      !WinGRecommendDIBFormat( 
         (BITMAPINFO far*)&(g_retroflat_state->buffer_bmi) )
   ) {
      retroflat_message(
         "Error", "Could not determine recommended format!" );
      g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
      retroflat_quit( g_retroflat_state->retval );
      return NULL;
   }
   g_retroflat_state->buffer_bmi.header.biSize = sizeof( BITMAPINFOHEADER );
   g_retroflat_state->buffer_bmi.header.biWidth = g_retroflat_state->screen_w;
   g_retroflat_state->buffer_bmi.header.biHeight *= g_retroflat_state->screen_h;

   GetSystemPaletteEntries( hdc_win, 0, 256, palette );
   for( i = 0 ; 256 > i ; i++ ) {
      g_retroflat_state->buffer_bmi.colors[i].rgbRed = palette[i].peRed;
      g_retroflat_state->buffer_bmi.colors[i].rgbGreen = palette[i].peGreen;
      g_retroflat_state->buffer_bmi.colors[i].rgbBlue = palette[i].peBlue;
      g_retroflat_state->buffer_bmi.colors[i].rgbReserved = 0;
   }

   return WinGCreateBitmap(
      g_retroflat_state->buffer.hdc_b,
      (BITMAPINFO far*)(&g_retroflat_state->buffer_bmi),
      &(g_retroflat_state->buffer_bits) );
}

#  endif /* RETROFLAT_WING */

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

static LRESULT CALLBACK WndProc(
   HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
) {
   PAINTSTRUCT ps;
#     ifndef RETROFLAT_OPENGL
   HDC hdc_win = (HDC)NULL;
#     endif /* !RETROFLAT_OPENGL */
   BITMAP srcBitmap;
   int screen_initialized = 0;
#     if defined( RETROFLAT_OPENGL )
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
#     endif /* RETROFLAT_OPENGL */
   static uint32_t next = 0;

   switch( message ) {
      case WM_CREATE:
#     if defined( RETROFLAT_OPENGL )
         
         g_retroflat_state->hdc_win = GetDC( hWnd );

         pixel_fmt_int =
            ChoosePixelFormat( g_retroflat_state->hdc_win, &pixel_fmt );
         SetPixelFormat(
            g_retroflat_state->hdc_win, pixel_fmt_int, &pixel_fmt );

         hrc_win = wglCreateContext( g_retroflat_state->hdc_win );
         wglMakeCurrent( g_retroflat_state->hdc_win, hrc_win );

#     else

         hdc_win = GetDC( hWnd );

         /* Setup the buffer HDC from which to blit to the window. */
         if( (HDC)NULL == g_retroflat_state->buffer.hdc_b ) {
#        ifdef RETROFLAT_WING
            debug_printf( 1, "retroflat: creating screen buffer WinGDC..." );
            g_retroflat_state->buffer.hdc_b = WinGCreateDC();
#        else
            debug_printf( 1, "retroflat: creating screen buffer HDC..." );
            g_retroflat_state->buffer.hdc_b = CreateCompatibleDC( hdc_win );
#        endif /* RETROFLAT_WING */
         }
         if( (HDC)NULL == g_retroflat_state->buffer.hdc_b ) {
            retroflat_message(
               "Error", "Could not determine buffer device context!" );
            g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retroflat_state->retval );
            break;
         }

         /* Setup the screen buffer. */
         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            debug_printf( 1, "retroflat: creating window buffer..." );
#        ifdef RETROFLAT_WING
         /* Do this in its own function so a one-time setup isn't using stack
          * in our WndProc!
          */
            g_retroflat_state->buffer.b =
               retroflat_wing_buffer_setup( hdc_win );
            g_retroflat_state->buffer.mask = (HBITMAP)NULL;
#        else
            g_retroflat_state->buffer.b = CreateCompatibleBitmap( hdc_win,
               g_retroflat_state->screen_v_w, g_retroflat_state->screen_v_h );
            g_retroflat_state->buffer.mask = (HBITMAP)NULL;
#        endif /* RETROFLAT_WING */
            screen_initialized = 1;
         }
         if( !retroflat_bitmap_ok( &(g_retroflat_state->buffer) ) ) {
            retroflat_message( "Error", "Could not create screen buffer!" );
            g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retroflat_state->retval );
            break;
         }

         if( screen_initialized ) {
            /* Create a new HDC for buffer and select buffer into it. */
            g_retroflat_state->buffer.old_hbm_b = SelectObject(
               g_retroflat_state->buffer.hdc_b,
               g_retroflat_state->buffer.b );
         }

#     endif /* RETROFLAT_OPENGL */
         break;

      case WM_CLOSE:
#     if defined( RETROFLAT_OPENGL )
         wglMakeCurrent( g_retroflat_state->hdc_win, NULL );
         wglDeleteContext( hrc_win );
#     endif /* RETROFLAT_OPENGL */

         /* Quit on window close. */
         retroflat_quit( 0 );
         break;

#     if !defined( RETROFLAT_OPENGL )
      case WM_PAINT:

         assert( (HBITMAP)NULL != g_retroflat_state->buffer.b );

         /* Create HDC for window to blit to. */
         /* maug_mzero( &ps, sizeof( PAINTSTRUCT ) ); */
         hdc_win = BeginPaint( hWnd, &ps );
         if( (HDC)NULL == hdc_win ) {
            retroflat_message(
               "Error", "Could not determine window device context!" );
            g_retroflat_state->retval = RETROFLAT_ERROR_GRAPHICS;
            retroflat_quit( g_retroflat_state->retval );
            break;
         }

         /* Load parameters of the buffer into info object (srcBitmap). */
         GetObject(
            g_retroflat_state->buffer.b, sizeof( BITMAP ), &srcBitmap );

#        ifdef RETROFLAT_WING
         WinGStretchBlt(
            hdc_win,
            0, 0,
            g_retroflat_state->screen_w, g_retroflat_state->screen_h,
            g_retroflat_state->buffer.hdc_b,
            0, 0,
            srcBitmap.bmWidth,
            srcBitmap.bmHeight
         );
#           ifdef RETROFLAT_API_WIN32
         GdiFlush();
#           endif /* RETROFLAT_API_WIN32 */
#        else
         StretchBlt(
            hdc_win,
            0, 0,
            g_retroflat_state->screen_w, g_retroflat_state->screen_h,
            g_retroflat_state->buffer.hdc_b,
            0, 0,
            srcBitmap.bmWidth,
            srcBitmap.bmHeight,
            SRCCOPY
         );
#        endif /* RETROFLAT_WING */

         DeleteDC( hdc_win );
         EndPaint( hWnd, &ps );
         hdc_win = (HDC)NULL;
         break;

#     endif /* !RETROFLAT_OPENGL */

      case WM_ERASEBKGND:
         return 1;

      case WM_KEYDOWN:
         g_retroflat_state->last_key = wParam;
         break;

      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
         g_retroflat_state->last_mouse = wParam;
         g_retroflat_state->last_mouse_x = GET_X_LPARAM( lParam );
         g_retroflat_state->last_mouse_y = GET_Y_LPARAM( lParam );
         break;

      case WM_DESTROY:
         if( retroflat_bitmap_ok( &g_retroflat_state->buffer ) ) {
            DeleteObject( g_retroflat_state->buffer.b );
         }
         PostQuitMessage( 0 );
         break;

      case WM_SIZE:
         retroflat_on_resize( LOWORD( lParam ), HIWORD( lParam ) );
         break;

      case WM_TIMER:
         g_retroflat_state->loop_iter( g_retroflat_state->loop_data );
         next = retroflat_get_ms() + retroflat_fps_next();
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
   if(
      RETROFLAT_FLAGS_UNLOCK_FPS !=
      (RETROFLAT_FLAGS_UNLOCK_FPS & g_retroflat_state->retroflat_flags) &&
      retroflat_get_ms() < g_retroflat_state->retroflat_next
   ) {
      return;
   }
   
   glutPostRedisplay();

   g_retroflat_state->retroflat_next = retroflat_get_ms() + retroflat_fps_next();
}

#ifdef RETROFLAT_OS_OS2
void APIENTRY
#else
void
#endif /* RETROFLAT_OS_OS2 */
retroflat_glut_key( unsigned char key, int x, int y ) {
   g_retroflat_state->retroflat_last_key = key;
}

#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* Still inside RETROFLT_C! */

/* === */

int retroflat_loop( retroflat_loop_iter loop_iter, void* data ) {

#  if defined( RETROFLAT_OS_WASM )

   emscripten_cancel_main_loop();
   emscripten_set_main_loop_arg( loop_iter, data, 0, 0 );

#  elif defined( RETROFLAT_API_ALLEGRO ) || \
   defined( RETROFLAT_API_SDL1 ) || \
   defined( RETROFLAT_API_SDL2 ) || \
   defined( RETROFLAT_API_LIBNDS )

   uint32_t next = 0;

   g_retroflat_state->retroflat_flags |= RETROFLAT_FLAGS_RUNNING;
   do {
      if(
         RETROFLAT_FLAGS_UNLOCK_FPS !=
         (RETROFLAT_FLAGS_UNLOCK_FPS & g_retroflat_state->retroflat_flags) &&
         retroflat_get_ms() < next
      ) {
         /* Sleep/low power for a bid. */
#     ifdef RETROFLAT_API_LIBNDS
         swiWaitForVBlank();
#     endif /* RETROFLAT_API_LIBNDS */
         continue;
      }
      loop_iter( data );
      next = retroflat_get_ms() + retroflat_fps_next();
   } while(
      RETROFLAT_FLAGS_RUNNING == (RETROFLAT_FLAGS_RUNNING & g_retroflat_state->retroflat_flags)
   );

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* Set these to be called from WndProc later. */
   g_retroflat_state->loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;

   /* Handle Windows messages until quit. */
   do {
      g_retroflat_state->msg_retval =
         GetMessage( &(g_retroflat_state->msg), 0, 0, 0 );
      TranslateMessage( &(g_retroflat_state->msg) );
      DispatchMessage( &(g_retroflat_state->msg) );
   } while( 0 < g_retroflat_state->msg_retval );

#  elif defined( RETROFLAT_API_GLUT )

   g_retroflat_state->loop_iter = (retroflat_loop_iter)loop_iter;
   g_retroflat_state->loop_data = (void*)data;
   glutMainLoop();

#  else
#     warning "loop not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   /* This should be set by retroflat_quit(). */
   return g_retroflat_state->retval;
}

/* === */

void retroflat_message( const char* title, const char* format, ... ) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

#  if defined( RETROFLAT_API_ALLEGRO )
   allegro_message( "%s", msg_out );
#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   /* TODO: Use a dialog box? */
   fprintf( stderr, "%s\n", msg_out );
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   /* TODO: Dialog box crashes! */
   /* MessageBox( NULL, msg_out, title, 0 ); */
   error_printf( "%s", msg_out );
#  elif defined( RETROFLAT_API_GLUT )
   /* TODO */
#  else
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   va_end( vargs );
}

#  ifdef RETROFLAT_SCREENSAVER

/* Windows screensaver (.scr) command-line arguments. */

static int retroflat_cli_p( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "p", arg, MAUG_CLI_SIGIL_SZ + 2 ) ) {
      /* The next arg must be the new var. */
   } else {
      g_retroflat_state->parent = (HWND)atoi( arg );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_s( const char* arg, struct RETROFLAT_ARGS* args ) {
   debug_printf( 3, "using screensaver mode..." );
   args->flags |= RETROFLAT_FLAGS_SCREENSAVER;
   return RETROFLAT_OK;
}

#  endif /* RETROFLAT_SCREENSAVER */

#  ifndef RETROFLAT_NO_CLI_SZ

static int retroflat_cli_rfw( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfx", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_w = atoi( arg );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_rfw_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == args->screen_w ) {
      args->screen_w = 320;
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_rfh( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "rfy", arg, MAUG_CLI_SIGIL_SZ + 4 ) ) {
      /* The next arg must be the new var. */
   } else {
      args->screen_h = atoi( arg );
   }
   return RETROFLAT_OK;
}

static int retroflat_cli_rfh_def( const char* arg, struct RETROFLAT_ARGS* args ) {
   if( 0 == args->screen_h ) {
      args->screen_h = 200;
   }
   return RETROFLAT_OK;
}

#  endif /* !RETROFLAT_NO_CLI_SZ */

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
   g_retroflat_state->ms++;
}

void retroflat_on_close_button() {
   g_retroflat_state->close_button = 1;
}
END_OF_FUNCTION( retroflat_on_close_button )

#  endif /* RETROFLAT_API_ALLEGRO */

/* === */

/* Still inside RETROFLT_C! */

int retroflat_init( int argc, char* argv[], struct RETROFLAT_ARGS* args ) {

   /* = Begin Init = */

   int retval = 0;
#  if defined( RETROFLAT_API_ALLEGRO ) && defined( RETROFLAT_OS_DOS )
#     if 0
   union REGS regs;
   struct SREGS sregs;
#     endif
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   WNDCLASS wc = { 0 };
   RECT wr = { 0, 0, 0, 0 };
   DWORD window_style = RETROFLAT_WIN_STYLE;
   DWORD window_style_ex = 0;
   int wx = CW_USEDEFAULT,
      wy = CW_USEDEFAULT;
#  elif defined( RETROFLAT_API_SDL1 )
   const SDL_VideoInfo* info = NULL;
#  elif defined( RETROFLAT_API_LIBNDS )
   int i = 0;
#  elif defined( RETROFLAT_API_GLUT )
   unsigned int glut_init_flags = 0;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  if defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
#     if defined( RETROFLAT_SDL_ICO )
   SDL_Surface* icon = NULL;
#     endif /* RETROFLAT_SDL_ICO */
#  endif /* RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 */
   
   debug_printf( 1, "retroflat: initializing..." );

   /* System sanity checks. */
   assert( 4 == sizeof( uint32_t ) );
   assert( 4 == sizeof( int32_t ) );
   assert( 2 == sizeof( uint16_t ) );
   assert( 2 == sizeof( int16_t ) );
   assert( 1 == sizeof( uint8_t ) );
   assert( 1 == sizeof( int8_t ) );
   assert( NULL != args );

   debug_printf( 1, "retroflat: allocating state (" SIZE_T_FMT " bytes",
      sizeof( struct RETROFLAT_STATE ) );

   g_retroflat_state_h = maug_malloc( 1, sizeof( struct RETROFLAT_STATE ) );
   if( (MAUG_MHANDLE)NULL == g_retroflat_state_h ) {
      retroflat_message( "Error", "Could not allocate global state!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   maug_mlock( g_retroflat_state_h, g_retroflat_state );
   if( (MAUG_MHANDLE)NULL == g_retroflat_state ) {
      retroflat_message( "Error", "Could not lock global state!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }
   maug_mzero( g_retroflat_state, sizeof( struct RETROFLAT_STATE ) );

   debug_printf( 1, "retroflat: parsing args..." );

   /* All platforms: add command-line args based on compile definitons. */

#  ifdef RETROFLAT_SCREENSAVER
	maug_add_arg( MAUG_CLI_SIGIL "p", MAUG_CLI_SIGIL_SZ + 2,
      "Preview screensaver", 0, (maug_cli_cb)retroflat_cli_p, NULL, args );
	maug_add_arg( MAUG_CLI_SIGIL "s", MAUG_CLI_SIGIL_SZ + 2,
      "Launch screensaver", 0, (maug_cli_cb)retroflat_cli_s, NULL, args );
#  endif /* RETROFLAT_SCREENSAVER */

#  ifndef RETROFLAT_NO_CLI_SZ
   maug_add_arg( MAUG_CLI_SIGIL "rfw", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen width.", 0,
      (maug_cli_cb)retroflat_cli_rfw,
      (maug_cli_cb)retroflat_cli_rfw_def, args );
   maug_add_arg( MAUG_CLI_SIGIL "rfh", MAUG_CLI_SIGIL_SZ + 4,
      "Set the screen height.", 0,
      (maug_cli_cb)retroflat_cli_rfh,
      (maug_cli_cb)retroflat_cli_rfh_def, args );
#  endif /* !RETROFLAT_NO_CLI_SZ */

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
      set_gfx_mode(
         GFX_AUTODETECT_WINDOWED, args->screen_w, args->screen_h, 0, 0 )
   ) {
#     endif /* RETROFLAT_OS_DOS */

      allegro_message( "could not setup graphics!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#  define RETROFLAT_COLOR_TABLE_ALLEGRO_INIT( idx, name_l, name_u, r, g, b ) \
   RETROFLAT_COLOR_ ## name_u = makecol( r, g, b );

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

   g_retroflat_state->buffer.b = create_bitmap( args->screen_w, args->screen_h );
   maug_cleanup_if_null( BITMAP*, g_retroflat_state->buffer.b, RETROFLAT_ERROR_GRAPHICS );

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   srand( time( NULL ) );

   if( SDL_Init( SDL_INIT_VIDEO ) ) {
      retroflat_message(
         "Error", "Error initializing SDL: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   info = SDL_GetVideoInfo();
   maug_cleanup_if_null_alloc( SDL_VideoInfo*, info );

#     ifdef RETROFLAT_OPENGL
   if(
      RETROFLAT_FLAGS_UNLOCK_FPS == (RETROFLAT_FLAGS_UNLOCK_FPS & args->flags)
   ) {
      SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );
   }
   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
   /* TODO: Retry with smaller depth buffers if this fails. */
   SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
#     endif /* RETROFLAT_OPENGL */

   g_retroflat_state->screen_v_w = args->screen_w;
   g_retroflat_state->screen_v_h = args->screen_h;

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
      args->screen_w, args->screen_h, info->vfmt->BitsPerPixel,
      SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_ANYFORMAT
#     ifdef RETROFLAT_OPENGL
      | SDL_OPENGL
#     endif /* RETROFLAT_OPENGL */
   );
   maug_cleanup_if_null(
      SDL_Surface*, g_retroflat_state->buffer.surface, RETROFLAT_ERROR_GRAPHICS );

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
      retroflat_message(
         "Error", "Error initializing SDL: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   g_retroflat_state->screen_v_w = args->screen_w;
   g_retroflat_state->screen_v_h = args->screen_h;
   g_retroflat_state->screen_w = args->screen_w;
   g_retroflat_state->screen_h = args->screen_h;

   /* Create the main window. */
   g_retroflat_state->window = SDL_CreateWindow( args->title,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      args->screen_w, args->screen_h, RETROFLAT_WIN_FLAGS );
   maug_cleanup_if_null( SDL_Window*, g_retroflat_state->window, RETROFLAT_ERROR_GRAPHICS );

   /* Create the main renderer. */
   g_retroflat_state->buffer.renderer = SDL_CreateRenderer(
      g_retroflat_state->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE );
   maug_cleanup_if_null(
      SDL_Renderer*, g_retroflat_state->buffer.renderer, RETROFLAT_ERROR_GRAPHICS );

   /* Create the buffer texture. */
   g_retroflat_state->buffer.texture = SDL_CreateTexture( g_retroflat_state->buffer.renderer,
      SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      g_retroflat_state->screen_w, g_retroflat_state->screen_h );

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

   debug_printf( 1, "retroflat: creating window class..." );

   /* Get the *real* size of the window, including titlebar. */
   wr.right = args->screen_w;
   wr.bottom = args->screen_h;
#     ifndef RETROFLAT_API_WINCE
   AdjustWindowRect( &wr, RETROFLAT_WIN_STYLE, FALSE );
#     endif /* !RETROFLAT_API_WINCE */

   g_retroflat_state->screen_w = args->screen_w;
   g_retroflat_state->screen_h = args->screen_h;
   g_retroflat_state->screen_v_w = args->screen_w;
   g_retroflat_state->screen_v_h = args->screen_h;

   memset(
      &g_retroflat_state->buffer, '\0', sizeof( struct RETROFLAT_BITMAP ) );
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
      retroflat_message( "Error", "Could not register window class!" );
      goto cleanup;
   }

   debug_printf( 1, "retroflat: creating window..." );
   
#     ifdef RETROFLAT_SCREENSAVER
   if( (HWND)0 != g_retroflat_state->parent ) {
      /* Shrink the child window into the parent. */
      debug_printf( 1, "retroflat: using window parent: " UPRINTF_U32,
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
      wx = 0;
      wy = 0;
      wr.left = 0;
      wr.top = 0;
      wr.right = GetSystemMetrics( SM_CXSCREEN );
      wr.bottom = GetSystemMetrics( SM_CYSCREEN );
   }
#     endif /* RETROFLAT_SCREENSAVER */

   g_retroflat_state->window = CreateWindowEx(
      window_style_ex, RETROFLAT_WINDOW_CLASS, args->title,
      window_style,
#     ifdef RETROFLAT_API_WINCE
      0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
#     else
      wx, wy,
      wr.right - wr.left, wr.bottom - wr.top,
#     endif /* RETROFLAT_API_WINCE */
#     ifdef RETROFLAT_SCREENSAVER
      g_retroflat_state->parent
#     else
      0
#     endif /* RETROFLAT_SCREENSAVER */
      , 0, g_retroflat_instance, 0
   );

#     ifdef RETROFLAT_API_WINCE
   GetClientRect( g_retroflat_state->window, &wr );
   g_retroflat_state->screen_v_w = wr.right - wr.left;
   g_retroflat_state->screen_v_h = wr.bottom - wr.top;
#     endif /* RETROFLAT_API_WINCE */

   if( !g_retroflat_state->window ) {
      retroflat_message( "Error", "Could not create window!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

   maug_cleanup_if_null_alloc( HWND, g_retroflat_state->window );

   debug_printf( 1, "setting up graphics timer every %d ms...",
      (int)(1000 / RETROFLAT_FPS) );
   if( !SetTimer(
      g_retroflat_state->window, RETROFLAT_WIN_GFX_TIMER_ID, (int)(1000 / RETROFLAT_FPS), NULL )
   ) {
      retroflat_message( "Error", "Could not create graphics timer!" );
      retval = RETROFLAT_ERROR_TIMER;
      goto cleanup;
   }

#ifndef RETROFLAT_OPENGL
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_RGBS )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRSET )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PNSET )
#endif /* !RETROFLAT_OPENGL */

   ShowWindow( g_retroflat_state->window, g_retroflat_cmd_show );

#  elif defined( RETROFLAT_API_LIBNDS )

   /* == Nintendo DS == */

   /* Setup color constants. */
#  define RETROFLAT_COLOR_TABLE_NDS_RGBS_INIT( idx, name_l, name_u, r, g, b ) \
         RETROFLAT_COLOR_ ## name_u = ARGB16( 1, r, g, b );
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
   g_retroflat_state->bg_id = bgInit( 0, BgType_Text8bpp, BgSize_T_256x256, 2, 7 );
   dmaFillWords( 0, g_retroflat_state->bg_tiles, sizeof( g_retroflat_state->bg_tiles ) );
   bgSetPriority( g_retroflat_state->bg_id, 2 );

   /* Put map at base 3, and tiles at base 0. */
   g_retroflat_state->window_id = bgInit( 1, BgType_Text8bpp, BgSize_T_256x256, 3, 0 );
   dmaFillWords( 0, g_retroflat_state->window_tiles, sizeof( g_retroflat_state->window_tiles ) );
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

   g_retroflat_state->screen_v_w = args->screen_w;
   g_retroflat_state->screen_v_h = args->screen_h;

   glutInit( &argc, argv );
   glut_init_flags = GLUT_DEPTH | GLUT_RGBA;
   if(
      RETROFLAT_FLAGS_UNLOCK_FPS != (RETROFLAT_FLAGS_UNLOCK_FPS & args->flags)
   ) {
      glut_init_flags |= GLUT_DOUBLE;
   }
   glutInitDisplayMode( glut_init_flags );
   glutInitWindowSize( g_retroflat_state->screen_v_w, g_retroflat_state->screen_v_h );
   glutCreateWindow( args->title );
   glutIdleFunc( retroflat_glut_idle );
   glutDisplayFunc( retroflat_glut_display );
   glutKeyboardFunc( retroflat_glut_key );

#  else
#     warning "init not implemented"
#  endif  /* RETROFLAT_API_ALLEGRO */

#  if defined( RETROFLAT_SOFT_SHAPES )
   retval = retrosoft_init();
   maug_cleanup_if_not_ok();
#  endif /* RETROFLAT_SOFT_SHAPES */

#  if defined( RETROFLAT_OPENGL )
   retval = retroglu_init_glyph_tex();
   maug_cleanup_if_not_ok();
#  endif /* RETROFLAT_OPENGL */

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown( int retval ) {

#  if defined( RETROFLAT_SOFT_SHAPES )
   retrosoft_shutdown();
#  endif /* RETROFLAT_SOFT_SHAPES */

#  if defined( RETROFLAT_OS_WASM )
   /* Do nothing, start the main loop later. */
   return;
#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   if( RETROFLAT_ERROR_ENGINE != retval ) {
      clear_keybuf();
   }

   retroflat_destroy_bitmap( &g_retroflat_state->buffer );

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

#     ifndef RETROFLAT_API_SDL1
   SDL_DestroyWindow( g_retroflat_state->window );
#     endif /* !RETROFLAT_API_SDL1 */

   SDL_Quit();

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* TODO: Windows shutdown? */

   if( (HDC)NULL != g_retroflat_state->buffer.hdc_b ) {
      /* Return the default object into the HDC. */
      SelectObject( g_retroflat_state->buffer.hdc_b, g_retroflat_state->buffer.old_hbm_b );
      DeleteDC( g_retroflat_state->buffer.hdc_b );
      g_retroflat_state->buffer.hdc_b = (HDC)NULL;

      /* TODO: Destroy buffer bitmap! */
   }

#ifndef RETROFLAT_OPENGL
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_BRRM )
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WIN_PENRM )
#endif /* !RETROFLAT_OPENGL */

#  elif defined( RETROFLAT_API_GLUT )

   /* TODO */

#  else
#     warning "shutdown not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 */

   maug_munlock( g_retroflat_state_h, g_retroflat_state );
   maug_mfree( g_retroflat_state );

}

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
#     warning "set title implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL */

   va_end( vargs );
}

/* === */

uint32_t retroflat_get_ms() {
#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   return g_retroflat_state->ms;

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

#  else
#  warning "get_ms not implemented"
#  endif /* RETROFLAT_API_* */
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
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

   if( NULL == bmp || &g_retroflat_state->buffer == bmp ) {
      /* Special case: Attempting to lock the screen. */
      bmp = (&g_retroflat_state->buffer);

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
         SDL_LockSurface( bmp->surface );
      }

   } else {
      /* Locking a bitmap for pixel drawing. */
      assert( 0 == (RETROFLAT_FLAGS_LOCK & bmp->flags) );
      bmp->flags |= RETROFLAT_FLAGS_LOCK;
      SDL_LockSurface( bmp->surface );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if( NULL != bmp && (&g_retroflat_state->buffer) != bmp ) {
      assert( NULL == bmp->renderer );
      assert( NULL != bmp->surface );
      bmp->renderer = SDL_CreateSoftwareRenderer( bmp->surface );
   } else {
      /* Target is the screen buffer. */
      SDL_SetRenderTarget( g_retroflat_state->buffer.renderer, g_retroflat_state->buffer.texture );
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   if( NULL == bmp ) {
      /* Do nothing. */
      /* TODO: Handle screen locking? */
#     ifdef RETROFLAT_WING
      /* The WinG HDC or whatever should be created already by WndProc. */
      assert( (HDC)NULL != g_retroflat_state->buffer.hdc_b );
#     endif /* RETROFLAT_WING */
      goto cleanup;
   }

   /* Sanity check. */
   assert( (HBITMAP)NULL != bmp->b );
   assert( (HBITMAP)NULL != bmp->mask );
   assert( (HDC)NULL == bmp->hdc_b );
   assert( (HDC)NULL == bmp->hdc_mask );

   /* Create HDC for source bitmap compatible with the buffer. */
   bmp->hdc_b = CreateCompatibleDC( (HDC)NULL );
   maug_cleanup_if_null( HDC, bmp->hdc_b, RETROFLAT_ERROR_BITMAP );

   /* Create HDC for source mask compatible with the buffer. */
   bmp->hdc_mask = CreateCompatibleDC( (HDC)NULL );
   maug_cleanup_if_null( HDC, bmp->hdc_mask, RETROFLAT_ERROR_BITMAP );

   bmp->old_hbm_b = SelectObject( bmp->hdc_b, bmp->b );
   bmp->old_hbm_mask = SelectObject( bmp->hdc_mask, bmp->mask );

cleanup:

#  elif defined( RETROFLAT_API_GLUT )

   /* TODO */

#  else
#     warning "draw lock not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_API_ALLEGRO )

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

   if( NULL == bmp || &g_retroflat_state->buffer == bmp ) {
      /* Special case: Attempting to release the screen. */
      bmp = (&g_retroflat_state->buffer);

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

#     ifdef RETROFLAT_OPENGL
         SDL_GL_SwapBuffers();
#     else
         SDL_Flip( bmp->surface );
#     endif /* RETROFLAT_OPENGL */
      }

   } else {
      /* Releasing a bitmap. */
      assert( RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & bmp->flags) );
      bmp->flags &= ~RETROFLAT_FLAGS_LOCK;
      SDL_UnlockSurface( bmp->surface );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if( NULL == bmp || (&g_retroflat_state->buffer) == bmp ) {
      /* Flip the screen. */
      SDL_SetRenderTarget( g_retroflat_state->buffer.renderer, NULL );
      SDL_RenderCopyEx(
         g_retroflat_state->buffer.renderer, g_retroflat_state->buffer.texture, NULL, NULL, 0, NULL, 0 );
      SDL_RenderPresent( g_retroflat_state->buffer.renderer );
   } else {
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
   }

cleanup:
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   if( NULL == bmp  ) {
      /* Trigger a screen refresh if this was a screen lock. */
      if( (HWND)NULL != g_retroflat_state->window ) {
#     ifdef RETROFLAT_OPENGL
         SwapBuffers( g_retroflat_state->hdc_win );
#     else
         InvalidateRect( g_retroflat_state->window, 0, TRUE );
#     endif /* RETROFLAT_OPENGL */
      }
      goto cleanup;
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

#  elif defined( RETROFLAT_API_GLUT )

   /* == GLUT == */

   if( NULL == bmp || &g_retroflat_state->buffer == bmp ) {
      /* Special case: Attempting to release the screen. */
      glutSwapBuffers();
   }

#  else
#     warning "draw release not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
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
   maug_cleanup_if_null( HBITMAP, bmp_out->mask, RETROFLAT_ERROR_BITMAP );

   lock_ret = retroflat_draw_lock( bmp_out );
   if( RETROFLAT_OK != lock_ret ) {
      retval = RETROFLAT_ERROR_BITMAP;
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

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   int retval = MERROR_OK;
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_API_SDL1 )
   SDL_Surface* tmp_surface = NULL;
#  elif defined( RETROFLAT_API_WIN16 ) || defined (RETROFLAT_API_WIN32 )
#     if defined( RETROFLAT_API_WIN16 )
   HDC hdc_win = (HDC)NULL;
   char* buf = NULL;
   BITMAPINFO* bmi = NULL;
   FILE* bmp_file = NULL;
   long int i, x, y, w, h, bpp, offset, sz, read;
#     elif defined( RETROFLAT_API_WIN32 )
   BITMAP bm;
#     endif /* RETROFLAT_API_WIN32 */
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   assert( NULL != bmp_out );

   /* Build the path to the bitmap. */
   memset( filename_path, '\0', RETROFLAT_PATH_MAX + 1 );
   maug_snprintf( filename_path, RETROFLAT_PATH_MAX, "%s%c%s.%s",
      g_retroflat_state->assets_path, RETROFLAT_PATH_SEP,
      filename, RETROFLAT_BITMAP_EXT );

   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

#  ifdef RETROFLAT_OPENGL

   /* TODO */

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

   tmp_surface = SDL_LoadBMP( filename_path ); /* Free stream on close. */
   /* TODO: maug_cleanup_if_null()? */
   if( NULL == tmp_surface ) {
      retroflat_message(
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = 0;
      goto cleanup;
   }

   debug_printf( 1, "loaded bitmap: %d x %d", tmp_surface->w, tmp_surface->h );

   bmp_out->surface = SDL_DisplayFormat( tmp_surface );
   if( NULL == bmp_out->surface ) {
      retroflat_message(
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   debug_printf( 1, "converted bitmap: %d x %d",
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

   bmp_out->renderer = NULL;
   
   bmp_out->surface = SDL_LoadBMP( filename_path );

   /* TODO: maug_cleanup_if_null()? */
   if( NULL == bmp_out->surface ) {
      retroflat_message(
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
      SDL_MapRGB( bmp_out->surface->format,
         RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );

   bmp_out->texture =
      SDL_CreateTextureFromSurface( g_retroflat_state->buffer.renderer, bmp_out->surface );
   /* TODO: maug_cleanup_if_null()? */
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

cleanup:

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

#     if defined( RETROFLAT_API_WIN16 )

   /* Win16 has a bunch of extra involved steps for getting a bitmap from
    * disk. These cause a crash in Win32.
    */

   /* == Win16 == */

   /* Load the bitmap file from disk. */
   bmp_file = fopen( filename_path, "rb" );
   /* TODO: maug_cleanup_if_null()? */
   if( NULL == bmp_file ) {
      retroflat_message(
         "Error", "Could not load bitmap: %s", filename_path );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }
   fseek( bmp_file, 0, SEEK_END );
   sz = ftell( bmp_file );
   fseek( bmp_file, 0, SEEK_SET );

   buf = calloc( sz, 1 );
   maug_cleanup_if_null_alloc( char*, buf );

   read = fread( buf, 1, sz, bmp_file );
   assert( read == sz );

   bmi = (BITMAPINFO*)&(buf[sizeof( BITMAPFILEHEADER )]);

   bpp = retroflat_bmp_int( unsigned short, buf, 28 );
   offset = retroflat_bmp_int( unsigned long, buf, 10 );

   assert( 0 < bmi->bmiHeader.biWidth );
   assert( 0 < bmi->bmiHeader.biHeight );
   assert( 0 == bmi->bmiHeader.biWidth % 8 );
   assert( 0 == bmi->bmiHeader.biHeight % 8 );

   hdc_win = GetDC( g_retroflat_state->window );
   bmp_out->b = CreateCompatibleBitmap( hdc_win,
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );
   maug_cleanup_if_null( HBITMAP, bmp_out->b, RETROFLAT_ERROR_BITMAP );

   SetDIBits( hdc_win, bmp_out->b, 0,
      bmi->bmiHeader.biHeight, &(buf[offset]), bmi,
      DIB_RGB_COLORS );

   retval = retroflat_bitmap_win_transparency( bmp_out,
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );

#     else

   /* Win32 greatly simplifies the loading portion. */

   /* == Win32 == */

#        ifdef RETROFLAT_API_WINCE
   bmp_out->b = SHLoadDIBitmap( filename_path );
#        else
   bmp_out->b = LoadImage(
      NULL, filename_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
#        endif /* RETROFLAT_API_WINCE */
   /* TODO: Error with bitmap path. */
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
      ReleaseDC( g_retroflat_state->window, hdc_win );
   }
#     endif /* RETROFLAT_API_WIN16 */

#  elif defined( RETROFLAT_API_GLUT )

   /* TODO */

#  else
#     warning "load bitmap not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   int w, int h, struct RETROFLAT_BITMAP* bmp_out
) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_API_ALLEGRO )
   
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
   SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
      SDL_MapRGB( bmp_out->surface->format,
         RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );

cleanup:
#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

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

   HDC hdc_win = (HDC)NULL;

   hdc_win = GetDC( g_retroflat_state->window );
   bmp_out->b = CreateCompatibleBitmap( hdc_win, w, h );
   maug_cleanup_if_null( HBITMAP, bmp_out->b, RETROFLAT_ERROR_BITMAP );

cleanup:
#  elif defined( RETROFLAT_API_GLUT )

   /* TODO */

#  else
#     warning "create bitmap not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   return retval;
}

/* === */

#ifdef RETROFLAT_XPM

MERROR_RETVAL retroflat_load_xpm(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out
) {
   MERROR_RETVAL retval = MERROR_OK;
   RETROFLAT_COLOR color;
   int xpm_idx = 0,
      x = 0,
      y = 0,
      bmp_w = 0,
      bmp_h = 0,
      bmp_colors,
      bmp_bypp;

   /* Hunt for the requested XPM in the compiled directory. */
   while( '\0' != gc_xpm_filenames[xpm_idx][0] ) {
      if( 0 == strcmp( filename, gc_xpm_filenames[xpm_idx] ) ) {
         goto xpm_found;
      }
      xpm_idx++;
   }

   retval = RETROFLAT_ERROR_BITMAP;
   goto cleanup;

xpm_found:

   debug_printf( 2, "found xpm: %s", gc_xpm_filenames[xpm_idx] );

   /* Load XPM and draw it to a new bitmap. */

   sscanf( gc_xpm_data[xpm_idx][0], "%d %d %d %d",
      &bmp_w, &bmp_h, &bmp_colors, &bmp_bypp );

   assert( 16 == bmp_colors );
   assert( 1 == bmp_bypp );

   retval = retroflat_create_bitmap( bmp_w, bmp_h, bmp_out );
   if( MERROR_OK != retval ) {
      goto cleanup;
   }

   debug_printf( 1, "created empty canvas: %dx%d", bmp_w, bmp_h );

   /* TODO: Draw XPM pixels to canvas. */

   retroflat_draw_lock( bmp_out );

   for( y = 0 ; bmp_h > y ; y++ ) {
      for( x = 0 ; bmp_w > x ; x++ ) {
         switch( gc_xpm_data[xpm_idx][17 + y][x] ) {
         case ' ':
            /* Transparent. */
            continue;
         case '.':
            color = RETROFLAT_COLOR_DARKBLUE;
            break;
         case 'X':
            color = RETROFLAT_COLOR_DARKGREEN;
            break;
         case 'o':
            color = RETROFLAT_COLOR_TEAL;
            break;
         case 'O':
            color = RETROFLAT_COLOR_DARKRED;
            break;
         case '+':
            color = RETROFLAT_COLOR_VIOLET;
            break;
         case '@':
            color = RETROFLAT_COLOR_BROWN;
            break;
         case '#':
            color = RETROFLAT_COLOR_GRAY;
            break;
         case '$':
            color = RETROFLAT_COLOR_DARKGRAY;
            break;
         case '%':
            color = RETROFLAT_COLOR_BLUE;
            break;
         case '&':
            color = RETROFLAT_COLOR_GREEN;
            break;
         case '*':
            color = RETROFLAT_COLOR_CYAN;
            break;
         case '=':
            color = RETROFLAT_COLOR_RED;
            break;
         case '-':
            color = RETROFLAT_COLOR_MAGENTA;
            break;
         case ';':
            color = RETROFLAT_COLOR_YELLOW;
            break;
         case ':':
            color = RETROFLAT_COLOR_WHITE;
            break;
         }

         retroflat_px( bmp_out, color, x, y, 0 );
      }
   }

   retroflat_draw_release( bmp_out );

cleanup:
   return retval;
}

/* === */

#endif /* RETROFLAT_XPM */

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

#  elif defined( RETROFLAT_API_GLUT )

   /* TODO */

#  else
#     warning "destroy bitmap not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int w, int h
) {
#  if defined( RETROFLAT_API_SDL1 )
   int retval = 0;
   SDL_Rect src_rect;
   SDL_Rect dest_rect;
#  elif defined( RETROFLAT_API_SDL2 )
   int retval = 0;
   int lock_ret = 0;
   int locked_target_internal = 0;
   SDL_Rect src_rect = { s_x, s_y, w, h };
   SDL_Rect dest_rect = { d_x, d_y, w, h };
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int lock_ret = 0;
   int locked_src_internal = 0;
   int locked_target_internal = 0;
#  endif /* RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_retroflat_state->buffer);
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

   assert( !retroflat_bitmap_locked( src ) );

   src_rect.x = s_x;
   src_rect.y = s_y;
   src_rect.w = w;
   src_rect.h = h;
   dest_rect.x = d_x;
   dest_rect.y = d_y;
   dest_rect.w = w;
   dest_rect.h = h;

#     ifdef RETROFLAT_API_SDL1
   assert( !retroflat_bitmap_locked( target ) );
   retval = 
      SDL_BlitSurface( src->surface, &src_rect, target->surface, &dest_rect );
   if( 0 != retval ) {
      error_printf( "could not blit surface: %s", SDL_GetError() );
   }
#     else
   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   retval = SDL_RenderCopy(
      target->renderer, src->texture, &src_rect, &dest_rect );
   if( 0 != retval ) {
      error_printf( "could not blit surface: %s", SDL_GetError() );
   }

cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }
#     endif /* !RETROFLAT_API_SDL1 */

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   assert( (HBITMAP)NULL != target->b );
   assert( (HBITMAP)NULL != src->b );

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

#  elif defined( RETROFLAT_API_GLUT )

   /* TODO */

#  else
#     warning "blit bitmap not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */
   return;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int x, int y, uint8_t flags
) {
   int locked_target_internal = 0;
   int lock_ret = 0;
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_API_SDL1 )
   int offset = 0;
   uint8_t* px_1 = NULL;
   uint16_t* px_2 = NULL;
   uint32_t* px_4 = NULL;
#  endif /* RETROFLAT_API_SDL1 */

   if( NULL == target ) {
      target = &(g_retroflat_state->buffer);
   }

   if(
      x < 0 || x >= retroflat_screen_w() ||
      y < 0 || y >= retroflat_screen_h()
   ) {
      return;
   }

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

#  if defined( RETROFLAT_OPENGL )

   /* TODO */

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   putpixel( target->b, x, y, color );

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

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

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   SDL_SetRenderDrawColor(
      target->renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderDrawPoint( target->renderer, x, y );

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   SetPixel( target->hdc_b, x, y, gc_retroflat_win_rgbs[color] );

#  elif defined( RETROFLAT_API_LIBNDS )

   /* == Nintendo DS == */

   uint16_t* px_ptr = NULL;

   px_ptr = bgGetGfxPtr( g_retroflat_state->px_id );
   px_ptr[(y * 256) + x] = color;

#  elif defined( RETROFLAT_API_GLUT )

   /* TODO */

#  else
#     warning "px not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }
}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags
) {
#if defined( RETROFLAT_OPENGL )
   float aspect_ratio = 0,
      screen_x = 0,
      screen_y = 0,
      screen_w = 0,
      screen_h = 0;
#elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   SDL_Rect area;
   int locked_target_internal = 0;
   int lock_ret = 0;
#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HBRUSH old_brush = (HBRUSH)NULL;
   int lock_ret = 0,
      locked_target_internal = 0;
   HPEN old_pen = (HPEN)NULL;
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_retroflat_state->buffer);
   }

#  if defined( RETROFLAT_OPENGL )

   retroflat_opengl_push( x, y, screen_x, screen_y, aspect_ratio );
   retroflat_opengl_whf( w, h, screen_w, screen_h, aspect_ratio );

   glBegin( GL_TRIANGLES );
   glColor3fv( color );
   glVertex3f( screen_x,            screen_y,            RETROFLAT_GL_Z );
   glVertex3f( screen_x,            screen_y - screen_h, RETROFLAT_GL_Z );
   glVertex3f( screen_x + screen_w, screen_y - screen_h, RETROFLAT_GL_Z );

   glVertex3f( screen_x + screen_w, screen_y - screen_h, RETROFLAT_GL_Z );
   glVertex3f( screen_x + screen_w, screen_y,            RETROFLAT_GL_Z );
   glVertex3f( screen_x,            screen_y,            RETROFLAT_GL_Z );
   glEnd();
   
   retroflat_opengl_pop();

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );
   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      rectfill( target->b, x, y, x + w, y + h, color );
   } else {
      rect( target->b, x, y, x + w, y + h, color );
   }

#  elif defined( RETROFLAT_API_SDL1 )

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   area.x = x;
   area.y = y;
   area.w = w;
   area.h = h;

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {

      /* Full-screen blank requested. */
      SDL_FillRect( target->surface, &area,
         SDL_MapRGB( target->surface->format, color->r, color->g, color->b ) );

   } else {
      /* Top */
      retroflat_line( target, color, x, y, x + w, y, 0 );
      /* Right */
      retroflat_line( target, color, x + w, y, x + w, y + h, 0 );
      /* Bottom */
      retroflat_line( target, color, x, y + h, x + w, y + h, 0 );
      /* Left */
      retroflat_line( target, color, x, y, x, y + h, 0 );
   }

cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  elif defined( RETROFLAT_API_SDL2 )

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

   assert( (HBITMAP)NULL != target->b );

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
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_SOFT_SHAPES )
#  elif defined( RETROFLAT_API_SDL2 )
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
      target = &(g_retroflat_state->buffer);
   }

#  if defined( RETROFLAT_OPENGL )

   /* TODO */

#  elif defined( RETROFLAT_SOFT_SHAPES )

   retrosoft_line( target, color, x1, y1, x2, y2, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );
   line( target->b, x1, y1, x2, y2, color );

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

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
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   int x, int y, int w, int h, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_SOFT_SHAPES )
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HPEN old_pen = (HPEN)NULL;
   HBRUSH old_brush = (HBRUSH)NULL;
   int lock_ret = 0,
      locked_target_internal = 0;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_retroflat_state->buffer);
   }

#  if defined( RETROFLAT_OPENGL )

   /* TODO */

#  elif defined( RETROFLAT_SOFT_SHAPES )

   retrosoft_ellipse( target, color, x, y, w, h, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      ellipsefill( target->b, x + (w / 2), y + (h / 2), w / 2, h / 2, color );
   } else {
      ellipse( target->b, x + (w / 2), y + (h / 2), w / 2, h / 2, color );
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

void retroflat_string_sz(
   struct RETROFLAT_BITMAP* target, const char* str, int str_sz,
   const char* font_str, int* w_out, int* h_out, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_SOFT_SHAPES )
#  elif defined( RETROFLAT_API_ALLEGRO )
   FONT* font_data = NULL;
   int font_loaded = 0;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int lock_ret = 0,
      locked_target_internal = 0;
   SIZE sz;
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_retroflat_state->buffer);
   }

#  if defined( RETROFLAT_OPENGL )

   /* TODO */

#  elif defined( RETROFLAT_SOFT_SHAPES )

   retrosoft_string_sz( target, str, str_sz, font_str, w_out, h_out, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

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
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   const char* str, int str_sz, const char* font_str, int x_orig, int y_orig,
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
   int lock_ret = 0,
      locked_target_internal = 0;
   RECT rect;
   SIZE sz;
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_retroflat_state->buffer);
   }

   if( 0 == str_sz ) {
      str_sz = strlen( str );
   }

#  if defined( RETROFLAT_OPENGL )

   retroflat_opengl_push( x_orig, y_orig, screen_x, screen_y, aspect_ratio );

   retroglu_string(
      screen_x, screen_y, 0, color, str, str_sz, font_str, flags );

   retroflat_opengl_pop();

#  elif defined( RETROFLAT_SOFT_SHAPES )

   retrosoft_string(
      target, color, str, str_sz, font_str, x_orig, y_orig, flags );

#  elif defined( RETROFLAT_API_ALLEGRO )

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
#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   int eres = 0;
   SDL_Event event;
#  endif /* RETROFLAT_API_ALLEGRO && RETROFLAT_OS_DOS */
   int key_out = 0;

   assert( NULL != input );

#  ifdef RETROFLAT_API_ALLEGRO

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
      return (readkey() >> 8);
   }

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

   SDL_PollEvent( &event );

   /* TODO: Handle SDL window close. */

   if( SDL_QUIT == event.type ) {
      retroflat_quit( 0 );
   } else if( SDL_KEYDOWN == event.type ) {
      key_out = event.key.keysym.sym;

      /* Flush key buffer to improve responsiveness. */
      if(
         RETROFLAT_FLAGS_KEY_REPEAT !=
         (RETROFLAT_FLAGS_KEY_REPEAT & g_retroflat_state->retroflat_flags)
      ) {
         while( (eres = SDL_PollEvent( &event )) );
      }

   } else if( SDL_MOUSEBUTTONUP == event.type ) {
      /* Stop dragging. */
      g_retroflat_state->mouse_state = 0;

   } else if( 0 != g_retroflat_state->mouse_state ) {

      /* Update coordinates and keep dragging. */

      SDL_GetMouseState( &(input->mouse_x), &(input->mouse_y) );
      key_out = g_retroflat_state->mouse_state;

   } else if( SDL_MOUSEBUTTONDOWN == event.type ) {

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

   if( g_retroflat_state->last_key ) {
      /* Return g_retroflat_state->last_key, which is set in WndProc when a keypress msg is
      * received.
      */
      key_out = g_retroflat_state->last_key;

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

   key_out = g_retroflat_state->retroflat_last_key;
   g_retroflat_state->retroflat_last_key = 0;

#  else
#     warning "poll input not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   return key_out;
}

/* === */

MERROR_RETVAL retroflat_config_open( RETROFLAT_CONFIG* config ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_CONFIG_USE_FILE )

   debug_printf( 1, "opening config file %s...",
      g_retroflat_state->config_path );

   *config = fopen( g_retroflat_state->config_path, "r" );
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
#     warning "config close not implemented"
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
#     warning "config close not implemented"
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

   retval = GetPrivateProfileString(
      sect_name, key_name, default_out, buffer_out, buffer_out_sz_max,
      g_retroflat_state->config_path );

#  elif defined( RETROFLAT_API_WIN32 )

   /* == Win32 (Registry) == */

   /* TODO */

#  else
#     warning "config read not implemented"
#  endif

   return retval;
}

#else /* End of RETROFLT_C */

   extern struct RETROFLAT_STATE* g_retroflat_state;
#     if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   extern HINSTANCE            g_retroflat_instance;
   extern int                  g_retroflat_cmd_show;
#     endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  include <uprintf.h>

#  if defined( RETROFLAT_SOFT_SHAPES ) && !defined( MAUG_NO_AUTO_C )
#     include <retrofp.h>
#     include <retrosft.h>
#  endif /* RETROFLAT_SOFT_SHAPES */

#  if defined( RETROFLAT_OPENGL ) && !defined( MAUG_NO_AUTO_C )
#     include <retroglu.h>
#  endif /* RETROFLAT_OPENGL */

#endif /* RETROFLT_C */

/*! \} */ /* maug_retroflt */

#endif /* RETROFLT_H */

