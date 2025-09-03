
#ifndef MSERIAL_H
#define MSERIAL_H

#ifndef MSERIAL_TRACE_LVL
#  define MSERIAL_TRACE_LVL 0
#endif /* !MSERIAL_TRACE_LVL */

#include "mrapiser.h"

#define MSERIALIZE_TYPE_OBJECT   1
#define MSERIALIZE_TYPE_STRING   2
#define MSERIALIZE_TYPE_INTEGER  3
#define MSERIALIZE_TYPE_FLOAT    4

off_t mserialize_header( mfile_t* ser_out, uint8_t type, uint8_t flags );

MERROR_RETVAL mserialize_footer( mfile_t* ser_out, off_t header, uint8_t flags );

MERROR_RETVAL mserialize_size_t(
   mfile_t* ser_out, size_t* p_sz, size_t* p_ser_int, int array );

MERROR_RETVAL mserialize_ssize_t(
   mfile_t* ser_out, size_t* p_sz, ssize_t* p_ser_int, int array );

MERROR_RETVAL mserialize_uint8_t(
   mfile_t* ser_out, size_t* p_sz, uint8_t* p_ser_int, int array );

MERROR_RETVAL mserialize_int8_t(
   mfile_t* ser_out, size_t* p_sz, int8_t* p_ser_int, int array );

MERROR_RETVAL mserialize_uint16_t(
   mfile_t* ser_out, size_t* p_sz, uint16_t* p_ser_int, int array );

MERROR_RETVAL mserialize_int16_t(
   mfile_t* ser_out, size_t* p_sz, int16_t* p_ser_int, int array );

MERROR_RETVAL mserialize_uint32_t(
   mfile_t* ser_out, size_t* p_sz, uint32_t* p_ser_int, int array );

MERROR_RETVAL mserialize_int32_t(
   mfile_t* ser_out, size_t* p_sz, int32_t* p_ser_int, int array );

MERROR_RETVAL mserialize_char(
   mfile_t* ser_out, size_t* p_sz, char* p_ser_char, int array );

MERROR_RETVAL mserialize_retroflat_asset_path(
   mfile_t* ser_out, size_t* p_sz, retroflat_asset_path* p_ser_char, int array );

MERROR_RETVAL mserialize_mfix_t(
   mfile_t* ser_out, size_t* p_sz, mfix_t* p_ser_int, int array );

MERROR_RETVAL mserialize_retrotile_coord_t(
   mfile_t* ser_out, size_t* p_sz, retrotile_coord_t* p_ser_int, int array );

MERROR_RETVAL mserialize_float(
   mfile_t* ser_out, size_t* p_sz, float* p_ser_float, int array );

MERROR_RETVAL mserialize_struct_RETROTILE_COORDS(
   mfile_t* ser_out, size_t* p_sz, struct RETROTILE_COORDS* p_ser_struct, int array );

MERROR_RETVAL mserialize_retroflat_dir4_t(
   mfile_t* ser_out, size_t* p_sz, retroflat_dir4_t* p_ser_int, int array );

MERROR_RETVAL mserialize_struct_MDATA_VECTOR(
   mfile_t* ser_out, size_t* p_sz, struct MDATA_VECTOR* p_ser_struct, int array );

MERROR_RETVAL mserialize_struct_MLISP_EXEC_STATE(
   mfile_t* ser_out, size_t* p_sz, struct MLISP_EXEC_STATE* p_ser_struct, int array );

#include "mrapiser.h"

#endif /* !MSERIAL_H */

