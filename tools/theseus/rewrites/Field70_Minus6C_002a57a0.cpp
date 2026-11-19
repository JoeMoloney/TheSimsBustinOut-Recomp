// Word +0x70 minus word +0x6C of a0 (4 call sites).
#include "theseus.h"

void Field70_Minus6C_002a57a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x70) - g.read32(g.arg(0) + 0x6C));
}
