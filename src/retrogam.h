
#ifndef RETROGAM_H
#define RETROGAM_H

#include <mtypes.h>
#include <uprintf.h>

#define retrogam_idx( x, y, w ) (((y) * (w)) + (x))

#define retrogam_generate_terrain( map, min_z, max_z, map_w, map_h ) \
   memset( map, -1, map_w * map_h ); \
   retrogam_generate_terrain_iter( \
      map, min_z, max_z, map_w, map_h, 0, 0, map_w, map_h );

void retrogam_generate_terrain_iter(
   int8_t* map, int8_t min_z, int8_t max_z, int16_t map_w, int16_t map_h,
   int16_t sect_x, int16_t sect_y, int16_t sect_w, int16_t sect_h );

#ifdef RETROGAM_C

void retrogam_generate_terrain_iter(
   int8_t* map, int8_t min_z, int8_t max_z, int16_t map_w, int16_t map_h,
   int16_t sect_x, int16_t sect_y, int16_t sect_w, int16_t sect_h
) {
   int16_t iter_x = 0,
      iter_y = 0,
      iter_depth = 0;
   int32_t avg = 0;

   /* Trivial case; end recursion. */
   if( 0 == sect_w ) {
      return;
   }

   iter_depth = map_w / sect_w;

   /* Generate missing corner data. */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {
         /* Multiply sect_w by iter_x to get the corners
          * (0, 0) (0, w), (w, 0), (w, w), then subtract iter_w so that
          * left corners (0, *) stay in bounds as 0, but right corners
          * get 1 shaved off to stay in bounds as (w - 1).
          */
         if(
            -1 ==
            map[retrogam_idx(
               (sect_x + (iter_x * sect_w)) - iter_x,
               (sect_y + (iter_y * sect_h)) - iter_y, map_w )]
         ) {
            avg = min_z + (rand() % (max_z - min_z));
            debug_printf( 0, "coord %d x %d: rand: %d",
               (sect_x + (iter_x * sect_w)) - iter_x,
               (sect_y + (iter_y * sect_h)) - iter_y,
               avg );
            assert( min_z <= avg );
            map[retrogam_idx(
               (sect_x + (iter_x * sect_w)) - iter_x,
               (sect_y + (iter_y * sect_h)) - iter_y, map_w )] = avg;
            assert( min_z <= map[retrogam_idx(
               (sect_x + (iter_x * sect_w)) - iter_x,
               (sect_y + (iter_y * sect_h)) - iter_y, map_w )] );
            assert( 127 >= map[retrogam_idx(
               (sect_x + (iter_x * sect_w)) - iter_x,
               (sect_y + (iter_y * sect_h)) - iter_y, map_w )] );
         }
      }
   }

   if( 2 == sect_w && 2 == sect_h ) {
      /* Nothing to average, this sector is just corners! */
      return;
   }
   
   /* Average corner data. */
   avg = map[retrogam_idx( sect_x, sect_y, map_w )] +
      map[retrogam_idx( sect_x, sect_y + (sect_h - 1), map_w )] +
      map[retrogam_idx( sect_x + (sect_w - 1), sect_y, map_w )] +
      map[retrogam_idx( sect_x + (sect_w - 1), sect_y + (sect_h - 1), map_w )];
   avg /= 4;
   debug_printf( 0, "%d: coords: %d x %d: avg of %d, %d, %d, %d: %d",
      iter_depth,
      sect_x + (sect_w / 2), sect_y + (sect_h / 2),
      map[retrogam_idx( sect_x, sect_y, map_w )],
      map[retrogam_idx( sect_x, sect_y + (sect_h - 1), map_w )],
      map[retrogam_idx( sect_x + (sect_w - 1), sect_y, map_w )],
      map[retrogam_idx( sect_x + (sect_w - 1), sect_y + (sect_h - 1), map_w )],
      avg );

   map[retrogam_idx( sect_x + (sect_w / 2), sect_y + (sect_h / 2), map_w )] =
      avg;

   assert( 0 <=
      map[retrogam_idx( sect_x + (sect_w / 2), sect_y + (sect_h) / 2, map_w )]
   );

   /* Recurse into subsectors. */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {
         assert( 0 == sect_w % 2 || 0 == sect_w );
         assert( 0 == sect_h % 2 || 0 == sect_h );
         debug_printf( 0, "%d: child sector at %d, %d, %d wide",
            iter_depth,
            sect_x + (iter_x * (sect_w / 2)),
            sect_y + (iter_y * (sect_h / 2)), sect_w / 2 );
         retrogam_generate_terrain_iter(
            map, min_z, max_z, map_w, map_h,
            sect_x + (iter_x * (sect_w / 2)),
            sect_y + (iter_y * (sect_h / 2)),
            sect_w / 2, sect_h / 2 );
      }
   }

   debug_printf( 0, "%d return", iter_depth );
}

#endif /* RETROGAM_C */

#endif /* !RETROGAM_H */

