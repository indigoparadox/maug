
#ifndef RETROGUI_H
#define RETROGUI_H

#ifndef RETROGUI_CTL_TEXT_SZ_MAX
#  define RETROGUI_CTL_TEXT_SZ_MAX 128
#endif /* !RETROGUI_CTL_TEXT_SZ_MAX */

#ifndef RETROGUI_CTL_SZ_MAX_INIT
#  define RETROGUI_CTL_SZ_MAX_INIT 10
#endif /* !RETROGUI_CTL_SZ_MAX_INIT */

#ifndef RETROGUI_PADDING
#  define RETROGUI_PADDING 5
#endif /* !RETROGUI_PADDING */

#ifndef RETROGUI_BTN_LBL_SZ_MAX
#  define RETROGUI_BTN_LBL_SZ_MAX 64
#endif /* !RETROGUI_BTN_LBL_SZ_MAX */

#define RETROGUI_CTL_TABLE( f ) \
   f( 0, NONE, void* none; ) \
   f( 1, LISTBOX, MAUG_MHANDLE list_h; char* list; size_t list_sz; size_t list_sz_max; size_t sel_idx; )

#if 0
   f( 2, BUTTON, char label[RETROGUI_BTN_LBL_SZ_MAX]; ) \
   f( 3, TEXTBOX, MAUG_MHANDLE text_h; char* text; size_t text_sz; ) \
   f( 4, SCROLLBAR, size_t min; size_t max; size_t value; )
#endif

#define retrogui_lock( gui ) \
   if( NULL == (gui)->ctls ) { \
      maug_mlock( (gui)->ctls_h, (gui)->ctls ); \
      maug_cleanup_if_null_alloc( union RETROGUI_CTL*, (gui)->ctls ); \
   }

#define retrogui_unlock( gui ) \
   if( NULL != (gui)->ctls ) { \
      maug_munlock( (gui)->ctls_h, (gui)->ctls ); \
   }

/*! \brief Unique identifying constant number for controls. */
typedef size_t RETROGUI_IDC;

#define RETROGUI_IDC_NONE -1

struct RETROGUI_CTL_BASE {
   uint8_t type;
   RETROGUI_IDC idc;
   size_t x;
   size_t y;
   size_t w;
   size_t h;
   RETROFLAT_COLOR bg_color;
   RETROFLAT_COLOR fg_color;
#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   HWND hwnd;
#endif
};

#define RETROGUI_CTL_TABLE_FIELDS( idx, c_name, c_fields ) \
   struct RETROGUI_CTL_ ## c_name { \
      struct RETROGUI_CTL_BASE base; \
      c_fields \
   };

RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_FIELDS )

#define RETROGUI_CTL_TABLE_TYPES( idx, c_name, c_fields ) \
   struct RETROGUI_CTL_ ## c_name c_name;

union RETROGUI_CTL {
   struct RETROGUI_CTL_BASE base;
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_TYPES )
};

struct RETROGUI {
   RETROGUI_IDC idc_prev;
   MAUG_MHANDLE ctls_h;
   union RETROGUI_CTL* ctls;
   size_t ctls_sz;
   size_t ctls_sz_max;
};

union RETROGUI_CTL* retrogui_get_ctl_by_idc( struct RETROGUI* gui, size_t idc );

/**
 * \brief Poll for the last clicked control and maintain listboxes
 *        and menus.
 * \param input Input integer returned from retroflat_poll_input().
 * \param input_evt ::RETROFLAT_INPUT initialized by retroflat_poll_input().
 * \return IDC of clicked control or ::RETROGUI_IDC_NONE if none clicked
 *         since last poll.
 */
RETROGUI_IDC retrogui_poll_ctls(
   struct RETROGUI* gui, int input, struct RETROFLAT_INPUT* input_evt );

void retrogui_redraw_ctls( struct RETROGUI* gui );

MERROR_RETVAL retrogui_push_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl );

size_t retrogui_get_ctl_sel_idx( struct RETROGUI* gui, size_t idc );

MERROR_RETVAL retrogui_init_ctl(
   union RETROGUI_CTL* ctl, uint8_t type, size_t idc );

MERROR_RETVAL retrogui_init( struct RETROGUI* gui );

#ifdef RETROGUI_C

#define RETROGUI_CTL_TABLE_CONSTS( idx, c_name, c_fields ) \
   MAUG_CONST uint8_t RETROGUI_CTL_TYPE_ ## c_name = idx;

RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_CONSTS )

#define RETROGUI_CTL_TABLE_NAMES( idx, c_name, f_fields ) \
   #c_name,

MAUG_CONST char* gc_retrogui_ctl_names[] = {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_NAMES )
   ""
};

/* === Control: NONE === */

void retrogui_redraw_NONE( struct RETROGUI* gui, union RETROGUI_CTL* ctl ) {
}

static MERROR_RETVAL retrogui_push_NONE( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_GUI;
  
   return retval;
}

MERROR_RETVAL retrogui_init_NONE( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_GUI;

   return retval;
}

/* === Control: LISTBOX === */

void retrogui_redraw_LISTBOX( struct RETROGUI* gui, union RETROGUI_CTL* ctl ) {
   size_t i = 0;
   
   assert( NULL == ctl->LISTBOX.list );

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   /* TODO: InvalidateRect()? */
#  else

   maug_mlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   if( NULL == ctl->LISTBOX.list ) {
      goto cleanup;
   }
   
   retroflat_rect( NULL, ctl->base.bg_color, ctl->base.x, ctl->base.y,
      ctl->base.w, ctl->base.h, RETROFLAT_FLAGS_FILL );

#if 0
   /* XXX: Parse out variable strings. */
   for( j = 0 ; gui->ctls[i].text_list_sz > j ; j++ ) {
      retroflat_string_sz(
         NULL, gui->ctls[i].text_list[j], 0, NULL, &w, &h, 0 );
      if( j == gui->ctls[i].sel_idx ) {
         /* TODO: Configurable selection colors. */
         retroflat_rect( NULL, RETROFLAT_COLOR_BLUE,
            gui->ctls[i].x, gui->ctls[i].y + (j * (h + RETROGUI_PADDING)),
            gui->ctls[i].w, h, RETROFLAT_FLAGS_FILL );
         
      }
      retroflat_string(
         NULL, gui->ctls[i].fg_color, gui->ctls[i].text_list[j], -1, NULL,
         gui->ctls[i].x, gui->ctls[i].y + (j * (h + RETROGUI_PADDING)), 0 );
   }
#endif

cleanup:

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

#  endif

}

MERROR_RETVAL retrogui_select_listbox_item(
   union RETROGUI_CTL* ctl, size_t item_idx
) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   /* Select sel_idx. */
   SendMessage( listbox->hwnd, LB_SETCURSEL, sel_idx, 0 );

#  else

   ctl->LISTBOX.sel_idx = item_idx;

#  endif

   return retval;
}

MERROR_RETVAL retrogui_push_listbox_item(
   struct RETROGUI* gui, size_t idc, const char* item, size_t item_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;
   MAUG_MHANDLE listbox_h_new = (MAUG_MHANDLE)NULL;

   retrogui_lock( gui );

   ctl = retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Adding item \"%s\" failed!", item );
      retval = MERROR_GUI;
      goto cleanup;
   }
   
#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   SendMessage( ctl->LISTBOX.base.hwnd, LB_ADDSTRING, 0, (LPARAM)item );

#  else

   if( 0 == ctl->LISTBOX.list_sz ) {
      ctl->LISTBOX.list_h = maug_malloc( 255, sizeof( char ) );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, ctl->LISTBOX.list_h );
      ctl->LISTBOX.list_sz_max = 255;
   }

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

   while( ctl->LISTBOX.list_sz + item_sz + 1 >= ctl->LISTBOX.list_sz_max )  {
      debug_printf( 1, "resizing listbox items to " SIZE_T_FMT "...",
         ctl->LISTBOX.list_sz );
      maug_mrealloc_test(
         listbox_h_new, ctl->LISTBOX.list_h,
         ctl->LISTBOX.list_sz_max * 2, sizeof( char ) );
      ctl->LISTBOX.list_sz_max *= 2;
   }

   maug_mlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   maug_cleanup_if_null_alloc( char*, ctl->LISTBOX.list );

   strncpy( &(ctl->LISTBOX.list[ctl->LISTBOX.list_sz]), item, item_sz );
   ctl->LISTBOX.list[ctl->LISTBOX.list_sz + item_sz] = '\0';
   ctl->LISTBOX.list_sz += item_sz + 1;

#endif

cleanup:

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

   return retval;
}

static MERROR_RETVAL retrogui_push_LISTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   ctl->base.hwnd = CreateWindow(
      "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
      ctl->base.x, ctl->base.y, ctl->base.w, ctl->base.h,
      g_retroflat_state->window, (HMENU)ctl->base.idc,
      g_retroflat_instance, NULL );
   if( (HWND)NULL == ctl->base.hwnd ) {
      error_printf( "could not create listbox" );
      retval = MERROR_GUI;
      goto cleanup;
   }

cleanup:

#  else

   /* TODO? */

#  endif

   return retval;
}

MERROR_RETVAL retrogui_init_LISTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1, "initializing listbox " SIZE_T_FMT "...", ctl->base.idc );

   ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   ctl->base.bg_color = RETROFLAT_COLOR_WHITE;

   return retval;
}

/* === Generic Functions === */

union RETROGUI_CTL* retrogui_get_ctl_by_idc(
   struct RETROGUI* gui, size_t idc
) {
   size_t i = 0;
   union RETROGUI_CTL* ctl = NULL;

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if( idc == gui->ctls[i].base.idc ) {
         ctl = &(gui->ctls[i]);
         break;
      }
   }

   if( NULL == ctl ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Could not find GUI item: " SIZE_T_FMT, idc );
   }

   return ctl;
}

static MERROR_RETVAL retrogui_push_BUTTON( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   ctl->base.hwnd = CreateWindow(
      "BUTTON", text, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
      ctl->base.x, ctl->base.y, ctl->base.w, ctl->base.h,
      g_retroflat_state->window, (HMENU)(ctl->base.idc),
      g_retroflat_instance, NULL );
   if( (HWND)NULL == ctl->base.hwnd ) {
      error_printf( "could not create button: %s", text );
      retval = MERROR_GUI;
      goto cleanup;
   }

cleanup:

#  endif

   return retval;
}

RETROGUI_IDC retrogui_poll_ctls( 
   struct RETROGUI* gui, int input, struct RETROFLAT_INPUT* input_evt
) {
   size_t i = 0;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if( gui->ctls[i].idc == gui->idc_prev ) {
         gui->idc_prev = 0;
         return gui->ctls[i].idc;
      }
   }
#  else
   /* Use our cross-platform controls. */
   size_t j = 0,
      w = 0,
      h = 0;

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if(
         RETROFLAT_MOUSE_B_LEFT == input &&
         input_evt->mouse_x > gui->ctls[i].base.x &&
         input_evt->mouse_y > gui->ctls[i].base.y &&
         input_evt->mouse_x < gui->ctls[i].base.x + gui->ctls[i].base.w &&
         input_evt->mouse_y < gui->ctls[i].base.y + gui->ctls[i].base.h
      ) {
      #if 0
         if( RETROGUI_CTL_TYPE_BUTTON == gui->ctls[i].base.type ) {
            /* Set the last button clicked. */
            if( gui->idc_prev == gui->ctls[i].base.idc ) {
               /* No repeated clicks! */
               return RETROGUI_IDC_NONE;
            } else {
               gui->idc_prev = gui->ctls[i].base.idc;
               return gui->ctls[i].base.idc;
            }
         } else XXX
         #endif 
         if( RETROGUI_CTL_TYPE_LISTBOX == gui->ctls[i].base.type ) {
            /* Figure out the item clicked. */
            for( j = 0 ; gui->ctls[i].LISTBOX.list_sz > j ; j++ ) {
            /*
               retroflat_string_sz(
                  NULL, gui->ctls[i].text_list[j], 0, NULL, &w, &h, 0 );
               if(
                  input_evt->mouse_y < 
                  gui->ctls[i].y + ((j + 1) * (h + RETROGUI_PADDING))
               ) {
                  gui->ctls[i].sel_idx = j;
                  return RETROGUI_IDC_NONE;
               }
            XXX
            */
            }
         }
      }
   }
   
   /* Reset repeat detector. */
   gui->idc_prev = RETROGUI_IDC_NONE;

#  endif

cleanup:

   return RETROGUI_IDC_NONE;
}

void retrogui_redraw_ctls( struct RETROGUI* gui ) {

   assert( NULL != gui->ctls );

   size_t i = 0,
      j = 0,
      w = 0,
      h = 0;

   #define RETROGUI_CTL_TABLE_PUSH( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == gui->ctls[i].base.type ) { \
         retrogui_redraw_ ## c_name( gui, &(gui->ctls[i]) ); \

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if( 0 ) {
      RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_PUSH )
      }
   }
}

MERROR_RETVAL retrogui_push_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != gui->ctls );

   /* TODO: Hunt for control IDC and fail if duplicate found! */

   /* TODO: Grow controls if needed. */
   assert( gui->ctls_sz + 1 < gui->ctls_sz_max );

   if( RETROFLAT_COLOR_NULL == ctl->base.bg_color ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Invalid background color specified for control " SIZE_T_FMT "!",
         ctl->base.idc );
      retval = MERROR_GUI;
      goto cleanup;

   }

   if( RETROFLAT_COLOR_NULL == ctl->base.fg_color ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Invalid foreground color specified for control " SIZE_T_FMT "!",
         ctl->base.idc );
      retval = MERROR_GUI;
      goto cleanup;
   }

   debug_printf( 1, "pushing ctl " SIZE_T_FMT " to slot " SIZE_T_FMT "...",
      ctl->base.idc, gui->ctls_sz );

   memcpy(
      &(gui->ctls[gui->ctls_sz]),
      ctl,
      sizeof( union RETROGUI_CTL ) );
   gui->ctls_sz++;

   #define RETROGUI_CTL_TABLE_PUSH( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         retval = retrogui_push_ ## c_name( ctl ); \
         maug_cleanup_if_not_ok();

   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_PUSH )
   }

cleanup:

   return retval;
}

MERROR_RETVAL retrogui_get_ctl_text(
   char* buffer, int buffer_sz, union RETROGUI_CTL* ctl
) {
   MERROR_RETVAL retval = MERROR_OK;
   
   /* TODO */

   return retval;
}

size_t retrogui_get_ctl_sel_idx( struct RETROGUI* gui, size_t idc ) {
   size_t idx = -1;
   union RETROGUI_CTL* ctl = NULL;

   ctl = retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      goto cleanup;
   }

   assert( RETROGUI_CTL_TYPE_LISTBOX == ctl->base.type );

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   idx = SendMessage( ctl->hwnd, LB_GETCARETINDEX, 0, 0 );
#  else
   idx = ctl->LISTBOX.sel_idx;
#  endif

cleanup:

   return idx;
}

MERROR_RETVAL retrogui_init_ctl(
   union RETROGUI_CTL* ctl, uint8_t type, size_t idc
) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1, "initializing control base " SIZE_T_FMT "...", idc );

   maug_mzero( ctl, sizeof( union RETROGUI_CTL ) );

   ctl->base.type = type;
   ctl->base.idc = idc;
   ctl->base.fg_color = RETROFLAT_COLOR_NULL;
   ctl->base.bg_color = RETROFLAT_COLOR_NULL;

   #define RETROGUI_CTL_TABLE_INITS( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         retrogui_init_ ## c_name( ctl ); \

   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_INITS )
   }

   return retval;
}

MERROR_RETVAL retrogui_init( struct RETROGUI* gui ) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( gui, sizeof( struct RETROGUI ) );

   gui->ctls_h = maug_malloc(
      RETROGUI_CTL_SZ_MAX_INIT, sizeof( struct RETROGUI ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, gui );
   gui->ctls_sz_max = RETROGUI_CTL_SZ_MAX_INIT;

cleanup:

   return retval;
}

#else

#define RETROGUI_CTL_TABLE_CONSTS( idx, c_name, c_fields ) \
   extern MAUG_CONST uint8_t RETROGUI_CTL_TYPE_ ## c_name;

RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_CONSTS )

extern MAUG_CONST char* gc_retrogui_ctl_names[];

#endif /* RETROGUI_C */

#endif /* !RETROGUI_H */

