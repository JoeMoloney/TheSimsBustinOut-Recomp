// Reads a2 bytes from the stream a0 (+8 read pointer, +0x18 position, both advanced per byte) into a1. Returns the number of bytes read. (The count compare is on whole registers, as in the original.) 1 call site.
#include "theseus.h"

void Stream_ReadBytes_002787f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t s = g.arg(0);
    if (g.arg64(2) == 0)
    {
        g.ret(0);
        return;
    }
    uint32_t dst = g.arg(1), n = 0;
    do
    {
        const uint32_t p = g.read32(s + 8);
        ++n;
        g.write8(dst, g.read8(p));  // gate: equivalent read16: write8 keeps only the low byte
        g.write32(s + 8, p + 1);
        ++dst;
        g.write32(s + 0x18, g.read32(s + 0x18) + 1);
    } while (static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(n))) < g.arg64(2));
    g.ret(n);
}
