// Clears the current selection a0+0x58 (index into the 16-byte record vector a0+0 / a0+4): unless it is already -1, a valid index (< count) gets its record'\''s +8 zeroed, then +0x5C and +0x58 become -1. A negative index (32-bit test, as the generated code does) just sets both to -1. Returns -1. 1 call site.
#include "theseus.h"

void Selection58_Clear_00351590(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0), index = g.read32(p + 0x58);
    if (index == 0xFFFFFFFFu)
    {
        g.ret(0xFFFFFFFFu);
        return;
    }
    if (static_cast<int32_t>(index) >= 0)
    {
        const uint32_t begin = g.read32(p);
        const uint32_t count = static_cast<uint32_t>(static_cast<int32_t>(g.read32(p + 4) - begin) >> 4);
        if (index < count)
            g.write32(begin + (index << 4) + 8, 0);
    }
    g.write32(p + 0x5C, 0xFFFFFFFFu);
    g.write32(p + 0x58, 0xFFFFFFFFu);
    g.ret(0xFFFFFFFFu);
}
