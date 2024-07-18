
#ifndef RETROPTH_H
#define RETROPTH_H

/**
 * \addtogroup dsekai_pathfind Pathfinding
 * \{
 */

/**
 * \file pathfind.h
 */

#ifndef RETROTILE_PATH_TRACE_LVL
#  define RETROTILE_PATH_TRACE_LVL 0
#endif /* !RETROTILE_PATH_TRACE_LVL */

#ifndef RETROTILE_PATH_LIST_MAX
#  define RETROTILE_PATH_LIST_MAX 20
#endif /* !RETROTILE_PATH_LIST_MAX */

struct RETROTILE_PATH_NODE {
   uint16_t x;
   uint16_t y;
   /*! \brief Total node cost. */
   uint8_t f;
   /*! \brief Distance from node to pathfinding start. */
   uint8_t g; 
   /*! \brief Estimated distance from node to pathfinding target. */
   uint8_t h;
   /*! \brief Direction of this node from its parent. */
   int8_t dir;
};

typedef int8_t RETROTILE_RETVAL;

typedef RETROTILE_RETVAL (*retrotile_blocked_cb)(
   uint16_t x, uint16_t y, uint8_t dir8, struct RETROTILE* t, void* data );

#define RETROTILE_RETVAL_BLOCKED MERROR_USR

/**
 * \brief Flag for retrotile_path_start() indicating the target tile is occupied
 *        (e.g. we're following another mobile), so don't try to land on it.
 */
#define retrotile_path_cmp_eq( a, b ) \
   (((a)->x == (b)->x) && (((a)->y == (b)->y)))

MERROR_RETVAL retrotile_path_start(
   uint16_t start_x, uint16_t start_y,
   uint16_t tgt_x, uint16_t tgt_y,
   struct RETROTILE_PATH_NODE* closed,
   size_t* p_closed_sz, size_t closed_sz_max,
   struct RETROTILE* t, uint8_t flags,
   retrotile_blocked_cb blocked_cb, void* blocked_cb_data );

/*! \} */ /* dsekai_pathfind */

#ifdef RETROTIL_C

static MERROR_RETVAL retrotile_path_list_append(
   struct RETROTILE_PATH_NODE* list, size_t* p_list_sz, size_t list_sz_max,
   struct RETROTILE_PATH_NODE* node
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( *p_list_sz + 1 >= RETROTILE_PATH_LIST_MAX ) {
      error_printf( "list overflow!" );
      retval = MERROR_OVERFLOW;
   } else {
      memcpy(
         &(list[*p_list_sz]), node,
         sizeof( struct RETROTILE_PATH_NODE ) );
      (*p_list_sz)++;
   }

   return retval;
}

static void retrotile_path_list_remove(
   struct RETROTILE_PATH_NODE* list, size_t* p_list_sz, size_t idx
) {
   assert( idx < *p_list_sz );
   while( idx + 1 < *p_list_sz ) {
      debug_printf( RETROTILE_PATH_TRACE_LVL,
         "replacing index " SIZE_T_FMT " with " SIZE_T_FMT "...",
         idx, idx + 1 );
      memcpy(
         &(list[idx]), &(list[idx + 1]),
         sizeof( struct RETROTILE_PATH_NODE ) );
      idx++;
   }
   (*p_list_sz)--;
}

/**
 * \return Index of the list node with the lowest total node cost.
 */
static
int8_t retrotile_path_list_lowest_f(
   struct RETROTILE_PATH_NODE* list, size_t list_sz
) {
   size_t i = 0;
   uint8_t lowest_f_idx = 0;

   for( i = 0 ; list_sz > i ; i++ ) {
      if( list[lowest_f_idx].f >= list[i].f ) {
         lowest_f_idx = i;
      }
   }

   debug_printf( RETROTILE_PATH_TRACE_LVL,
      "lowest cost open tile is %d, %d, cost is %d",
      list[lowest_f_idx].x, list[lowest_f_idx].y,
      list[lowest_f_idx].f );

   return lowest_f_idx;
}

static MERROR_RETVAL retrotile_path_list_test_add_child( 
   struct RETROTILE_PATH_NODE* p_adjacent, uint8_t dir,
   uint8_t iter_closed_idx,
   uint16_t tgt_x, uint16_t tgt_y,
   struct RETROTILE_PATH_NODE* open, size_t* p_open_sz,
   struct RETROTILE_PATH_NODE* closed, size_t closed_sz, uint8_t flags,
   struct RETROTILE* t,
   retrotile_blocked_cb blocked_cb, void* blocked_cb_data
) {
   uint8_t i = 0;
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROTILE_PATH_TRACE_LVL,
      "> testing adjacent tile at %d, %d...",
      p_adjacent->x, p_adjacent->y );
   
   /* Make sure adjacent is not in the closed list. */
   for( i = 0 ; closed_sz > i ; i++ ) {
      if( retrotile_path_cmp_eq( p_adjacent, &(closed[i]) ) ) {
         debug_printf( RETROTILE_PATH_TRACE_LVL,
            ">> tile is already closed!" );
         goto cleanup;
      }
   }

   /* Don't use occupied tiles. */
   if(
      /* Otherwise check for occupied tile. */
      (NULL != blocked_cb &&
      RETROTILE_RETVAL_BLOCKED == blocked_cb(
         closed[iter_closed_idx].x,
         closed[iter_closed_idx].y, dir, t, blocked_cb_data ))
   ) {
      debug_printf( RETROTILE_PATH_TRACE_LVL,
         ">> tile %d, %d blocked by mobile or terrain!",
         closed[iter_closed_idx].x + gc_retroflat_offsets4_x[dir],
         closed[iter_closed_idx].y + gc_retroflat_offsets4_y[dir] );
      retval = RETROTILE_RETVAL_BLOCKED;
      goto cleanup;
   }

   /* Calculate adjacent pathfinding properties. */

   debug_printf( RETROTILE_PATH_TRACE_LVL,
      ">> tile parent %d, %d, distance %d",
      closed[iter_closed_idx].x,
      closed[iter_closed_idx].y,
      closed[iter_closed_idx].g );

   /* Use Manhattan heuristic since we can only move in 4 dirs. */
   p_adjacent->g = closed[iter_closed_idx].g + 1;
   p_adjacent->h =
      abs( (int16_t)((int16_t)p_adjacent->x - (int16_t)tgt_x) ) +
      abs( (int16_t)((int16_t)p_adjacent->y - (int16_t)tgt_y) );
   p_adjacent->f = p_adjacent->g + p_adjacent->h;
   debug_printf( RETROTILE_PATH_TRACE_LVL,
      ">> tile dist: %d, heur: %d, cost: %d",
         p_adjacent->g, p_adjacent->h, p_adjacent->f );

   for( i = 0 ; *p_open_sz > i ; i++ ) {
      /* Make sure adjacent is not already on the open list. */
      if( retrotile_path_cmp_eq( p_adjacent, &(open[i]) ) ) {
         goto cleanup;
      }
   }

   /* Add the child to the list. */
   debug_printf( RETROTILE_PATH_TRACE_LVL,
      ">> adding tile to open list" );
   retval = retrotile_path_list_append(
      open, p_open_sz, RETROTILE_PATH_LIST_MAX, p_adjacent );
   maug_cleanup_if_not_ok();

cleanup:

   return retval;
}

MERROR_RETVAL retrotile_path_start(
   uint16_t start_x, uint16_t start_y,
   uint16_t tgt_x, uint16_t tgt_y,
   struct RETROTILE_PATH_NODE* path,
   size_t* p_path_sz, size_t path_sz_max,
   struct RETROTILE* t, uint8_t flags,
   retrotile_blocked_cb blocked_cb, void* blocked_cb_data
) {
   struct RETROTILE_PATH_NODE open[RETROTILE_PATH_LIST_MAX];
   struct RETROTILE_PATH_NODE closed[RETROTILE_PATH_LIST_MAX];
   struct RETROTILE_PATH_NODE adjacent;
   size_t open_sz = 0,
      closed_sz = 0;
   uint8_t iter_idx = 0,
      tgt_reached = 0,
      i = 0;
   MERROR_RETVAL retval = RETROTILE_RETVAL_BLOCKED;

   /* Zero out lists and nodes. */
   maug_mzero(
      open,
      sizeof( struct RETROTILE_PATH_NODE ) * RETROTILE_PATH_LIST_MAX );
   maug_mzero(
      closed, 
      sizeof( struct RETROTILE_PATH_NODE ) * RETROTILE_PATH_LIST_MAX );
   maug_mzero( &adjacent, sizeof( struct RETROTILE_PATH_NODE ) );
   
   /* Add the start node to the open list. */
   adjacent.x = start_x;
   adjacent.y = start_y;
   retval = retrotile_path_list_append(
      open, &open_sz, RETROTILE_PATH_LIST_MAX, &adjacent );
   maug_cleanup_if_not_ok();
   
   debug_printf( RETROTILE_PATH_TRACE_LVL, "---BEGIN PATHFIND---" );
   debug_printf( RETROTILE_PATH_TRACE_LVL,
      "pathfinding to %d, %d...", tgt_x, tgt_y );

   while( 0 < open_sz ) {
      iter_idx = retrotile_path_list_lowest_f( open, open_sz );

      /* Move the iter node to the closed list. */
      debug_printf( RETROTILE_PATH_TRACE_LVL,
         "moving %d, %d to closed[" SIZE_T_FMT
         "] and evaluating...",
         open[iter_idx].x, open[iter_idx].y, closed_sz );
      memcpy(
         &(closed[closed_sz]), &(open[iter_idx]), 
         sizeof( struct RETROTILE_PATH_NODE ) );
      /* This should be identical but it's not: */
      /* retval = retrotile_path_list_append(
         closed, p_closed_sz, &(open[iter_idx]) );
      maug_cleanup_if_not_ok(); */
      if(
         /* Don't add block tiles to list. */
         (NULL != blocked_cb &&
         RETROTILE_RETVAL_BLOCKED != blocked_cb(
            open[iter_idx].x,
            open[iter_idx].y, open[iter_idx].dir, t, blocked_cb_data ))
      ) {
         retval = retrotile_path_list_append(
            path, p_path_sz, path_sz_max, &(open[iter_idx]) );
         maug_cleanup_if_not_ok();
      }
      retrotile_path_list_remove( open, &open_sz, iter_idx );
      iter_idx = closed_sz;
      (closed_sz)++;
      debug_printf( RETROTILE_PATH_TRACE_LVL,
         ">> open list now has " SIZE_T_FMT " tiles", open_sz );
      
      /* Validate closed list size. */
      if( closed_sz >= RETROTILE_PATH_LIST_MAX ) {
         debug_printf( RETROTILE_PATH_TRACE_LVL, 
            "> pathfind stack exceeded" );
         retval = MERROR_OVERFLOW; 
         goto cleanup;
      }

      /* Check to see if we've reached the target. */
      if(
         closed[iter_idx].x == tgt_x &&
         closed[iter_idx].y == tgt_y
      ) {
         debug_printf( RETROTILE_PATH_TRACE_LVL, "> target reached!" );
         tgt_reached = 1;
         break;
      }
      
      /* Test and add each of the 4 adjacent tiles. */
      for( i = 0 ; 4 > i ; i++ ) {
         /* Don't wander off the map! */
         if(
            (0 == closed[iter_idx].x &&
               gc_retroflat_offsets4_x[i] == -1) ||
            (0 == closed[iter_idx].y &&
               gc_retroflat_offsets4_y[i] == -1) ||
            (t->tiles_w - 1 == closed[iter_idx].x &&
               gc_retroflat_offsets4_x[i] == 1) ||
            (t->tiles_h - 1 == closed[iter_idx].y &&
               gc_retroflat_offsets4_y[i] == 1)
         ) {
            debug_printf( RETROTILE_PATH_TRACE_LVL,
               "> skipping overflow tile!" );
            continue;
         }

         /* Setup tentative adjacent tile physical properties. */
         /* Scores will be calculated in
          * retrotile_path_list_test_add_child(). */
         adjacent.x = closed[iter_idx].x + gc_retroflat_offsets4_x[i];
         adjacent.y = closed[iter_idx].y + gc_retroflat_offsets4_y[i];
         adjacent.dir = i;
         retval = retrotile_path_list_test_add_child(
            &adjacent, i, iter_idx, tgt_x, tgt_y, open, &open_sz,
            closed, closed_sz, flags, t, blocked_cb, blocked_cb_data );
         if( RETROTILE_RETVAL_BLOCKED == retval ) {
            /* TODO */
            retval = 0;
         }
         maug_cleanup_if_not_ok();

         debug_printf( RETROTILE_PATH_TRACE_LVL,
            ">> open list now has " SIZE_T_FMT " tiles", open_sz );
      }
   }

   if( tgt_reached ) {
      /* Return the next closest tile to the target. */
      debug_printf( RETROTILE_PATH_TRACE_LVL, "> tgt reached! %d, %d",
         path[*p_path_sz - 1].x, path[*p_path_sz - 1].y );
      retval = MERROR_OK;
   } else {
      debug_printf( RETROTILE_PATH_TRACE_LVL,
         "> blocked! (closed sz " SIZE_T_FMT ")", *p_path_sz );
   }

cleanup:

   debug_printf( RETROTILE_PATH_TRACE_LVL, "---END PATHFIND---" );

   return retval;
}

#endif /* RETROTIL_C */

#endif /* !RETROPTH_H */

