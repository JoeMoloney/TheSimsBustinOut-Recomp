// Number of 16-byte elements between +0 (begin) and +4 (end) of a0, as (end - begin) >> 4
// signed (4 call sites).
#include "theseus.h"

void Vector_Count16_00351558(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(static_cast<uint32_t>(static_cast<int32_t>(g.read32(g.arg(0) + 4) - g.read32(g.arg(0))) >> 4));
}
