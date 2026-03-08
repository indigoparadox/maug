
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

   /* Setup the screen display and drawing buffers. */
   SetDefDispEnv(
      &(g_retroflat_state->platform.disp[0]), 0, 0,
      args->screen_w, args->screen_h );
   SetDefDispEnv(
      &(g_retroflat_state->platform.disp[1]), 0, args->screen_h,
      args->screen_w, args->screen_h );

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
   debug_printf( 1, "rand: %u (%u)",
      g_retroflat_state->platform.rand_state,
      g_retroflat_state->platform.rand_state % 4 );
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

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[MAUG_PATH_SZ_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, RETROFLAT_BITMAP_EXT,
      filename_path, MAUG_PATH_SZ_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

   /* TODO */
#  pragma message( "warning: load_bitmap not implemented" )

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   /* Setup drawenv defaults. */
   SetDefDrawEnv( &(bmp_out->draw[0]), 0, h, w, h );
   SetDefDrawEnv( &(bmp_out->draw[1]), 0, 0, w, h );
   setRGB0( &(bmp_out->draw[0]), 0, 0, 0 );
   setRGB0( &(bmp_out->draw[1]), 0, 0, 0 );
   bmp_out->draw[0].isbg = 1;
   bmp_out->draw[1].isbg = 1;
   bmp_out->w = w;
   bmp_out->h = h;

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

