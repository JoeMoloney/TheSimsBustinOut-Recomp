// Index of the first node in the list from [a0+0x19C] (node +0 = item, +8 = next) whose item has
// bit 3 of its word at +0x18 set, or -1. The index comes from a register first set with `daddu`
// (low 64 bits), so v0 keeps a0's upper half when one is found. 1 call site.
#include "theseus.h"

void List8_IndexOfFlag3_0019c7c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i a0_entry = GPR_VEC(ctx, 4);  // gate: equivalent 5: only v0's upper half (not part of the ABI compare)
    uint32_t index = 0;
    for (uint32_t node = g.read32(g.arg(0) + 0x19C); node != 0; node = g.read32(node + 8), ++index)
        if ((static_cast<int32_t>(g.read32(g.read32(node) + 0x18)) >> 3) & 1)  // gate: equivalent read16: only bit 3 is used
        {
            g.ret_with_upper(static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(index))), a0_entry);
            return;
        }
    g.ret(0xFFFFFFFF);
}
