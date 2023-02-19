
#include <math.h>
#include <stdio.h>

#define PI (3.14159f)

int main() {
   FILE* h_out = NULL;
   float i = 0;

   h_out = fopen( "lut.h", "w" );

   fprintf( h_out, "MAUG_CONST int16_t g_retroflat_soft_cos[] = {\n" );
   for( i = 0 ; 2 * PI > i ; i += 0.1 ) {
      fprintf( h_out, "   %d,\n", (int)(cos( i ) * 1000) );
   }
   fprintf( h_out, "};\n\n" );

   fprintf( h_out, "MAUG_CONST int16_t g_retroflat_soft_sin[] = {\n" );
   for( i = 0 ; 2 * PI > i ; i += 0.1 ) {
      printf( "sin( %f ) = %f\n", i, sin( i ) );
      fprintf( h_out, "   %d,\n", (int)(sin( i ) * 1000) );
   }
   fprintf( h_out, "};\n\n" );

   fprintf( h_out, "MAUG_CONST int16_t g_retroflat_soft_tan[] = {\n" );
   for( i = 0 ; 2 * PI > i ; i += 0.1 ) {
      fprintf( h_out, "   %d,\n", (int)(tan( i ) * 1000) );
   }
   fprintf( h_out, "};\n\n" );

   fclose( h_out );

   return 0;
}

