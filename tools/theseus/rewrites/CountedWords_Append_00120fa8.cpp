// Appends a1 to the counted word array at a0 (count +0, words from +4; no bound check): words[count] = a1, count re-read and incremented. Returns a0 + count * 4 (v1 = the new count). 1 call site. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void CountedWords_Append_00120fa8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t list = g.arg(0), slot = list + (g.read32(list) << 2);
    g.write32(slot + 4, g.arg(1));
    const uint32_t count = g.read32(list) + 1;
    g.write32(list, count);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(count));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(slot);
}
