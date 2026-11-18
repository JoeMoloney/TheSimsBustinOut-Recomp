// bstring.cpp: take another reference to a string representation and return it.
// Replaces FUN_002c85c8 (5 instructions). The reference count is the u32 at +0xC; the shared
// "empty string" representations (0x4564F0, 0x456500) use the same field.
#include "theseus.h"

void bstring_AddRef_002c85c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t rep = g.arg(0);
    g.write32(rep + 0xC, g.read32(rep + 0xC) + 1);
    g.ret_copy_of_arg(0);
}
