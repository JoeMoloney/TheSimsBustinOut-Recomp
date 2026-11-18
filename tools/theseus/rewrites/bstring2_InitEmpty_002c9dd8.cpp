// bstring2.cpp: make a string share bstring2's global empty representation (0x456500).
// Replaces FUN_002c9dd8 (45 callers); same shape as bstring_InitEmpty_002c8a68.
#include "theseus.h"

namespace
{
    constexpr uint32_t kEmptyRep = 0x00456500;
}

void bstring2_InitEmpty_002c9dd8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t self = g.arg(0);
    g.write32(self, kEmptyRep);
    g.write32(kEmptyRep + 0xC, g.read32(kEmptyRep + 0xC) + 1);
    g.ret_copy_of_arg(0);
}
