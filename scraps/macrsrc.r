
#include "Types.r"

resource 'BNDL' (128, "MyApp Bundle") {
    'zzz', 0,
    {
        'ICN#', { 128 },
        'FREF', { 128 }
    }
};

resource 'FREF' (128, "MyApp FREF") {
    'APPL', 0, ""
};

data 'ICN#' (128, "MyApp Icon") {
   include "xxx"
};

