
#ifndef RETROGAM_H
#define RETROGAM_H

#include <mtypes.h>
#include <uprintf.h>

#define retrogam_idx( x, y, w ) (((y) * (w)) + (x))

#define retrogam_generate_terrain( map, max_z, map_w, map_h ) \
   retrogam_generate_terrain_iter( \
      map, max_z, map_w, map_h, 0, 0, map_w - 1, map_h - 1 );

void retrogam_generate_terrain_iter(
   int8_t* map, int16_t max_z, int16_t map_w, int16_t map_h,
   int16_t sect_x, int16_t sect_y, int16_t sect_w, int16_t sect_h );

#ifdef RETROGAM_C

void retrogam_generate_terrain_iter(
   int8_t* map, int16_t max_z, int16_t map_w, int16_t map_h,
   int16_t sect_x, int16_t sect_y, int16_t sect_w, int16_t sect_h
) {
   int16_t iter_x = 0,
      iter_y = 0,
      iter_depth = 0;

   /* Trivial case; end recursion. */
   if( 0 == sect_w ) {
      return;
   }

   iter_depth = map_w / sect_w;

   /* Generate missing corner data. */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {
         if(
            -1 ==
            map[retrogam_idx(
               sect_x + (iter_x * sect_w),
               sect_y + (iter_y * sect_h), map_w )]
         ) {
            map[retrogam_idx(
               sect_x + (iter_x * sect_w),
               sect_y + (iter_y * sect_h), map_w )] = rand() % max_z;
         }
      }
   }
   
   /* Average corner data. */
   debug_printf( 1, "%d: avg of %d, %d, %d, %d",
      iter_depth,
      map[retrogam_idx( sect_x, sect_y, map_w )],
      map[retrogam_idx( sect_x, sect_y + (sect_h), map_w )],
      map[retrogam_idx( sect_x + (sect_w), sect_y, map_w )],
      map[retrogam_idx( sect_x + (sect_w), sect_y + (sect_h), map_w )] );

   map[retrogam_idx( sect_x + (sect_w / 2), sect_y + (sect_h) / 2, map_w )] =
      (map[retrogam_idx( sect_x, sect_y, map_w )] +
      map[retrogam_idx( sect_x, sect_y + (sect_h), map_w )] +
      map[retrogam_idx( sect_x + (sect_w), sect_y, map_w )] +
      map[retrogam_idx( sect_x + (sect_w), sect_y + (sect_h), map_w )])
      / 4;

   /* Recurse into subsectors. */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {
         if( 0 == sect_w / 2 ) {
            debug_printf( 1, "%d: return", iter_depth );
            return;
         }
         debug_printf( 1, "%d: child sector at %d, %d, %d wide\n",
            iter_depth,
            sect_x + (iter_x * (sect_w / 2)),
            sect_y + (iter_y * (sect_h / 2)), sect_w / 2 );
         retrogam_generate_terrain_iter(
            map, max_z, map_w, map_h,
            sect_x + (iter_x * (sect_w / 2)),
            sect_y + (iter_y * (sect_h / 2)),
            sect_w / 2, sect_h / 2 );
      }
   }

   debug_printf( 1, "%d return", iter_depth );
}

#endif /* RETROGAM_C */

#endif /* !RETROGAM_H */

