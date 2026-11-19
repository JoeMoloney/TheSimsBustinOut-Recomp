// Reads the rep pointer at +0 of this, calls 0x2BBB28(this), then Str_FreeUnlessStatic(this,
// old rep) (itself a rewrite that calls Heap_Free: three fibers deep in the game). Returns this
// (all 64 bits). 56 call sites.
#include "theseus.h"

void Str_Destroy_002bbbd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t self = g.arg64(0);
    const uint32_t rep = g.read32(g.arg(0));  // read before the first call (in its delay slot)
    g.call(0x2BBB28, 0x2BBBEC);
    g.set_arg64(0, self);
    g.set_arg64(1, static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(rep))));
    g.call(0x2BB9D8, 0x2BBBF8);
    g.ret64(self);
}
