
#ifndef MFMT_H
#define MFMT_H

#include <mfile.h>

#define MFMT_BMPINFO_OFS_WIDTH 4
#define MFMT_BMPINFO_OFS_HEIGHT 8
#define MFMT_BMPINFO_OFS_COLOR_PLANES 12
#define MFMT_BMPINFO_OFS_BPP 14
#define MFMT_BMPINFO_OFS_COMPRESSION 16
#define MFMT_BMPINFO_OFS_SZ 20
#define MFMT_BMPINFO_OFS_HRES 24
#define MFMT_BMPINFO_OFS_VRES 28
#define MFMT_BMPINFO_OFS_PAL_SZ 32
#define MFMT_BMPINFO_OFS_IMP_COLORS 36

#define MFMT_BMP_COMPRESSION_NONE (0)

struct MFMT_STRUCT {
   uint32_t sz;
};

struct MFMT_STRUCT_BMPINFO {
   uint32_t sz;
   int32_t width;
   int32_t height;
   uint16_t color_planes;
   uint16_t bpp;
   uint32_t compression;
   uint32_t img_sz;
   uint32_t hres;
   uint32_t vres;
   uint32_t palette_ncolors;
   uint32_t imp_colors;
};

typedef MERROR_RETVAL (*mfmt_read_header_cb)(
   struct MFMT_STRUCT* header, mfile_t* p_file_in,
   uint32_t file_offset, size_t file_sz );

typedef MERROR_RETVAL (*mfmt_read_palette_cb)(
   struct MFMT_STRUCT* header, uint32_t* palette, size_t palette_sz,
   mfile_t* p_file_in, uint32_t file_offset, size_t file_sz );

typedef MERROR_RETVAL (*mfmt_read_px_cb)(
   struct MFMT_STRUCT* header, uint8_t* px, size_t px_sz,
   mfile_t* p_file_in, uint32_t file_offset, size_t file_sz );

MERROR_RETVAL mfmt_read_bmp_header(
   struct MFMT_STRUCT* header, mfile_t* p_file_in,
   uint32_t file_offset, size_t file_sz );

MERROR_RETVAL mfmt_read_bmp_palette(
   struct MFMT_STRUCT* header, uint32_t* palette, size_t palette_sz,
   mfile_t* p_file_in, uint32_t file_offset, size_t file_sz );

MERROR_RETVAL mfmt_read_bmp_px(
   struct MFMT_STRUCT* header, uint8_t* px, size_t px_sz,
   mfile_t* p_file_in, uint32_t file_offset, size_t file_sz );

#ifdef MFMT_C

MERROR_RETVAL mfmt_read_bmp_header(
   struct MFMT_STRUCT* header, mfile_t* p_file_in,
   uint32_t file_offset, size_t file_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MFMT_STRUCT_BMPINFO* header_bmp_info = NULL;
   uint32_t file_hdr_sz = 0;

   /* TODO: Check header sz (file header or info header). */
   if( 40 == header->sz ) {
      header_bmp_info = (struct MFMT_STRUCT_BMPINFO*)header;
   } else {
      error_printf( "unable to select read header!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

   /* Read the bitmap image header. */
   mfile_u32read_lsbf_at( p_file_in, &(file_hdr_sz),
      file_offset );
   if( 40 != file_hdr_sz ) { /* Windows BMP. */
      error_printf( "invalid header size: %u", file_hdr_sz );
      retval = MERROR_FILE;
      goto cleanup;
   }
   debug_printf( 2, "bitmap header is %u bytes", file_hdr_sz );

   /* Read bitmap image dimensions. */
   mfile_u32read_lsbf_at( p_file_in, &(header_bmp_info->width),
      file_offset + 4 );
   mfile_u32read_lsbf_at( p_file_in, &(header_bmp_info->height),
      file_offset + 8 );

   /* Check that we're a palettized image. */
   mfile_u16read_lsbf_at( p_file_in, &(header_bmp_info->bpp),
      file_offset + 14 );
   if( 8 < header_bmp_info->bpp ) {
      error_printf( "invalid bitmap bpp: %u", header_bmp_info->bpp );
      retval = MERROR_FILE;
      goto cleanup;
   }

   /* Make sure there's no weird compression. */
   mfile_u32read_lsbf_at( p_file_in,
      &(header_bmp_info->compression), file_offset + 16 );
   if( MFMT_BMP_COMPRESSION_NONE != header_bmp_info->compression ) {
      error_printf( "invalid bitmap compression: %u",
         header_bmp_info->compression );
      retval = MERROR_FILE;
      goto cleanup;
   }

   mfile_u32read_lsbf_at( p_file_in,
      &(header_bmp_info->palette_ncolors), file_offset + 32 );

   debug_printf( 2, "bitmap is " UPRINTF_S32_FMT " x " UPRINTF_S32_FMT
      ", %u bpp (palette has " UPRINTF_U32_FMT " colors)",
      header_bmp_info->width, header_bmp_info->height,
      header_bmp_info->bpp, header_bmp_info->palette_ncolors );

cleanup:

   return retval;
}

MERROR_RETVAL mfmt_read_bmp_palette(
   struct MFMT_STRUCT* header, uint32_t* palette, size_t palette_sz,
   mfile_t* p_file_in, uint32_t file_offset, size_t file_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MFMT_STRUCT_BMPINFO* header_bmp_info = NULL;
   size_t i = 0;

   if( 40 == header->sz ) {
      header_bmp_info = (struct MFMT_STRUCT_BMPINFO*)header;
   } else {
      error_printf( "unable to select read header!" );
      retval = MERROR_FILE;
      goto cleanup;
   }
 
   for( i = 0 ; header_bmp_info->palette_ncolors > i ; i++ ) {
      if( i * 4 > palette_sz ) {
         error_printf( "palette overflow!" );
         retval = MERROR_OVERFLOW;
         goto cleanup;
      }
      mfile_u32read_lsbf_at( p_file_in, &(palette[i]),
         file_offset + (i * 4) );
      debug_printf( 1,
         "set palette entry " SIZE_T_FMT " to " UPRINTF_X32_FMT,
         i, palette[i] );
   }

cleanup:

   return retval;
}

MERROR_RETVAL mfmt_read_bmp_px(
   struct MFMT_STRUCT* header, uint8_t* px, size_t px_sz,
   mfile_t* p_file_in, uint32_t file_offset, size_t file_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MFMT_STRUCT_BMPINFO* header_bmp_info = NULL;
   uint32_t x = 0,
      y = 0,
      i = 0,
      byte_idx = 0,
      bit_idx = 0;
   uint8_t byte_buffer = 0,
      byte_mask = 0,
      pixel_buffer = 0;

   if( 40 == header->sz ) {
      header_bmp_info = (struct MFMT_STRUCT_BMPINFO*)header;
   } else {
      error_printf( "unable to select read header!" );
      retval = MERROR_FILE;
      goto cleanup;
   }
 
   /* TODO: Handle upside-down? */
   y = header_bmp_info->height - 1;
   while( header_bmp_info->height > y ) {
      /* Each iteration is a single, fresh pixel. */
      pixel_buffer = 0;

      debug_printf( 0, "bmp: byte_idx %u, bit %u (%u), row %d, col %d (%u)",
         byte_idx, bit_idx, header_bmp_info->bpp, y, x, (y * header_bmp_info->width) + x );

      if( 0 == bit_idx ) {
         /* Move on to a new byte. */
         mfile_cread_at(
            p_file_in, &(byte_buffer), file_offset + byte_idx );
         byte_idx++;

         /* Start at 8 bits from the right (0 from the left). */
         bit_idx = 8;

         /* Build a bitwise mask based on the bitmap's BPP. */
         byte_mask = 0;
         for( i = 0 ; header_bmp_info->bpp > i ; i++ ) {
            byte_mask >>= 1;
            byte_mask |= 0x80;
         }
      }

      /* TODO: Bounds checking! */
      /* assert( (y * sz_x) + x < (buf_sz * (8 / grid->bpp)) ); */

      /* Use the byte mask to place the bits for this pixel in the
       * pixel buffer.
       */
      pixel_buffer |= byte_buffer & byte_mask;
      
      /* Shift the pixel buffer so the index lines up at the first bit. */
      pixel_buffer >>= 
         /* Index starts from the right, so the current bits from the left
          * minus 1 * bpp.
          */
         (bit_idx - header_bmp_info->bpp);
      debug_printf( 0,
         "byte_mask: 0x%02x, bit_idx: %u, pixel_buffer: 0x%02x",
         byte_mask, bit_idx, pixel_buffer );

      /* Place the pixel buffer at the X/Y in the grid. */
      px[(y * header_bmp_info->width) + x] = pixel_buffer;

      /* Increment the bits position byte mask by the bpp so it's pointing
       * to the next pixel in the bitmap for the next go around.
       */
      byte_mask >>= header_bmp_info->bpp;
      bit_idx -= header_bmp_info->bpp;
      assert( 8 > bit_idx );

      /* Move to the next pixel. */
      x++;
      if( x >= header_bmp_info->width ) {
         /* Move to the next row. */
         y--;
         x = 0;
         while( byte_idx % 4 != 0 ) {
            byte_idx++;
         }
         /* Get past the padding. */
      }
   }

cleanup:

   return retval;
}

#endif /* MFMT_C */

#endif /* !MFMT_H */

