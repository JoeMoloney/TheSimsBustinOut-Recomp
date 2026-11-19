// Word +4 minus word +0 of a0: a vector's size in bytes (4 call sites).
#include "theseus.h"

void Vector_Bytes_003471e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 4) - g.read32(g.arg(0)));
}
