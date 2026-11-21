// 1 if the signed byte a0 is a tab (9) or a space (0x20), else 0. 1 call site.
#include "theseus.h"

void Char_IsBlank_001026c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int8_t c = static_cast<int8_t>(g.arg(0));
    g.ret(c == 9 || c == 0x20 ? 1 : 0);
}
