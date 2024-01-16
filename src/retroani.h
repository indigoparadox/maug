
#ifndef RETROANI_H
#define RETROANI_H

/**
 * \addtogroup unilayer_animate Unilayer Animation Layer
 * \brief Collection of common animation effects and related utilities.
 *
 * \{
 */

/*! \file animate.h
 *  \brief Tools for making common animations and effects.
 */

/**
 * \addtogroup unilayer_animate_effects_sect Unilayer Animation Effects
 * \{
 *
 * \page unilayer_animate_effects Unilayer Animation Effects
 * \tableofcontents
 *
 * \section unilayer_animate_effects_circle CIRCLE
 * Circle starting at center and radiating to w, h.
 * 
 * \section unilayer_animate_effects_rectangle RECTANGLE
 * (TODO) Rectangle starting at center and radiating to w, h.
 *
 * \section unilayer_animate_effects_fire FIRE
 * \image html firecga.png
 * Gradient of fire propagating upwards.
 *
 * \section unilayer_animate_effects_snow SNOW
 * (TODO) Dots moving from top left to bottom right.
 *
 * \section unilayer_animate_effects_frames FRAMES
 * (TODO) Individual bitmap frames from ::RESOURCE_ID.
 *
 * \}
 */
#define RETROANI_CB_TABLE( f ) f( 0, CIRCLE ) f( 1, RECTANGLE ) f( 2, FIRE ) f( 3, SNOW ) f( 4, CLOUDS ) f( 6, FRAMES )
/* f( 5, STRING ) */

/*! \brief Return from animate_create() indicating a problem. */
#define RETROANI_ERROR -1

#define RETROANI_TEMP_LOW()    RETROFLAT_COLOR_RED
#define RETROANI_TEMP_MED()    RETROFLAT_COLOR_YELLOW
#define RETROANI_TEMP_HIGH()   RETROFLAT_COLOR_WHITE

/**
 * \addtogroup unilayer_animate_flags Unilayer Animation Flags
 * \brief Flags to control ::ANIMATION behavior.
 *
 * \{
 */

/**
 * \relates ANIMATION
 * \brief ::ANIMATION::flags indicating animation is active and playing.
 */
#define RETROANI_FLAG_ACTIVE   0x0001
/**
 * \relates ANIMATION
 * \brief ::ANIMATION::flags indicating animation has been initialized.
 */
#define RETROANI_FLAG_INIT     0x0002
/**
 * \relates ANIMATION
 * \brief ::ANIMATION::flags indicating animation should black out previous
 *        frame's non-black pixels.
 */
#define RETROANI_FLAG_CLEANUP  0x0004
/**
 * \relates ANIMATION
 * \brief ::ANIMATION::flags indicating animation has been temporarily paused
 *        and should not update or draw on-screen.
 */
#define RETROANI_FLAG_PAUSED   0x0008

#define RETROANI_FLAG_CLOUDS_ROTATE 0x0010

/*! \} */

#define RETROANI_TILE_W 16
#define RETROANI_TILE_H 16
#define RETROANI_TILE_SZ (RETROANI_TILE_W * RETROANI_TILE_H)

#define RETROANI_FIRE_COOLING_MAX 35
#define RETROANI_FIRE_COOLING_MIN 25
#define RETROANI_FIRE_WIND 1
#define RETROANI_FIRE_HEAT_INIT 70
#define RETROANI_FIRE_HEAT_RANGE 40

#define RETROANI_TEXT_HEADER_Y_OFFSET   0
#define RETROANI_TEXT_HEADER_Y_COUNT    1
#define RETROANI_TEXT_HEADER_STR_SZ     2
#define RETROANI_TEXT_HEADER_COLOR_IDX  3
#define RETROANI_TEXT_HEADER_STR        4
#define RETROANI_TEXT_MAX_SZ            (RETROANI_TILE_SZ - RETROANI_TEXT_HEADER_STR)

#define RETROANI_CLOUD_WISP_LEN 8

#if 0
/*! \brief Used with FRAMES ::ANIMATION::type ANIMATION::data to list frames to
 *         play. */
/* TODO: Implementation. */
struct RETROANI_FRAME {
   RESOURCE_ID frame;
   MEMORY_HANDLE next;
};
#endif

/*! \brief Internal representation of an animation. Do not call directly; use
 *         retroani_create() instead.
 */
struct RETROANI {
   /*! \brief Type of animation this is (See ::RETROANI_CB_TABLE). */
   uint8_t type;
   /*! \brief Left origin of animation on screen in pixels. */
   int16_t x;
   /*! \brief Top origin of animation on screen in pixels. */
   int16_t y;
   /*! \brief Width of animation on screen in pixels. */
   int16_t w;
   /*! \brief Height of animation on screen in pixels. */
   int16_t h;
   /**
    * \brief Bitfield indicating animation's \ref unilayer_animate_flags.
    *
    * Lower 8 bits are Unilayer-specified flags. Upper 8 bits are application-
    * specific and will be ignored by Unilayer.
    */
   uint16_t flags;
   /*! \brief Data specific to particular animation playing. */
   int8_t tile[RETROANI_TILE_SZ];
   struct RETROFLAT_BITMAP* target;
};

/*! \brief Callback to call on active animations for every frame. */
typedef void (*RETROANI_CB)( struct RETROANI* a );

#if 0
/**
 * \brief Setup string animation.
 */
void retroani_set_string(
   struct RETROANI* ani_stack, size_t ani_stack_sz,
   int8_t a_idx, char* str_in, uint8_t str_sz_in, uint8_t color_idx_in );
#endif

/**
 * \brief Create a new animation in the global animations list.
 * \param type See ::RETROANI_CB_TABLE for animation types.
 * \param x Left origin of animation on screen in pixels.
 * \param y Top origin of animation on screen in pixels.
 * \param w Width of animation on screen in pixels.
 * \param h Height of animation on screen in pixels.
 * \return Internal index of newly created animation or ::RETROANI_ERROR.
 */
int8_t retroani_create(
   struct RETROANI* ani_stack, size_t ani_stack_sz,
   uint8_t type, uint16_t flags, int16_t x, int16_t y, int16_t w, int16_t h );

/**
 * \brief Draw the animation tile to the screen, tiled to fill its area.
 */
void retroani_tesselate( struct RETROANI* a, int16_t y_orig );

/**
 * \brief Should be called during every frame to overlay animations on screen.
 * \param flags Bitfield indicating which animations to animate/draw.
 */
void retroani_frame( struct RETROANI* ani_stack, size_t ani_stack_sz, uint16_t flags );

/**
 * \brief Pause all animations with the given flags without deleting them.
 * \param flags Bitfield indicating which animations to pause.
 */
void retroani_pause( struct RETROANI* ani_stack, size_t ani_stack_sz, uint16_t flags );

/**
 * \brief Resume all animations with the given flags that have been paused with
 *        retroani_pause().
 * \param flags Bitfield indicating which animations to resume.
 */
void retroani_resume( struct RETROANI* ani_stack, size_t ani_stack_sz, uint16_t flags );

/**
 * \brief Stop the animation with the given internal index.
 * \param idx Index to stop as returned from retroani_create().
 */
void retroani_stop( struct RETROANI* ani_stack, size_t ani_stack_sz, int8_t idx );

/**
 * \brief Stop all currently running animations on screen.
 */
void retroani_stop_all( struct RETROANI* ani_stack, size_t ani_stack_sz );

#define RETROANI_CB_TABLE_DRAW_PROTOTYPES( idx, name ) \
   void retroani_draw_ ## name( struct RETROANI* );

RETROANI_CB_TABLE( RETROANI_CB_TABLE_DRAW_PROTOTYPES )

#ifdef RETROANI_C

#define RETROANI_CB_TABLE_LIST( idx, name ) retroani_draw_ ## name,

const RETROANI_CB gc_animate_draw[] = {
   RETROANI_CB_TABLE( RETROANI_CB_TABLE_LIST )
};

#define RETROANI_CB_TABLE_TYPES( idx, name ) const uint8_t RETROANI_TYPE_ ## name = idx;

RETROANI_CB_TABLE( RETROANI_CB_TABLE_TYPES );

#if 0
static const RETROFLAT_COLOR gc_animation_colors[] = {
   RETROFLAT_COLOR_BLACK,
   RETROFLAT_COLOR_CYAN,
   RETROFLAT_COLOR_MAGENTA,
   RETROFLAT_COLOR_WHITE,
#ifdef DEPTH_VGA
   RETROFLAT_COLOR_DARKBLUE,
   RETROFLAT_COLOR_DARKGREEN,
   RETROFLAT_COLOR_TEAL,
   RETROFLAT_COLOR_DARKRED,
   RETROFLAT_COLOR_VIOLET,
   RETROFLAT_COLOR_BROWN,
   RETROFLAT_COLOR_GRAY,
   RETROFLAT_COLOR_DARKGRAY,
   RETROFLAT_COLOR_BLUE,
   RETROFLAT_COLOR_GREEN,
   RETROFLAT_COLOR_RED,
   RETROFLAT_COLOR_YELLOW
#endif /* DEPTH_VGA */
};
#endif

void retroani_draw_RECTANGLE( struct RETROANI* a ) {
   /* TODO */
}

void retroani_draw_CIRCLE( struct RETROANI* a ) {
   /* TODO */
}

void retroani_draw_FIRE( struct RETROANI* a ) {
   int8_t x = 0,
      next_x = 0,
      y = 0;
   uint16_t
      idx = 0,
      next_idx = 0;

   if( !(a->flags & RETROANI_FLAG_INIT) ) {
      /* Setup initial "heat line" from which fire is drawn. */
      for( x = 0 ; RETROANI_TILE_W > x ; x++ ) {
         idx = ((RETROANI_TILE_H - 1) * RETROANI_TILE_W) + x;
         /* a->tile[idx] = graphics_get_random( 70, 101 ); */
         a->tile[idx] = RETROANI_FIRE_HEAT_INIT +
            retroflat_get_rand() % RETROANI_FIRE_HEAT_RANGE;
         assert( 0 < a->tile[idx] );
      }

      a->flags |= RETROANI_FLAG_INIT;
   }

   for( y = 0 ; RETROANI_TILE_H - 1 > y ; y++ ) {
      /* debug_printf( 3, "%d, %d: %d", 0, y, data[(y * a->w)] ); */
      for( x = 0 ; RETROANI_TILE_W > x ; x++ ) {
         idx = (y * RETROANI_TILE_W) + x;

         /* Make sure we don't overflow the buffer. */
         /* next_x = x + graphics_get_random( -1, 3 ); */
         next_x = (x - 1) + retroflat_get_rand() % 3;
         if( 0 > next_x ) {
            next_x = RETROANI_TILE_W - 1;
         } else if( RETROANI_TILE_W <= next_x ) {
            next_x = 0;
         }

         next_idx = ((y + 1) * RETROANI_TILE_W) + next_x;

         /* Make sure integers don't rollover. */
         if( RETROANI_FIRE_COOLING_MAX + 3 >= a->tile[next_idx] ) {
            if( -1 == a->tile[idx] ) {
               /* Presume hiding was done. */
               a->tile[idx] = 0;

            } else if( 0 != a->tile[idx] ) {
               /* Hide previous pixel. */
               a->tile[idx] = -1;
            }
         } else {
            /* Propagate heat. */
            /* a->tile[idx] = a->tile[next_idx] - graphics_get_random(
               RETROANI_FIRE_COOLING_MIN, RETROANI_FIRE_COOLING_MAX ); */
            a->tile[idx] = (a->tile[next_idx] - RETROANI_FIRE_COOLING_MIN) +
               (retroflat_get_rand() % RETROANI_FIRE_COOLING_MAX);
         }
      }
   }

   /* Only tesselate the bottom row. */
   retroani_tesselate( a, a->h - RETROANI_TILE_H );

}

void retroani_draw_SNOW( struct RETROANI* a ) {
   int16_t
      x = 0,
      y = 0,
      idx = 0,
      new_idx = 0;

   if( !(a->flags & RETROANI_FLAG_INIT) ) {
      /* Create initial snowflakes along the left side of the tile. */
      for( y = 0 ; RETROANI_TILE_H > y ; y += 4 ) {
         idx = (y * RETROANI_TILE_W);
         a->tile[idx] = 1;
      }

      a->flags |= RETROANI_FLAG_INIT;
   }
 
   for( y = RETROANI_TILE_H - 1 ; 0 <= y ; y-- ) {
      for( x = RETROANI_TILE_W - 1 ; 0 <= x ; x-- ) {
         idx = (y * RETROANI_TILE_W) + x;

         if( -1 == a->tile[idx] ) {
            /* Presume hiding was done. */
            a->tile[idx] = 0;

         } else if( 0 < a->tile[idx] ) {
            /* Hide the snowflake's previous position. */
            a->tile[idx] = -1;

            do {
               /* Move the snowflake down and maybe to the right. */
               /* = idx + RETROANI_TILE_W + graphics_get_random( 0, 3 ); */
               new_idx = idx + RETROANI_TILE_W + (retroflat_get_rand() % 3);

                  /* Wrap the snowflake if it moves off-tile. */
               if( new_idx >= RETROANI_TILE_SZ ) {
                  new_idx -= RETROANI_TILE_SZ;
               }

            /* Don't let snowflakes merge over time. */
            } while( a->tile[new_idx] );
            
            /* Show the snowflake at its new position. */
            a->tile[new_idx] = 1;
         }
      }
   }

   retroani_tesselate( a, 0 );
}

void retroani_draw_CLOUDS( struct RETROANI* a ) {
   int8_t row_start_idx = 0,
      row_start_last = 0,
      row_col_end_buffer = 0, /* Last pixel wrapped off row end. */
      row_col_offset = 0,
      prev_row_col_offset = 0;
   int16_t
      x = 0,
      y = 0,
      idx = 0,
      prev_idx = 0;

   if( !(a->flags & RETROANI_FLAG_INIT) ) {
      /* Create initial cloud lines along the left side of the tile. */
      for( y = 0 ; RETROANI_TILE_H > y ; y++ ) {
         
         /* Get new non-repeating offset for each row. */
         do {
            /* = graphics_get_random( 0, RETROANI_TILE_W / 4 ); */
            row_start_idx = retroflat_get_rand() % (RETROANI_TILE_W / 4 );
         } while( row_start_idx == row_start_last );

         /* Draw the row's initial state. */
         for( x = 0 ; RETROANI_TILE_W > x ; x++ ) {
            if(
               RETROANI_FLAG_CLOUDS_ROTATE !=
               (RETROANI_FLAG_CLOUDS_ROTATE & a->flags)
            ) {
               idx = (y * RETROANI_TILE_W) + x;
            } else {
               idx = (x * RETROANI_TILE_W) + y;
            }
            if(
               x > row_start_idx && x < row_start_idx + RETROANI_CLOUD_WISP_LEN
            ) {
               a->tile[idx] = 100;
            } else {
               a->tile[idx] = 33;
            }
         }

         /* Save the row length to compare on next loop. */
         row_start_last = row_start_idx;
      }

      a->flags |= RETROANI_FLAG_INIT;
   }

   /* TODO: Add a parallax effect, maybe? */

#if 0
   if(
      RETROANI_FLAG_CLOUDS_ROTATE ==
      (RETROANI_FLAG_CLOUDS_ROTATE & a->flags)
   ) {
      for( y = RETROANI_TILE_H - 1 ; 0 <= y ; y-- ) {

         * Do we advance this wisp on this iteration? Not always. */
         row_col_offset = graphics_get_random( 0, 30 );
         if( 5 < row_col_offset ) {
            continue;
         }

         /* Iterate each row. */
         row_col_idx = (y * RETROANI_TILE_W);
         row_col_end_buffer = a->tile[row_col_idx + (RETROANI_TILE_W - 1)];

         for( x = RETROANI_TILE_W - 1 ; 0 <= x ; x-- ) {
            idx = row_col_idx + x;

            if( 0 == x ) {
               /* Wrap-around. */
               a->tile[row_col_idx] = row_col_end_buffer;

            } else {
               /* Cloud advance. */
               a->tile[idx] = a->tile[idx - 1];
            }
         }
      }

   } else {

      for( x = RETROANI_TILE_W - 1 ; 0 <= x ; x-- ) {

         /* Do we advance this wisp on this iteration? Not always. */
         row_col_offset = graphics_get_random( 0, 30 );
         if( 5 < row_col_offset ) {
            continue;
         }

         /* Iterate each row. */
         for( y = RETROANI_TILE_H - 1 ; 0 <= y ; y-- ) {
            idx = (y * RETROANI_TILE_W) + x;
            row_col_end_buffer = a->tile[((RETROANI_TILE_H - 1) * y) + x];
            prev_idx = idx - RETROANI_TILE_W;

            if( 0 == y ) {
               /* Wrap-around. */
               a->tile[idx] = row_col_end_buffer;

            } else {
               /* Cloud advance. */
               a->tile[idx] = a->tile[prev_idx];
            }
         }
      }

   }
#endif

   for( y = RETROANI_TILE_H - 1 ; 0 <= y ; y-- ) {

      /* TODO: Adapt this for rotated orientation... somewhat more complicated.
       */
      if(
         RETROANI_FLAG_CLOUDS_ROTATE !=
         (RETROANI_FLAG_CLOUDS_ROTATE & a->flags)
      ) {
         /* Do we advance this wisp on this iteration? Not always. */
         prev_row_col_offset = row_col_offset;
         /* row_col_offset = graphics_get_random( 0, 70 ); */
         row_col_offset = retroflat_get_rand() % 70;
         if( 45 > row_col_offset || 45 > prev_row_col_offset ) {
            continue;
         }
      }

      /* Iterate each row. */
      for( x = RETROANI_TILE_W - 1 ; 0 <= x ; x-- ) {

         idx = (y * RETROANI_TILE_W) + x;
         if(
            RETROANI_FLAG_CLOUDS_ROTATE ==
            (RETROANI_FLAG_CLOUDS_ROTATE & a->flags)
         ) {
            row_col_end_buffer = 
               /* Grab the pixel in this column off the bottom. */
               a->tile[((RETROANI_TILE_H - 1) * RETROANI_TILE_W) + x];
            prev_idx = idx - RETROANI_TILE_W;
         } else {
            row_col_end_buffer =
               /* Grab the pixel in this row off the right. */
               a->tile[(y * RETROANI_TILE_W) + (RETROANI_TILE_W - 1)];
            prev_idx = idx - 1;
         }

         if( 
            (RETROANI_FLAG_CLOUDS_ROTATE ==
               (RETROANI_FLAG_CLOUDS_ROTATE & a->flags) && 0 == y) ||
            (RETROANI_FLAG_CLOUDS_ROTATE !=
               (RETROANI_FLAG_CLOUDS_ROTATE & a->flags) && 0 == x)
         ) {
            /* Wrap-around. */
            a->tile[idx] = row_col_end_buffer;

         } else {
            /* Cloud advance. */
            a->tile[idx] = a->tile[prev_idx];
         }
      }
   }

   retroani_tesselate( a, 0 );

}

#if 0
/* TODO: Fix color stuff to use retroflat. */
void retroani_draw_STRING( struct RETROANI* a ) {
   int8_t* y_offset = (int8_t*)&(a->tile[RETROANI_TEXT_HEADER_Y_OFFSET]);
   uint8_t str_sz = (uint8_t)(a->tile[RETROANI_TEXT_HEADER_STR_SZ]),
      color_idx = (uint8_t)(a->tile[RETROANI_TEXT_HEADER_COLOR_IDX]),
      * y_count = (uint8_t*)&(a->tile[RETROANI_TEXT_HEADER_Y_COUNT]);
   char* str = (char*)&(a->tile[RETROANI_TEXT_HEADER_STR]);
   RETROFLAT_COLOR color;

#ifdef DEPTH_VGA
   assert( color_idx <= 16 );
#else
   assert( color_idx <= 3 );
#endif /* DEPTH_VGA */

   /* Select the color and draw the animation text. */
   color = gc_animation_colors[color_idx];
   graphics_string_at(
      str, str_sz, a->x, a->y + a->h - (*y_offset), color, 0 );

   /* Frame advancement delay. */
   if( *y_count < 2 ) {
      (*y_count)++;
      return;
   } else {
      *y_count = 0;
   }

   /* Move the text up half a line until it would leave the animation. */
   *y_offset += (FONT_H / 2);
   if( *y_offset > RETROANI_TILE_H ) {
      a->flags &= ~RETROANI_FLAG_ACTIVE;
   }
}
#endif

void retroani_draw_FRAMES( struct RETROANI* a ) {
   /* TODO */
}

#if 0
void retroani_set_string(
   struct RETROANI* ani_stack, size_t ani_stack_sz,
   int8_t a_idx, char* str_in, uint8_t str_sz_in, uint8_t color_idx_in
) {
   struct RETROANI* a = &(ani_stack[a_idx]);
   int8_t* y_offset = (int8_t*)&(a->tile[RETROANI_TEXT_HEADER_Y_OFFSET]);
   uint8_t* str_sz = (uint8_t*)&(a->tile[RETROANI_TEXT_HEADER_STR_SZ]),
      * color_idx = (uint8_t*)&(a->tile[RETROANI_TEXT_HEADER_COLOR_IDX]);
   char* str = (char*)&(a->tile[RETROANI_TEXT_HEADER_STR]);

   assert( 0 <= a_idx );
   assert( ani_stack_sz > a_idx );
   assert( RETROANI_TEXT_MAX_SZ > *str_sz );
   assert( RETROANI_TYPE_STRING == a->type );

   maug_memcopy( str, str_in, str_sz_in );
   *str_sz = str_sz_in;
   *color_idx = color_idx_in;
   *y_offset = FONT_H;
}
#endif

int8_t retroani_create(
   struct RETROANI* ani_stack, size_t ani_stack_sz,
   uint8_t type, uint16_t flags, int16_t x, int16_t y, int16_t w, int16_t h
) {
   int8_t i = 0,
      idx_out = RETROANI_ERROR;

   for( i = 0 ; ani_stack_sz > i ; i++ ) {
      if( !(ani_stack[i].flags & RETROANI_FLAG_ACTIVE) ) {
         idx_out = i;
         break;
      }
   }

   if( 0 > idx_out ) {
      error_printf( "animation table is full!" );
      goto cleanup;
   }

   ani_stack[i].flags = RETROANI_FLAG_ACTIVE | flags;
   ani_stack[i].x = x;
   ani_stack[i].y = y;
   ani_stack[i].w = w;
   ani_stack[i].h = h;
   ani_stack[i].type = type;
   maug_mzero( &(ani_stack[i].tile), RETROANI_TILE_SZ );

cleanup:
   return idx_out;
}

void retroani_tesselate( struct RETROANI* a, int16_t y_orig ) {
   int8_t
      /* Address of the current pixel rel to top-left corner of tile. */
      x = 0,
      y = 0;
   int16_t
      idx = 0,
      /* Address of the current tile's top-left corner rel to animation. */
      t_x = 0,
      t_y = 0,
      /* Address of the current pixel rel to screen. */
      p_x = 0,
      p_y = 0;

   /* Lock the target buffer for per-pixel manipulation. */
   retroflat_px_lock( a->target );

   /* Iterate over every tile covered by the animation's screen area. */
   for( t_y = y_orig ; a->h > t_y ; t_y += RETROANI_TILE_H ) {
      for( t_x = 0 ; a->w > t_x ; t_x += RETROANI_TILE_W ) {
         /* Iterate over every pixel of the animation grid. */
         for( y = 0 ; RETROANI_TILE_H > y ; y++ ) {
            for( x = 0 ; RETROANI_TILE_W > x ; x++ ) {
               idx = (y * RETROANI_TILE_W) + x;

               /* TODO: Try to trim animation to its area. */

               p_x = a->x + t_x + x;
               p_y = a->y + t_y + y;

               if(
                  -1 == a->tile[idx] &&
                  RETROANI_FLAG_CLEANUP == (RETROANI_FLAG_CLEANUP & a->flags)
               ) {
                  retroflat_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x, p_y, 0 );

               } else if( 0 < a->tile[idx] && RETROANI_TYPE_SNOW == a->type ) {
                  retroflat_px(
                     a->target, RETROFLAT_COLOR_WHITE, p_x, p_y, 0 );
#ifndef NO_SNOW_OUTLINE
                  retroflat_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x - 1, p_y, 0 );
                  retroflat_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x + 1, p_y, 0 );
                  retroflat_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x, p_y - 1, 0 );
                  retroflat_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x, p_y + 1, 0 );
#endif /* !NO_SNOW_OUTLINE */

               } else if( 90 < a->tile[idx] ) {
                  retroflat_px(
                     a->target, RETROANI_TEMP_HIGH(), p_x, p_y, 0 );
               } else if( 60 < a->tile[idx] ) {
                  retroflat_px( a->target, RETROANI_TEMP_MED(), p_x, p_y, 0 );
               } else if( 30 < a->tile[idx] ) {
                  retroflat_px( a->target, RETROANI_TEMP_LOW(), p_x, p_y, 0 );
               }
            }
         }
      }
   }

   retroflat_px_release( a->target );
}

void retroani_frame(
   struct RETROANI* ani_stack, size_t ani_stack_sz, uint16_t flags
) {
   int8_t i = 0;

   for( i = 0 ; ani_stack_sz > i ; i++ ) {
      if(
         RETROANI_FLAG_ACTIVE != (ani_stack[i].flags & RETROANI_FLAG_ACTIVE) ||
         RETROANI_FLAG_PAUSED == (ani_stack[i].flags & RETROANI_FLAG_PAUSED) ||
         flags != (flags & ani_stack[i].flags)
         
      ) {
         continue;
      }
      gc_animate_draw[ani_stack[i].type]( &(ani_stack[i]) );
   }
}

void retroani_pause(
   struct RETROANI* ani_stack, size_t ani_stack_sz, uint16_t flags
) {
   int8_t i = 0;

   for( i = 0 ; ani_stack_sz > i ; i++ ) {
      if( flags == (ani_stack[i].flags & flags) ) {
         ani_stack[i].flags |= RETROANI_FLAG_PAUSED;
      }
   }
}

void retroani_resume(
   struct RETROANI* ani_stack, size_t ani_stack_sz, uint16_t flags
) {
   int8_t i = 0;

   for( i = 0 ; ani_stack_sz > i ; i++ ) {
      if( flags == (ani_stack[i].flags & flags) ) {
         ani_stack[i].flags &= ~RETROANI_FLAG_PAUSED;
      }
   }
}

void retroani_stop(
   struct RETROANI* ani_stack, size_t ani_stack_sz, int8_t idx
) {
   maug_mzero( &(ani_stack[idx]), sizeof( struct RETROANI ) );
}

void retroani_stop_all(
   struct RETROANI* ani_stack, size_t ani_stack_sz
) {
   int8_t i = 0;

   for( i = 0 ; ani_stack_sz > i ; i++ ) {
      retroani_stop( ani_stack, ani_stack_sz, i );
   }
}

#else

/*! \brief Internal animation callback table. */
extern const RETROANI_CB gc_animate_draw[];

#define RETROANI_CB_TABLE_TYPES( idx, name ) extern const uint8_t RETROANI_TYPE_ ## name;

RETROANI_CB_TABLE( RETROANI_CB_TABLE_TYPES )

#endif /* RETROANI_C */

/*! \} */

#endif /* !RETROANI_H */

