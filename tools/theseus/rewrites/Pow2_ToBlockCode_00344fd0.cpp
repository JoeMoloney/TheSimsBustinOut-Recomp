// m = 1 << a0. With a1 != 0, *a1 = 0 and for m = 16/32/64/128 a code 2/4/1/3 instead; returns m, except 16 for m = 32 and 32 for m = 64/128 (a size class). Without a1 it just returns m. 2 call sites.
#include "theseus.h"

void Pow2_ToBlockCode_00344fd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t m = 1u << (g.arg(0) & 31);
    if (g.arg64(1) == 0)
    {
        g.ret(m);
        return;
    }
    const uint32_t out = g.arg(1);
    g.write32(out, 0);
    switch (m)
    {
    case 16: g.write32(out, 2); g.ret(16); return;  // gate: equivalent write16: out was zeroed above, its upper half stays 0
    case 32: g.write32(out, 4); g.ret(16); return;  // gate: equivalent write16: out was zeroed above, its upper half stays 0
    case 64: g.write32(out, 1); g.ret(32); return;  // gate: equivalent write16: out was zeroed above, its upper half stays 0
    case 128: g.write32(out, 3); g.ret(32); return;  // gate: equivalent write16: out was zeroed above, its upper half stays 0
    default: g.ret(m); return;
    }
}
