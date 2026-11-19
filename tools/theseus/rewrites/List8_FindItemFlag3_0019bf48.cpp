// Walks the list whose head pointer is at a0 (node: +0 item, +8 next) and returns the first
// item whose word +0x18 has bit 3 set; 0 if none (5 call sites).
#include "theseus.h"

void List8_FindItemFlag3_0019bf48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    for (uint32_t node = g.read32(g.arg(0)); node != 0; node = g.read32(node + 8))
    {
        const uint32_t item = g.read32(node);
        const uint32_t flags = g.read32(item + 0x18);  // gate: equivalent read16: only bit 3 is used
        if ((flags >> 3) & 1)
        {
            g.set_arg64(2, static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(item))));  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
            g.ret(item);
            return;
        }
    }
    g.set_arg64(2, 0);  // scratch register, not in the ABI compare (kept for the caller check); gate: equivalent
    g.ret(0);
}
