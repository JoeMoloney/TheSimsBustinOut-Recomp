// Increments the global counter 0x3F83C4 ($gp - 0x772C), stores the new value at a0+0x40 and returns it (a serial number). 2 call sites.
#include "theseus.h"

void Counter3F83C4_NextId_002d4c28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t id = g.read32(0x3F83C4) + 1;
    g.write32(0x3F83C4, id);
    g.write32(g.arg(0) + 0x40, id);
    g.ret(id);
}
