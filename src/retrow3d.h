
#ifndef RETROW3D_H
#define RETROW3D_H

#ifndef RETROWIN3D_TRACE_LVL
#  define RETROWIN3D_TRACE_LVL 0
#endif /* !RETROWIN3D_TRACE_LVL */

#define RETROWIN3D_FLAG_INIT_GUI          0x01

#define RETROWIN3D_FLAG_INIT_BMP          0x02

#define retro3dw_win_is_active( win ) \
   (RETROWIN3D_FLAG_INIT_BMP == (RETROWIN3D_FLAG_INIT_BMP & (win)->flags))

struct RETROWIN3D {
   uint8_t flags;
   size_t idc;
   size_t x;
   size_t y;
   struct RETROGUI* gui;
   struct RETROFLAT_BITMAP gui_bmp;
};

MERROR_RETVAL retro3dw_redraw_win( struct RETROWIN3D* win );

MERROR_RETVAL retro3dw_redraw_win_stack( struct MDATA_VECTOR* win_stack );

retrogui_idc_t retro3dw_poll_win_stack( 
   struct MDATA_VECTOR* win_stack, retrogui_idc_t idc_active,
   RETROFLAT_IN_KEY* p_input, struct RETROFLAT_INPUT* input_evt );

void retro3dw_free_win( struct RETROWIN3D* win );

/**
 * \brief Create a new window.
 * \param gui Pointer to a preinitialized ::RETROGUI or NULL if the window
 *            should create and manage its own.
 * \param font_filename Font to load into the GUI. Only used if this window
 *                      will manage its own GUI.
 */
MERROR_RETVAL retro3dw_push_win(
   struct RETROGUI* gui, struct MDATA_VECTOR* win_stack,
   size_t idc, const char* font_filename,
   size_t x, size_t y, size_t w, size_t h, uint8_t flags );

MERROR_RETVAL retro3dw_destroy_win(
   struct MDATA_VECTOR* win_stack, size_t idc );

#ifdef RETROW3D_C

MERROR_RETVAL retro3dw_redraw_win( struct RETROWIN3D* win ) {
   float aspect_ratio = 0,
      screen_x = 0,
      screen_y = 0,
      gui_w_f = 0,
      gui_h_f = 0;
   MERROR_RETVAL retval = MERROR_OK;

   assert( 0 < win->gui_bmp.tex.id );

    /* Prepare projection to draw the overlay on the top of the screen.
     * Do this before any possible failure, so that the retroglu_pop_overlay()
     * in cleanup: is always needed!
     */
   retroglu_push_overlay(
      win->gui->x, win->gui->y, screen_x, screen_y, aspect_ratio );
   retval = retroglu_check_errors( "overlay push" );
   maug_cleanup_if_not_ok();

   retroglu_whf( win->gui->w, win->gui->h, gui_w_f, gui_h_f, aspect_ratio );

   retroflat_draw_lock( &(win->gui_bmp) );
   maug_cleanup_if_null_lock( uint8_t*, win->gui_bmp.tex.bytes );

   /* Dirty detection is in retrogui_redraw_ctls(). */
   win->gui->draw_bmp = &(win->gui_bmp);
   /* This is a bit of a hack... Set X/Y to 0 so that we draw at the top
    * of the bitmap that will be used as a texture. Reset it below so input
    * detection works!
    */
   win->gui->x = 0;
   win->gui->y = 0;
   retrogui_redraw_ctls( win->gui );
   win->gui->x = win->x;
   win->gui->y = win->y;

   /* Set a white background for the overlay poly. */
   glColor3fv( RETROGLU_COLOR_WHITE );
   retval = retroglu_check_errors( "overlay color" );
   maug_cleanup_if_not_ok();

   /* debug_printf( 1, "overlay texture: %d x %d (%p)",
      win->gui_bmp.tex.w, win->gui_bmp.tex.h,
      win->gui_bmp.tex.bytes ); */
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
      win->gui_bmp.tex.w, win->gui_bmp.tex.h, 0,
      GL_RGBA, GL_UNSIGNED_BYTE,
      win->gui_bmp.tex.bytes );

   retval = retroglu_check_errors( "overlay texture" );
   maug_cleanup_if_not_ok();

   /* Make the background transparency work. */
   glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   /* Break up overlay into multiple triangles with offset textures. */

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

   /* Clear texture after drawing. */
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, NULL ); 

cleanup:

   /* This can be called even if not needed. */
   retroflat_draw_release( &(win->gui_bmp) );

   retroglu_pop_overlay();

   return retval;
}

MERROR_RETVAL retro3dw_redraw_win_stack( struct MDATA_VECTOR* win_stack ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   struct RETROWIN3D* win = NULL;

   assert( !mdata_vector_is_locked( win_stack ) );
   mdata_vector_lock( win_stack );

   for( i = 0 ; mdata_vector_ct( win_stack ) > i ; i++ ) {
      win = mdata_vector_get( win_stack, i, struct RETROWIN3D );
      assert( NULL != win );

      if( !retro3dw_win_is_active( win ) ) {
         continue;
      }

      debug_printf( RETROWIN3D_TRACE_LVL, "redrawing window: " SIZE_T_FMT,
         win->idc );

      retrogui_lock( win->gui );
      retval = retro3dw_redraw_win( win );
      retrogui_unlock( win->gui );
      maug_cleanup_if_not_ok();
   }

cleanup:

   mdata_vector_unlock( win_stack );

   return retval;
}

retrogui_idc_t retro3dw_poll_win_stack( 
   struct MDATA_VECTOR* win_stack, retrogui_idc_t idc_active,
   RETROFLAT_IN_KEY* p_input, struct RETROFLAT_INPUT* input_evt
) {
   size_t i = 0;
   retrogui_idc_t idc_out = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN3D* win = NULL;

   assert( !mdata_vector_is_locked( win_stack ) );
   mdata_vector_lock( win_stack );

   for( i = 0 ; mdata_vector_ct( win_stack ) > i ; i++ ) {
      win = mdata_vector_get( win_stack, i, struct RETROWIN3D );
      assert( NULL != win );
      if( idc_active != win->idc || !retro3dw_win_is_active( win ) ) {
         continue;
      }

      debug_printf( RETROWIN3D_TRACE_LVL, "polling window: " SIZE_T_FMT,
         win->idc );

      retrogui_lock( win->gui );

      idc_out = retrogui_poll_ctls( win->gui, p_input, input_evt );

      retrogui_unlock( win->gui );

      break;
   }

cleanup:

   if( MERROR_OK != retval ) {
      error_printf( "error redrawing windows!" );
      idc_out = 0;
   }

   mdata_vector_unlock( win_stack );

   return idc_out;
}

void retro3dw_free_win( struct RETROWIN3D* win ) {

   if( RETROWIN3D_FLAG_INIT_BMP == (RETROWIN3D_FLAG_INIT_BMP & win->flags) ) {
      retroflat_destroy_bitmap( &(win->gui_bmp) );
      win->gui->draw_bmp = NULL;
   }

   if( RETROWIN3D_FLAG_INIT_GUI == (RETROWIN3D_FLAG_INIT_GUI & win->flags) ) {
#ifndef RETROGXC_PRESENT
      if( (MAUG_MHANDLE)NULL != win->gui->font_h ) {
         maug_mfree( win->gui->font_h );
      }
#endif /* RETROGXC_PRESENT */

      /* This GUI was created by a NULL to push_win(). */
      retrogui_free( win->gui );
      free( win->gui );
   }

   maug_mzero( win, sizeof( struct RETROWIN3D ) );
}

MERROR_RETVAL retro3dw_push_win(
   struct RETROGUI* gui, struct MDATA_VECTOR* win_stack,
   size_t idc, const char* font_filename,
   size_t x, size_t y, size_t w, size_t h, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN3D win;
   ssize_t append_retval = 0;

   /* TODO: Implement background image. */

   maug_mzero( &win, sizeof( struct RETROWIN3D ) );

   win.flags = 0;

   if( NULL != gui ) {
      win.gui = gui;
   } else {
      /* TODO: Use maug_malloc(). */
      win.gui = calloc( 1, sizeof( struct RETROGUI ) );
      maug_cleanup_if_null_alloc( struct RETROGUI*, win.gui );

      retval = retrogui_init( win.gui );
      maug_cleanup_if_not_ok();

      win.flags |= RETROWIN3D_FLAG_INIT_GUI;

      /* TODO: Parse font height from filename and only load printable glyphs. */
      /* TODO: Use cache if available. */
      retval = retrofont_load( font_filename, &(win.gui->font_h), 0, 33, 93 );
      maug_cleanup_if_not_ok();
   }

   retval = retroflat_create_bitmap( w, h, &(win.gui_bmp), 0 );
   maug_cleanup_if_not_ok();

   win.flags |= RETROWIN3D_FLAG_INIT_BMP;

   win.gui->w = w;
   win.gui->h = h;
   /* These might seem redundant, but check out retro3dw_redraw_win()
    * to see how they're used.
    */
   win.x = x;
   win.y = y;
   win.gui->x = x;
   win.gui->y = y;
   win.idc = idc;
   win.flags |= flags;

   debug_printf( RETROWIN3D_TRACE_LVL,
      "pushed window: " SIZE_T_FMT ": " SIZE_T_FMT "x" SIZE_T_FMT
      " @ " SIZE_T_FMT ", " SIZE_T_FMT,
      win.idc, win.gui->w, win.gui->h, win.gui->x, win.gui->y );

   append_retval = mdata_vector_append(
      win_stack, &win, sizeof( struct RETROWIN3D ) );
   if( 0 > append_retval ) {
      retval = mdata_retval( append_retval );
      goto cleanup;
   }

   if( w != h ) {
      error_printf(
         "non-square window created; some systems may have trouble!" )
   }

cleanup:

   return retval;
}

MERROR_RETVAL retro3dw_destroy_win(
   struct MDATA_VECTOR* win_stack, size_t idc
) {
   size_t i = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN3D* win = NULL;

   debug_printf( RETROWIN3D_TRACE_LVL,
      "attempting to destroy window: " SIZE_T_FMT, idc );

   assert( !mdata_vector_is_locked( win_stack ) );
   mdata_vector_lock( win_stack );

   for( i = 0 ; mdata_vector_ct( win_stack ) > i ; i++ ) {
      win = mdata_vector_get( win_stack, i, struct RETROWIN3D );
      assert( NULL != win );
      if( idc != win->idc || !retro3dw_win_is_active( win ) ) {
         continue;
      }

      debug_printf( RETROWIN3D_TRACE_LVL, "freeing window: " SIZE_T_FMT,
         win->idc );

      retro3dw_free_win( win );

      break;
   }

cleanup:

   mdata_vector_unlock( win_stack );

   return retval;
}

#endif /* RETROW3D_C */

#endif /* !RETROW3D_H */

