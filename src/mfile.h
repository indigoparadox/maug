
#ifndef MFILE_H
#define MFILE_H

#if !defined( DEBUG_THRESHOLD )
#  define DEBUG_THRESHOLD 1
#endif /* !DEBUG_THRESHOLD */

#ifndef UPRINTF_BUFFER_SZ_MAX
#  define UPRINTF_BUFFER_SZ_MAX 1024
#endif /* !UPRINTF_BUFFER_SZ_MAX */

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

#ifndef MAUG_PATH_SZ_MAX
/*! \brief Maximum size allocated for asset paths. */
#  define MAUG_PATH_SZ_MAX 256
#endif /* !MAUG_PATH_SZ_MAX */

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

/**
 * \relates MFILE_CADDY
 * \brief Flag for MFILE_CADDY::flags indicating this file is read-only.
 */
#define MFILE_FLAG_READ_ONLY     0x01

/**
 * \relates MFILE_CADDY
 * \brief Flag for MFILE_CADDY::flags indicating subsequent internal unlocks
 *        should unlock the handle back to its buffer. Should only be set
 *        internally!
 */
#define MFILE_FLAG_HANDLE_LOCKED 0x02

/**
 * \addtogroup maug_mfile_byte_order RetroFile Byte Order
 * \brief Flags controlling byte order for read operations.
 *
 * If neither of these flags are specified, mfile_read_int_t() operations will
 * default to the native byte order of the current system.
 *
 * \note These flags are both the same flag, essentially, so they are set to
 *       0x01. This is so that we can check for their presence vs the current
 *       platform endianness. Essentially they are ignored if the requested
 *       endianness matches the current platform's default endianness. Please
 *       see mfile_file_read_int() for the implementation.
 */

/**
 * \relates mfile_read_int_t
 * \brief Flag for mfile_read_int_t() indicating integer should always be read
 *        least significant byte first.
 */
#define MFILE_READ_FLAG_LSBF     0x01

/**
 * \relates mfile_read_int_t
 * \brief Flag for mfile_read_int_t() indicating integer should always be read
 *        most significant byte first.
 */
#define MFILE_READ_FLAG_MSBF     0x01

#define MFILE_ASSIGN_FLAG_TRIM_EXT 0x01

#ifndef MFILE_READ_TRACE_LVL
#  define MFILE_READ_TRACE_LVL 0
#endif /* !MFILE_READ_TRACE_LVL */

#ifndef MFILE_WRITE_TRACE_LVL
#  define MFILE_WRITE_TRACE_LVL 0
#endif /* !MFILE_WRITE_TRACE_LVL */

#ifndef MFILE_SEEK_TRACE_LVL
#  define MFILE_SEEK_TRACE_LVL 0
#endif /* !MFILE_SEEK_TRACE_LVL */

#ifndef MFILE_CONTENTS_TRACE_LVL
#  define MFILE_CONTENTS_TRACE_LVL 0
#endif /* !MFILE_CONTENTS_TRACE_LVL */

/**
 * \addtogroup maug_retroflt_assets RetroFlat Assets API
 * \brief Functions and macros for handling graphical asset files.
 * \todo This is kind of a mess and needs better integration with the rest!
 * \{
 */

/**
 * \brief Path/name used to load an asset from disk.
 */
typedef char retroflat_asset_path[MAUG_PATH_SZ_MAX + 1];

/**
 * \brief Compare two asset paths. Return 0 if they're the same.
 */
#define mfile_cmp_path( a, b ) strncmp( a, b, MAUG_PATH_SZ_MAX )

/*! \} */ /* maug_retroflt_assets */

struct MFILE_CADDY;
typedef struct MFILE_CADDY mfile_t;

typedef off_t (*mfile_cursor_t)( struct MFILE_CADDY* p_file );
typedef off_t (*mfile_has_bytes_t)( struct MFILE_CADDY* p_file );
typedef MERROR_RETVAL (*mfile_read_byte_t)(
   struct MFILE_CADDY* p_file, uint8_t* buf );
typedef MERROR_RETVAL (*mfile_read_block_t)(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz );
typedef MERROR_RETVAL (*mfile_seek_t)( struct MFILE_CADDY* p_file, off_t pos );
typedef MERROR_RETVAL (*mfile_read_int_t)(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz, uint8_t flags );
typedef MERROR_RETVAL (*mfile_read_line_t)(
   struct MFILE_CADDY* p_file, char* buf, off_t buf_sz, uint8_t flags );
typedef MERROR_RETVAL (*mfile_printf_t)(
   struct MFILE_CADDY* p_file, uint8_t flags, const char* fmt, ... );
typedef MERROR_RETVAL (*mfile_write_block_t)(
   struct MFILE_CADDY* p_f, const uint8_t* buf, size_t buf_sz );

off_t mfile_mem_cursor( struct MFILE_CADDY* p_file );
off_t mfile_mem_has_bytes( struct MFILE_CADDY* p_file );
MERROR_RETVAL mfile_mem_read_byte( struct MFILE_CADDY* p_file, uint8_t* buf );
MERROR_RETVAL mfile_mem_read_block(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz );
MERROR_RETVAL mfile_mem_seek( struct MFILE_CADDY* p_file, off_t pos );
MERROR_RETVAL mfile_mem_read_line(
   struct MFILE_CADDY* p_f, char* buffer, off_t buffer_sz, uint8_t flags );
MERROR_RETVAL mfile_mem_vprintf(
   mfile_t* p_file, uint8_t flags, const char* fmt, va_list args );

/**
 * \related MFILE_CADDY
 * \brief Insert provided buffer into the given file.
 * 
 * Note that this method *inserts* the given buffer into the file, shifting the
 * rest of the contents forward.
 */
MERROR_RETVAL mfile_mem_write_block(
   struct MFILE_CADDY* p_f, const uint8_t* buf, size_t buf_sz );

MERROR_RETVAL mfile_plt_init();

/**
 * \related MFILE_CADDY
 * \brief Callback to printf the given format string, replacing tokens from
 *        the providied *pre-initialized* list of args.
 * \param Pre-initialized list of token format args.
 * \warning The args parameter must have already been initialized with va_start!
 */
typedef MERROR_RETVAL (*mfile_vprintf_t)(
   struct MFILE_CADDY* p_file, uint8_t flags, const char* fmt, va_list args );

/* Load the platform-specific file API. */
#include <mrapifil.h>
#include <mrapilog.h>

struct MFILE_CADDY {
   /*! \brief The \ref maug_mfile_types flag describing this file. */
   uint8_t type;
   /*! \brief The physical handle or pointer to access the file by. */
   union MFILE_HANDLE h;
   off_t last_read;
   /*! \brief Current position if its type is ::MFILE_CADDY_TYPE_MEM_BUFFER. */
   off_t mem_cursor;
   /*! \brief Locked pointer for MFILE_HANDLE::mem. */
   uint8_t* mem_buffer;
   uint8_t flags;
   /*! \brief Size of the current file/buffer in bytes. */
   off_t sz;
   char filename[MAUG_PATH_SZ_MAX + 1];
   mfile_has_bytes_t has_bytes;
   mfile_cursor_t cursor;
   mfile_read_byte_t read_byte;
   mfile_read_block_t read_block;
   mfile_seek_t seek;
   mfile_read_int_t read_int;
   mfile_read_line_t read_line;
   mfile_printf_t printf;
   mfile_vprintf_t vprintf;
   mfile_write_block_t write_block;
};

typedef struct MFILE_CADDY mfile_t;

/**
 * \addtogroup maug_retroflt_assets RetroFlat Assets API
 * \{
 */

MERROR_RETVAL mfile_assign_path(
   retroflat_asset_path tgt, const retroflat_asset_path src, uint8_t flags );

/*! \} */

off_t mfile_file_has_bytes( struct MFILE_CADDY* p_file );

MERROR_RETVAL mfile_file_read_byte( struct MFILE_CADDY* p_file, uint8_t* buf );

MERROR_RETVAL mfile_file_read_block(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz );

MERROR_RETVAL mfile_file_read_int(
   struct MFILE_CADDY* p_f, uint8_t* buf, size_t buf_sz, uint8_t flags );

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos );

MERROR_RETVAL mfile_file_read_line(
   struct MFILE_CADDY* p_f, char* buffer, off_t buffer_sz, uint8_t flags );

MERROR_RETVAL mfile_file_printf(
   struct MFILE_CADDY* p_f, uint8_t flags, const char* fmt, ... );

MERROR_RETVAL mfile_file_write_block(
   struct MFILE_CADDY* p_f, const uint8_t* buf, size_t buf_sz );

MERROR_RETVAL mfile_file_vprintf(
   struct MFILE_CADDY* p_f, uint8_t flags, const char* fmt, va_list args );

#define mfile_get_sz( p_file ) ((p_file)->sz)

/**
 * \brief Lock a buffer and assign it to an ::mfile_t to read/write.
 */
MERROR_RETVAL mfile_lock_buffer(
   MAUG_MHANDLE, void* ptr, off_t, mfile_t* p_file );

/**
 * \brief Open a file and read it into memory or memory-map it.
 * \param filename NULL-terminated path to file to open.
 */
MERROR_RETVAL mfile_open_read( const char* filename, mfile_t* p_file );

MERROR_RETVAL mfile_open_write( const char* filename, mfile_t* p_file );

/**
 * \brief Close a file opened with mfile_open_read().
 */
void mfile_close( mfile_t* p_file );

#ifdef MFILE_C

#include <mrapifil.h>
#include <mrapilog.h>

off_t mfile_file_has_bytes( struct MFILE_CADDY* p_file ) {
   size_t cursor = 0;
   cursor = p_file->cursor( p_file );
   if( 0 <= cursor ) {
#if MFILE_READ_TRACE_LVL > 0
      debug_printf( MFILE_READ_TRACE_LVL,
         "file has " OFF_T_FMT " bytes left...",
         p_file->sz - cursor );
#endif /* MFILE_READ_TRACE_LVL */
      return p_file->sz - cursor;
   } else {
#if MFILE_READ_TRACE_LVL > 0
      /* TODO: Improved error message/handling. */
      debug_printf( MFILE_READ_TRACE_LVL, "file has error bytes left!" );
#endif /* MFILE_READ_TRACE_LVL */
      return 0;
   }
}

/* === */

MERROR_RETVAL mfile_assign_path(
   retroflat_asset_path tgt, const retroflat_asset_path src, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   char* ext_ptr = NULL;

   maug_snprintf( tgt, MAUG_PATH_SZ_MAX, "%s", src );

   if( MFILE_ASSIGN_FLAG_TRIM_EXT == (MFILE_ASSIGN_FLAG_TRIM_EXT & flags) ) {
      ext_ptr = maug_strrchr( tgt, '.' );
      if( NULL != ext_ptr ) {
         *ext_ptr = '\0';
      }
   }

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_read_int(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   if(
#ifdef MAUG_LSBF
      MFILE_READ_FLAG_LSBF == (MFILE_READ_FLAG_LSBF & flags)
#elif defined( MAUG_MSBF )
      MFILE_READ_FLAG_MSBF != (MFILE_READ_FLAG_MSBF & flags)
#endif
   ) {
      debug_printf( MFILE_READ_TRACE_LVL, "reading integer forward" );
      /* Shrink the buffer moving right and read into it. */
      retval = p_file->read_block( p_file, buf, buf_sz );
 
   } else {
      debug_printf( MFILE_READ_TRACE_LVL, "reading integer reversed" );
      /* Move to the end of the output buffer and read backwards. */
      while( 0 < buf_sz ) {
         retval = p_file->read_byte( p_file, (buf + (buf_sz - 1)) );
         maug_cleanup_if_not_ok();
         buf_sz--;
      }
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_printf(
   struct MFILE_CADDY* p_file, uint8_t flags, const char* fmt, ...
) {
   MERROR_RETVAL retval = MERROR_OK;
   va_list vargs;

   va_start( vargs, fmt );
   retval = p_file->vprintf( p_file, flags, fmt, vargs );
   va_end( vargs );

   return retval;
}

/* === */

off_t mfile_mem_cursor( struct MFILE_CADDY* p_file ) {
   return p_file->mem_cursor;
}

/* === */

off_t mfile_mem_has_bytes( struct MFILE_CADDY* p_file ) {
   return p_file->sz - p_file->mem_cursor;
} 

/* === */

static MERROR_RETVAL mfile_mem_lock( struct MFILE_CADDY* p_f ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert(
      MFILE_FLAG_HANDLE_LOCKED != (MFILE_FLAG_HANDLE_LOCKED & p_f->flags) );

   assert( MFILE_CADDY_TYPE_MEM_BUFFER == p_f->type );

   /* Only lock if this buffer uses a handle and not a pointer. */
   if( (MAUG_MHANDLE)NULL != p_f->h.mem ) {
      assert( NULL == p_f->mem_buffer );
      maug_mlock( p_f->h.mem, p_f->mem_buffer );
      p_f->flags |= MFILE_FLAG_HANDLE_LOCKED;
   }

   return retval;
}

/* === */

static void mfile_mem_release( struct MFILE_CADDY* p_f ) {

   assert( MFILE_CADDY_TYPE_MEM_BUFFER == p_f->type );
   
   if( MFILE_FLAG_HANDLE_LOCKED == (MFILE_FLAG_HANDLE_LOCKED & p_f->flags) ) {
      maug_munlock( p_f->h.mem, p_f->mem_buffer );
      p_f->flags &= ~MFILE_FLAG_HANDLE_LOCKED;
   }
}

/* === */

MERROR_RETVAL mfile_mem_read_byte( struct MFILE_CADDY* p_file, uint8_t* buf ) {
   return p_file->read_block( p_file, buf, 1 );
}

/* === */

MERROR_RETVAL mfile_mem_read_block(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( p_file->mem_cursor >= p_file->sz ) {
      return MERROR_FILE;
   }

   retval = mfile_mem_lock( p_file );
   maug_cleanup_if_not_ok();

   while( 0 < buf_sz-- ) {
      *(buf++) = p_file->mem_buffer[p_file->mem_cursor++];
   }

cleanup:

   mfile_mem_release( p_file );

   return retval;
}

/* === */

MERROR_RETVAL mfile_mem_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_MEM_BUFFER == p_file->type );

   p_file->mem_cursor = pos;

   debug_printf( MFILE_SEEK_TRACE_LVL,
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

MERROR_RETVAL mfile_mem_vprintf(
   mfile_t* p_file, uint8_t flags, const char* fmt, va_list args
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & p_file->flags) ) {
      return MERROR_FILE;
   }

   /* TODO: Enable writing to memory buffer. */
   error_printf( "writing to memory buffer not currently supported!" );

   /* TODO: Expand buffer and reflect this in sz if writing beyond the end
    *       of the buffer.
    */

   return retval;
}

/* === */

MERROR_RETVAL mfile_mem_write_block(
   struct MFILE_CADDY* p_f, const uint8_t* buf, size_t buf_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t mv_sz = 0;

   if( 0 == buf_sz ) {
      /* Short-circuit empty writes. */
      return MERROR_OK;
   }

   if( MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & p_f->flags) ) {
      return MERROR_FILE;
   }

   retval = mfile_mem_lock( p_f );
   maug_cleanup_if_not_ok();

#if MFILE_WRITE_TRACE_LVL > 0
   debug_printf( MFILE_WRITE_TRACE_LVL, "p: %p, sz: %u, cur: %u, buf_sz: %u\n",
      p_f->mem_buffer, p_f->sz, p_f->mem_cursor, buf_sz );
#endif /* MFILE_WRITE_TRACE_LVL */

   mv_sz = (p_f->sz - (p_f->mem_cursor + buf_sz));
   if( 0 < mv_sz ) {
      memmove(
         &(p_f->mem_buffer[p_f->mem_cursor + buf_sz]),
         &(p_f->mem_buffer[p_f->mem_cursor]),
         mv_sz );
   }

   memcpy( &(p_f->mem_buffer[p_f->mem_cursor]), buf, buf_sz );
   p_f->mem_cursor += buf_sz;

   /* TODO: Expand buffer and reflect this in sz if writing beyond the end
    *       of the buffer.
    */

cleanup:

   mfile_mem_release( p_f );

   return retval;
}

/* === */

MERROR_RETVAL mfile_lock_buffer(
   MAUG_MHANDLE handle, void* ptr, off_t handle_sz, mfile_t* p_file
) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( MFILE_SEEK_TRACE_LVL,
      "locking handle %p as file %p (" OFF_T_FMT " bytes)...",
      handle, p_file, handle_sz );

   maug_mzero( p_file, sizeof( struct MFILE_CADDY ) );
   
   /* Determine if this is based on a handle or a pointer. */
   if( (MAUG_MHANDLE)NULL == handle && NULL != ptr ) {
      p_file->mem_buffer = ptr;
   } else if( (MAUG_MHANDLE)NULL != handle && NULL == ptr ) {
      p_file->h.mem = handle;
      /* maug_mlock( handle, p_file->mem_buffer ); */
   } else {
      error_printf( "must specify handle or pointer!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

   p_file->type = MFILE_CADDY_TYPE_MEM_BUFFER;

   p_file->has_bytes = mfile_mem_has_bytes;
   p_file->cursor = mfile_mem_cursor;
   p_file->read_byte = mfile_mem_read_byte;
   p_file->read_block = mfile_mem_read_block;
   p_file->read_int = mfile_file_read_int;
   p_file->seek = mfile_mem_seek;
   p_file->read_line = mfile_mem_read_line;
   p_file->write_block = mfile_mem_write_block;

   p_file->sz = handle_sz;

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Call the platform-specific actual file opener from mrapifil.h. */
   retval = mfile_plt_open_read( filename, p_file );

   if( MERROR_OK == retval ) {
      /* Store filename. */
      maug_mzero( p_file->filename, MAUG_PATH_SZ_MAX );
      maug_strncpy( p_file->filename, filename, MAUG_PATH_SZ_MAX );
   }

   return retval;
}

/* === */

MERROR_RETVAL mfile_open_write( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;

   retval = mfile_plt_open_write( filename, p_file );

   if( MERROR_OK == retval ) {
      /* Store filename. */
      maug_mzero( p_file->filename, MAUG_PATH_SZ_MAX );
      maug_strncpy( p_file->filename, filename, MAUG_PATH_SZ_MAX );
   }

   return retval;
}

/* === */

void mfile_close( mfile_t* p_file ) {
#  if MFILE_SEEK_TRACE_LVL > 0
   debug_printf( MFILE_SEEK_TRACE_LVL, "closing file..." );
#  endif /* MFILE_SEEK_TRACE_LVL */
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
         debug_printf( MFILE_SEEK_TRACE_LVL,
            "unlocked handle %p from file %p...",
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

