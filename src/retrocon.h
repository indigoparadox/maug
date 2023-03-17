
#ifndef RETROCON_H
#define RETROCON_H

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

#ifndef RETROCON_WIN_H
#  define RETROCON_WIN_H 110
#endif /* !RETROCON_WIN_H */

#define RETROCON_DEBOUNCE_WAIT 3

#define RETROCON_FLAG_ACTIVE 0x01

struct RETROCON;

typedef MERROR_RETVAL (*retrocon_cb)(
   struct RETROCON* con, const char* line, size_t line_sz, void* data );

struct RETROCON {
   uint8_t flags;
   int input_prev;
   int debounce_wait;
   char sbuffer[RETROCON_SBUFFER_SZ_MAX + 1];
   size_t sbuffer_sz;
   size_t sbuffer_lines[RETROCON_SBUFFER_LINES_MAX];
   size_t sbuffer_lines_sz;
   char lbuffer[RETROCON_LBUFFER_SZ_MAX + 1];
   size_t lbuffer_sz;
   void* callback_data[RETROCON_CB_SZ_MAX];
   char callback_names[RETROCON_CB_SZ_MAX][RETROCON_CB_NAME_SZ_MAX + 1];
   retrocon_cb callbacks[RETROCON_CB_SZ_MAX];
   size_t callbacks_sz;
   RETROFLAT_COLOR lbuffer_color;
   RETROFLAT_COLOR sbuffer_color;
   RETROFLAT_COLOR bg_color;
};

MERROR_RETVAL retrocon_init( struct RETROCON* con );

MERROR_RETVAL retrocon_add_command(
   struct RETROCON* con, const char* cmd, retrocon_cb cb, void* cb_data );

MERROR_RETVAL retrocon_display(
   struct RETROCON* con, struct RETROFLAT_BITMAP* display );

void retrocon_print_line( struct RETROCON* con, const char* line );

MERROR_RETVAL retrocon_exec_line(
   struct RETROCON* con, const char* line, size_t line_sz );

int retrocon_debounce( struct RETROCON* con, int c );

MERROR_RETVAL retrocon_input( struct RETROCON* con, int* p_c );

#ifdef RETROCON_C

static char retrocon_vk_to_ascii( int k ) {
   char c = 0;
   
   switch( k ) {
   case RETROFLAT_KEY_A: c = 0x41; break;
   case RETROFLAT_KEY_B: c = 0x42; break;
   case RETROFLAT_KEY_C: c = 0x43; break;
   case RETROFLAT_KEY_D: c = 0x44; break;
   case RETROFLAT_KEY_E: c = 0x45; break;
   case RETROFLAT_KEY_F: c = 0x46; break;
   case RETROFLAT_KEY_G: c = 0x47; break;
   case RETROFLAT_KEY_H: c = 0x48; break;
   case RETROFLAT_KEY_I: c = 0x49; break;
   case RETROFLAT_KEY_J: c = 0x4a; break;
   case RETROFLAT_KEY_K: c = 0x4b; break;
   case RETROFLAT_KEY_L: c = 0x4c; break;
   case RETROFLAT_KEY_M: c = 0x4d; break;
   case RETROFLAT_KEY_N: c = 0x4e; break;
   case RETROFLAT_KEY_O: c = 0x4f; break;
   case RETROFLAT_KEY_P: c = 0x50; break;
   case RETROFLAT_KEY_Q: c = 0x51; break;
   case RETROFLAT_KEY_R: c = 0x52; break;
   case RETROFLAT_KEY_S: c = 0x53; break;
   case RETROFLAT_KEY_T: c = 0x54; break;
   case RETROFLAT_KEY_U: c = 0x55; break;
   case RETROFLAT_KEY_V: c = 0x56; break;
   case RETROFLAT_KEY_W: c = 0x57; break;
   case RETROFLAT_KEY_X: c = 0x58; break;
   case RETROFLAT_KEY_Y: c = 0x59; break;
   case RETROFLAT_KEY_Z: c = 0x60; break;
   case RETROFLAT_KEY_0: c = 0x30; break;
   case RETROFLAT_KEY_1: c = 0x31; break;
   case RETROFLAT_KEY_2: c = 0x32; break;
   case RETROFLAT_KEY_3: c = 0x33; break;
   case RETROFLAT_KEY_4: c = 0x34; break;
   case RETROFLAT_KEY_5: c = 0x35; break;
   case RETROFLAT_KEY_6: c = 0x36; break;
   case RETROFLAT_KEY_7: c = 0x37; break;
   case RETROFLAT_KEY_8: c = 0x38; break;
   case RETROFLAT_KEY_9: c = 0x39; break;
   case RETROFLAT_KEY_SPACE: c = ' '; break;
   case RETROFLAT_KEY_BKSP: c = 0x08; break;
   case RETROFLAT_KEY_ENTER: c = '\n'; break;
   }

   return c;
}

static MERROR_RETVAL retrocon_cmd_print(
   struct RETROCON* con, const char* line, size_t line_sz, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;
   char* print_line = NULL;

   print_line = strchr( line, ' ' );
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

MERROR_RETVAL retrocon_init( struct RETROCON* con ) {
   MERROR_RETVAL retval = MERROR_OK;

   retval = retrocon_add_command( con, "PRINT", retrocon_cmd_print, NULL );
   retval = retrocon_add_command( con, "QUIT", retrocon_cmd_quit, NULL );

   return retval;
}

MERROR_RETVAL retrocon_add_command(
   struct RETROCON* con, const char* cmd, retrocon_cb cb, void* cb_data
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_cleanup_if_ge_overflow( con->callbacks_sz + 1, RETROCON_CB_SZ_MAX );

   strncpy(
      con->callback_names[con->callbacks_sz], cmd, RETROCON_CB_NAME_SZ_MAX );

   con->callbacks[con->callbacks_sz] = cb;

   con->callback_data[con->callbacks_sz] = cb_data;

   con->callbacks_sz++;

cleanup:

   return retval;
}

MERROR_RETVAL retrocon_display(
   struct RETROCON* con, struct RETROFLAT_BITMAP* display
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0,
      line_sz = 0;

   if( RETROCON_FLAG_ACTIVE != (RETROCON_FLAG_ACTIVE & con->flags) ) {
      goto cleanup;
   }

   if( NULL == con->bg_color ) {
      error_printf( "colors not set!" );
      goto cleanup;
   }

   retroflat_rect(
      NULL, con->bg_color, 10, 10,
      300, RETROCON_WIN_H, RETROFLAT_FLAGS_FILL );

   retroflat_string(
      NULL, con->lbuffer_color, con->lbuffer, -1, NULL,
      15, 15, 0 );

   /* Draw each line, one by one. */
   for( i = 0 ; con->sbuffer_lines_sz > i ; i++ ) {
      if( i + 1 < con->sbuffer_lines_sz ) {
         line_sz = con->sbuffer_lines[i + 1] - con->sbuffer_lines[i];
      } else {
         line_sz = -1;
      }
      
      retroflat_string(
         NULL, con->sbuffer_color,
         &(con->sbuffer[con->sbuffer_lines[i]]), line_sz, NULL,
         15, 25 + (i * 10), 0 );
   }

cleanup:

   return retval;
}

void retrocon_print_line( struct RETROCON* con, const char* line ) {
   size_t line_sz = 0;
   size_t i = 0;

   line_sz = strlen( line );
   if( line_sz + con->sbuffer_sz >= RETROCON_SBUFFER_SZ_MAX ) {
      /* TODO: Handle this better? */
      con->sbuffer_sz = 0;
      con->sbuffer[con->sbuffer_sz] = '\0';
      con->sbuffer_lines_sz = 0;
   }

   assert( line_sz < RETROCON_SBUFFER_SZ_MAX );

   /* Create line pointer for display function. */
   con->sbuffer_lines[con->sbuffer_lines_sz++] = con->sbuffer_sz;

   /* Copy line to buffer and terminate with newline. */
   for( i = 0 ; line_sz > i ; i++ ) {
      con->sbuffer[con->sbuffer_sz + i] = line[i];
   }

   debug_printf( 1, "println: %s (at " SIZE_T_FMT " chars)",
      &(con->sbuffer[con->sbuffer_sz]),
      con->sbuffer_lines[con->sbuffer_lines_sz - 1] );

   con->sbuffer_sz += line_sz;
}

MERROR_RETVAL retrocon_exec_line(
   struct RETROCON* con, const char* line, size_t line_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   /* Find callback with name starting line. */
   for( i = 0 ; con->callbacks_sz > i ; i++ ) {
      if(
         0 == strncmp(
            /* TODO: Compare up to first space in line. */
            con->callback_names[i], line, strlen( con->callback_names[i] ) )
      ) {
         retval = con->callbacks[i](
            con, line, line_sz, con->callback_data[i] );
         goto cleanup;
      }
   }

   retrocon_print_line( con, "command not found!" );

cleanup:

   return retval;
}

int retrocon_debounce( struct RETROCON* con, int c ) {
   if( 0 == c ) {
      return 0;
   }

   /* Debounce/disallow repeat even if it's allowed outside. */
   if( con->input_prev == c && 0 < con->debounce_wait ) {
      con->debounce_wait--;
      debug_printf( 0, "dbwait (%d)", con->debounce_wait );
      return 0;
   } else {
      con->input_prev = c;
      con->debounce_wait = RETROCON_DEBOUNCE_WAIT;
      debug_printf( 0, "new prev: %c", c );
   }

   return c;
}

MERROR_RETVAL retrocon_input( struct RETROCON* con, int* p_c ) {
   MERROR_RETVAL retval = MERROR_OK;
   int c = 0;

   /* Put keycode on retrocon track. Clear pass-track if console active. */
   c = *p_c;
   if( RETROCON_FLAG_ACTIVE == (RETROCON_FLAG_ACTIVE & con->flags) ) {
      *p_c = 0;
   }

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
      /* Execute/reset line. */
      retval = retrocon_exec_line( con, con->lbuffer, con->lbuffer_sz );
      con->lbuffer_sz = 0;
      con->lbuffer[con->lbuffer_sz] = '\0';
      break;

   default:
      c = retrocon_vk_to_ascii( c );
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

cleanup:

   return retval;
}

#endif /* RETROCON_C */

#endif /* !RETROCON_H */

