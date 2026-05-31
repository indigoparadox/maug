
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAUG_NO_RETRO
#define MAUG_C
#include "maug.h"
#include "mvmd.h"
#include "mvmc.h"

/* === */

int main( int argc, char* argv[] ) {
   int retval = 0;
   FILE* src_f = NULL;
   size_t src_sz = 0;
   uint8_t* src_buf = NULL;
   struct MVMC_PARSER parser;
   size_t i = 0;
   size_t src_read = 0;

   memset( &parser, '\0', sizeof( struct MVMC_PARSER ) );

   /* Load the source file into memory. */
   src_f = fopen( argv[1], "r" );
   if( NULL == src_f ) {
      retval = 1;
      goto cleanup;
   }
   fseek( src_f, 0, SEEK_END );
   src_sz = ftell( src_f );
   fseek( src_f, 0, SEEK_SET );
   src_buf = calloc( 1, src_sz );
   if( NULL == src_buf ) {
      retval = 1;
      goto cleanup;
   }
   src_read = fread( src_buf, 1, src_sz, src_f );
   if( src_read < src_sz ) {
      fprintf( stderr, "insufficient read bytes: %ld\n", src_read );
      retval = 1;
      goto cleanup;
   }

   /* Parse the source file. */
   for( i = 0 ; src_sz > i ; i++ ) {
      retval = mvmc_parse( &parser, src_buf[i] );
      if( retval ) {
         goto cleanup;
      }
   }

cleanup:

   return retval;
}

