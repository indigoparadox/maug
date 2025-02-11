
#ifndef RETROACV_H
#define RETROACV_H

#ifndef RETROACV_ACHIEVEMENT_ID_SZ_MAX
#  define RETROACV_ACHIEVEMENT_ID_SZ_MAX 10
#endif /* !RETROACV_ACHIEVEMENT_ID_SZ_MAX */

#define RETROACV_FLAG_ACHIEVED 0x01

struct RETROACV_ACHIEVEMENT {
   uint8_t flags;
   char id[RETROACV_ACHIEVEMENT_ID_SZ_MAX];
};

struct RETROACV_LIST {
   uint8_t flags;
   struct RETROGUI* gui;
   size_t acv_count;
   struct RETROACV_ACHIEVEMENT* acv;
}

MERROR_RETVAL retroacv_init( struct RETROACV_LIST* acv_list );

MERROR_RETVAL retroacv_update(
   struct RETROACV_LIST* acv_list, char* id, uint8_t flags );

MERROR_RETVAL retroacv_save( struct RETROACV_LIST* acv_list );

void retroacv_shutdown( struct RETROACV_LIST* acv_list );

#ifdef RETROACV_C

MERROR_RETVAL retroacv_init( struct RETROACV_LIST* acv_list ) {
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == acv_list ) {
      retval = MERROR_ALLOC;
      goto cleanup;
   }


cleanup:

   return retval;
};

void retroacv_shutdown( struct RETROACV_LIST* acv_list ) {

};

#endif /* RETROACV_C */

#endif /* !RETROACV_H */

