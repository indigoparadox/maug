
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

#define RETROTILE_PATH_ERROR_FARTHER -2

#define RETROTILE_PATH_ERROR_CLOSED -3

#define RETROTILE_RETVAL_BLOCKED -1

/**
 * \brief Flag for retrotile_path_start() indicating the target tile is occupied
 *        (e.g. we're following another mobile), so don't try to land on it.
 */
#define RETROTILE_PATH_FLAGS_TGT_OCCUPIED 0x01

#define retrotile_path_cmp_eq( a, b ) \
   (((a)->x == (b)->x) && (((a)->y == (b)->y)))

int8_t retrotile_path_start(
   uint16_t start_x, uint16_t start_y,
   uint16_t tgt_x, uint16_t tgt_y,
   struct RETROTILE_PATH_NODE* closed,
   size_t* p_closed_sz, size_t closed_sz_max,
   struct RETROTILE* t, uint8_t flags,
   retrotile_blocked_cb blocked_cb, void* blocked_cb_data );

/*! \} */ /* dsekai_pathfind */

#ifdef RETROTIL_C

static MERROR_RETVAL retrotile_path_list_append(
   struct RETROTILE_PATH_NODE* list, size_t* p_list_sz,
   struct RETROTILE_PATH_NODE* node
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( *p_list_sz + 1 >= RETROTILE_PATH_LIST_MAX ) {
      retval = MERROR_OVERFLOW;
   } else {
      memcpy( &(list[*p_list_sz]), node, sizeof( struct RETROTILE_PATH_NODE ) );
      (*p_list_sz)++;
   }

   return retval;
}

static void retrotile_path_list_remove(
   struct RETROTILE_PATH_NODE* list, size_t* p_list_sz, size_t idx
) {
   assert( idx < *p_list_sz );
   while( idx + 1 < *p_list_sz ) {
      memcpy(
         &(list[idx]), &(list[idx + 1]), sizeof( struct RETROTILE_PATH_NODE ) );
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
      if( list[lowest_f_idx].f > list[i].f ) {
         lowest_f_idx = i;
      }
   }

   debug_printf( RETROTILE_PATH_TRACE_LVL,
      "lowest cost open tile is %d, %d, cost is %d",
      list[lowest_f_idx].x, list[lowest_f_idx].y,
      list[lowest_f_idx].f );

   return lowest_f_idx;
}

static int8_t retrotile_path_list_test_add_child( 
   struct RETROTILE_PATH_NODE* p_adjacent, uint8_t dir,
   uint8_t iter_closed_idx,
   uint16_t tgt_x, uint16_t tgt_y,
   struct RETROTILE_PATH_NODE* open, size_t* p_open_sz,
   struct RETROTILE_PATH_NODE* closed, size_t closed_sz, uint8_t flags,
   struct RETROTILE* t,
   retrotile_blocked_cb blocked_cb, void* blocked_cb_data
) {
   uint8_t i = 0,
      child_open_idx = 0;
   uint16_t a_x = p_adjacent->x,
      a_y = p_adjacent->y;
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROTILE_PATH_TRACE_LVL,
      "> testing adjacent tile at %d, %d...",
      p_adjacent->x, p_adjacent->y );
   
   /* Make sure adjacent is not in the closed list. */
   for( i = 0 ; closed_sz > i ; i++ ) {
      if( retrotile_path_cmp_eq( p_adjacent, &(closed[i]) ) ) {
         debug_printf( RETROTILE_PATH_TRACE_LVL,
            ">> tile is already closed!" );
         return RETROTILE_PATH_ERROR_CLOSED;
      }
   }

   /* Don't use occupied tiles. */
   if(
      (
         /* TODO: Does this still work in retrotile? */
         RETROTILE_PATH_FLAGS_TGT_OCCUPIED !=
         (RETROTILE_PATH_FLAGS_TGT_OCCUPIED & flags) ||
         /* Target tile is occupied (e.g. we're pathfinding to another
          * mobile), so skip this check for it. */
         (tgt_x != p_adjacent->x && tgt_y != p_adjacent->y)
      ) &&
      /* Otherwise check for occupied tile. */
      (NULL != blocked_cb &&
      RETROTILE_RETVAL_BLOCKED == blocked_cb(
         closed[iter_closed_idx].x,
         closed[iter_closed_idx].y, dir, t, blocked_cb_data ))
   ) {
      debug_printf( RETROTILE_PATH_TRACE_LVL,
         ">> tile %d, %d blocked by mobile or terrain!",
         closed[iter_closed_idx].x + gc_retrotile_offsets8_x[dir],
         closed[iter_closed_idx].y + gc_retrotile_offsets8_y[dir] );
      return RETROTILE_RETVAL_BLOCKED;
   }

   /* Calculate adjacent pathfinding properties. */

   debug_printf( RETROTILE_PATH_TRACE_LVL,
      ">> tile parent %d, %d, distance %d",
      closed[iter_closed_idx].x,
      closed[iter_closed_idx].y,
      closed[iter_closed_idx].g );

   p_adjacent->g = closed[iter_closed_idx].g + 1;
   debug_printf( RETROTILE_PATH_TRACE_LVL,
      ">> tile distance is %d", p_adjacent->g );
   
   /* Use Manhattan heuristic since we can only move in 4 dirs. */
   p_adjacent->h = abs( a_x - tgt_x ) + abs( a_y - tgt_y );
   debug_printf( RETROTILE_PATH_TRACE_LVL,
      ">> tile heuristic is %d", p_adjacent->h );

   p_adjacent->f = p_adjacent->g + p_adjacent->h;
   debug_printf( RETROTILE_PATH_TRACE_LVL,
      ">> tile cost is %d", p_adjacent->f );

   for( i = 0 ; *p_open_sz > i ; i++ ) {
      /* Make sure adjacent is not already on the open list. */
      if( retrotile_path_cmp_eq( p_adjacent, &(open[i]) ) ) {
         return RETROTILE_PATH_ERROR_FARTHER;
      }
   }

   /* Add the child to the list. */
   debug_printf( RETROTILE_PATH_TRACE_LVL,
      ">> adding tile to open list" );
   retval = retrotile_path_list_append( open, p_open_sz, p_adjacent );
   if( MERROR_OK != retval ) {
      debug_printf( RETROTILE_PATH_TRACE_LVL, ">> open list full!" );
      return MERROR_OVERFLOW;
   }

   return child_open_idx;
}

int8_t retrotile_path_start(
   uint16_t start_x, uint16_t start_y,
   uint16_t tgt_x, uint16_t tgt_y,
   struct RETROTILE_PATH_NODE* closed,
   size_t* p_closed_sz, size_t closed_sz_max,
   struct RETROTILE* t, uint8_t flags,
   retrotile_blocked_cb blocked_cb, void* blocked_cb_data
) {
   struct RETROTILE_PATH_NODE open[RETROTILE_PATH_LIST_MAX];
   struct RETROTILE_PATH_NODE adjacent;
   size_t open_sz = 0;
   uint8_t iter_idx = 0,
      tgt_reached = 0,
      i = 0;
   MERROR_RETVAL retval = RETROTILE_RETVAL_BLOCKED;

   /* We directly subscript 1 below. */
   assert( 2 <= closed_sz_max );

   /* Zero out lists and nodes. */
   maug_mzero(
      open,
      sizeof( struct RETROTILE_PATH_NODE ) * RETROTILE_PATH_LIST_MAX );
   maug_mzero(
      closed, 
      sizeof( struct RETROTILE_PATH_NODE ) * closed_sz_max );
   maug_mzero( &adjacent, sizeof( struct RETROTILE_PATH_NODE ) );
   
   /* Add the start node to the open list. */
   adjacent.x = start_x;
   adjacent.y = start_y;
   retrotile_path_list_append( open, &open_sz, &adjacent );
   
   debug_printf( RETROTILE_PATH_TRACE_LVL, "---BEGIN PATHFIND---" );
   debug_printf( RETROTILE_PATH_TRACE_LVL,
      "pathfinding to %d, %d...", tgt_x, tgt_y );

   while( 0 < open_sz ) {
      iter_idx = retrotile_path_list_lowest_f( open, open_sz );

      /* Move the iter node to the closed list. */
      debug_printf( 1,
         "moving %d, %d to closed list idx " SIZE_T_FMT
         " and evaluating...",
         open[iter_idx].x, open[iter_idx].y, *p_closed_sz );
      memcpy(
         &(closed[*p_closed_sz]), &(open[iter_idx]), 
         sizeof( struct RETROTILE_PATH_NODE ) );
      retrotile_path_list_remove( open, &open_sz, iter_idx );
      iter_idx = *p_closed_sz;
      (*p_closed_sz)++;
      
      /* Validate closed list size. */
      if( *p_closed_sz >= closed_sz_max ) {
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
            (0 == closed[iter_idx].x && gc_retrotile_offsets8_x[i] == -1) ||
            (0 == closed[iter_idx].y && gc_retrotile_offsets8_y[i] == -1) ||
            (255 == closed[iter_idx].x && gc_retrotile_offsets8_x[i] == 1) ||
            (255 == closed[iter_idx].y && gc_retrotile_offsets8_y[i] == 1)
         ) {
            debug_printf( RETROTILE_PATH_TRACE_LVL,
               "> skipping overflow tile!" );
            continue;
         }

         /* Setup tentative adjacent tile physical properties. */
         /* Scores will be calculated in
          * retrotile_path_list_test_add_child(). */
         adjacent.x = closed[iter_idx].x + 
            gc_retrotile_offsets8_x[i];
         adjacent.y = closed[iter_idx].y +
            gc_retrotile_offsets8_y[i];
         adjacent.dir = i;
         retrotile_path_list_test_add_child(
            &adjacent, i, iter_idx, tgt_x, tgt_y, open, &open_sz,
            closed, *p_closed_sz, flags, t, blocked_cb, blocked_cb_data );

         debug_printf( RETROTILE_PATH_TRACE_LVL,
            ">> open list now has " SIZE_T_FMT " tiles", open_sz );
      }
   }

   if( tgt_reached ) {
      /* Return the next closest tile to the target. */
      debug_printf( RETROTILE_PATH_TRACE_LVL, "> tgt reached! %d, %d",
         closed[1].x, closed[1].y );
      retval = closed[1].dir;
   } else {
      debug_printf( RETROTILE_PATH_TRACE_LVL,
         "> blocked! (closed sz " SIZE_T_FMT ")", *p_closed_sz );
   }

cleanup:

   debug_printf( RETROTILE_PATH_TRACE_LVL, "---END PATHFIND---" );

   return retval;
}

#endif /* RETROTIL_C */

#endif /* !RETROPTH_H */

