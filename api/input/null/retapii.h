
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

typedef int16_t RETROFLAT_IN_KEY;

#elif defined( RETROFLT_C )

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   /* TODO */

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

