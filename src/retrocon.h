
#ifndef RETROCON_H
#define RETROCON_H

#define RETROCON_DEBOUNCE_WAIT 3

#define RETROCON_FLAG_ACTIVE 0x01

#define RETROCON_IDC_TEXTBOX  1

#define RETROCON_IDC_CON_BASE 10

#define RETROCON_IDC_CLOSE 65535

#ifndef RETROCON_TRACE_LVL
#  define RETROCON_TRACE_LVL 0
#endif /* !RETROCON_TRACE_LVL */

#ifdef RETROCON_DISABLE

struct RETROCON {
   uint8_t flags;
   RETROFLAT_COLOR lbuffer_color;
   RETROFLAT_COLOR sbuffer_color;
   RETROFLAT_COLOR bg_color;
};

#  define retrocon_init( con, fn, x, y, w, h ) (MERROR_OK)

#  define retrocon_add_command( con, cmd, cb, cb_data ) (MERROR_OK)

#  define retrocon_display( con, display )

#  define retrocon_print_line( con, line )

#  define retrocon_exec_line( con, line, line_sz )

#  define retrocon_debounce( con, c )

#  define retrocon_input( con, p_c, input_evt, p_idc, win_stack )

#  define retrocon_shutdown( con )

#else

/**
 * \addtogroup maug_console In-Situ Console API
 * \{
 */

#ifndef RETROCON_TRACE_LVL
#  define RETROCON_TRACE_LVL 0
#endif /* !RETROCON_TRACE_LVL */

#ifndef RETROCON_SBUFFER_SZ_MAX
#  define RETROCON_SBUFFER_SZ_MAX 4096
#endif /* !RETROCON_SBUFFER_SZ_MAX */

#ifndef RETROCON_SBUFFER_LINES_MAX
#  define RETROCON_SBUFFER_LINES_MAX 30
#endif /* !RETROCON_SBUFFER_LINES_MAX */

#ifndef RETROCON_LBUFFER_SZ_MAX
#  define RETROCON_LBUFFER_SZ_MAX 256
#endif /* !RETROCON_LBUFFER_SZ_MAX */

#ifndef RETROCON_ACTIVE_KEY
#  define RETROCON_ACTIVE_KEY RETROFLAT_KEY_GRAVE
#endif /* !RETROCON_ACTIVE_KEY */

#ifndef RETROCON_CB_NAME_SZ_MAX
#  define RETROCON_CB_NAME_SZ_MAX 32
#endif /* !RETROCON_CB_NAME_SZ_MAX */

#ifndef RETROCON_CB_SZ_MAX
#  define RETROCON_CB_SZ_MAX 128
#endif /* !RETROCON_CB_SZ_MAX */

struct RETROCON;

typedef MERROR_RETVAL (*retrocon_cb)(
   struct RETROCON* con, const char* line, size_t line_sz, void* data );

struct RETROCON {
   uint8_t flags;
   struct RETROGUI gui;
   int input_prev;
   int debounce_wait;
   void* callback_data[RETROCON_CB_SZ_MAX];
   char callback_names[RETROCON_CB_SZ_MAX][RETROCON_CB_NAME_SZ_MAX + 1];
   retrocon_cb callbacks[RETROCON_CB_SZ_MAX];
   size_t callbacks_sz;
   RETROFLAT_COLOR lbuffer_color;
   RETROFLAT_COLOR sbuffer_color;
   RETROFLAT_COLOR bg_color;
   size_t sbuffer_lines;
};

MERROR_RETVAL retrocon_init(
   struct RETROCON* con, const char* font_name,
   size_t x, size_t y, size_t w, size_t h );

MERROR_RETVAL retrocon_add_command(
   struct RETROCON* con, const char* cmd, retrocon_cb cb, void* cb_data );

MERROR_RETVAL retrocon_print_line( struct RETROCON* con, const char* line );

MERROR_RETVAL retrocon_exec_line(
   struct RETROCON* con, char* line, size_t line_sz );

int retrocon_debounce( struct RETROCON* con, int c );

/**
 * \brief Process input from retroflat_poll_input() and apply it to the
 *        console, if open.
 *
 * \param p_c Pointer to the input value returned from retroflat_poll_input().
 * \param p_idc_out Pointer to the retrogui_idc_t to set based on console
 *                  input. The value this is initially set to will be used for
 *                  the console window if win_stack is not NULL.
 * \param win_stack If this is not NULL, then the "open console" key will
 *                  put the console in a RETROWIN on the given stack.
 * \note This function will set input to 0 (nothing) if it was handled by
 *       an open console!
 */
MERROR_RETVAL retrocon_input(
   struct RETROCON* con, RETROFLAT_IN_KEY* p_c,
   struct RETROFLAT_INPUT* input_evt,
   retrogui_idc_t* p_idc_out, struct MDATA_VECTOR* win_stack );

MERROR_RETVAL retrocon_display(
   struct RETROCON* con, retroflat_blit_t* gui_bmp );

void retrocon_shutdown( struct RETROCON* con );

#ifdef RETROCON_C

static MERROR_RETVAL retrocon_cmd_print(
   struct RETROCON* con, const char* line, size_t line_sz, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;
   char* print_line = NULL;

   print_line = maug_strchr( line, ' ' );
   if( NULL == print_line ) {
      /* Not technically an error. */
      goto cleanup;
   }

   /* Skip space. */
   print_line++;
   
   retrocon_print_line( con, print_line );

cleanup:

   return retval;
}

static MERROR_RETVAL retrocon_cmd_quit(
   struct RETROCON* con, const char* line, size_t line_sz, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;

   retroflat_quit( 0 );

   return retval;
}

MERROR_RETVAL retrocon_init(
   struct RETROCON* con, const char* font_name,
   size_t x, size_t y, size_t w, size_t h
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL ctl;
   size_t ctl_y_iter = 0;

   debug_printf( RETROCON_TRACE_LVL, "initializing console..." );

   retval = retrogui_init( &(con->gui) );
   maug_cleanup_if_not_ok();

   /* TODO: Parse font height from filename and only load printable glyphs. */
#ifdef RETROGXC_PRESENT
   con->gui.font_idx = retrogxc_load_font( font_name, 0, 33, 93 );
#else
   retval = retrofont_load( font_name, &(con->gui.font_h), 0, 33, 93 );
#endif /* RETROGXC_PRESENT */
   maug_cleanup_if_not_ok();

   con->sbuffer_color = RETROFLAT_COLOR_DARKBLUE;
   con->lbuffer_color = RETROFLAT_COLOR_BLACK;
   con->gui.x = x;
   con->gui.y = y;
   con->gui.w = w;
   con->gui.h = h;

   retrogui_lock( &(con->gui) );

      retrogui_init_ctl(
         &ctl, RETROGUI_CTL_TYPE_TEXTBOX, RETROCON_IDC_TEXTBOX );

      ctl.base.x = 5;
      ctl.base.y = 5;
      ctl.base.w = con->gui.w - 10;
      ctl.base.h = 20; /* TODO: Dynamic height based on font. */
      ctl.base.fg_color = RETROFLAT_COLOR_BLACK;

      retval = retrogui_push_ctl( &(con->gui), &ctl );
      maug_cleanup_if_not_ok();

      ctl_y_iter = ctl.base.y + ctl.base.h + 1;
      while( h - 5 > ctl_y_iter + 8 + 1 ) {
         retrogui_init_ctl(
            &ctl, RETROGUI_CTL_TYPE_LABEL,
            RETROCON_IDC_CON_BASE + con->sbuffer_lines );

         ctl.base.x = 5;
         ctl.base.y = ctl_y_iter;
         ctl.base.w = con->gui.w - 10;
         ctl.base.h = 8; /* TODO: Dynamic height based on font. */
         ctl.base.fg_color = RETROFLAT_COLOR_DARKGRAY;
         ctl.LABEL.label = "xxxxxx";

         retval = retrogui_push_ctl( &(con->gui), &ctl );
         maug_cleanup_if_not_ok();

         ctl_y_iter += ctl.base.h + 1;
         con->sbuffer_lines++;
      }

   retrogui_unlock( &(con->gui) );

   con->gui.focus = RETROCON_IDC_TEXTBOX;

   retval = retrocon_add_command( con, "PRINT", retrocon_cmd_print, NULL );
   maug_cleanup_if_not_ok();
   retval = retrocon_add_command( con, "QUIT", retrocon_cmd_quit, NULL );
   maug_cleanup_if_not_ok();

cleanup:

   return retval;
}

MERROR_RETVAL retrocon_add_command(
   struct RETROCON* con, const char* cmd, retrocon_cb cb, void* cb_data
) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROCON_TRACE_LVL, "adding console command: %s", cmd );

   maug_cleanup_if_ge_overflow( con->callbacks_sz + 1, RETROCON_CB_SZ_MAX );

   maug_strncpy(
      con->callback_names[con->callbacks_sz], cmd, RETROCON_CB_NAME_SZ_MAX );

   con->callbacks[con->callbacks_sz] = cb;

   con->callback_data[con->callbacks_sz] = cb_data;

   con->callbacks_sz++;

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrocon_print_line( struct RETROCON* con, const char* line ) {
   char sbuffer_shift[RETROCON_LBUFFER_SZ_MAX + 1] = { 0 };
   size_t i = 0;
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO: Escape newlines in line. */

   /* Shift existing screen buffer lines all down by one. */
   for( i = con->sbuffer_lines - 1 ; 0 < i ; i-- ) {
      debug_printf( RETROCON_TRACE_LVL,
         "copying sbuffer line " SIZE_T_FMT " to " SIZE_T_FMT "...",
         i - 1, i );
      maug_mzero( sbuffer_shift, RETROCON_LBUFFER_SZ_MAX + 1 );
      retrogui_lock( &(con->gui) );
      retval = retrogui_get_ctl_text(
         &(con->gui), RETROCON_IDC_CON_BASE + i - 1,
         sbuffer_shift, RETROCON_LBUFFER_SZ_MAX );
      retval = retrogui_set_ctl_text(
         &(con->gui), RETROCON_IDC_CON_BASE + i,
         RETROCON_LBUFFER_SZ_MAX, sbuffer_shift );
      retrogui_unlock( &(con->gui) );
      maug_cleanup_if_not_ok();
   }

   /* Put line in first sbuffer line. */
   retrogui_lock( &(con->gui) );
   retval = retrogui_set_ctl_text(
      &(con->gui), RETROCON_IDC_CON_BASE,
      RETROCON_LBUFFER_SZ_MAX, line );
   retrogui_unlock( &(con->gui) );
   maug_cleanup_if_not_ok();

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrocon_exec_line(
   struct RETROCON* con, char* line, size_t line_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   char line_cap[RETROCON_LBUFFER_SZ_MAX + 1];

   /* Create an uppercase for comparison. */
   maug_mzero( line_cap, RETROCON_LBUFFER_SZ_MAX + 1 );
   strncpy( line_cap, line, RETROCON_LBUFFER_SZ_MAX );
   maug_str_upper( line_cap, line_sz );

   /* Find callback with name starting line. */
   for( i = 0 ; con->callbacks_sz > i ; i++ ) {
      if(
         0 == strncmp(
            /* TODO: Compare up to first space in line. */
            con->callback_names[i], line_cap,
            maug_strlen( con->callback_names[i] ) )
      ) {
         retval = con->callbacks[i](
            con, line, line_sz, con->callback_data[i] );
         goto cleanup;
      }
   }

   retrocon_print_line( con, "COMMAND NOT FOUND!" );

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrocon_input(
   struct RETROCON* con, RETROFLAT_IN_KEY* p_c,
   struct RETROFLAT_INPUT* input_evt,
   retrogui_idc_t* p_idc_out, struct MDATA_VECTOR* win_stack
) {
   MERROR_RETVAL retval = MERROR_OK;
   char lbuffer[RETROCON_LBUFFER_SZ_MAX + 1] = { 0 };

   *p_idc_out = RETROGUI_IDC_NONE;

   if( *p_c == RETROCON_ACTIVE_KEY ) {
      debug_printf( RETROCON_TRACE_LVL, "active key pressed" );
      if( RETROCON_FLAG_ACTIVE != (RETROCON_FLAG_ACTIVE & (con)->flags) ) {
         debug_printf( RETROCON_TRACE_LVL, "opening console..." );
         (con)->flags |= RETROCON_FLAG_ACTIVE;
         (con)->gui.flags |= RETROGUI_FLAGS_DIRTY;

         if( NULL != win_stack ) {
            retval = retrowin_push_win(
               &((con)->gui), win_stack,
               /* Use the IDC provided in p_idc_out for the window IDC. */
               *p_idc_out,
               NULL, (con)->gui.x, (con)->gui.y,
               (con)->gui.w, (con)->gui.h, RETROWIN_FLAG_BORDER_BLUE );
            maug_cleanup_if_not_ok();
         }

         debug_printf( RETROCON_TRACE_LVL, "console open!" );
      } else {
         debug_printf( RETROCON_TRACE_LVL, "closing console..." );
         con->flags &= ~RETROCON_FLAG_ACTIVE;

         if( NULL != win_stack ) {
            retrowin_destroy_win( win_stack, *p_idc_out );
            debug_printf( RETROCON_TRACE_LVL, "console closed!" );
         }

#  if defined( RETROTILE_PRESENT ) && !defined( RETROFLAT_NO_VIEWPORT_REFRESH )
         /* Mark all tiles as dirty when console is opened/closed. */
         retroflat_viewport_lock_refresh();
         retval = retrotile_clear_refresh( retroflat_viewport_screen_h() );
         retroflat_viewport_unlock_refresh();
#  endif /* RETROTILE_PRESENT && !RETROFLAT_NO_VIEWPORT_REFRESH */

         /* Return, in case the caller needs to do something with this. */
         *p_idc_out = RETROCON_IDC_CLOSE;
      }
      *p_c = 0;
      goto cleanup;
   } else if( RETROCON_FLAG_ACTIVE != (RETROCON_FLAG_ACTIVE & (con)->flags) ) {
      goto cleanup;
   }

   /* debug_printf( RETROCON_TRACE_LVL, "processing console input..." ); */

   retrogui_lock( &(con->gui) );

      *p_idc_out = retrogui_poll_ctls( &(con->gui), p_c, input_evt );

   retrogui_unlock( &(con->gui) );

   *p_c = 0; /* If we got this far then don't pass keystroke back. */

   if( RETROCON_IDC_TEXTBOX == *p_idc_out ) {
      retrogui_lock( &(con->gui) );
      retval = retrogui_get_ctl_text(
         &(con->gui), RETROCON_IDC_TEXTBOX, lbuffer, RETROCON_LBUFFER_SZ_MAX );
      retval = retrogui_set_ctl_text(
         &(con->gui), RETROCON_IDC_TEXTBOX, 1, "" );
      retrogui_unlock( &(con->gui) );
      maug_cleanup_if_not_ok();

      if( 0 == maug_strlen( lbuffer ) ) {
         /* Do nothing if line is empty. */
         goto cleanup;
      }

      /* Execute/reset line. */
      retval = retrocon_exec_line( con, lbuffer, maug_strlen( lbuffer ) );
   }

#if 0
   /* Debounce retrocon track only! */
   if( !retrocon_debounce( con, c ) ) {
      goto cleanup;
   }

   /* Process input. */
   switch( c ) {
   case RETROCON_ACTIVE_KEY:
      if( RETROCON_FLAG_ACTIVE == (RETROCON_FLAG_ACTIVE & con->flags) ) {
         con->flags &= ~RETROCON_FLAG_ACTIVE;
      } else {
         con->flags |= RETROCON_FLAG_ACTIVE;
      }
      break;

   case 0:
      break;

   case 0x08:
      /* Backspace. */
      if( 0 < con->lbuffer_sz ) {
         con->lbuffer_sz--;
         con->lbuffer[con->lbuffer_sz] = '\0';
      }
      break;

   case '\r':
   case '\n':
      if( 0 == con->lbuffer_sz ) {
         /* Do nothing if line is empty. */
         break;
      }

      /* Execute/reset line. */
      retval = retrocon_exec_line( con, con->lbuffer, con->lbuffer_sz );
      con->lbuffer_sz = 0;
      con->lbuffer[con->lbuffer_sz] = '\0';
      break;

   default:
      c = retroflat_vk_to_ascii(
         c, input_evt->key_flags | RETROFLAT_INPUT_FORCE_UPPER );
      if(
         /* Active and printable chars get added to line buffer. */
         RETROCON_FLAG_ACTIVE == (RETROCON_FLAG_ACTIVE & con->flags)  &&
         0 < c
      ) {
         con->lbuffer[con->lbuffer_sz++] = c;
         con->lbuffer[con->lbuffer_sz] = '\0';
      }
      break;
   }
#endif

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrocon_display(
   struct RETROCON* con, retroflat_blit_t* gui_bmp
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( RETROCON_FLAG_ACTIVE != (RETROCON_FLAG_ACTIVE & (con)->flags) ) {
      goto cleanup;
   }

   (con)->gui.draw_bmp = (gui_bmp);
   (con)->gui.flags |= RETROGUI_FLAGS_DIRTY;

   retrogui_lock( &((con)->gui) );
   retrogui_redraw_ctls( &((con)->gui) );
   retrogui_unlock( &((con)->gui) );

cleanup:

   return retval;
}

/* === */

void retrocon_shutdown( struct RETROCON* con ) {
#ifndef RETROGXC_PRESENT
   maug_mfree( con->gui.font_h );
#endif /* !RETROGXC_PRESENT */
   retrogui_free( &(con->gui) );
}

#endif /* RETROCON_C */

/*! \} */ /* maug_console */

#endif /* RETROCON_DISABLE */

#endif /* !RETROCON_H */

