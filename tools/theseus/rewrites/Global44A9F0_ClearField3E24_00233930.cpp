// Unless the global 0x44A9D8 is nonzero, zeroes +0x3E24 of the object [0x44A9F0] (when there is one). v0 = 0x450000 in the first case, else the object pointer. 2 call sites. (The map joined other functions' epilogues after the `jr $ra`.)
#include "theseus.h"

void Global44A9F0_ClearField3E24_00233930(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.read32(0x44A9D8) != 0)
    {
        g.ret(0x450000);
        return;
    }
    const uint32_t obj = g.read32(0x44A9F0);
    if (obj != 0)
        g.write32(obj + 0x3E24, 0);
    g.ret(obj);
}
