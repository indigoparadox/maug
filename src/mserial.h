
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

typedef MERROR_RETVAL (*mserialize_cb_t)(
   mfile_t* ser_out, void* p_ser_int, int array  );

MERROR_RETVAL mserialize_vector(
   mfile_t* ser_f, struct MDATA_VECTOR* p_ser_vec, mserialize_cb_t cb );

off_t mserialize_header( mfile_t* ser_out, uint8_t type, uint8_t flags );

MERROR_RETVAL mserialize_footer( mfile_t* ser_out, off_t header, uint8_t flags );

MERROR_RETVAL mserialize_size_t(
   mfile_t* ser_out, size_t* p_ser_int, int array );

MERROR_RETVAL mserialize_vector_size_t(
   mfile_t* ser_out, struct MDATA_VECTOR* p_ser_vec );

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

MERROR_RETVAL mserialize_vector_struct_MLISP_ENV_NODE(
   mfile_t* ser_out, struct MDATA_VECTOR* p_ser_vec );

MERROR_RETVAL mserialize_union_MLISP_VAL(
   mfile_t* ser_out, union MLISP_VAL* p_ser_val, int array );

#include "mrapiser.h"

#endif /* !MSERIAL_H */

