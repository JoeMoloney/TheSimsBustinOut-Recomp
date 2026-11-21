// Sets the byte at a0+0x1018 to 2 when the word at a0+0x1078 is nonzero, else 1; returns it. 2 call sites.
#include "theseus.h"

void Byte1018_Mode12From1078_0019afc0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint8_t mode = g.read32(g.arg(0) + 0x1078) != 0 ? 2 : 1;
    g.write8(g.arg(0) + 0x1018, mode);
    g.ret(mode);
}
