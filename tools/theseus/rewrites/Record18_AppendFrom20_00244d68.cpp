// Appends the 0x18-byte record at a0+0x20 to the array at a0+0x238, indexed by the count at
// a0+0x3BC (no bound check), then increments the count. Returns the new count. 5 call sites.
#include "theseus.h"

void Record18_AppendFrom20_00244d68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.arg(0), count = g.read32(obj + 0x3BC);
    const uint32_t dst = obj + 0x238 + count * 0x18;
    const uint64_t w0 = g.read64(obj + 0x20), w1 = g.read64(obj + 0x28), w2 = g.read64(obj + 0x30);
    g.write64(dst, w0);
    g.write64(dst + 8, w1);
    g.write64(dst + 0x10, w2);
    g.write32(obj + 0x3BC, count + 1);
    g.ret(count + 1);
}
