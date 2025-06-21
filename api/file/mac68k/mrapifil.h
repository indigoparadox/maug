
#if !defined( MAUG_API_FIL_H_DEFS )
#define MAUG_API_FIL_H_DEFS

#include <MacTypes.h>
#include <Files.h>

union MFILE_HANDLE {
   int16_t file_ref;
   int32_t file_pos;
   MAUG_MHANDLE mem;
};

#  define mfile_has_bytes( p_file ) \
      ((MFILE_CADDY_TYPE_FILE == ((p_file)->type) ? \
         (noErr != GetFPos( (p_file)->h.file_ref, &((p_file)->h.file_pos) ) ? \
            (p_file)->h.file_pos : 0 ) : \
         (p_file)->mem_cursor) < (p_file)->sz)

#elif defined( MFILE_C )

MERROR_RETVAL mfile_file_read_int(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

#if 0
   ssize_t last_read = 0;
   if( MFILE_READ_FLAG_LSBF == (MFILE_READ_FLAG_LSBF & flags) ) {
      /* Shrink the buffer moving right and read into it. */
      last_read = fread( buf, 1, buf_sz, p_file->h.file );
      if( buf_sz > last_read ) {
         error_printf( "unable to read from file!" );
         retval = MERROR_FILE;
         goto cleanup;
      }
   
   } else {
      /* Move to the end of the output buffer and read backwards. */
      while( 0 < buf_sz ) {
         last_read = fread( (buf + (buf_sz - 1)), 1, 1, p_file->h.file );
         if( 0 >= last_read ) {
            error_printf( "unable to read from file!" );
            retval = MERROR_FILE;
            goto cleanup;
         }
         buf_sz--;
      }
   }

cleanup:
#endif

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

#if 0
   if( fseek( p_file->h.file, pos, SEEK_SET ) ) {
      error_printf( "unable to seek file!" );
      retval = MERROR_FILE;
   }
#endif

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_read_line(
   struct MFILE_CADDY* p_f, char* buffer, off_t buffer_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_f->type );

#if 0
   /* Trivial case; use a native function. Much faster! */
   if( NULL == fgets( buffer, buffer_sz - 1, p_f->h.file ) ) {
      error_printf( "error while reading line from file!" );
      retval = MERROR_FILE;
   }
#endif

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
   MERROR_RETVAL tok_retval = MERROR_OK;
   struct stat file_stat;
   int16_t vol_ref = 0;
   size_t dir_idx = 0;
   char dir_name[MAUG_PATH_MAX + 1];
   char pas_dir_name[MAUG_PATH_MAX + 2];
   CInfoPBRec pb;
   OSErr pb_err;
   int32_t last_dir_id = 0;
   Str255 vol_name;

   /* Get current working directory. */
   GetVol( vol_name, &vol_ref );
   GetWDInfo( vol_ref, &vol_ref, &last_dir_id, nil );

   while( MERROR_OK == tok_retval ) {
      maug_mzero( dir_name, MAUG_PATH_MAX + 1 );
      maug_mzero( pas_dir_name, MAUG_PATH_MAX + 2 );

      tok_retval = maug_tok_str(
         dir_idx, filename, strlen( filename ), dir_name, MAUG_PATH_MAX, "/" );

      if( MERROR_OK == tok_retval ) {
         debug_printf( 1, "dir: %s", dir_name );

         retval = maug_str_c2p( dir_name, pas_dir_name, MAUG_PATH_MAX + 1 );
         maug_cleanup_if_not_ok_msg( "filename string too long!" );

         /* Find the subdirectory. */
         maug_mzero( &pb, sizeof( CInfoPBRec ) );
         pb.hFileInfo.ioNamePtr = (StringPtr)pas_dir_name;
         pb.hFileInfo.ioVRefNum = vol_ref;
         pb.hFileInfo.ioFDirIndex = 0; /* Search by name. */
         pb.hFileInfo.ioDirID = last_dir_id;
         pb_err = PBGetCatInfoSync( &pb );

         if( noErr != pb_err ) {
            /* Whatever this is, it's not a directory. */
            error_printf( "unable to open directory: %s", dir_name );
            break;
         }

         debug_printf(
            1, "opened dir: %s (" S32_FMT ")", dir_name, last_dir_id );

         dir_idx++;
      }
   }

   /* Open file from the directory. */
   pb_err = FSOpen(
      (StringPtr)pas_dir_name, last_dir_id, &(p_file->h.file_ref) );
   if( noErr != pb_err ) {
      error_printf(
         "unable to open file \"%s\" (%u): %d",
         &(pas_dir_name[1]), *((uint8_t*)pas_dir_name), pb_err );
      retval = MERROR_FILE;
      goto cleanup;
   }

   debug_printf( 1, "opened file %s (" OFF_T_FMT " bytes)...",
      filename, p_file->sz );

   p_file->type = MFILE_CADDY_TYPE_FILE;

   p_file->read_int = mfile_file_read_int;
   p_file->seek = mfile_file_seek;
   p_file->read_line = mfile_file_read_line;
   p_file->flags = MFILE_FLAG_READ_ONLY;

cleanup:

   return retval;
}

/* === */

void mfile_plt_close( mfile_t* p_file ) {
   FSClose( p_file->h.file_ref );
}

#endif /* !MAUG_API_FIL_H_DEFS */

