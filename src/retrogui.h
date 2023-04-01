
#ifndef RETROGUI_H
#define RETROGUI_H

/**
 * \addtogroup maug_retrogui RetroGUI API
 * \{
 * \file retrogui.h
 */

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

/**
 * \addtogroup maug_retrogui_ctl RetroGUI Controls
 * \{
 */

/**
 * \brief Table defining all control types and their specific fields in
 *        ::RETROGUI_CTL.
 *
 * All entries in this table have corresponding retrogui_redraw_*,
 * retrogui_poll_*, retrogui_init_*, and retrogui_push_* functions which are
 * called internally from retrogui_redraw_ctls(), retrogui_poll_ctls(),
 * retrogui_init_ctl(), and retrogui_push_ctl(), respectively.
 */
#define RETROGUI_CTL_TABLE( f ) \
   f( 0, NONE, void* none; ) \
   f( 1, LISTBOX, MAUG_MHANDLE list_h; char* list; size_t list_sz; size_t list_sz_max; size_t sel_idx; ) \
   f( 2, BUTTON, char label[RETROGUI_BTN_LBL_SZ_MAX]; )

#if 0
   f( 3, TEXTBOX, MAUG_MHANDLE text_h; char* text; size_t text_sz; ) \
   f( 4, SCROLLBAR, size_t min; size_t max; size_t value; )
#endif

/*! \brief Fields common to ALL ::RETROGUI_CTL types. */
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

/**
 * \brief Creates the corresponding RETROGUI_* structs from
 *        ::RETROGUI_CTL_TABLE that populate union ::RETROGUI_CTL.
 */
#define RETROGUI_CTL_TABLE_FIELDS( idx, c_name, c_fields ) \
   struct RETROGUI_CTL_ ## c_name { \
      struct RETROGUI_CTL_BASE base; \
      c_fields \
   };

RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_FIELDS )

/**
 * \brief Adds the structs created by ::RETROGUI_CTL_TABLE_FIELDS to
 *        union ::RETROGUI_CTL.
 */
#define RETROGUI_CTL_TABLE_TYPES( idx, c_name, c_fields ) \
   struct RETROGUI_CTL_ ## c_name c_name;

union RETROGUI_CTL {
   struct RETROGUI_CTL_BASE base;
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_TYPES )
};

/*! \} */ /* maug_retrogui_ctl */

struct RETROGUI {
   RETROGUI_IDC idc_prev;
   MAUG_MHANDLE ctls_h;
   union RETROGUI_CTL* ctls;
   size_t ctls_sz;
   size_t ctls_sz_max;
};

MERROR_RETVAL retrogui_push_listbox_item(
   struct RETROGUI* gui, RETROGUI_IDC idc, const char* item, size_t item_sz );

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

static RETROGUI_IDC retrogui_poll_NONE( 
   union RETROGUI_CTL* ctl, int input, struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;

   return idc_out;
}

void retrogui_redraw_NONE( struct RETROGUI* gui, union RETROGUI_CTL* ctl ) {
}

static MERROR_RETVAL retrogui_push_NONE( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_GUI;
  
   return retval;
}

static void retrogui_free_NONE( union RETROGUI_CTL* ctl ) {
}

static MERROR_RETVAL retrogui_init_NONE( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_GUI;

   return retval;
}

/* === Control: LISTBOX === */

static RETROGUI_IDC retrogui_poll_LISTBOX( 
   union RETROGUI_CTL* ctl, int input, struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;
   size_t i = 0,
      j = 0,
      w = 0,
      h = 0;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   /* Do nothing. */
#  else

   assert( NULL == ctl->LISTBOX.list );
   maug_mlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );

   /* Figure out the item clicked. */
   while( i < ctl->LISTBOX.list_sz ) {
      retroflat_string_sz( NULL, &(ctl->LISTBOX.list[i]), 0, NULL, &w, &h, 0 );

      if(
         input_evt->mouse_y < 
         ctl->base.y + ((j + 1) * (h + RETROGUI_PADDING))
      ) {
         ctl->LISTBOX.sel_idx = j;
         break;
      }

      /* Try next variable-length string. */
      i += strlen( &(ctl->LISTBOX.list[i]) ) + 1;
      assert( i <= ctl->LISTBOX.list_sz );
      j++;
   }

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

#endif

   return idc_out;
}

static void retrogui_redraw_LISTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   size_t i = 0,
      j = 0,
      w = 0,
      h = 0;
   
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

   /* Parse out variable-length strings. */
   while( i < ctl->LISTBOX.list_sz ) {
      retroflat_string_sz( NULL, &(ctl->LISTBOX.list[i]), 0, NULL, &w, &h, 0 );
      if( j == ctl->LISTBOX.sel_idx ) {
         /* TODO: Configurable selection colors. */
         retroflat_rect( NULL, RETROFLAT_COLOR_BLUE,
            ctl->base.x, ctl->base.y + (j * (h + RETROGUI_PADDING)),
            ctl->base.w, h, RETROFLAT_FLAGS_FILL );
         
      }
      retroflat_string(
         NULL, ctl->base.fg_color, &(ctl->LISTBOX.list[i]), 0, NULL,
         ctl->base.x, ctl->base.y + (j * (h + RETROGUI_PADDING)), 0 );

      /* Move to next variable-length string. */
      i += strlen( &(ctl->LISTBOX.list[i]) ) + 1;
      assert( i <= ctl->LISTBOX.list_sz );
      j++;
   }

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
   SendMessage( ctl->base.hwnd, LB_SETCURSEL, item_idx, 0 );

#  else

   ctl->LISTBOX.sel_idx = item_idx;

#  endif

   return retval;
}

MERROR_RETVAL retrogui_push_listbox_item(
   struct RETROGUI* gui, RETROGUI_IDC idc, const char* item, size_t item_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;
   MAUG_MHANDLE listbox_h_new = (MAUG_MHANDLE)NULL;

   retrogui_lock( gui );

   debug_printf( 1, "pushing item \"%s\" to listbox " SIZE_T_FMT "...",
      item, idc );

   ctl = retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Adding item \"%s\" failed: Control missing!", item );
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
      g_retroflat_state->window, (HMENU)(ctl->base.idc),
      g_retroflat_instance, NULL );
   debug_printf( 1, "listbox hwnd: %p", ctl->LISTBOX.base.hwnd );
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

static void retrogui_free_LISTBOX( union RETROGUI_CTL* ctl ) {
   assert( NULL == ctl->LISTBOX.list );
   maug_mfree( ctl->LISTBOX.list_h );
}

static MERROR_RETVAL retrogui_init_LISTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1, "initializing listbox " SIZE_T_FMT "...", ctl->base.idc );

   ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   ctl->base.bg_color = RETROFLAT_COLOR_WHITE;

   return retval;
}

/* === Control: BUTTON === */

static RETROGUI_IDC retrogui_poll_BUTTON( 
   union RETROGUI_CTL* ctl, int input, struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;

   /* Set the last button clicked. */
   idc_out = ctl->base.idc;

   return idc_out;
}

static void retrogui_redraw_BUTTON(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   size_t w = 0,
      h = 0;

   retroflat_rect( NULL, ctl->base.bg_color, ctl->base.x, ctl->base.y,
      ctl->base.w, ctl->base.h, RETROFLAT_FLAGS_FILL );

   /* TODO: Draw outlines for outset/inset depending on idc_prev. */
      
   retroflat_string_sz( NULL, ctl->BUTTON.label, 0, NULL, &w, &h, 0 );

   retroflat_string(
      NULL, ctl->base.fg_color, ctl->BUTTON.label, 0, NULL,
      ctl->base.x + ((ctl->base.w / 2) - (w / 2)),
      ctl->base.y + ((ctl->base.h / 2) - (h / 2)), 0 );
}

static MERROR_RETVAL retrogui_push_BUTTON( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   ctl->base.hwnd = CreateWindow(
      "BUTTON", ctl->BUTTON.label, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
      ctl->base.x, ctl->base.y, ctl->base.w, ctl->base.h,
      g_retroflat_state->window, (HMENU)(ctl->base.idc),
      g_retroflat_instance, NULL );
   if( (HWND)NULL == ctl->base.hwnd ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Could not create button " SIZE_T_FMT ": %s",
         ctl->base.idc, ctl->BUTTON.label );
      retval = MERROR_GUI;
      goto cleanup;
   }

cleanup:

#  endif

   return retval;
}

static void retrogui_free_BUTTON( union RETROGUI_CTL* ctl ) {
}

static MERROR_RETVAL retrogui_init_BUTTON( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1, "initializing button " SIZE_T_FMT "...", ctl->base.idc );

   ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   ctl->base.bg_color = RETROFLAT_COLOR_GRAY;

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

RETROGUI_IDC retrogui_poll_ctls( 
   struct RETROGUI* gui, int input, struct RETROFLAT_INPUT* input_evt
) {
   size_t i = 0;
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;

   assert( NULL != gui->ctls );

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if( gui->ctls[i].base.idc == g_retroflat_state->last_idc ) {
         g_retroflat_state->last_idc = 0;
         return gui->ctls[i].base.idc;
      }
   }

#  else

   /* Use our cross-platform controls. */

   #define RETROGUI_CTL_TABLE_POLL( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == gui->ctls[i].base.type ) { \
         idc_out = \
            retrogui_poll_ ## c_name( &(gui->ctls[i]), input, input_evt ); \

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if(
         RETROFLAT_MOUSE_B_LEFT == input &&
         input_evt->mouse_x > gui->ctls[i].base.x &&
         input_evt->mouse_y > gui->ctls[i].base.y &&
         input_evt->mouse_x < gui->ctls[i].base.x + gui->ctls[i].base.w &&
         input_evt->mouse_y < gui->ctls[i].base.y + gui->ctls[i].base.h
      ) {
         if( gui->idc_prev == gui->ctls[i].base.idc ) {
            /* No repeated clicks! */
            /* TODO: Allow exceptions for e.g. scrollbars. */
            idc_out = RETROGUI_IDC_NONE;
            goto cleanup;
         }

         gui->idc_prev = gui->ctls[i].base.idc;

         if( 0 ) {
         RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_POLL )
         }
         break;
      }
   }

   /* Reset repeat detector. */
   gui->idc_prev = RETROGUI_IDC_NONE;

cleanup:

#  endif

   return idc_out;
}

void retrogui_redraw_ctls( struct RETROGUI* gui ) {
   size_t i = 0;

   assert( NULL != gui->ctls );

   #define RETROGUI_CTL_TABLE_REDRAW( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == gui->ctls[i].base.type ) { \
         retrogui_redraw_ ## c_name( gui, &(gui->ctls[i]) ); \

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if( 0 ) {
      RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_REDRAW )
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

   debug_printf( 1, "pushing %s " SIZE_T_FMT " to slot " SIZE_T_FMT "...",
      gc_retrogui_ctl_names[ctl->base.type], ctl->base.idc, gui->ctls_sz );

   memcpy(
      &(gui->ctls[gui->ctls_sz]),
      ctl,
      sizeof( union RETROGUI_CTL ) );
   gui->ctls_sz++;

   #define RETROGUI_CTL_TABLE_PUSH( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         retval = retrogui_push_ ## c_name( &(gui->ctls[gui->ctls_sz - 1]) ); \
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
   idx = SendMessage( ctl->base.hwnd, LB_GETCARETINDEX, 0, 0 );
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
         retrogui_init_ ## c_name( ctl );

   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_INITS )
   }

   return retval;
}

void retrogui_free( struct RETROGUI* gui ) {
   size_t i = 0;

   if( NULL == gui->ctls && (MAUG_MHANDLE)NULL != gui->ctls_h ) {
      maug_mlock( gui->ctls_h, gui->ctls );
      if( NULL == gui->ctls ) {
         goto cleanup;
      }
   }

   #define RETROGUI_CTL_TABLE_FREE( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == gui->ctls[i].base.type ) { \
         retrogui_free_ ## c_name( &(gui->ctls[i]) );

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if( 0 ) {
      RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_FREE )
      }
   }

   maug_munlock( gui->ctls_h, gui->ctls );

cleanup:

   maug_mfree( gui->ctls_h );

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

/*! \} */ /* maug_retrogui */

#endif /* !RETROGUI_H */

