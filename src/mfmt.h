
#ifndef MFMT_H
#define MFMT_H

#include <mfile.h>

/**
 * \addtogroup maug_fmt Maug File Format API
 * \{
 */

/**
 * \file mfmt.h
 */

/**
 * \addtogroup maug_fmt_bmp Maug File Format: Bitmap
 * \brief Constants and functions describing the Windows bitmap format.
 * \{
 */

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

/*! \brief MFMT_STRUCT_BMPINFO::compression value indicating none. */
#define MFMT_BMP_COMPRESSION_NONE (0)
/*! \brief MFMT_STRUCT_BMPINFO::compression value indicating 8-bit RLE. */
#define MFMT_BMP_COMPRESSION_RLE8 (1)
/*! \brief MFMT_STRUCT_BMPINFO::compression value indicating 4-bit RLE. */
#define MFMT_BMP_COMPRESSION_RLE4 (2)

/*! \} */ /* maug_fmt_bmp */

#define MFMT_DECOMP_FLAG_4BIT 0x01
#define MFMT_DECOMP_FLAG_8BIT 0x02

#define MFMT_PX_FLAG_INVERT_Y 0x01

#ifndef MFMT_TRACE_BMP_LVL
#  define MFMT_TRACE_BMP_LVL 0
#endif /* !MFMT_TRACE_BMP_LVL */

#ifndef MFMT_TRACE_RLE_LVL
#  define MFMT_TRACE_RLE_LVL 0
#endif /* !MFMT_TRACE_RLE_LVL */

/**
 * \brief Generic image description struct.
 *
 * This should actually be an instance of ::MFMT_STRUCT_BMPINFO or a similar
 * struct with overlapping MFMT_STRUCT::sz field which can be used to determine
 * the actual type.
 */
struct MFMT_STRUCT {
   /*! \brief Size of this struct (use to tell apart). */
   uint32_t sz;
};

/**
 * \addtogroup maug_fmt_bmp
 * \{
 */

#define mfmt_bmp_check_header() \
   if( 40 == header->sz ) { \
      header_bmp_info = (struct MFMT_STRUCT_BMPINFO*)header; \
   } else if( 0x4d42 == *((uint16_t*)header) ) { \
      debug_printf( MFMT_TRACE_BMP_LVL, "bmp file header detected" ); \
      header_bmp_file = (struct MFMT_STRUCT_BMPFILE*)header; \
      header_bmp_info = &(header_bmp_file->info); \
   } else { \
      error_printf( "unable to select read header!" ); \
      retval = MERROR_FILE; \
      goto cleanup; \
   }

/*! \brief BITMAPINFO struct that comes before Windows bitmap data. */
struct MFMT_STRUCT_BMPINFO {
   /*! \brief Size of this struct in bytes (only 40 is supported). */
   uint32_t sz;
   /*! \brief Width of the bitmap in pixels. */
   int32_t width;
   /*! \brief Height of the bitmap in pixels. */
   int32_t height;
   /*! \brief Number of color planes (only 0 or 1 are supported). */
   uint16_t color_planes;
   /*! \brief Number of bits per pixel (only =<8 are supported). */
   uint16_t bpp;
   /*! \brief Type of compression used. */
   uint32_t compression;
   /*! \brief Size of pixel data in bytes. */
   uint32_t img_sz;
   /*! \brief Horizontal resolution in pixels per inch (unsupported). */
   uint32_t hres;
   /*! \brief Vertical resolution in pixels per inch (unsupported). */
   uint32_t vres;
   /*! \brief Number of palette colors in this bitmap (<256 supported). */
   uint32_t palette_ncolors;
   /*! \bried Number of important colors (unsupported). */
   uint32_t imp_colors;
};

struct MFMT_STRUCT_BMPFILE {
   char magic[2];
   uint32_t file_sz;
   uint16_t reserved1;
   uint16_t reserved2;
   uint32_t px_offset;
   struct MFMT_STRUCT_BMPINFO info;
};

/*! \} */ /* maug_fmt_bmp */

/**
 * \brief Callback to decode compressed data.
 * \param p_file_in Pointer to file to read compressed data from.
 * \param file_offset Number of bytes into p_file where data starts.
 * \param file_sz Number of bytes of compressed data in p_file.
 * \param buffer_out Unlocked handle to write uncompressed data to.
 * \param buffer_out_sz Maximum number of bytes buffer_out can hold.
 * \param flags Additional flags for compression options.
 */
typedef MERROR_RETVAL (*mfmt_decode)(
   mfile_t* p_file_in, off_t file_offset, off_t file_sz, size_t line_w,
   MAUG_MHANDLE buffer_out, off_t buffer_out_sz, uint8_t flags );

/**
 * \brief Callback to read image header and get properties.
 */
typedef MERROR_RETVAL (*mfmt_read_header_cb)(
   struct MFMT_STRUCT* header, mfile_t* p_file_in,
   uint32_t file_offset, off_t file_sz, uint8_t* p_flags );

/**
 * \brief Callback to read image palette into 24-bit RGB values.
 */
typedef MERROR_RETVAL (*mfmt_read_palette_cb)(
   struct MFMT_STRUCT* header, uint32_t* palette, size_t palette_sz,
   mfile_t* p_file_in, uint32_t file_offset, off_t file_sz,
   uint8_t flags );

/**
 * \brief Callback to read image pixels into 8-bit values.
 */
typedef MERROR_RETVAL (*mfmt_read_px_cb)(
   struct MFMT_STRUCT* header, uint8_t SEG_FAR* px, off_t px_sz,
   mfile_t* p_file_in, uint32_t file_offset, off_t file_sz,
   uint8_t flags );

/**
 * \brief Decode RLE-encoded data from an input file into a memory buffer.
 */
MERROR_RETVAL mfmt_decode_rle(
   mfile_t* p_file_in, off_t file_offset, off_t file_sz, size_t line_w,
   MAUG_MHANDLE buffer_out, off_t buffer_out_sz, uint8_t flags );

MERROR_RETVAL mfmt_read_bmp_header(
   struct MFMT_STRUCT* header, mfile_t* p_file_in,
   uint32_t file_offset, off_t file_sz, uint8_t* p_flags );

MERROR_RETVAL mfmt_read_bmp_palette(
   struct MFMT_STRUCT* header, uint32_t* palette, size_t palette_sz,
   mfile_t* p_file_in, uint32_t file_offset, off_t file_sz,
   uint8_t flags );

/**
 * \brief Read \ref mfmt_bitmap pixels into an 8-bit memory bitmap.
 */
MERROR_RETVAL mfmt_read_bmp_px(
   struct MFMT_STRUCT* header, uint8_t SEG_FAR* px, off_t px_sz,
   mfile_t* p_file_in, uint32_t file_offset, off_t file_sz,
   uint8_t flags );

#ifdef MFMT_C

MERROR_RETVAL mfmt_decode_rle(
   mfile_t* p_file_in, off_t file_offset, off_t file_sz, size_t line_w,
   MAUG_MHANDLE buffer_out_h, off_t buffer_out_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t* buffer_out = NULL;
   off_t in_byte_cur = 0,
      out_byte_cur = 0;
   uint8_t out_mask_cur = 0xf0,
      run_char = 0,
      run_count = 0,
      decode_state = 0,
      unpadded_written = 0,
      line_px_written = 0,
      lines_out = 0,
      byte_buffer = 0;

   #define MFMT_RLE_DECODE_RUN         0
   #define MFMT_RLE_DECODE_CHAR        1
   #define MFMT_RLE_DECODE_ABS_RIGHT   2
   #define MFMT_RLE_DECODE_ABS_DOWN    3
   #define MFMT_RLE_DECODE_ESC         4
   #define MFMT_RLE_DECODE_LITERAL     5
   #define MFMT_RLE_DECODE_LITERAL_PAD 6

   assert( flags == MFMT_DECOMP_FLAG_4BIT );

   #define mfmt_decode_rle_state( new_state ) \
      debug_printf( MFMT_TRACE_RLE_LVL, "new state: %s", #new_state ); \
      decode_state = new_state;

   #define mfmt_decode_rle_check_eol() \
      if( line_px_written >= line_w ) { \
         debug_printf( MFMT_TRACE_RLE_LVL, \
            "EOL: %u px written (between runs)", \
            line_px_written ); \
         mfmt_decode_rle_reset_line(); \
      }

   /* Flip between odd/even nibble as advanced. */
   #define mfmt_decode_rle_advance_mask() \
      out_mask_cur >>= 4; \
      if( 0 == out_mask_cur ) { \
         out_byte_cur++; \
         if( out_byte_cur > buffer_out_sz ) { \
            error_printf( \
               "out byte " OFF_T_FMT " outside of " OFF_T_FMT \
               " pixel buffer!", out_byte_cur, buffer_out_sz ); \
            retval = MERROR_OVERFLOW; \
            goto cleanup; \
         } else if( out_byte_cur < buffer_out_sz ) { \
            /* We're not at the end of the file yet! */ \
            buffer_out[out_byte_cur] = 0; \
         } \
         out_mask_cur = 0xf0; \
      }

   #define mfmt_decode_rle_reset_line() \
      if( line_w != line_px_written ) { \
         error_printf( \
            "line written pixels %u does not match line width " SIZE_T_FMT, \
            line_px_written, line_w ); \
         retval = MERROR_OVERFLOW; \
         goto cleanup; \
      } \
      line_px_written = 0; \
      out_mask_cur = 0xf0; \
      lines_out++; \
      debug_printf( MFMT_TRACE_RLE_LVL, "now on line: %u", lines_out );

   #define mfmt_decode_rle_inc_line_w( incr ) \
      line_px_written += incr; \
      if( line_w < line_px_written ) { \
         error_printf( \
            "line byte %u outside of " SIZE_T_FMT \
            " line width!", line_px_written, line_w ); \
         retval = MERROR_OVERFLOW; \
         goto cleanup; \
      }

   debug_printf( 1, "decompressing RLE into temporary buffer..." );

   maug_mlock( buffer_out_h, buffer_out );

   maug_mzero( buffer_out, buffer_out_sz );

   do {
      retval = p_file_in->seek( p_file_in, file_offset + in_byte_cur++ );
      maug_cleanup_if_not_ok();
      retval = p_file_in->read_int( p_file_in, &byte_buffer, 1, 0 );
      maug_cleanup_if_not_ok();

      /*
      mfile_cread_at(
         p_file_in, &(byte_buffer), file_offset + in_byte_cur++ );
      */
      debug_printf( MFMT_TRACE_RLE_LVL, "in byte " OFF_T_FMT
         ": 0x%02x, out byte " OFF_T_FMT ", line px: %u",
         in_byte_cur, byte_buffer, out_byte_cur, line_px_written );
      
      switch( byte_buffer ) {
      case 0:
         if( MFMT_RLE_DECODE_RUN == decode_state ) {
            mfmt_decode_rle_state( MFMT_RLE_DECODE_ESC );
            break;

         } else if( MFMT_RLE_DECODE_LITERAL_PAD == decode_state ) {
            /* This is just a padding byte to make sure literals are %16. */
            assert( 0 == byte_buffer );
            mfmt_decode_rle_state( MFMT_RLE_DECODE_RUN );
            break;

         } else if( MFMT_RLE_DECODE_ESC == decode_state ) {
            /* This is an EOL marker. */
            debug_printf( MFMT_TRACE_RLE_LVL,
               "EOL: %u px written", line_px_written );
            while( line_px_written < line_w ) {
               /* Pad out the end of the line. */
               assert( 0 == line_px_written % 2 );
               buffer_out[out_byte_cur++] = 0x00;
               mfmt_decode_rle_inc_line_w( 2 );
               debug_printf( MFMT_TRACE_RLE_LVL,
                  "padded line (%u written)", line_px_written );
            }
            mfmt_decode_rle_reset_line();

            /* Diversion over, go back to hunting for runs. */
            mfmt_decode_rle_state( MFMT_RLE_DECODE_RUN );
            break;
         }

      case 1:
         if( MFMT_RLE_DECODE_ESC == decode_state ) {
            debug_printf( MFMT_TRACE_RLE_LVL, "EOBM" );
            /* End of bitmap, so pad the rest of the file. */

            while( out_byte_cur < buffer_out_sz ) {
               /* Pad out the end of the line. */
               assert( 0 == line_px_written % 2 );
               mfmt_decode_rle_check_eol();
               buffer_out[out_byte_cur++] = 0x00;
               mfmt_decode_rle_inc_line_w( 2 );
               debug_printf( MFMT_TRACE_RLE_LVL,
                  "padded file (%u written)", line_px_written );
            }

            mfmt_decode_rle_state( MFMT_RLE_DECODE_RUN );
            break;
         }

      case 2:
         if( MFMT_RLE_DECODE_ESC == decode_state ) {
            debug_printf( MFMT_TRACE_RLE_LVL, "absolute mode: right" );
            /* TODO: Absolute mode. */
            assert( 1 == 0 );
            mfmt_decode_rle_state( MFMT_RLE_DECODE_ABS_RIGHT );
            break;
         }

      default:
         switch( decode_state ) {
         case MFMT_RLE_DECODE_LITERAL:

            mfmt_decode_rle_check_eol();

            run_count -= 2;
            unpadded_written += 2;
            mfmt_decode_rle_inc_line_w( 2 );
            debug_printf( MFMT_TRACE_RLE_LVL,
               "writing literal: 0x%02x (%u left, unpadded run val: %u)",
               byte_buffer, run_count, unpadded_written );
            buffer_out[out_byte_cur++] = byte_buffer;

            if( 0 == run_count ) {
               if( 0 != unpadded_written % 4 ) {
                  /* Uneven number of literals copied. */
                  /* Ignore the byte, as it's a pad to word-size/16-bits. */
                  debug_printf( MFMT_TRACE_RLE_LVL,
                     "unpadded: %u, next is pad byte", unpadded_written );
                  /* assert( 0 == byte_buffer ); */
                  mfmt_decode_rle_state( MFMT_RLE_DECODE_LITERAL_PAD );
               } else {

                  /* Diversion over, go back to hunting for runs. */
                  mfmt_decode_rle_state( MFMT_RLE_DECODE_RUN );
               }
            }
            break;

         case MFMT_RLE_DECODE_ESC:
            run_count = byte_buffer;
            unpadded_written = 0;
            debug_printf( MFMT_TRACE_RLE_LVL,
               "literal mode: %u nibbles", run_count );
            assert( 0 == run_count % 2 );
            mfmt_decode_rle_state( MFMT_RLE_DECODE_LITERAL );
            break;

         case MFMT_RLE_DECODE_ABS_RIGHT:
            debug_printf( MFMT_TRACE_RLE_LVL, "absolute mode: up" );
            /* TODO: Absolute mode. */
            assert( 1 == 0 );
            mfmt_decode_rle_state( MFMT_RLE_DECODE_ABS_DOWN );
            break;

         case MFMT_RLE_DECODE_RUN:

            mfmt_decode_rle_check_eol();

            run_count = byte_buffer;
            debug_printf( MFMT_TRACE_RLE_LVL,
               "starting run: %u nibbles", run_count );
            mfmt_decode_rle_state( MFMT_RLE_DECODE_CHAR );
            break;

         case MFMT_RLE_DECODE_CHAR:
            assert( 0 != run_count );
            run_char = byte_buffer;
            debug_printf( MFMT_TRACE_RLE_LVL,
               "%u-long run of 0x%02x...", run_count, run_char );
            do {
               /* Expand the run into the prescribed number of nibbles. */
               debug_printf( MFMT_TRACE_RLE_LVL,
                  "writing 0x%02x & 0x%02x #%u (line px #%u)...",
                  run_char, out_mask_cur, run_count, line_px_written );
               buffer_out[out_byte_cur] |= (run_char & out_mask_cur);
               mfmt_decode_rle_advance_mask();
               mfmt_decode_rle_inc_line_w( 1 );
               run_count--;

            } while( 0 < run_count );

            /* Diversion over, go back to hunting for runs. */
            mfmt_decode_rle_state( MFMT_RLE_DECODE_RUN );
            break;

         }
         break;
      }
   } while( in_byte_cur < file_sz );

   debug_printf(
      MFMT_TRACE_RLE_LVL, "wrote " OFF_T_FMT " bytes (%u lines)",
      out_byte_cur, lines_out );

cleanup:

   if( NULL != buffer_out ) {
      maug_munlock( buffer_out_h, buffer_out );
   }
   
   return retval;
}

MERROR_RETVAL mfmt_read_bmp_header(
   struct MFMT_STRUCT* header, mfile_t* p_file_in,
   uint32_t file_offset, off_t file_sz, uint8_t* p_flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MFMT_STRUCT_BMPINFO* header_bmp_info = NULL;
   struct MFMT_STRUCT_BMPFILE* header_bmp_file = NULL;
   uint32_t file_hdr_sz = 0;
   off_t header_offset = 0;

   mfmt_bmp_check_header();

   if( NULL != header_bmp_file ) {
      header_offset = 14; /* Size of info header. */

      /* Grab file header info. */
      retval = p_file_in->seek( p_file_in, file_offset + 2 );
      maug_cleanup_if_not_ok();
      retval = p_file_in->read_int( p_file_in,
         (uint8_t*)&(header_bmp_file->file_sz), 4, MFILE_READ_FLAG_LSBF );
      maug_cleanup_if_not_ok();

      retval = p_file_in->seek( p_file_in, file_offset + 10 );
      maug_cleanup_if_not_ok();
      retval = p_file_in->read_int( p_file_in,
         (uint8_t*)&(header_bmp_file->px_offset), 4, MFILE_READ_FLAG_LSBF );
      maug_cleanup_if_not_ok();
      
      debug_printf( MFMT_TRACE_BMP_LVL,
         "bitmap file " UPRINTF_U32_FMT " bytes long, px at "
            UPRINTF_U32_FMT " bytes",
         header_bmp_file->file_sz, header_bmp_file->px_offset );
   }

   /* Read the bitmap image header. */
   retval = p_file_in->seek( p_file_in, file_offset + header_offset );
   maug_cleanup_if_not_ok();
   retval = p_file_in->read_int( p_file_in,
      (uint8_t*)&file_hdr_sz, 4, MFILE_READ_FLAG_LSBF );
   maug_cleanup_if_not_ok();
   if( 40 != file_hdr_sz ) { /* Windows BMP. */
      error_printf( "invalid header size: " UPRINTF_U32_FMT, file_hdr_sz );
      retval = MERROR_FILE;
      goto cleanup;
   }
   debug_printf(
      MFMT_TRACE_BMP_LVL, "bitmap header is " UPRINTF_U32_FMT " bytes",
      file_hdr_sz );

   if( 40 > file_sz - (file_offset + header_offset) ) {
      error_printf(
         "bitmap header overflow! (only " OFF_T_FMT " bytes remain!)",
         file_sz - (file_offset + header_offset) );
      retval = MERROR_OVERFLOW;
      goto cleanup;
   }

   /* Read bitmap image dimensions. */
   retval = p_file_in->seek( p_file_in,
      file_offset + header_offset + MFMT_BMPINFO_OFS_WIDTH );
   maug_cleanup_if_not_ok();
   retval = p_file_in->read_int( p_file_in,
      (uint8_t*)&(header_bmp_info->width), 4, MFILE_READ_FLAG_LSBF );
   maug_cleanup_if_not_ok();

   retval = p_file_in->seek( p_file_in,
      file_offset + header_offset + MFMT_BMPINFO_OFS_HEIGHT );
   maug_cleanup_if_not_ok();
   retval = p_file_in->read_int( p_file_in,
      (uint8_t*)&(header_bmp_info->height), 4, MFILE_READ_FLAG_LSBF );
   maug_cleanup_if_not_ok();

   if( 0 > header_bmp_info->height ) {
      debug_printf(
         MFMT_TRACE_BMP_LVL, "bitmap Y coordinate is inverted..." );
      *p_flags |= MFMT_PX_FLAG_INVERT_Y;
   }

   retval = p_file_in->seek( p_file_in,
      file_offset + header_offset + MFMT_BMPINFO_OFS_SZ );
   maug_cleanup_if_not_ok();
   retval = p_file_in->read_int( p_file_in,
      (uint8_t*)&(header_bmp_info->img_sz), 4, MFILE_READ_FLAG_LSBF );
   maug_cleanup_if_not_ok();

   /* Check that we're a palettized image. */
   retval = p_file_in->seek( p_file_in,
      file_offset + header_offset + MFMT_BMPINFO_OFS_BPP );
   maug_cleanup_if_not_ok();
   retval = p_file_in->read_int( p_file_in,
      (uint8_t*)&(header_bmp_info->bpp), 2, MFILE_READ_FLAG_LSBF );
   maug_cleanup_if_not_ok();

   if( 8 < header_bmp_info->bpp ) {
      error_printf( "invalid bitmap bpp: %u", header_bmp_info->bpp );
      retval = MERROR_FILE;
      goto cleanup;
   }

   /* Make sure there's no weird compression. */
   retval = p_file_in->seek( p_file_in,
      file_offset + header_offset + MFMT_BMPINFO_OFS_COMPRESSION );
   maug_cleanup_if_not_ok();
   retval = p_file_in->read_int( p_file_in,
      (uint8_t*)&(header_bmp_info->compression), 4, MFILE_READ_FLAG_LSBF );
   maug_cleanup_if_not_ok();

   if( 
      MFMT_BMP_COMPRESSION_NONE != header_bmp_info->compression &&
      MFMT_BMP_COMPRESSION_RLE4 != header_bmp_info->compression
   ) {
      error_printf( "invalid bitmap compression: " UPRINTF_U32_FMT,
         header_bmp_info->compression );
      retval = MERROR_FILE;
      goto cleanup;
   }

   /* Get the number of palette colors. */

   retval = p_file_in->seek( p_file_in,
      file_offset + header_offset + MFMT_BMPINFO_OFS_PAL_SZ );
   maug_cleanup_if_not_ok();
   retval = p_file_in->read_int( p_file_in,
      (uint8_t*)&(header_bmp_info->palette_ncolors), 4, MFILE_READ_FLAG_LSBF );
   maug_cleanup_if_not_ok();

   debug_printf( 2, "bitmap is " UPRINTF_S32_FMT " x " UPRINTF_S32_FMT
      ", %u bpp (palette has " UPRINTF_U32_FMT " colors)",
      header_bmp_info->width, header_bmp_info->height,
      header_bmp_info->bpp, header_bmp_info->palette_ncolors );

cleanup:

   return retval;
}

MERROR_RETVAL mfmt_read_bmp_palette(
   struct MFMT_STRUCT* header, uint32_t* palette, size_t palette_sz,
   mfile_t* p_file_in, uint32_t file_offset, off_t file_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MFMT_STRUCT_BMPINFO* header_bmp_info = NULL;
   struct MFMT_STRUCT_BMPFILE* header_bmp_file = NULL;
   off_t i = 0;

   mfmt_bmp_check_header();
 
   retval = p_file_in->seek( p_file_in, file_offset );
   maug_cleanup_if_not_ok();
   for( i = 0 ; header_bmp_info->palette_ncolors > i ; i++ ) {
      if( i * 4 > palette_sz ) {
         error_printf( "palette overflow!" );
         retval = MERROR_OVERFLOW;
         goto cleanup;
      }

      retval = p_file_in->read_int( p_file_in,
         (uint8_t*)&(palette[i]), 4, MFILE_READ_FLAG_LSBF );
      maug_cleanup_if_not_ok();

      debug_printf( MFMT_TRACE_BMP_LVL,
         "set palette entry " OFF_T_FMT " to " UPRINTF_X32_FMT,
         i, palette[i] );
   }

cleanup:

   return retval;
}

MERROR_RETVAL mfmt_read_bmp_px(
   struct MFMT_STRUCT* header, uint8_t SEG_FAR* px, off_t px_sz,
   mfile_t* p_file_in, uint32_t file_offset, off_t file_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MFMT_STRUCT_BMPINFO* header_bmp_info = NULL;
   struct MFMT_STRUCT_BMPFILE* header_bmp_file = NULL;
   int32_t x = 0,
      y = 0;
   off_t i = 0,
      byte_in_idx = 0,
      byte_out_idx = 0,
      bit_idx = 0;
   uint8_t byte_buffer = 0,
      byte_mask = 0,
      pixel_buffer = 0;
   MAUG_MHANDLE decomp_buffer_h = (MAUG_MHANDLE)NULL;
   mfile_t file_decomp;
   mfile_t *p_file_bmp = p_file_in;

   /* Check header for validation and info on how to decode pixels. */

   mfmt_bmp_check_header();
  
   if( 0 == header_bmp_info->height ) {
      error_printf( "bitmap height is 0!" );
      retval = MERROR_FILE;
      goto cleanup;
   }
 
   if( 0 == header_bmp_info->width ) {
      error_printf( "bitmap width is 0!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

   if( 0 == header_bmp_info->bpp ) {
      error_printf( "bitmap BPP is 0!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

   if( 8 < header_bmp_info->bpp ) {
      error_printf( ">8BPP bitmaps not supported!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

   maug_mzero( &file_decomp, sizeof( mfile_t ) );
   if( MFMT_BMP_COMPRESSION_RLE4 == header_bmp_info->compression ) {
      debug_printf( 1, "allocating decompression buffer..." );

      /* Create a temporary memory buffer and decompress into it. */
      decomp_buffer_h = maug_malloc(
         header_bmp_info->width, header_bmp_info->height );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, decomp_buffer_h );

      retval = mfmt_decode_rle(
         p_file_in, file_offset, header_bmp_info->img_sz,
         header_bmp_info->width,
         decomp_buffer_h, header_bmp_info->width * header_bmp_info->height,
         MFMT_DECOMP_FLAG_4BIT );
      maug_cleanup_if_not_ok();

      retval = mfile_lock_buffer(
         decomp_buffer_h,  header_bmp_info->width * header_bmp_info->height,
         &file_decomp );
      maug_cleanup_if_not_ok();

      /* Switch out the file used below for the decomp buffer mfile_t. */
      p_file_bmp = &file_decomp;
   }

   /* TODO: Handle padding for non-conforming images. */
   assert( 0 == header_bmp_info->width % 4 );

   #define mfmt_read_bmp_px_out_idx() \
      (MFMT_PX_FLAG_INVERT_Y == (MFMT_PX_FLAG_INVERT_Y & flags) ? \
      ((header_bmp_info->height - y - 1) * header_bmp_info->width) : \
      ((y) * header_bmp_info->width))

   y = header_bmp_info->height - 1;
   byte_out_idx = mfmt_read_bmp_px_out_idx();
   if( p_file_bmp == p_file_in ) {
      /* Only seek to offset if we're using the original (not translated from
       * RLE or something.
       */
      p_file_bmp->seek( p_file_bmp, file_offset );
   }
   while( 0 <= y ) {
      /* Each iteration is a single, fresh pixel. */
      pixel_buffer = 0;

      debug_printf( MFMT_TRACE_BMP_LVL,
         "byte in: " OFF_T_FMT " (" OFF_T_FMT 
            "), bit " OFF_T_FMT ", y: " UPRINTF_U32_FMT
            ", x: " UPRINTF_U32_FMT "), byte out: " OFF_T_FMT,
         byte_in_idx, file_sz, bit_idx, y, x, byte_out_idx );

      /* Buffer bounds check. */
      if( px_sz <= byte_out_idx ) {
         error_printf(
            "byte " OFF_T_FMT " outside of " OFF_T_FMT
            " pixel buffer!", byte_out_idx, px_sz );
         retval = MERROR_OVERFLOW;
         goto cleanup;
      }

      /* Byte finished check. */
      if( 0 == bit_idx ) {
         if( byte_in_idx >= file_sz ) {
            /* TODO: Figure out why ICO parser messes up size. */
            error_printf(
               "input bitmap has insufficient size " OFF_T_FMT " bytes)!",
               file_sz );
            /* retval = MERROR_OVERFLOW;
            goto cleanup; */
         }

         /* Move on to a new byte. */
         /* TODO: Bad cursor? */
         retval = p_file_bmp->read_int( p_file_bmp, &byte_buffer, 1, 0 );
         maug_cleanup_if_not_ok();
         /*
         mfile_cread( p_file_bmp, &(byte_buffer) );
         */
         byte_in_idx++;

         /* Start at 8 bits from the right (0 from the left). */
         bit_idx = 8;

         /* Build a bitwise mask based on the bitmap's BPP. */
         byte_mask = 0;
         for( i = 0 ; header_bmp_info->bpp > i ; i++ ) {
            byte_mask >>= 1;
            byte_mask |= 0x80;
         }
      }

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
      debug_printf( MFMT_TRACE_BMP_LVL,
         "byte_mask: 0x%02x, bit_idx: " OFF_T_FMT
            ", pixel_buffer: 0x%02x",
         byte_mask, bit_idx, pixel_buffer );

      /* Place the pixel buffer at the X/Y in the grid. */
      debug_printf( MFMT_TRACE_BMP_LVL,
         "writing byte " OFF_T_FMT " (x: " UPRINTF_S32_FMT
         ", y: " UPRINTF_S32_FMT ")",
         byte_out_idx, x, y );
      px[byte_out_idx] = pixel_buffer;
      byte_out_idx++;

      /* Increment the bits position byte mask by the bpp so it's pointing
       * to the next pixel in the bitmap for the next go around.
       */
      byte_mask >>= header_bmp_info->bpp;
      bit_idx -= header_bmp_info->bpp;

      /* Move to the next pixel. */
      x++;
      if( x >= header_bmp_info->width ) {
         /* Move to the next row of the input. */
         y--;
         x = 0;
         while( byte_in_idx % 4 != 0 ) {
            byte_in_idx++;
            p_file_bmp->seek( p_file_bmp, file_offset + byte_in_idx );
         }

         /* Move to the next row of the output. */
         byte_out_idx = mfmt_read_bmp_px_out_idx();

         /* TODO Get past the padding. */

         debug_printf( MFMT_TRACE_BMP_LVL,
            "new row starting at byte_out_idx: " OFF_T_FMT,
            byte_out_idx );
      }
   }

cleanup:

   mfile_close( &file_decomp );
   /* decomp_buffer_h = file_decomp.h.mem; */

   if( NULL != decomp_buffer_h ) {
      debug_printf( 1, "freeing decomp buffer %p...", decomp_buffer_h );
      maug_mfree( decomp_buffer_h );
   }

   return retval;
}

#endif /* MFMT_C */

/*! \} */ /* maug_fmt */

#endif /* !MFMT_H */

