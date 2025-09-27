
#ifndef MAUGCHCK_H
#define MAUGCHCK_H

#if defined( MAUG_FAKECHECK )
#  include <mfakechk.h>
#else
#  include <check.h>
#endif /* MAUG_FAKECHECK */

#if defined( RETROFLAT_OS_UNIX )
#define TMP_PATH "/tmp/"
#elif defined( RETROFLAT_OS_DOS ) || defined( RETROFLAT_OS_WIN )
#define TMP_PATH "C:/temp"
#else
#error "don't know temporary path!"
#endif /* MAUG_OS_UNIX || MAUG_OS_WIN */

#include <maug.h>
#include <mlisps.h>
#include <mlispp.h>
#include <mlispe.h>
#include "mserial.h"

#define MAUGCHK_TABLE( f ) \
   f( mdat ) \
   f( mfmt ) \
   f( mlsp ) \
   f( mfil ) \
   f( mser )

MERROR_RETVAL open_temp( const char* filename, mfile_t* p_file );

void close_temp( mfile_t* p_file );

#endif /* !MAUGCHCK_H */

