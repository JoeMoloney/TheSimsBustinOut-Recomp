// Address of element a1 (44-byte elements) of the array from +0x4C (begin) to +0x50 (end) of a0,
// or 0 if a1 is negative or not below the count. The count is (end - begin) / 44, computed as
// a multiply by the inverse 0xBA2E8BA3 (exact for multiples of 44). 9 call sites.
#include "theseus.h"

void Array44_At_00370968(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const uint32_t begin = g.read32(a + 0x4C);
    if (g.sarg64(1) < 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t end = g.read32(a + 0x50);
    const int32_t count = static_cast<int32_t>((end - begin) * 0xBA2E8BA3u) >> 2;
    if (g.arg64(1) < static_cast<uint64_t>(static_cast<int64_t>(count)))
        g.ret(g.arg(1) * 44 + begin);
    else
        g.ret(0);
}
