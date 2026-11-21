// For the object [a0+4]: unless its latch (+0x84) is already set, checks whether every bit of a1 is set in its flags (+0x18) and if so sets the latch to 1. Returns 1 when it latched now, else 0. 3 call sites. (The map joined another function's epilogue after the `jr $ra`.)
#include "theseus.h"

void Flags18_LatchAll_00257c58(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.read32(g.arg(0) + 4);
    if (g.read32(obj + 0x84) != 0)
    {
        g.ret(0);
        return;
    }
    const uint64_t flags = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(obj + 0x18))));
    const uint64_t want = g.arg64(1);  // `and`/`xor` on whole registers
    if (((flags & want) ^ want) == 0)
    {
        g.write32(obj + 0x84, 1);  // gate: equivalent write16: the latch was 0 on this path
        g.ret(1);
        return;
    }
    g.ret(0);
}
