
#ifndef RETPLTF_H
#define RETPLTF_H

#ifndef RETROFLAT_OPENGL

static SDL_Surface* _retroflat_sdl_load_bitmap( maug_path path ) {
   MERROR_RETVAL retval = MERROR_OK;
   SDL_Surface* bmp_out = NULL;
   SDL_RWops* rwo = NULL;
   mfile_t bmp_file;
   MAUG_MHANDLE buffer_h = (MAUG_MHANDLE)NULL;
   uint8_t* buffer = NULL;

   retval = mfile_open_read( path, &bmp_file );
   maug_cleanup_if_not_ok();

   /* Create a memory buffer. */
   maug_malloc_test( buffer_h, 1, bmp_file.sz );
   maug_mlock( buffer_h, buffer );
   maug_cleanup_if_null_lock( uint8_t*, buffer );

   /* Read the file into a memory buffer. */
   retval = bmp_file.read_block( &bmp_file, buffer, bmp_file.sz );
   maug_cleanup_if_not_ok();

   debug_printf( 1, "%c%c", buffer[0], buffer[1] );

   rwo = SDL_RWFromConstMem( buffer, bmp_file.sz );
   if( !rwo ) {
      error_printf( "could not setup SDL RWOps from mfile buffer!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

   /* Read the buffer into SDL (1 in second arg frees the RWOps)! */
   bmp_out = SDL_LoadBMP_RW( rwo, 1 );

cleanup:

   if( NULL != buffer ) {
      maug_munlock( buffer_h, buffer );
   }

   if( (MAUG_MHANDLE)NULL != buffer_h ) {
      maug_mfree( buffer_h );
   }

   return bmp_out;
}

#endif /* !RETROFLAT_OPENGL */

/* === */

MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_SDL_ICO )
   SDL_Surface* icon = NULL;
#  endif /* RETROFLAT_SDL_ICO */

   /* TODO: Add some flexibility for simulating lower-color platforms. */
   g_retroflat_state->screen_colors = 16;

   /* == Platform-Specific Init == */

   /* == SDL2 == */

   srand( time( NULL ) );

   if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER ) ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Error initializing SDL: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   /* Setup color palettes. */
#     ifndef RETROFLAT_OPENGL
#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
            g_retroflat_state->palette[idx].r = rd; \
            g_retroflat_state->palette[idx].g = gd; \
            g_retroflat_state->palette[idx].b = bd;
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL )
#     endif /* !RETROFLAT_OPENGL */

   /* Create the main window. */
   g_retroflat_state->platform.window = SDL_CreateWindow( args->title,
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      g_retroflat_state->screen_w,
      g_retroflat_state->screen_h, RETROFLAT_WIN_FLAGS );
   maug_cleanup_if_null(
      SDL_Window*, g_retroflat_state->platform.window,
      RETROFLAT_ERROR_GRAPHICS );

   /* Create the main renderer. */
   g_retroflat_state->platform.screen_buffer.renderer = SDL_CreateRenderer(
      g_retroflat_state->platform.window, -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE );
   maug_cleanup_if_null(
      SDL_Renderer*, g_retroflat_state->platform.screen_buffer.renderer,
      RETROFLAT_ERROR_GRAPHICS );

   /* Create the buffer texture. */
   g_retroflat_state->platform.screen_buffer.texture =
      SDL_CreateTexture(
         g_retroflat_state->platform.screen_buffer.renderer,
         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
         g_retroflat_state->screen_v_w
         /* Add off-screen tiles. */
         + (2 * RETROFLAT_TILE_W),
         g_retroflat_state->screen_v_h
         /* Add off-screen tiles. */
         + (2 * RETROFLAT_TILE_H) );

   g_retroflat_state->retroflat_flags |= RETROFLAT_STATE_FLAG_HWSCROLLING;

   /* Setup the viewport blit source rect to blit from the center of the
    * unnaturally large buffer by default. */
   g_retroflat_state->platform.viewport_rect.x = RETROFLAT_TILE_W;
   g_retroflat_state->platform.viewport_rect.y = RETROFLAT_TILE_H;
   g_retroflat_state->platform.viewport_rect.w =
      g_retroflat_state->screen_v_w;
   g_retroflat_state->platform.viewport_rect.h =
      g_retroflat_state->screen_v_h;

#     ifdef RETROFLAT_SDL_ICO
   debug_printf( 1, "setting SDL window icon..." );
   icon = SDL_LoadBMP_RW(
      SDL_RWFromConstMem( obj_ico_sdl_ico_bmp,
      obj_ico_sdl_ico_bmp_len ), 1 );
   maug_cleanup_if_null( SDL_Surface*, icon, MERROR_GUI );
   SDL_SetWindowIcon( g_retroflat_state->platform.window, icon );
#     endif /* RETROFLAT_SDL_ICO */

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {

   if( NULL != g_retroflat_state ) {
      SDL_DestroyWindow( g_retroflat_state->platform.window );
   }

   SDL_Quit();
}

/* === */

MERROR_RETVAL retroflat_loop(
   retroflat_loop_iter frame_iter, retroflat_loop_iter loop_iter, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_OS_WASM )

   /* TODO: Work in frame_iter if provided. */
   emscripten_cancel_main_loop();
   emscripten_set_main_loop_arg( frame_iter, data, 0, 0 );

   /* TODO: Sleep forever? */

#  else

   /* Just skip to the generic loop. */
   retval = retroflat_loop_generic( frame_iter, loop_iter, data );

#  endif /* RETROFLAT_OS_WASM */

   /* This should be set by retroflat_quit(). */
   return retval;
}

/* === */

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;
   uint32_t sdl_msg_flags = 0;

   maug_mzero( msg_out, RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   switch( (flags & RETROFLAT_MSG_FLAG_TYPE_MASK) ) {
   case RETROFLAT_MSG_FLAG_ERROR:
      sdl_msg_flags = SDL_MESSAGEBOX_ERROR;
      break;

   case RETROFLAT_MSG_FLAG_INFO:
      sdl_msg_flags = SDL_MESSAGEBOX_INFORMATION;
      break;

   case RETROFLAT_MSG_FLAG_WARNING:
      sdl_msg_flags = SDL_MESSAGEBOX_WARNING;
      break;
   }

   SDL_ShowSimpleMessageBox(
      sdl_msg_flags, title, msg_out, g_retroflat_state->platform.window );

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Build the title. */
   va_start( vargs, format );
   maug_mzero( title, RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

   SDL_SetWindowTitle( g_retroflat_state->platform.window, title );

   va_end( vargs );
}

/* === */

retroflat_ms_t retroflat_get_ms() {
   return SDL_GetTicks();
}

/* === */


uint32_t retroflat_get_rand() {
   return rand();
}

/* === */

MERROR_RETVAL retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

#  ifdef RETROFLAT_OPENGL

   if( NULL != bmp && &(g_retroflat_state->platform.screen_buffer) != bmp ) {
      debug_printf( RETRO2D_TRACE_LVL, "called retroflat_draw_lock()!" );
   }
 
#  else

   /* == SDL2 == */

   if(
      NULL == bmp
#     ifdef RETROFLAT_VDP
      && NULL == g_retroflat_state->vdp_buffer
#     endif /* RETROFLAT_VDP */
   ) {

      /* Target is the screen buffer. */
      SDL_SetRenderTarget(
         g_retroflat_state->platform.screen_buffer.renderer,
         g_retroflat_state->platform.screen_buffer.texture );

      goto cleanup;

#     ifdef RETROFLAT_VDP
   } else if( NULL == bmp && NULL != g_retroflat_state->vdp_buffer ) {
      /* Lock the VDP buffer for drawing. */
      bmp = g_retroflat_state->vdp_buffer;
#     endif /* RETROFLAT_VDP */
   }

   assert( NULL == bmp->renderer );
   assert( NULL != bmp->surface );
   bmp->renderer = SDL_CreateSoftwareRenderer( bmp->surface );

cleanup:
#endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

#  ifdef RETROFLAT_OPENGL

   if( NULL == bmp || &(g_retroflat_state->platform.screen_buffer) == bmp ) {
      /* SDL has its own OpenGL flip functions.*/
      SDL_GL_SwapWindow( g_retroflat_state->platform.window );
   } else {
      debug_printf( RETRO2D_TRACE_LVL, "called retroflat_draw_lock()!" );
   }

#  else

   /* == SDL2 == */

   if( 
      NULL == bmp
#     ifdef RETROFLAT_VDP
      && NULL == g_retroflat_state->vdp_buffer
#     endif /* RETROFLAT_VDP */
   ) {
      /* Flip the screen. */
      SDL_SetRenderTarget( g_retroflat_state->platform.screen_buffer.renderer, NULL );
      SDL_RenderCopyEx(
         g_retroflat_state->platform.screen_buffer.renderer,
         g_retroflat_state->platform.screen_buffer.texture,
         &(g_retroflat_state->platform.viewport_rect),
         NULL, 0, NULL, 0 );
      SDL_RenderPresent( g_retroflat_state->platform.screen_buffer.renderer );

      goto cleanup;

#     ifdef RETROFLAT_VDP
   } else if( NULL == bmp && NULL != g_retroflat_state->vdp_buffer ) {
      bmp = g_retroflat_state->vdp_buffer;
#     endif /* RETROFLAT_VDP */
   }

   /* It's a bitmap. */

   /* Scrap the software renderer. */
   /* SDL_RenderPresent( bmp->renderer ); */
   SDL_DestroyRenderer( bmp->renderer );
   bmp->renderer = NULL;

   /* Scrap the old texture and recreate it from the updated surface. */
   /* The renderer should be a software renderer pointing to the surface,
    * created in retroflat_lock() above.
    */
   assert( NULL != bmp->texture );
   SDL_DestroyTexture( bmp->texture );
   bmp->texture = SDL_CreateTextureFromSurface(
      g_retroflat_state->platform.screen_buffer.renderer, bmp->surface );
   maug_cleanup_if_null(
      SDL_Texture*, bmp->texture, MERROR_GUI );

cleanup:
#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   maug_path filename_path;
   MERROR_RETVAL retval = MERROR_OK;
#  if !defined( RETROFLAT_OPENGL )
   SDL_Surface* tmp_surface = NULL;
#  endif /* !RETROFLAT_OPENGL */
#  if defined( RETROFLAT_API_SDL2 )
   /* Need these to do the brute-force pixel conversion below. */
   int color_idx = 0,
      x = 0,
      y = 0;
   RETROFLAT_COLOR_DEF* color = NULL;
#  endif /* RETROFLAT_API_SDL2 */

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, RETROFLAT_BITMAP_EXT, filename_path,
      MAUG_PATH_SZ_MAX, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

   if( retroflat_bitmap_has_flags( bmp_out, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return retval;
   }

#  ifdef RETROFLAT_OPENGL

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_load_bitmap()!" );
   /*
   assert( NULL != bmp_out );
   retval = retro3d_texture_load_bitmap(
      filename_path, &(bmp_out->tex), flags );
   */

#  else

   /* == SDL2 == */

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "loading bitmap: %s", filename_path );

   bmp_out->renderer = NULL;
   
   tmp_surface = _retroflat_sdl_load_bitmap( filename_path );
   if( NULL == tmp_surface ) {
      error_printf( "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Create a screen-format bitmap to hold the final bitmap. */

   bmp_out->surface = SDL_CreateRGBSurface( 0, tmp_surface->w, tmp_surface->h,
      RETROFLAT_SDL_BPP, 0, 0, 0, 0 );
   if( NULL == bmp_out->surface ) {
      error_printf( "SDL unable to create screen bitmap: %s", SDL_GetError() );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Perform this brute-force pixel conversion to convert the 16-color
    * indexed image to depth that can be readily converted to a textre.
    */
   /* TODO: Break this out to its own helper function? */
   SDL_LockSurface( bmp_out->surface );
   for( y = 0 ; tmp_surface->h > y ; y++ ) {
      for( x = 0 ; tmp_surface->w > x ; x++ ) {
         color_idx = ((uint8_t*)(tmp_surface->pixels))[y * tmp_surface->w + x];
         color = &(g_retroflat_state->palette[color_idx]);
         ((uint32_t*)(bmp_out->surface->pixels))[y * tmp_surface->w + x] =
            SDL_MapRGB(
               bmp_out->surface->format, color->r, color->g, color->b );
      }
   }
   SDL_UnlockSurface( bmp_out->surface );

   SDL_FreeSurface( tmp_surface );

   /* Setup color-keying if not opaque. */
   if( RETROFLAT_BITMAP_FLAG_OPAQUE != (RETROFLAT_BITMAP_FLAG_OPAQUE & flags) ) {
      SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
         SDL_MapRGB( bmp_out->surface->format,
            RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );
   }

   /* Create a texture from the surface. */
   bmp_out->texture = SDL_CreateTextureFromSurface(
      g_retroflat_state->platform.screen_buffer.renderer, bmp_out->surface );
   if( NULL == bmp_out->texture ) {
      error_printf( "SDL unable to create texture: %s", SDL_GetError() );
      retval = MERROR_GUI;
      if( NULL != bmp_out->surface ) {
         SDL_FreeSurface( bmp_out->surface );
         bmp_out->surface = NULL;
      }
      goto cleanup;
   }

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "successfully loaded bitmap: %s", filename_path );

#  endif /* RETROFLAT_OPENGL */

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   if( retroflat_bitmap_has_flags( bmp_out, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return retval;
   }

#  ifdef RETROFLAT_OPENGL

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_create_bitmap()!" );
   /*
   assert( NULL != bmp_out );
   retval = retro3d_texture_create( w, h, &(bmp_out->tex), flags );
   */

#  else

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   /* == SDL2 == */

   /* Create surface. */
   bmp_out->surface = SDL_CreateRGBSurface( 0, w, h,
      /* TODO: Are these masks right? */
      RETROFLAT_SDL_BPP, 0, 0, 0, 0 );
   if( NULL == bmp_out->surface ) {
      error_printf( "could not create surface: %s", SDL_GetError() );
   }
   maug_cleanup_if_null(
      SDL_Surface*, bmp_out->surface, MERROR_GUI );
   if( RETROFLAT_BITMAP_FLAG_OPAQUE != (RETROFLAT_BITMAP_FLAG_OPAQUE & flags) ) {
      SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
         SDL_MapRGB( bmp_out->surface->format,
            RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );
   }

   /* Convert new surface to texture. */
   bmp_out->texture = SDL_CreateTextureFromSurface(
      g_retroflat_state->platform.screen_buffer.renderer, bmp_out->surface );
   if( NULL == bmp_out->texture ) {
      error_printf( "could not create texture: %s", SDL_GetError() );
   }
   maug_cleanup_if_null(
      SDL_Texture*, bmp_out->texture, MERROR_GUI );
      
cleanup:
#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

   if( retroflat_bitmap_has_flags( bmp, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

#  ifdef RETROFLAT_OPENGL

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_destroy_bitmap()!" );
   /*
   assert( NULL != bmp );
   retro3d_texture_destroy( &(bmp->tex) );
   */

#  else

   assert( NULL != bmp );
   assert( NULL != bmp->surface );

   assert( NULL != bmp->texture );

   SDL_DestroyTexture( bmp->texture );
   bmp->texture = NULL;

   SDL_FreeSurface( bmp->surface );
   bmp->surface = NULL;

#  endif /* RETROFLAT_OPENGL */

}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   retroflat_pxxy_t s_x, retroflat_pxxy_t s_y,
   retroflat_pxxy_t d_x, retroflat_pxxy_t d_y,
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;
#  ifndef RETROFLAT_OPENGL
   SDL_Rect src_rect = { s_x, s_y, w, h };
   SDL_Rect dest_rect = { d_x, d_y, w, h };
   SDL_Texture* tmp_tex = NULL;
   int is_screen = 0;
#  endif /* !RETROFLAT_OPENGL */

   assert( NULL != src );

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      retval = MERROR_GUI;
      return retval;
   }

#if 0
   /* SDL not setup for hardware scrolling. */
   if(
      0 > d_x || 0 > d_y ||
      retroflat_bitmap_w( target ) + w <= d_x ||
      retroflat_bitmap_h( target ) + h <= d_y
   ) {
      return;
   }
#endif

#  ifdef RETROFLAT_OPENGL

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_blit_bitmap()!" );
   /*
   assert( NULL != target );
   retval = retro3d_texture_blit(
      &(target->tex), &(src->tex), s_x, s_y, d_x, d_y, w, h, instance );
   */

#  else

   /* == SDL == */

   if( NULL == target || retroflat_screen_buffer() == target ) {
      is_screen = 1;
      target = retroflat_screen_buffer();

      retval = _retroview_hwscroll( &d_x, &d_y, w, h, instance );
      maug_cleanup_if_not_ok();
   }

   /* Trim sprite to stay on-screen. */
   retval = _retroview_trim_px(
      target, instance, &s_x, &s_y, &d_x, &d_y, &w, &h );
   maug_cleanup_if_not_ok();

   src_rect.x = s_x;
   src_rect.y = s_y;
   src_rect.w = w;
   src_rect.h = h;
   dest_rect.x = d_x;
   dest_rect.y = d_y;
   dest_rect.w = w;
   dest_rect.h = h;

   assert( d_x >= 0 );
   assert( d_y >= 0 );
   assert( d_x < retroflat_bitmap_w( target ) );
   assert( d_y < retroflat_bitmap_h( target ) );

   assert( retroflat_bitmap_locked( target ) );
   
   if( is_screen ) {
      tmp_tex = src->texture;

   } else {
      assert( !retroflat_bitmap_locked( src ) );
      retroflat_draw_lock( src );
      /* This little roundabout song-and-dance allows us to copy from a surface
       * updated by a software renderer (off-screen bitmap) onto another
       * software renderer (off-screen bitmap) without getting the dreaded
       * "Texture was not created with this renderer" error.
       */
      tmp_tex = SDL_CreateTexture(
         target->renderer,
         SDL_GetWindowPixelFormat( g_retroflat_state->platform.window ),
         SDL_TEXTUREACCESS_STREAMING,
         retroflat_bitmap_w( src ),
         retroflat_bitmap_h( src ) );
      if( NULL == tmp_tex ) {
         error_printf(
            "could not create temporary texture: %s", SDL_GetError() );
         retval = MERROR_GUI;
      }
      SDL_UpdateTexture(
         tmp_tex, NULL, src->surface->pixels, src->surface->pitch );
      retroflat_draw_release( src );
   }

   retval = SDL_RenderCopy(
      target->renderer, tmp_tex, &src_rect, &dest_rect );
   if( 0 != retval ) {
      error_printf( "could not blit surface: %s", SDL_GetError() );
      retval = MERROR_GUI;
   }

cleanup:

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x, retroflat_pxxy_t y, uint8_t flags
) {
#  ifndef RETROFLAT_OPENGL
   RETROFLAT_COLOR_DEF* color = &(g_retroflat_state->palette[color_idx]);
#  endif /* !RETROFLAT_OPENGL */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

   retroflat_viewport_constrain_px( x, y, target, return );

#  if defined( RETROFLAT_OPENGL )

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_px()!" );
   /*
   assert( NULL != target );
   retro3d_texture_px( &(target->tex), color_idx, x, y, flags );
   */

#  else

   /* == SDL2 == */

   assert( retroflat_bitmap_locked( target ) );

   SDL_SetRenderDrawColor(
      target->renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderDrawPoint( target->renderer, x, y );

#  endif /* RETROFLAT_OPENGL */
}

/* === */

#  ifndef RETROFLAT_SOFT_SHAPES

void retroflat_rect(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
   float aspect_ratio = 0,
      screen_x = 0,
      screen_y = 0,
      screen_w = 0,
      screen_h = 0;
#  endif /* RETROFLAT_OPENGL */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

#  ifdef RETROFLAT_OPENGL

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_rect()!" );

#  else

   if(
      MERROR_OK != _retroview_trim_px( target, 0, NULL, NULL, &x, &y, &w, &h )
   ) {
      return;
   }

   assert( retroflat_bitmap_locked( target ) );

   area.x = x;
   area.y = y;
   area.w = w;
   area.h = h;

   SDL_SetRenderDrawColor(
      target->renderer, color->r, color->g, color->b, 255 );

   if( RETROFLAT_DRAW_FLAG_FILL == (RETROFLAT_DRAW_FLAG_FILL & flags) ) {
      SDL_RenderFillRect( target->renderer, &area );
   } else {
      SDL_RenderDrawRect( target->renderer, &area );
   }

#  endif /* RETROFLAT_OPENGL */
}

#  endif /* !RETROFLAT_SOFT_SHAPES */

/* === */

#  ifndef RETROFLAT_SOFT_LINES

void retroflat_line(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x1, retroflat_pxxy_t y1,
   retroflat_pxxy_t x2, retroflat_pxxy_t y2, uint8_t flags
) {
#  ifndef RETROFLAT_OPENGL
   MERROR_RETVAL retval = MERROR_OK;
   RETROFLAT_COLOR_DEF color = g_retroflat_state->palette[color_idx];
#  endif /* !RETROFLAT_OPENGL */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

#  ifdef RETROFLAT_OPENGL

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_line()!" );
#if 0
   if( NULL == target || retroflat_screen_buffer() == target ) {
      /* TODO: Draw line in ortho. */
   } else {
      retrosoft_line( target, color_idx, x1, y1, x2, y2, flags );
   }
#endif

#  else

   /* == SDL2 == */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

   assert( retroflat_bitmap_locked( target ) );

   SDL_SetRenderDrawColor(
      target->renderer, color->r, color->g, color->b, 255 );
   SDL_RenderDrawLine( target->renderer, x1, y1, x2, y2 );
 
#  endif /* RETROFLAT_OPENGL */
}

#  endif /* !RETROFLAT_SOFT_LINES */

/* === */

#  ifndef RETROFLAT_SOFT_SHAPES

void retroflat_ellipse(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h, uint8_t flags
) {
   if( RETROFLAT_COLOR_NULL == color ) {
      return;
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

   retrosoft_ellipse( target, color, x, y, w, h, flags );
}

#  endif /* !RETROFLAT_SOFT_SHAPES */

/* === */

void retroflat_get_palette( uint8_t idx, uint32_t* p_rgb ) {

#  ifdef RETROFLAT_BMP_TEX

   *p_rgb = 0;
   *p_rgb |= g_retroflat_state->tex_palette[idx][0] & 0xff;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][1] & 0xff) << 8;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][2] & 0xff) << 16;

#  else
#     pragma message( "warning: get palette not implemented" )
#  endif

}

/* === */

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 3,
      "setting texture palette #%u to " X32_FMT "...",
      idx, rgb );

#  ifdef RETROFLAT_BMP_TEX

   g_retroflat_state->tex_palette[idx][0] = rgb & 0xff;
   g_retroflat_state->tex_palette[idx][1] = (rgb & 0xff00) >> 8;
   g_retroflat_state->tex_palette[idx][2] = (rgb & 0xff0000) >> 16;

#  else
#     pragma message( "warning: set palette not implemented" )
#  endif

   return retval;
}

/* === */

void retroflat_resize_v() {

   /*
   g_retroflat_state->screen_v_w = g_retroflat_state->screen_w;
   g_retroflat_state->screen_v_h = g_retroflat_state->screen_h;
   */

   assert( NULL != g_retroflat_state->platform.screen_buffer.texture );
   SDL_DestroyTexture( g_retroflat_state->platform.screen_buffer.texture );

   /* Create the buffer texture. */
   g_retroflat_state->platform.screen_buffer.texture =
      SDL_CreateTexture( g_retroflat_state->platform.screen_buffer.renderer,
         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
         g_retroflat_state->screen_w, g_retroflat_state->screen_h );

}

/* === */

uint8_t retroflat_focus_platform() {
   uint32_t app_state = 0;
   uint8_t app_state_out = 0;

   app_state = SDL_GetWindowFlags( g_retroflat_state->platform.window );

   if( SDL_WINDOW_INPUT_FOCUS == (SDL_WINDOW_INPUT_FOCUS & app_state) ) {
      app_state_out |= RETROFLAT_FOCUS_FLAG_ACTIVE;
   }

   if( SDL_WINDOW_SHOWN == (SDL_WINDOW_SHOWN & app_state) ) {
      app_state_out |= RETROFLAT_FOCUS_FLAG_VISIBLE;
   }

   if( app_state_out != g_retroflat_state->last_focus_flags ) {
      debug_printf( 1, "focus state: %02x", app_state_out );
   }

   return app_state_out;
}

/* === */

uint8_t retroview_move_x( retroflat_pxxy_t x ) {
   uint8_t move; /* Really a boolean. */

   _retroview_move_xy( x, move, x, w, RETROFLAT_TILE_W );
   if( !move ) {
      goto cleanup;
   }

   /* Add the scroll amounts to the platform-specific viewport so the blits
    * above know where to grab from on the full hardware buffer.
    */
   g_retroflat_state->platform.viewport_rect.x += x;
   if(
      0 >= g_retroflat_state->platform.viewport_rect.x ||
      RETROFLAT_TILE_W * 2 <= g_retroflat_state->platform.viewport_rect.x
   ) {
      /* Move the viewport back to the center of the real screen buffer. */
      g_retroflat_state->platform.viewport_rect.x = RETROFLAT_TILE_W;
   }

cleanup:

   return move;
}

/* === */

uint8_t retroview_move_y( retroflat_pxxy_t y ) {
   uint8_t move; /* Really a boolean. */

   _retroview_move_xy( y, move, y, h, RETROFLAT_TILE_H );
   if( !move ) {
      goto cleanup;
   }

   g_retroflat_state->platform.viewport_rect.y += y;
   if(
      0 >= g_retroflat_state->platform.viewport_rect.y ||
      RETROFLAT_TILE_H * 2 <= g_retroflat_state->platform.viewport_rect.y
   ) {
      /* Move the viewport back to the center of the real screen buffer. */
      g_retroflat_state->platform.viewport_rect.y = RETROFLAT_TILE_H;
   }

cleanup:

   return move;
}

#endif /* !RETPLTF_H */

