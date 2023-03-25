
#ifndef MCSS_H
#define MCSS_H

#define MCSS_PROP_TABLE( f ) \
   f( 0, WIDTH, size_t ) \
   f( 1, HEIGHT, size_t ) \
   f( 2, COLOR, RETROFLAT_COLOR ) \
   f( 3, BACKGROUND_COLOR, RETROFLAT_COLOR )

#define MCSS_PROP_TABLE_PROPS( idx, prop_name, prop_type ) \
   prop_type prop_name;

struct MCSS_STYLE {
   MCSS_PROP_TABLE( MCSS_PROP_TABLE_PROPS )
};

struct MCSS_PARSER {
   uint16_t pstate[MPARSER_STACK_SZ_MAX];
   size_t pstate_sz;
   char token[MHTML_PARSER_TOKEN_SZ_MAX];
   size_t token_sz;
   size_t i;
   MAUG_MHANDLE styles_h;
   struct MCSS_STYLE* styles;
   size_t styles_sz;
   size_t styles_sz_max;
};

#endif /* !MCSS_H */

