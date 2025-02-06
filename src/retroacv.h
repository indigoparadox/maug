
#ifndef RETROACV_H
#define RETROACV_H

MERROR_RETVAL retroacv_init();

void retroacv_shutdown();

#ifdef RETROACV_C

MERROR_RETVAL retroacv_init() {
   MERROR_RETVAL retval = MERROR_OK;

   return retval;
};

void retroacv_shutdown() {

};

#endif /* RETROACV_C */

#endif /* !RETROACV_H */

