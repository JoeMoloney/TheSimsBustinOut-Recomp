// bstring.cpp: make a string share the global empty representation, and return the string.
// Replaces FUN_002c8a68. The empty rep lives in .bss at 0x4564F0 and is reference-counted
// like any other (+0xC). The string's rep pointer is stored before the count is read, as in
// the original (it matters only if the string overlaps the count).
#include "theseus.h"

namespace
{
    constexpr uint32_t kEmptyRep = 0x004564F0;
}

void bstring_InitEmpty_002c8a68(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t self = g.arg(0);
    g.write32(self, kEmptyRep);
    g.write32(kEmptyRep + 0xC, g.read32(kEmptyRep + 0xC) + 1);
    g.ret_copy_of_arg(0);
}
