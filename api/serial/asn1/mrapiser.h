
#if !defined( MAUG_API_SER_H_DEFS )
#define MAUG_API_SER_H_DEFS

#define MSERIALIZE_ASN_TYPE_STRING     0x16
#define MSERIALIZE_ASN_TYPE_INT        0x02
#define MSERIALIZE_ASN_TYPE_BLOB       0x04
#define MSERIALIZE_ASN_TYPE_SEQUENCE   0x30
#define MSERIALIZE_ASN_TYPE_REAL       0x09

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
      if( (value_sz - 1) == i && 0 > value && 0 == int_buf ) {
         /* Two's complement buffer to disambiguate that this is negative. */
         int_buf = 0xff;
      }
      retval = p_file->write_block( p_file, &int_buf, 1 );
      maug_cleanup_if_not_ok();
#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "serialized byte #%d of %d: 0x%02x", value_sz - i, value, int_buf );
#endif /* MSERIALIZE_TRACE_LVL */
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
      
   } else if( value == (value & 0x7fff) ) {
      val_sz = 2;

   } else if( value == (value & 0x7fffff) ) {
      val_sz = 3;

   } else if( value == (value & 0x7fffffff) ) {
      val_sz = 4;
   }

   return val_sz;
}

/* === */

MERROR_RETVAL _mserialize_asn_sz( mfile_t* ser_out, size_t sz ) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t sz_of_sz = 0;
   uint8_t sz_of_sz_asn = 0;

#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL, "writing size: " SIZE_T_FMT, sz );
#endif /* MSERIALIZE_TRACE_LVL */

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

MERROR_RETVAL mserialize_int( mfile_t* ser_out, int32_t value, int array ) {
   MERROR_RETVAL retval = MERROR_OK;
   int8_t val_sz = 0;
   uint8_t type_val = MSERIALIZE_ASN_TYPE_INT;

   val_sz = _mserialize_asn_get_int_sz( value );
   if( 0 >= val_sz ) {
      error_printf( "invalid value size" );
      retval = MERROR_FILE;
      goto cleanup;
   }
#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL,
      "serializing integer value %d (0x%02x, %d byte(s))",
      value, value, val_sz );
#endif /* MSERIALIZE_TRACE_LVL */

   assert( 0 < val_sz );

   /* TODO: Warning if value > INT32_MAX? */

   /* Write the integer type to the buffer. */
   if( 0 > value ) {
      type_val |= 0x40;
      value *= -1;
#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "value %d (0x%02x) type after neg is %02x", value, value, type_val );
#endif /* MSERIALIZE_TRACE_LVL */
   }

   /* Write the type and size of the integer to the buffer. */
   retval = ser_out->write_block( ser_out, &type_val, 1 );
   maug_cleanup_if_not_ok();
   retval = _mserialize_asn_sz( ser_out, val_sz );
   maug_cleanup_if_not_ok();

   /* Write the actual value to the buffer. */
   retval = _mserialize_asn_int_value( ser_out, value, val_sz );

#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL, "serialized integer value %d.", value );
#endif /* MSERIALIZE_TRACE_LVL */

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mserialize_vector(
   mfile_t* ser_f, struct MDATA_VECTOR* p_ser_vec, mserialize_cb_t cb
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   off_t header = 0;
   int autolock = 0;
   
#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL, "serializing vector %p of %d...",
      p_ser_vec, mdata_vector_ct( p_ser_vec ) );
#endif /* MSERIALIZE_TRACE_LVL */
   
   header = mserialize_header( ser_f, MSERIALIZE_TYPE_ARRAY, 0 );
   
   if( 0 == mdata_vector_ct( p_ser_vec ) ) {
      /* Close empty vector. */
      retval = mserialize_footer( ser_f, header, 0 );
#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL, "serialized empty vector." );
#endif /* MSERIALIZE_TRACE_LVL */
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
#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL, "serialized vector %p.", p_ser_vec );
#endif /* MSERIALIZE_TRACE_LVL */

cleanup:
   if( autolock ) {
      mdata_vector_unlock( p_ser_vec );
   }
   return retval;
}

/* === */

MERROR_RETVAL mserialize_table(
   mfile_t* ser_f, struct MDATA_TABLE* p_ser_tab, mserialize_cb_t cb
) {
   /* TODO */
}

/* === */

MERROR_RETVAL mserialize_block(
   mfile_t* ser_f, void* p_block, size_t block_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   off_t header = 0;
   
#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL,
      "serializing block of " SIZE_T_FMT " bytes...",
      block_sz );
#endif /* MSERIALIZE_TRACE_LVL */

   header = mserialize_header( ser_f, MSERIALIZE_TYPE_BLOB, 0 );

   retval = ser_f->write_block( ser_f, p_block, block_sz );
   maug_cleanup_if_not_ok();
   
   retval = mserialize_footer( ser_f, header, 0 );
   maug_cleanup_if_not_ok();
   
#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL,
      "serialized block of " SIZE_T_FMT " bytes.", block_sz );
#endif /* MSERIALIZE_TRACE_LVL */

cleanup:
   return retval;
}

/* === */

off_t mserialize_header( mfile_t* ser_out, uint8_t type, uint8_t flags ) {
   MERROR_RETVAL retval = MERROR_USR;
   uint8_t asn_seq = MSERIALIZE_ASN_TYPE_SEQUENCE;
   
   /* Don't let us use this for integers! */
   assert(
      MSERIALIZE_TYPE_ARRAY == type ||
      MSERIALIZE_TYPE_OBJECT == type ||
      MSERIALIZE_TYPE_STRING == type ||
      MSERIALIZE_TYPE_BLOB == type );
 
   switch( type ) {
   case MSERIALIZE_TYPE_ARRAY:
      break;

   case MSERIALIZE_TYPE_OBJECT:
      break;

   case MSERIALIZE_TYPE_STRING:
      asn_seq = MSERIALIZE_ASN_TYPE_STRING;
      break;

   case MSERIALIZE_TYPE_BLOB:
      asn_seq = MSERIALIZE_ASN_TYPE_BLOB;
      break;

   default:
      goto cleanup;
   }

   retval = ser_out->write_block( ser_out, &asn_seq, 1 );

#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL,
      "beginning sequence with type: 0x%02x", asn_seq );
#endif /* MSERIALIZE_TRACE_LVL */

   assert( MERROR_OK == retval );

cleanup:

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

   seq_sz = ser_out->sz - header;
#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL,
      "ending sequence of " SIZE_T_FMT " bytes", seq_sz );
#endif /* MSERIALIZE_TRACE_LVL */

   assert( ser_out->cursor( ser_out ) == ser_out->sz );

   ser_out->seek( ser_out, header );
   retval = _mserialize_asn_sz( ser_out, seq_sz );
   ser_out->seek( ser_out, ser_out->sz );

   assert( MERROR_OK == retval );

   return retval;

}

/* === */

MERROR_RETVAL mserialize_char(
   mfile_t* ser_out, char* p_ser_char, int array 
) {
   MERROR_RETVAL retval = MERROR_OK;
   off_t header = 0;

   if( 1 < array ) {
#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "serializing string: %s", p_ser_char );
#endif /* MSERIALIZE_TRACE_LVL */
      header = mserialize_header( ser_out, MSERIALIZE_TYPE_STRING, 0 );
      retval = ser_out->write_block(
         ser_out, (uint8_t*)p_ser_char, strlen( p_ser_char ) );
      maug_cleanup_if_not_ok();
      retval = mserialize_footer( ser_out, header, 0 );
   } else {
#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "serializing character: %c", *p_ser_char );
#endif /* MSERIALIZE_TRACE_LVL */
      retval = mserialize_int( ser_out, *p_ser_char, 1 );
   }

   assert( MERROR_OK == retval );

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

MERROR_RETVAL mserialize_float(
   mfile_t* ser_out, float* p_ser_float, int array 
) {
   /* TODO */
   return MERROR_FILE;
}

#if 0
MERROR_RETVAL mserialize_struct_MLISP_ENV_NODE(
   mfile_t* ser_out, struct MLISP_ENV_NODE* p_ser_struct, int array
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   off_t header = 0,
      header_array = 0;

   if( 1 < array ) {
      header_array = mserialize_header( ser_out, MSERIALIZE_TYPE_ARRAY, 0 );
   }
   
   for( i = 0 ; array > i ; i++ ) {
      /* Skip env nodes that are builtins, as they should be added back by
       * the system when deserialized so as to have valid e.g. pointers to
       * callback functions!
       */
      /* TODO: When deserializing, be sure to put built-ins back in FIRST,
       *       to preserve possible later defines/overrides! */
      if(
         MLISP_ENV_FLAG_BUILTIN ==
         (MLISP_ENV_FLAG_BUILTIN & p_ser_struct->flags)
      ) {
#if MSERIALIZE_TRACE_LVL > 0
         debug_printf( MSERIALIZE_TRACE_LVL,
            "skipping serializing builtin env node: " SIZE_T_FMT, i );
#endif /* MSERIALIZE_TRACE_LVL */
         continue;
      }

#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "serializing struct MLISP_ENV_NODE..." );
#endif /* MSERIALIZE_TRACE_LVL */

      header = mserialize_header( ser_out, MSERIALIZE_TYPE_OBJECT, 0 );
      
#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL, "serializing field: flags" );
#endif /* MSERIALIZE_TRACE_LVL */
      retval = mserialize_uint8_t( ser_out, &(p_ser_struct->flags), 1 );
      maug_cleanup_if_not_ok();
      
#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL, "serializing field: type" );
#endif /* MSERIALIZE_TRACE_LVL */
      retval = mserialize_uint8_t( ser_out, &(p_ser_struct->type), 1 );
      maug_cleanup_if_not_ok();

#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL, "serializing field: value" );
#endif /* MSERIALIZE_TRACE_LVL */
      retval = mserialize_union_MLISP_VAL( ser_out, &(p_ser_struct->value), 1 );
      maug_cleanup_if_not_ok();

      retval = mserialize_footer( ser_out, header, 0 );
      maug_cleanup_if_not_ok();

#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL, "serialized struct MLISP_ENV_NODE." );
#endif /* MSERIALIZE_TRACE_LVL */
   }
   
   if( 1 < array ) {
      retval = mserialize_footer( ser_out, header_array, 0 );
   }

cleanup:
   return retval;
}
#endif

MERROR_RETVAL mserialize_union_MLISP_VAL(
   mfile_t* ser_out, union MLISP_VAL* p_ser_val, int array 
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   for( i = 0 ; array > i ; i++ ) {
      retval = mserialize_block(
         ser_out, p_ser_val, sizeof( union MLISP_VAL ) );
      maug_cleanup_if_not_ok();
   }

cleanup:
   return retval;
}

/* === */

static MERROR_RETVAL _mdeserialize_asn_sz(
   mfile_t* ser_in, int32_t* p_sz, size_t* p_value_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   uint8_t byte_buf = 0;

   retval = ser_in->read_block( ser_in, &byte_buf, 1 );
   maug_cleanup_if_not_ok_msg( "error reading integer size" );

   *p_value_sz = 0;
   *p_sz = 0;

   if( 0x80 == (0x80 & byte_buf) ) {
      /* This is actually the size of the size. */
      *p_value_sz = byte_buf & ~0x80;

#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL, "value_sz determined: " SIZE_T_FMT,
         *p_value_sz );
#endif /* MSERIALIZE_TRACE_LVL */

      assert( *p_value_sz <= 4 );

      for( i = 0 ; *p_value_sz > i ; i++ ) {
         /* TODO: Detect if first octet is 0xff (negative). */

         /* Left-shift first to make more room. */
         *p_sz <<= 8;

         retval = ser_in->read_block( ser_in, &byte_buf, 1 );
         maug_cleanup_if_not_ok_msg( "error reading field size" );

         *p_sz |= byte_buf;
      }

      /* Add 1 for the size of size. */
      *p_value_sz += 1;
   } else {
      /* No high bit, so it's a single-bit size or raw integer. */
      *p_sz = byte_buf;
      *p_value_sz = 1;
   }

cleanup:
   return retval;
}

static MERROR_RETVAL _mdeserialize_asn_int_value(
   mfile_t* ser_in, int32_t* p_value, size_t value_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t byte_buf = 0;
   size_t i = 0;

   assert( value_sz <= 4 );
   assert( value_sz > 0 );

   *p_value = 0;

   for( i = 0 ; value_sz > i ; i++ ) {
      /* TODO: Detect if first octet is 0xff (negative). */

      /* Left-shift first to make more room. */
      *p_value <<= 8;

      retval = ser_in->read_block( ser_in, &byte_buf, 1 );
      maug_cleanup_if_not_ok_msg( "error reading field size" );

      *p_value |= byte_buf;
   }

cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_header(
   mfile_t* ser_in, uint8_t* p_type, ssize_t* p_sz, size_t* p_header_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   int32_t sz_buf = 0;
#if MSERIALIZE_TRACE_LVL > 0
   off_t offset = 0;
   
   offset = ser_in->cursor( ser_in );
#endif /* MSERIALIZE_TRACE_LVL */
   retval = ser_in->read_block( ser_in, p_type, 1 );
   maug_cleanup_if_not_ok_msg( "error reading field type" );

   retval = _mdeserialize_asn_sz( ser_in, &sz_buf, p_header_sz );
   *p_header_sz += 1; /* For type field. */
   maug_cleanup_if_not_ok();

   *p_sz = sz_buf;

#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL,
      "offset: " OFF_T_FMT " (0x%04x), type: 0x%02x, sz: %d (0x%04x)",
      offset, offset, *p_type, sz_buf, sz_buf );
#endif /* MSERIALIZE_TRACE_LVL */

cleanup:

   return retval;
}

MERROR_RETVAL mdeserialize_int(
   mfile_t* ser_in, int32_t* p_int, int array, ssize_t* p_ser_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t type = 0;
   ssize_t sz_seq = 0;
   ssize_t sz = 0;
   size_t i = 0;
   size_t header_sz_seq = 0;
   size_t header_sz = 0;

   retval = mdeserialize_header( ser_in, &type, &sz, &header_sz_seq );
   maug_cleanup_if_not_ok();

   /* Setup sizes. Saying the seq size is the total size is fine since it's
    * unused if this isn't an array, anyway.
    */
   *p_ser_sz = sz + header_sz_seq;
   sz_seq = sz;

   for( i = 0 ; array > i ; i++ ) {
      if( array > 1 ) {
         /* Array-specific processing. If this is an array, then assume we're
          * embedded in the array right now.
          */
         if( 0 >= sz_seq ) {
            error_printf(
               "sequence contained fewer values (" SIZE_T_FMT " than expected ("
                  SIZE_T_FMT ")!",
               i, array );
            break;
         }

         retval = mdeserialize_header( ser_in, &type, &sz, &header_sz );
         maug_cleanup_if_not_ok();

         if( MSERIALIZE_ASN_TYPE_SEQUENCE == type ) {
            error_printf( "invalid int: detected sequence!" );
            retval = MERROR_FILE;
            goto cleanup;
         }
      }
      
      retval = _mdeserialize_asn_int_value( ser_in, &(p_int[i]), sz );
      maug_cleanup_if_not_ok();

#if MSERIALIZE_TRACE_LVL > 0
      if( array > 1 ) {
         debug_printf( MSERIALIZE_TRACE_LVL,
            "deserialized integer: %d (" SSIZE_T_FMT " + " SIZE_T_FMT " bytes)",
            p_int[i], sz, header_sz );
      } else {
         debug_printf( MSERIALIZE_TRACE_LVL,
            "deserialized integer: %d (" SSIZE_T_FMT " + " SIZE_T_FMT " bytes)",
            p_int[i], sz, header_sz_seq );
      }
#endif /* MSERIALIZE_TRACE_LVL */

      /* Detect if there are more values to deserialize. */
      sz_seq -= sz - header_sz;
   }

cleanup:

   return retval;
}

MERROR_RETVAL mdeserialize_vector(
   mfile_t* ser_in, struct MDATA_VECTOR* p_ser_vec, mdeserialize_cb_t cb,
   uint8_t* buf, size_t buf_sz, ssize_t* p_ser_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t type = 0;
   ssize_t sz_seq = 0;
   ssize_t sz = 0;
   size_t header_sz_seq = 0;
   size_t count = 0;
   ssize_t idx_added = 0;

#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL, "deserializing vector..." );
#endif /* MSERIALIZE_TRACE_LVL */

   retval = mdeserialize_header( ser_in, &type, &sz_seq, &header_sz_seq );
   maug_cleanup_if_not_ok();

   *p_ser_sz = sz + header_sz_seq;

   if( MSERIALIZE_ASN_TYPE_SEQUENCE != type ) {
      error_printf( "expected sequence! found: 0x%02x", type );
      retval = MERROR_FILE;
      goto cleanup;
   }

   while( 0 < sz_seq ) {
      debug_printf( MSERIALIZE_TRACE_LVL,
         "reading vector item " SIZE_T_FMT " (" SSIZE_T_FMT
            " bytes remaining)...",
         count, sz_seq );
      retval = cb( ser_in, buf, 1, &sz );
      maug_cleanup_if_not_ok();
#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "adding " SIZE_T_FMT "-byte item as "
            SIZE_T_FMT "-byte item to vector...",
         sz, buf_sz );
#endif /* MSERIALIZE_TRACE_LVL */
      idx_added = mdata_vector_append( p_ser_vec, buf, buf_sz );
      if( 0 > idx_added ) {
         retval = merror_sz_to_retval( idx_added );
         goto cleanup;
      }
      sz_seq -= sz;
      count++;
   }

#if MSERIALIZE_TRACE_LVL > 0
   debug_printf( MSERIALIZE_TRACE_LVL,
      "deserialized " SSIZE_T_FMT " values.", count );
#endif /* MSERIALIZE_TRACE_LVL */

cleanup:
   return retval;
}

MERROR_RETVAL mdeserialize_table(
   mfile_t* ser_in, struct MDATA_TABLE* p_ser_tab, mdeserialize_cb_t cb,
   uint8_t* buf, size_t buf_sz, ssize_t* p_ser_sz
) {
   /* TODO */
}

MERROR_RETVAL mdeserialize_float(
   mfile_t* ser_in, float* p_ser_float, int array, ssize_t* p_ser_sz 
) {
   /* TODO */
   return MERROR_FILE;
}

MERROR_RETVAL mdeserialize_char(
   mfile_t* ser_in, char* p_ser_char, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   uint8_t type = 0;
   ssize_t sz = 0;
   ssize_t cpy_sz = array;
   size_t header_sz_seq = 0;
   off_t start = 0;

   retval = mdeserialize_header( ser_in, &type, &sz, &header_sz_seq );
   maug_cleanup_if_not_ok();

   *p_ser_sz = header_sz_seq + sz;

   /* Grab the start to seek to if provided is more than expected. */
   start = ser_in->cursor( ser_in );

   /* Use the lesser of (expected/provided) sizes. */
   if( sz < cpy_sz ) {
      cpy_sz = sz;
   }

   ser_in->read_block( ser_in, (uint8_t*)p_ser_char, cpy_sz );

   /* Make sure we're focused on the next field when we're done. */
   ser_in->seek( ser_in, start + sz );

#if MSERIALIZE_TRACE_LVL > 0
   if( 1 < array ) {
      debug_printf(
         MSERIALIZE_TRACE_LVL, "deserialized string: %s", p_ser_char );
   } else {
      debug_printf(
         MSERIALIZE_TRACE_LVL, "deserialized char: %c", *p_ser_char );
   }
#endif /* MSERIALIZE_TRACE_LVL */

cleanup:

   return retval;
}

MERROR_RETVAL mdeserialize_retroflat_asset_path(
   mfile_t* ser_in, retroflat_asset_path* p_ser_char, int array,
   ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   ssize_t path_sz;

   *p_ser_sz = 0;

   for( i = 0 ; array > i ; i++ ) {
      retval = mdeserialize_char(
         ser_in, p_ser_char[i], MAUG_PATH_SZ_MAX, &path_sz );
      maug_cleanup_if_not_ok();
      *p_ser_sz += path_sz;
   }

cleanup:

   return retval;
}

#if 0
MERROR_RETVAL mdeserialize_struct_MLISP_ENV_NODE(
   mfile_t* ser_in, struct MLISP_ENV_NODE* p_ser_struct, int array,
   ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   ssize_t seq_sz = 0;
   ssize_t struct_sz = 0;
   ssize_t struct_remaining = 0;
   ssize_t field_sz = 0;
   size_t seq_header_sz = 0;
   size_t header_sz = 0;
   uint8_t struct_type = 0;
   uint8_t type = 0;

   maug_mzero( p_ser_struct, sizeof( struct MLISP_ENV_NODE ) * array );
   retval = mdeserialize_header(
      ser_in, &struct_type, &struct_sz, &seq_header_sz );
   maug_cleanup_if_not_ok();

   *p_ser_sz = struct_sz + seq_header_sz;
   struct_remaining = struct_sz;
   seq_sz = struct_sz;
   
#if MSERIALIZE_TRACE_LVL > 0
   if( array > 1 ) {
      debug_printf( MSERIALIZE_TRACE_LVL,
         "deserializing array of %d struct MLISP_ENV_NODE...", array );
   }
#endif

   for( i = 0 ; array > i ; i++ ) {
      if( array > 1 ) {
         if( 0 >= seq_sz ) {
            error_printf(
               "sequence contained fewer values (" SIZE_T_FMT
                  " than expected (" SIZE_T_FMT ")!",
               i, array );
            break;
         }
         retval = mdeserialize_header( ser_in, &type, &struct_sz, &header_sz );
         maug_cleanup_if_not_ok();
      }

#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "deserializing struct MLISP_ENV_NODE (" SSIZE_T_FMT " bytes)...",
         struct_sz );
      debug_printf( MSERIALIZE_TRACE_LVL, "deserializing field: flags" );
#endif /* MSERIALIZE_TRACE_LVL */
      field_sz = 0;
      retval = mdeserialize_uint8_t(
         ser_in, &(p_ser_struct->flags), 1, &field_sz );
      maug_cleanup_if_not_ok();
      struct_remaining -= field_sz;
      if( 0 >= struct_remaining ) {
         error_printf( "struct MLISP_ENV_NODE was smaller than expected!" );
         goto cleanup;
      }

#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL, "deserializing field: type" );
#endif /* MSERIALIZE_TRACE_LVL */
      field_sz = 0;
      retval = mdeserialize_uint8_t(
         ser_in, &(p_ser_struct->type), 1, &field_sz );
      maug_cleanup_if_not_ok();
      struct_remaining -= field_sz;
      if( 0 >= struct_remaining ) {
         error_printf( "struct MLISP_ENV_NODE was smaller than expected!" );
         goto cleanup;
      }

#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "deserializing field: value" );
#endif /* MSERIALIZE_TRACE_LVL */
      field_sz = 0;
      retval = mdeserialize_union_MLISP_VAL(
         ser_in, &(p_ser_struct->value), 1, &field_sz );
      maug_cleanup_if_not_ok();
      struct_remaining -= field_sz;
      if( 0 >= struct_remaining ) {
         error_printf( "struct MLISP_ENV_NODE was smaller than expected!" );
         goto cleanup;
      }

#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "deserialized struct MLISP_ENV_NODE." );
#endif /* MSERIALIZE_TRACE_LVL */
      seq_sz -= (struct_sz + header_sz);
   }

cleanup:
   return retval;
}
#endif

MERROR_RETVAL mdeserialize_union_MLISP_VAL(
   mfile_t* ser_in, union MLISP_VAL* p_ser_union, int array, ssize_t* p_ser_sz 
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   ssize_t seq_sz = 0;
   ssize_t union_sz = 0;
   ssize_t union_remaining = 0;
   size_t seq_header_sz = 0;
   size_t header_sz = 0;
   uint8_t union_type = 0;
   uint8_t type = 0;

   maug_mzero( p_ser_union, sizeof( union MLISP_VAL ) * array );
   retval = mdeserialize_header(
      ser_in, &union_type, &union_sz, &seq_header_sz );
   maug_cleanup_if_not_ok();

   *p_ser_sz = union_sz + seq_header_sz;
   union_remaining = union_sz;
   seq_sz = union_sz;
   
#if MSERIALIZE_TRACE_LVL > 0
   if( array > 1 ) {
      debug_printf( MSERIALIZE_TRACE_LVL,
         "deserializing array of %d union MLISP_VAL...", array );
   }
#endif

   for( i = 0 ; array > i ; i++ ) {
      if( array > 1 ) {
         if( 0 >= seq_sz ) {
            error_printf(
               "sequence contained fewer values (" SIZE_T_FMT
                  " than expected (" SIZE_T_FMT ")!",
               i, array );
            break;
         }
         retval = mdeserialize_header( ser_in, &type, &union_sz, &header_sz );
         maug_cleanup_if_not_ok();
      }

#if MSERIALIZE_TRACE_LVL > 0
      debug_printf( MSERIALIZE_TRACE_LVL,
         "deserializing union MLISP_VAL (" SSIZE_T_FMT " bytes)...",
         union_sz );
#endif /* MSERIALIZE_TRACE_LVL */
      retval = ser_in->read_block( ser_in, (uint8_t*)&p_ser_union, union_sz );
      maug_cleanup_if_not_ok();
      union_remaining -= union_sz;
      if( 0 >= union_remaining ) {
         error_printf( "union MLISP_VAL was smaller than expected!" );
         goto cleanup;
      }
   }

cleanup:

   return retval;
}

#endif /* !MAUG_API_SER_H_DEFS */

