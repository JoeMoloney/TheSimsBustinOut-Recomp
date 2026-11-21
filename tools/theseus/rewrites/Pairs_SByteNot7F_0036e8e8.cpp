// 1 unless the signed byte at a0 + a1 * 2 is 0x7F. 1 call site.
#include "theseus.h"

void Pairs_SByteNot7F_0036e8e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.sread8(g.arg(0) + (g.arg(1) << 1)) != 0x7F ? 1 : 0);  // gate: equivalent read8: only the value 0x7F matters, the same either way
}
