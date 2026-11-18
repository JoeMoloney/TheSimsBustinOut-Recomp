// bstring: the reference count of a string's representation (string->rep->refCount).
// Replaces FUN_002c9078. Layout so far: string +0 = rep pointer; rep +0xC = reference count.
#include "theseus.h"

void bstring_RefCount_002c9078(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t rep = g.read32(g.arg(0));
    g.ret(g.read32(rep + 0xC));
}
