// The string's characters as a C string (bstring2.cpp): the rep's data pointer, or the shared
// empty string at 0x3E6948 when the length is 0 or the data pointer is null. 63 call sites.
// First non-leaf rewrite: calls the length function (a copy of bstring_Length) like the
// original, with the original's return address.
#include "theseus.h"

void bstring2_CStr_002cac50(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t str = g.arg(0);
    g.call(0x2CB258, 0x2CAC64);  // length (a0 = str, unchanged)
    if (GPR_U64(g.ctx, 2) == 0)
    {
        g.ret(0x003E6948);
        return;
    }
    const uint32_t data = g.read32(g.read32(str));
    g.ret(data != 0 ? data : 0x003E6948);
}
