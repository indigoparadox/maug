
#ifndef RETPLTF_H
#define RETPLTF_H

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

void retroflat_blit_bitmap(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   int s_x, int s_y, int d_x, int d_y, int16_t w, int16_t h
) {
#  if defined( RETROFLAT_API_SDL1 ) && !defined( RETROFLAT_OPENGL )
   MERROR_RETVAL retval = MERROR_OK;
   SDL_Rect src_rect;
   SDL_Rect dest_rect;
#  elif defined( RETROFLAT_API_SDL2 )
   MERROR_RETVAL retval = MERROR_OK;
   SDL_Rect src_rect = { s_x, s_y, w, h };
   SDL_Rect dest_rect = { d_x, d_y, w, h };
#  endif /* RETROFLAT_API_SDL2 || RETROFLAT_API_SDL1 */

#  ifndef RETROFLAT_OPENGL
   if( NULL == target ) {
      target = retroflat_screen_buffer();
   }
#  endif /* RETROFLAT_OPENGL */

   assert( NULL != src );

#  if defined( RETROFLAT_OPENGL )

   retroglu_blit_bitmap( target, src, s_x, s_y, d_x, d_y, w, h );

#  elif defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )

   /* == SDL == */

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
   }
#     else

   assert( retroflat_bitmap_locked( target ) );
   retval = SDL_RenderCopy(
      target->renderer, src->texture, &src_rect, &dest_rect );
   if( 0 != retval ) {
      error_printf( "could not blit surface: %s", SDL_GetError() );
   }

#     endif /* !RETROFLAT_API_SDL1 */

#  endif

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


#endif /* !RETPLTF_H */

