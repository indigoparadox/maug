
#ifndef MCSSMERG_H
#define MCSSMERG_H

#ifndef MCSSMERG_TRACE_LVL
#  define MCSSMERG_TRACE_LVL 0
#endif /* !MCSSMERG_TRACE_LVL */

void mcssmerge_styles(
   struct MCSS_STYLE* effect_style,
   struct MCSS_STYLE* parent_style,
   struct MCSS_STYLE* tag_style,
   size_t tag_type );

#ifdef MCSSMERG_C

/* This is primarily used in retrohtr, but on its own causes the retrohtr.c
 * stub segment to become too large because of its use of the preprocessor.
 */

void mcssmerge_merge_prop(
   int p_id,
   const char* prop_name,
   size_t prop_sz,
   size_t tag_type,
   void* parent_prop, uint8_t* parent_prop_flags,
   void* effect_prop, uint8_t* effect_prop_flags,
   void* tag_prop, uint8_t* tag_prop_flags
) {
   if(
      (
         /* Only do inheritence for some special cases.
          * e.g. We don't want to inherit width/height/X/etc! */
         mcss_prop_is_heritable( p_id )
      ) && (NULL != parent_prop && (
         (
            NULL != tag_prop &&
            /* Parent is important and new property isn't. */
            mcss_prop_is_active_flag( *parent_prop, IMPORTANT ) &&
            /* TODO: Is not active OR important? */
            !mcss_prop_is_important( *tag_prop ) &&
            !mcss_prop_is_important( *effect_prop )
         ) || (
            NULL != tag_prop &&
            /* New property is not active. */
            !mcss_prop_is_active( *tag_prop ) &&
            !mcss_prop_is_active( *effect_prop )
         ) || (
            /* No competition. */
            NULL == tag_prop &&
            !mcss_prop_is_active( *effect_prop )
         )
      ))
   ) {
      /* Inherit parent property. */
      if( MCSS_PROP_BACKGROUND_COLOR == p_id ) {
#if MCSSMERG_TRACE_LVL > 0
         debug_printf( MCSSMERG_TRACE_LVL, "background color was %s",
            0 <= *(RETROFLAT_COLOR*)effect_prop ?
            gc_retroflat_color_names[*(RETROFLAT_COLOR*)effect_prop] : "NULL" );
#endif /* MCSSMERG_TRACE_LVL */
      } else if( MCSS_PROP_COLOR == p_id ) {
#if MCSSMERG_TRACE_LVL > 0
         debug_printf( MCSSMERG_TRACE_LVL, "color was %s",
            0 <= *(RETROFLAT_COLOR*)effect_prop ?
            gc_retroflat_color_names[*(RETROFLAT_COLOR*)effect_prop] : "NULL" );
#endif /* MCSSMERG_TRACE_LVL */
      }
#if MCSSMERG_TRACE_LVL > 0
      debug_printf( MCSSMERG_TRACE_LVL,
         "%s using parent %s: " SSIZE_T_FMT,
         gc_mhtml_tag_names[tag_type], prop_name, *(ssize_t*)parent_prop );
#endif /* MCSSMERG_TRACE_LVL */
      /* XXX: Assign */
      memcpy( effect_prop, parent_prop, prop_sz );
      *effect_prop_flags = *parent_prop_flags;
      if( MCSS_PROP_BACKGROUND_COLOR == p_id ) {
#if MCSSMERG_TRACE_LVL > 0
         debug_printf( MCSSMERG_TRACE_LVL, "background color %s",
            0 <= *(RETROFLAT_COLOR*)effect_prop ?
            gc_retroflat_color_names[*(RETROFLAT_COLOR*)effect_prop] : "NULL" );
#endif /* MCSSMERG_TRACE_LVL */
      } else if( MCSS_PROP_COLOR == p_id ) {
#if MCSSMERG_TRACE_LVL > 0
         debug_printf( MCSSMERG_TRACE_LVL, "color %s",
            0 <= *(RETROFLAT_COLOR*)effect_prop ?
            gc_retroflat_color_names[*(RETROFLAT_COLOR*)effect_prop] : "NULL" );
#endif /* MCSSMERG_TRACE_LVL */
      }
   } else if(
      NULL != tag_prop &&
      mcss_prop_is_active( *tag_prop )
   ) {
      /* Use new property. */
#if MCSSMERG_TRACE_LVL > 0
      debug_printf( MCSSMERG_TRACE_LVL, "%s using style %s: " SSIZE_T_FMT,
         gc_mhtml_tag_names[tag_type], prop_name,
         *(ssize_t*)tag_prop );
      if( MCSS_PROP_COLOR == p_id ) {
         debug_printf( MCSSMERG_TRACE_LVL, "color %s",
            0 <= *(RETROFLAT_COLOR*)effect_prop ?
            gc_retroflat_color_names[*(RETROFLAT_COLOR*)effect_prop] : "NULL" );
      }
#endif /* MCSSMERG_TRACE_LVL */
      memcpy( effect_prop, tag_prop, prop_sz );
      *effect_prop_flags = *tag_prop_flags;
   }
}

/* === */

void mcssmerge_styles(
   struct MCSS_STYLE* effect_style,
   struct MCSS_STYLE* parent_style,
   struct MCSS_STYLE* tag_style,
   size_t tag_type
) {
   /* At least one of these styles probably comes from the locked vector of the
    * CSS styles parser, so... keep that in mind?
    */

   if(
      MCSS_STYLE_FLAG_ACTIVE !=
      (MCSS_STYLE_FLAG_ACTIVE & effect_style->flags)
   ) {
      mcss_style_init( effect_style );
   }

   /* Perform inheritence of special cases. */

   #define MCSS_PROP_TABLE_MERGE( p_id, prop_n, prop_t, prop_p, def ) \
      mcssmerge_merge_prop( p_id, \
         #prop_n, \
         sizeof( prop_t ), \
         tag_type, \
         NULL != parent_style ? &(parent_style->prop_n) : NULL, \
         NULL != parent_style ? &(parent_style->prop_n ## _flags) : 0, \
         NULL != effect_style ? &(effect_style->prop_n) : NULL, \
         NULL != effect_style ? &(effect_style->prop_n ## _flags) : 0, \
         NULL != tag_style ? &(tag_style->prop_n) : NULL, \
         NULL != tag_style ? &(tag_style->prop_n ## _flags) : 0 );

   MCSS_PROP_TABLE( MCSS_PROP_TABLE_MERGE )

   /* Apply defaults for display. */

   if(
      MCSS_PROP_FLAG_ACTIVE !=
      (MCSS_PROP_FLAG_ACTIVE & effect_style->DISPLAY_flags)
   ) {
      /* Set the display property based on the tag's default. */

      #define MHTML_TAG_TABLE_DISP( tag_id, tag_name, fields, disp ) \
         } else if( tag_id == tag_type ) { \
            effect_style->DISPLAY = MCSS_DISPLAY_ ## disp; \
            debug_printf( MCSSMERG_TRACE_LVL, "%s defaulting to %s DISPLAY", \
               gc_mhtml_tag_names[tag_type], \
               gc_mcss_display_names[effect_style->DISPLAY] );

      if( 0 ) {
      MHTML_TAG_TABLE( MHTML_TAG_TABLE_DISP )
      }

   }

   return;
}

#endif /* MCSSMERG_C */

#endif /* !MCSSMERG_H */

