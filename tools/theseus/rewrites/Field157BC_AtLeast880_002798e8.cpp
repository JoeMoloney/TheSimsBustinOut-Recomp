// 1 if the signed word at a0+0x157BC is >= 0x880, else 0. 1 call site.
#include "theseus.h"

void Field157BC_AtLeast880_002798e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(static_cast<int32_t>(g.read32(g.arg(0) + 0x157BC)) >= 0x880 ? 1 : 0);
}
