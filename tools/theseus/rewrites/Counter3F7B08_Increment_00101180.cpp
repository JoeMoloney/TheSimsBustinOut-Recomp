// Increments the global counter 0x3F7B08 ($gp - 0x7FE8) and returns the new value. 1 call site.
#include "theseus.h"

void Counter3F7B08_Increment_00101180(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read32(0x3F7B08) + 1;
    g.write32(0x3F7B08, value);
    g.ret(value);
}
