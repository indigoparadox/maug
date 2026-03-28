
#ifndef RETRODLG_H
#define RETRODLG_H

#ifndef RETROWIN_NO_DLG

/**
 * \addtogroup maug_retroflt
 * \{
 * \addtogroup maug_retrodlg RetroFlat Common Dialogs
 * \brief Provides some common dialog types using \ref maug_retrogui and
 *        \ref maug_retrowin.
 * \{
 * \file retrodlg.h
 */

#ifndef RETRODLG_TRACE_LVL
#  define RETRODLG_TRACE_LVL 0
#endif /* !RETRODLG_TRACE_LVL */

#ifndef RETRODLG_IDC_CHAT_LABEL
#  define RETRODLG_IDC_CHAT_LABEL 100
#endif /* !RETRODLG_IDC_CHAT_LABEL */

#ifndef RETRODLG_IDC_CHAT_PORTRAIT
#  define RETRODLG_IDC_CHAT_PORTRAIT 200
#endif /* !RETRODLG_IDC_CHAT_PORTRAIT */

MERROR_RETVAL retrodlg_chat_update(
   maug_path portrait_path, const char* msg, size_t msg_sz,
   size_t chat_idc, struct MDATA_VECTOR* win_stack );

MERROR_RETVAL retrodlg_chat_show_ui(
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   maug_path font_path, size_t chat_idc, uint8_t win_flags,
   struct MDATA_VECTOR* win_stack );

#ifdef RETRODLG_C

MERROR_RETVAL retrodlg_chat_update(
   maug_path portrait_path, const char* msg, size_t msg_sz,
   size_t chat_idc, struct MDATA_VECTOR* win_stack
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROWIN* win = NULL;
   ssize_t win_idx = -1;

   win_idx = retrowin_get_by_idc( chat_idc, win_stack );
   if( 0 > win_idx ) {
      /* Need to open chat window first! */
      error_printf( "chat window not currently open!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

#if RETRODLG_TRACE_LVL > 0
   debug_printf( RETRODLG_TRACE_LVL,
      "portrait %s: say %s", portrait_path, msg );
#endif /* RETRODLG_TRACE_LVL */

   mdata_vector_lock( win_stack );
   win = mdata_vector_get( win_stack, win_idx, struct RETROWIN );
   assert( NULL != win );
   retrowin_lock_gui( win );

   /* Update the GUI. */
   if( NULL != msg ) {
      retrogui_set_ctl_text(
         win->gui_p, RETRODLG_IDC_CHAT_LABEL, msg_sz, "%s", msg );
   } else {
      retrogui_set_ctl_text( win->gui_p, RETRODLG_IDC_CHAT_LABEL, 0, NULL );
   }

   if( '\0' != portrait_path[0] ) {
      retrogui_set_ctl_image(
         win->gui_p, RETRODLG_IDC_CHAT_PORTRAIT, portrait_path, 0 );
   } else {
      retrogui_set_ctl_image(
         win->gui_p, RETRODLG_IDC_CHAT_PORTRAIT, NULL, 0 );
   }

   retrowin_unlock_gui( win );

cleanup:

   if( mdata_vector_is_locked( win_stack ) ) {
      mdata_vector_unlock( win_stack );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrodlg_chat_show_ui(
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   maug_path font_path, size_t chat_idc, uint8_t win_flags,
   struct MDATA_VECTOR* win_stack
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL ctl;
   struct RETROWIN* win = NULL;
   ssize_t chat_win = -1;

   chat_win = retrowin_push_win(
      NULL, /* This window should create and manage its own GUI. */
      win_stack, chat_idc, font_path,
      x, y, w, h, win_flags );
   if( 0 > chat_win ) {
      retval = merror_sz_to_retval( chat_win );
      goto cleanup;
   }

#if RETRODLG_TRACE_LVL < 0
   debug_printf( RETRODLG_TRACE_LVL,
      "chat window is now idx: " SSIZE_T_FMT, chat_win );
#endif /* RETRODLG_TRACE_LVL */

   mdata_vector_lock( win_stack );
   win = mdata_vector_get( win_stack, chat_win, struct RETROWIN );
   assert( NULL != win );
   retrowin_lock_gui( win );

      retrogui_init_ctl(
         &ctl, RETROGUI_CTL_TYPE_LABEL, RETRODLG_IDC_CHAT_LABEL );

      ctl.base.x = 32;
      ctl.base.y = 2;
      ctl.base.w = w - 32;
      ctl.base.h = 20;
      ctl.base.fg_color = RETROFLAT_COLOR_WHITE;
      ctl.LABEL.label = "";
      ctl.LABEL.label_sz = 0;

      retval = retrogui_push_ctl( win->gui_p, &ctl );
      maug_cleanup_if_not_ok();

      /* Portrait */

      retrogui_init_ctl(
         &ctl, RETROGUI_CTL_TYPE_IMAGE, RETRODLG_IDC_CHAT_PORTRAIT );

      ctl.base.x = 8;
      ctl.base.y = 8;
      ctl.base.w = RETROFLAT_TILE_W;
      ctl.base.h = RETROFLAT_TILE_H;

      retval = retrogui_push_ctl( win->gui_p, &ctl );
      maug_cleanup_if_not_ok();

   retrowin_unlock_gui( win );

cleanup:

   if( mdata_vector_is_locked( win_stack ) ) {
      mdata_vector_unlock( win_stack );
   }

   return retval;
}

#endif /* RETRODLG_C */

/*! \} */ /* maug_retrodlg */

/*! \} */ /* maug_retroflt */

#endif /* !RETROWIN_NO_DLG */

#endif /* !RETRODLG_H */

