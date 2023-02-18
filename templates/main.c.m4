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
   int input = 0;
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
   struct dataa(TEMPLATE) data;

   /* === Setup === */

   args.screen_w = 320;
   args.screen_h = 200;
   args.title = "template";
   args.assets_path = "";
   
   retval = retroflat_init( argc, argv, &args );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

   data.init = 0;

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)loopa(template), &data );

cleanup:

   retroflat_shutdown( retval );

   return retval;
}
END_OF_MAIN()

