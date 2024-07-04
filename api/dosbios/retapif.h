
#ifndef RETPLTF_H
#define RETPLTF_H

static volatile retroflat_ms_t g_ms = 0;

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   /* TODO: Display error somehow. */
   error_printf( "%s", msg_out );

   va_end( vargs );
}

/* === */

void __interrupt __far retroflat_timer_handler() {
   static unsigned long count = 0;

   ++g_ms;
   count += RETROFLAT_DOS_TIMER_DIV; /* Original DOS timer in parallel. */
   if( 65536 <= count ) {
      /* Call the original handler. */
      count -= 65536;
      _chain_intr( g_retroflat_state->old_timer_interrupt );
   } else {
      /* Acknowledge interrupt */
      outp( 0x20, 0x20 );
   }
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Do nothing. */

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   return /**((uint16_t far*)0x046c) >> 4;*/ g_ms;
}

/* === */

uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   return retval;
}

#endif /* !RETPLTF_H */

