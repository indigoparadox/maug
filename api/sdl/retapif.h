
#ifndef RETPLTF_H
#define RETPLTF_H

MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_SDL_ICO )
   SDL_Surface* icon = NULL;
#  endif /* RETROFLAT_SDL_ICO */
#  if defined( RETROFLAT_API_SDL1 )
   const SDL_VideoInfo* info = NULL;
   char sdl_video_parms[256] = "";
#     if defined( RETROFLAT_OPENGL )
   int gl_retval = 0,
      gl_depth = 16;
#     endif /* RETROFLAT_OPENGL */
#  endif /* RETROFLAT_API_SDL1 */

#  if defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   /* Random seed. */
   srand( time( NULL ) );

   /* Startup SDL. */
   if( SDL_Init( SDL_INIT_VIDEO ) ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Error initializing SDL: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   /* Get info on best available video mode. */
   info = SDL_GetVideoInfo();
   maug_cleanup_if_null_alloc( SDL_VideoInfo*, info );

   debug_printf( 3, "maximum window size: %ux%u",
      info->current_w, info->current_h );

#     ifndef RETROFLAT_OS_WIN

   /* TODO: Maximum screen size detection returns bogus values in Windows! */

   /* Setup default screen position. */
   if( 0 == args->screen_x ) {
      /* Get screen width so we can center! */
      args->screen_x = (info->current_w / 2) -
         (g_retroflat_state->screen_w / 2);
   }

   if( 0 == args->screen_y ) {
      /* Get screen height so we can center! */
      args->screen_y = (info->current_h / 2) -
         (g_retroflat_state->screen_h / 2);
   }

   maug_snprintf( sdl_video_parms, 255, "SDL_VIDEO_WINDOW_POS=%d,%d",
       args->screen_x, args->screen_y );
   putenv( sdl_video_parms );

#     endif /* !RETROFLAT_OS_WIN */

   /* Setup color palettes. */
#     ifdef RETROFLAT_OPENGL
#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
            g_retroflat_state->palette[idx] = RETROGLU_COLOR_ ## name_u;
#     else
#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
            g_retroflat_state->palette[idx].r = rd; \
            g_retroflat_state->palette[idx].g = gd; \
            g_retroflat_state->palette[idx].b = bd;
#     endif /* RETROFLAT_OPENGL */
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL )

#     ifdef RETROFLAT_OPENGL
   
#        ifndef RETROFLAT_OS_WASM
   /* (This doesn't compile in emcc.) */
   if(
      RETROFLAT_FLAGS_UNLOCK_FPS == (RETROFLAT_FLAGS_UNLOCK_FPS & args->flags)
   ) {
      SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );
   }
#        endif /* !RETROFLAT_OS_WASM */

   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
   do {
      /* Retry with smaller depth buffers if this fails. */
      gl_retval = SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, gl_depth );
      if( gl_retval ) {
         error_printf( "unable to set depth buffer to %d!", gl_depth );
         gl_depth -= 4;
      }
   } while( gl_retval );
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
#     endif /* RETROFLAT_OPENGL */

   if( NULL != args->title ) {
      retroflat_set_title( args->title );
   }

#     ifdef RETROFLAT_SDL_ICO
   debug_printf( 1, "setting SDL window icon..." );
   icon = SDL_LoadBMP_RW(
      SDL_RWFromConstMem( obj_ico_sdl_ico_bmp,
      obj_ico_sdl_ico_bmp_len ), 1 );
   maug_cleanup_if_null( SDL_Surface*, icon, RETROFLAT_ERROR_BITMAP );
   SDL_WM_SetIcon( icon, 0 ); /* TODO: Constant mask. */
#     endif /* RETROFLAT_SDL_ICO */

   g_retroflat_state->buffer.surface = SDL_SetVideoMode(
      g_retroflat_state->screen_w, g_retroflat_state->screen_h,
      info->vfmt->BitsPerPixel,
      SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_ANYFORMAT
#     ifdef RETROFLAT_OPENGL
      | SDL_OPENGL
#     endif /* RETROFLAT_OPENGL */
   );
   maug_cleanup_if_null(
      SDL_Surface*, g_retroflat_state->buffer.surface,
      RETROFLAT_ERROR_GRAPHICS );

   /* Setup key repeat. */
   if(
      RETROFLAT_FLAGS_KEY_REPEAT == (RETROFLAT_FLAGS_KEY_REPEAT & args->flags)
   ) {
      if( 0 != SDL_EnableKeyRepeat(
         1, SDL_DEFAULT_REPEAT_INTERVAL
      ) ) {
         error_printf( "could not enable key repeat!" );
      } else {
         debug_printf( 3, "key repeat enabled" );
      }
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   srand( time( NULL ) );

   if( SDL_Init( SDL_INIT_VIDEO ) ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Error initializing SDL: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_ENGINE;
      goto cleanup;
   }

   /* Setup color palettes. */
#     ifdef RETROFLAT_OPENGL
#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, r, g, b, cgac, cgad ) \
            g_retroflat_state->palette[idx] = RETROGLU_COLOR_ ## name_u;
#     else
#        define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
            g_retroflat_state->palette[idx].r = rd; \
            g_retroflat_state->palette[idx].g = gd; \
            g_retroflat_state->palette[idx].b = bd;
#     endif /* RETROFLAT_OPENGL */
   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL )

   /* Create the main window. */
   g_retroflat_state->platform.window = SDL_CreateWindow( args->title,
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      args->screen_w, args->screen_h, RETROFLAT_WIN_FLAGS );
   maug_cleanup_if_null(
      SDL_Window*, g_retroflat_state->platform.window,
      RETROFLAT_ERROR_GRAPHICS );

   /* Create the main renderer. */
   g_retroflat_state->buffer.renderer = SDL_CreateRenderer(
      g_retroflat_state->platform.window, -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE );
   maug_cleanup_if_null(
      SDL_Renderer*, g_retroflat_state->buffer.renderer,
      RETROFLAT_ERROR_GRAPHICS );

   /* Create the buffer texture. */
   g_retroflat_state->buffer.texture =
      SDL_CreateTexture( g_retroflat_state->buffer.renderer,
      SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      g_retroflat_state->screen_w, g_retroflat_state->screen_h );

   /* TODO: This doesn't seem to do anything. */
   if(
      RETROFLAT_FLAGS_SCALE2X == (RETROFLAT_FLAGS_SCALE2X & args->flags)
   ) {
      debug_printf( 1, "setting SDL window scale to 2x..." );
      SDL_RenderSetScale( g_retroflat_state->buffer.renderer, 2.0f, 2.0f );
   }

#     ifdef RETROFLAT_SDL_ICO
   debug_printf( 1, "setting SDL window icon..." );
   icon = SDL_LoadBMP_RW(
      SDL_RWFromConstMem( obj_ico_sdl_ico_bmp,
      obj_ico_sdl_ico_bmp_len ), 1 );
   maug_cleanup_if_null( SDL_Surface*, icon, RETROFLAT_ERROR_BITMAP );
   SDL_SetWindowIcon( g_retroflat_state->platform.window, icon );
#     endif /* RETROFLAT_SDL_ICO */

#  endif /* RETROFLAT_API_SDL1 || RETROFLAT_API_SDL2 */

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {

#     ifndef RETROFLAT_API_SDL1
   SDL_DestroyWindow( g_retroflat_state->platform.window );
#     endif /* !RETROFLAT_API_SDL1 */

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
#  ifdef RETROFLAT_API_SDL2
   uint32_t sdl_msg_flags = 0;
#  elif (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_WIN ))
   uint32_t win_msg_flags = 0;
#  endif

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

#  if defined( RETROFLAT_API_SDL2 )
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

#  elif (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_WIN ))
   switch( (flags & RETROFLAT_MSG_FLAG_TYPE_MASK) ) {
   case RETROFLAT_MSG_FLAG_ERROR:
      win_msg_flags |= MB_ICONSTOP;
      break;

   case RETROFLAT_MSG_FLAG_INFO:
      win_msg_flags |= MB_ICONINFORMATION;
      break;

   case RETROFLAT_MSG_FLAG_WARNING:
      win_msg_flags |= MB_ICONEXCLAMATION;
      break;
   }

   MessageBox( retroflat_root_win(), msg_out, title, win_msg_flags );

#  elif (defined( RETROFLAT_API_SDL1 ) && defined( RETROFLAT_OS_UNIX ))

   /* TODO */
   error_printf( "%s", msg_out );

#  else
#     pragma message( "warning: not implemented" )
#  endif /* RETROFLAT_API_ALLEGRO || RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   va_end( vargs );
}

/* === */

void retroflat_set_title( const char* format, ... ) {
   char title[RETROFLAT_TITLE_MAX + 1];
   va_list vargs;

   /* Build the title. */
   va_start( vargs, format );
   memset( title, '\0', RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

#if defined( RETROFLAT_API_SDL1 )
   SDL_WM_SetCaption( title, NULL );
#elif defined( RETROFLAT_API_SDL2 )
   SDL_SetWindowTitle( g_retroflat_state->platform.window, title );
#  endif /* RETROFLAT_API_SDL */

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

int retroflat_draw_lock( struct RETROFLAT_BITMAP* bmp ) {
   int retval = RETROFLAT_OK;

#  if defined( RETROFLAT_OPENGL )

   retval = retroglu_draw_lock( bmp );

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   /* SDL locking semantics are the opposite of every other platform. See
    * retroflat_px_lock() for a proxy to SDL_LockSurface().
    */

   if( NULL == bmp || &(g_retroflat_state->buffer) == bmp ) {
      /* Special case: Attempting to lock the screen. */
      bmp = &(g_retroflat_state->buffer);

      if(
         RETROFLAT_FLAGS_SCREEN_LOCK !=
         (RETROFLAT_FLAGS_SCREEN_LOCK & bmp->flags)
      ) {
         /* Do a perfunctory "screen lock" since programs are supposed to
          * lock the screen before doing any drawing.
          */
         bmp->flags |= RETROFLAT_FLAGS_SCREEN_LOCK;

      } else {
         /* We actually want to lock the buffer for pixel manipulation. */
         assert( 0 == (RETROFLAT_FLAGS_LOCK & bmp->flags) );
         bmp->flags |= RETROFLAT_FLAGS_LOCK;
      }

   } else {
      /* Locking a bitmap for pixel drawing. */
      assert( 0 == (RETROFLAT_FLAGS_LOCK & bmp->flags) );
      bmp->flags |= RETROFLAT_FLAGS_LOCK;
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if(
      NULL == bmp
#     ifdef RETROFLAT_VDP
      && NULL == g_retroflat_state->vdp_buffer
#     endif /* RETROFLAT_VDP */
   ) {

      /* Target is the screen buffer. */
      SDL_SetRenderTarget(
         g_retroflat_state->buffer.renderer,
         g_retroflat_state->buffer.texture );

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
#endif /* RETROFLAT_API */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

#  ifdef RETROFLAT_OPENGL

   retval = retroglu_draw_release( bmp );

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   if( NULL == bmp || &(g_retroflat_state->buffer) == bmp ) {
      /* Special case: Attempting to release the (real, non-VDP) screen. */
      bmp = &(g_retroflat_state->buffer);

      if(
         RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & bmp->flags)
      ) {
         /* The screen was locked for pixel manipulation. */
         bmp->flags &= ~RETROFLAT_FLAGS_LOCK;
         SDL_UnlockSurface( bmp->surface );

      } else {
         assert( 
            RETROFLAT_FLAGS_SCREEN_LOCK ==
            (RETROFLAT_FLAGS_SCREEN_LOCK & bmp->flags) );
         bmp->flags &= ~RETROFLAT_FLAGS_SCREEN_LOCK;

#     if defined( RETROFLAT_VDP )
         retroflat_vdp_call( "retroflat_vdp_flip" );
#     endif /* RETROFLAT_VDP */

         SDL_Flip( bmp->surface );
      }

   } else {
      /* Releasing a bitmap. */
      assert( RETROFLAT_FLAGS_LOCK == (RETROFLAT_FLAGS_LOCK & bmp->flags) );
      bmp->flags &= ~RETROFLAT_FLAGS_LOCK;
      SDL_UnlockSurface( bmp->surface );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if( 
      NULL == bmp
#     ifdef RETROFLAT_VDP
      && NULL == g_retroflat_state->vdp_buffer
#     endif /* RETROFLAT_VDP */
   ) {
      /* Flip the screen. */
      SDL_SetRenderTarget( g_retroflat_state->buffer.renderer, NULL );
      SDL_RenderCopyEx(
         g_retroflat_state->buffer.renderer,
         g_retroflat_state->buffer.texture, NULL, NULL, 0, NULL, 0 );
      SDL_RenderPresent( g_retroflat_state->buffer.renderer );

      goto cleanup;

#     ifdef RETROFLAT_VDP
   } else if( NULL == bmp && NULL != g_retroflat_state->vdp_buffer ) {
      bmp = g_retroflat_state->vdp_buffer;
#     endif /* RETROFLAT_VDP */
   }

   /* It's a bitmap. */

   /* Scrap the software renderer. */
   SDL_RenderPresent( bmp->renderer );
   SDL_DestroyRenderer( bmp->renderer );
   bmp->renderer = NULL;

   /* Scrap the old texture and recreate it from the updated surface. */
   /* The renderer should be a software renderer pointing to the surface,
      * created in retroflat_lock() above.
      */
   assert( NULL != bmp->texture );
   SDL_DestroyTexture( bmp->texture );
   bmp->texture = SDL_CreateTextureFromSurface(
      g_retroflat_state->buffer.renderer, bmp->surface );
   maug_cleanup_if_null(
      SDL_Texture*, bmp->texture, RETROFLAT_ERROR_BITMAP );

cleanup:
#  endif /* RETROFLAT_API_ALLEGRO */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   char filename_path[RETROFLAT_PATH_MAX + 1];
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_API_SDL1 ) && !defined( RETROFLAT_OPENGL )
   SDL_Surface* tmp_surface = NULL;
#  endif

   assert( NULL != bmp_out );
   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );
   retval = retroflat_build_filename_path(
      filename, filename_path, RETROFLAT_PATH_MAX + 1, flags );
   maug_cleanup_if_not_ok();
   debug_printf( 1, "retroflat: loading bitmap: %s", filename_path );

#  ifdef RETROFLAT_OPENGL

   retval = retroglu_load_bitmap( filename_path, bmp_out, flags );

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "loading bitmap: %s", filename_path );

   tmp_surface = SDL_LoadBMP( filename_path ); /* Free stream on close. */
   if( NULL == tmp_surface ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = 0;
      goto cleanup;
   }

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "loaded bitmap: %d x %d", tmp_surface->w, tmp_surface->h );

   bmp_out->surface = SDL_DisplayFormat( tmp_surface );
   if( NULL == bmp_out->surface ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL, "converted bitmap: %d x %d",
      bmp_out->surface->w, bmp_out->surface->h );

   SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
      SDL_MapRGB( bmp_out->surface->format,
         RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );

   if( NULL != tmp_surface ) {
      SDL_FreeSurface( tmp_surface );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "loading bitmap: %s", filename_path );

   bmp_out->renderer = NULL;
   
   bmp_out->surface = SDL_LoadBMP( filename_path );
   if( NULL == bmp_out->surface ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      goto cleanup;
   }

   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
         SDL_MapRGB( bmp_out->surface->format,
            RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );
   }

   bmp_out->texture = SDL_CreateTextureFromSurface(
      g_retroflat_state->buffer.renderer, bmp_out->surface );
   if( NULL == bmp_out->texture ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "SDL unable to create texture: %s", SDL_GetError() );
      retval = RETROFLAT_ERROR_BITMAP;
      if( NULL != bmp_out->surface ) {
         SDL_FreeSurface( bmp_out->surface );
         bmp_out->surface = NULL;
      }
      goto cleanup;
   }

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "successfully loaded bitmap: %s", filename_path );

#  endif

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

#  if defined( RETROFLAT_OPENGL )

   retval = retroglu_create_bitmap( w, h, bmp_out, flags );

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   bmp_out->surface = SDL_CreateRGBSurface( 0, w, h,
      32, 0, 0, 0, 0 );
   maug_cleanup_if_null(
      SDL_Surface*, bmp_out->surface, RETROFLAT_ERROR_BITMAP );
   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
         SDL_MapRGB( bmp_out->surface->format,
            RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );
   }

cleanup:
#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   /* Create surface. */
   bmp_out->surface = SDL_CreateRGBSurface( 0, w, h,
      /* TODO: Are these masks right? */
      32, 0, 0, 0, 0 );
   maug_cleanup_if_null(
      SDL_Surface*, bmp_out->surface, RETROFLAT_ERROR_BITMAP );
   if( RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) ) {
      SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
         SDL_MapRGB( bmp_out->surface->format,
            RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );
   }

   /* Convert new surface to texture. */
   bmp_out->texture = SDL_CreateTextureFromSurface(
      g_retroflat_state->buffer.renderer, bmp_out->surface );
   maug_cleanup_if_null(
      SDL_Texture*, bmp_out->texture, RETROFLAT_ERROR_BITMAP );
      
cleanup:
#  endif

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {

#  if defined( RETROFLAT_OPENGL )

   retroglu_destroy_bitmap( bmp );

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   assert( NULL != bmp );
   assert( NULL != bmp->surface );

#     ifndef RETROFLAT_API_SDL1
   assert( NULL != bmp->texture );

   SDL_DestroyTexture( bmp->texture );
   bmp->texture = NULL;
#     endif /* !RETROFLAT_API_SDL1 */

   SDL_FreeSurface( bmp->surface );
   bmp->surface = NULL;

#  endif

}

/* === */

MERROR_RETVAL retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, int16_t d_x, int16_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_API_SDL1 ) && !defined( RETROFLAT_OPENGL )
   SDL_Rect src_rect;
   SDL_Rect dest_rect;
#  elif defined( RETROFLAT_API_SDL2 ) && !defined( RETROFLAT_OPENGL )
   SDL_Rect src_rect = { s_x, s_y, w, h };
   SDL_Rect dest_rect = { d_x, d_y, w, h };
#  endif /* RETROFLAT_API_SDL2 || RETROFLAT_API_SDL1 */

   assert( NULL != src );

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

#  if defined( RETROFLAT_OPENGL )

   retval = 
      retroglu_blit_bitmap( target, src, s_x, s_y, d_x, d_y, w, h, instance );

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   src_rect.x = s_x;
   src_rect.y = s_y;
   src_rect.w = w;
   src_rect.h = h;
   dest_rect.x = d_x;
   dest_rect.y = d_y;
   dest_rect.w = w;
   dest_rect.h = h;

#     ifdef RETROFLAT_API_SDL1
   assert( 0 == src->autolock_refs );
   assert( 0 == target->autolock_refs );
   retval = 
      SDL_BlitSurface( src->surface, &src_rect, target->surface, &dest_rect );
   if( 0 != retval ) {
      error_printf( "could not blit surface: %s", SDL_GetError() );
      retval = MERROR_GUI;
   }
#     else

   assert( retroflat_bitmap_locked( target ) );
   retval = SDL_RenderCopy(
      target->renderer, src->texture, &src_rect, &dest_rect );
   if( 0 != retval ) {
      error_printf( "could not blit surface: %s", SDL_GetError() );
      retval = MERROR_GUI;
   }

#     endif /* !RETROFLAT_API_SDL1 */

#  endif

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
#  elif defined( RETROFLAT_API_SDL1 )
   int offset = 0;
   uint8_t* px_1 = NULL;
   uint16_t* px_2 = NULL;
   uint32_t* px_4 = NULL;
   RETROFLAT_COLOR_DEF* color = &(g_retroflat_state->palette[color_idx]);
#  elif defined( RETROFLAT_API_SDL2 )
   RETROFLAT_COLOR_DEF* color = &(g_retroflat_state->palette[color_idx]);
#  endif /* RETROFLAT_API_SDL1 */

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

   retroflat_constrain_px( x, y, target, return );

#  if defined( RETROFLAT_OPENGL )

   retroglu_px( target, color_idx, x, y, flags );

#  elif defined( RETROFLAT_API_SDL1 )

   /* == SDL1 == */

   retroflat_px_lock( target );

   assert( 0 < target->autolock_refs );

   offset = (y * target->surface->pitch) +
      (x * target->surface->format->BytesPerPixel);

   switch( target->surface->format->BytesPerPixel ) {
   case 4:
      px_4 = (uint32_t*)&(((uint8_t*)(target->surface->pixels))[offset]);
      *px_4 =
         SDL_MapRGB( target->surface->format, color->r, color->g, color->b );
      break;

   case 2:
      px_2 = (uint16_t*)&(((uint8_t*)(target->surface->pixels))[offset]);
      *px_2 =
         SDL_MapRGB( target->surface->format, color->r, color->g, color->b );
      break;

   case 1:
      px_1 = (uint8_t*)&(((uint8_t*)(target->surface->pixels))[offset]);
      *px_1 =
         SDL_MapRGB( target->surface->format, color->r, color->g, color->b );
      break;
   }

   retroflat_px_release( target );

#  elif defined( RETROFLAT_API_SDL2 )

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
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {
#  if defined( RETROFLAT_OPENGL )
   float aspect_ratio = 0,
      screen_x = 0,
      screen_y = 0,
      screen_w = 0,
      screen_h = 0;
#  endif /* RETROFLAT_API_WIN16 || RETROFLAT_API_WIN32 */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

#  if defined( RETROFLAT_OPENGL )

   assert( NULL != target );

   /* Draw the rect onto the given 2D texture. */
   retrosoft_rect( target, color_idx, x, y, w, h, flags );

#  else

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
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

   if( RETROFLAT_FLAGS_FILL == (RETROFLAT_FLAGS_FILL & flags) ) {
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
   int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t flags
) {
#  if !defined( RETROFLAT_OPENGL )
   MERROR_RETVAL retval = MERROR_OK;
   RETROFLAT_COLOR_DEF color = g_retroflat_state->palette[color_idx];
#  endif /* !RETROFLAT_OPENGL */

   if( RETROFLAT_COLOR_NULL == color_idx ) {
      return;
   }

#  if defined( RETROFLAT_OPENGL )

   if( NULL == target || retroflat_screen_buffer() == target ) {
      /* TODO: Draw line in ortho. */
   } else {
      retrosoft_line( target, color_idx, x1, y1, x2, y2, flags );
   }

#  elif defined( RETROFLAT_API_SDL2 )

   /* == SDL2 == */

   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }

   if(
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
   ) {
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
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t flags
) {
   if( RETROFLAT_COLOR_NULL == color ) {
      return;
   }

   if(
      NULL != target &&
      RETROFLAT_FLAGS_BITMAP_RO == (RETROFLAT_FLAGS_BITMAP_RO & target->flags)
   ) {
      return;
   }

   retrosoft_ellipse( target, color, x, y, w, h, flags );
}

#  endif /* !RETROFLAT_SOFT_SHAPES */

/* === */

void retroflat_get_palette( uint8_t idx, uint32_t* p_rgb ) {

#  ifdef RETROFLAT_OPENGL

   *p_rgb = 0;
   *p_rgb |= g_retroflat_state->tex_palette[idx][0] & 0xff;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][1] & 0xff) << 8;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][2] & 0xff) << 16;

#  elif defined( RETROFLAT_API_SDL2 )

   *p_rgb = 0;
   *p_rgb |= g_retroflat_state->palette[idx].b & 0xff;
   *p_rgb |= ((g_retroflat_state->palette[idx].g & 0xff) << 8);
   *p_rgb |= ((g_retroflat_state->palette[idx].r & 0xff) << 16);

#  else
#     pragma message( "warning: get palette not implemented" )
#  endif

}

/* === */

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 3,
      "setting texture palette #%u to " UPRINTF_X32_FMT "...",
      idx, rgb );

#  ifdef RETROFLAT_OPENGL

   g_retroflat_state->tex_palette[idx][0] = rgb & 0xff;
   g_retroflat_state->tex_palette[idx][1] = (rgb & 0xff00) >> 8;
   g_retroflat_state->tex_palette[idx][2] = (rgb & 0xff0000) >> 16;

#  elif defined( RETROFLAT_API_SDL2 )

   g_retroflat_state->palette[idx].b = rgb & 0xff;
   g_retroflat_state->palette[idx].g = (rgb & 0xff00) >> 8;
   g_retroflat_state->palette[idx].r = (rgb & 0xff0000) >> 16;

#  else
#     pragma message( "warning: set palette not implemented" )
#  endif

   return retval;
}

/* === */

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   int eres = 0;
   SDL_Event event;
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   SDL_PollEvent( &event );

   switch( event.type ) {
   case SDL_QUIT:
      /* Handle SDL window close. */
      retroflat_quit( 0 );
      break;

   case SDL_KEYDOWN:
      key_out = event.key.keysym.sym;

      if(
         KMOD_RSHIFT == (KMOD_RSHIFT & event.key.keysym.mod) ||
         KMOD_LSHIFT == (KMOD_LSHIFT & event.key.keysym.mod) 
      ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_SHIFT;
      }

      if( KMOD_CTRL == (KMOD_CTRL & event.key.keysym.mod) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_CTRL;
      }

      if( KMOD_ALT == (KMOD_ALT & event.key.keysym.mod) ) {
         input->key_flags |= RETROFLAT_INPUT_MOD_ALT;
      }

      /* Flush key buffer to improve responsiveness. */
      if(
         RETROFLAT_FLAGS_KEY_REPEAT !=
         (RETROFLAT_FLAGS_KEY_REPEAT & g_retroflat_state->retroflat_flags)
      ) {
         while( (eres = SDL_PollEvent( &event )) );
      }
      break;

   case SDL_MOUSEBUTTONUP:
      /* Stop dragging. */
      g_retroflat_state->platform.mouse_state = 0;
      break;

   case SDL_MOUSEBUTTONDOWN:

      /* Begin dragging. */

      input->mouse_x = event.button.x;  
      input->mouse_y = event.button.y;  

      /* Differentiate which button was clicked. */
      if( SDL_BUTTON_LEFT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_LEFT;
         g_retroflat_state->platform.mouse_state = RETROFLAT_MOUSE_B_LEFT;
      } else if( SDL_BUTTON_RIGHT == event.button.button ) {
         key_out = RETROFLAT_MOUSE_B_RIGHT;
         g_retroflat_state->platform.mouse_state = RETROFLAT_MOUSE_B_RIGHT;
      }

      /* Flush key buffer to improve responsiveness. */
      /*while( (eres = SDL_PollEvent( &event )) );*/
      break;

#  if !defined( RETROFLAT_API_SDL1 )
   case SDL_WINDOWEVENT:
      switch( event.window.event ) {
      case SDL_WINDOWEVENT_RESIZED:
         retroflat_on_resize( event.window.data1, event.window.data2 );
         if( NULL != g_retroflat_state->on_resize ) {
            g_retroflat_state->on_resize(
               event.window.data1, event.window.data2,
               g_retroflat_state->on_resize_data );
         }
         break;
      }
      break;
#  endif /* !RETROFLAT_API_SDL1 */

   default:
      /* Check for mouse dragging if mouse was previously held down. */
      if( 0 != g_retroflat_state->platform.mouse_state ) {
         /* Update coordinates and keep dragging. */
         SDL_GetMouseState( &(input->mouse_x), &(input->mouse_y) );
         key_out = g_retroflat_state->platform.mouse_state;
      }
      break;
   }

   return key_out;
}

/* === */

void retroflat_resize_v() {
#  if defined( RETROFLAT_API_SDL2 )

   g_retroflat_state->screen_v_w = g_retroflat_state->screen_w;
   g_retroflat_state->screen_v_h = g_retroflat_state->screen_h;

   assert( NULL != g_retroflat_state->buffer.texture );
   SDL_DestroyTexture( g_retroflat_state->buffer.texture );

   /* Create the buffer texture. */
   g_retroflat_state->buffer.texture =
      SDL_CreateTexture( g_retroflat_state->buffer.renderer,
      SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      g_retroflat_state->screen_w, g_retroflat_state->screen_h );

#  endif /* RETROFLAT_API_SDL2 */
}

#endif /* !RETPLTF_H */

