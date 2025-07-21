
#ifndef RETPLTF_H
#define RETPLTF_H

static MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct timespec spec;
   time_t tm;
   int screen_w, screen_h;

   /* TODO: Add some flexibility for simulating lower-color platforms. */
   g_retroflat_state->screen_colors = 16;

   g_retroflat_state->platform.display = XOpenDisplay( NULL );
   if( NULL == g_retroflat_state->platform.display ) {
      error_printf( "unable to open display" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   g_retroflat_state->platform.screen = DefaultScreen(
      g_retroflat_state->platform.display );

   screen_w = DisplayWidth(
      g_retroflat_state->platform.display,
      g_retroflat_state->platform.screen );
   screen_h = DisplayHeight(
      g_retroflat_state->platform.display,
      g_retroflat_state->platform.screen );

   g_retroflat_state->platform.window = XCreateSimpleWindow(
      g_retroflat_state->platform.display,
      RootWindow(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.screen ),
      (screen_w - g_retroflat_state->screen_w) / 2,
      (screen_h - g_retroflat_state->screen_h) / 2,
      g_retroflat_state->screen_w,
      g_retroflat_state->screen_h,
      1,
      BlackPixel(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.screen ),
      WhitePixel(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.screen ) );

   g_retroflat_state->platform.visual = DefaultVisual(
      g_retroflat_state->platform.display,
      g_retroflat_state->platform.screen );

   /* TODO: Check? */

   /* Show the window. */
   XMapWindow(
      g_retroflat_state->platform.display,
      g_retroflat_state->platform.window );

   /* Create the buffer. */
   g_retroflat_state->buffer.bits_sz =
      g_retroflat_state->screen_w * g_retroflat_state->screen_h * 4;
   g_retroflat_state->buffer.bits = calloc(
      g_retroflat_state->screen_w * g_retroflat_state->screen_h, 4 );
   if( NULL == g_retroflat_state->buffer.bits ) {
      error_printf( "coult not allocate buffer bitmap!" );
      retval = MERROR_GUI;
      goto cleanup;
   }
   g_retroflat_state->buffer.img = XCreateImage(
      g_retroflat_state->platform.display, g_retroflat_state->platform.visual, 
      DefaultDepth(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.screen ),
      ZPixmap,
      0,
      g_retroflat_state->buffer.bits,
      g_retroflat_state->screen_w,
      g_retroflat_state->screen_h, 32, 0 );
   if( NULL == g_retroflat_state->buffer.img ) {
      error_printf( "coult not allocate buffer image!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   clock_gettime( CLOCK_MONOTONIC, &spec );
   g_retroflat_state->platform.s_launch = spec.tv_sec;

   srand( (unsigned int)time( &tm ) );

#  define RETROFLAT_COLOR_TABLE_XLIB( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
      g_retroflat_state->palette[idx] = \
         (((rd) & 0xff) << 16) | (((gd) & 0xff) << 8) | ((bd) & 0xff);
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_XLIB )

   /* TODO: Split out into input API. */
   XSelectInput(
      g_retroflat_state->platform.display,
      g_retroflat_state->platform.window, ExposureMask | KeyPressMask );

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {
   if( NULL != g_retroflat_state->buffer.img ) {
      XDestroyImage( g_retroflat_state->buffer.img );
   }
   /* if( NULL != g_buffer_bits ) {
      free( g_buffer_bits );
   } */
   XCloseDisplay( g_retroflat_state->platform.display );
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

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
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
   memset( title, '\0', RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

   /* TODO */
#  pragma message( "warning: set_title not implemented" )

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   struct timespec spec;
   uint32_t ms_out = 0,
      ms_launch_delta = 0;
   
   clock_gettime( CLOCK_MONOTONIC, &spec );

   /* Get the seconds since program launched. Multiply by 1000, so we want a
    * smaller number than seconds since the epoch. */
   ms_launch_delta = spec.tv_sec - g_retroflat_state->platform.s_launch;
   ms_out += ms_launch_delta * 1000;
   ms_out += spec.tv_nsec / 1000000;

   return ms_out;
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

   /* TODO */
#  pragma message( "warning: draw_lock not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == bmp || retroflat_screen_buffer() == bmp ) {
      XPutImage(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.window,
         DefaultGC(
            g_retroflat_state->platform.display,
            g_retroflat_state->platform.screen ),
         g_retroflat_state->buffer.img,
         0, 0, 0, 0,
         g_retroflat_state->screen_w, g_retroflat_state->screen_h );
   }

   return retval;
}

/* === */

static MERROR_RETVAL _retroflat_load_bitmap_px_cb(
   void* data, uint8_t px, int32_t x, int32_t y,
   void* header_info, uint8_t flags
) {
   struct RETROFLAT_BITMAP* b = (struct RETROFLAT_BITMAP*)data;

   if( MFMT_PX_FLAG_NEW_LINE != (MFMT_PX_FLAG_NEW_LINE & flags) ) {
      XPutPixel( b->img, x, y, g_retroflat_state->palette[px] );
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;
   mfile_t bmp_file;
   struct MFMT_STRUCT_BMPFILE header_bmp;
   uint8_t bmp_flags = 0;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, RETROFLAT_PATH_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

   bmp_out->flags = flags;

   /* Open the bitmap file. */
   retval = mfile_open_read( filename_path, &bmp_file );
   maug_cleanup_if_not_ok();

   /* mfmt file detection system. */
   maug_mzero( &header_bmp, sizeof( struct MFMT_STRUCT_BMPFILE ) );
   header_bmp.magic[0] = 'B';
   header_bmp.magic[1] = 'M';
   header_bmp.info.sz = 40;

   retval = mfmt_read_bmp_header(
      (struct MFMT_STRUCT*)&header_bmp,
      &bmp_file, 0, mfile_get_sz( &bmp_file ), &bmp_flags );
   maug_cleanup_if_not_ok();

   /* Create buffer and XImage struct. */
   bmp_out->bits = calloc(
      header_bmp.info.width * header_bmp.info.height, 4 );
   if( NULL == bmp_out->bits ) {
      error_printf( "could not allocate bitmap bits!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   bmp_out->img = XCreateImage(
      g_retroflat_state->platform.display,
      g_retroflat_state->platform.visual, 
      DefaultDepth(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.screen ),
      ZPixmap,
      0,
      bmp_out->bits,
      header_bmp.info.width,
      header_bmp.info.height,
      32, 0 );
   if( NULL == bmp_out->img ) {
      error_printf( "could not create bitmap image!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   retval = mfmt_read_bmp_px_cb(
      (struct MFMT_STRUCT*)&header_bmp,
      &bmp_file,
      header_bmp.px_offset,
      mfile_get_sz( &bmp_file ) - header_bmp.px_offset,
      bmp_flags,
      _retroflat_load_bitmap_px_cb,
      bmp_out );
   maug_cleanup_if_not_ok();

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

   bmp_out->flags = flags;

   /* Create the buffer. */
   bmp_out->bits_sz = w * h * 4;
   bmp_out->bits = calloc( w * h, 4 );
   if( NULL == bmp_out->bits ) {
      error_printf( "coult not allocate bitmap bits!" );
      retval = MERROR_GUI;
      goto cleanup;
   }
   bmp_out->img = XCreateImage(
      g_retroflat_state->platform.display, g_retroflat_state->platform.visual, 
      DefaultDepth(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.screen ),
      ZPixmap,
      0,
      bmp_out->bits,
      w, h, 32, 0 );
   if( NULL == bmp_out->img ) {
      error_printf( "coult not allocate bitmap image!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

cleanup:

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

   /* This should also automatically free bmp->bits. */
   XDestroyImage( bmp->img );
   bmp->img = NULL;
   bmp->bits = NULL;
   bmp->bits_sz = 0;

}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;
   Pixmap target_pxm = 0;
   size_t t_w, t_h, x, y;
   uint32_t px = 0;

   assert( NULL != src );

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
   ) {
      retval = MERROR_GUI;
      goto cleanup;
   }

   if( RETROFLAT_FLAGS_OPAQUE == (RETROFLAT_FLAGS_OPAQUE & src->flags) ) {
      /* Bitmap is opaque, so do this the relatively fast way. */

      t_w = target->img->width;
      t_h = target->img->height;

      /* Convert the target into a client pixmap so we can blit to it. */
      target_pxm = XCreatePixmap(
         g_retroflat_state->platform.display,
         g_retroflat_state->platform.window,
         t_w, t_h,
         DefaultDepth(
            g_retroflat_state->platform.display,
            g_retroflat_state->platform.screen ) );
      XPutImage(
         g_retroflat_state->platform.display,
         target_pxm,
         DefaultGC(
            g_retroflat_state->platform.display,
            g_retroflat_state->platform.screen ),
         target->img,
         0, 0, 0, 0, t_w, t_h );

      /* Replace the old target image with the newly-blitted target pixmap. */

      if( 0 == target_pxm ) {
         error_printf( "could not create target pixmap!" );
         retval = MERROR_GUI;
         goto cleanup;
      }

      /* Perform the actual blit. */
      XPutImage(
         g_retroflat_state->platform.display,
         target_pxm,
         DefaultGC(
            g_retroflat_state->platform.display,
            g_retroflat_state->platform.screen ),
         src->img,
         s_x, s_y, d_x, d_y, w, h );

      /* Replace the old target image with the newly-blitted target pixmap. */
      XDestroyImage( target->img );
      target->img = XGetImage(
         g_retroflat_state->platform.display,
         target_pxm,
         0, 0, t_w, t_h, AllPlanes, ZPixmap );
   } else {
      /* Bitmap is transparent, so go pixel-by-pixel. */
      for( y = 0 ; h > y ; y++ ) {
         for( x = 0 ; w > x ; x++ ) {
            px = XGetPixel( src->img, s_x + x, s_y + y );
            if( px ) {
               /* Non-transparent pixel! */
               XPutPixel( target->img, d_x + x, d_y + y, px );
            }
         }
      }
   }

cleanup:

   if( 0 < target_pxm ) {
      XFreePixmap( g_retroflat_state->platform.display, target_pxm );
   }

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

   XPutPixel( target->img, x, y, g_retroflat_state->palette[color_idx] );
}

/* === */

void retroflat_resize_v() {
   /* Platform does not support resizing. */
}

#endif /* !RETPLTF_H */

