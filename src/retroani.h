
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

#ifndef RETROANI_TRACE_LVL
#  define RETROANI_TRACE_LVL 0
#endif /* !RETROANI_TRACE_LVL */

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
#define RETROANI_CB_TABLE( f ) f( 0, CIRCLE ) f( 1, RECTANGLE ) f( 2, FIRE ) f( 3, SNOW ) f( 4, CLOUDS ) f( 5, FRAMES ) f( 6, STRING )

/**
 * \addtogroup unilayer_animate_flags Unilayer Animation Flags
 * \brief Flags to control ::RETROANI behavior.
 *
 * \{
 */

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

#define RETROANI_TILE_SZ (RETROFLAT_TILE_W * RETROFLAT_TILE_H)

#define RETROANI_FIRE_COOLING_MAX 35
#define RETROANI_FIRE_COOLING_MIN 25
#define RETROANI_FIRE_WIND 1
#define RETROANI_FIRE_HEAT_INIT 70
#define RETROANI_FIRE_HEAT_RANGE 40

#define RETROANI_TEXT_HEADER_Y_OFFSET   0
#define RETROANI_TEXT_HEADER_STR_SZ     1
#define RETROANI_TEXT_HEADER_STR        2
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

#ifndef RETROANI_COLOR_CT_MAX
#  define RETROANI_COLOR_CT_MAX 4
#endif /* !RETROANI_COLOR_CT_MAX */

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
   RETROFLAT_COLOR colors[RETROANI_COLOR_CT_MAX];
};

/*! \brief Callback to call on active animations for every frame. */
typedef MERROR_RETVAL (*RETROANI_CB)( struct RETROANI* a );

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

MERROR_RETVAL retroani_set_colors(
   struct MDATA_VECTOR* ani_stack, size_t a_idx,
   RETROFLAT_COLOR c1, RETROFLAT_COLOR c2, RETROFLAT_COLOR c3,
   RETROFLAT_COLOR c4 );

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
   MERROR_RETVAL retroani_draw_ ## name( struct RETROANI* );

RETROANI_CB_TABLE( RETROANI_CB_TABLE_DRAW_PROTOTYPES )

#ifdef RETROANI_C

#define RETROANI_CB_TABLE_LIST( idx, name ) retroani_draw_ ## name,

MAUG_CONST RETROANI_CB SEG_MCONST gc_animate_draw[] = {
   RETROANI_CB_TABLE( RETROANI_CB_TABLE_LIST )
};

#define RETROANI_CB_TABLE_TYPES( idx, name ) MAUG_CONST uint8_t SEG_MCONST RETROANI_TYPE_ ## name = idx;

RETROANI_CB_TABLE( RETROANI_CB_TABLE_TYPES );

MERROR_RETVAL retroani_draw_RECTANGLE( struct RETROANI* a ) {
   /* TODO */
   return MERROR_OK;
}

MERROR_RETVAL retroani_draw_CIRCLE( struct RETROANI* a ) {
   /* TODO */
   return MERROR_OK;
}

MERROR_RETVAL retroani_draw_FIRE( struct RETROANI* a ) {
   int8_t x = 0,
      next_x = 0,
      y = 0;
   uint16_t
      idx = 0,
      next_idx = 0;

   if( !(a->flags & RETROANI_FLAG_INIT) ) {
      /* Setup initial "heat line" from which fire is drawn. */
      for( x = 0 ; RETROFLAT_TILE_W > x ; x++ ) {
         idx = ((RETROFLAT_TILE_H - 1) * RETROFLAT_TILE_W) + x;
         /* a->tile[idx] = graphics_get_random( 70, 101 ); */
         a->tile[idx] = RETROANI_FIRE_HEAT_INIT +
            retroflat_get_rand() % RETROANI_FIRE_HEAT_RANGE;
         assert( 0 < a->tile[idx] );
      }

      a->flags |= RETROANI_FLAG_INIT;
   }

   for( y = 0 ; RETROFLAT_TILE_H - 1 > y ; y++ ) {
      /* debug_printf( 3, "%d, %d: %d", 0, y, data[(y * a->w)] ); */
      for( x = 0 ; RETROFLAT_TILE_W > x ; x++ ) {
         idx = (y * RETROFLAT_TILE_W) + x;

         /* Make sure we don't overflow the buffer. */
         /* next_x = x + graphics_get_random( -1, 3 ); */
         next_x = (x - 1) + retroflat_get_rand() % 3;
         if( 0 > next_x ) {
            next_x = RETROFLAT_TILE_W - 1;
         } else if( RETROFLAT_TILE_W <= next_x ) {
            next_x = 0;
         }

         next_idx = ((y + 1) * RETROFLAT_TILE_W) + next_x;

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
   retroani_tesselate( a, a->h - RETROFLAT_TILE_H );

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retroani_draw_SNOW( struct RETROANI* a ) {
   int16_t
      x = 0,
      y = 0,
      idx = 0,
      new_idx = 0;

   if( !(a->flags & RETROANI_FLAG_INIT) ) {
      /* Create initial snowflakes along the left side of the tile. */
      for( y = 0 ; RETROFLAT_TILE_H > y ; y += 4 ) {
         idx = (y * RETROFLAT_TILE_W);
         a->tile[idx] = 1;
      }

      a->flags |= RETROANI_FLAG_INIT;
   }
 
   for( y = RETROFLAT_TILE_H - 1 ; 0 <= y ; y-- ) {
      for( x = RETROFLAT_TILE_W - 1 ; 0 <= x ; x-- ) {
         idx = (y * RETROFLAT_TILE_W) + x;

         if( -1 == a->tile[idx] ) {
            /* Presume hiding was done. */
            a->tile[idx] = 0;

         } else if( 0 < a->tile[idx] ) {
            /* Hide the snowflake's previous position. */
            a->tile[idx] = -1;

            do {
               /* Move the snowflake down and maybe to the right. */
               /* = idx + RETROFLAT_TILE_W + graphics_get_random( 0, 3 ); */
               new_idx = idx + RETROFLAT_TILE_W + (retroflat_get_rand() % 3);

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

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retroani_draw_CLOUDS( struct RETROANI* a ) {
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
      for( y = 0 ; RETROFLAT_TILE_H > y ; y++ ) {
         
         /* Get new non-repeating offset for each row. */
         do {
            /* = graphics_get_random( 0, RETROFLAT_TILE_W / 4 ); */
            row_start_idx = retroflat_get_rand() % (RETROFLAT_TILE_W / 4 );
         } while( row_start_idx == row_start_last );

         /* Draw the row's initial state. */
         for( x = 0 ; RETROFLAT_TILE_W > x ; x++ ) {
            if(
               RETROANI_FLAG_CLOUDS_ROTATE !=
               (RETROANI_FLAG_CLOUDS_ROTATE & a->flags)
            ) {
               idx = (y * RETROFLAT_TILE_W) + x;
            } else {
               idx = (x * RETROFLAT_TILE_W) + y;
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
      for( y = RETROFLAT_TILE_H - 1 ; 0 <= y ; y-- ) {

         * Do we advance this wisp on this iteration? Not always. */
         row_col_offset = graphics_get_random( 0, 30 );
         if( 5 < row_col_offset ) {
            continue;
         }

         /* Iterate each row. */
         row_col_idx = (y * RETROFLAT_TILE_W);
         row_col_end_buffer = a->tile[row_col_idx + (RETROFLAT_TILE_W - 1)];

         for( x = RETROFLAT_TILE_W - 1 ; 0 <= x ; x-- ) {
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

      for( x = RETROFLAT_TILE_W - 1 ; 0 <= x ; x-- ) {

         /* Do we advance this wisp on this iteration? Not always. */
         row_col_offset = graphics_get_random( 0, 30 );
         if( 5 < row_col_offset ) {
            continue;
         }

         /* Iterate each row. */
         for( y = RETROFLAT_TILE_H - 1 ; 0 <= y ; y-- ) {
            idx = (y * RETROFLAT_TILE_W) + x;
            row_col_end_buffer = a->tile[((RETROFLAT_TILE_H - 1) * y) + x];
            prev_idx = idx - RETROFLAT_TILE_W;

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

   for( y = RETROFLAT_TILE_H - 1 ; 0 <= y ; y-- ) {

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
      for( x = RETROFLAT_TILE_W - 1 ; 0 <= x ; x-- ) {

         idx = (y * RETROFLAT_TILE_W) + x;
         if(
            RETROANI_FLAG_CLOUDS_ROTATE ==
            (RETROANI_FLAG_CLOUDS_ROTATE & a->flags)
         ) {
            row_col_end_buffer = 
               /* Grab the pixel in this column off the bottom. */
               a->tile[((RETROFLAT_TILE_H - 1) * RETROFLAT_TILE_W) + x];
            prev_idx = idx - RETROFLAT_TILE_W;
         } else {
            row_col_end_buffer =
               /* Grab the pixel in this row off the right. */
               a->tile[(y * RETROFLAT_TILE_W) + (RETROFLAT_TILE_W - 1)];
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

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retroani_draw_STRING( struct RETROANI* ani ) {
   int8_t* p_y_offset = NULL;
   uint8_t* p_str_sz = NULL;
   char* str = NULL;
   size_t str_height = 0;

   p_y_offset = (int8_t*)&(ani->tile[RETROANI_TEXT_HEADER_Y_OFFSET]);
   str = (char*)&(ani->tile[RETROANI_TEXT_HEADER_STR]);
   p_str_sz = (uint8_t*)&(ani->tile[RETROANI_TEXT_HEADER_STR_SZ]);

   if( *p_y_offset > RETROFLAT_TILE_H ) {
      return MERROR_EXEC;
   }

   /* Draw the animation text. */
#ifdef RETROGXC_PRESENT
   retrogxc_string_sz(
      ani->target, str, *p_str_sz,
      ani->font_idx, ani->w, ani->h, NULL, &str_height, 0 );
   retrogxc_string(
      ani->target, ani->colors[0], str, *p_str_sz, ani->font_idx,
#else
   retrofont_string_sz(
      ani->target, str, *p_str_sz,
      ani->font_h, ani->w, ani->h, NULL, &str_height, 0 );
   retrofont_string(
      ani->target, ani->colors[0], str, *p_str_sz, ani->font_h,
#endif /* RETROGXC_PRESENT */
      ani->x, ani->y + ani->h - (*p_y_offset), ani->w, ani->h, 0 );

   if( 0 == retroflat_heartbeat() ) {
      /* Move the text up half a line until it would leave the animation. */
      *p_y_offset += str_height;
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retroani_draw_FRAMES( struct RETROANI* a ) {
   /* TODO */
   return MERROR_OK;
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
   int8_t* p_y_offset = NULL;
   uint8_t* p_str_sz = NULL;
   char* str = NULL;
   struct RETROANI* ani = NULL;
   size_t str_height = 0;

   assert( mdata_vector_ct( ani_stack ) > a_idx );

   mdata_vector_lock( ani_stack );
   ani = mdata_vector_get( ani_stack, a_idx, struct RETROANI );
   assert( NULL != ani );

   p_y_offset = (int8_t*)&(ani->tile[RETROANI_TEXT_HEADER_Y_OFFSET]);
   str = (char*)&(ani->tile[RETROANI_TEXT_HEADER_STR]);
   p_str_sz = (uint8_t*)&(ani->tile[RETROANI_TEXT_HEADER_STR_SZ]),

   /* assert( RETROANI_TEXT_MAX_SZ > *p_str_sz );
   assert( RETROANI_TYPE_STRING == ani->type ); */
 
#ifdef RETROGXC_PRESENT
   ani->font_idx = retrogxc_load_font( font_name_in, 0, 33, 93 );
   retrogxc_string_sz(
      ani->target, str, str_sz_in,
      ani->font_idx, ani->w, ani->h, NULL, &str_height, 0 );
#else
   retrofont_string_sz(
      ani->target, str, *p_str_sz,
      ani->font_h, ani->w, ani->h, NULL, &str_height, 0 );
   retval = retrofont_load( font_name_in, &(ani->font_h), 0, 33, 93 );
#endif /* RETROGXC_PRESENT */

   maug_strncpy( str, str_in, RETROANI_TEXT_MAX_SZ - 1 );
   *p_str_sz = str_sz_in;
   ani->colors[0] = color_idx_in;
   *p_y_offset = str_height;

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

      debug_printf( RETROANI_TRACE_LVL,
         "setting hole for animation " SIZE_T_FMT ": "
         PXXY_FMT ", " PXXY_FMT " (" PXXY_FMT " x " PXXY_FMT ")",
         i, x, y, w, h );

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

MERROR_RETVAL retroani_set_colors(
   struct MDATA_VECTOR* ani_stack, size_t a_idx,
   RETROFLAT_COLOR c1, RETROFLAT_COLOR c2, RETROFLAT_COLOR c3,
   RETROFLAT_COLOR c4
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROANI* ani = NULL;

   if( a_idx >= mdata_vector_ct( ani_stack ) ) {
      return MERROR_OVERFLOW;
   }

   mdata_vector_lock( ani_stack );
   ani = mdata_vector_get( ani_stack, a_idx, struct RETROANI );
   assert( NULL != ani );

   ani->colors[0] = c1;
   ani->colors[1] = c2;
   ani->colors[2] = c3;
   ani->colors[3] = c4;

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

   debug_printf( RETROANI_TRACE_LVL,
      "creating animation type: %u, flags: 0x%04x at %d, %d (%d x %d)",
      type, flags, x, y, w, h );

   ani_new.flags = flags;
   ani_new.x = x;
   ani_new.y = y;
   ani_new.w = w;
   ani_new.h = h;
   ani_new.type = type;
   ani_new.target = NULL;
   ani_new.next_frame_ms = 0;
   ani_new.mspf = RETROANI_DEFAUL_MSPF;
   maug_mzero( ani_new.tile, RETROANI_TILE_SZ );
   maug_mzero( &(ani_new.hole), sizeof( struct RETROANI_HOLE ) );

   if( RETROANI_TYPE_FIRE == type ) {
      ani_new.colors[0] = RETROFLAT_COLOR_RED;
      ani_new.colors[1] = RETROFLAT_COLOR_YELLOW;
      ani_new.colors[2] = RETROFLAT_COLOR_WHITE;
   } else if( RETROANI_TYPE_SNOW == type ) {
      ani_new.colors[0] = RETROFLAT_COLOR_WHITE;
   } else if( RETROANI_TYPE_STRING == type ) {
      ani_new.colors[0] = RETROFLAT_COLOR_BLACK;
   }

   idx_out = mdata_vector_append(
      ani_stack, &ani_new, sizeof( struct RETROANI ) );

   debug_printf( RETROANI_TRACE_LVL,
      "created animation at idx: " SSIZE_T_FMT, idx_out );

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
   for( t_y = y_orig ; a->h > t_y ; t_y += RETROFLAT_TILE_H ) {
      for( t_x = 0 ; a->w > t_x ; t_x += RETROFLAT_TILE_W ) {
         /* Iterate over every pixel of the animation grid. */
         for( y = 0 ; RETROFLAT_TILE_H > y ; y++ ) {
            for( x = 0 ; RETROFLAT_TILE_W > x ; x++ ) {
               idx = (y * RETROFLAT_TILE_W) + x;

               /* TODO: Try to trim animation to its area. */

               p_x = a->x + t_x + x;
               p_y = a->y + t_y + y;

               if( h_on && p_x > h_l && p_x < h_r && p_y > h_t && p_y < h_b ) {
                  /* We're inside an active animation "hole". */
                  debug_printf(
                     RETROANI_TRACE_LVL,
                     "%u 0x%02x: " PXXY_FMT ", " PXXY_FMT " is inside hole at "
                     "L" PXXY_FMT ", R" PXXY_FMT ", T" PXXY_FMT ", B" PXXY_FMT,
                     a->type, h_on, p_x, p_y, h_l, h_r, h_t, h_b );

                  continue;
               }

               if(
                  -1 == a->tile[idx] &&
                  RETROANI_FLAG_CLEANUP == (RETROANI_FLAG_CLEANUP & a->flags)
               ) {
                  retroflat_2d_px(
                     a->target, RETROFLAT_COLOR_BLACK, p_x, p_y, 0 );

               } else if( 0 < a->tile[idx] && RETROANI_TYPE_SNOW == a->type ) {
                  retroflat_2d_px( a->target, a->colors[0], p_x, p_y, 0 );
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
                  retroflat_2d_px( a->target, a->colors[2], p_x, p_y, 0 );
               } else if( 60 < a->tile[idx] ) {
                  retroflat_2d_px( a->target, a->colors[1], p_x, p_y, 0 );
               } else if( 30 < a->tile[idx] ) {
                  retroflat_2d_px( a->target, a->colors[0], p_x, p_y, 0 );
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
   ssize_t i = 0; /* So i can be -1 if we delete the first ani. */
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

      debug_printf( RETROANI_TRACE_LVL,
         "drawing animatione: " SSIZE_T_FMT ", type: %d", i, ani->type );

      ani->next_frame_ms = now_ms + ani->mspf;
      if( MERROR_EXEC == gc_animate_draw[ani->type]( ani ) ) {
         mdata_vector_unlock( ani_stack );
         mdata_vector_remove( ani_stack, i );
         mdata_vector_lock( ani_stack );
         i--; /* Back up once, since this was deleted. */
      }

      debug_printf( RETROANI_TRACE_LVL,
         "drawing animation " SSIZE_T_FMT " complete!", i );

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

