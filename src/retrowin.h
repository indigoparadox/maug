
#ifndef RETROW3D_H
#define RETROW3D_H

#ifndef RETROWIN_TRACE_LVL
#  define RETROWIN_TRACE_LVL 0
#endif /* !RETROWIN_TRACE_LVL */

#define RETROWIN_FLAG_INIT_GUI          0x10

#define RETROWIN_FLAG_INIT_BMP          0x20

#define RETROWIN_FLAG_BORDER_NONE              0x00
#define RETROWIN_FLAG_BORDER_GRAY              0x01
#define RETROWIN_FLAG_BORDER_BLUE              0x02

#define retrowin_win_is_active( win ) \
   (RETROWIN_FLAG_INIT_BMP == (RETROWIN_FLAG_INIT_BMP & (win)->flags))

#define RETROWIN_FLAG_BORDER_MASK              0x03

struct RETROWIN {
   uint8_t flags;
   size_t idc;
   size_t x;
   size_t y;
   struct RETROGUI* gui;
   retroflat_blit_t gui_bmp;
};

MERROR_RETVAL retrowin_redraw_win( struct RETROWIN* win );

MERROR_RETVAL retrowin_redraw_win_stack( struct MDATA_VECTOR* win_stack );

/**
 * \brief Force all windows on the stack to redraw.
 */
MERROR_RETVAL retrowin_refresh_win_stack( struct MDATA_VECTOR* win_stack );

retrogui_idc_t retrowin_poll_win_stack(
   struct MDATA_VECTOR* win_stack, retrogui_idc_t idc_active,
   RETROFLAT_IN_KEY* p_input, struct RETROFLAT_INPUT* input_evt );

void retrowin_free_win( struct RETROWIN* win );

/**
 * \brief Create a new window.
 * \param gui Pointer to a preinitialized ::RETROGUI or NULL if the window
 *            should create and manage its own.
 * \param font_filename Font to load into the GUI. Only used if this window
 *                      will manage its own GUI.
 */
MERROR_RETVAL retrowin_push_win(
   struct RETROGUI* gui, struct MDATA_VECTOR* win_stack,
   size_t idc, const char* font_filename,
   size_t x, size_t y, size_t w, size_t h, uint8_t flags );

/**
 * \brief Destroy the given window's resources and remove it from the window
 *        stack.
 * \param idc Identifier (NOT index) of the window to destroy.
 */
MERROR_RETVAL retrowin_destroy_win(
   struct MDATA_VECTOR* win_stack, size_t idc );

#ifdef RETROW3D_C

MERROR_RETVAL retrowin_redraw_win( struct RETROWIN* win ) {
   MERROR_RETVAL retval = MERROR_OK;

   retroflat_2d_lock_bitmap( &(win->gui_bmp) );

   /* Dirty detection is in retrogui_redraw_ctls(). */
   win->gui->draw_bmp = &(win->gui_bmp);

   if( RETROGUI_FLAGS_DIRTY == (RETROGUI_FLAGS_DIRTY & win->gui->flags) ) {
      debug_printf( 1, "redrawing window " SIZE_T_FMT "...", win->idc );

      switch( RETROWIN_FLAG_BORDER_MASK & win->flags ) {
      case RETROWIN_FLAG_BORDER_GRAY:
         retroflat_2d_rect(
            win->gui->draw_bmp, RETROFLAT_COLOR_GRAY, 0, 0,
            retroflat_2d_bitmap_w( win->gui->draw_bmp ),
            retroflat_2d_bitmap_h( win->gui->draw_bmp ),
            RETROFLAT_FLAGS_FILL );

         /* Draw the border. */
         retroflat_2d_rect(
            win->gui->draw_bmp, RETROFLAT_COLOR_BLACK, 0, 0,
            retroflat_2d_bitmap_w( win->gui->draw_bmp ),
            retroflat_2d_bitmap_h( win->gui->draw_bmp ),
            0 );
         retroflat_2d_line(
            win->gui->draw_bmp, RETROFLAT_COLOR_WHITE,
            1, 1, retroflat_2d_bitmap_w( win->gui->draw_bmp ) - 2, 1, 0 );
         retroflat_2d_line(
            win->gui->draw_bmp, RETROFLAT_COLOR_WHITE,
            1, 2, 1, retroflat_2d_bitmap_h( win->gui->draw_bmp ) - 3, 0 );
         break;

      case RETROWIN_FLAG_BORDER_BLUE:
         retroflat_2d_rect(
            win->gui->draw_bmp, RETROFLAT_COLOR_BLUE, 0, 0,
            retroflat_2d_bitmap_w( win->gui->draw_bmp ),
            retroflat_2d_bitmap_h( win->gui->draw_bmp ),
            RETROFLAT_FLAGS_FILL );

         /* Draw the border. */
         retroflat_2d_rect(
            win->gui->draw_bmp, RETROFLAT_COLOR_BLACK, 2, 2,
            retroflat_2d_bitmap_w( win->gui->draw_bmp ) - 4,
            retroflat_2d_bitmap_h( win->gui->draw_bmp ) - 4,
            0 );
         retroflat_2d_rect(
            win->gui->draw_bmp, RETROFLAT_COLOR_WHITE, 1, 1,
            retroflat_2d_bitmap_w( win->gui->draw_bmp ) - 2,
            retroflat_2d_bitmap_h( win->gui->draw_bmp ) - 2,
            0 );
         retroflat_2d_rect(
            win->gui->draw_bmp, RETROFLAT_COLOR_BLACK, 0, 0,
            retroflat_2d_bitmap_w( win->gui->draw_bmp ),
            retroflat_2d_bitmap_h( win->gui->draw_bmp ),
            0 );
         break;
      }
   }

   /* This is a bit of a hack... Set X/Y to 0 so that we draw at the top
    * of the bitmap that will be used as a texture. Reset it below so input
    * detection works!
    */
   win->gui->x = 0;
   win->gui->y = 0;
   retval = retrogui_redraw_ctls( win->gui );
   win->gui->x = win->x;
   win->gui->y = win->y;
   maug_cleanup_if_not_ok();

   retroflat_2d_release_bitmap( &(win->gui_bmp) );

   retval = retroflat_2d_blit_win( &(win->gui_bmp), win->gui->x, win->gui->y );

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrowin_redraw_win_stack( struct MDATA_VECTOR* win_stack ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   struct RETROWIN* win = NULL;

   if( 0 == mdata_vector_ct( win_stack ) ) {
      goto cleanup;
   }

   assert( !mdata_vector_is_locked( win_stack ) );
   mdata_vector_lock( win_stack );

   for( i = 0 ; mdata_vector_ct( win_stack ) > i ; i++ ) {
      win = mdata_vector_get( win_stack, i, struct RETROWIN );
      assert( NULL != win );

      if( !retrowin_win_is_active( win ) ) {
         continue;
      }

      debug_printf( RETROWIN_TRACE_LVL, "redrawing window: " SIZE_T_FMT,
         win->idc );

      retrogui_lock( win->gui );
      retval = retrowin_redraw_win( win );
      retrogui_unlock( win->gui );
      maug_cleanup_if_not_ok();
   }

cleanup:

   mdata_vector_unlock( win_stack );

   return retval;
}

/* === */

MERROR_RETVAL retrowin_refresh_win_stack( struct MDATA_VECTOR* win_stack ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   struct RETROWIN* win = NULL;

   if( 0 == mdata_vector_ct( win_stack ) ) {
      goto cleanup;
   }

   assert( !mdata_vector_is_locked( win_stack ) );
   mdata_vector_lock( win_stack );

   for( i = 0 ; mdata_vector_ct( win_stack ) > i ; i++ ) {
      win = mdata_vector_get( win_stack, i, struct RETROWIN );
      assert( NULL != win );

      if( !retrowin_win_is_active( win ) ) {
         continue;
      }

      win->gui->flags |= RETROGUI_FLAGS_DIRTY;
   }

cleanup:

   mdata_vector_unlock( win_stack );

   return retval;
}

/* === */

retrogui_idc_t retrowin_poll_win_stack(
   struct MDATA_VECTOR* win_stack, retrogui_idc_t idc_active,
   RETROFLAT_IN_KEY* p_input, struct RETROFLAT_INPUT* input_evt
) {
   size_t i = 0;
   retrogui_idc_t idc_out = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN* win = NULL;

   assert( !mdata_vector_is_locked( win_stack ) );
   mdata_vector_lock( win_stack );

   for( i = 0 ; mdata_vector_ct( win_stack ) > i ; i++ ) {
      win = mdata_vector_get( win_stack, i, struct RETROWIN );
      assert( NULL != win );
      if( idc_active != win->idc || !retrowin_win_is_active( win ) ) {
         continue;
      }

      debug_printf( RETROWIN_TRACE_LVL, "polling window: " SIZE_T_FMT,
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

/* === */

void retrowin_free_win( struct RETROWIN* win ) {

   if( RETROWIN_FLAG_INIT_BMP == (RETROWIN_FLAG_INIT_BMP & win->flags) ) {
      retroflat_2d_destroy_bitmap( &(win->gui_bmp) );
      win->gui->draw_bmp = NULL;
   }

   if( RETROWIN_FLAG_INIT_GUI == (RETROWIN_FLAG_INIT_GUI & win->flags) ) {
#ifndef RETROGXC_PRESENT
      if( (MAUG_MHANDLE)NULL != win->gui->font_h ) {
         maug_mfree( win->gui->font_h );
      }
#endif /* RETROGXC_PRESENT */

      /* This GUI was created by a NULL to push_win(). */
      retrogui_free( win->gui );
      free( win->gui );
   }

   maug_mzero( win, sizeof( struct RETROWIN ) );
}

/* === */

MERROR_RETVAL retrowin_push_win(
   struct RETROGUI* gui, struct MDATA_VECTOR* win_stack,
   size_t idc, const char* font_filename,
   size_t x, size_t y, size_t w, size_t h, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN win;
   ssize_t append_retval = 0;
   ssize_t font_retval = 0;

   maug_mzero( &win, sizeof( struct RETROWIN ) );

   win.flags = 0;

   if( NULL != gui ) {
      win.gui = gui;
   } else {
      /* TODO: Use maug_malloc(). */
      win.gui = calloc( 1, sizeof( struct RETROGUI ) );
      maug_cleanup_if_null_alloc( struct RETROGUI*, win.gui );

      retval = retrogui_init( win.gui );
      maug_cleanup_if_not_ok();

      win.flags |= RETROWIN_FLAG_INIT_GUI;

      /* TODO: Parse font height from filename and only load printable glyphs. */
      /* TODO: Use cache if available. */
#ifdef RETROGXC_PRESENT
      font_retval = retrogxc_load_font( font_filename, 0, 33, 93 );
      if( 0 > font_retval ) {
         retval = font_retval * -1;
         goto cleanup;
      }
      win.gui->font_idx = font_retval;
#else
      retval = retrofont_load( font_filename, &(win.gui->font_h), 0, 33, 93 );
#endif /* RETROGXC_PRESENT */
      maug_cleanup_if_not_ok();
   }

   retval = retroflat_2d_create_bitmap(
      w, h, &(win.gui_bmp), RETROFLAT_FLAGS_OPAQUE );
   maug_cleanup_if_not_ok();

   win.flags |= RETROWIN_FLAG_INIT_BMP;

   win.gui->w = w;
   win.gui->h = h;
   /* These might seem redundant, but check out retrowin_redraw_win()
    * to see how they're used.
    */
   win.x = x;
   win.y = y;
   win.gui->x = x;
   win.gui->y = y;
   win.idc = idc;
   win.flags |= flags;

   debug_printf( RETROWIN_TRACE_LVL,
      "pushed window: " SIZE_T_FMT ": " SIZE_T_FMT "x" SIZE_T_FMT
      " @ " SIZE_T_FMT ", " SIZE_T_FMT,
      win.idc, win.gui->w, win.gui->h, win.gui->x, win.gui->y );

   append_retval = mdata_vector_append(
      win_stack, &win, sizeof( struct RETROWIN ) );
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

/* === */

MERROR_RETVAL retrowin_destroy_win(
   struct MDATA_VECTOR* win_stack, size_t idc
) {
   size_t i = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN* win = NULL;
   ssize_t i_free = -1;

   debug_printf( RETROWIN_TRACE_LVL,
      "attempting to destroy window: " SIZE_T_FMT, idc );

   assert( !mdata_vector_is_locked( win_stack ) );
   mdata_vector_lock( win_stack );

   for( i = 0 ; mdata_vector_ct( win_stack ) > i ; i++ ) {
      win = mdata_vector_get( win_stack, i, struct RETROWIN );
      assert( NULL != win );
      if( idc != win->idc ) {
         continue;
      }

      if( !retrowin_win_is_active( win ) ) {
         debug_printf( RETROWIN_TRACE_LVL,
            "window IDC " SIZE_T_FMT " found, but not active! (flags: 0x%02x)",
            idc, win->flags );
         continue;
      }

      debug_printf( RETROWIN_TRACE_LVL, "freeing window: " SIZE_T_FMT,
         win->idc );

      retrowin_free_win( win );

      i_free = i;

      break;
   }

   /* Remove the window from the vector if asked to. */
   if( 0 <= i_free ) {
      mdata_vector_unlock( win_stack );
      mdata_vector_remove( win_stack, i_free );
      mdata_vector_lock( win_stack );
   }

cleanup:

   mdata_vector_unlock( win_stack );

   return retval;
}

#endif /* RETROW3D_C */

#endif /* !RETROW3D_H */

