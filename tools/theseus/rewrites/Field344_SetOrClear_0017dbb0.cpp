// Sets (a2 != 0) or clears (a2 == 0) the bits a1 in the word at a0+0x344; returns the new word. 3 call sites.
#include "theseus.h"

void Field344_SetOrClear_0017dbb0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    // or/and on whole registers: v0 keeps a1's upper bits (a caller may read v0 as 64 bits)
    const uint32_t field = g.arg(0) + 0x344;
    const uint64_t old = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(field))));
    const uint64_t value = g.arg64(2) != 0 ? old | g.arg64(1) : old & ~g.arg64(1);
    g.write32(field, static_cast<uint32_t>(value));
    g.ret64(value);
}
