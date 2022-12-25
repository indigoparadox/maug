
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
 *       
 *          / * Zero out the data holder. * /
 *          memset( &data, '\0', sizeof( struct EXAMPLE_DATA ) );
 *       
 *          / * Tell retroflat where to find graphical assets. * /
 *          retroflat_set_assets_path( "assets" );
 *       
 *          / * === Setup === * /
 *          / * Create a window titled "Example Program", 320x200 pixels
 *            * large.
 *            * /
 *          retval = retroflat_init( "Example Program", 320, 200, argc, argv );
 *
 *          / * Make sure setup completed successfully! * /
 *          if( RETROFLAT_OK != retval ) {
 *             retroflat_message( "Error", "Could not initialize." );
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
 * \{
 * \page maug_retroflt_makefile_page RetroFlat Project Makefiles
 *
 * # Win16 (OpenWatcom)
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

/**
 * \addtogroup maug_retroflt_retval RetroFlat API Return Values
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

/*! \} */

/**
 * \addtogroup maug_retroflt_drawing RetroFlat Drawing API
 * \{
 */

/**
 * \brief Flag for retroflat_rect() indicating drawn shape should be filled.
 */
#define RETROFLAT_FLAGS_FILL     0x01

/*! \} */

/**
 * \brief Flag for g_retroflat_flags indicating that retroflat_loop() should
 *        continue executing.
 * \warning This flag is not used on all platforms! It should only be removed
 *          using retroflat_quit().
 */
#define RETROFLAT_FLAGS_RUNNING  0x01

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
 *        retroflat_load_bitmap().
 */
#define RETROFLAT_BITMAP_EXT "bmp"

/* Transparency background color: black by default, to match Allegro. */
#  ifndef RETROFLAT_TXP_R
/**
 * \brief Compiler-define-overridable constant indicating the Red value of the
 *        transparency color on platforms that support it (mainly Win16/SDL).
 */
#  define RETROFLAT_TXP_R 0x00
#  endif /* !RETROFLAT_TXP_R */

#  ifndef RETROFLAT_TXP_G
/**
 * \brief Compiler-define-overridable constant indicating the Green value of the
 *        transparency color on platforms that support it (mainly Win16/SDL).
 */
#  define RETROFLAT_TXP_G 0x00
#  endif /* !RETROFLAT_TXP_G */

#  ifndef RETROFLAT_TXP_B
/**
 * \brief Compiler-define-overridable constant indicating the Blue value of the
 *        transparency color on platforms that support it (mainly Win16/SDL).
 */
#  define RETROFLAT_TXP_B 0x00
#  endif /* !RETROFLAT_TXP_B */

/* Convenience macro for auto-locking inside of draw functions. */
#  define retroflat_internal_autolock_bitmap( bmp, lock_ret, lock_auto ) \
   if( !retroflat_bitmap_locked( bmp ) ) { \
      lock_ret = retroflat_draw_lock( bmp ); \
      if( RETROFLAT_OK != lock_ret ) { \
         goto cleanup; \
      } \
      lock_auto = 1; \
   }

/*! \} */

/**
 * \brief Prototype for the main loop function passed to retroflat_loop().
 */
typedef void (*retroflat_loop_iter)(void* data);

/**
 * \addtogroup maug_retroflt_input RetroFlat Input API
 * \{
 */

struct RETROFLAT_INPUT {
   int mouse_x;
   int mouse_y;
};

/*! \} */

/**
 * \addtogroup maug_retroflt_drawing
 * \{
 */

#ifndef RETROFLAT_LINE_THICKNESS
/*! \brief Line drawing thickness (only works on some platforms). */
#define RETROFLAT_LINE_THICKNESS 1
#endif /* !RETROFLAT_LINE_THICKNESS */

/*! \} "*/

#ifndef RETROFLAT_FPS
/* TODO: Support Allegro! */
/*! \brief Target FPS. */
#define RETROFLAT_FPS 15
#endif /* !RETROFLAT_FPS */

#ifndef RETROFLAT_WINDOW_CLASS
/*! \brief Unique window class to use on some platforms (e.g. Win32). */
#define RETROFLAT_WINDOW_CLASS "RetroFlatWindowClass"
#endif /* !RETROFLAT_WINDOW_CLASS */

#ifndef RETROFLAT_WIN_GFX_TIMER_ID
/**
 * \brief Unique ID for the timer that execute graphics ticks in Win16/Win32.
 */
#define RETROFLAT_WIN_GFX_TIMER_ID 6001
#endif /* !RETROFLAT_WIN_GFX_TIMER_ID */

#ifndef RETROFLAT_MSG_MAX
#define RETROFLAT_MSG_MAX 4096
#endif /* !RETROFLAT_MSG_MAX */

#ifndef RETROFLAT_PATH_MAX
#define RETROFLAT_PATH_MAX 256
#endif /* !RETROFLAT_PATH_MAX */

#ifndef RETROFLAT_WIN_STYLE
#define RETROFLAT_WIN_STYLE (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME)
#endif /* !RETROFLAT_WIN_STYLE */

/* === Platform-Specific Includes and Defines === */

#if defined( RETROFLAT_API_ALLEGRO )

/* == Allegro == */

#  include <allegro.h>

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

/* TODO: Handle retval. */
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

#elif defined( RETROFLAT_API_SDL )

#  include <SDL.h>

struct RETROFLAT_BITMAP {
   unsigned char flags;
   SDL_Surface* surface;
   SDL_Texture* texture;
   SDL_Renderer* renderer;
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
#  define RETROFLAT_KEY_ESC	SDLK_ESC
#  define RETROFLAT_KEY_ENTER	SDLK_ENTER
#  define RETROFLAT_KEY_HOME	SDLK_HOME
#  define RETROFLAT_KEY_END	SDLK_END

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->surface)
#  define retroflat_bitmap_locked( bmp ) (NULL != (bmp)->renderer)
#  define retroflat_screen_w() g_screen_v_w
#  define retroflat_screen_h() g_screen_v_h

/* TODO: Handle retval. */
#  define retroflat_quit( retval ) \
   g_retroflat_flags &= ~RETROFLAT_FLAGS_RUNNING;

#define END_OF_MAIN()

#define RETROFLAT_COLOR const SDL_Color*

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

#define RETROFLAT_COLOR_BLACK    (&gc_black)
#define RETROFLAT_COLOR_GRAY     (&gc_gray)
#define RETROFLAT_COLOR_WHITE    (&gc_white)

#elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

/* == Win16/Win32 == */

#  include <windows.h>
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

#  define RETROFLAT_COLOR_BLACK        RGB(0,   0,   0)
#  define RETROFLAT_COLOR_DARKBLUE     RGB(0, 0, 170)
#  define RETROFLAT_COLOR_DARKGREEN    RGB(0, 170, 0)
#  define RETROFLAT_COLOR_TEAL         RGB(0, 170, 170)
#  define RETROFLAT_COLOR_DARKRED      RGB(170, 0, 0)
#  define RETROFLAT_COLOR_VIOLET       RGB(170, 0, 170)
#  define RETROFLAT_COLOR_BROWN        RGB(170, 85, 0)
#  define RETROFLAT_COLOR_GRAY         RGB(170, 170, 170)
#  define RETROFLAT_COLOR_DARKGRAY     RGB(85, 85, 85)
#  define RETROFLAT_COLOR_BLUE         RGB(85, 85, 255)
#  define RETROFLAT_COLOR_GREEN        RGB(85, 255, 85)
#  define RETROFLAT_COLOR_CYAN         RGB(85, 255, 255)
#  define RETROFLAT_COLOR_RED          RGB(255, 85, 85)
#  define RETROFLAT_COLOR_MAGENTA      RGB(255, 85, 255)
#  define RETROFLAT_COLOR_YELLOW       RGB(255, 255, 85)
#  define RETROFLAT_COLOR_WHITE        RGB(255, 255, 255)

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)
#  define retroflat_bitmap_locked( bmp ) ((HDC)NULL != (bmp)->hdc_b)
#  define retroflat_screen_w() g_screen_v_w
#  define retroflat_screen_h() g_screen_v_h
#  define retroflat_quit( retval ) PostQuitMessage( retval );

/* Create a brush and set it to the target HDC. */
#  define retroflat_win_setup_brush( brush, old_brush, target, flags ) \
   if( RETROFLAT_FLAGS_FILL != (RETROFLAT_FLAGS_FILL & flags) ) { \
      old_brush = SelectObject( target->hdc_b, GetStockObject( NULL_BRUSH ) ); \
   } else { \
      brush = CreateSolidBrush( color ); \
      if( (HBRUSH)NULL == brush ) { \
         goto cleanup; \
      } \
      old_brush = SelectObject( target->hdc_b, brush ); \
   }

/* Create a pen and set it to the target HDC. */
#  define retroflat_win_setup_pen( pen, old_pen, target, flags ) \
   pen = CreatePen( PS_SOLID, RETROFLAT_LINE_THICKNESS, color ); \
   if( (HPEN)NULL == pen ) { \
      goto cleanup; \
   } \
   old_pen = SelectObject( target->hdc_b, pen );

#  define retroflat_win_cleanup_brush( brush, old_brush, target ) \
   if( (HBRUSH)NULL != old_brush ) { \
      SelectObject( target->hdc_b, old_brush ); \
   } \
   if( (HBRUSH)NULL != brush ) { \
      DeleteObject( brush ); \
   }

#  define retroflat_win_cleanup_pen( pen, old_pen, target ) \
   if( (HPEN)NULL != pen ) { \
      SelectObject( target->hdc_b, old_pen ); \
      DeleteObject( pen ); \
   }

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
#  define RETROFLAT_KEY_ESC	VK_ESC
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
      g_instance = hInstance; \
      g_cmd_show = nCmdShow; \
      return main( __argc, __argv ); \
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

/*! \} */

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
 * \addtogroup maug_retroflt_color RetroFlat Colors
 * \brief Color definitions RetroFlat is aware of, for use with the
 *        \ref maug_retroflt_drawing.
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

/*! \} */

#endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#ifdef RETROFLAT_OS_DOS
#  define RETROFLAT_PATH_SEP '\\'
#else
/*! \brief The valid path separator on the target platform. */
#  define RETROFLAT_PATH_SEP '/'
#endif /* RETROFLAT_OS_DOS */

/*! \brief Maximum size of the assets path, to allow room for appending. */
#define RETROFLAT_ASSETS_PATH_MAX (RETROFLAT_PATH_MAX / 2)

/* === Translation Module === */

/* Declare the prototypes so that internal functions can call each other. */

/**
 * \brief This should be called once in the main body of the program in order
 *        to enter the main loop. The main loop will continuously call
 *        loop_iter with data as an argument until retroflat_quit() is called.
 */
int retroflat_loop( retroflat_loop_iter iter, void* data );

void retroflat_message( const char* title, const char* format, ... );

/**
 * \brief Initialize RetroFlat and its underlying layers. This should be
 *        called once at the beginning of the program and should quit if
 *        the return value indicates any failures.
 * \param title Title to set for the main program Window if applicable on the
 *        target platform.
 * \param int screen_w Desired screen or window width in pixels.
 * \param int screen_h Desired screen or window height in pixels.
 * \return ::RETROFLAT_OK if there were no problems or other
 *         \ref maug_retroflt_retval if there were.
 */
int retroflat_init(
   const char* title, int screen_w, int screen_h, int argc, char* argv[] );

/**
 * \brief Deinitialize RetroFlat and its underlying layers. This should be
 *        called once at the end of the program, after retroflat_loop().
 * \param retval Return value from retroflat_init(), so we know what layers
 *        are in what state.
 */
void retroflat_shutdown( int retval );

/**
 * \brief Set the assets path that will be prepended to all calls to
 *        retroflat_load_bitmap().
 */
void retroflat_set_assets_path( const char* path );

/**
 * \addtogroup maug_retroflt_bitmap
 * \{
 */

/**
 * \brief Load a bitmap into the given ::RETROFLAT_BITMAP structure if
 *        it is available. Bitmaps are subject to the limitations enumerated in
 *        \ref maug_retroflt_bitmap.
 * \param filename Filename of the bitmap under retroflat_set_assets_path(),
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

/*! \} */

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
 * \param target Pointer to the ::RETROFLAT_BITMAP to draw onto.
 * \param color \ref maug_retroflt_color in which to draw.
 * \param flags Flags to control drawing. The following flags apply:
 *        ::RETROFLAT_FLAGS_FILL
 */
void retroflat_rect(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags );

/**
 * \brief Draw an ellipse onto the target ::RETROFLAT_BITMAP.
 * \param target Pointer to the ::RETROFLAT_BITMAP to draw onto.
 * \param color \ref maug_retroflt_color in which to draw.
 * \param flags Flags to control drawing. The following flags apply:
 *        ::RETROFLAT_FLAGS_FILL
 */
void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags );

void retroflat_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, unsigned char flags );

/*! \} */

/**
 * \addtogroup maug_retroflt_input
 * \{
 */

int retroflat_poll_input();

/*! \} */

#ifdef RETROFLT_C

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <stdarg.h>

void* g_loop_data = NULL;
int g_retval = 0;

#  if defined( RETROFLAT_API_SDL )

SDL_Window* g_window = NULL;
int g_screen_w = 0;
int g_screen_h = 0;
int g_screen_v_w = 0;
int g_screen_v_h = 0;
int g_mouse_state = 0;

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

/* Windows-specific global handles for the window/instance. */
HINSTANCE g_instance;
HWND g_window;
MSG g_msg;
int g_msg_retval = 0;
int g_screen_w = 0;
int g_screen_h = 0;
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

#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

/* === Globals === */

char g_retroflat_assets_path[RETROFLAT_ASSETS_PATH_MAX + 1];
struct RETROFLAT_BITMAP g_screen;
unsigned char g_retroflat_flags = 0;

/* === Function Definitions === */

#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

static LRESULT CALLBACK WndProc(
   HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
) {
   PAINTSTRUCT ps;
   HDC hdcScreen;
   HDC hdcBuffer;
   BITMAP srcBitmap;
   HBITMAP oldHbm;

   switch( message ) {
      case WM_CREATE:
         break;

      case WM_PAINT:

         /* Create HDC for screen. */
         hdcScreen = BeginPaint( hWnd, &ps );
         if( (HDC)NULL == hdcScreen ) {
            break;
         }

         if( !retroflat_bitmap_ok( &g_screen ) ) {
            g_screen.b = CreateCompatibleBitmap( hdcScreen,
               g_screen_v_w, g_screen_v_h );
         }
         if( (HBITMAP)NULL == g_screen.b ) {
            break;
         }

         /* Create a new HDC for buffer and select buffer into it. */
         hdcBuffer = CreateCompatibleDC( hdcScreen );
         if( (HDC)NULL == hdcBuffer ) {
            break;
         }
         oldHbm = SelectObject( hdcBuffer, g_screen.b );

         /* Load parameters of the buffer into info object (srcBitmap). */
         GetObject( g_screen.b, sizeof( BITMAP ), &srcBitmap );

         StretchBlt(
            hdcScreen,
            0, 0,
            g_screen_w, g_screen_h,
            hdcBuffer,
            0, 0,
            srcBitmap.bmWidth,
            srcBitmap.bmHeight,
            SRCCOPY
         );

         /* Return the default object into the HDC. */
         SelectObject( hdcScreen, oldHbm );

         DeleteDC( hdcBuffer );
         DeleteDC( hdcScreen );

         EndPaint( hWnd, &ps );
         break;

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
         if( retroflat_bitmap_ok( &g_screen ) ) {
            DeleteObject( g_screen.b );
         }
         PostQuitMessage( 0 );
         break;

      case WM_TIMER:
         g_loop_iter( g_loop_data );

         /* Kind of a hack so that we can have a cheap timer. */
         g_ms += 100;
         break;

      default:
         return DefWindowProc( hWnd, message, wParam, lParam );
   }

   return 0;
}

#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

int retroflat_loop( retroflat_loop_iter loop_iter, void* data ) {

#  if defined( RETROFLAT_API_ALLEGRO ) || defined( RETROFLAT_API_SDL )

   g_retroflat_flags |= RETROFLAT_FLAGS_RUNNING;
   do {
      loop_iter( data );
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
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_SDL || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   /* This should be set by retroflat_quit(). */
   return g_retval;
}

void retroflat_message( const char* title, const char* format, ... ) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

#  if defined( RETROFLAT_API_ALLEGRO )
   allegro_message( "%s", msg_out );
#  elif defined( RETROFLAT_API_SDL )
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

int retroflat_init(
   const char* title, int screen_w, int screen_h, int argc, char* argv[]
) {
   int retval = 0;
#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   WNDCLASS wc = { 0 };
   RECT wr = { 0, 0, 0, 0 };
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

   if( allegro_init() ) {
      allegro_message( "could not setup allegro!" );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   install_keyboard();
#     ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
   install_timer();
#     endif /* RETROFLAT_MOUSE */

#     ifdef RETROFLAT_OS_DOS
   /* Don't try windowed mode in DOS. */
   if( set_gfx_mode( GFX_AUTODETECT, screen_w, screen_h, 0, 0 ) ) {
#     else
   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, screen_w, screen_h, 0, 0 ) ) {
#     endif /* RETROFLAT_OS_DOS */

      allegro_message( "could not setup graphics!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#     ifndef RETROFLAT_OS_DOS
   set_window_title( title );
#     endif /* !RETROFLAT_OS_DOS */

#     ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
   if( 0 > install_mouse() ) {
      allegro_message( "could not setup mouse!" );
      retval = RETROFLAT_ERROR_MOUSE;
      goto cleanup;
   }
#     endif /* RETROFLAT_MOUSE */

   g_screen.b = create_bitmap( screen_w, screen_h );
   if( NULL == g_screen.b ) {
      allegro_message( "unable to allocate screen buffer!" );
      goto cleanup;
   }

#  elif defined( RETROFLAT_API_SDL )

   /* == SDL == */

   if( SDL_Init( SDL_INIT_EVERYTHING ) ) {
      retroflat_message(
         "Error", "Error initializing SDL: %s", SDL_GetError() );
   }

   g_screen_v_w = screen_w;
   g_screen_v_h = screen_h;
   g_screen_w = screen_w;
   g_screen_h = screen_h;
   g_window = SDL_CreateWindow( title,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      screen_w, screen_h, 0 );
   if( NULL == g_window ) {
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }
   g_screen.surface = SDL_GetWindowSurface( g_window );
   if( NULL == g_screen.surface ) {
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }
   g_screen.renderer = SDL_CreateSoftwareRenderer( g_screen.surface );
   if( NULL == g_screen.renderer ) {
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16/Win32 == */

   /* Get the *real* size of the window, including titlebar. */
   wr.right = screen_w;
   wr.bottom = screen_h;
   AdjustWindowRect( &wr, RETROFLAT_WIN_STYLE, FALSE );

   g_screen_w = screen_w;
   g_screen_h = screen_h;
   g_screen_v_w = screen_w;
   g_screen_v_h = screen_h;

   memset( &g_screen, '\0', sizeof( struct RETROFLAT_BITMAP ) );
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
      0, RETROFLAT_WINDOW_CLASS, title,
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

   ShowWindow( g_window, g_cmd_show );

#  else
#     warning "not implemented"
#  endif  /* RETROFLAT_API_ALLEGRO */

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown( int retval ) {

#ifdef RETROFLAT_API_ALLEGRO
   if( RETROFLAT_ERROR_ENGINE != retval ) {
      clear_keybuf();
   }

   retroflat_destroy_bitmap( &g_screen );
#endif /* RETROFLAT_API_ALLEGRO */

}

/* === */

void retroflat_set_assets_path( const char* path ) {
   memset( g_retroflat_assets_path, '\0', RETROFLAT_ASSETS_PATH_MAX );
   strncpy( g_retroflat_assets_path, path, RETROFLAT_ASSETS_PATH_MAX );
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

#  elif defined( RETROFLAT_API_SDL )

   /* == SDL == */

   if( NULL != bmp && (&g_screen) != bmp ) {
      assert( NULL == bmp->renderer );
      bmp->renderer = SDL_CreateSoftwareRenderer( bmp->surface );
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
#     warning "not implemented"
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
   blit( g_screen.b, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

   /* Release the screen. */
   release_screen();
#     ifdef RETROFLAT_MOUSE
      /* XXX: Broken in DOS. */
   show_mouse( screen ); /* Enable mouse after drawing. */
#     endif /* RETROFLAT_MOUSE */
   vsync();

#  elif defined( RETROFLAT_API_SDL )

   if( NULL == bmp || (&g_screen) == bmp ) {
      /* Flip the screen. */
      SDL_UpdateWindowSurface( g_window );
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
         InvalidateRect( g_window, 0, TRUE );
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
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */
}

/* === */

int retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   int retval = 0;
#if defined( RETROFLAT_API_WIN16 ) || defined (RETROFLAT_API_WIN32 )
   HDC hdc_win = (HDC)NULL;
   long i, x, y, w, h, bpp, offset, sz, read;
   char* buf = NULL;
   BITMAPINFO* bmi = NULL;
   FILE* bmp_file = NULL;
   unsigned long txp_color = 0;
   BITMAP bm;
   int lock_ret = 0;
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   assert( NULL != bmp_out );

   /* Build the path to the bitmap. */
   memset( filename_path, '\0', RETROFLAT_PATH_MAX + 1 );
   snprintf( filename_path, RETROFLAT_PATH_MAX, "%s%c%s.%s",
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

#  elif defined( RETROFLAT_API_SDL )

   /* == SDL == */

   bmp_out->renderer = NULL;

   bmp_out->surface = SDL_LoadBMP( filename_path );
   if( NULL == bmp_out->surface ) {
      retroflat_message(
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   SDL_SetColorKey( bmp_out->surface, SDL_TRUE,
      SDL_MapRGB( bmp_out->surface->format,
         RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );

   bmp_out->texture =
      SDL_CreateTextureFromSurface( g_screen.renderer, bmp_out->surface );
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

#  elif defined( RETROFLAT_API_WIN16 )

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

   SetDIBits( hdc_win, bmp_out->b, 0,
      bmi->bmiHeader.biHeight, &(buf[offset]), bmi,
      DIB_RGB_COLORS );

   /* Setup bitmap transparency mask. */
   bmp_out->mask = CreateBitmap(
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight, 1, 1, NULL );

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
      bmp_out->hdc_mask, 0, 0,
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight,
      bmp_out->hdc_b, 0, 0, SRCCOPY );
   BitBlt(
      bmp_out->hdc_b, 0, 0,
      bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight,
      bmp_out->hdc_mask, 0, 0, SRCINVERT );

cleanup:

   if( NULL != buf ) {
      free( buf );
   }

   if( NULL != bmp_file ) {
      fclose( bmp_file );
   }

   if( (HDC)NULL != hdc_win ) {
      ReleaseDC( g_window, hdc_win );
   }

   if( RETROFLAT_OK == lock_ret ) {
      retroflat_draw_release( bmp_out );
   }

#  elif defined( RETROFLAT_API_WIN32 )

   /* == Win32 == */

   bmp_out->b = LoadImage( NULL, b->id, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

#  else
#     warning "not implemented"
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

#  elif defined( RETROFLAT_API_SDL )

   assert( NULL != bitmap );
   assert( NULL != bitmap->texture );
   assert( NULL != bitmap->surface );

   SDL_DestroyTexture( bitmap->texture );
   bitmap->texture = NULL;
   SDL_FreeSurface( bitmap->surface );
   bitmap->texture = NULL;

#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* == Win16 == */

   if( NULL == bitmap->b ) {
      return;
   }

   DeleteObject( bitmap->b );
   bitmap->b = NULL;

#  else
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */
}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int w, int h
) {
#  if defined( RETROFLAT_API_SDL )
   int lock_ret = 0;
   int locked_target_internal = 0;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   int lock_ret = 0;
   int locked_src_internal = 0;
   int locked_target_internal = 0;
#  endif /* RETROFLAT_API_SDL || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_screen);
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

#  elif defined( RETROFLAT_API_SDL )

   /* == SDL == */

   SDL_Rect dest_rect = {
      d_x, 
      d_y,
      w, 
      h};
   SDL_Rect src_rect = {
      s_x, s_y, w, h };

   retroflat_internal_autolock_bitmap(
      target, lock_ret, locked_target_internal );

   SDL_RenderCopy( target->renderer, src->texture, &src_rect, &dest_rect );

cleanup:

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

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
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */
   return;
}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags
) {
#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HBRUSH brush = (HBRUSH)NULL;
   HBRUSH old_brush = (HBRUSH)NULL;
   int lock_ret = 0,
      locked_target_internal = 0;
   HPEN pen = (HPEN)NULL;
   HPEN old_pen = (HPEN)NULL;
#endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_screen);
   }

#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );
   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      rectfill( target->b, x, y, x + w, y + h, color );
   } else {
      rect( target->b, x, y, x + w, y + h, color );
   }

#  elif defined( RETROFLAT_API_SDL )

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

   retroflat_win_setup_brush( brush, old_brush, target, flags );
   retroflat_win_setup_pen( pen, old_pen, target, flags );

   Rectangle( target->hdc_b, x, y, x + w, y + h );

cleanup:

   retroflat_win_cleanup_brush( brush, old_brush, target )
   retroflat_win_cleanup_pen( pen, old_pen, target )

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  else
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, unsigned char flags
) {
#  if defined( RETROFLAT_API_SDL )
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
      target = &(g_screen);
   }

#  if defined( RETROFLAT_API_ALLEGRO )

   /* == Allegro == */

   assert( NULL != target->b );
   line( target->b, x1, y1, x2, y2, color );

#  elif defined( RETROFLAT_API_SDL )

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

   retroflat_win_setup_pen( pen, old_pen, target, flags );

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
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags
) {
#  if defined( RETROFLAT_API_SDL )
   int lock_ret = 0,
      locked_target_internal = 0;
#  elif defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HPEN pen = (HPEN)NULL;
   HPEN old_pen = (HPEN)NULL;
   HBRUSH brush = (HBRUSH)NULL;
   HBRUSH old_brush = (HBRUSH)NULL;
   int lock_ret = 0,
      locked_target_internal = 0;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( NULL == target ) {
      target = &(g_screen);
   }

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

   assert( NULL != target->b );

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      ellipsefill( target->b, x + (w / 2), y + (h / 2), w / 2, h / 2, color );
   } else {
      ellipse( target->b, x + (w / 2), y + (h / 2), w / 2, h / 2, color );
   }

#  elif defined( RETROFLAT_API_SDL )

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

   retroflat_win_setup_brush( brush, old_brush, target, flags );
   retroflat_win_setup_pen( pen, old_pen, target, flags );

   Ellipse( target->hdc_b, x, y, x + w, y + h );

cleanup:

   retroflat_win_cleanup_brush( brush, old_brush, target )
   retroflat_win_cleanup_pen( brush, old_brush, target )

   if( locked_target_internal ) {
      retroflat_draw_release( target );
   }

#  else
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */
}

int retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   int key_out = 0;

   assert( NULL != input );

#  ifdef RETROFLAT_API_ALLEGRO

   /* == Allegro == */

#     ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
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
#     endif /* RETROFLAT_MOUSE */

   poll_keyboard();
   if( keypressed() ) {
      return (readkey() >> 8);
   }

#  elif defined( RETROFLAT_API_SDL )

   /* == SDL == */

   int eres = 0;
   SDL_Event event;

   SDL_PollEvent( &event );

   if( SDL_KEYDOWN == event.type ) {
      key_out = event.key.keysym.sym;

      /* Flush key buffer to improve responsiveness. */
      while( (eres = SDL_PollEvent( &event )) );

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
#     warning "not implemented"
#  endif /* RETROFLAT_API_ALLEGRO */

   return key_out;
}

#endif /* RETROFLT_C */

/*! \} */ /* maug_retroflt */

#endif /* RETROFLT_H */

