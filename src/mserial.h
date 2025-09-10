
#ifndef MSERIAL_H
#define MSERIAL_H

/**
 * \addtogroup maug_serial Serialization/Deserialization API
 * \brief Tools for serializing memory structures to on-disk formats.
 * \{
 * \file "mserial.h"
 * \brief Functions and macros related to the serialization/deserialization API.
 */

/**
 * \addtogroup maug_serial_tool Serialize Code Generation Tool
 * \{
 * \page maug_serial_tool_page Serialize Code Generation Tool
 * This tool, located in `$MAUG_ROOT/tools/serial.c`, generates code based on
 * the header files it is pointed at. In general, header files processed by this
 * tool must meet the following requirements:
 *
 * * Struct fields are defined one per line.
 * * Constants (e.g. for array counts) are defined in the same file.
 * * Types of struct fields should be maug, native, or defined earlier in the
 *   same header. Field types that are not any of these must be predefined in
 *   \ref maug_serial_msercust_page, with appropriate serialize/deserialize
 *   callbacks provided in another translation unit at link time.
 * 
 * There may be additional restrictions to these that are not noted here.
 *
 * \page maug_serial_msercust_page Serialize Tool Custom Types
 * Custom typedefs should be provided in msercust.h in the following format:
 *
 *     #  define parse_field_type_table( f ) \
 *           parse_field_type_table_base( f ) \
 *           f( custom_type_a,              101 ) \
 *           f( struct custom_type_b,       102 ) \
 *
 * In this example, `custom_type_a` is the name of the type and `101` is its ID.
 * The IDs start from 100 and there can be thousands.
 * 
 * If `custom_type_a` is provided here, a callback of type mserialize_cb_t()
 * and mdeserialize_cb_t() (called mseralize_custom_type_a and
 * mdeserialize_custom_type_b) must be provided by a linked translation unit.
 *
 * For `struct custom_type_b`, the callbacks would be called
 * mseralize_struct_custom_type_b() and mdeseralize_struct_custom_type_b().
 * \}
 */

#ifndef MSERIALIZE_TRACE_LVL
#  define MSERIALIZE_TRACE_LVL 0
#endif /* !MSERIALIZE_TRACE_LVL */

#include "mrapiser.h"

/*! \brief Parameter type for mserialize_header() for objects. */
#define MSERIALIZE_TYPE_OBJECT   1
/*! \brief Parameter type for mserialize_header() for character strings. */
#define MSERIALIZE_TYPE_STRING   2
/*! \brief Parameter type for mserialize_header() for integers. */
#define MSERIALIZE_TYPE_INTEGER  3
/*! \brief Parameter type for mserialize_header() for floating-point numbers. */
#define MSERIALIZE_TYPE_FLOAT    4
/*! \brief Parameter type for mserialize_header() for arrays. */
#define MSERIALIZE_TYPE_ARRAY    5
/*! \brief Parameter type for mserialize_header() for opaque binary blobs. */
#define MSERIALIZE_TYPE_BLOB     6

/**
 * \brief Serialization function for a struct or primative type.
 * \param ser_out File object to write serialized struct to.
 * \param p_ser_val Pointer to the struct to serialize.
 * \param array Number of items in the array to serialize.
 *
 * Code that calls this kind of function generally knows what specific struct
 * it's serializing, as it's called recursively to handle fields that are e.g.
 * pre-defined as arrays.
 *
 * \warning Code in callbacks tethered in through
 *          \ref maug_serial_msercust_page should refrain from writing directly
 *          to ser_out, unless they are written with a specific format in mind
 *          and that format is sure to be the only format linked. They should
 *          instead call other mserialize_/mdeserialize_ callbacks defined
 *          by the format-specific API!
 */
typedef MERROR_RETVAL (*mserialize_cb_t)(
   mfile_t* ser_out, void* p_ser_val, int array  );

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
   mfile_t* ser_in, uint8_t* p_type, ssize_t* p_sz, size_t* p_header_sz );

MERROR_RETVAL mdeserialize_int(
   mfile_t* ser_out, int32_t* p_ser_int, int array, ssize_t* p_ser_sz );

MERROR_RETVAL mdeserialize_vector(
   mfile_t* ser_f, struct MDATA_VECTOR* p_ser_vec, mdeserialize_cb_t cb,
   uint8_t* buf, size_t buf_sz, ssize_t* p_ser_sz );

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

/*! \} */ /* maug_serial */

#endif /* !MSERIAL_H */

