
#ifndef RETROSND_H
#define RETROSND_H

#ifdef RETROSND_API_GUS
#  include <conio.h>
#endif /* RETROSND_API_GUS */

struct RETROSND_STATE {
#ifdef RETROSND_API_GUS
   uint16_t io_base;
#endif /* RETROSND_API_GUS */
};

#ifdef RETROSND_C
struct RETROSND_STATE g_retrosnd_state;
#endif /* RETROSND_C */

#if defined( RETROSND_API_GUS ) && defined( RETROSND_C )

static void retrosnd_gus_poke( uint32_t loc, uint8_t b ) {
   uint16_t add_lo = loc & 0xffff;
   uint8_t add_hi = (uint32_t)((loc & 0xff0000)) >> 16;

   debug_printf( 1,
      "poke hi: 0x%02x, lo: 0x%04x: writing: %02x", add_hi, add_lo, b );
   
   outp( g_retrosnd_state.io_base + 0x103, 0x43 );
   outpw( g_retrosnd_state.io_base + 0x104, add_lo );
   outp( g_retrosnd_state.io_base + 0x103, 0x44 );
   outp( g_retrosnd_state.io_base + 0x105, add_hi );
   outp( g_retrosnd_state.io_base + 0x107, b );

}

static uint8_t retrosnd_gus_peek( uint32_t loc ) {
   uint8_t b = 0;
   uint16_t add_lo = loc & 0xffff;
   uint8_t add_hi = (uint32_t)((loc & 0xff0000)) >> 16;

   outp( g_retrosnd_state.io_base + 0x103, 0x43 );
   outpw( g_retrosnd_state.io_base + 0x104, add_lo );
   outp( g_retrosnd_state.io_base + 0x103, 0x44 );
   outp( g_retrosnd_state.io_base + 0x105, add_hi );

   b = inp( g_retrosnd_state.io_base + 0x107 );

   debug_printf( 1, "read: 0x%02x", b );

   return b;
}

#endif /* RETROSND_API_GUS && RETROSND_C */

int16_t retrosnd_init( struct RETROFLAT_ARGS* args ) {
   uint8_t b = 0;

#ifdef RETROSND_API_GUS

   g_retrosnd_state.io_base = 0x220;
   
   outp( g_retrosnd_state.io_base + 0x103, 0x4c );
   outp( g_retrosnd_state.io_base + 0x105, 0 );
   sleep( 1 );
   outp( g_retrosnd_state.io_base + 0x103, 0x4c );
   outp( g_retrosnd_state.io_base + 0x105, 1 );

   retrosnd_gus_poke( 0, 0xf );
   retrosnd_gus_poke( 0x100, 0x55 );

   b = retrosnd_gus_peek( 0 );
   if( 0xf == b ) {
      debug_printf( 3, "found gravis ultrasound!" );
   }

#endif /* RETROSND_API_GUS */

   return RETROFLAT_OK;
}

void retrosnd_shutdown() {

}

#endif /* !RETROSND_H */

