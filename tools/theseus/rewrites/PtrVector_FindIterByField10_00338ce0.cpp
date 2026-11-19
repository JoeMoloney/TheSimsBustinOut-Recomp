// Iterator to the first object pointer in the vector from +0x24 (begin) to +0x28 (end) of a0
// whose object has word +0x10 equal to a1 (all 64 bits); end if none (4 call sites).
#include "theseus.h"

void PtrVector_FindIterByField10_00338ce0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t end = g.read32(g.arg(0) + 0x28);
    uint32_t it = g.read32(g.arg(0) + 0x24);
    while (it != end && static_cast<int64_t>(static_cast<int32_t>(g.read32(g.read32(it) + 0x10))) != g.sarg64(1))
        it += 4;
    g.ret(it);
}
