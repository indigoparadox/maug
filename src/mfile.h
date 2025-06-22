
#ifndef MFILE_H
#define MFILE_H

#if !defined( MFILE_MMAP ) && \
   !defined( RETROFLAT_API_WINCE ) && \
   !defined( RETROFLAT_API_PALM )
#  include <sys/stat.h>
#endif /* !MFILE_MMAP */

/* TODO: async file_open() call that kicks off download to mem buffer that can
 *       be checked with looped check function.
 */

/**
 * \addtogroup maug_mfile RetroFile API
 * \brief Abstraction layer for dealing with files on retro systems.
 * \{
 */

/**
 * \file mfile.h
 */

#ifndef MAUG_PATH_MAX
/*! \brief Maximum size allocated for asset paths. */
#  define MAUG_PATH_MAX 256
#endif /* !MAUG_PATH_MAX */

/**
 * \addtogroup maug_mfile_types RetroFile Types
 * \brief Types of files/data stores that mfile can abstract from.
 *
 * This library is designed with the intention of including various archive
 * and "weird hardware storage" formats.
 * \{
 */

/**
 * \brief A standard UNIX file.
 */
#define MFILE_CADDY_TYPE_FILE 0x01

/**
 * \brief An array of bytes in memory abstracted through this library.
 */
#define MFILE_CADDY_TYPE_MEM_BUFFER 0x80

/*! \} */ /* maug_mfile_types */

#define MFILE_FLAG_READ_ONLY     0x01

#define MFILE_READ_FLAG_LSBF     0x01

#ifndef MFILE_TRACE_LVL
#  define MFILE_TRACE_LVL 0
#endif /* !MFILE_TRACE_LVL */

struct MFILE_CADDY;

typedef off_t (*mfile_has_bytes_t)( struct MFILE_CADDY* p_file );
typedef MERROR_RETVAL (*mfile_seek_t)( struct MFILE_CADDY* p_file, off_t pos );
typedef MERROR_RETVAL (*mfile_read_int_t)(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz, uint8_t flags );
typedef MERROR_RETVAL (*mfile_read_line_t)(
   struct MFILE_CADDY* p_file, char* buf, off_t buf_sz, uint8_t flags );

/* TODO: Don't load platform-specific file stuff if MVFS is enabled. */
#include <mrapifil.h>

struct MFILE_CADDY {
   /*! \brief The \ref maug_mfile_types flag describing this file. */
   uint8_t type;
   /*! \brief The physical handle or pointer to access the file by. */
   union MFILE_HANDLE h;
   off_t sz;
   off_t last_read;
   /*! \brief Current position if its type is ::MFILE_CADDY_TYPE_MEM_BUFFER. */
   off_t mem_cursor;
   /*! \brief Locked pointer for MFILE_HANDLE::mem. */
   uint8_t* mem_buffer;
   uint8_t flags;
   mfile_has_bytes_t has_bytes;
   mfile_seek_t seek;
   mfile_read_int_t read_int;
   mfile_read_line_t read_line;
};

typedef struct MFILE_CADDY mfile_t;

MERROR_RETVAL mfile_file_read_int(
   struct MFILE_CADDY* p_f, uint8_t* buf, size_t buf_sz, uint8_t flags );

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos );

MERROR_RETVAL mfile_file_read_line(
   struct MFILE_CADDY* p_f, char* buffer, off_t buffer_sz, uint8_t flags );

#define mfile_check_lock( p_file ) (NULL != (p_file)->mem_buffer)

#define mfile_get_sz( p_file ) ((p_file)->sz)

/**
 * \brief Lock a buffer and assign it to an ::mfile_t to read/write.
 */
MERROR_RETVAL mfile_lock_buffer( MAUG_MHANDLE, off_t, mfile_t* p_file );

/**
 * \brief Open a file and read it into memory or memory-map it.
 * \param filename NULL-terminated path to file to open.
 */
MERROR_RETVAL mfile_open_read( const char* filename, mfile_t* p_file );

/**
 * \brief Close a file opened with mfile_open_read().
 */
void mfile_close( mfile_t* p_file );

#ifdef MFILE_C

#include <mrapifil.h>

#ifdef MVFS_ENABLED
#  include <mvfs.h>
#endif /* MVFS_ENABLED */

off_t mfile_mem_has_bytes( struct MFILE_CADDY* p_file ) {
   return p_file->sz - p_file->mem_cursor;
}

/* === */

MERROR_RETVAL mfile_mem_read_int(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_MEM_BUFFER == p_file->type );

   if( MFILE_READ_FLAG_LSBF != (MFILE_READ_FLAG_LSBF & flags) ) {
      /* Shrink the buffer moving right and read into it. */
      while( 0 < buf_sz ) {
         /* Check for EOF. */
         if( p_file->mem_cursor >= p_file->sz ) {
            retval = MERROR_FILE;
            error_printf(
               "cursor " OFF_T_FMT " beyond end of buffer " OFF_T_FMT "!",
               p_file->mem_cursor, p_file->sz );
            goto cleanup;
         }

         buf[buf_sz - 1] = p_file->mem_buffer[p_file->mem_cursor];
         debug_printf( MFILE_TRACE_LVL, "byte #" SIZE_T_FMT " = # " OFF_T_FMT,
            buf_sz - 1, p_file->mem_cursor );
         buf_sz--;
         p_file->mem_cursor++;
      }
   
   } else {
      /* Move to the end of the output buffer and read backwards. */
      while( 0 < buf_sz ) {
         /* Check for EOF. */
         if( p_file->mem_cursor >= p_file->sz ) {
            retval = MERROR_FILE;
            error_printf(
               "cursor " OFF_T_FMT " beyond end of buffer " OFF_T_FMT "!",
               p_file->mem_cursor, p_file->sz );
            goto cleanup;
         }

         buf[buf_sz - 1] = p_file->mem_buffer[p_file->mem_cursor];
         debug_printf( MFILE_TRACE_LVL, "byte #" SIZE_T_FMT " = # " OFF_T_FMT,
            buf_sz - 1, p_file->mem_cursor );
         buf_sz--;
         buf++;
         p_file->mem_cursor++;
      }
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_mem_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_MEM_BUFFER == p_file->type );

   p_file->mem_cursor = pos;

   debug_printf( 1,
      "seeking memory buffer to position " OFF_T_FMT " (" OFF_T_FMT ")",
      pos, p_file->mem_cursor );

   return retval;
}

/* === */

MERROR_RETVAL mfile_mem_read_line(
   struct MFILE_CADDY* p_f, char* buffer, off_t buffer_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   off_t i = 0;

   assert( MFILE_CADDY_TYPE_MEM_BUFFER == p_f->type );

   while( i < buffer_sz - 1 && p_f->has_bytes( p_f ) ) {
      /* Check for potential overflow. */
      if( i + 1 >= buffer_sz ) {
         error_printf( "overflow reading string from file!" );
         retval = MERROR_FILE;
         break;
      }

      p_f->read_int( p_f, (uint8_t*)&(buffer[i]), 1, 0 );
      if( '\n' == buffer[i] ) {
         /* Break on newline and overwrite it below. */
         break;
      }
      i++;
   }

   assert( i < buffer_sz ); /* while() above stops before buffer_sz! */

   /* Append a null terminator. */
   buffer[i] = '\0';

   return retval;
}

/* === */

MERROR_RETVAL mfile_lock_buffer(
   MAUG_MHANDLE handle, off_t handle_sz,  mfile_t* p_file
) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1,
      "locking handle %p as file %p (" OFF_T_FMT " bytes)...",
      handle, p_file, handle_sz );

   maug_mzero( p_file, sizeof( struct MFILE_CADDY ) );
   maug_mlock( handle, p_file->mem_buffer );
   p_file->type = MFILE_CADDY_TYPE_MEM_BUFFER;

   p_file->read_int = mfile_mem_read_int;
   p_file->seek = mfile_mem_seek;
   p_file->read_line = mfile_mem_read_line;

   p_file->sz = handle_sz;

   return retval;
}

/* === */

MERROR_RETVAL mfile_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
#  if defined( MVFS_ENABLED )
   size_t i = 0;
#  elif defined( MFILE_MMAP )
   uint8_t* bytes_ptr = NULL;
   struct stat st;
   int in_file = 0;
#  else
#  endif /* MFILE_MMAP */

   /* MAUG_MHANDLE* p_bytes_ptr_h, off_t* p_bytes_sz */

   /* TODO: Move MVFS stuff into its own platform handler or something? */
#  if defined( MVFS_ENABLED )

   while( NULL != gc_mvfs_data[i] ) {
      if( 0 == strcmp( filename, gc_mvfs_filenames[i] ) ) {
         debug_printf( 1, "found file \"%s\" at VFS index: " SIZE_T_FMT
         " (size: " OFF_T_FMT " bytes)",
            filename, i, *(gc_mvfs_lens[i]) );
         break;
      }
      i++;
   }

   if( NULL == gc_mvfs_data[i] ) {
      retval = MERROR_FILE;
      error_printf( "file \"%s\" not found in VFS!", filename );
      goto cleanup;
   }

   p_file->type = MFILE_CADDY_TYPE_MEM_BUFFER;

   p_file->has_bytes = mfile_mem_has_bytes;
   p_file->read_int = mfile_mem_read_int;
   p_file->seek = mfile_mem_seek;
   p_file->read_line = mfile_mem_read_line;
   p_file->flags = MFILE_FLAG_READ_ONLY;
   p_file->mem_buffer = gc_mvfs_data[i];
   p_file->sz = *(gc_mvfs_lens[i]);
   p_file->mem_cursor = 0;

cleanup:
   
#  else

   /* Call the platform-specific actual file opener from mrapifil.h. */
   retval = mfile_plt_open_read( filename, p_file );

#  endif /* MVFS_ENABLED */

   return retval;
}

/* === */

void mfile_close( mfile_t* p_file ) {
#  ifdef MFILE_MMAP
   munmap( bytes_ptr_h, bytes_sz );
#  else
   /* maug_mfree( bytes_ptr_h ); */
   switch( p_file->type ) {
   case 0:
      /* Do nothing silently. */
      break;

   case MFILE_CADDY_TYPE_FILE:
      mfile_plt_close( p_file );
      p_file->type = 0;
      break;

   case MFILE_CADDY_TYPE_MEM_BUFFER:
      if( NULL != p_file->mem_buffer ) {
         maug_munlock( p_file->h.mem, p_file->mem_buffer );
         debug_printf( 1, "unlocked handle %p from file %p...",
            p_file->h.mem, p_file );
         p_file->type = 0;
      }
      break;
      
   default:
      error_printf( "unknown file type: %d", (p_file)->type );
      break;
   }
#  endif /* MFILE_MMAP */
}

#endif /* MFILE_C */

/*! \} */ /* maug_mfile */

#endif /* !MFILE_H */

