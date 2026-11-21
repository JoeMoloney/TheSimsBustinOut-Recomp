// Ors a1 into the word at a0+0x354. The `or` works on whole registers, so v0 = the 64-bit result (a1'\''s upper bits included); the low 32 bits are stored. 1 call site.
#include "theseus.h"

void Field354_OrBits_00169b48(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t field = g.arg(0) + 0x354;
    const uint64_t value = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(g.read32(field)))) | g.arg64(1);
    g.write32(field, static_cast<uint32_t>(value));
    g.ret64(value);
}
