
#ifndef RETROW3D_H
#define RETROW3D_H

/**
 * \addtogroup maug_retroflt
 * \{
 * \addtogroup maug_retrowin RetroFlat Window API
 * \brief Builds on \ref maug_retrogui to allow for multiple windows.
 * \{
 * \file retrowin.h
 * \page maug_retrowin_example_page RetroWin Example
 *
 *       struct RETROWIN* win = NULL;
 *       struct MDATA_VECTOR win_stack;
 *
 *       retrowin_push_win(
 *          NULL, / * This window should create and manage its own GUI. * /
 *          &win_stack,
 *          IDC_EXAMPLE_WIN, "unscii_8.hex",
 *          / * Center * /
 *          (retroflat_screen_w() >> 1) - (EXAMPLE_WIN_W >> 1),
 *          / * Center * /
 *          (retroflat_screen_h() >> 1) - (EXAMPLE_WIN_H >> 1),
 *          EXAMPLE_WIN_W, EXAMPLE_WIN_H, RETROWIN_FLAG_BORDER_BLUE );
 *
 *       mdata_vector_lock( &win_stack );
 *       win = mdata_vector_get_last( &win_stack, struct RETROWIN );
 *       maug_cleanup_if_null_lock( struct RETROWIN*, win );
 *
 *       / * Lock macros will automatically fail to cleanup: label if something
 *         * goes wrong, so no error handling here!
 *         * /
 *       retrowin_lock_gui( win );
 *
 *       / *** Pause Example to Insert Controls *** /
 *
 * For inserting controls into a ::RETROGUI, please see
 * \ref maug_retrogui_example_page. Use win->gui_p as the GUI to lock
 * and push controls onto, instead of the gui_p variable in that example.
 *
 *       / *** Resume Example *** /
 *
 *       / * Unlock window and window stack after controls have been added. * /
 *       retrowin_unlock_gui( win );
 *       mdata_vector_unlock( &win_stack );
 */

#ifndef RETROWIN_TRACE_LVL
#  define RETROWIN_TRACE_LVL 0
#endif /* !RETROWIN_TRACE_LVL */

/**
 * \related RETROWIN
 * \brief Flag for RETROWIN::flags indicating RETROWIN::gui_p should be locked
 *        from RETROWIN::gui_h before use.
 */
#define RETROWIN_FLAG_INIT_GUI          0x10

#define RETROWIN_FLAG_INIT_BMP          0x20

#define RETROWIN_FLAG_GUI_LOCKED        0x04

#define RETROWIN_FLAG_BORDER_NONE              0x00
#define RETROWIN_FLAG_BORDER_GRAY              0x01
#define RETROWIN_FLAG_BORDER_BLUE              0x02

#define retrowin_win_is_active( win ) \
   (RETROWIN_FLAG_INIT_BMP == (RETROWIN_FLAG_INIT_BMP & (win)->flags))

#define RETROWIN_FLAG_BORDER_MASK              0x03

#define retrowin_lock_gui( win ) \
   if( \
      RETROWIN_FLAG_INIT_GUI == (RETROWIN_FLAG_INIT_GUI & (win)->flags) && \
      RETROWIN_FLAG_GUI_LOCKED != (RETROWIN_FLAG_GUI_LOCKED & (win)->flags) \
   ) { \
      debug_printf( RETROWIN_TRACE_LVL, "locking managed gui handle %p...", \
         (win)->gui_h ); \
      maug_mlock( (win)->gui_h, (win)->gui_p ); \
      maug_cleanup_if_null_lock( struct RETROGUI*, (win)->gui_p ); \
      (win)->flags |= RETROWIN_FLAG_GUI_LOCKED; \
      debug_printf( RETROWIN_TRACE_LVL, "locked managed gui to pointer %p!", \
         (win)->gui_p ); \
   }

#define retrowin_unlock_gui( win ) \
   if( \
      RETROWIN_FLAG_INIT_GUI == (RETROWIN_FLAG_INIT_GUI & (win)->flags) && \
      RETROWIN_FLAG_GUI_LOCKED == (RETROWIN_FLAG_GUI_LOCKED & (win)->flags) \
   ) { \
      debug_printf( RETROWIN_TRACE_LVL, "unlocking managed gui pointer %p...", \
         (win)->gui_h ); \
      maug_munlock( (win)->gui_h, (win)->gui_p ); \
      (win)->flags &= ~RETROWIN_FLAG_GUI_LOCKED; \
   }

struct RETROWIN {
   uint8_t flags;
   size_t idc;
   size_t x;
   size_t y;
   MAUG_MHANDLE gui_h;
   struct RETROGUI* gui_p;
   retroflat_blit_t gui_bmp;
};

MERROR_RETVAL retrowin_redraw_win_stack( struct MDATA_VECTOR* win_stack );

/**
 * \brief Force all windows on the stack to redraw.
 */
MERROR_RETVAL retrowin_refresh_win_stack( struct MDATA_VECTOR* win_stack );

retrogui_idc_t retrowin_poll_win_stack(
   struct MDATA_VECTOR* win_stack, retrogui_idc_t idc_active,
   RETROFLAT_IN_KEY* p_input, struct RETROFLAT_INPUT* input_evt );

void retrowin_free_win( struct RETROWIN* win );

ssize_t retrowin_get_by_idc( size_t idc, struct MDATA_VECTOR* win_stack );

/**
 * \brief Create a new window on the given win_stack.
 * \param gui Pointer to a ::RETROGUI already initialized with retrogui_init(),
 *            or NULL if the window should create and manage its own RETROGUI.
 * \param win_stack The vector on which windows are stored.
 * \param font_filename Font to load into the GUI. Only used if this window
 *                      will manage its own GUI.
 */
ssize_t retrowin_push_win(
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

static MERROR_RETVAL _retrowin_draw_border( struct RETROWIN* win ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert(
      RETROWIN_FLAG_INIT_GUI != (RETROWIN_FLAG_INIT_GUI & win->flags) ||
      RETROWIN_FLAG_GUI_LOCKED == (RETROWIN_FLAG_GUI_LOCKED & win->flags) );

   switch( RETROWIN_FLAG_BORDER_MASK & win->flags ) {
   case RETROWIN_FLAG_BORDER_NONE:
      retroflat_2d_rect(
         win->gui_p->draw_bmp, RETROFLAT_COLOR_BLACK,
         0, 0,
         retroflat_2d_bitmap_w( win->gui_p->draw_bmp ),
         retroflat_2d_bitmap_h( win->gui_p->draw_bmp ),
         RETROFLAT_FLAGS_FILL );
      break;

   case RETROWIN_FLAG_BORDER_GRAY:
      retroflat_2d_rect(
         win->gui_p->draw_bmp,
         2 < retroflat_screen_colors() ?
            RETROFLAT_COLOR_GRAY : RETROFLAT_COLOR_WHITE, 0, 0,
         retroflat_2d_bitmap_w( win->gui_p->draw_bmp ),
         retroflat_2d_bitmap_h( win->gui_p->draw_bmp ),
         RETROFLAT_FLAGS_FILL );

      /* Draw the border. */
      retroflat_2d_rect(
         win->gui_p->draw_bmp,
         2 < retroflat_screen_colors() ?
            RETROGUI_COLOR_BORDER : RETROFLAT_COLOR_BLACK, 0, 0,
         retroflat_2d_bitmap_w( win->gui_p->draw_bmp ),
         retroflat_2d_bitmap_h( win->gui_p->draw_bmp ),
         0 );
      if( 2 < retroflat_screen_colors() ) {
         /* Draw highlight lines only visible in >2-color mode. */
         retroflat_2d_line(
            win->gui_p->draw_bmp, RETROFLAT_COLOR_WHITE,
            1, 1, retroflat_2d_bitmap_w( win->gui_p->draw_bmp ) - 2, 1, 0 );
         retroflat_2d_line(
            win->gui_p->draw_bmp, RETROFLAT_COLOR_WHITE,
            1, 2, 1, retroflat_2d_bitmap_h( win->gui_p->draw_bmp ) - 3, 0 );
      }
      break;

   case RETROWIN_FLAG_BORDER_BLUE:
      retroflat_2d_rect(
         win->gui_p->draw_bmp,
         2 < retroflat_screen_colors() ?
            RETROFLAT_COLOR_BLUE : RETROFLAT_COLOR_BLACK, 0, 0,
         retroflat_2d_bitmap_w( win->gui_p->draw_bmp ),
         retroflat_2d_bitmap_h( win->gui_p->draw_bmp ),
         RETROFLAT_FLAGS_FILL );

      /* Draw the border. */
      retroflat_2d_rect(
         win->gui_p->draw_bmp,
         2 < retroflat_screen_colors() ?
            RETROGUI_COLOR_BORDER : RETROFLAT_COLOR_BLACK, 2, 2,
         retroflat_2d_bitmap_w( win->gui_p->draw_bmp ) - 4,
         retroflat_2d_bitmap_h( win->gui_p->draw_bmp ) - 4,
         0 );
      retroflat_2d_rect(
         win->gui_p->draw_bmp, RETROFLAT_COLOR_WHITE, 1, 1,
         retroflat_2d_bitmap_w( win->gui_p->draw_bmp ) - 2,
         retroflat_2d_bitmap_h( win->gui_p->draw_bmp ) - 2,
         0 );
      retroflat_2d_rect(
         win->gui_p->draw_bmp,
         2 < retroflat_screen_colors() ?
            RETROGUI_COLOR_BORDER : RETROFLAT_COLOR_BLACK, 0, 0,
         retroflat_2d_bitmap_w( win->gui_p->draw_bmp ),
         retroflat_2d_bitmap_h( win->gui_p->draw_bmp ),
         0 );
      break;
   }

   return retval;
}

/* === */

static MERROR_RETVAL _retrowin_redraw_win( struct RETROWIN* win ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert(
      RETROWIN_FLAG_INIT_GUI != (RETROWIN_FLAG_INIT_GUI & win->flags) ||
      RETROWIN_FLAG_GUI_LOCKED == (RETROWIN_FLAG_GUI_LOCKED & win->flags) );

   retroflat_2d_lock_bitmap( &(win->gui_bmp) );

   /* Dirty detection is in retrogui_redraw_ctls(). */
   win->gui_p->draw_bmp = &(win->gui_bmp);

   debug_printf( RETROWIN_TRACE_LVL,
      "redrawing window " SIZE_T_FMT " (GUI %p)...", win->idc, win->gui_p );

   _retrowin_draw_border( win );

   /* This is a bit of a hack... Set X/Y to 0 so that we draw at the top
    * of the bitmap that will be used as a texture. Reset it below so input
    * detection works!
    */
   win->gui_p->x = 0;
   win->gui_p->y = 0;
   retval = retrogui_redraw_ctls( win->gui_p );
   win->gui_p->x = win->x;
   win->gui_p->y = win->y;
   maug_cleanup_if_not_ok();

   retroflat_2d_release_bitmap( &(win->gui_bmp) );

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

      retrowin_lock_gui( win );

      /* OpenGL tends to call glClear on every frame, so always redraw! */
      if( RETROGUI_FLAGS_DIRTY == (RETROGUI_FLAGS_DIRTY & win->gui_p->flags) ) {
         debug_printf( RETROWIN_TRACE_LVL,
            "redrawing window idx " SIZE_T_FMT ", IDC " SIZE_T_FMT,
            i, win->idc );

         /* Redraw the window bitmap, including controls. */
         retval = _retrowin_redraw_win( win );
         maug_cleanup_if_not_ok();
      }

      /* Always blit the finished window to the screen, to compensate for e.g.
       * the implicit screen-clearing of the engine loop.
       */
      retval = retroflat_2d_blit_win(
         &(win->gui_bmp), win->gui_p->x, win->gui_p->y );

      retrowin_unlock_gui( win );
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

      retrowin_lock_gui( win );

      debug_printf( RETROWIN_TRACE_LVL,
         "refreshing window idx " SIZE_T_FMT ", IDC " SIZE_T_FMT,
         i, win->idc );

      win->gui_p->flags |= RETROGUI_FLAGS_DIRTY;

      retrowin_unlock_gui( win );
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

      retrowin_lock_gui( win );

      idc_out = retrogui_poll_ctls( win->gui_p, p_input, input_evt );

      retrowin_unlock_gui( win );

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
   MERROR_RETVAL retval = MERROR_OK;

   if( RETROWIN_FLAG_INIT_BMP == (RETROWIN_FLAG_INIT_BMP & win->flags) ) {
      retroflat_2d_destroy_bitmap( &(win->gui_bmp) );
      retrowin_lock_gui( win );
      win->gui_p->draw_bmp = NULL;
      retrowin_unlock_gui( win );
   }

   if( RETROWIN_FLAG_INIT_GUI == (RETROWIN_FLAG_INIT_GUI & win->flags) ) {
      /* This GUI was created by a NULL to push_win(). */
      retrowin_lock_gui( win )
      retrogui_destroy( win->gui_p );
      retrowin_unlock_gui( win )
      maug_mfree( win->gui_h );
   }

cleanup:

   if( MERROR_OK != retval ) {
      error_printf( "error while locking self-managed GUI! not freed!" );
   }

   maug_mzero( win, sizeof( struct RETROWIN ) );
}

/* === */

ssize_t retrowin_get_by_idc( size_t idc, struct MDATA_VECTOR* win_stack ) {
   ssize_t idx_out = -1;
   int autolock = 0;
   size_t i = 0;
   struct RETROWIN* win = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   if( 0 == mdata_vector_ct( win_stack ) ) {
      goto cleanup;
   }

   if( !mdata_vector_is_locked( win_stack ) ) {
      mdata_vector_lock( win_stack );
      autolock = 1;
   }

   for( i = 0 ; mdata_vector_ct( win_stack ) > i ; i++ ) {
      win = mdata_vector_get( win_stack, i, struct RETROWIN );
      if( idc == win->idc ) {
         idx_out = i;
         goto cleanup;
      }
   }

cleanup:

   if( autolock ) {
      mdata_vector_unlock( win_stack );
   }

   if( MERROR_OK != retval ) {
      idx_out = merror_retval_to_sz( retval );
   }

   return idx_out;
}

/* === */

ssize_t retrowin_push_win(
   struct RETROGUI* gui, struct MDATA_VECTOR* win_stack,
   size_t idc, const char* font_filename,
   size_t x, size_t y, size_t w, size_t h, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN win;
   ssize_t idx_out = -1;

   idx_out = retrowin_get_by_idc( idc, win_stack );
   if( 0 <= idx_out ) {
      error_printf( "window IDC " SIZE_T_FMT " already exists!", idc );
      goto cleanup;
   }

   maug_mzero( &win, sizeof( struct RETROWIN ) );

   win.flags = 0;

   if( NULL != gui ) {
      win.gui_p = gui;
   } else {
      win.gui_p = NULL;
      win.gui_h = maug_malloc( 1, sizeof( struct RETROGUI ) );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, win.gui_h );

      win.flags |= RETROWIN_FLAG_INIT_GUI;

      /* Prepare gui_p for use as if it had just been assigned. */
      retrowin_lock_gui( &win );

      retval = retrogui_init( win.gui_p );
      maug_cleanup_if_not_ok();

      retval = retrogui_set_font( win.gui_p, font_filename );
      maug_cleanup_if_not_ok();
   }

   retval = retroflat_2d_create_bitmap( w, h, &(win.gui_bmp), 0 );
   maug_cleanup_if_not_ok();

   win.flags |= RETROWIN_FLAG_INIT_BMP;

   win.gui_p->w = w;
   win.gui_p->h = h;
   /* These might seem redundant, but check out retrowin_redraw_win()
    * to see how they're used.
    */
   win.x = x;
   win.y = y;
   win.gui_p->x = x;
   win.gui_p->y = y;
   win.idc = idc;
   win.flags |= flags;

   debug_printf( RETROWIN_TRACE_LVL,
      "pushing window IDC " SIZE_T_FMT ", GUI %p: " SIZE_T_FMT "x" SIZE_T_FMT
      " @ " SIZE_T_FMT ", " SIZE_T_FMT,
      win.idc, win.gui_p,
      win.gui_p->w, win.gui_p->h, win.gui_p->x, win.gui_p->y );

   retrowin_unlock_gui( &win );

   idx_out = mdata_vector_append( win_stack, &win, sizeof( struct RETROWIN ) );
   if( 0 > idx_out ) {
      goto cleanup;
   }

   if( w != h ) {
      /* TODO: Update to detect proportional windows. */
      error_printf(
         "non-square window created; some systems may have trouble!" );
   }

   retrowin_refresh_win_stack( win_stack );

cleanup:

   /* Unlock if unlock above was skipped due to error. */
   retrowin_unlock_gui( &win );

   return idx_out;
}

/* === */

MERROR_RETVAL retrowin_destroy_win(
   struct MDATA_VECTOR* win_stack, size_t idc
) {
   size_t i = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN* win = NULL;
   ssize_t i_free = -1;
   int autolock = 0;

   debug_printf( RETROWIN_TRACE_LVL,
      "attempting to destroy window: " SIZE_T_FMT, idc );

   if( !mdata_vector_is_locked( win_stack ) ) {
      mdata_vector_lock( win_stack );
      autolock = 1;
   }

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
      if( autolock ) {
         mdata_vector_unlock( win_stack );
      }
      mdata_vector_remove( win_stack, i_free );
      if( autolock ) {
         mdata_vector_lock( win_stack );
      }
   }

cleanup:

   if( autolock ) {
      mdata_vector_unlock( win_stack );
   }

   return retval;
}

#endif /* RETROW3D_C */

/*! \} */ /* maug_retrowin */

/*! \} */ /* maug_retroflt */

#endif /* !RETROW3D_H */

