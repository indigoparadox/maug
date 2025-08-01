
#if !defined( MAUG_API_FIL_H_DEFS )
#define MAUG_API_FIL_H_DEFS

#include <MacTypes.h>
#include <Files.h>
#include <OSUtils.h>
#include <Devices.h>
#include <Errors.h>

union MFILE_HANDLE {
   int16_t file_ref;
   MAUG_MHANDLE mem;
};

#  ifndef MFILE_LINE_BUF_SZ
#     define MFILE_LINE_BUF_SZ 1024
#  endif /* !MFILE_LINE_BUF_SZ */

#elif defined( MFILE_C )

off_t mfile_file_has_bytes( struct MFILE_CADDY* p_file ) {
   int32_t cursor = 0;
   OSErr err;

   err = GetFPos( p_file->h.file_ref, &cursor );
   if( noErr != err ) {
      debug_printf( MFILE_TRACE_LVL, "file has error bytes left!" );
      return 0;
   }

   /*
   debug_printf( MFILE_TRACE_LVL, "file has " S32_FMT " bytes left...",
      p_file->sz - cursor );
   */

   return p_file->sz - cursor;
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
   int32_t count = buf_sz;
   OSErr err;

   err = FSRead( p_file->h.file_ref, &count, buf );
   if( 0 == count || noErr != err ) {
      error_printf( "unable to read from file!" );
      retval = MERROR_FILE;
   }

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;
   OSErr err;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   err = SetFPos( p_file->h.file_ref, fsFromStart, pos );
   if( noErr != err ) {
      error_printf( "unable to seek file!" );
      retval = MERROR_FILE;
   }

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_read_line(
   struct MFILE_CADDY* p_f, char* buffer, off_t buffer_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t buf_i = 0;
   OSErr err;
   int32_t count = 1;

   assert( MFILE_CADDY_TYPE_FILE == p_f->type );

   maug_mzero( buffer, buffer_sz );

   while( buf_i + 1 < buffer_sz ) {
      err = FSRead( p_f->h.file_ref, &count, &(buffer[buf_i]) );
      if( '\n' == buffer[buf_i] || '\r' == buffer[buf_i] ) {
         debug_printf( MFILE_TRACE_CONTENTS_LVL,
            "read line (" SIZE_T_FMT " chars): %s", buf_i, buffer );
         goto cleanup;
      }
      if( noErr != err ) {
         error_printf( "error reading file: %d", err );
         retval = MERROR_FILE;
         goto cleanup;
      }
      if( 0 == count ) {
         error_printf( "unable to read from file!" );
         retval = MERROR_FILE;
         goto cleanup;
      }
      buf_i++;
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_vprintf(
   mfile_t* p_file, uint8_t flags, const char* fmt, va_list args
) {
   MERROR_RETVAL retval = MERROR_OK;
   char line_buf[MFILE_LINE_BUF_SZ + 1];
   ParamBlockRec pb;

   if( MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & p_file->flags) ) {
      return MERROR_FILE;
   }

   maug_vsnprintf( line_buf, MFILE_LINE_BUF_SZ, fmt, args );
   strcat( line_buf, "\r" );

   pb.ioParam.ioRefNum = p_file->h.file_ref;
   pb.ioParam.ioBuffer = line_buf;
   pb.ioParam.ioReqCount = maug_strlen( line_buf );
   pb.ioParam.ioPosMode = fsAtMark;
   pb.ioParam.ioPosOffset = 0;

   PBWrite( &pb, false );

   return retval;
}

/* === */

MERROR_RETVAL _mfile_plt_open(
   uint8_t flags, const char* filename, mfile_t* p_file
) {
   MERROR_RETVAL retval = MERROR_OK;
   MERROR_RETVAL tok_retval = MERROR_OK;
   int16_t vol_ref = 0;
   size_t dir_idx = 0;
   char dir_name[MAUG_PATH_SZ_MAX + 1];
   char pas_dir_name[MAUG_PATH_SZ_MAX + 2];
   CInfoPBRec pb;
   OSErr pb_err;
   int32_t last_dir_id = 0,
      iter_id = 0;
   Str255 vol_name;

   /* Get current working directory. */
   GetVol( vol_name, &vol_ref );
   GetWDInfo( vol_ref, &vol_ref, &iter_id, nil );
   debug_printf( MFILE_TRACE_LVL, "vol: %s", &(vol_name[1]) );

   /* Iterate subdirectories from the current working directory by breaking
    * the path up by '/' separators.
    */
   while( MERROR_OK == tok_retval ) {
      maug_mzero( dir_name, MAUG_PATH_SZ_MAX + 1 );
      maug_mzero( pas_dir_name, MAUG_PATH_SZ_MAX + 2 );

      /* Get the current path token. */
      tok_retval = maug_tok_str(
         dir_idx, filename,
         maug_strlen( filename ), dir_name, MAUG_PATH_SZ_MAX, "/" );

      if( MERROR_OK == tok_retval ) {
         debug_printf( MFILE_TRACE_LVL, "dir: %s", dir_name );

         /* Translate the path token into a pascal string. */
         retval = maug_str_c2p( dir_name, pas_dir_name, MAUG_PATH_SZ_MAX + 1 );
         maug_cleanup_if_not_ok_msg( "filename string too long!" );

         /* Get info for item represented by path token. */
         maug_mzero( &pb, sizeof( CInfoPBRec ) );
         pb.hFileInfo.ioNamePtr = (StringPtr)pas_dir_name;
         pb.hFileInfo.ioVRefNum = vol_ref;
         pb.hFileInfo.ioFDirIndex = 0; /* Search by name. */
         pb.hFileInfo.ioDirID = iter_id;
         pb_err = PBGetCatInfoSync( &pb );

         if( noErr != pb_err ) {
            /* Whatever this is, it's not a directory. */
            error_printf( "unable to open path item: %s", dir_name );
            retval = MERROR_FILE;
            goto cleanup;
         }

         /* Stow the last directory for later and keep going. */
         last_dir_id = iter_id;
         iter_id = pb.hFileInfo.ioDirID;

         debug_printf( MFILE_TRACE_LVL,
            "opened item: %s (id: " S32_FMT ")", dir_name, iter_id );

         /* Figure out if this is a subdirectory or a file that can't be
          * traversed any further.
          */
         if( !(pb.hFileInfo.ioFlAttrib & ioDirMask) ) {
            debug_printf( MFILE_TRACE_LVL, "item is a file!" );
            break;
         }

         dir_idx++;
      }
   }

   /* Open file with name from the last *directory* found in the path above. */
   pb_err = HOpen(
      vol_ref, last_dir_id, (StringPtr)pas_dir_name,
      MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & flags) ?
         fsRdPerm : fsWrPerm, &(p_file->h.file_ref) );
   if( noErr != pb_err ) {
      error_printf(
         "unable to open file \"%s\" (len: %u, dir_id: " S32_FMT "): %d",
         &(pas_dir_name[1]), *((uint8_t*)pas_dir_name), last_dir_id, pb_err );
      retval = MERROR_FILE;
      goto cleanup;
   }

   /* pb was setup by the traversal above, which should've hit this file last
    * (even if it aborted once it determined it wasn't a directory)!
    */
   p_file->sz = pb.hFileInfo.ioFlLgLen;

   debug_printf( MFILE_TRACE_LVL, "opened file %s (sz: " OFF_T_FMT " bytes)...",
      filename, p_file->sz );

   p_file->type = MFILE_CADDY_TYPE_FILE;

   p_file->has_bytes = mfile_file_has_bytes;
   p_file->read_byte = mfile_file_read_byte;
   p_file->read_block = mfile_file_read_block;
   p_file->read_int = mfile_file_read_int;
   p_file->seek = mfile_file_seek;
   p_file->read_line = mfile_file_read_line;
   p_file->printf = mfile_file_printf;
   p_file->vprintf = mfile_file_vprintf;

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;

   retval = _mfile_plt_open( MFILE_FLAG_READ_ONLY, filename, p_file );
   maug_cleanup_if_not_ok();

   p_file->flags = MFILE_FLAG_READ_ONLY;

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_open_write( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;

   retval = _mfile_plt_open( 0, filename, p_file );
   maug_cleanup_if_not_ok();

cleanup:

   return retval;
}

/* === */

void mfile_plt_close( mfile_t* p_file ) {
   FSClose( p_file->h.file_ref );
}

#endif /* !MAUG_API_FIL_H_DEFS */

