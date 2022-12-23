
#ifndef RETROFLT_H
#define RETROFLT_H

/* === Generic Includes and Defines === */

#define RETROFLAT_OK             0x00
#define RETROFLAT_ERROR_ENGINE   0x01
#define RETROFLAT_ERROR_GRAPHICS 0x02
#define RETROFLAT_ERROR_MOUSE    0x04
#define RETROFLAT_ERROR_BITMAP   0x08

#define RETROFLAT_FLAGS_FILL     0x01

#ifdef DEBUG
#include <assert.h>
#else
#define assert( x )
#endif /* DEBUG */

#define RETROFLAT_BITMAP_EXT "bmp"

struct RETROFLAT_INPUT {
   int mouse_x;
   int mouse_y;
};

/* === Platform-Specific Includes and Defines === */

#ifdef RETROFLAT_ALLEGRO
#  include <allegro.h>
struct RETROFLAT_BITMAP {
   BITMAP* b;
};
typedef int RETROFLAT_COLOR;
#  define RETROFLAT_COLOR_BLACK     makecol( 0, 0, 0 )
#  define RETROFLAT_COLOR_WHITE     makecol( 255, 255, 255 )
#  define RETROFLAT_COLOR_GRAY      makecol( 128, 128, 128 )

#  define retroflat_bitmap_ok( bitmap ) (NULL != (bitmap)->b)

#  define RETROFLAT_MOUSE_B_LEFT    -1
#  define RETROFLAT_MOUSE_B_RIGHT   -2

#endif /* RETROFLAT_ALLEGRO */

#ifdef RETROFLAT_OS_DOS
#  define RETROFLAT_PATH_SEP '\\'
#else
#  define RETROFLAT_PATH_SEP '/'
#endif /* RETROFLAT_OS_DOS */

/*! \brief Maximum size of the assets path, to allow room for appending. */
#define RETROFLAT_ASSETS_PATH_MAX (PATH_MAX / 2)

/* === Translation Module === */

/* Declare the prototypes so that internal functions can call each other. */
int retroflat_init( int screen_w, int screen_h );
void retroflat_shutdown( int retval );
void retroflat_set_assets_path( const char* path );
void retroflat_draw_lock();
void retroflat_draw_flip();
int retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out );
void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bitmap );
void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int x, int y );
void retroflat_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, unsigned char flags );
int retroflat_poll_input();

#ifdef RETROFLT_C

#  include <stdlib.h>

/* === Globals === */

char g_retroflat_assets_path[RETROFLAT_ASSETS_PATH_MAX + 1];
struct RETROFLAT_BITMAP g_screen;

/* === Function Definitions === */

int retroflat_init( int screen_w, int screen_h ) {
   int retval = 0;

#  ifdef RETROFLAT_ALLEGRO
   if( allegro_init() ) {
      allegro_message( "could not setup allegro!" );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   install_keyboard();
#ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
   install_timer();
#endif /* RETROFLAT_MOUSE */

#ifdef RETROFLAT_OS_DOS
   /* Don't try windowed mode in DOS. */
   if( set_gfx_mode( GFX_AUTODETECT, screen_w, screen_h, 0, 0 ) ) {
#else
   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, screen_w, screen_h, 0, 0 ) ) {
#endif /* RETROFLAT_OS_DOS */

      allegro_message( "could not setup graphics!" );
      retval = RETROFLAT_ERROR_GRAPHICS;
      goto cleanup;
   }

#ifdef RETROFLAT_MOUSE
   /* XXX: Broken in DOS. */
   if( 0 > install_mouse() ) {
      allegro_message( "could not setup mouse!" );
      retval = RETROFLAT_ERROR_MOUSE;
      goto cleanup;
   }
#endif /* RETROFLAT_MOUSE */

   g_screen.b = create_bitmap( SCREEN_W, SCREEN_H );
   if( NULL == g_screen.b ) {
      allegro_message( "unable to allocate screen buffer!" );
      goto cleanup;
   }

#  else
#     error "not implemented"
#  endif  /* RETROFLAT_ALLEGRO */

cleanup:
   return retval;
}

/* === */

void retroflat_shutdown( int retval ) {

#ifdef RETROFLAT_ALLEGRO
   if( RETROFLAT_ERROR_ENGINE != retval ) {
      clear_keybuf();
   }

   retroflat_destroy_bitmap( &g_screen );
#endif /* RETROFLAT_ALLEGRO */

}

/* === */

void retroflat_set_assets_path( const char* path ) {
   memset( g_retroflat_assets_path, '\0', RETROFLAT_ASSETS_PATH_MAX );
   strncpy( g_retroflat_assets_path, path, RETROFLAT_ASSETS_PATH_MAX );
}

/* === */

void retroflat_draw_lock() {

#  ifdef RETROFLAT_ALLEGRO
#     ifdef RETROFLAT_MOUSE
      /* XXX: Broken in DOS. */
   show_mouse( NULL ); /* Disable mouse before drawing. */
#     endif
   acquire_screen();
#  else
#     error "not implemented"
#  endif /* RETROFLAT_ALLEGRO */

}

/* === */

void retroflat_draw_flip() {

#  ifdef RETROFLAT_ALLEGRO
   blit( g_screen.b, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
   release_screen();
#     ifdef RETROFLAT_MOUSE
      /* XXX: Broken in DOS. */
   show_mouse( screen ); /* Enable mouse after drawing. */
#     endif /* RETROFLAT_MOUSE */
   vsync();
#  else
#     error "not implemented"
#  endif /* RETROFLAT_ALLEGRO */

}

/* === */

int retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out
) {
   char filename_path[PATH_MAX + 1];

   assert( NULL != bmp_out );

   /* Build the path to the bitmap. */
   memset( filename_path, '\0', PATH_MAX + 1 );
   snprintf( filename_path, PATH_MAX, "%s%c%s.%s",
      g_retroflat_assets_path, RETROFLAT_PATH_SEP,
      filename, RETROFLAT_BITMAP_EXT );

#  ifdef RETROFLAT_ALLEGRO
   bmp_out->b = load_bitmap( filename_path, NULL );

   if( NULL != bmp_out->b ) {
      return RETROFLAT_OK;
   } else {
      allegro_message( "unable to load %s", filename_path );
      return RETROFLAT_ERROR_BITMAP;
   }
#  else
#     error "not implemented"
#  endif /* RETROFLAT_ALLEGRO */
}

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bitmap ) {
#  ifdef RETROFLAT_ALLEGRO
   if( NULL == bitmap->b ) {
      return;
   }

   destroy_bitmap( bitmap->b );
   bitmap->b = NULL;
#  else
#     error "not implemented"
#  endif /* RETROFLAT_ALLEGRO */
}

/* === */

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src, int x, int y
) {

   if( NULL == target ) {
      target = &(g_screen);
   }
   assert( NULL != target->b );
   assert( NULL != src );
   assert( NULL != src->b );

#  ifdef RETROFLAT_ALLEGRO
   draw_sprite( target->b, src->b, x, y );
#  else
#     error "not implimented"
#  endif /* RETROFLAT_ALLEGRO */

}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x, int y, int w, int h, unsigned char flags
) {

   if( NULL == target ) {
      target = &(g_screen);
   }
   assert( NULL != target->b );

#  ifdef RETROFLAT_ALLEGRO
   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
      rectfill( target->b, x, y, w, h, color );
   } else {
      rect( target->b, x, y, w, h, color );
   }
#  else
#     error "not implemented"
#  endif /* RETROFLAT_ALLEGRO */
}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, RETROFLAT_COLOR color,
   int x1, int y1, int x2, int y2, unsigned char flags
) {

   if( NULL == target ) {
      target = &(g_screen);
   }
   assert( NULL != target->b );

#  ifdef RETROFLAT_ALLEGRO
   line( target->b, x1, y1, x2, y2, color );
#  else
#     error "not implemented"
#  endif /* RETROFLAT_ALLEGRO */
}

/* === */

int retroflat_poll_input( struct RETROFLAT_INPUT* input ) {

#  ifdef RETROFLAT_ALLEGRO
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

   return 0;
#  else
#     error "not implemented"
#  endif /* RETROFLAT_ALLEGRO */

}

#endif /* RETROFLT_C */

#endif /* RETROFLT_H */

