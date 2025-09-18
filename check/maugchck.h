
#ifndef MAUGCHCK_H
#define MAUGCHCK_H

#if defined( MAUG_FAKECHECK )
#  include <mfakechk.h>
#else
#  include <check.h>
#endif /* MAUG_OS_WIN */

#include <maug.h>
#include <mlisps.h>
#include <mlispp.h>
#include <mlispe.h>
#include "mserial.h"

#ifdef RETROFLAT_API_WIN16
#  define MAUGCHK_TABLE( f ) \
      f( mdat ) \
      f( mfmt ) \
      f( mlsp ) \
      f( mser )
#else
#  define MAUGCHK_TABLE( f ) \
      f( mdat ) \
      f( mfmt ) \
      f( mlsp ) \
      f( mfil ) \
      f( mser )
#endif

#endif /* !MAUGCHCK_H */

