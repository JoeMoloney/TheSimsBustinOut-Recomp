// 1 if a0 (all 64 bits) is one of 8 hash values, else 0 (the compiled chain of compares). 1 call site.
#include "theseus.h"

void HashSet8_Member_00228d68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    static constexpr uint32_t kHashes[] = {0x24C95F99, 0xC5F45AA3, 0x39E377CF, 0xFFECF6B5, 0x9087E0A2, 0x4740F763, 0xA4E8B034, 0x8B519FA6};
    const int64_t key = g.sarg64(0);
    for (const uint32_t h : kHashes)
        if (static_cast<int32_t>(h) == key)
        {
            g.ret(1);
            return;
        }
    g.ret(0);
}
