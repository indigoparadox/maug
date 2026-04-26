
#ifndef RETPLTF_H
#define RETPLTF_H

#ifndef RETROFLAT_OPENGL

struct MDATA_VECTOR g_retroflat_sdl_bitmap_list;

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

/* === */

#endif /* !RETROFLAT_OPENGL */

MERROR_RETVAL retroflat_init_platform(
   int argc, char* argv[], struct RETROFLAT_ARGS* args
) {
   MERROR_RETVAL retval = MERROR_OK;
#if defined( RETROFLAT_SDL_ICO )
   SDL_Surface* icon = NULL;
#endif /* RETROFLAT_SDL_ICO */
   const SDL_VideoInfo* info = NULL;
   char sdl_video_parms[256] = "";
#  if defined( RETROFLAT_OPENGL )
   int gl_retval = 0,
      gl_depth = 16;
#  endif /* RETROFLAT_OPENGL */

#ifndef RETROFLAT_OPENGL
   maug_mzero( &g_retroflat_sdl_bitmap_list, sizeof( struct MDATA_VECTOR ) );
#endif /* !RETROFLAT_OPENGL */

   /* TODO: Add some flexibility for simulating lower-color platforms. */
   g_retroflat_state->screen_colors = 16;

   /* == Platform-Specific Init == */

   /* Random seed. */
   srand( time( NULL ) );

   /* Startup SDL. */
   if( SDL_Init(
      SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER
#ifndef RETROFLAT_NO_SOUND
      | SDL_INIT_AUDIO
#endif /* RETROFLAT_NO_SOUND */
   ) ) {
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

#ifndef RETROFLAT_OS_WIN

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

#endif /* !RETROFLAT_OS_WIN */

   /* Setup color palettes. */
#ifndef RETROFLAT_OPENGL

#  define RETROFLAT_COLOR_TABLE_SDL( idx, name_l, name_u, rd, gd, bd, cgac, cgad ) \
      g_retroflat_state->palette[idx].r = rd; \
      g_retroflat_state->palette[idx].g = gd; \
      g_retroflat_state->palette[idx].b = bd;

   RETROFLAT_COLOR_TABLE( RETROFLAT_COLOR_TABLE_SDL )

#endif /* !RETROFLAT_OPENGL */

#ifdef RETROFLAT_OPENGL
   
#  ifndef RETROFLAT_OS_WASM
   /* (This doesn't compile in emcc.) */
   if(
      RETROFLAT_STATE_FLAG_UNLOCK_FPS == (RETROFLAT_STATE_FLAG_UNLOCK_FPS & args->flags)
   ) {
      SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );
   }
#  endif /* !RETROFLAT_OS_WASM */

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
#endif /* RETROFLAT_OPENGL */

   if( NULL != args->title ) {
      retroflat_set_title( args->title );
   }

#ifdef RETROFLAT_SDL_ICO
   debug_printf( 1, "setting SDL window icon..." );
   icon = SDL_LoadBMP_RW(
      SDL_RWFromConstMem( obj_ico_sdl_ico_bmp,
      obj_ico_sdl_ico_bmp_len ), 1 );
   maug_cleanup_if_null( SDL_Surface*, icon, MERROR_GUI );
   SDL_WM_SetIcon( icon, 0 ); /* TODO: Constant mask. */
#endif /* RETROFLAT_SDL_ICO */

#ifndef RETROFLAT_OPENGL

   /* Create our first-stage buffer, which gets drawn on directly. */
   g_retroflat_state->platform.screen_buffer.flags =
      RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER | RETROFLAT_BITMAP_FLAG_OPAQUE;
   assert(
      RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER ==
      (RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER &
         retroflat_screen_buffer()->flags ) );
   g_retroflat_state->platform.screen_buffer.surface = SDL_CreateRGBSurface(
      0,
      g_retroflat_state->screen_v_w
         /* Hardware scrolling example point 1: */
         /* Add off-screen tiles. */
         + (2 * RETROFLAT_TILE_W)
      , g_retroflat_state->screen_v_h
         /* Add off-screen tiles. */
         + (2 * RETROFLAT_TILE_H)
      , 8, 0, 0, 0, 0 );
   if( NULL == g_retroflat_state->platform.screen_buffer.surface ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
         "Error", "Error initializing SDL buffer surface: %s", SDL_GetError() );
   }
   SDL_SetColors(
      g_retroflat_state->platform.screen_buffer.surface,
      g_retroflat_state->palette, 0, RETROFLAT_COLORS_CT_MAX );

   /* Hardware scrolling example point 2: */
   g_retroflat_state->retroflat_flags |= RETROFLAT_STATE_FLAG_HWSCROLLING;

   /* Hardware scrolling example point 3: */
   /* Setup the viewport blit source rect to blit from the center of the
    * unnaturally large buffer by default. */
   g_retroflat_state->platform.viewport_rect.x = RETROFLAT_TILE_W;
   g_retroflat_state->platform.viewport_rect.y = RETROFLAT_TILE_H;
   g_retroflat_state->platform.viewport_rect.w =
      g_retroflat_state->screen_v_w;
   g_retroflat_state->platform.viewport_rect.h =
      g_retroflat_state->screen_v_h;

   /* Insert a normal surface as the standard buffer that things draw to, so
    * those things can be scaled onto the scale buffer as the last step.
    */
   g_retroflat_state->platform.screen_scale.sz =
      sizeof( struct RETROFLAT_BITMAP );
   g_retroflat_state->platform.screen_scale.surface = SDL_CreateRGBSurface(
      0, g_retroflat_state->screen_w, g_retroflat_state->screen_h,
      8, 0, 0, 0, 0 );
   SDL_SetColors(
      g_retroflat_state->platform.screen_scale.surface,
      g_retroflat_state->palette, 0, RETROFLAT_COLORS_CT_MAX );

/* Only attach bitmappable screen if not using OpenGL. */
   g_retroflat_state->platform.screen_final.sz =
      sizeof( struct RETROFLAT_BITMAP );
   g_retroflat_state->platform.screen_final.surface =
#endif /* !RETROFLAT_OPENGL */
   SDL_SetVideoMode( /* See bitmap assignment above ^ */
      g_retroflat_state->screen_w,
      g_retroflat_state->screen_h,
      info->vfmt->BitsPerPixel,
      SDL_DOUBLEBUF | SDL_RESIZABLE
#ifdef RETROFLAT_OPENGL
      | SDL_HWSURFACE | SDL_OPENGL
#else
      | SDL_SWSURFACE
#endif /* RETROFLAT_OPENGL */
   );

#ifndef RETROFLAT_OPENGL
   SDL_SetColors(
      g_retroflat_state->platform.screen_final.surface,
      g_retroflat_state->palette, 0, RETROFLAT_COLORS_CT_MAX );
   maug_cleanup_if_null(
      SDL_Surface*, g_retroflat_state->platform.screen_buffer.surface,
      RETROFLAT_ERROR_GRAPHICS );

#  ifdef RETROFLAT_VDP
   debug_printf( 1, "setting up VDP buffer links..." );
   g_retroflat_state->platform.screen_vdp.sz =
      sizeof( struct RETROFLAT_BITMAP );
   g_retroflat_state->platform.screen_vdp.surface = SDL_CreateRGBSurface(
      0, g_retroflat_state->screen_w, g_retroflat_state->screen_h,
      info->vfmt->BitsPerPixel, 0, 0, 0, 0 );
   g_retroflat_state->vdp_buffer_in =
      &(g_retroflat_state->platform.screen_vdp);
   g_retroflat_state->vdp_buffer_out =
      &(g_retroflat_state->platform.screen_final);
#  endif /* RETROFLAT_VDP */

   assert(
      RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER ==
      (RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER & retroflat_screen_buffer()->flags ) );

#endif /* !RETROFLAT_OPENGL */

cleanup:

   return retval;
}

/* === */

void retroflat_shutdown_platform( MERROR_RETVAL retval ) {

#ifndef RETROFLAT_OPENGL
   if( NULL != g_retroflat_state ) {
      SDL_FreeSurface( g_retroflat_state->platform.screen_buffer.surface );
      SDL_FreeSurface( g_retroflat_state->platform.screen_scale.surface );
#  ifdef RETROFLAT_VDP
      SDL_FreeSurface( g_retroflat_state->platform.screen_vdp.surface );
#  endif /* RETROFLAT_VDP */
   }
#endif /* !RETROFLAT_OPENGL */

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
#  ifdef RETROFLAT_OS_WIN
   uint32_t win_msg_flags = 0;
#  endif /* RETROFLAT_OS_WIN */

   maug_mzero( msg_out, RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

#  ifdef RETROFLAT_OS_WIN
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

#  elif defined( RETROFLAT_OS_UNIX )

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
   maug_mzero( title, RETROFLAT_TITLE_MAX + 1 );
   maug_vsnprintf( title, RETROFLAT_TITLE_MAX, format, vargs );

   SDL_WM_SetCaption( title, NULL );

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

#  if defined( RETROFLAT_OPENGL )

   if( NULL != bmp && &(g_retroflat_state->platform.screen_buffer) != bmp ) {
      debug_printf( RETRO2D_TRACE_LVL, "called retroflat_draw_lock()!" );
   }
 
#  else

   /* == SDL1 == */

   /* SDL locking semantics are the opposite of every other platform. See
    * retroflat_px_lock() for a proxy to SDL_LockSurface().
    */

   if( NULL == bmp || &(g_retroflat_state->platform.screen_buffer) == bmp ) {
      /* Special case: Attempting to lock the screen. */
      bmp = &(g_retroflat_state->platform.screen_buffer);

      if(
         RETROFLAT_BITMAP_FLAG_SCREEN_LOCK !=
         (RETROFLAT_BITMAP_FLAG_SCREEN_LOCK & bmp->flags)
      ) {
         /* Do a perfunctory "screen lock" since programs are supposed to
          * lock the screen before doing any drawing.
          */
         bmp->flags |= RETROFLAT_BITMAP_FLAG_SCREEN_LOCK;

      } else {
         /* We actually want to lock the buffer for pixel manipulation. */
         assert( 0 == (RETROFLAT_BITMAP_FLAG_LOCK & bmp->flags) );
         bmp->flags |= RETROFLAT_BITMAP_FLAG_LOCK;
      }

   } else {
      /* Locking a bitmap for pixel drawing. */
      assert( 0 == (RETROFLAT_BITMAP_FLAG_LOCK & bmp->flags) );
      bmp->flags |= RETROFLAT_BITMAP_FLAG_LOCK;
   }

#  endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_draw_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;
#if !defined( RETROFLAT_OPENGL )
   SDL_Rect scale_rect;
#endif /* !RETROFLAT_OPENGL */

#ifdef RETROFLAT_OPENGL

   if( NULL == bmp || &(g_retroflat_state->platform.screen_buffer) == bmp ) {
      /* SDL has its own OpenGL flip functions.*/
      SDL_GL_SwapBuffers();
   } else {
      debug_printf( RETRO2D_TRACE_LVL, "called retroflat_draw_lock()!" );
   }

#else

   /* == SDL1 == */

   if( NULL == bmp || &(g_retroflat_state->platform.screen_buffer) == bmp ) {
      /* Special case: Attempting to release the (real, non-VDP) screen. */
      bmp = &(g_retroflat_state->platform.screen_buffer);

      if(
         RETROFLAT_BITMAP_FLAG_LOCK == (RETROFLAT_BITMAP_FLAG_LOCK & bmp->flags)
      ) {
         /* The screen was locked for pixel manipulation. */
         bmp->flags &= ~RETROFLAT_BITMAP_FLAG_LOCK;
         SDL_UnlockSurface( bmp->surface );

      } else {
         assert( 
            RETROFLAT_BITMAP_FLAG_SCREEN_LOCK ==
            (RETROFLAT_BITMAP_FLAG_SCREEN_LOCK & bmp->flags) );
         bmp->flags &= ~RETROFLAT_BITMAP_FLAG_SCREEN_LOCK;

         /* Do the scaled blit from the intermediate scaling buffer to the
          * real screen buffer before flip.
          */
         scale_rect.x = 0;
         scale_rect.y = 0;
         scale_rect.w = g_retroflat_state->screen_w;
         scale_rect.h = g_retroflat_state->screen_h;
         SDL_SoftStretch(
            /* Hardware scrolling example point 4: */
            /* If double-buffering, blit from the platform-specific rect
             * inside of the full buffer. Only applicable on windowed systems,
             * generally.
             */
            g_retroflat_state->platform.screen_buffer.surface,
            &(g_retroflat_state->platform.viewport_rect),
            g_retroflat_state->platform.screen_scale.surface,
            &scale_rect );

         /* We need to do one more step since SDL_SoftStretch() can't cope with
          * different bit depths. This gets us from 8-bit paletted to 32-bit
          * real colors.
          */
         SDL_BlitSurface(
            g_retroflat_state->platform.screen_scale.surface, NULL,
#     ifdef RETROFLAT_VDP
            retroflat_vdp_available() ?
               g_retroflat_state->platform.screen_vdp.surface :
#     endif /* RETROFLAT_VDP */
            g_retroflat_state->platform.screen_final.surface,
            NULL );

#  if defined( RETROFLAT_VDP )
         retroflat_vdp_call( "retroflat_vdp_flip" );
#  endif /* RETROFLAT_VDP */

         SDL_Flip( g_retroflat_state->platform.screen_final.surface );
      }

   } else {
      /* Releasing a bitmap. */
      assert( RETROFLAT_BITMAP_FLAG_LOCK == (RETROFLAT_BITMAP_FLAG_LOCK & bmp->flags) );
      bmp->flags &= ~RETROFLAT_BITMAP_FLAG_LOCK;
      SDL_UnlockSurface( bmp->surface );
   }

#endif /* RETROFLAT_OPENGL */

   return retval;
}

/* === */

MERROR_RETVAL retroflat_load_bitmap(
   const char* filename, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   maug_path filename_path;
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETROFLAT_BMP_TEX
   SDL_Surface* tmp_surface = NULL;
   ssize_t inv_idx = 0;
#  endif /* !RETROFLAT_BMP_TEX */

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

#ifdef RETROFLAT_BMP_TEX

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_load_bitmap()!" );
   /*
   assert( NULL != bmp_out );
   retval = retro3d_texture_load_bitmap(
      filename_path, &(bmp_out->tex), flags );
   */

#  else

   /* == SDL1 == */

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "loading bitmap: %s", filename_path );

   tmp_surface = _retroflat_sdl_load_bitmap( filename_path );
   if( NULL == tmp_surface ) {
      error_printf( "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = MERROR_GUI;
      goto cleanup;
   }

   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "loaded bitmap: %d x %d", tmp_surface->w, tmp_surface->h );

   bmp_out->surface = SDL_DisplayFormat( tmp_surface );
   if( NULL == bmp_out->surface ) {
      error_printf( "SDL unable to load bitmap: %s", SDL_GetError() );
      retval = MERROR_GUI;
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

   /* Add the surface to global inventory for palette swaps. */
   inv_idx = mdata_vector_append(
      &g_retroflat_sdl_bitmap_list, &(bmp_out->surface),
      sizeof( SDL_Surface* ) );
   if( 0 > inv_idx ) {
      retval = merror_sz_to_retval( inv_idx );
      goto cleanup;
   }
   debug_printf( RETRO2D_TRACE_LVL,
      "adding bitmap surface to global cache: %p",
      bmp_out->surface );

#  endif /* RETROFLAT_BMP_TEX */

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retroflat_create_bitmap(
   retroflat_pxxy_t w, retroflat_pxxy_t h,
   struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETROFLAT_BMP_TEX
   ssize_t inv_idx = 0;
#endif /* !RETROFLAT_BMP_TEX */

   maug_mzero( bmp_out, sizeof( struct RETROFLAT_BITMAP ) );

   if( retroflat_bitmap_has_flags( bmp_out, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return retval;
   }

#ifdef RETROFLAT_BMP_TEX

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_create_bitmap()!" );
   /*
   assert( NULL != bmp_out );
   retval = retro3d_texture_create( w, h, &(bmp_out->tex), flags );
   */

#else

   bmp_out->sz = sizeof( struct RETROFLAT_BITMAP );

   bmp_out->flags |= flags;

   /* == SDL1 == */

   assert( RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER !=
      (RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER & flags) );

   /* Create image surface. */
   bmp_out->surface = SDL_CreateRGBSurface( 0, w, h,
      /* If we get the screen buffer flag, it's probably from the VDP creation,
       * so use a 32-bit image capable of effects rather than an 8-bit paletted
       * image. The init callback calls SDL_CreateRGBSurface() directly.
       */
      8, 0, 0, 0, 0 );
   maug_cleanup_if_null(
      SDL_Surface*, bmp_out->surface, MERROR_GUI );
   SDL_SetColors( /* TODO: Sync colors with screen throughout lifecycle? */
      bmp_out->surface,
      g_retroflat_state->palette, 0, RETROFLAT_COLORS_CT_MAX );

   /* Add the surface to global inventory for palette swaps. */
   inv_idx = mdata_vector_append(
      &g_retroflat_sdl_bitmap_list, &(bmp_out->surface),
      sizeof( SDL_Surface* ) );
   if( 0 > inv_idx ) {
      retval = merror_sz_to_retval( inv_idx );
      goto cleanup;
   }
   debug_printf( RETRO2D_TRACE_LVL,
      "adding bitmap surface to global cache: %p",
      bmp_out->surface );
   if(
      RETROFLAT_BITMAP_FLAG_OPAQUE != (RETROFLAT_BITMAP_FLAG_OPAQUE & flags)
   ) {
      SDL_SetColorKey( bmp_out->surface, RETROFLAT_SDL_CC_FLAGS,
         SDL_MapRGB( bmp_out->surface->format,
            RETROFLAT_TXP_R, RETROFLAT_TXP_G, RETROFLAT_TXP_B ) );
   }

cleanup:
#endif /* RETROFLAT_BMP_TEX */

   return retval;
}

/* === */

void retroflat_destroy_bitmap( struct RETROFLAT_BITMAP* bmp ) {
#ifndef RETROFLAT_BMP_TEX
   SDL_Surface** st = NULL;
   size_t i = 0;
   MERROR_RETVAL retval = MERROR_OK;
#endif /* !RETROFLAT_BMP_TEX */

   if( retroflat_bitmap_has_flags( bmp, RETROFLAT_BITMAP_FLAG_RO ) ) {
      return;
   }

#ifdef RETROFLAT_BMP_TEX

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_destroy_bitmap()!" );
   /*
   assert( NULL != bmp );
   retro3d_texture_destroy( &(bmp->tex) );
   */

#else

   assert( NULL != bmp );
   assert( NULL != bmp->surface );

   if( 0 < mdata_vector_ct( &g_retroflat_sdl_bitmap_list ) ) {
      mdata_vector_lock( &g_retroflat_sdl_bitmap_list );
      for( i = 0 ; mdata_vector_ct( &g_retroflat_sdl_bitmap_list ) > i ; i++ ) {
         st = mdata_vector_get( &g_retroflat_sdl_bitmap_list, i, SDL_Surface* );
         if( *st == bmp->surface ) {
            debug_printf( RETRO2D_TRACE_LVL, "found surface: %p", *st );
            break;
         }
      }
      mdata_vector_unlock( &g_retroflat_sdl_bitmap_list );
      if( mdata_vector_ct( &g_retroflat_sdl_bitmap_list ) > i ) {
         mdata_vector_remove( &g_retroflat_sdl_bitmap_list, i );
      } else {
         error_printf( "surface %p not in inventory vector!", *st );
      }
   }

   SDL_FreeSurface( bmp->surface );
   bmp->surface = NULL;

cleanup:

   if( MERROR_OK != retval ) {
      error_printf( "error during bitmap destroy!" );
   }

   return;
#endif /* RETROFLAT_BMP_TEX */

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
#  ifndef  RETROFLAT_OPENGL
   SDL_Rect src_rect;
   SDL_Rect dest_rect;
#  endif /* !RETROFLAT_OPENGL */

   assert( NULL != src );

#  if defined( RETROFLAT_OPENGL )

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_blit_bitmap()!" );
   /*
   assert( NULL != target );
   retval = retro3d_texture_blit(
      &(target->tex), &(src->tex), s_x, s_y, d_x, d_y, w, h, instance );
   */

#  else

   /* == SDL == */

   if( NULL == target || retroflat_screen_buffer() == target ) {
      target = retroflat_screen_buffer();

      assert(
         RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER ==
         (RETROFLAT_BITMAP_FLAG_SCREEN_BUFFER & target->flags ) );

      /* Hardware scrolling example point 5: */
      /* Run bitmaps targeted at the screen through the hwscroll preprocessor.
       * This applies adjustments based on the instance, so tiles, sprites,
       * and window bitmaps appear naturally.
       */
      retval = _retroview_hwscroll( &d_x, &d_y, w, h, instance );
      maug_cleanup_if_not_ok();
   }

   if( retroflat_bitmap_has_flags( target, RETROFLAT_BITMAP_FLAG_RO ) ) {
      retval = MERROR_GUI;
      return retval;
   }

   /* Hardware scrolling example point 6: */
   /* Use the trim_px function to keep stuff drawn on-screen, but make sure
    * the retroflat_bitmap_w() passes the REAL screen buffer size (including
    * 2x tile w/h additions on each side) when passed the bitmap behind
    * retroflat_screen_buffer()!
    */

   /* Trim sprite to stay on-screen. */
   retval = _retroview_trim_px(
      target, instance, &s_x, &s_y, &d_x, &d_y, &w, &h );
   maug_cleanup_if_not_ok();

   assert( d_x >= 0 );
   assert( d_y >= 0 );
   assert( d_x < retroflat_bitmap_w( target ) );
   assert( d_y < retroflat_bitmap_h( target ) );

   src_rect.x = s_x;
   src_rect.y = s_y;
   src_rect.w = w;
   src_rect.h = h;
   dest_rect.x = d_x;
   dest_rect.y = d_y;
   dest_rect.w = w;
   dest_rect.h = h;

   assert( 0 == src->autolock_refs );
   assert( 0 == target->autolock_refs );

   assert( d_x >= 0 );
   assert( d_y >= 0 );
   assert( d_x < retroflat_bitmap_w( target ) );
   assert( d_y < retroflat_bitmap_h( target ) );

   retval = 
      SDL_BlitSurface( src->surface, &src_rect, target->surface, &dest_rect );
   if( 0 != retval ) {
      error_printf( "could not blit surface: %s", SDL_GetError() );
      retval = MERROR_GUI;
   }

cleanup:

#  endif

   return retval;
}

/* === */

void retroflat_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   retroflat_pxxy_t x, retroflat_pxxy_t y, uint8_t flags
) {
#  ifndef RETROFLAT_OPENGL
   int offset = 0;
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

#  if defined( RETROFLAT_OPENGL )

   debug_printf( RETRO2D_TRACE_LVL, "called retroflat_px()!" );
   /*
   assert( NULL != target );
   retro3d_texture_px( &(target->tex), color_idx, x, y, flags );
   */

#  else

   /* == SDL1 == */

   /* retroflat_px_lock( target ); */

   retroflat_viewport_constrain_px( x, y, target, return );

   assert( 0 < target->autolock_refs );
   assert( 1 == target->surface->format->BytesPerPixel );

   assert( x >= 0 );
   assert( y >= 0 );
   assert( x < retroflat_bitmap_w( target ) );
   assert( y < retroflat_bitmap_h( target ) );

   offset = (y * target->surface->pitch) + x;

   ((uint8_t*)(target->surface->pixels))[offset] = color_idx;

   /* retroflat_px_release( target ); */

#  endif /* RETROFLAT_OPENGL */
}

/* === */

void retroflat_get_palette( uint8_t idx, uint32_t* p_rgb ) {

#  ifdef RETROFLAT_BMP_TEX

   *p_rgb = 0;
   *p_rgb |= g_retroflat_state->tex_palette[idx][0] & 0xff;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][1] & 0xff) << 8;
   *p_rgb |= (g_retroflat_state->tex_palette[idx][2] & 0xff) << 16;

#  else

   *p_rgb = 0;
   *p_rgb |= g_retroflat_state->palette[idx].r & 0xff;
   *p_rgb |= (g_retroflat_state->palette[idx].g & 0xff) << 8;
   *p_rgb |= (g_retroflat_state->palette[idx].b & 0xff) << 16;

#  endif

}

/* === */

MERROR_RETVAL retroflat_set_palette( uint8_t idx, uint32_t rgb ) {
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETROFLAT_BMP_TEX
   size_t i = 0;
   SDL_Surface** st = NULL;
#endif /* !RETROFLAT_BMP_TEX */

   if( RETROFLAT_COLORS_CT_MAX <= idx ) {
      error_printf( "invalid color index: %u", idx );
      return MERROR_GUI;
   }

#ifdef RETROFLAT_BMP_TEX

#if RETRO2D_TRACE_LVL > 0
   debug_printf( RETRO2D_TRACE_LVL,
      "setting texture palette #%u to " X32_FMT "...",
      idx, rgb );
#endif /* RETRO2D_TRACE_LVL */

   g_retroflat_state->tex_palette[idx][0] = rgb & 0xff;
   g_retroflat_state->tex_palette[idx][1] = (rgb & 0xff00) >> 8;
   g_retroflat_state->tex_palette[idx][2] = (rgb & 0xff0000) >> 16;

#else

#if RETRO2D_TRACE_LVL > 0
   debug_printf( RETRO2D_TRACE_LVL,
      "setting palette #%u to " X32_FMT "...",
      idx, rgb );
#endif /* RETRO2D_TRACE_LVL */

   g_retroflat_state->palette[idx].r = rgb & 0xff;
   g_retroflat_state->palette[idx].g = (rgb & 0xff00) >> 8;
   g_retroflat_state->palette[idx].b = (rgb & 0xff0000) >> 16;

   SDL_SetColors(
      g_retroflat_state->platform.screen_buffer.surface,
      g_retroflat_state->palette, 0, RETROFLAT_COLORS_CT_MAX );

   SDL_SetColors(
      g_retroflat_state->platform.screen_scale.surface,
      g_retroflat_state->palette, 0, RETROFLAT_COLORS_CT_MAX );

   /* Set the palettes on all loaded bitmaps. */
   if( 0 < mdata_vector_ct( &g_retroflat_sdl_bitmap_list ) ) {
      mdata_vector_lock( &g_retroflat_sdl_bitmap_list );
      for( i = 0 ; mdata_vector_ct( &g_retroflat_sdl_bitmap_list ) > i ; i++ ) {
         st = mdata_vector_get( &g_retroflat_sdl_bitmap_list, i, SDL_Surface* );
         SDL_SetColors(
            *st, g_retroflat_state->palette, 0, RETROFLAT_COLORS_CT_MAX );
      }
      mdata_vector_unlock( &g_retroflat_sdl_bitmap_list );
   }

cleanup:
#  endif

   return retval;
}

/* === */

void retroflat_resize_v() {
   /* TODO */
}

/* === */

uint8_t retroflat_focus_platform() {
   uint8_t app_state = 0;
   uint8_t app_state_out = 0;

   app_state = SDL_GetAppState();

   if( SDL_APPINPUTFOCUS == (SDL_APPINPUTFOCUS & app_state) ) {
      app_state_out |= RETROFLAT_FOCUS_FLAG_ACTIVE;
   }

   if( SDL_APPACTIVE == (SDL_APPACTIVE & app_state) ) {
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

   /* Hardware scrolling example point 7: */
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

