
#ifndef MSERIAL_H
#define MSERIAL_H

#ifndef MSERIALIZE_TRACE_LVL
#  define MSERIALIZE_TRACE_LVL 0
#endif /* !MSERIALIZE_TRACE_LVL */

#include "mrapiser.h"

#define MSERIALIZE_TYPE_OBJECT   1
#define MSERIALIZE_TYPE_STRING   2
#define MSERIALIZE_TYPE_INTEGER  3
#define MSERIALIZE_TYPE_FLOAT    4
#define MSERIALIZE_TYPE_ARRAY    5
#define MSERIALIZE_TYPE_BLOB     6

typedef MERROR_RETVAL (*mserialize_cb_t)(
   mfile_t* ser_out, void* p_ser_int, int array  );

typedef MERROR_RETVAL (*mdeserialize_cb_t)(
   mfile_t* ser_out, void* p_ser_int, int array, ssize_t* p_ser_sz  );

MERROR_RETVAL mserialize_int( mfile_t* ser_out, int32_t value, int array );

MERROR_RETVAL mserialize_vector(
   mfile_t* ser_f, struct MDATA_VECTOR* p_ser_vec, mserialize_cb_t cb );

MERROR_RETVAL mserialize_block(
   mfile_t* p_file, void* p_block, size_t block_sz );

off_t mserialize_header( mfile_t* ser_out, uint8_t type, uint8_t flags );

MERROR_RETVAL mserialize_footer( mfile_t* ser_out, off_t header, uint8_t flags );

MERROR_RETVAL mserialize_size_t(
   mfile_t* ser_out, size_t* p_ser_int, int array );

MERROR_RETVAL mserialize_ssize_t(
   mfile_t* ser_out, ssize_t* p_ser_int, int array );

MERROR_RETVAL mserialize_uint8_t(
   mfile_t* ser_out, uint8_t* p_ser_int, int array );

MERROR_RETVAL mserialize_int8_t(
   mfile_t* ser_out, int8_t* p_ser_int, int array );

MERROR_RETVAL mserialize_uint16_t(
   mfile_t* ser_out, uint16_t* p_ser_int, int array );

MERROR_RETVAL mserialize_int16_t(
   mfile_t* ser_out, int16_t* p_ser_int, int array );

MERROR_RETVAL mserialize_uint32_t(
   mfile_t* ser_out, uint32_t* p_ser_int, int array );

MERROR_RETVAL mserialize_int32_t(
   mfile_t* ser_out, int32_t* p_ser_int, int array );

MERROR_RETVAL mserialize_float(
   mfile_t* ser_out, float* p_ser_float, int array );

MERROR_RETVAL mserialize_mfix_t(
   mfile_t* ser_out, mfix_t* p_ser_int, int array );

MERROR_RETVAL mserialize_char(
   mfile_t* ser_out, char* p_ser_char, int array );

MERROR_RETVAL mserialize_retroflat_asset_path(
   mfile_t* ser_out, retroflat_asset_path* p_ser_char, int array );

MERROR_RETVAL mserialize_retrotile_coord_t(
   mfile_t* ser_out, retrotile_coord_t* p_ser_int, int array );

MERROR_RETVAL mserialize_mdata_strpool_idx_t(
   mfile_t* ser_out, mdata_strpool_idx_t* p_ser_int, int array );

MERROR_RETVAL mserialize_retroflat_dir4_t(
   mfile_t* ser_out, retroflat_dir4_t* p_ser_int, int array );

MERROR_RETVAL mserialize_retroflat_ms_t(
   mfile_t* ser_out, retroflat_ms_t* p_ser_int, int array );

MERROR_RETVAL mserialize_struct_RETROTILE_COORDS(
   mfile_t* ser_out, struct RETROTILE_COORDS* p_ser_struct, int array );

MERROR_RETVAL mserialize_struct_MLISP_ENV_NODE(
   mfile_t* ser_out, struct MLISP_ENV_NODE* p_ser_struct, int array );

MERROR_RETVAL mserialize_union_MLISP_VAL(
   mfile_t* ser_out, union MLISP_VAL* p_ser_val, int array );

/* === */

MERROR_RETVAL mdeserialize_header(
   mfile_t* ser_in, uint8_t* p_type, ssize_t* p_sz );

MERROR_RETVAL mdeserialize_int(
   mfile_t* ser_out, int32_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_vector(
   mfile_t* ser_f, struct MDATA_VECTOR* p_ser_vec, mdeserialize_cb_t cb,
   ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_size_t(
   mfile_t* ser_out, size_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_ssize_t(
   mfile_t* ser_out, ssize_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_uint8_t(
   mfile_t* ser_out, uint8_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_int8_t(
   mfile_t* ser_out, int8_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_uint16_t(
   mfile_t* ser_out, uint16_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_int16_t(
   mfile_t* ser_out, int16_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_uint32_t(
   mfile_t* ser_out, uint32_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_int32_t(
   mfile_t* ser_out, int32_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_float(
   mfile_t* ser_out, float* p_ser_float, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_mfix_t(
   mfile_t* ser_out, mfix_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_char(
   mfile_t* ser_out, char* p_ser_char, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_retroflat_asset_path(
   mfile_t* ser_out, retroflat_asset_path* p_ser_char, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_retrotile_coord_t(
   mfile_t* ser_out, retrotile_coord_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_mdata_strpool_idx_t(
   mfile_t* ser_out, mdata_strpool_idx_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_retroflat_dir4_t(
   mfile_t* ser_out, retroflat_dir4_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_retroflat_ms_t(
   mfile_t* ser_out, retroflat_ms_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_struct_RETROTILE_COORDS(
   mfile_t* ser_out, struct RETROTILE_COORDS* p_ser_struct, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_struct_MLISP_ENV_NODE(
   mfile_t* ser_out, struct MLISP_ENV_NODE* p_ser_struct, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_vector_struct_MLISP_ENV_NODE(
   mfile_t* ser_out, struct MDATA_VECTOR* p_ser_vec );

MERROR_RETVAL mdeserialize_union_MLISP_VAL(
   mfile_t* ser_out, union MLISP_VAL* p_ser_val, int array, ssize_t* p_ser_sz );

#include "mrapiser.h"

#ifdef MSERIAL_C

MERROR_RETVAL mserialize_size_t(
   mfile_t* ser_out, size_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_ssize_t(
   mfile_t* ser_out, ssize_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_uint8_t(
   mfile_t* ser_out, uint8_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_int8_t(
   mfile_t* ser_out, int8_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_uint16_t(
   mfile_t* ser_out, uint16_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_int16_t(
   mfile_t* ser_out, int16_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_uint32_t(
   mfile_t* ser_out, uint32_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_int32_t(
   mfile_t* ser_out, int32_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_mfix_t(
   mfile_t* ser_out, mfix_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_retrotile_coord_t(
   mfile_t* ser_out, retrotile_coord_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_mdata_strpool_idx_t (
   mfile_t* ser_out, mdata_strpool_idx_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_retroflat_dir4_t(
   mfile_t* ser_out, retroflat_dir4_t* p_ser_int, int array 
) {
   return mserialize_int( ser_out, *p_ser_int, array );
}

/* === */

MERROR_RETVAL mdeserialize_size_t(
   mfile_t* ser_out, size_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_ssize_t(
   mfile_t* ser_out, ssize_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_uint8_t(
   mfile_t* ser_out, uint8_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_int8_t(
   mfile_t* ser_out, int8_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_uint16_t(
   mfile_t* ser_out, uint16_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_int16_t(
   mfile_t* ser_out, int16_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_uint32_t(
   mfile_t* ser_out, uint32_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_int32_t(
   mfile_t* ser_out, int32_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_mfix_t(
   mfile_t* ser_out, mfix_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_retrotile_coord_t(
   mfile_t* ser_out, retrotile_coord_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_mdata_strpool_idx_t(
   mfile_t* ser_out, mdata_strpool_idx_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_retroflat_dir4_t(
   mfile_t* ser_out, retroflat_dir4_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_retroflat_ms_t(
   mfile_t* ser_out, retroflat_ms_t* p_ser_int, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t value = 0;
   retval = mdeserialize_int( ser_out, &value, array, p_ser_sz );
   maug_cleanup_if_not_ok();
   *p_ser_int = value;
cleanup:
   return retval;
}

#endif /* MSERIAL_C */

#endif /* !MSERIAL_H */

