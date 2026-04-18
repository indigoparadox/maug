
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
   assert( NULL != state->vdp_buffer );
   assert( sizeof( struct RETROFLAT_BITMAP ) == state->vdp_buffer->sz );

   state->vdp_data = calloc( 1, sizeof( struct VDP_DATA ) );
   maug_cleanup_if_null_alloc( void*, state->vdp_data );
   data = (struct VDP_DATA*)(state->vdp_data);

   state->vdp_flags = RETROFLAT_VDP_FLAG_PXLOCK;

   /* Initialize CRT buffer. */
   crt_init(
      &(data->crt), state->screen_v_w, state->screen_v_h,
#if defined( RETROFLAT_API_SDL1 )
      CRT_PIX_FORMAT_RGBA, state->platform.screen_buffer.surface->pixels );
#elif defined( RETROFLAT_API_SDL2 )
      CRT_PIX_FORMAT_RGBA, NULL );
#else
      /* TODO */
      CRT_PIX_FORMAT_BGRA, state->platform.screen_buffer.bits );
#endif /* RETROFLAT_API_SDL1 */
   data->crt.blend = 1;
   data->crt.scanlines = 1;

   data->ntsc.w = state->screen_v_w;
   data->ntsc.h = state->screen_v_h;
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
   assert( 4 == state->platform.screen_buffer.surface->format->BytesPerPixel );
#endif /* RETROFLAT_API_SDL1 */
   assert( 0 < state->screen_v_w );
   assert( 0 < state->screen_v_h );
   assert( NULL != data );
   assert( NULL != state->vdp_buffer );

#if defined( RETROFLAT_API_SDL1 ) || defined( RETROFLAT_API_SDL2 )
   data->ntsc.data = state->vdp_buffer->surface->pixels;
   data->crt.out = state->platform.screen_buffer.surface->pixels;
   data->ntsc.format = CRT_PIX_FORMAT_RGBA;
#else
   assert( NULL != state->vdp_buffer->bits );

   data->ntsc.data = state->vdp_buffer->bits;
   data->crt.out = state->platform.screen_buffer.bits;
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

   return retval;
}

