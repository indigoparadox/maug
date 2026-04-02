
#ifndef RETPLTF_H
#define RETPLTF_H

static MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO */
#  pragma message( "warning: init_platform not implemented" )

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

   /* TODO */
#  pragma message( "warning: draw_release not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   maug_path filename_path;
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, RETROFLAT_BITMAP_EXT,
      filename_path, MAUG_PATH_SZ_MAX, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

   /* TODO */
#  pragma message( "warning: load_bitmap not implemented" )

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
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
   retroflat_pxxy_t s_x, retroflat_pxxy_t s_y,
   retroflat_pxxy_t d_x, retroflat_pxxy_t d_y,
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != src );

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
   ) {
      return MERROR_GUI;
   }

   /* TODO: This is only needed for a soft viewport... if the viewport can
    * scroll then use that!
    */
   /* Trim sprite to stay on-screen. */
   retval = retroflat_trim_px(
      instance, &s_x, &s_y, &d_x, &d_y, &w, &h );
   maug_cleanup_if_not_ok();

   /* TODO */
#  pragma message( "warning: blit_bitmap not implemented" )

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x, retroflat_pxxy_t y, uint8_t flags
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
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h, uint8_t flags
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
   retroflat_constrain_px( x + w, y + h, target, return );

   /* TODO */
#  pragma message( "warning: rect not implemented" )

}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x1, retroflat_pxxy_t y1,
   retroflat_pxxy_t x2, retroflat_pxxy_t y2, uint8_t flags
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

   retroflat_constrain_px( x1, y1, target, return );
   retroflat_constrain_px( x2, y2, target, return );

   /* TODO */
#  pragma message( "warning: line not implemented" )

}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h, uint8_t flags
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
#  pragma message( "warning: ellipse not implemented" )

}

/* === */

void retroflat_resize_v() {
   /* Platform does not support resizing. */
}

#endif /* !RETPLTF_H */

