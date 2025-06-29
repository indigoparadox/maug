
#if !defined( RETPLTI_H_DEFS )
#define RETPLTI_H_DEFS

struct RETROFLAT_INPUT_STATE {
   uint8_t flags;
   int16_t              last_key;
   uint8_t              vk_mods;
   unsigned int         last_mouse;
   unsigned int         last_mouse_x;
   unsigned int         last_mouse_y;
};

typedef int16_t RETROFLAT_IN_KEY;

#  ifndef VK_OEM_1
#     define VK_OEM_1 0xba
#  endif /* !VK_OEM_1 */

#  ifndef VK_OEM_2
#     define VK_OEM_2 0xbf
#  endif /* !VK_OEM_2 */

#  ifndef VK_OEM_3
#     define VK_OEM_3 0xc0
#  endif /* !VK_OEM_3 */

#  ifndef VK_OEM_4
#     define VK_OEM_4 0xdb
#  endif /* !VK_OEM_4 */

#  ifndef VK_OEM_5
#     define VK_OEM_5 0xdc
#  endif /* !VK_OEM_5 */

#  ifndef VK_OEM_6
#     define VK_OEM_6 0xdd
#  endif /* !VK_OEM_6 */

#  ifndef VK_OEM_7
#     define VK_OEM_7 0xde
#  endif /* !VK_OEM_7 */

#  ifndef VK_OEM_MINUS
#     define VK_OEM_MINUS 0xbd
#  endif /* !VK_OEM_MINUS */

#  ifndef VK_OEM_PLUS
#     define VK_OEM_PLUS 0xbb
#  endif /* !VK_OEM_PLUS */

#  ifndef VK_OEM_PERIOD
#     define VK_OEM_PERIOD 0xbe
#  endif /* !VK_OEM_PERIOD */

#  ifndef VK_OEM_COMMA
#     define VK_OEM_COMMA 0xbc
#  endif /* !VK_OEM_COMMA */

#  define RETROFLAT_KEY_GRAVE VK_OEM_3
#  define RETROFLAT_KEY_SLASH VK_OEM_2
#  define RETROFLAT_KEY_UP	   VK_UP
#  define RETROFLAT_KEY_DOWN	VK_DOWN
#  define RETROFLAT_KEY_RIGHT	VK_RIGHT
#  define RETROFLAT_KEY_LEFT	VK_LEFT
#  define RETROFLAT_KEY_BKSP  VK_BACK
#  define RETROFLAT_KEY_A	   0x41
#  define RETROFLAT_KEY_B	   0x42
#  define RETROFLAT_KEY_C	   0x43
#  define RETROFLAT_KEY_D	   0x44
#  define RETROFLAT_KEY_E	   0x45
#  define RETROFLAT_KEY_F	   0x46
#  define RETROFLAT_KEY_G	   0x47
#  define RETROFLAT_KEY_H	   0x48
#  define RETROFLAT_KEY_I	   0x49
#  define RETROFLAT_KEY_J	   0x4a
#  define RETROFLAT_KEY_K	   0x4b
#  define RETROFLAT_KEY_L	   0x4c
#  define RETROFLAT_KEY_M	   0x4d
#  define RETROFLAT_KEY_N	   0x4e
#  define RETROFLAT_KEY_O	   0x4f
#  define RETROFLAT_KEY_P	   0x50
#  define RETROFLAT_KEY_Q	   0x51
#  define RETROFLAT_KEY_R	   0x52
#  define RETROFLAT_KEY_S	   0x53
#  define RETROFLAT_KEY_T	   0x54
#  define RETROFLAT_KEY_U	   0x55
#  define RETROFLAT_KEY_V	   0x56
#  define RETROFLAT_KEY_W	   0x57
#  define RETROFLAT_KEY_X	   0x58
#  define RETROFLAT_KEY_Y	   0x59
#  define RETROFLAT_KEY_Z	   0x60
#  define RETROFLAT_KEY_0     0x30
#  define RETROFLAT_KEY_1     0x31
#  define RETROFLAT_KEY_2     0x32
#  define RETROFLAT_KEY_3     0x33
#  define RETROFLAT_KEY_4     0x34
#  define RETROFLAT_KEY_5     0x35
#  define RETROFLAT_KEY_6     0x36
#  define RETROFLAT_KEY_7     0x37
#  define RETROFLAT_KEY_8     0x38
#  define RETROFLAT_KEY_9     0x39
#  define RETROFLAT_KEY_TAB	VK_TAB
#  define RETROFLAT_KEY_SPACE	VK_SPACE
#  define RETROFLAT_KEY_ESC	VK_ESCAPE
#  define RETROFLAT_KEY_ENTER	VK_RETURN
#  define RETROFLAT_KEY_HOME	VK_HOME
#  define RETROFLAT_KEY_END	VK_END
#  define RETROFLAT_KEY_DELETE   VK_DELETE
#  define RETROFLAT_KEY_PGUP     VK_PRIOR
#  define RETROFLAT_KEY_PGDN     VK_NEXT
#  define RETROFLAT_KEY_SEMICOLON   VK_OEM_1
#  define RETROFLAT_KEY_PERIOD      VK_OEM_PERIOD
#  define RETROFLAT_KEY_COMMA       VK_OEM_COMMA
#  define RETROFLAT_KEY_EQUALS      VK_OEM_PLUS
#  define RETROFLAT_KEY_DASH        VK_OEM_MINUS
#  define RETROFLAT_KEY_BACKSLASH   VK_OEM_5
#  define RETROFLAT_KEY_QUOTE      VK_OEM_7
#  define RETROFLAT_KEY_BRACKETL    VK_OEM_4
#  define RETROFLAT_KEY_BRACKETR    VK_OEM_6
#  define RETROFLAT_KEY_INSERT    VK_INSERT

#  define RETROFLAT_MOUSE_B_LEFT    VK_LBUTTON
#  define RETROFLAT_MOUSE_B_RIGHT   VK_RBUTTON


#elif defined( RETROFLT_C )

RETROFLAT_IN_KEY retroflat_poll_input( struct RETROFLAT_INPUT* input ) {
   RETROFLAT_IN_KEY key_out = 0;

   assert( NULL != input );

   input->key_flags = 0;

   if( g_retroflat_state->input.last_key ) {
      /* Return g_retroflat_state->last_key, which is set in WndProc when a
       * keypress msg is received.
       */
      key_out = g_retroflat_state->input.last_key;
      input->key_flags = g_retroflat_state->input.vk_mods;

      debug_printf( RETROFLAT_KB_TRACE_LVL, "raw key: 0x%04x", key_out );

      /* Reset pressed key. */
      g_retroflat_state->input.last_key = 0;

   } else if( g_retroflat_state->input.last_mouse ) {
      if(
         MK_LBUTTON == (MK_LBUTTON & g_retroflat_state->input.last_mouse)
      ) {
         input->mouse_x = g_retroflat_state->input.last_mouse_x;
         input->mouse_y = g_retroflat_state->input.last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_LEFT;
      } else if(
         MK_RBUTTON == (MK_RBUTTON & g_retroflat_state->input.last_mouse)
      ) {
         input->mouse_x = g_retroflat_state->input.last_mouse_x;
         input->mouse_y = g_retroflat_state->input.last_mouse_y;
         key_out = RETROFLAT_MOUSE_B_RIGHT;
      }
      g_retroflat_state->input.last_mouse = 0;
      g_retroflat_state->input.last_mouse_x = 0;
      g_retroflat_state->input.last_mouse_y = 0;
   }

#     ifdef RETROFLAT_SCREENSAVER
   if( 
      (RETROFLAT_FLAGS_SCREENSAVER ==
      (RETROFLAT_FLAGS_SCREENSAVER & g_retroflat_state->retroflat_flags))
      && 0 != key_out
   ) {
      /* retroflat_quit( 0 ); */
   }
#     endif /* RETROFLAT_SCREENSAVER */

   return key_out;
}

#endif /* !RETPLTI_H_DEFS || RETROFLT_C */

