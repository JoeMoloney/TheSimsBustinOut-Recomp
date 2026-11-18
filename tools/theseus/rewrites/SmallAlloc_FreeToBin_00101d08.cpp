// Small-block allocator: return a block to its size-class free list (201 callers).
// Replaces FUN_00101d08 (block = a0, size = a1). Bins hold blocks of 16-byte size classes:
// bin (size - 1) / 16 of the table at 0x3A45C0. The block's first word links to the old head.
// Returns the previous head of the bin.
#include "theseus.h"

namespace
{
    constexpr uint32_t kBins = 0x003A45C0;
}

void SmallAlloc_FreeToBin_00101d08(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t block = g.arg(0);
    const uint32_t bin = kBins + ((g.arg(1) - 1) >> 4) * 4;
    const uint32_t head = g.read32(bin);
    g.write32(block, head);
    g.write32(bin, block);
    g.ret(head);
}
