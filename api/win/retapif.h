
#ifndef RETPLTF_H
#define RETPLTF_H

/* TODO */

#ifndef RETROFLAT_OPENGL

static int retroflat_bitmap_win_transparency(
   struct RETROFLAT_BITMAP* bmp_out, int16_t w, int16_t h  
) {
   int retval = RETROFLAT_OK;
   unsigned long txp_color = 0;

   /* Setup bitmap transparency mask. */
   bmp_out->mask = CreateBitmap( w, h, 1, 1, NULL );
   maug_cleanup_if_null( HBITMAP, bmp_out->mask, RETROFLAT_ERROR_BITMAP );

   retval = retroflat_draw_lock( bmp_out );
   maug_cleanup_if_not_ok();

   /* Convert the color key into bitmap format. */
   txp_color |= (RETROFLAT_TXP_B & 0xff);
   txp_color <<= 8;
   txp_color |= (RETROFLAT_TXP_G & 0xff);
   txp_color <<= 8;
   txp_color |= (RETROFLAT_TXP_R & 0xff);
   SetBkColor( bmp_out->hdc_b, txp_color );

   /* Create the mask from the color key. */
   BitBlt(
      bmp_out->hdc_mask, 0, 0, w, h, bmp_out->hdc_b, 0, 0, SRCCOPY );
   BitBlt(
      bmp_out->hdc_b, 0, 0, w, h, bmp_out->hdc_mask, 0, 0, SRCINVERT );

cleanup:

   if( RETROFLAT_OK == retval ) {
      retroflat_draw_release( bmp_out );
   }

   return retval;
}

#endif /* !RETROFLAT_OPENGL */


#endif /* !RETPLTF_H */

