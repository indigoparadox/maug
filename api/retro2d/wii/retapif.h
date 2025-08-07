
#ifndef RETPLTF_H
#define RETPLTF_H

#include <malloc.h> /* memalign */
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/cache.h>
#include <ogc/lwp_watchdog.h> /* gettime, ticks_to_millisecs */

static uint32_t _retroflat_wii_rgb2ycbcr( uint8_t r, uint8_t g, uint8_t b ) {
   uint32_t y, cb, cr;
   
   y = (299 * r + 587 * g + 114 * b) / 1000;
   cb = (-16874 * r - 33126 * g + 50000 * b + 12800000) / 100000;
   cr = (50000 * r - 41869 * g - 8131 * b + 12800000) / 100000;
   
   return (y << 24) | (cb << 16) | (y << 8) | cr;
}

/* === */

static MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Allocate framebuffer. */
   debug_printf( 1, "setting up framebuffer..." );

   VIDEO_Init();
   g_retroflat_state->platform.rmode = VIDEO_GetPreferredMode( NULL );
   g_retroflat_state->buffer.bits = MEM_K0_TO_K1( SYS_AllocateFramebuffer(
      g_retroflat_state->platform.rmode ) );
   if( NULL == g_retroflat_state->buffer.bits ) {
      error_printf( "could not setup framebuffer memory!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   /* Setup rest of video config. */
   debug_printf( 1, "setting up video configuration..." );

   VIDEO_SetNextFramebuffer( g_retroflat_state->buffer.bits );
   VIDEO_SetBlack( FALSE );
   VIDEO_Configure( g_retroflat_state->platform.rmode );
   VIDEO_SetPostRetraceCallback( NULL );

   VIDEO_Flush();
   VIDEO_WaitVSync();
   if( g_retroflat_state->platform.rmode->viTVMode & VI_NON_INTERLACE ) {
      /* Extra sync for progressive mode. */
      VIDEO_WaitVSync();
   }

   /* Setup color palette. */
#  define RETROFLAT_COLOR_TABLE_WII( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
      g_retroflat_state->palette[idx] = _retroflat_wii_rgb2ycbcr( rd, gd, bd );

      /*
         (0xff000000 | \
         (((rd) & 0xff) << 16) | (((gd) & 0xff) << 8) | ((bd) & 0xff)); \
      debug_printf( 1, "setting color %d to: 0x%08x", \
         idx, g_retroflat_state->palette[idx] );
      */

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_WII )

   /* Seed random number generator. */
   srand( ticks_to_microsecs( gettime() ) );

   /* Make sure screen size agrees in all state fields. */
   g_retroflat_state->screen_v_w =
      g_retroflat_state->platform.rmode->viTVMode & VI_NON_INTERLACE ?
      g_retroflat_state->platform.rmode->fbWidth :
      g_retroflat_state->platform.rmode->fbWidth / 2;
   g_retroflat_state->screen_v_h =
      g_retroflat_state->platform.rmode->viTVMode & VI_NON_INTERLACE ?
      g_retroflat_state->platform.rmode->xfbHeight :
      g_retroflat_state->platform.rmode->xfbHeight / 2;
   g_retroflat_state->screen_w = g_retroflat_state->screen_v_w;
   g_retroflat_state->screen_h = g_retroflat_state->screen_v_h;
   g_retroflat_state->buffer.w = g_retroflat_state->screen_v_w;
   g_retroflat_state->buffer.h = g_retroflat_state->screen_v_h;

   /* Setup screen colors. */
   g_retroflat_state->scale = 1;
   g_retroflat_state->screen_colors = 16;

   /* TODO: Move to input API. */
   WPAD_Init();

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {
   /* TODO */
#  pragma message( "warning: shutdown not implemented" )
}

/* === */

MERROR_RETVAL retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Just skip to the generic loop. */
   retval = retroflat_loop_generic( frame_iter, loop_iter, data );

   /* This should be set by retroflat_quit(). */
   return retval;
}

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   maug_mzero( msg_out, RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   /* TODO */
#  pragma message( "warning: message not implemented" )

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Build the title. */
   va_start( vargs, format );
   maug_mzero( title, RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

   /* TODO */
#  pragma message( "warning: set_title not implemented" )

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   return ticks_to_millisecs( gettime() );
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

   if( NULL == bmp || retroflat_screen_buffer() == bmp ) {
      VIDEO_WaitVSync();
      VIDEO_SetNextFramebuffer( g_retroflat_state->buffer.bits );
   }

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == bmp || retroflat_screen_buffer() == bmp ) {
      /* Wait for vsync and then flip the screen buffer. */
      VIDEO_Flush();
      /*
      DCFlushRange( g_retroflat_state->buffer.bits,
         4 * g_retroflat_state->screen_w * g_retroflat_state->screen_h );
      */
      g_retroflat_state->platform.flags ^= RETROFLAT_WII_FLAG_FRAME_ODD;
   }

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap_px_cb(
   void* data, uint8_t px, int32_t x, int32_t y,
   void* header_info, uint8_t flags
) {
   struct RETROFLAT_BITMAP* b = (struct RETROFLAT_BITMAP*)data;

   if( MFMT_PX_FLAG_NEW_LINE != (MFMT_PX_FLAG_NEW_LINE & flags) ) {
      retroflat_px( b, px, x, y, 0 );
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   bmp_out->bits = memalign( 32, w * h * sizeof( uint32_t ) );
   if( NULL == bmp_out->bits ) {
      error_printf( "could not allocate new bitmap!" );
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   bmp_out->w = w;
   bmp_out->h = h;

cleanup:

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {
   free( bmp->bits );
   maug_mzero( bmp, sizeof( struct RETROFLAT_BITMAP ) );
}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != src );

   /* TODO */
#  pragma message( "warning: blit_bitmap not implemented" )

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags
) {
   uint8_t autolock_px = 0;
   size_t px_idx = 0;

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
   ) {
      return;
   }

   retroflat_constrain_px( x, y, target, return );

   /* Autolock pixels if needed. */
   if(
      RETROFLAT_WII_FLAG_PX_LOCKED ==
      (RETROFLAT_WII_FLAG_PX_LOCKED & target->flags)
   ) {
      autolock_px = 1;
      retroflat_px_lock( target );
   }

   if(
      /* Non-screen bitmap. */
      retroflat_screen_buffer() != target ||
      /* TODO: Test progressive mode. */
      (g_retroflat_state->platform.rmode->viTVMode & VI_NON_INTERLACE)
   ) {
      px_idx = (y * target->w) + x;

   } else if(
      RETROFLAT_WII_FLAG_FRAME_ODD ==
      (RETROFLAT_WII_FLAG_FRAME_ODD & g_retroflat_state->platform.flags)
   ) {
      px_idx = (y * target->w * 2) + x;

   } else {
      /* Offset the scanline for interlaced mode. */
      px_idx = (y * target->w * 2) + target->w + x;
   }

   /* Perform the actual pixel. */
   target->bits[px_idx] = g_retroflat_state->palette[color_idx];

cleanup:

   if( autolock_px ) {
      retroflat_px_release( target );
   }

}

/* === */

void retroflat_resize_v() {
   /* Platform does not support resizing. */
}

#endif /* !RETPLTF_H */

