
#ifndef RETPLTF_H
#define RETPLTF_H

void retroflat_message(
   uint8_t flags, const char* title, const char* format, ...
) {
   char msg_out[RETROFLAT_MSG_MAX + 1];
   va_list vargs;

   memset( msg_out, '\0', RETROFLAT_MSG_MAX + 1 );
   va_start( vargs, format );
   maug_vsnprintf( msg_out, RETROFLAT_MSG_MAX, format, vargs );

   allegro_message( "%s", msg_out );

   va_end( vargs );
}

#endif /* !RETPLTF_H */

