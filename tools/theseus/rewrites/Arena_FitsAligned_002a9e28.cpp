// Whether a1 bytes fit at the next a2-aligned position: 0 if the capacity [a0+8] < a1 (a1 on all
// 64 bits); else 1 if (align_up(top, a2) - top) + a1 <= capacity (top = [a0+4]; unsigned 32-bit;
// a2 must be a power of two). 1 call site.
#include "theseus.h"

void Arena_FitsAligned_002a9e28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t capacity = g.read32(g.arg(0) + 8);
    if (static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(capacity))) < g.arg64(1))
    {
        g.ret(0);
        return;
    }
    const uint32_t top = g.read32(g.arg(0) + 4), mask = g.arg(2) - 1;
    const uint32_t needed = ((top + mask) & ~mask) - top + g.arg(1);
    g.ret(capacity < needed ? 0 : 1);
}
