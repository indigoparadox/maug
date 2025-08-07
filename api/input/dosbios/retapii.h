
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
};

#elif defined( RETROFLT_C )

unsigned int g_retroflat_mouse_cursor[2][16] = {
	{
		0xfff0,
		0xffe0,
		0xffc0,
		0xff81,
		0xff03,
		0x0607,
		0x000f,
		0x001f,
		0xc03f,
		0xf07f,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff
	},
	{
		0x0000,
		0x0006,
		0x000c,
		0x0018,
		0x0030,
		0x0060,
		0x70c0,
		0x1d80,
		0x0700,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000
	}
};

/* === */

MERROR_RETVAL retroflat_init_input( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;
   union REGS r;
   struct SREGS s;

   /* Get mouse info. */
   r.x.ax = 0;
   int86( 0x33, &r, &r );
   debug_printf( 3, "mouse status: %d, %d", r.x.ax, r.x.bx );
   if( -1 != r.x.ax ) {
      retval = MERROR_GUI;
      error_printf( "mouse not found!" );
      goto cleanup;
   }

   /* Set mouse cursor. */
   r.x.ax = 0x9;
   r.x.bx = 0; /* Hotspot X */
   r.x.cx = 0; /* Hotspot Y */
   r.x.dx = (unsigned int)g_retroflat_mouse_cursor;
   segread( &s );
   s.es = s.ds;
   int86x( 0x33, &r, &r, &s );

cleanup:

   return retval;
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   union REGS inregs;
   union REGS outregs;
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   /* TODO: Poll the mouse. */

   /* TODO: Implement RETROFLAT_MOD_SHIFT. */

   if( kbhit() ) {
      /* Poll the keyboard. */
      key_out = getch();
      debug_printf( 2, "key: 0x%02x", key_out );
      if( 0 == key_out ) {
         /* Special key was pressed that returns two scan codes. */
         key_out = getch();
         switch( key_out ) {
         case 0x48: key_out = RETROFLAT_KEY_UP; break;
         case 0x4b: key_out = RETROFLAT_KEY_LEFT; break;
         case 0x4d: key_out = RETROFLAT_KEY_RIGHT; break;
         case 0x50: key_out = RETROFLAT_KEY_DOWN; break;
         case 0x4f: key_out = RETROFLAT_KEY_HOME; break;
         case 0x47: key_out = RETROFLAT_KEY_END; break;
         case 0x51: key_out = RETROFLAT_KEY_PGDN; break;
         case 0x49: key_out = RETROFLAT_KEY_PGUP; break;
         case 0x52: key_out = RETROFLAT_KEY_INSERT; break;
         case 0x53: key_out = RETROFLAT_KEY_DELETE; break;
         }
      }
      if(
         RETROFLAT_FLAGS_KEY_REPEAT !=
         (RETROFLAT_FLAGS_KEY_REPEAT & g_retroflat_state->retroflat_flags)
      ) {
         while( kbhit() ) {
            getch();
         }
      }
   }

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

