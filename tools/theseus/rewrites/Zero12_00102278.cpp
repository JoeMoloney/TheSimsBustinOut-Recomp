// Clears the three words at a0 (+4, +0, +8, in the original's order). 41 call sites (14 identical copies).
// theseus: copies 0x17bb58, 0x1dfe50, 0x25a7c8, 0x26eea0, 0x26efd0, 0x26f0d0, 0x2735c0, 0x27df60, 0x27e088, 0x27e4b8, 0x27e560, 0x285388, 0x2c3988
#include "theseus.h"

void Zero12_00102278(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 4, 0);
    g.write32(g.arg(0), 0);
    g.write32(g.arg(0) + 8, 0);
    g.ret_void();
}
