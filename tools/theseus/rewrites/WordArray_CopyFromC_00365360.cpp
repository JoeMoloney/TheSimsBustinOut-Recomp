// Copies [a0+4] words from a1'\''s word array (+0xC) to a0'\''s (+0xC); both pointers and the count are re-read every step. Returns 0 (the last compare), or the count when it is <= 0. 2 call sites.
#include "theseus.h"

void WordArray_CopyFromC_00365360(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t dst = g.arg(0), src = g.arg(1);
    const int32_t count = static_cast<int32_t>(g.read32(dst + 4));
    if (count <= 0)
    {
        g.ret(static_cast<uint32_t>(count));
        return;
    }
    int32_t i = 0;
    do
    {
        const uint32_t off = static_cast<uint32_t>(i) << 2;
        const uint32_t from = g.read32(src + 0xC) + off;
        const uint32_t to = g.read32(dst + 0xC) + off;
        g.write32(to, g.read32(from));
        ++i;
    } while (i < static_cast<int32_t>(g.read32(dst + 4)));
    g.ret(0);
}
