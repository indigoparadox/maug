
#define RETROVDP_C
#include <maug.h>
#include <retroflt.h>

#if !defined( RETROFLAT_API_SDL1 ) && \
   !defined( RETROFLAT_API_WIN32 ) && \
   !defined( RETROFLAT_API_SDL2 )
#   error "NTSC presently only works with SDL or WIN32!"
#endif /* !RETROFLAT_API_SDL1 && !RETROFLAT_API_WIN32 */

#define NTSC_C
#include "ntsc.h"

struct VDP_DATA {
   struct NTSC_SETTINGS ntsc;
   struct CRT crt;
   int field;
   int noise;
};

#ifdef RETROFLAT_OS_WIN
extern __declspec( dllexport )
#endif /* RETROFLAT_OS_WIN */
MERROR_RETVAL retroflat_vdp_init( struct RETROFLAT_STATE* state ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct VDP_DATA* data = NULL;

   debug_printf( 3, "setting up NTSC..." );

   /* Make sure we used the same header to define bitmap struct! */
   assert( NULL != state->vdp_buffer_in );
   assert( sizeof( struct RETROFLAT_BITMAP ) == state->vdp_buffer_in->sz );

   state->vdp_data = calloc( 1, sizeof( struct VDP_DATA ) );
   maug_cleanup_if_null_alloc( void*, state->vdp_data );
   data = (struct VDP_DATA*)(state->vdp_data);

   state->vdp_flags = RETROFLAT_VDP_FLAG_PXLOCK;

   /* Initialize CRT buffer. */
   crt_init(
      &(data->crt), state->screen_w, state->screen_v_h,
#if defined( RETROFLAT_API_SDL1 )
      CRT_PIX_FORMAT_RGBA, state->vdp_buffer_out->surface->pixels
#elif defined( RETROFLAT_API_SDL2 )
      CRT_PIX_FORMAT_RGBA, NULL
#else
      /* TODO */
      CRT_PIX_FORMAT_BGRA, state->vdp_buffer_out->bits
#endif /* RETROFLAT_API_SDL1 */
   );
   data->crt.blend = 1;
   data->crt.scanlines = 1;

   data->ntsc.w = state->screen_w;
   data->ntsc.h = state->screen_h;
   data->ntsc.as_color = 1;
   data->ntsc.raw = 0;
   if( 0 < strlen( state->vdp_args ) ) {
      debug_printf( 1, "NTSC noise: %d\n", atoi( state->vdp_args ) );
      data->noise = atoi( state->vdp_args );
   }

cleanup:

   return retval;
}

#ifdef RETROFLAT_OS_WIN
extern __declspec( dllexport )
#endif /* RETROFLAT_OS_WIN */
MERROR_RETVAL retroflat_vdp_shutdown( struct RETROFLAT_STATE* state ) {
   MERROR_RETVAL retval = MERROR_OK;
   debug_printf( 3, "shutting down NTSC..." );
   free( state->vdp_data );
   return retval;
}

#ifdef RETROFLAT_OS_WIN
extern __declspec( dllexport )
#endif /* RETROFLAT_OS_WIN */
MERROR_RETVAL retroflat_vdp_flip( struct RETROFLAT_STATE* state ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct VDP_DATA* data = (struct VDP_DATA*)(state->vdp_data);

#if defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   if(
      4 != state->vdp_buffer_out->surface->format->BytesPerPixel
   ) {
      error_printf( "invalid output buffer BPP: %d",
         state->vdp_buffer_out->surface->format->BytesPerPixel );
      retval = MERROR_GUI;
      goto cleanup;
   }
#endif /* RETROFLAT_API_SDL1 */
   if( 0 >= state->screen_w || 0 >= state->screen_h ) {
      error_printf( "invalid screen dimensions:",
         state->screen_w, state->screen_h );
      retval = MERROR_GUI;
      goto cleanup;
   }
   assert( NULL != data );
   if( NULL == state->vdp_buffer_in ) {
      error_printf( "undefined input buffer!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

#if defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   data->ntsc.data = state->vdp_buffer_in->surface->pixels;
   data->crt.out = state->vdp_buffer_out->surface->pixels;
   data->ntsc.format = CRT_PIX_FORMAT_RGBA;
#else
   if( NULL == state->vdp_buffer_in->bits ) {
      error_printf( "uninitialized input buffer bits!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   data->ntsc.data = state->vdp_buffer_in->bits;
   data->crt.out = state->vdp_buffer_out->bits;
   data->ntsc.format = CRT_PIX_FORMAT_BGRA;
#endif /* RETROFLAT_API_SDL1 */

   assert( NULL != data->ntsc.data );
   assert( NULL != data->crt.out );

   data->ntsc.field = data->field & 1;
   if( 0 == data->ntsc.field ) {
      data->ntsc.frame ^= 1;
   }
   crt_modulate( &(data->crt), &(data->ntsc) );
   crt_demodulate( &(data->crt), data->noise );
   data->ntsc.field ^= 1;

cleanup:

   return retval;
}

