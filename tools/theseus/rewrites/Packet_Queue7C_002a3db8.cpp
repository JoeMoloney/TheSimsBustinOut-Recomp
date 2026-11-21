// Copies 8 bytes from a0+0x58 to a1+0x50; unless a0+0x98 is set, pushes a1 on the list at a0+0x7C (link a1+0x64) and adds 1 (or 2 when byte a1+0x5F is nonzero) to the count a0+0x80. v0 = the 8 bytes (64 bits) when it stops early, else old count + 2. 1 call site.
#include "theseus.h"

void Packet_Queue7C_002a3db8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t stamp = g.read64(g.arg(0) + 0x58);
    g.write64(g.arg(1) + 0x50, stamp);
    if (g.read32(g.arg(0) + 0x98) != 0)
    {
        g.ret64(stamp);
        return;
    }
    g.write32(g.arg(1) + 0x64, g.read32(g.arg(0) + 0x7C));
    g.write32(g.arg(0) + 0x7C, g.arg(1));
    const uint32_t n = g.read32(g.arg(0) + 0x80);
    g.write32(g.arg(0) + 0x80, n + 1);
    if (g.read8(g.arg(1) + 0x5F) != 0)
        g.write32(g.arg(0) + 0x80, n + 2);
    g.ret(n + 2);
}
