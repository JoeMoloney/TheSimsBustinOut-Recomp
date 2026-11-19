// Address of the last element of the array of 168-byte elements from +0x488 (begin) to +0x48C
// (end) of a0, or 0 if it is empty. The count is (end - begin) / 168, computed as a multiply by
// the inverse 0x3CF3CF3D (exact for multiples of 168). 3 call sites.
#include "theseus.h"

void Array168_Back_00329450(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t a = g.arg(0);
    const uint32_t begin = g.read32(a + 0x488);
    const int32_t count = static_cast<int32_t>((g.read32(a + 0x48C) - begin) * 0x3CF3CF3Du) >> 3;
    if (count == 0)
        g.ret(0);
    else
        g.ret(static_cast<uint32_t>(count - 1) * 0xA8 + begin);
}
