// A clamped cursor over the counted word buffer **a0 (count in the word before it; no buffer = 0 words): if the index a0+4 is below the count, returns the element'\''s address and increments the index; otherwise returns the last element'\''s address (buffer - 4 when there is no buffer). 1 call site. Pairs with Field4_SetLastIndex_00360878.
#include "theseus.h"

void Cursor_NextOrLast_00360810(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t holder = g.read32(g.arg(0));
    const uint32_t buf = g.read32(holder);
    const int32_t count = static_cast<int32_t>(buf == 0 ? 0 : g.read32(buf - 4));
    const int32_t index = static_cast<int32_t>(g.read32(g.arg(0) + 4));
    if (index < count)
    {
        g.write32(g.arg(0) + 4, static_cast<uint32_t>(index + 1));
        g.ret(g.read32(holder) + (static_cast<uint32_t>(index) << 2));
        return;
    }
    const uint32_t b = g.read32(holder);
    const uint32_t last = (b == 0 ? 0 : g.read32(b - 4)) - 1;
    g.ret(g.read32(holder) + (last << 2));
}
