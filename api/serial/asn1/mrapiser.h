
#if !defined( MAUG_API_SER_H_DEFS )
#define MAUG_API_SER_H_DEFS

#define MSERIALIZE_ASN_TYPE_STRING     0x16
#define MSERIALIZE_ASN_TYPE_INT        0x02
#define MSERIALIZE_ASN_TYPE_BLOB       0x04
#define MSERIALIZE_ASN_TYPE_SEQUENCE   0x30

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
      debug_printf( MSERIALIZE_TRACE_LVL,
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

MERROR_RETVAL _mserialize_asn_sz( mfile_t* ser_out, size_t sz ) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t sz_of_sz = 0;
   uint8_t sz_of_sz_asn = 0;

   /* TODO: Boundary checking for int32_t vs size_t. */
   sz_of_sz = _mserialize_asn_get_int_sz( sz );

   if( 127 < sz ) {
      /* 0x80 | size of size, followed by size. */
      sz_of_sz_asn = 0x80 | sz_of_sz;
      ser_out->write_block( ser_out, &sz_of_sz_asn, 1 );
      retval = _mserialize_asn_int_value( ser_out, sz, sz_of_sz );
   } else {
      retval = ser_out->write_block( ser_out, (uint8_t*)&sz, 1 );
   }

   return retval;
}

/* === */

MERROR_RETVAL _mserialize_asn_int( mfile_t* ser_out, int32_t value, int array ) {
   MERROR_RETVAL retval = MERROR_OK;
   int8_t val_sz = 0;
   uint8_t type_val = MSERIALIZE_ASN_TYPE_INT;

   val_sz = _mserialize_asn_get_int_sz( value );
   if( 0 >= val_sz ) {
      error_printf( "invalid value size" );
      retval = MERROR_FILE;
      goto cleanup;
   }
   debug_printf( MSERIALIZE_TRACE_LVL,
      "serializing integer value %d (0x%02x, %d byte(s))",
      value, value, val_sz );

   assert( 0 < val_sz );

   /* TODO: Warning if value > INT32_MAX? */

   /* Write the integer type to the buffer. */
   if( 0 > value ) {
      type_val |= 0x40;
      value *= -1;
      debug_printf( MSERIALIZE_TRACE_LVL,
         "value %d (0x%02x) type after neg is %02x", value, value, type_val );
   }

   /* Write the type and size of the integer to the buffer. */
   retval = ser_out->write_block( ser_out, &type_val, 1 );
   maug_cleanup_if_not_ok();
   retval = _mserialize_asn_sz( ser_out, val_sz );
   maug_cleanup_if_not_ok();

   /* Write the actual value to the buffer. */
   retval = _mserialize_asn_int_value( ser_out, value, val_sz );

   debug_printf( MSERIALIZE_TRACE_LVL, "serialized integer value %d.", value );

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL _mserialize_asn_vector_int(
   mfile_t* ser_f, struct MDATA_VECTOR* p_ser_vec, mserialize_cb_t cb
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   off_t header = 0;
   int autolock = 0;
   debug_printf( MSERIALIZE_TRACE_LVL, "serializing vector of integers..." );
   header = mserialize_header( ser_f, MSERIALIZE_TYPE_ARRAY, 0 );
   if( 0 == mdata_vector_ct( p_ser_vec ) ) {
      retval = mserialize_footer( ser_f, header, 0 );
      return retval;
   }
   if( !mdata_vector_is_locked( p_ser_vec ) ) {
      mdata_vector_lock( p_ser_vec );
      autolock = 1;
   }
   for( i = 0 ; mdata_vector_ct( p_ser_vec ) > i ; i++ ) {
      retval = cb( ser_f, mdata_vector_get_void( p_ser_vec, i ), 1 );
      maug_cleanup_if_not_ok();
   }
   retval = mserialize_footer( ser_f, header, 0 );
   debug_printf( MSERIALIZE_TRACE_LVL, "serialized vector of integers." );
cleanup:
   if( autolock ) {
      mdata_vector_unlock( p_ser_vec );
   }
   return retval;
}

/* === */

off_t mserialize_header( mfile_t* ser_out, uint8_t type, uint8_t flags ) {
   MERROR_RETVAL retval = MERROR_USR;
   uint8_t asn_seq = MSERIALIZE_ASN_TYPE_SEQUENCE;

   switch( type ) {
   case MSERIALIZE_TYPE_OBJECT:
      retval = ser_out->write_block( ser_out, &asn_seq, 1 );
      break;

   case MSERIALIZE_TYPE_STRING:
      asn_seq = MSERIALIZE_ASN_TYPE_STRING;
      retval = ser_out->write_block( ser_out, &asn_seq, 1 );
      break;
   }

   if( MERROR_OK == retval ) {
      return ser_out->cursor( ser_out );
   } else {
      return 0;
   }
}

/* === */

MERROR_RETVAL mserialize_footer(
   mfile_t* ser_out, off_t header, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t seq_sz = 0;

   if( 0 == header ) {
      /* Invalid header value. Must not need a header! */
      goto cleanup;
   }
   
   seq_sz = ser_out->sz - header;
   debug_printf( MSERIALIZE_TRACE_LVL,
      "ending sequence of " SIZE_T_FMT " bytes", seq_sz );

   assert( ser_out->cursor( ser_out ) == ser_out->sz );

   ser_out->seek( ser_out, header );
   retval = _mserialize_asn_sz( ser_out, seq_sz );
   ser_out->seek( ser_out, ser_out->sz );

cleanup:

   return retval;

}

/* === */

MERROR_RETVAL mserialize_size_t(
   mfile_t* ser_out, size_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_vector_size_t(
   mfile_t* ser_out, struct MDATA_VECTOR* p_ser_vec
) {
   return _mserialize_asn_vector_int(
      ser_out, p_ser_vec, (mserialize_cb_t)mserialize_size_t );
}

MERROR_RETVAL mserialize_ssize_t(
   mfile_t* ser_out, ssize_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_uint8_t(
   mfile_t* ser_out, uint8_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_int8_t(
   mfile_t* ser_out, int8_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_uint16_t(
   mfile_t* ser_out, uint16_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_int16_t(
   mfile_t* ser_out, int16_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_uint32_t(
   mfile_t* ser_out, uint32_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_int32_t(
   mfile_t* ser_out, int32_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_char(
   mfile_t* ser_out, char* p_ser_char, int array 
) {
   MERROR_RETVAL retval = MERROR_OK;
   off_t header = 0;

   if( 1 < array ) {
      debug_printf( MSERIALIZE_TRACE_LVL,
         "serializing string: %s", p_ser_char );
      header = mserialize_header( ser_out, MSERIALIZE_TYPE_STRING, 0 );
      retval = ser_out->write_block(
         ser_out, (uint8_t*)p_ser_char, strlen( p_ser_char ) );
      maug_cleanup_if_not_ok();
      retval = mserialize_footer( ser_out, header, 0 );
   } else {
      debug_printf( MSERIALIZE_TRACE_LVL,
         "serializing character: %c", *p_ser_char );
      retval = _mserialize_asn_int( ser_out, *p_ser_char, 1 );
   }

cleanup:

   return retval;
}

MERROR_RETVAL mserialize_retroflat_asset_path(
   mfile_t* ser_out, retroflat_asset_path* p_ser_char, int array 
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   for( i = 0 ; array > i ; i++ ) {
      retval = mserialize_char( ser_out, p_ser_char[i], MAUG_PATH_SZ_MAX );
      maug_cleanup_if_not_ok();
   }

cleanup:
   return retval;
}

MERROR_RETVAL mserialize_mfix_t(
   mfile_t* ser_out, mfix_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_retrotile_coord_t(
   mfile_t* ser_out, retrotile_coord_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_mdata_strpool_idx_t (
   mfile_t* ser_out, mdata_strpool_idx_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_float(
   mfile_t* ser_out, float* p_ser_float, int array 
) {
   /* TODO */
}

MERROR_RETVAL mserialize_struct_RETROTILE_COORDS(
   mfile_t* ser_out, struct RETROTILE_COORDS* p_ser_struct, int array 
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   off_t header = 0;
   debug_printf( MSERIALIZE_TRACE_LVL,
      "serializing struct RETROTILE_COORDS..." );
   header = mserialize_header( ser_out, MSERIALIZE_TYPE_OBJECT, 0 );
   for( i = 0 ; array > i ; i++ ) {
      debug_printf( MSERIALIZE_TRACE_LVL, "serializing field: x" );
      retval = mserialize_retrotile_coord_t( ser_out, &(p_ser_struct->x), 1 );
      maug_cleanup_if_not_ok();
      debug_printf( MSERIALIZE_TRACE_LVL, "serializing field: y" );
      retval = mserialize_retrotile_coord_t( ser_out, &(p_ser_struct->y), 1 );
      maug_cleanup_if_not_ok();
   }
   retval = mserialize_footer( ser_out, header, 0 );
   debug_printf( MSERIALIZE_TRACE_LVL, "serialized struct BASE_DEF." );
cleanup:
   return retval;
}

MERROR_RETVAL mserialize_retroflat_dir4_t(
   mfile_t* ser_out, retroflat_dir4_t* p_ser_int, int array 
) {
   return _mserialize_asn_int( ser_out, *p_ser_int, array );
}

MERROR_RETVAL mserialize_union_MLISP_VAL(
   mfile_t* ser_out, union MLISP_VAL* p_ser_val, int array 
) {
}

#endif /* !MAUG_API_SER_H_DEFS */

