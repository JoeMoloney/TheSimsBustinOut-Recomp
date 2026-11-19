// Small-block allocation of a0 bytes: pops the head of the free list for its 16-byte size
// class (table 0x3A45C0, the counterpart of SmallAlloc_FreeToBin); if the list is empty, calls
// 0x102138 (a0 = size, a1 = the list head's address) and returns its result. 123 call sites.
#include "theseus.h"

void SmallAlloc_Alloc_001021e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t bin = (((g.arg(0) - 1) >> 4) << 2) + 0x3A45C0;
    const uint32_t head = g.read32(bin);
    if (head != 0)
    {
        g.write32(bin, g.read32(head));  // next block becomes the head
        g.ret(head);
        return;
    }
    g.set_arg(1, bin);
    g.call(0x102138, 0x102220);
    g.ret_void();
}
