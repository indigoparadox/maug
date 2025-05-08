
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

#ifndef RETROANI_DEFAUL_MSPF
#  define RETROANI_DEFAUL_MSPF 100
#endif /* !RETROANI_DEFAUL_MSPF */

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
#define RETROANI_CB_TABLE( f ) f( 0, CIRCLE ) f( 1, RECTANGLE ) f( 2, FIRE ) f( 3, SNOW ) f( 4, CLOUDS ) f( 6, FRAMES ) f( 5, STRING )

#define RETROANI_TEMP_LOW()    RETROFLAT_COLOR_RED
#define RETROANI_TEMP_MED()    RETROFLAT_COLOR_YELLOW
#define RETROANI_TEMP_HIGH()   RETROFLAT_COLOR_WHITE

/**
 * \addtogroup unilayer_animate_flags Unilayer Animation Flags
 * \brief Flags to control ::RETROANI behavior.
 *
 * \{
 */

/**
 * \relates RETROANI
 * \brief ::RETROANI::flags indicating animation is active and playing.
 */
#define RETROANI_FLAG_ACTIVE   0x0001
/**
 * \relates RETROANI
 * \brief ::RETROANI::flags indicating animation has been initialized.
 */
#define RETROANI_FLAG_INIT     0x0002
/**
 * \relates RETROANI
 * \brief ::RETROANI::flags indicating animation should black out previous
 *        frame's non-black pixels.
 */
#define RETROANI_FLAG_CLEANUP  0x0004
/**
 * \relates RETROANI
 * \brief ::RETROANI::flags indicating animation has been temporarily paused
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
/*! \brief Used with FRAMES ::RETROANI::type RETROANI::data to list frames to
 *         play. */
/* TODO: Implementation. */
struct RETROANI_FRAME {
   RESOURCE_ID frame;
   MEMORY_HANDLE next;
};
#endif

#define retroani_set_mspf( ani_stack, ani_idx, mspf ) \
   ani_stack[ani_idx].mspf = mspf;

struct RETROANI_HOLE {
   retroflat_pxxy_t x;
   retroflat_pxxy_t y;
   retroflat_pxxy_t w;
   retroflat_pxxy_t h;
};

#ifndef RETROANI_HOLE_CT_MAX
#  define RETROANI_HOLE_CT_MAX 4
#endif /* !RETROANI_HOLE_CT_MAX */

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
   retroflat_blit_t* target;
   retroflat_ms_t next_frame_ms;
#ifdef RETROGXC_PRESENT
   ssize_t font_idx;
#else
   MAUG_MHANDLE font_h;
#endif /* RETROGXC_PRESENT */
   uint16_t mspf;
   struct RETROANI_HOLE hole;
};

/*! \brief Callback to call on active animations for every frame. */
typedef void (*RETROANI_CB)( struct RETROANI* a );

MERROR_RETVAL retroani_set_target(
   struct MDATA_VECTOR* ani_stack, size_t a_idx, retroflat_blit_t* target );

/**
 * \brief Setup string animation.
 */
MERROR_RETVAL retroani_set_string(
   struct MDATA_VECTOR* ani_stack, size_t a_idx,
   const char* str_in, size_t str_sz_in,
   const retroflat_asset_path font_name_in,
   RETROFLAT_COLOR color_idx_in );

/**
 * \brief Punch a "hole" in all animations with the given flags.
 */
MERROR_RETVAL retroani_set_hole(
   struct MDATA_VECTOR* ani_stack, uint16_t flags,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h );

/**
 * \brief Create a new animation in the global animations list.
 * \param type See ::RETROANI_CB_TABLE for animation types.
 * \param x Left origin of animation on screen in pixels.
 * \param y Top origin of animation on screen in pixels.
 * \param w Width of animation on screen in pixels.
 * \param h Height of animation on screen in pixels.
 * \return Internal index of newly created animation or \ref maug_error.
 */
ssize_t retroani_create(
   struct MDATA_VECTOR* ani_stack,
   uint8_t type, uint16_t flags, int16_t x, int16_t y, int16_t w, int16_t h );

/**
 * \brief Draw the animation tile to the screen, tiled to fill its area.
 */
void retroani_tesselate( struct RETROANI* a, int16_t y_orig );

/**
 * \brief Should be called during every frame to overlay animations on screen.
 * \param flags Bitfield indicating which animations to animate/draw.
 */
MERROR_RETVAL retroani_frame( struct MDATA_VECTOR* ani_stack, uint16_t flags );

/**
 * \brief Pause all animations with the given flags without deleting them.
 * \param flags Bitfield indicating which animations to pause.
 */
MERROR_RETVAL retroani_pause( struct MDATA_VECTOR* ani_stack, uint16_t flags );

/**
 * \brief Resume all animations with the given flags that have been paused with
 *        retroani_pause().
 * \param flags Bitfield indicating which animations to resume.
 */
MERROR_RETVAL retroani_resume( struct MDATA_VECTOR* ani_stack, uint16_t flags );

/**
 * \brief Stop the animation with the given internal index.
 * \param idx Index to stop as returned from retroani_create().
 */
MERROR_RETVAL retroani_stop( struct MDATA_VECTOR* ani, size_t idx );

/**
 * \brief Stop all currently running animations on screen.
 */
MERROR_RETVAL retroani_stop_all( struct MDATA_VECTOR* ani_stack );

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

/* === */

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

/* === */

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

/* === */

void retroani_draw_STRING( struct RETROANI* ani ) {
   int8_t* y_offset = NULL;
   uint8_t* str_sz = NULL,
      * color_idx = NULL,
      * y_count = NULL;
   char* str = NULL;

   y_offset = (int8_t*)&(ani->tile[RETROANI_TEXT_HEADER_Y_OFFSET]);
   y_count = (uint8_t*)&(ani->tile[RETROANI_TEXT_HEADER_Y_COUNT]);
   str = (char*)&(ani->tile[RETROANI_TEXT_HEADER_STR]);
   str_sz = (uint8_t*)&(ani->tile[RETROANI_TEXT_HEADER_STR_SZ]),
   color_idx = (uint8_t*)&(ani->tile[RETROANI_TEXT_HEADER_COLOR_IDX]);

   /* Draw the animation text. */
#ifdef RETROGXC_PRESENT
   retrogxc_string(
      ani->target, *color_idx, str, *str_sz, ani->font_idx,
#else
   retrofont_string(
      ani->target, *color_idx, str, *str_sz, ani->font_h,
#endif /* RETROGXC_PRESENT */
      ani->x, ani->y + ani->h - (*y_offset), ani->w, ani->h, 0 );

   /* Frame advancement delay. */
   if( *y_count < 2 ) {
      (*y_count)++;
      return;
   } else {
      *y_count = 0;
   }

   /* Move the text up half a line until it would leave the animation. */
   *y_offset += (10 / 2); /* TODO: Dynamic font height. */
   if( *y_offset > RETROANI_TILE_H ) {
      ani->flags &= ~RETROANI_FLAG_ACTIVE;
   }
}

/* === */

void retroani_draw_FRAMES( struct RETROANI* a ) {
   /* TODO */
}

/* === */

MERROR_RETVAL retroani_set_target(
   struct MDATA_VECTOR* ani_stack, size_t a_idx, retroflat_blit_t* target
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROANI* ani = NULL;

   assert( mdata_vector_ct( ani_stack ) > a_idx );

   mdata_vector_lock( ani_stack );
   ani = mdata_vector_get( ani_stack, a_idx, struct RETROANI );
   assert( NULL != ani );

   ani->target = target;

cleanup:

   mdata_vector_unlock( ani_stack );

   return retval;
}

/* === */

MERROR_RETVAL retroani_set_string(
   struct MDATA_VECTOR* ani_stack, size_t a_idx,
   const char* str_in, size_t str_sz_in,
   const retroflat_asset_path font_name_in,
   RETROFLAT_COLOR color_idx_in
) {
   MERROR_RETVAL retval = MERROR_OK;
   int8_t* y_offset = NULL;
   uint8_t* str_sz = NULL,
      * color_idx = NULL;
   char* str = NULL;
   struct RETROANI* ani = NULL;

   assert( 0 <= a_idx );
   assert( mdata_vector_ct( ani_stack ) > a_idx );

   mdata_vector_lock( ani_stack );
   ani = mdata_vector_get( ani_stack, a_idx, struct RETROANI );
   assert( NULL != ani );

   y_offset = (int8_t*)&(ani->tile[RETROANI_TEXT_HEADER_Y_OFFSET]);
   str = (char*)&(ani->tile[RETROANI_TEXT_HEADER_STR]);
   str_sz = (uint8_t*)&(ani->tile[RETROANI_TEXT_HEADER_STR_SZ]),
   color_idx = (uint8_t*)&(ani->tile[RETROANI_TEXT_HEADER_COLOR_IDX]);

   assert( RETROANI_TEXT_MAX_SZ > *str_sz );
   assert( RETROANI_TYPE_STRING == ani->type );
 
#ifdef RETROGXC_PRESENT
   ani->font_idx = retrogxc_load_font( font_name_in, 0, 33, 93 );
#else
   retval = retrofont_load( font_name_in, &(ani->font_h), 0, 33, 93 );
#endif /* RETROGXC_PRESENT */

   maug_strncpy( str, str_in, RETROANI_TEXT_MAX_SZ - 1 );
   *str_sz = str_sz_in;
   *color_idx = color_idx_in;
   *y_offset = 10; /* TODO: Get font height dynamically. */

cleanup:

   mdata_vector_unlock( ani_stack );

   return retval;
}

/* === */

MERROR_RETVAL retroani_set_hole(
   struct MDATA_VECTOR* ani_stack, uint16_t flags,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   struct RETROANI* ani = NULL;

   if( 0 == mdata_vector_ct( ani_stack ) ) {
      return MERROR_OK;
   }

   mdata_vector_lock( ani_stack );
   for( i = 0 ; mdata_vector_ct( ani_stack ) > i ; i++ ) {
      ani = mdata_vector_get( ani_stack, i, struct RETROANI );
      assert( NULL != ani );
      if( flags != (flags & ani->flags) ) {
         continue;
      }
      ani->hole.x = x;
      ani->hole.y = y;
      ani->hole.w = w;
      ani->hole.h = h;
   }

cleanup:

   mdata_vector_unlock( ani_stack );

   return retval;

}

/* === */

ssize_t retroani_create(
   struct MDATA_VECTOR* ani_stack,
   uint8_t type, uint16_t flags, int16_t x, int16_t y, int16_t w, int16_t h
) {
   ssize_t idx_out = -1;
   struct RETROANI ani_new;

   ani_new.flags = RETROANI_FLAG_ACTIVE | flags;
   ani_new.x = x;
   ani_new.y = y;
   ani_new.w = w;
   ani_new.h = h;
   ani_new.type = type;
   ani_new.target = NULL;
   ani_new.next_frame_ms = 0;
   ani_new.mspf = RETROANI_DEFAUL_MSPF;
   maug_mzero( ani_new.tile, RETROANI_TILE_SZ );

   idx_out = mdata_vector_append(
      ani_stack, &ani_new, sizeof( struct RETROANI ) );

   return idx_out;
}

/* === */

void retroani_tesselate( struct RETROANI* a, int16_t y_orig ) {
   int8_t
      /* Address of the current pixel rel to top-left corner of tile. */
      x = 0,
      y = 0;
   int16_t idx = 0;
   retroflat_pxxy_t
      /* Address of the current tile's top-left corner rel to animation. */
      t_x = 0,
      t_y = 0,
      /* Address of the current pixel rel to screen. */
      p_x = 0,
      p_y = 0,
      h_on = 0,
      h_l = 0,
      h_r = 0,
      h_t = 0,
      h_b = 0;

   /* Lock the target buffer for per-pixel manipulation. */
   retroflat_px_lock( a->target );

   /* Setup the animation "hole" if defined. */
   if( 0 < a->hole.w && 0 < a->hole.h ) {
      h_on = 1;
      h_l = a->hole.x;
      h_r = a->hole.x + a->hole.w;
      h_t = a->hole.y;
      h_b = a->hole.y + a->hole.h;
   }

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

               if( h_on && p_x > h_l && p_x < h_r && p_y > h_t && p_y < h_b ) {
                  /* We're inside an active animation "hole". */
                  continue;
               }

               if(
                  -1 == a->tile[idx] &&
                  RETROANI_FLAG_CLEANUP == (RETROANI_FLAG_CLEANUP & a->flags)
               ) {
                  retroflat_2d_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x, p_y, 0 );

               } else if( 0 < a->tile[idx] && RETROANI_TYPE_SNOW == a->type ) {
                  retroflat_2d_px(
                     a->target, RETROFLAT_COLOR_WHITE, p_x, p_y, 0 );
#ifndef NO_SNOW_OUTLINE
                  retroflat_2d_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x - 1, p_y, 0 );
                  retroflat_2d_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x + 1, p_y, 0 );
                  retroflat_2d_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x, p_y - 1, 0 );
                  retroflat_2d_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x, p_y + 1, 0 );
#endif /* !NO_SNOW_OUTLINE */

               } else if( 90 < a->tile[idx] ) {
                  retroflat_2d_px(
                     a->target, RETROANI_TEMP_HIGH(), p_x, p_y, 0 );
               } else if( 60 < a->tile[idx] ) {
                  retroflat_2d_px( a->target, RETROANI_TEMP_MED(), p_x, p_y, 0 );
               } else if( 30 < a->tile[idx] ) {
                  retroflat_2d_px( a->target, RETROANI_TEMP_LOW(), p_x, p_y, 0 );
               }
            }
         }
      }
   }

   retroflat_px_release( a->target );
}

/* === */

MERROR_RETVAL retroani_frame( struct MDATA_VECTOR* ani_stack, uint16_t flags ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   uint32_t now_ms = 0;
   struct RETROANI* ani = NULL;

   if( 0 == mdata_vector_ct( ani_stack ) ) {
      return MERROR_OK;
   }

   now_ms = retroflat_get_ms();

   mdata_vector_lock( ani_stack );
   for( i = 0 ; mdata_vector_ct( ani_stack ) > i ; i++ ) {
      ani = mdata_vector_get( ani_stack, i, struct RETROANI );
      assert( NULL != ani );
      if(
         RETROANI_FLAG_PAUSED == (ani->flags & RETROANI_FLAG_PAUSED) ||
         flags != (flags & ani->flags) ||
         now_ms < ani->next_frame_ms
      ) {
         continue;
      }
      gc_animate_draw[ani->type]( ani );
      ani->next_frame_ms = now_ms + ani->mspf;
   }

cleanup:

   mdata_vector_unlock( ani_stack );

   return retval;
}

MERROR_RETVAL retroani_pause( struct MDATA_VECTOR* ani_stack, uint16_t flags ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   struct RETROANI* ani = NULL;

   if( 0 == mdata_vector_ct( ani_stack ) ) {
      return MERROR_OK;
   }

   mdata_vector_lock( ani_stack );
   for( i = 0 ; mdata_vector_ct( ani_stack ) > i ; i++ ) {
      ani = mdata_vector_get( ani_stack, i, struct RETROANI );
      assert( NULL != ani );
      if( flags == (ani->flags & flags) ) {
         ani->flags |= RETROANI_FLAG_PAUSED;
      }
   }

cleanup:

   mdata_vector_unlock( ani_stack );

   return retval;
}

/* === */

MERROR_RETVAL retroani_resume(
   struct MDATA_VECTOR* ani_stack, uint16_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   struct RETROANI* ani = NULL;

   if( 0 == mdata_vector_ct( ani_stack ) ) {
      return MERROR_OK;
   }

   mdata_vector_lock( ani_stack );
   for( i = 0 ; mdata_vector_ct( ani_stack ) > i ; i++ ) {
      ani = mdata_vector_get( ani_stack, i, struct RETROANI );
      assert( NULL != ani );
      if( flags == (ani->flags & flags) ) {
         ani->flags &= ~RETROANI_FLAG_PAUSED;
      }
   }

cleanup:

   mdata_vector_unlock( ani_stack );

   return retval;
}

/* === */

MERROR_RETVAL retroani_stop( struct MDATA_VECTOR* ani_stack, size_t idx ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROANI* ani = NULL;

   if(
      0 == mdata_vector_ct( ani_stack ) || mdata_vector_ct( ani_stack ) <= idx
   ) {
      return MERROR_OVERFLOW;
   }

   mdata_vector_lock( ani_stack );
   ani = mdata_vector_get( ani_stack, idx, struct RETROANI );
   assert( NULL != ani );
   maug_mzero( ani, sizeof( struct RETROANI ) );

cleanup:

   mdata_vector_unlock( ani_stack );

   return retval;
}

/* === */

MERROR_RETVAL retroani_stop_all( struct MDATA_VECTOR* ani_stack ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   if( 0 == mdata_vector_ct( ani_stack ) ) {
      return MERROR_OK;
   }

   for( i = 0 ; mdata_vector_ct( ani_stack ) > i ; i++ ) {
      retval = retroani_stop( ani_stack, i );
      maug_cleanup_if_not_ok();
   }

   mdata_vector_free( ani_stack );

cleanup:

   return retval;
}

#else

/*! \brief Internal animation callback table. */
extern const RETROANI_CB gc_animate_draw[];

#define RETROANI_CB_TABLE_TYPES( idx, name ) extern const uint8_t RETROANI_TYPE_ ## name;

RETROANI_CB_TABLE( RETROANI_CB_TABLE_TYPES )

#endif /* RETROANI_C */

/*! \} */

#endif /* !RETROANI_H */

