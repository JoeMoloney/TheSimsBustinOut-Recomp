// If the global 0x3F9228 points to an object, copies its +0 to its +4; then zeroes the global. Returns the copied word (v0 untouched without an object). 1 call site.
#include "theseus.h"

void Global3F9228_Detach_0021ca20(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.read32(0x3F9228);
    if (obj == 0)
    {
        g.write32(0x3F9228, 0);  // gate: equivalent write16,drop: the global is already 0 here
        g.ret_void();  // v0 untouched
        return;
    }
    const uint32_t v0 = g.read32(obj);
    g.write32(obj + 4, v0);
    g.write32(0x3F9228, 0);
    g.ret(v0);
}
