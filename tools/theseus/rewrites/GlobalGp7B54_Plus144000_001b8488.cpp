// The global at $gp - 0x7B54 (0x3F7F9C) plus 0x144000 (4 call sites).
#include "theseus.h"

void GlobalGp7B54_Plus144000_001b8488(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.gp() - 0x7B54) + 0x144000);
}
