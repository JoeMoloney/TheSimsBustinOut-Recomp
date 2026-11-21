// Searches a vector of 0x18-byte groups ([a0] begin, [a0+4] end); each group holds a vector of
// 0x58-byte items ([g] begin, [g+4] end). The first item whose key (+0xC) equals a1: if a3 != 0
// it is stored in the group's +0x14; *a2 = the group's +0x10 and the item is returned. Not
// found: *a2 = 0, returns 0. 9 call sites.
#include "theseus.h"

void Groups18_FindItem58_0036a268(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int64_t key = g.sarg64(1);  // `bne` compares all 64 bits with the sign-extended field
    for (uint32_t group = g.read32(g.arg(0)); group != g.read32(g.arg(0) + 4); group += 0x18)
    {
        const uint32_t end = g.read32(group + 4);
        for (uint32_t item = g.read32(group); item != end; item += 0x58)
        {
            if (static_cast<int32_t>(g.read32(item + 0xC)) == key)
            {
                if (g.arg64(3) != 0)
                    g.write32(group + 0x14, g.arg(3));
                g.write32(g.arg(2), g.read32(group + 0x10));
                g.ret(item);
                return;
            }
        }
    }
    g.write32(g.arg(2), 0);
    g.ret(0);
}
