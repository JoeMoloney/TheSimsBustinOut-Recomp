// The signed word at +0x48 of a0 divided by 2, rounding toward zero (`srl 31` + `addu` + `sra`).
// 8 call sites.
#include "theseus.h"

void Field48_Half_0033cd68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t v = static_cast<int32_t>(g.read32(g.arg(0) + 0x48));
    g.ret(static_cast<uint32_t>(v / 2));
}
