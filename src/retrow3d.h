
#ifndef RETROW3D_H
#define RETROW3D_H

#define RETROWIN3D_FLAG_INIT_GUI          0x01
#define RETROWIN3D_FLAG_INIT_BMP          0x02

struct RETROWIN3D {
   uint8_t flags;
   size_t w;
   size_t h;
   struct RETROGUI gui;
   struct RETROFLAT_BITMAP gui_bmp;
};

#ifdef RETRO3DW_C

void retro3dw_redraw( struct RETROWIN3D* win ) {
   float aspect_ratio = 0,
      screen_x = 0,
      screen_y = 0,
      gui_w_f = 0,
      gui_h_f = 0;

   /* TODO: Drawing everything onto one big texture seems to fail under win32,
    *       leaving us with a big, opaque poly with no texture.
    */

   assert( 0 < win->gui_bmp.tex.id );

   retroflat_draw_lock( &(win->gui_bmp) );

   assert( NULL != win->gui_bmp.tex.bytes );

   win->gui.draw_bmp = &(win->gui_bmp);
   retrogui_redraw_ctls( &(win->gui) );

   /* Set a white background for the overlay poly. */
   glColor3fv( RETROGLU_COLOR_WHITE );
   if( MERROR_OK != retroglu_check_errors( "overlay color" ) ) {
      goto cleanup;
   }

   /*
   memset( win->gui_bmp.tex.bytes, 0xff, win->gui_bmp.w * win->gui_bmp.h * 4 );
   memset( win->gui_bmp.tex.bytes, 0x00, win->gui_bmp.w * (win->gui_bmp.h / 2) * 4 );
   */
   /* retroflat_px( &(win->gui_bmp), RETROFLAT_COLOR_RED, 10, 10, 0 ); */

   assert( NULL != win->gui_bmp.tex.bytes );

   /* Prepare projection to draw the overlay on the top of the screen. */
   retroglu_push_overlay( 0, 0, screen_x, screen_y, aspect_ratio );
   if( MERROR_OK != retroglu_check_errors( "overlay push" ) ) {
      goto cleanup;
   }
   retroglu_whf( win->w, win->h, gui_w_f, gui_h_f, aspect_ratio );

/* #ifdef RETROGLU_NO_TEXTURES */
   /* debug_printf( 1, "overlay texture: %d x %d (%p)",
      win->gui_bmp.tex.w, win->gui_bmp.tex.h,
      win->gui_bmp.tex.bytes ); */
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
      win->gui_bmp.tex.w, win->gui_bmp.tex.h, 0,
      GL_RGBA, GL_UNSIGNED_BYTE,
      win->gui_bmp.tex.bytes );
/* #else
   glBindTexture( GL_TEXTURE_2D, win->gui_bmp.tex.id );
#endif */ /* RETROGLU_NO_TEXTURES */

   if( MERROR_OK != retroglu_check_errors( "overlay texture" ) ) {
      goto cleanup;
   }

   /* Make the background transparency work. */
   glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   /* TODO: Break up overlay into multiple triangles with offset textures. */

   glBegin( GL_TRIANGLES );
   glTexCoord2f( 0, 0 );
   glVertex3f( screen_x,            screen_y,            RETROFLAT_GL_Z );
   glTexCoord2f( 0, 1 );
   glVertex3f( screen_x,            screen_y - gui_h_f, RETROFLAT_GL_Z );
   glTexCoord2f( 1, 1 );
   glVertex3f( screen_x + gui_w_f, screen_y - gui_h_f, RETROFLAT_GL_Z );

   glTexCoord2f( 1, 1 );
   glVertex3f( screen_x + gui_w_f, screen_y - gui_h_f, RETROFLAT_GL_Z );
   glTexCoord2f( 1, 0 );
   glVertex3f( screen_x + gui_w_f, screen_y,            RETROFLAT_GL_Z );
   glTexCoord2f( 0, 0 );
   glVertex3f( screen_x,            screen_y,            RETROFLAT_GL_Z );
   glEnd();

   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
      0, 0, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, NULL ); 

cleanup:

   retroflat_draw_release( &(win->gui_bmp) );

#if 0
#ifndef RETROGLU_NO_TEXTURES
   /* Reset texture so overlay isn't drawn on subsequent polys. */
   glBindTexture( GL_TEXTURE_2D, 0 );
#endif /* !RETROGLU_NO_TEXTURES */
#endif

   retroglu_pop_overlay();
}

void retro3dw_free( struct RETROWIN3D* win ) {

#ifndef RETROGXC_PRESENT
   if( (MAUG_MHANDLE)NULL != win->gui.font_h ) {
      maug_mfree( win->gui.font_h );
   }
#endif /* RETROGXC_PRESENT */

   if( RETROWIN3D_FLAG_INIT_GUI == (RETROWIN3D_FLAG_INIT_GUI & win->flags) ) {
      retrogui_free( &(win->gui) );
   }

   if( RETROWIN3D_FLAG_INIT_BMP == (RETROWIN3D_FLAG_INIT_BMP & win->flags) ) {
      retroflat_destroy_bitmap( &(win->gui_bmp) );
   }
}

MERROR_RETVAL retro3dw_init(
   struct RETROWIN3D* win, const char* font_filename,
   size_t x, size_t y, size_t w, size_t h
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( win, sizeof( struct RETROWIN3D ) );

   retval = retroflat_create_bitmap( w, h, &(win->gui_bmp), 0 );
   maug_cleanup_if_not_ok();

   win->flags |= RETROWIN3D_FLAG_INIT_BMP;

   retval = retrogui_init( &(win->gui) );
   maug_cleanup_if_not_ok();

   win->flags |= RETROWIN3D_FLAG_INIT_GUI;

   /* TODO: Parse font properties from filename. */
   retval = retrofont_load( font_filename, &(win->gui.font_h), 8, 33, 93 );
   maug_cleanup_if_not_ok();

   win->w = w;
   win->h = h;
   win->gui.x = x;
   win->gui.y = y;

   if( w != h ) {
      error_printf(
         "non-square window created; some systems may have trouble!" )
   }

cleanup:

   return retval;
}

#endif /* RETRO3DW_C */

#endif /* !RETROW3D_H */

