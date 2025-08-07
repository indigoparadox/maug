
#if !defined( MAUG_API_FIL_H_DEFS )
#define MAUG_API_FIL_H_DEFS

union MFILE_HANDLE {
   MAUG_MHANDLE mem;
};

#elif defined( MFILE_C )

MERROR_RETVAL mfile_plt_open_write( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_FILE;
   return retval;
}

/* === */

void mfile_plt_close( mfile_t* p_file ) {
}

#endif /* !MAUG_API_FIL_H_DEFS */

