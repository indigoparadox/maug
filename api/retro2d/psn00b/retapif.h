
#ifndef RETPLTF_H
#define RETPLTF_H

static volatile int32_t g_ms;

void _retroflat_psx_clear_buffers() {
   /* Clear and setup the draw operations ordering table and primitive buffer.
    * Use the current drawing screen's primative/ordering buffers, even if this
    * is an offscreen bitmap.
    */
   /* We're not using a reverse buffer here since this is for 2D! */
   ClearOTag(
      g_retroflat_state->platform.ot[retroflat_screen_buffer()->draw_idx],
      RETROFLAT_PSX_OT_LEN );
   g_retroflat_state->platform.next_prim =
      g_retroflat_state->platform.prim_buff[g_retroflat_state->buffer.draw_idx];
   g_retroflat_state->platform.used_prim = 0;
}

void _retroflat_psx_draw_buffers() {
   DrawOTag(
      g_retroflat_state->platform.ot[retroflat_screen_buffer()->draw_idx] );

   /* Wait for drawing to complete. */
   DrawSync( 0 );
}

void _retroflat_psx_add_prim( void* prim, size_t prim_sz ) {

   if(
      RETROFLAT_PSX_PRIM_BUF_SZ <=
      g_retroflat_state->platform.used_prim + prim_sz
   ) {
      /* The primitive buffer filled up! So draw it and then clear it so it's
       * ready for more!
       */
      error_printf( "primitive buffer exceeded! flushing!" );
      _retroflat_psx_draw_buffers();
      _retroflat_psx_clear_buffers();
      return;
   }

   g_retroflat_state->platform.next_prim += prim_sz;
   g_retroflat_state->platform.used_prim += prim_sz;
   /*
   debug_printf( 1, "primitive buffer used: " SIZE_T_FMT, 
      g_retroflat_state->platform.used_prim );
   */

   /* Add the draw packet for this line from the primitive buffer to the
    * ordering table.
    */
   AddPrim(
      g_retroflat_state->platform.ot[retroflat_screen_buffer()->draw_idx],
      prim );
}

void _retroflat_psx_timer2_isr() {
   /* Increment ms timer. */
   g_ms++;
}

void _retroflat_psx_dbg_constrain( retroflat_pxxy_t x, retroflat_pxxy_t y ) {
#if RETRO2D_TRACE_LVL > 0
   error_printf( "attempted offscreen draw: %d, %d", x, y );
#endif /* RETRO2D_TRACE_LVL */
}

MERROR_RETVAL _retroflat_psx_fit_vram(
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   retroflat_pxxy_t* p_x, retroflat_pxxy_t* p_y
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0,
      j = 0,
      best_i = PXXY_MAX,
      best_j = 0,
      insert_bottom_right = 0,
      removed_pts_ct = 0;
   retroflat_pxxy_t iter_x = 0,
      iter_y = 0,
      best_x = PXXY_MAX,
      best_y = PXXY_MAX,
      new_best_y = 0;
   struct RETROFLAT_PSX_OSB_PT* pt_i = NULL;
   struct RETROFLAT_PSX_OSB_PT* pt_j = NULL;

   if( 0 == g_retroflat_state->platform.osb_pts_ct ) {
      /* Initialize the point array if none exist. */
      g_retroflat_state->platform.osb_pts[0].x = 0;
      g_retroflat_state->platform.osb_pts[0].y = 0;
      g_retroflat_state->platform.osb_pts_ct = 1;
   }

   for( i = 0 ; g_retroflat_state->platform.osb_pts_ct > i ; i++ ) {
      pt_i = &(g_retroflat_state->platform.osb_pts[i]);
      iter_x = pt_i->x;
      iter_y = pt_i->y;
      debug_printf( 1, "checking pt " SIZE_T_FMT " (%u, %u)...",
         i, pt_i->x, pt_i->y );
      
      if( RETROFLAT_PSX_VRAM_W - iter_x < w || iter_y >= best_y ) {
         /* We can't fit this bitmap in VRAM! */
         debug_printf( 1, "cannot fit %ux%u in current pack...", w, h );
         break;
      }

      for( j = i + 1 ; g_retroflat_state->platform.osb_pts_ct > j ; j++ ) {
         debug_printf( 1, "against pt " SIZE_T_FMT "...", i );
         pt_j = &(g_retroflat_state->platform.osb_pts[j]);

         if( iter_x + w <= pt_j->x ) {
            /* We cannot reach the next point. */
            break;
         }

         if( iter_y < pt_j->y ) {
            /* Raise Y to avoid clobbering this point. */
            iter_y = pt_j->y;
         }
      }

      if( iter_y >= best_y || RETROFLAT_PSX_VRAM_H - iter_y < h ) {
         continue;
      }

      /* If we made it this far, this point is our best bet! */
      best_i = i;
      best_j = j;
      debug_printf( 1, "best i: " SIZE_T_FMT "; best j: " SIZE_T_FMT,
         best_i, best_j );
      best_x = iter_x;
      best_y = iter_y;
   }

   if( PXXY_MAX == best_i ) {
      error_printf( "%ux%u will not fit in VRAM!", w, h );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Determine number of points to remove by distance between corner points
    * found above.
    */
   removed_pts_ct = best_j - best_i;
   assert( 1 <= best_j );
   new_best_y = g_retroflat_state->platform.osb_pts[best_j - 1].y;
   debug_printf( 1, "removing %d points...", removed_pts_ct );
   insert_bottom_right =
      best_j < g_retroflat_state->platform.osb_pts_ct ?
      /* Don't insert bottom-right if it's in the middle of a left wall. */
      best_x < g_retroflat_state->platform.osb_pts[best_j].x :
      /* Don't insert bottom-right if it butts up against VRAM end. */
      best_x < RETROFLAT_PSX_VRAM_W;
   debug_printf( 1, "inserting %d points...", insert_bottom_right + 1 );

   /* Make sure we haven't used up our points allocation. */
   if( insert_bottom_right + 1 >= RETROFLAT_PSX_OSB_PTS_CT_MAX ) {
      error_printf( "too many skyline points in VRAM!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Add space or remove points determined above. */
   if( insert_bottom_right + 1 > removed_pts_ct ) {
      /* Insert space for new point(s). */
      i = g_retroflat_state->platform.osb_pts_ct - 1;
      j = i + ((insert_bottom_right + 1) - removed_pts_ct);
      /* TODO: Can we use memmove() here? */
      for( ; i >= best_j ; --i, --j ) {
         memcpy(
            &(g_retroflat_state->platform.osb_pts[j]),
            &(g_retroflat_state->platform.osb_pts[i]),
            sizeof( struct RETROFLAT_PSX_OSB_PT ) );
      }
      g_retroflat_state->platform.osb_pts_ct += 
         ((insert_bottom_right + 1) - removed_pts_ct);
   } else if( insert_bottom_right + 1 < removed_pts_ct ) {
      /* Remove overlapped points. */
      i = best_j;
      j = i - (removed_pts_ct - (insert_bottom_right + 1));
      /* TODO: Can we use memmove() here? */
      for( ; i < g_retroflat_state->platform.osb_pts_ct ; i++, j++ ) {
         memcpy(
            &(g_retroflat_state->platform.osb_pts[j]),
            &(g_retroflat_state->platform.osb_pts[i]),
            sizeof( struct RETROFLAT_PSX_OSB_PT ) );
      }
      g_retroflat_state->platform.osb_pts_ct -=
         (removed_pts_ct - (insert_bottom_right + 1));
   }

   /* Perform the actual insertion(s) at the new best point. */
   g_retroflat_state->platform.osb_pts[best_i].x = best_x;
   g_retroflat_state->platform.osb_pts[best_i].y = best_y + h;
   if( insert_bottom_right ) {
      g_retroflat_state->platform.osb_pts[best_i + 1].x = best_x + w;
      g_retroflat_state->platform.osb_pts[best_i + 1].y = new_best_y;
   }

   *p_x = best_x;
   *p_y = best_y;

cleanup:

   return retval;
}

static MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Force screen size. */
   args->screen_w = 320;
   args->screen_h = 240;

   /*
   g_retroflat_state->buffer.w = 320;
   g_retroflat_state->buffer.h = 240;
   */

   /* Setup PSX graphics. */
   debug_printf( 1, "setting up GPU..." );
   ResetGraph( 0 );
   SetVideoMode( 0 );

   /* Setup the counter to provide a monotonic clock. */
   debug_printf( 1, "setting up tick timer..." );
   EnterCriticalSection();
	InterruptCallback( IRQ_TIMER2, &_retroflat_psx_timer2_isr );
   ResetRCnt( RCntCNT2 );
   SetRCnt(
      RCntCNT2, 0xffff, RCntMdINTR | RCntMdSC | RCntMdDIV8 | RCntMdTARGET );
   StartRCnt( RCntCNT2 );
	ExitCriticalSection();

   debug_printf( 1, "creating screen buffer..." );
   retroflat_create_bitmap(
      args->screen_w, args->screen_h, &(g_retroflat_state->buffer),
      RETROFLAT_FLAGS_SCREEN_BUFFER );

   PutDispEnv( &(g_retroflat_state->platform.disp[0]) );
   PutDrawEnv( &(g_retroflat_state->buffer.draw[0]) );

   _retroflat_psx_clear_buffers();

   /* Show the screen. */
   SetDispMask( 1 );

   /* Setup color constants. */
#  define RETROFLAT_COLOR_TABLE_PSX_RGBS_INIT( idx, name_l, name_u, ir, ig, ib, cgac, cgad ) \
      g_retroflat_state->palette[idx].r = ir; \
      g_retroflat_state->palette[idx].g = ig; \
      g_retroflat_state->palette[idx].b = ib;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_PSX_RGBS_INIT )

   /* Seed the RNG based on how long it took to get this far. */
   g_retroflat_state->platform.rand_state = retroflat_get_ms();

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {
   /* Nothing to do! Shutdown is power-off! */
}

/* === */

MERROR_RETVAL retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Just skip to the generic loop. */
   retval = retroflat_loop_generic( frame_iter, loop_iter, data );

   /* This should be set by retroflat_quit(). */
   return retval;
}

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   maug_mzero( msg_out, RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   /* TODO */
#  pragma message( "warning: message not implemented" )

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Build the title. */
   va_start( vargs, format );

   /* Don't do anything! */

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   retroflat_ms_t ms = g_ms;

   /* debug_printf( 1, "ms: %d", ms ); */

   return ms;
}

/* === */

uint32_t retroflat_get_rand() {
   /* LCG based on "Numerical Recipes" chapter 7.1. */
   g_retroflat_state->platform.rand_state =
      g_retroflat_state->platform.rand_state * 1664525 + 1013904233;
   return g_retroflat_state->platform.rand_state;
}

/* === */

MERROR_RETVAL retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = RETROFLAT_OK;

   if( NULL == bmp ) {
      bmp = retroflat_screen_buffer();
   }

   if( retroflat_screen_buffer() == bmp ) {
      PutDispEnv( &(g_retroflat_state->platform.disp[bmp->draw_idx]) );
   }

   /* Assign the active drawing env to the given bitmap. */
   /* TODO: Create stack! */
   /* TODO: Draw current top of stack before locking this bitmap. */
   /* 
   _retroflat_psx_draw_buffers();
   */
   PutDrawEnv( &(bmp->draw[bmp->draw_idx]) );

   _retroflat_psx_clear_buffers();

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == bmp ) {
      bmp = retroflat_screen_buffer();
   }

   /* Draw any remaining operations to the released bitmap. */
   _retroflat_psx_draw_buffers();

   if( retroflat_screen_buffer() == bmp ) {
      /* Wait for GPU to finish drawing. */
      VSync( 0 );

      /* Flip the screen buffer index for the next screen-draw. */
      bmp->draw_idx = !(bmp->draw_idx);
   }

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap_px_cb(
   void* data, uint8_t px, int32_t x, int32_t y,
   void* header_info, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_BITMAP* b = (struct RETROFLAT_BITMAP*)data;

   retroflat_px( b, px, x, y, 0 );

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   retroflat_pxxy_t x1 = 0,
      y1 = 0,
      x2 = 0,
      y2 = 0;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   /* Try to allocate space on the VRAM "canvas" for the main drawing buffer of
    * this bitmap.
    */
   retval = _retroflat_psx_fit_vram( w, h, &x1, &y1 );
   maug_cleanup_if_not_ok();

   debug_printf( 1, "creating %dx%d bitmap at %d, %d in VRAM...",
      w, h, x1, y1 );

   SetDefDrawEnv( &(bmp_out->draw[0]), x1, y1, w, h );

   /* Setup drawenv defaults. */
   if(
      RETROFLAT_FLAGS_SCREEN_BUFFER == (RETROFLAT_FLAGS_SCREEN_BUFFER & flags)
   ) {
      /* Try to allocate space in VRAM for the "alternate" buffer for page
       * flipping.
       */
      /* TODO: Remove main bitmap if this fails, too. (But that should never
       *       happen!
       */
      retval = _retroflat_psx_fit_vram( w, h, &x2, &y2 );
      maug_cleanup_if_not_ok();

      debug_printf( 1, "creating %dx%d alternate buffer at %d, %d in VRAM...",
         w, h, x2, y2 );

      /* Setup drawenv for alternate buffer. */
      SetDefDrawEnv( &(bmp_out->draw[1]), x2, y2, w, h );
      setRGB0( &(bmp_out->draw[1]), 0, 0, 0 );

      /* Setup the screen display buffer environments.
       * Note that 0 uses 1's coords and 1 uses 0's coords, so that 0 is
       * showing while 1 is drawing and vice-versa.
       */
      SetDefDispEnv( &(g_retroflat_state->platform.disp[1]), x1, y1, w, h );
      SetDefDispEnv( &(g_retroflat_state->platform.disp[0]), x2, y2, w, h );

      /* Setup bitmap system data. */
      bmp_out->draw[1].isbg = 1;
      bmp_out->flags |= RETROFLAT_FLAGS_SCREEN_BUFFER;
   }
   setRGB0( &(bmp_out->draw[0]), 0, 0, 0 );
   bmp_out->draw[0].isbg = 1;
   bmp_out->w = w;
   bmp_out->h = h;

cleanup:

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

   /* TODO */
#  pragma message( "warning: destroy_bitmap not implemented" )

   bmp->sz = 0;

}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != src );

   /* TODO */
#  pragma message( "warning: blit_bitmap not implemented" )

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags
) {
   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
   ) {
      return;
   }

   retroflat_constrain_px( x, y, target,
      _retroflat_psx_dbg_constrain( x, y ); return );

   /* TODO */
#  pragma message( "warning: px not implemented" )

}

/* === */

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   /* TODO
   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
   } else { */
   retroflat_line( target, color_idx, x, y, x + w, y, 0 );
   retroflat_line( target, color_idx, x + w, y, x + w, y + h, 0 );
   retroflat_line( target, color_idx, x, y + h, x + w, y + h, 0 );
   retroflat_line( target, color_idx, x, y, x, y + h, 0 );
}

/* === */

void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t flags
) {
   LINE_F2* line = (LINE_F2*)(g_retroflat_state->platform.next_prim);

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   retroflat_constrain_px( x1, y1, target,
      _retroflat_psx_dbg_constrain( x1, y1 ); return );
   retroflat_constrain_px( x2, y2, target,
      _retroflat_psx_dbg_constrain( x2, y2 ); return );

   setLineF2( line );
   setRGB0( line,
      g_retroflat_state->palette[color_idx].r,
      g_retroflat_state->palette[color_idx].g,
      g_retroflat_state->palette[color_idx].b );
   setXY2( line, x1, y1, x2, y2 );

   _retroflat_psx_add_prim( line, sizeof( LINE_F2 ) );
}

/* === */

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {
   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   /* TODO
   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
   } else { */
   retrosoft_ellipse( target, color_idx, x, y, w, h, flags );
}

/* === */

void retroflat_resize_v() {
   /* Platform does not support resizing. */
}

#endif /* !RETPLTF_H */

