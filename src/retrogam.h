
#ifndef RETROGAM_H
#define RETROGAM_H

#include <mtypes.h>
#include <uprintf.h>

/**
 * \addtogroup maug_retrogam RetroGam API
 * \{
 *
 * \file retrogam.h
 */

#define retrogam_idx( x, y, w ) (((y) * (w)) + (x))

/**
 * \addtogroup maug_retrogam_terrain RetroGam Terrain Generation
 * \{
 */

/**
 * \brief Parameter data structure for retrogam_generate_diamond_square_iter().
 */
struct RETROGAM_GENDATA_DSQUARE {
   int16_t sect_x;
   int16_t sect_y;
   int16_t sect_w;
   int16_t sect_h;
};

typedef void (*retrogam_generator)(
   int8_t* map, int8_t min_z, int8_t max_z, int16_t map_w, int16_t map_h,
   void* data );

#define retrogam_generate_diamond_square( map, min_z, max_z, map_w, map_h ) \
   memset( map, -1, map_w * map_h ); \
   retrogam_generate_diamond_square_iter( \
      map, min_z, max_z, map_w, map_h, NULL );

/**
 * \brief Diamond-square terrain generator.
 *
 * This generator creates random numbers in the corners of progressively
 * smaller map sectors while averaging between them for continuity.
 */
void retrogam_generate_diamond_square_iter(
   int8_t* map, int8_t min_z, int8_t max_z, int16_t map_w, int16_t map_h,
   void* data );

/*! \} */ /* maug_retrogam_terrain */

#ifdef RETROGAM_C

void retrogam_generate_diamond_square_iter(
   int8_t* map, int8_t min_z, int8_t max_z, int16_t map_w, int16_t map_h,
   void* data
) {
   int16_t iter_x = 0,
      iter_y = 0,
      iter_depth = 0;
   int16_t corners_x[2][2];
   int16_t corners_y[2][2];
   int32_t avg = 0;
   struct RETROGAM_GENDATA_DSQUARE data_ds_sub;
   struct RETROGAM_GENDATA_DSQUARE* data_ds = NULL;

   if( NULL == data ) {
      data_ds = calloc( sizeof( struct RETROGAM_GENDATA_DSQUARE ), 1 );
      data_ds->sect_w = map_w;
      data_ds->sect_h = map_h;
   } else {
      data_ds = (struct RETROGAM_GENDATA_DSQUARE*)data;
   }
   assert( NULL != data_ds );

   /* Trivial case; end recursion. */
   if( 0 == data_ds->sect_w ) {
      debug_printf( 0, "%d return: null sector", iter_depth );
      goto cleanup;
   }

   if(
      data_ds->sect_x + data_ds->sect_w > map_w ||
      data_ds->sect_y + data_ds->sect_h > map_h
   ) {
      debug_printf( 0, "%d return: overflow sector", iter_depth );
      goto cleanup;
   }

   iter_depth = map_w / data_ds->sect_w;

   /* Generate missing corner data. */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {
         /* Make sure corner X is in bounds. */
         corners_x[iter_x][iter_y] =
            (data_ds->sect_x - 1) + (iter_x * data_ds->sect_w);
         if( 0 > corners_x[iter_x][iter_y] ) {
            corners_x[iter_x][iter_y] += 1;
         }

         /* Make sure corner Y is in bounds. */
         corners_y[iter_x][iter_y] =
            (data_ds->sect_y - 1) + (iter_y * data_ds->sect_h);
         if( 0 > corners_y[iter_x][iter_y] ) {
            corners_y[iter_x][iter_y] += 1;
         }

         if(
            -1 != map[retrogam_idx(
               corners_x[iter_x][iter_y], corners_y[iter_x][iter_y], map_w )]
         ) {
            debug_printf( 0, "corner coord %d x %d present: %d",
               corners_x[iter_x][iter_y], corners_y[iter_x][iter_y],
               map[retrogam_idx( 
                  corners_x[iter_x][iter_y],
                  corners_y[iter_x][iter_y], map_w )]
            );
            continue;
         }

         /* Fill in missing corner. */
         avg = min_z + (rand() % (max_z - min_z));
         debug_printf( 0, "missing corner coord %d x %d: rand: %d",
            corners_x[iter_x][iter_y], corners_y[iter_x][iter_y], avg );
         
         assert( min_z <= avg );

         map[retrogam_idx( 
            corners_x[iter_x][iter_y], corners_y[iter_x][iter_y], map_w )]
               = avg;
      }
   }

   if( 2 == data_ds->sect_w && 2 == data_ds->sect_h ) {
      /* Nothing to average, this sector is just corners! */
      debug_printf( 0, "%d return: reached innermost point", iter_depth );
      goto cleanup;
   }
   
   /* Average corner data. */
   for( iter_y = 0 ; 2 > iter_y ; iter_y++ ) {
      for( iter_x = 0 ; 2 > iter_x ; iter_x++ ) {
         debug_printf( 0, "%d: adding from coords %d x %d: %d",
            iter_depth,
            corners_x[iter_x][iter_y], corners_y[iter_x][iter_y],
            map[retrogam_idx( 
               corners_x[iter_x][iter_y], corners_y[iter_x][iter_y], map_w )]
         );
         avg += map[retrogam_idx( 
            corners_x[iter_x][iter_y], corners_y[iter_x][iter_y], map_w )];
      }
   }

   avg /= 4;
   debug_printf( 0, "%d: avg: %d", iter_depth, avg );

   assert( -1 == map[retrogam_idx( data_ds->sect_x + (data_ds->sect_w / 2), data_ds->sect_y + (data_ds->sect_h / 2), map_w )] );
   map[retrogam_idx( data_ds->sect_x + (data_ds->sect_w / 2), data_ds->sect_y + (data_ds->sect_h / 2), map_w )] =
      avg;

   assert( 0 <=
      map[retrogam_idx( data_ds->sect_x + (data_ds->sect_w / 2), data_ds->sect_y + (data_ds->sect_h) / 2, map_w )]
   );

   /* Recurse into subsectors. */
   for( iter_y = data_ds->sect_y ; iter_y < (data_ds->sect_y + data_ds->sect_h) ; iter_y++ ) {
      for( iter_x = data_ds->sect_x ; iter_x < (data_ds->sect_x + data_ds->sect_w) ; iter_x++ ) {
         data_ds_sub.sect_x = data_ds->sect_x + iter_x;

         data_ds_sub.sect_y = data_ds->sect_y + iter_y;

         data_ds_sub.sect_w = data_ds->sect_w / 2;
         data_ds_sub.sect_h = data_ds->sect_h / 2;

         debug_printf( 0, "%d: child sector at %d, %d, %d wide",
            iter_depth,
            data_ds_sub.sect_x, data_ds_sub.sect_y, data_ds_sub.sect_w );

         retrogam_generate_diamond_square_iter(
            map, min_z, max_z, map_w, map_h, &data_ds_sub );
      }
   }

   debug_printf( 0, "%d return: all sectors complete", iter_depth );

cleanup:

   if( NULL == data && NULL != data_ds ) {
      /* We must've alloced this internally. */
      free( data_ds );
   }

   return;
}

#endif /* RETROGAM_C */

/*! \} */ /* maug_retrogam */

#endif /* !RETROGAM_H */

