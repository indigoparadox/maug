divert(-1)
define(`loopa', `$1_loop')
define(`dataa', `$1_DATA')
divert(0)
`#'define MAUG_C
`#'include <maug.h>

`#'define RETROFLT_C
`#'include <retroflt.h>

struct dataa(TEMPLATE) {
   int init;
};

void loopa(template)( struct dataa(TEMPLATE)* data ) {
   RETROFLAT_IN_KEY input = 0;
   struct RETROFLAT_INPUT input_evt;

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* Drawing */

   retroflat_draw_lock( NULL );

   retroflat_rect(
      NULL, RETROFLAT_COLOR_BLACK, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );

   retroflat_draw_release( NULL );
}

int main( int argc, char** argv ) {
   int retval = 0;
   struct RETROFLAT_ARGS args;
   MAUG_MHANDLE data_h = (MAUG_MHANDLE)NULL;
   struct dataa(TEMPLATE)* data = NULL;
   
   /* === Setup === */

   logging_init();

   memset( &args, '\0', sizeof( struct RETROFLAT_ARGS ) );

   args.title = "template";
   args.assets_path = "assets";
   
   retval = retroflat_init( argc, argv, &args );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

   data_h = maug_malloc( 1, sizeof( struct dataa(TEMPLATE) ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, data_h );
   maug_mlock( data_h, data );
   maug_cleanup_if_null_alloc( struct dataa(TEMPLATE)*, data );
   maug_mzero( data, sizeof( struct dataa(TEMPLATE) ) );

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)loopa(template), NULL, data );

cleanup:

#ifndef RETROFLAT_OS_WASM

   if( NULL != data ) {
      maug_munlock( data_h, data );
   }

   if( NULL != data_h ) {
      maug_mfree( data_h );
   }

   retroflat_shutdown( retval );

   logging_shutdown();

#endif /* !RETROFLAT_OS_WASM */

   return retval;
}
END_OF_MAIN()

