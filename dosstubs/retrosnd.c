
#ifndef MAUG_NO_RETRO
#if !defined( RETROFLAT_NO_RETROSND ) && !defined( RETROFLAT_NO_SOUND )
#  define MAUG_NO_RETRO
#  define RETROSND_C
#  include <maug.h>
#  include <mfile.h>
#  include <retroflt.h>
#  include <retrosnd.h>
#endif /* !RETROFLAT_NO_RETROCFG */
#endif /* !MAUG_NO_RETRO */

