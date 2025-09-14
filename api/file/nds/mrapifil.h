
#if !defined( MAUG_API_FIL_H_DEFS )
#define MAUG_API_FIL_H_DEFS

union MFILE_HANDLE {
   MAUG_MHANDLE mem;
};

#elif defined( MFILE_C )

#ifdef MVFS_ENABLED
/* Load the MVFS assets from the include dir specified in the Makefile
 * (probably via Makevfs.inc). These can be used as a backup for NDSASSET. */
#  include <mvfs.h>
#endif /* MVFS_ENABLED */

/* TODO: Use libfat to read files from SD card if not in MVFS. */

off_t mfile_file_has_bytes( struct MFILE_CADDY* p_file ) {
   
   /* TODO */

   return 0;
}

/* === */

MERROR_RETVAL mfile_file_read_byte( struct MFILE_CADDY* p_file, uint8_t* buf ) {
   return mfile_file_read_block( p_file, buf, 1 );
}

/* === */

MERROR_RETVAL mfile_file_read_block(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   /* TODO */

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   /* TODO */

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_read_line(
   struct MFILE_CADDY* p_file, char* buffer, off_t buffer_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   /* TODO */

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

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_init() {
   return MERROR_OK;
}

/* === */

MERROR_RETVAL mfile_plt_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   while( NULL != gc_mvfs_data[i] ) {
      if( 0 == strcmp( filename, gc_mvfs_filenames[i] ) ) {
         debug_printf( MFILE_TRACE_LVL,
            "found filename \"%s\" at VFS index: " SIZE_T_FMT
            " (size: " OFF_T_FMT " bytes)",
               filename, i, *(gc_mvfs_lens[i]) );
         break;
      }
      i++;
   }

   if( NULL == gc_mvfs_data[i] ) {
      retval = MERROR_FILE;
      error_printf( "file \"%s\" not readable in VFS!", filename );
      goto cleanup;
   }

   p_file->type = MFILE_CADDY_TYPE_MEM_BUFFER;

   p_file->has_bytes = mfile_mem_has_bytes;
   p_file->read_byte = mfile_mem_read_byte;
   p_file->read_block = mfile_mem_read_block;
   p_file->read_int = mfile_file_read_int;
   p_file->seek = mfile_mem_seek;
   p_file->read_line = mfile_mem_read_line;
   p_file->printf = mfile_file_printf;
   p_file->vprintf = mfile_mem_vprintf;
   p_file->write_block = mfile_file_write_block;

   p_file->flags = MFILE_FLAG_READ_ONLY;
   p_file->mem_buffer = gc_mvfs_data[i];
   p_file->sz = *(gc_mvfs_lens[i]);
   p_file->mem_cursor = 0;

cleanup:

   return retval;
}

/* === */
 
MERROR_RETVAL mfile_plt_open_write( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_FILE;
   return retval;
}

/* === */

void mfile_plt_close( mfile_t* p_file ) {
   /* TODO */
}

#endif /* !MAUG_API_FIL_H_DEFS */

