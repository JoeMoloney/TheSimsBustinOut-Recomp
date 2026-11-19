// Frees the block a1 with Heap_Free unless it is one of the two static blocks 0x3A8AD0 /
// 0x3A8AD8 (all 64 bits compared). Without the call, v0 is left holding the constant it was
// compared with, as in the original. 103 call sites.
#include "theseus.h"

void Str_FreeUnlessStatic_002bb9d8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.set_arg64(0, g.arg64(1));  // daddu a0, a1, $zero (on every path)
    if (g.sarg64(1) == 0x3A8AD0)
    {
        g.ret(0x3A8AD0);
        return;
    }
    if (g.sarg64(1) == 0x3A8AD8)
    {
        g.ret(0x3A8AD8);
        return;
    }
    g.call(0x268500, 0x2BBA08);
    g.ret_void();
}
