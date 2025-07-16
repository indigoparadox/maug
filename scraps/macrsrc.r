
#include "Types.r"

#include "Retro68APPL.r"

type 'zzz' { pstring; };

resource 'zzz' (0) {
   "Maug Application zzz\n";
};

resource 'FREF' (128) { 'APPL', 0, "" };

resource 'BNDL' (128) {
   'zzz', 0, { 'ICN#', { 0, 128 }; 'FREF', { 0, 128 } }
};

