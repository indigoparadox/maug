
#if !defined( MAUG_API_FIL_H_DEFS )
#define MAUG_API_FIL_H_DEFS

#include <psxcd.h>
#include <psxgpu.h>

#define MFILE_PSX_CD_BUSY 0x01

#define MFILE_PSX_SECTOR_SZ 2048

struct  MFILE_CDLFILE {
   CdlFILE cdf;
   __attribute__((aligned(4))) uint8_t sector_buffer[MFILE_PSX_SECTOR_SZ];
   off_t sector_lba_offset;
   off_t cursor;
};

union MFILE_HANDLE {
   MAUG_MHANDLE mem;
   struct MFILE_CDLFILE file;
};

#elif defined( MFILE_C )

MERROR_RETVAL mfile_psx_cache_block(
   struct MFILE_CADDY* p_file, off_t lba_offset
) {
   MERROR_RETVAL retval = MERROR_OK;
   off_t file_plus_lba_offset = 0;
   CdlLOC sector_pos;

   /* cdf.pos is a special time signature for where to find the first sector
    * of the file. Convert it to an integer so we can add the offset LBA to it.
    */
   file_plus_lba_offset = CdPosToInt( &(p_file->h.file.cdf.pos) ) + lba_offset;

   /* Wait for the CD-ROM to be ready if needed. */
   while( MFILE_PSX_CD_BUSY == (MFILE_PSX_CD_BUSY & CdStatus()) ) {
      CdControl( CdlNop, 0, 0 ); /* Refresh CD status. */
#if MFILE_SEEK_TRACE_LVL > 0
      debug_printf(
         MFILE_SEEK_TRACE_LVL, "waiting for CD-ROM (status: 0x%02x)...",
         CdStatus() );
#endif /* MFILE_SEEK_TRACE_LVL */
      CdSync( 0, 0 ); /* Wait for CD-ROM. */
   }

#if MFILE_SEEK_TRACE_LVL > 0
   debug_printf( MFILE_SEEK_TRACE_LVL,
      "attempting to read file %s (%d) lba offset (%d)...",
      p_file->filename, CdPosToInt( &(p_file->h.file.cdf.pos) ), lba_offset );
#endif /* MFILE_SEEK_TRACE_LVL */

   /* Move the CD-ROM laser to the desired sector and copy the sector contents
    * into the sector cache buffer.
    */
   CdIntToPos( file_plus_lba_offset, &sector_pos );
   CdControl( CdlSetloc, &sector_pos, 0 );
   CdRead( 1, (uint32_t*)(p_file->h.file.sector_buffer), CdlModeSpeed );
	if( 0 > CdReadSync( 0, 0 ) ) {
      error_printf( "CD sector read failure on sector: %d",
         file_plus_lba_offset );
      retval = MERROR_FILE;
      goto cleanup;
   }

   /* Make a note of the sector the cache buffer is caching so that we can
    * expire and refill it later if needed.
    */
   p_file->h.file.sector_lba_offset = lba_offset;

cleanup:

   return retval;
}

off_t mfile_file_cursor( struct MFILE_CADDY* p_file ) {
   return p_file->h.file.cursor;
}

/* === */

MERROR_RETVAL mfile_file_read_byte( struct MFILE_CADDY* p_file, uint8_t* buf ) {
   return mfile_file_read_block( p_file, buf, 1 );
}

/* === */

MERROR_RETVAL mfile_file_read_block(
   struct MFILE_CADDY* p_file, uint8_t* buffer, size_t buffer_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   off_t sector_byte_offset = 0,
      copy_sz = 0,
      buffer_i = 0;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   maug_mzero( buffer, buffer_sz );

   while( buffer_i < buffer_sz ) {
      /* Make sure we have the relevant sector to the cursor. */
      if(
         p_file->h.file.cursor / MFILE_PSX_SECTOR_SZ !=
         p_file->h.file.sector_lba_offset
      ) {
#if MFILE_SEEK_TRACE_LVL > 0
         debug_printf( MFILE_SEEK_TRACE_LVL, "caching next block..." );
#endif /* MFILE_SEEK_TRACE_LVL */
         retval = mfile_psx_cache_block(
            p_file, p_file->h.file.cursor / MFILE_PSX_SECTOR_SZ );
         maug_cleanup_if_not_ok();
      }

      /* Read the cached sector into the buffer until the sector is out of
       * bytes or the buffer is full.
       */
      sector_byte_offset = p_file->h.file.cursor % MFILE_PSX_SECTOR_SZ;
      copy_sz = MFILE_PSX_SECTOR_SZ - sector_byte_offset > buffer_sz ?
         buffer_sz : MFILE_PSX_SECTOR_SZ - sector_byte_offset;
      memcpy(
         &(buffer[buffer_i]),
         &(p_file->h.file.sector_buffer[sector_byte_offset]),
         copy_sz );
      p_file->h.file.cursor += copy_sz;
      buffer_i += copy_sz;
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   p_file->h.file.cursor = pos;

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_read_line(
   struct MFILE_CADDY* p_file, char* buffer, off_t buffer_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   off_t sector_byte_offset = 0,
      i = 0,
      buffer_i = 0;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   maug_mzero( buffer, buffer_sz );

   while( buffer_i < buffer_sz ) {
      /*
      debug_printf( 1,
         "cursor: %d, cursor_lba: %d, current_lba: %d",
         p_file->h.file.cursor,
         p_file->h.file.cursor / MFILE_PSX_SECTOR_SZ,
         p_file->h.file.sector_lba_offset ); */
      /* Make sure we have the relevant sector to the cursor. */
      if(
         p_file->h.file.cursor / MFILE_PSX_SECTOR_SZ !=
         p_file->h.file.sector_lba_offset
      ) {
#if MFILE_SEEK_TRACE_LVL > 0
         debug_printf( MFILE_SEEK_TRACE_LVL, "caching next block..." );
#endif /* MFILE_SEEK_TRACE_LVL */
         retval = mfile_psx_cache_block(
            p_file, p_file->h.file.cursor / MFILE_PSX_SECTOR_SZ );
         maug_cleanup_if_not_ok();
      }

      /* Read each byte of the cached sector into the buffer until the sector
       * is out of bytes or the buffer is full.
       *
       * We can't use memcpy here since we're looking for \n bytes!
       */
      sector_byte_offset = p_file->h.file.cursor % MFILE_PSX_SECTOR_SZ;
      for(
         i = sector_byte_offset ;
         MFILE_PSX_SECTOR_SZ > i && buffer_i < buffer_sz ;
         i++
      ) {
         if( '\n' == p_file->h.file.sector_buffer[i] ) {
            /* Found a newline! Line terminated. */
            p_file->h.file.cursor++; /* Account for the newline. */
            goto cleanup;
         }
         buffer[buffer_i++] = p_file->h.file.sector_buffer[i];
         p_file->h.file.cursor++;
      }
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_vprintf(
   mfile_t* p_file, uint8_t flags, const char* fmt, va_list args
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   if( MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & p_file->flags) ) {
      return MERROR_FILE;
   }

   /* TODO */
#  pragma message( "warning: file_vprintf not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_write_block(
   struct MFILE_CADDY* p_f, const uint8_t* buf, size_t buf_sz
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & p_f->flags) ) {
      return MERROR_FILE;
   }

   /* TODO: Implement insert rather than replace like in UNIX mfile API. */
#  pragma message( "warning: file_write_block not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_init() {
   
   /* Initialize graphics to get interrupts going. We can do this again once
    * retro2D inits, but we need interrupts for CD-ROM init to complete!
    */
   ResetGraph( 0 );

   /* Perform actual CD-ROM init. */
   debug_printf( 1, "initializing CD-ROM..." );
   CdInit();
   debug_printf( 1, "CD-ROM initialized!" );

   return MERROR_OK;
}

/* === */

MERROR_RETVAL mfile_plt_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
   maug_path filepath;
   size_t i_in = 0;
   char* filepath_p = (char*)filepath;

   /* Fix up filename for CD-ROM filesystem. */
   maug_mzero( filepath, MAUG_PATH_SZ_MAX );
   *(filepath_p++) = '\\';
   for( i_in = 0 ; strlen( filename ) > i_in ; i_in++ ) {
      if( '/' == filename[i_in] ) {
         /* Replace path separators with backslash. */
         *(filepath_p++) = '\\';
      } else if( 0x61 <= filename[i_in] && 0x7a >= filename[i_in] ) {
         /* Make filename uppercase for CD-ROM filesystem. */
         *(filepath_p++) = filename[i_in] - 0x20;
      } else {
         *(filepath_p++) = filename[i_in];
      }
   }
   /* Add file version. */
   *(filepath_p++) = ';';
   *(filepath_p++) = '1';

   /* Wait for CD-ROM if needed. */
   while( MFILE_PSX_CD_BUSY == (MFILE_PSX_CD_BUSY & CdStatus()) ) {
      CdControl( CdlNop, 0, 0 );
#if MFILE_SEEK_TRACE_LVL > 0
      debug_printf(
         MFILE_SEEK_TRACE_LVL, "waiting for CD-ROM (status: 0x%02x)...",
         CdStatus() );
#endif /* MFILE_SEEK_TRACE_LVL */
      CdSync( 0, 0 );
   }

#if MFILE_SEEK_TRACE_LVL > 0
   debug_printf(
      MFILE_SEEK_TRACE_LVL, "attempting to open file %s...", filepath );
#endif /* MFILE_SEEK_TRACE_LVL */

   /* Attempt to find the file on-disc. */
   if( !CdSearchFile( &(p_file->h.file.cdf), filepath ) ) {
      error_printf( "could not find file: %s", filepath );
      retval = MERROR_FILE;
      goto cleanup;
   }

   p_file->sz = p_file->h.file.cdf.size;

#if MFILE_SEEK_TRACE_LVL > 0
   debug_printf( MFILE_SEEK_TRACE_LVL,
      "file LBA for %s is: %d, size (in bytes) is: %d",
      filepath, CdPosToInt( &(p_file->h.file.cdf.pos) ), p_file->sz );
#endif /* MFILE_SEEK_TRACE_LVL */

   /* Setup file type and function pointers.*/
   p_file->type = MFILE_CADDY_TYPE_FILE;
   p_file->has_bytes = mfile_file_has_bytes;
   p_file->cursor = mfile_file_cursor;
   p_file->read_byte = mfile_file_read_byte;
   p_file->read_int = mfile_file_read_int;
   p_file->read_block = mfile_file_read_block;
   p_file->seek = mfile_file_seek;
   p_file->read_line = mfile_file_read_line;
   p_file->printf = mfile_file_printf;
   p_file->vprintf = mfile_file_vprintf;
   p_file->write_block = mfile_file_write_block;

   /* Read the first sector into the sector buffer. */
   p_file->h.file.cursor = 0;
   retval = mfile_psx_cache_block( p_file, 0 );

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_open_write( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO */
#  pragma message( "warning: open_write not implemented" )

   return retval;
}

/* === */

void mfile_plt_close( mfile_t* p_file ) {
   /* TODO */
#  pragma message( "warning: close not implemented" )
}

#endif /* !MAUG_API_FIL_H_DEFS */

