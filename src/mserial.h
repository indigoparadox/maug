
#ifndef MSERIAL_H
#define MSERIAL_H

#include "mrapiser.h"

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

MERROR_RETVAL mserialize_char(
   mfile_t* ser_out, char* p_ser_char, int array );

MERROR_RETVAL mserialize_retroflat_asset_path(
   mfile_t* ser_out, retroflat_asset_path* p_ser_char, int array );

MERROR_RETVAL mserialize_mfix_t(
   mfile_t* ser_out, mfix_t* p_ser_int, int array );

MERROR_RETVAL mserialize_retrotile_coord_t(
   mfile_t* ser_out, retrotile_coord_t* p_ser_int, int array );

MERROR_RETVAL mserialize_float(
   mfile_t* ser_out, float* p_ser_float, int array );

MERROR_RETVAL mserialize_struct_RETROTILE_COORDS(
   mfile_t* ser_out, struct RETROTILE_COORDS* p_ser_struct, int array );

MERROR_RETVAL mserialize_retroflat_dir4_t(
   mfile_t* ser_out, retroflat_dir4_t* p_ser_int, int array );

MERROR_RETVAL mserialize_struct_MDATA_VECTOR(
   mfile_t* ser_out, struct MDATA_VECTOR* p_ser_struct, int array );

MERROR_RETVAL mserialize_struct_MLISP_EXEC_STATE(
   mfile_t* ser_out, struct MLISP_EXEC_STATE* p_ser_struct, int array );

#include "mrapiser.h"

#endif /* !MSERIAL_H */

