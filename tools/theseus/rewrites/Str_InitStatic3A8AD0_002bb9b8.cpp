// Points the string at a0 at the shared static rep 0x3A8AD0 (an empty string; see
// Str_FreeUnlessStatic_002bb9d8) and returns that address. 61 call sites.
#include "theseus.h"

void Str_InitStatic3A8AD0_002bb9b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0x003A8AD0);
    g.ret(0x003A8AD0);
}
