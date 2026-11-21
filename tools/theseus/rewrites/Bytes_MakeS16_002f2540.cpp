// (int16)(((a0 & 0xFF) << 8) | (a1 & 0xFF)). 1 call site.
#include "theseus.h"

void Bytes_MakeS16_002f2540(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(static_cast<int16_t>(((g.arg(0) & 0xFF) << 8) | (g.arg(1) & 0xFF)));
}
