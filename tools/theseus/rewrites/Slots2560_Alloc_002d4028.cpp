// Allocates a slot in the table at a0: 80 bitmap words at +0x2800 (used) and +0x2940 (reserved), searched from word 64 when a2 != 0 (whole register), else from word 0. The first word whose combined bits are not all set gets its lowest clear bit set in the used map, and entry (word * 32 + bit) of the pointer table at a0 = a1. Returns the slot number, -1 when full, -2 when the system global 0x3F83C0 is 0. 2 call sites.
#include "theseus.h"

void Slots2560_Alloc_002d4028(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(0x3F83C0) == 0)
    {
        g.ret(0xFFFFFFFEu);
        return;
    }
    const uint32_t table = g.arg(0);
    for (uint32_t word = g.arg64(2) != 0 ? 64 : 0; word < 80; ++word)
    {
        const uint32_t used = table + 0x2800 + (word << 2);
        const uint32_t bits = g.read32(used + 0x140) | g.read32(used);
        if (bits == 0xFFFFFFFFu)
            continue;
        uint32_t n = 0, bit = 1;
        while (bits & bit)
        {
            bit <<= 1;
            ++n;
        }
        g.write32(used, g.read32(used) | bit);
        const uint32_t slot = (word << 5) + n;
        g.write32(table + (slot << 2), g.arg(1));
        g.ret(slot);
        return;
    }
    g.ret(0xFFFFFFFFu);
}
