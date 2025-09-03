
#if !defined( MAUG_API_SER_H_DEFS )
#define MAUG_API_SER_H_DEFS

#define MSERIAL_ASN_TYPE_STRING     0x16
#define MSERIAL_ASN_TYPE_INT        0x02
#define MSERIAL_ASN_TYPE_BLOB       0x04
#define MSERIAL_ASN_TYPE_SEQUENCE   0x30

#elif defined( MSERIAL_C )

static MERROR_RETVAL _mserialize_asn_int_value(
   mfile_t* p_file, int32_t value, int8_t value_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t i = 0;
   uint8_t int_buf = 0;

   assert( 0 < value_sz );

   for( i = value_sz - 1 ; 0 <= i ; i-- ) {
      /* Grab the i'th byte of the value from left and place it in the buffer.
       */
      int_buf = ((value >> (i * 8)) & 0xff);
      retval = p_file->write_block( p_file, &int_buf, 1 );
      maug_cleanup_if_not_ok();
      debug_printf( MSERIAL_TRACE_LVL,
         "wrote byte #%d to buffer: 0x%02x", int_buf );
   }

cleanup:

   return retval;
}

/* === */

static int8_t _mserialize_asn_get_int_sz( int32_t value ) {
   int8_t val_sz = -1;

   if( value <= 0x7f ) {
      /* TODO: 0x7f only for integer types? */
      val_sz = 1;
      
   } else if( value == (value & 0xffff) ) {
      val_sz = 2;

   } else if( value == (value & 0xffffff) ) {
      val_sz = 3;

   } else if( value == (value & 0xffffffff) ) {
      val_sz = 4;
   }

   return val_sz;
}

/* === */

MERROR_RETVAL _mserialize_asn_int(
   mfile_t* ser_out, size_t* p_sz, int32_t value, int array
) {
   MERROR_RETVAL retval = MERROR_OK;
   int8_t val_sz = 0;
   uint8_t type_val = MSERIAL_ASN_TYPE_INT;

   val_sz = _mserialize_asn_get_int_sz( value );
   if( 0 >= val_sz ) {
      error_printf( "invalid value size" );
      retval = MERROR_FILE;
      goto cleanup;
   }
   debug_printf( MSERIAL_TRACE_LVL,
      "serializing integer value %d (0x%02x, %d byte(s))",
      value, value, val_sz );

   assert( 0 < val_sz );

   /* TODO: Warning if value > INT32_MAX? */

   /* Write the integer type to the buffer. */
   if( 0 > value ) {
      type_val |= 0x40;
      value *= -1;
      debug_printf( MSERIAL_TRACE_LVL,
         "value %d (0x%02x) type after neg is %02x", value, value, type_val );
   }

   /* Write the type and size of the integer to the buffer. */
   ser_out->write_block( ser_out, &type_val, 1 );
   ser_out->write_block( ser_out, (uint8_t*)&val_sz, 1 );

   /* Write the actual value to the buffer. */
   retval = _mserialize_asn_int_value( ser_out, value, val_sz );

   debug_printf( MSERIAL_TRACE_LVL, "serialized integer value %d.", value );

cleanup:

   return retval;
}

/* === */

off_t mserialize_header( mfile_t* ser_out, uint8_t type, uint8_t flags ) {
   /* TODO */
}

/* === */

MERROR_RETVAL mserialize_footer(
   mfile_t* ser_out, off_t header, uint8_t flags
) {
   /* TODO */
}

/* === */

MERROR_RETVAL mserialize_size_t(
   mfile_t* ser_out, size_t* p_sz, size_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_ssize_t(
   mfile_t* ser_out, size_t* p_sz, ssize_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_uint8_t(
   mfile_t* ser_out, size_t* p_sz, uint8_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_int8_t(
   mfile_t* ser_out, size_t* p_sz, int8_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_uint16_t(
   mfile_t* ser_out, size_t* p_sz, uint16_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_int16_t(
   mfile_t* ser_out, size_t* p_sz, int16_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_uint32_t(
   mfile_t* ser_out, size_t* p_sz, uint32_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_int32_t(
   mfile_t* ser_out, size_t* p_sz, int32_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_char(
   mfile_t* ser_out, size_t* p_sz, char* p_ser_char, int array 
) {
   /* TODO */
}

MERROR_RETVAL mserialize_retroflat_asset_path(
   mfile_t* ser_out, size_t* p_sz, retroflat_asset_path* p_ser_char, int array 
) {
   /* TODO */
}

MERROR_RETVAL mserialize_mfix_t(
   mfile_t* ser_out, size_t* p_sz, mfix_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_retrotile_coord_t(
   mfile_t* ser_out, size_t* p_sz, retrotile_coord_t* p_ser_int, int array 
) {
   /* TODO */
}

MERROR_RETVAL mserialize_float(
   mfile_t* ser_out, size_t* p_sz, float* p_ser_float, int array 
) {
   /* TODO */
}

MERROR_RETVAL mserialize_struct_RETROTILE_COORDS(
   mfile_t* ser_out, size_t* p_sz, struct RETROTILE_COORDS* p_ser_struct, int array 
) {
   /* TODO */
}

MERROR_RETVAL mserialize_retroflat_dir4_t(
   mfile_t* ser_out, size_t* p_sz, retroflat_dir4_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, p_sz, *p_ser_int, array );
}

MERROR_RETVAL mserialize_struct_MLISP_EXEC_STATE(
   mfile_t* ser_out, size_t* p_sz, struct MLISP_EXEC_STATE* p_ser_struct, int array 
) {
   /* TODO */
}

MERROR_RETVAL mserialize_vector_struct_MLISP_EXEC_STATE(
   mfile_t* ser_out, size_t* p_sz, struct MDATA_VECTOR* p_ser_vec
) {
   /* TODO */
}

MERROR_RETVAL mserialize_vector_struct_MLISP_ENV_NODE(
   mfile_t* ser_out, size_t* p_sz, struct MDATA_VECTOR* p_ser_vec
) {
   /* TODO */
}

#endif /* !MAUG_API_SER_H_DEFS */

