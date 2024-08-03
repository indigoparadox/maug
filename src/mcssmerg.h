
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
      if( \
         ( \
            /* Only do inheritence for some special cases.
             * e.g. We don't want to inherit width/height/X/etc! */ \
            mcss_prop_is_heritable( p_id ) \
         ) && (NULL != parent_style && ( \
            ( \
               NULL != tag_style && \
               /* Parent is important and new property isn't. */ \
               mcss_prop_is_active_flag( parent_style->prop_n, IMPORTANT ) && \
               /* TODO: Is not active OR important? */ \
               !mcss_prop_is_important( tag_style->prop_n ) && \
               !mcss_prop_is_important( effect_style->prop_n ) \
            ) || ( \
               NULL != tag_style && \
               /* New property is not active. */ \
               !mcss_prop_is_active( tag_style->prop_n ) && \
               !mcss_prop_is_active( effect_style->prop_n ) \
            ) || (\
               /* No competition. */ \
               NULL == tag_style && \
               !mcss_prop_is_active( effect_style->prop_n ) \
            ) \
         )) \
      ) { \
         /* Inherit parent property. */ \
         if( MCSS_PROP_BACKGROUND_COLOR == p_id ) { \
            debug_printf( MCSSMERG_TRACE_LVL, "background color was %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } else if( MCSS_PROP_COLOR == p_id ) { \
            debug_printf( MCSSMERG_TRACE_LVL, "color was %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } \
         debug_printf( MCSSMERG_TRACE_LVL, \
            "%s using parent %s: " SSIZE_T_FMT, \
            gc_mhtml_tag_names[tag_type], #prop_n, \
            (ssize_t)(parent_style->prop_n) ); \
         effect_style->prop_n = parent_style->prop_n; \
         effect_style->prop_n ## _flags = parent_style->prop_n ## _flags; \
         if( MCSS_PROP_BACKGROUND_COLOR == p_id ) { \
            debug_printf( MCSSMERG_TRACE_LVL, "background color %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } else if( MCSS_PROP_COLOR == p_id ) { \
            debug_printf( MCSSMERG_TRACE_LVL, "color %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } \
      } else if( \
         NULL != tag_style && \
         mcss_prop_is_active( tag_style->prop_n ) \
      ) { \
         /* Use new property. */ \
         debug_printf( MCSSMERG_TRACE_LVL, "%s using style %s: " SSIZE_T_FMT, \
            gc_mhtml_tag_names[tag_type], #prop_n, \
            (ssize_t)(tag_style->prop_n) ); \
         if( MCSS_PROP_COLOR == p_id ) { \
            debug_printf( MCSSMERG_TRACE_LVL, "color %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } \
         effect_style->prop_n = tag_style->prop_n; \
         effect_style->prop_n ## _flags = tag_style->prop_n ## _flags; \
      }

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

