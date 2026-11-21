// Grows the rectangle {left +0, top +4, right +8, bottom +0xC} at a0 by a1 horizontally and a2 vertically on each side. Returns 1. 3 call sites.
#include "theseus.h"

void Rect_Inflate_00350cd8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t r = g.arg(0), dx = g.arg(1), dy = g.arg(2);
    const uint32_t left = g.read32(r), right = g.read32(r + 8), top = g.read32(r + 4), bottom = g.read32(r + 0xC);
    g.write32(r, left - dx);
    g.write32(r + 8, right + dx);
    g.write32(r + 0xC, bottom + dy);
    g.write32(r + 4, top - dy);
    g.ret(1);
}
