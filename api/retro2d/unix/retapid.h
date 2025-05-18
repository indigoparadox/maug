
#  include <alsa/asoundlib.h>

struct RETROFLAT_SOUND {
   uint8_t flags;
   snd_seq_t* seq_handle;
   int seq_port;
   int out_client;
   int out_port;
};

