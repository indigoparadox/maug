
#ifndef MAUGCHCK_H
#define MAUGCHCK_H

#if defined( MAUG_OS_WIN ) || defined( RETROFLAT_OS_WIN )
#  include <mfakechk.h>
#else
#  include <check.h>
#endif /* MAUG_OS_WIN */

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

#endif /* !MAUGCHCK_H */

