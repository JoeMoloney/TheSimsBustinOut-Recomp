// Points the string at a0 at the shared static rep 0x3A8AC8 and returns that address. 27 call sites.
#include "theseus.h"

void Str_InitStatic3A8AC8_002bae68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0x003A8AC8);
    g.ret(0x003A8AC8);
}
