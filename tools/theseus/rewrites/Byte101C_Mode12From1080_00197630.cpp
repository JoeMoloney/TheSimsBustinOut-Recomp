// Sets the byte at a0+0x101C to 2 when the word at a0+0x1080 is nonzero, else 1; returns it. 2 call sites.
#include "theseus.h"

void Byte101C_Mode12From1080_00197630(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint8_t mode = g.read32(g.arg(0) + 0x1080) != 0 ? 2 : 1;
    g.write8(g.arg(0) + 0x101C, mode);
    g.ret(mode);
}
