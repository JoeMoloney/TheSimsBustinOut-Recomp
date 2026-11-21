// Increments the global counter 0x3A8A88 and returns the new value. 17 call sites.
#include "theseus.h"

void Global3A8A88_Increment_002b17b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(0x003A8A88) + 1;
    g.write32(0x003A8A88, value);
    g.ret(value);
}
