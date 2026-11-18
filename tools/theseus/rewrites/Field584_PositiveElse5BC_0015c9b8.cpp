// 1 if the signed field at +0x584 is positive, else the field at +0x5BC (23 callers).
// Replaces FUN_0015c9b8. Both fields belong to a large object; their meaning is not known yet.
#include "theseus.h"

void Field584_PositiveElse5BC_0015c9b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t self = g.arg(0);
    if (static_cast<int32_t>(g.read32(self + 0x584)) > 0)
        g.ret(1);
    else
        g.ret(g.read32(self + 0x5BC));
}
