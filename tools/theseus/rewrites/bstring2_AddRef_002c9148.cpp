// bstring2.cpp: take another reference to a string representation and return it.
// Replaces FUN_002c9148, an identical copy of bstring_AddRef_002c85c8 (the compiler emitted
// the same inline function in both files). Reference count: u32 at +0xC.
#include "theseus.h"

void bstring2_AddRef_002c9148(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t rep = g.arg(0);
    g.write32(rep + 0xC, g.read32(rep + 0xC) + 1);
    g.ret_copy_of_arg(0);
}
