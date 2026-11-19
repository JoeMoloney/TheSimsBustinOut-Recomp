// With a1 = 0: true if the flags at +0x8 of the object +0x4 of a0 points to are nonzero;
// otherwise true if they contain every bit of a1 (all 64 bits). 3 call sites.
#include "theseus.h"

void Flags08_HasAll_00257ba8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.read32(g.arg(0) + 4);
    const uint64_t flags = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(obj + 0x8))));
    if (g.arg64(1) == 0)
        g.ret(flags != 0 ? 1 : 0);
    else
        g.ret((flags & g.arg64(1)) == g.arg64(1) ? 1 : 0);
}
