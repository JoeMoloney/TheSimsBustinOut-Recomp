// Are all bits of `mask` set in the flag word at a0? (34 callers). Replaces FUN_00345150
// (flags = a0, mask = a1). Computed on 64 bits like the original (`nor`, `and`, `sltiu`): the
// loaded word is sign-extended and the mask register is used whole.
#include "theseus.h"

void Flags_HasAll_00345150(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t flags = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(g.arg(0)))));
    g.ret((~flags & g.arg64(1)) == 0);
}
