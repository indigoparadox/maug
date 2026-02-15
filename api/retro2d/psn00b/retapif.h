
#ifndef RETPLTF_H
#define RETPLTF_H

static MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Force screen size. */
   args->screen_w = 256;
   args->screen_h = 192;

   /*
   g_retroflat_state->buffer.w = 256;
   g_retroflat_state->buffer.h = 192;
   */

   /* Setup PSX graphics. */
   ResetGraph( 0 );
   SetDefDispEnv(
      &(g_retroflat_state->platform.disp[0]), 0, 0,
      args->screen_w, args->screen_h );
   SetDefDispEnv(
      &(g_retroflat_state->platform.disp[1]), 0,
      args->screen_h, args->screen_w, args->screen_h );
   SetDefDrawEnv(
      &(g_retroflat_state->platform.draw[0]), 0,
      args->screen_h, args->screen_w, args->screen_h );
   SetDefDrawEnv(
      &(g_retroflat_state->platform.draw[1]), 0,
      0, args->screen_w, args->screen_h );
   setRGB0( &(g_retroflat_state->platform.draw[0]), 0, 0, 127 );
   setRGB0( &(g_retroflat_state->platform.draw[1]), 0, 0, 127 );
   g_retroflat_state->platform.draw[0].isbg = 1;
   g_retroflat_state->platform.draw[1].isbg = 1;
   PutDispEnv( &(g_retroflat_state->platform.disp[0]) );
   PutDrawEnv( &(g_retroflat_state->platform.draw[0]) );
   g_retroflat_state->platform.buffer_idx = 0;

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
   /* TODO */
#  pragma message( "warning: get_ms not implemented" )
}

/* === */

uint32_t retroflat_get_rand() {
   /* TODO */
#  pragma message( "warning: get_rand not implemented" )
}

/* === */

MERROR_RETVAL retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = RETROFLAT_OK;

   /* TODO */
#  pragma message( "warning: draw_lock not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == bmp || retroflat_screen_buffer() == bmp ) {
      DrawSync( 0 );
      VSync( 0 );
      g_retroflat_state->platform.buffer_idx =
         !(g_retroflat_state->platform.buffer_idx);
      PutDispEnv(
         &(g_retroflat_state->platform.disp[
            g_retroflat_state->platform.buffer_idx]) );
      PutDrawEnv(
         &(g_retroflat_state->platform.draw[
            g_retroflat_state->platform.buffer_idx]) );
      SetDispMask( 1 );
   }

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[MAUG_PATH_SZ_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, RETROFLAT_BITMAP_EXT,
      filename_path, MAUG_PATH_SZ_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

   /* TODO */
#  pragma message( "warning: load_bitmap not implemented" )

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   /* TODO */
#  pragma message( "warning: create_bitmap not implemented" )

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

   /* TODO */
#  pragma message( "warning: destroy_bitmap not implemented" )

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

   /* TODO */
#  pragma message( "warning: px not implemented" )

}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   /* TODO */
#  pragma message( "warning: rect not implemented" )

}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   /* TODO */
#  pragma message( "warning: line not implemented" )

}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   /* TODO */
#  pragma message( "warning: ellipse not implemented" )

}

/* === */

void retroflat_resize_v() {
   /* Platform does not support resizing. */
}

#endif /* !RETPLTF_H */

