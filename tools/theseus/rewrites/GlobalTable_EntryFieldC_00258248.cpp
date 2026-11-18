// Field +0xC of entry i of a global pointer table at 0x3F92C8 (77 callers). Replaces
// FUN_00258248 (a0 unused, i = a1). What the table holds is not known yet.
#include "theseus.h"

namespace
{
    constexpr uint32_t kTable = 0x003F92C8;
}

void GlobalTable_EntryFieldC_00258248(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t entry = g.read32(kTable + g.arg(1) * 4);
    g.ret(g.read32(entry + 0xC));
}
