
#ifndef RETROW3D_H
#define RETROW3D_H

#ifndef RETROWIN3D_TRACE_LVL
#  define RETROWIN3D_TRACE_LVL 0
#endif /* !RETROWIN3D_TRACE_LVL */

#define RETROWIN3D_FLAG_INIT_GUI          0x01

#define RETROWIN3D_FLAG_INIT_BMP          0x02

struct RETROWIN3D {
   uint8_t flags;
   size_t idc;
   size_t w;
   size_t h;
   struct RETROGUI gui;
   struct RETROFLAT_BITMAP gui_bmp;
};

MERROR_RETVAL retro3dw_redraw_win( struct RETROWIN3D* win );

MERROR_RETVAL retro3dw_redraw_win_stack(
   struct RETROWIN3D* win_stack, size_t win_stack_sz );

RETROGUI_IDC retro3dw_poll_win( 
   struct RETROWIN3D* win_stack, size_t win_stack_sz, RETROGUI_IDC idc,
   RETROFLAT_IN_KEY* p_input, struct RETROFLAT_INPUT* input_evt );

void retro3dw_free( struct RETROWIN3D* win );

MERROR_RETVAL retro3dw_push_win(
   struct RETROWIN3D* win_stack, size_t win_stack_sz,
   size_t idc, const char* font_filename,
   size_t x, size_t y, size_t w, size_t h, uint8_t flags );

#ifdef RETRO3DW_C

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
      win->gui.x, win->gui.y, screen_x, screen_y, aspect_ratio );
   retval = retroglu_check_errors( "overlay push" );
   maug_cleanup_if_not_ok();

   retroglu_whf( win->w, win->h, gui_w_f, gui_h_f, aspect_ratio );

   retroflat_draw_lock( &(win->gui_bmp) );
   maug_cleanup_if_null_lock( uint8_t*, win->gui_bmp.tex.bytes );

   /* Dirty detection is in retrogui_redraw_ctls(). */
   win->gui.draw_bmp = &(win->gui_bmp);
   retrogui_redraw_ctls( &(win->gui) );

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

MERROR_RETVAL retro3dw_redraw_win_stack(
   struct RETROWIN3D* win_stack, size_t win_stack_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   for( i = 0 ; win_stack_sz > i ; i++ ) {
      if(
         RETROWIN3D_FLAG_INIT_GUI !=
            (RETROWIN3D_FLAG_INIT_GUI & win_stack[i].flags)
      ) {
         continue;
      }

      debug_printf( RETROWIN3D_TRACE_LVL, "redrawing window: " SIZE_T_FMT,
         win_stack[i].idc );

      retrogui_lock( &(win_stack[i].gui) );
      retval = retro3dw_redraw_win( &(win_stack[i]) );
      retrogui_unlock( &(win_stack[i].gui) );
      maug_cleanup_if_not_ok();
   }

cleanup:

   return retval;
}

RETROGUI_IDC retro3dw_poll_win( 
   struct RETROWIN3D* win_stack, size_t win_stack_sz, RETROGUI_IDC idc,
   RETROFLAT_IN_KEY* p_input, struct RETROFLAT_INPUT* input_evt
) {
   size_t i = 0;
   RETROGUI_IDC idc_out = 0;
   MERROR_RETVAL retval = MERROR_OK;

   for( i = 0 ; win_stack_sz > i ; i++ ) {
      if(
         idc != win_stack[i].idc ||
         RETROWIN3D_FLAG_INIT_GUI !=
            (RETROWIN3D_FLAG_INIT_GUI & win_stack[i].flags)
      ) {
         continue;
      }

      debug_printf( RETROWIN3D_TRACE_LVL, "polling window: " SIZE_T_FMT,
         win_stack[i].idc );

      retrogui_lock( &(win_stack[i].gui) );

      idc = retrogui_poll_ctls( &(win_stack[i].gui), p_input, input_evt );

      retrogui_unlock( &(win_stack[i].gui) );

      break;
   }

cleanup:

   if( MERROR_OK != retval ) {
      error_printf( "error redrawing windows!" );
      idc_out = 0;
   }

   return idc_out;
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

MERROR_RETVAL retro3dw_push_win(
   struct RETROWIN3D* win_stack, size_t win_stack_sz,
   size_t idc, const char* font_filename,
   size_t x, size_t y, size_t w, size_t h, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN3D* win = &(win_stack[0]);

   /* TODO: Implement background image. */

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
   win->idc = idc;
   win->flags |= flags;

   debug_printf( RETROWIN3D_TRACE_LVL,
      "pushed window: " SIZE_T_FMT ": " SIZE_T_FMT "x" SIZE_T_FMT
      " @ " SIZE_T_FMT ", " SIZE_T_FMT,
      win->idc, win->w, win->h, win->gui.x, win->gui.y );

   if( w != h ) {
      error_printf(
         "non-square window created; some systems may have trouble!" )
   }

cleanup:

   return retval;
}

#endif /* RETRO3DW_C */

#endif /* !RETROW3D_H */

