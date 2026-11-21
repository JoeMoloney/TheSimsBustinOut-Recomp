// Element a1 + 1 of the word array *a0 (([a0])[a1 + 1]). 1 call site.
#include "theseus.h"

void PtrArray_At1_00275ea8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.read32(g.arg(0)) + (g.arg(1) << 2) + 4));
}
