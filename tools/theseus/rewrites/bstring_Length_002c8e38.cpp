// String length: the string's rep pointer (+0) points to its rep, whose +4 is the length
// (bstring.cpp layout; FUN_002c85e0 stores it). 42 call sites over 2 identical copies.
// theseus: copies 0x2cb258
#include "theseus.h"

void bstring_Length_002c8e38(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t rep = g.read32(g.arg(0));
    g.ret(g.read32(rep + 4));
}
