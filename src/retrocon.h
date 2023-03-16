
#ifndef RETROCON_H
#define RETROCON_H

#ifndef RETROCON_SBUFFER_SZ
#  define RETROCON_SBUFFER_SZ 4096
#endif /* !RETROCON_SBUFFER_SZ */

#ifndef RETROCON_LBUFFER_SZ
#  define RETROCON_LBUFFER_SZ 256
#endif /* !RETROCON_LBUFFER_SZ */

#ifndef RETROCON_ACTIVE_KEY
#  define RETROCON_ACTIVE_KEY '`'
#endif /* !RETROCON_ACTIVE_KEY */

#define RETROCON_DEBOUNCE_WAIT 3

#define RETROCON_FLAG_ACTIVE 0x01

struct RETROCON {
   uint8_t flags;
   int input_prev;
   int debounce_wait;
   char sbuffer[RETROCON_SBUFFER_SZ];
   char lbuffer[RETROCON_LBUFFER_SZ];
   size_t lbuffer_sz;
};

MERROR_RETVAL retrocon_display(
   struct RETROCON* con, struct RETROFLAT_BITMAP* display );

MERROR_RETVAL retrocon_input( struct RETROCON* con, int* p_c );

#ifdef RETROCON_C

MERROR_RETVAL retrocon_display(
   struct RETROCON* con, struct RETROFLAT_BITMAP* display
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( RETROCON_FLAG_ACTIVE != (RETROCON_FLAG_ACTIVE & con->flags) ) {
      goto cleanup;
   }

   retroflat_rect(
      NULL, RETROFLAT_COLOR_WHITE, 10, 10, 300, 110, RETROFLAT_FLAGS_FILL );

   retroflat_string(
      NULL, RETROFLAT_COLOR_BLACK, con->lbuffer, -1, NULL, 15, 20, 0 );

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

   case '\r':
   case '\n':
      con->lbuffer_sz = 0;
      con->lbuffer[con->lbuffer_sz] = '\0';
      break;

   default:
      if(
         RETROCON_FLAG_ACTIVE == (RETROCON_FLAG_ACTIVE & con->flags)  &&
         0x1f < c && 0x7e > c
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

