// True if the word at +0x12C of the object in a0 is 6 (55 call sites).
#include "theseus.h"

void Field12C_Is6_00162298(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x12C) == 6 ? 1 : 0);
}
