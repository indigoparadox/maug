
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
      NULL, RETROFLAT_COLOR_BLACK, "Hello!", -1, NULL, 15, 20, 0 );

cleanup:

   return retval;
}

int retrocon_debounce( struct RETROCON* con, int c ) {
   /* Debounce/disallow repeat even if it's allowed outside. */
   if( con->input_prev == c || 0 == c ) {
      if( 0 < con->debounce_wait ) {
         con->debounce_wait--;
         return 0;
      } else if( con->input_prev == c ) {
         return 0;
      } else {
         con->input_prev = c;
      }
   } else {
      con->input_prev = c;
      con->debounce_wait = RETROCON_DEBOUNCE_WAIT;
   }

   return c;
}

MERROR_RETVAL retrocon_input( struct RETROCON* con, int* p_c ) {
   MERROR_RETVAL retval = MERROR_OK;

   if(
      RETROCON_ACTIVE_KEY != *p_c &&
      RETROCON_FLAG_ACTIVE != (RETROCON_FLAG_ACTIVE & con->flags)
   ) {
      /* None of our business! */
      goto cleanup;

   } else if( !retrocon_debounce( con, *p_c ) ) {
      goto cleanup;
   }

   debug_printf( 1, "conp: %c", (char)*p_c );

   /* Process input. */
   switch( *p_c ) {
   case RETROCON_ACTIVE_KEY:
      if( RETROCON_FLAG_ACTIVE == (RETROCON_FLAG_ACTIVE & con->flags) ) {
         con->flags &= ~RETROCON_FLAG_ACTIVE;
      } else {
         con->flags |= RETROCON_FLAG_ACTIVE;
      }
      *p_c = 0;
      break;

   case 0:
      break;

   default:
      if( RETROCON_FLAG_ACTIVE == (RETROCON_FLAG_ACTIVE & con->flags) ) {
         *p_c = 0;
         debug_printf( 1, "no! %d", *p_c );
      }
      break;
   }

   con->input_prev = 0;

cleanup:

   return retval;
}

#endif /* RETROCON_C */

#endif /* !RETROCON_H */

