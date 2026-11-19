// Maps the signed 16-bit value in a0: 1 -> 0x100, 2 -> 0x1000, 3 -> 0x2000, anything else -> 0
// (3 call sites).
#include "theseus.h"

void Kind16_ToFlag_002c73d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    switch (static_cast<int16_t>(g.arg(0)))
    {
    case 1: g.ret(0x100); return;
    case 2: g.ret(0x1000); return;
    case 3: g.ret(0x2000); return;
    default: g.ret(0); return;
    }
}
