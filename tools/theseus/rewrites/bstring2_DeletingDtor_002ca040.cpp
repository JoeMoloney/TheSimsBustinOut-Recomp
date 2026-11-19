// bstring2 deleting destructor: the destructor body 0x2C9028(this), then operator delete(this)
// if bit 0 of a1 is set. v0 is the last callee's. 82 call sites.
#include "theseus.h"

void bstring2_DeletingDtor_002ca040(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t self = g.arg64(0), flags = g.arg64(1);
    g.call(0x2C9028, 0x2CA05C);
    if (flags & 1)
    {
        g.set_arg64(0, self);
        g.call(0x102610, 0x2CA070);
    }
    g.ret_void();
}
