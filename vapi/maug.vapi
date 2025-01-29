
[CCode (cheader_filename = "maug.h")]
namespace Maug {
   [CCode (cname = "struct RETROFLAT_ARGS")]
   public struct Args {
      public int ver;
   }
	[CCode (cprefix = "retroflat_")]
   [Compact]
	public class RetroFlat {
      [CCode (cname = "retroflat_init")]
      public static int init(
         int argc, string* argv, Args* args_struct );

      public static void shutdown( int retval );
   }

	[CCode (cprefix = "retroflat_", cname = "struct RETROFLAT_BITMAP", free_function = "retroflat_free_bitmap")]
   [Compact]
   public class Bitmap {
      [CCode (cname = "retroflat_new_bitmap")]
      public Bitmap( size_t w, size_t h, uint8 flags );


   //   public void string(
   //struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color,
   //const char* str, int str_sz, const char* font_str, int16_t x_orig, int16_t y_orig,
   }
}

