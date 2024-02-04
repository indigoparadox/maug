
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

int main( int argc, char* argv[] ) {
   FILE* bmp_f = NULL;
   int retval = 0;
   struct stat statbuf;
   uint8_t* bmp_bytes = NULL;
   uint32_t* bmp_field = NULL;
   size_t i = 0;
   char* ext_ptr = NULL;

   if( 2 > argc ) {
      retval = 1;
      goto cleanup;
   }

   stat( argv[1], &statbuf );

   bmp_f = fopen( argv[1], "rb" );
   if( NULL == bmp_f ) {
      retval = 2;
      goto cleanup;
   }
   
   bmp_bytes = malloc( statbuf.st_size );
   if( NULL == bmp_bytes ) {
      retval = 4;
      goto cleanup;
   }

   fread( bmp_bytes, 1, statbuf.st_size, bmp_f );

   bmp_field = (uint32_t*)&(bmp_bytes[14]);
   assert( 40 == *bmp_field );

   bmp_field = (uint32_t*)&(bmp_bytes[10]);
   /* printf( "ds: 0x%08x\n", *bmp_field ); */

   /* Chop off file extension. */
   ext_ptr = strrchr( argv[1], '.' );
   *ext_ptr = '\0';

   printf( "uint8_t gc_%s[] = {", argv[1] );
   for( i = *bmp_field ; statbuf.st_size > i ; i++ ) {
      if( 0 == (i - *bmp_field) % 10 ) {
         printf( "\n   /* %03lu */", i - *bmp_field );
      }
      printf( " 0x%02x,", bmp_bytes[i] );
   }
   printf( "\n};\n\n" );

cleanup:

   if( NULL != bmp_bytes ) {
      free( bmp_bytes );
   }

   if( NULL != bmp_f ) {
      fclose( bmp_f );
   }

   return retval;
}

