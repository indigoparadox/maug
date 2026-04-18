
#define RETROVDP_C
#include <maug.h>
#include <retroflt.h>

#ifdef RETROFLAT_OS_WIN
extern __declspec( dllexport )
#endif /* RETROFLAT_OS_WIN */
MERROR_RETVAL retroflat_vdp_init( struct RETROFLAT_STATE* state ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 3, "setting up blit VDP..." );

   state->vdp_flags = RETROFLAT_VDP_FLAG_PXLOCK;

cleanup:

   return retval;
}

#ifdef RETROFLAT_OS_WIN
extern __declspec( dllexport )
#endif /* RETROFLAT_OS_WIN */
MERROR_RETVAL retroflat_vdp_shutdown( struct RETROFLAT_STATE* state ) {
   MERROR_RETVAL retval = MERROR_OK;
   debug_printf( 3, "shutting down blit VDP..." );
   free( state->vdp_data );
   return retval;
}

#ifdef RETROFLAT_OS_WIN
extern __declspec( dllexport )
#endif /* RETROFLAT_OS_WIN */
MERROR_RETVAL retroflat_vdp_flip( struct RETROFLAT_STATE* state ) {
   MERROR_RETVAL retval = MERROR_OK;

#ifdef RETROFLAT_OS_WIN
   memcpy( state->buffer.bits, state->vdp_buffer->bits,
      state->vdp_buffer->w * state->vdp_buffer->h * 4 );
#endif

   return retval;
}

