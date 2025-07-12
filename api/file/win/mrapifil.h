
#if !defined( MAUG_API_FIL_H_DEFS )
#define MAUG_API_FIL_H_DEFS

union MFILE_HANDLE {
   HANDLE handle;
   MAUG_MHANDLE mem;
};

#  ifndef MFILE_READ_LINE_BUF_SZ
#     define MFILE_READ_LINE_BUF_SZ 4096
#  endif /* !MFILE_READ_LINE_BUF_SZ */

#elif defined( MFILE_C )

off_t mfile_file_has_bytes( struct MFILE_CADDY* p_file ) {
   off_t cursor = 0;

   cursor = (off_t)SetFilePointer( p_file->h.handle, 0, NULL, FILE_CURRENT );

   if( 0 <= cursor ) {
      debug_printf( MFILE_TRACE_LVL, "file has " OFF_T_FMT " bytes left...",
         p_file->sz - cursor );
      return p_file->sz - cursor;
   } else {
      debug_printf( MFILE_TRACE_LVL, "file has error bytes left!" );
      return 0;
   }
}

/* === */

MERROR_RETVAL mfile_file_read_byte( struct MFILE_CADDY* p_f, uint8_t* buf ) {
   return mfile_file_read_block( p_f, buf, 1 );
}

/* === */


MERROR_RETVAL mfile_file_read_block(
   struct MFILE_CADDY* p_f, uint8_t* buf, size_t buf_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   DWORD last_read = 0;
   BOOL read_ok = 0;

   read_ok = ReadFile( p_f->h.handle, buf, buf_sz, &last_read, NULL );
   if( !read_ok || 1 > last_read ) {
      error_printf( "unable to read from file!" );
      retval = MERROR_FILE;
   }

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   SetFilePointer( (p_file)->h.handle, pos, NULL, FILE_BEGIN );

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_read_line(
   struct MFILE_CADDY* p_f, char* buffer, off_t buffer_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   DWORD start = 0;
   DWORD chunk_bytes_read = 0;
   DWORD line_bytes_read = 0;
   int32_t newline_diff = 0;
   char line_buf[MFILE_READ_LINE_BUF_SZ + 1];
   char* newline_ptr = NULL;

   assert( MFILE_CADDY_TYPE_FILE == p_f->type );

   start = SetFilePointer( p_f->h.handle, 0, NULL, FILE_CURRENT );

   while( ReadFile(
      p_f->h.handle, line_buf, MFILE_READ_LINE_BUF_SZ, &chunk_bytes_read, NULL
   ) ) {
      /* debug_printf( 1, "---" );
      debug_printf( 1, "chunk read: %u bytes: %s", chunk_bytes_read, line_buf ); */
      line_buf[chunk_bytes_read] = '\0';
      newline_ptr = maug_strchr( line_buf, '\n' );
      if( NULL != newline_ptr ) {
         /* Move the read counter back in the chunk to the newline. */
         newline_diff = (newline_ptr - line_buf) + 1;
         *newline_ptr = '\0';
         /* debug_printf( 1, "len to newline is: %u bytes...", newline_diff ); */
         /* Back up the file pointer to the end of the line we just read and
          * continue with the length of that line.
          */
         SetFilePointer(
            p_f->h.handle,
            (chunk_bytes_read - newline_diff) * -1, NULL, FILE_CURRENT );
         /* debug_printf( 1, "backing up file ptr by %u bytes (sfp: %d)...",
            chunk_bytes_read - newline_diff, sfp ); */
         chunk_bytes_read = newline_diff;
      }
      debug_printf( 1, "line buffer (%u): %s", chunk_bytes_read, line_buf );
      if( line_bytes_read + chunk_bytes_read > buffer_sz ) {
         error_printf( "line buffer exceeded!" );
         retval = MERROR_FILE;
         goto cleanup;
      }
      strncpy(
         &(buffer[line_bytes_read]), line_buf, chunk_bytes_read );
      if( NULL != newline_ptr ) {
         /* Line was completed. */
         break;
      }
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( RETROFLAT_API_WINCE )
   STATSTG file_stat;
#  endif /* RETROFLAT_API_WINCE */
#  ifdef MAUG_WCHAR
   wchar_t filename_w[MAUG_PATH_MAX + 1] = { 0 };
#  endif /* MAUG_WCHAR */

#     ifdef MAUG_WCHAR
   if( 0 == MultiByteToWideChar(
      CP_ACP, MB_PRECOMPOSED, filename, -1, filename_w, MAUG_PATH_MAX
   ) ) {
      error_printf( "could not create wide filename path!" );
      retval = MERROR_FILE;
      goto cleanup;
   }
#     endif /* MAUG_WCHAR */

   /* Actually open the file. */
   p_file->h.handle = CreateFile( 
#     ifdef MAUG_WCHAR
      filename_w,
#     else
      filename,
#     endif /* MAUG_WCHAR */
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL );

   if( INVALID_HANDLE_VALUE == p_file->h.handle ) {
      error_printf( "could not open file: %s", filename );
   }

   p_file->sz = GetFileSize( p_file->h.handle, NULL );

   debug_printf( 1, "opened file %s (" OFF_T_FMT " bytes)...",
      filename, p_file->sz );

   p_file->type = MFILE_CADDY_TYPE_FILE;

   p_file->has_bytes = mfile_file_has_bytes;
   p_file->read_byte = mfile_file_read_byte;
   p_file->read_int = mfile_file_read_int;
   p_file->seek = mfile_file_seek;
   p_file->read_line = mfile_file_read_line;
   p_file->flags = MFILE_FLAG_READ_ONLY;

cleanup:

   return retval;
}

/* === */

void mfile_plt_close( mfile_t* p_file ) {
   CloseHandle( p_file->h.handle );
}

#endif /* !MAUG_API_FIL_H_DEFS */

