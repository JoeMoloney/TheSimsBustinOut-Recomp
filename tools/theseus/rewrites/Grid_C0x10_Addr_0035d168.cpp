// Address of cell (a1, a2) in the grid whose base is at +0x28 of a0: base + a1 * 0xC0 + a2 * 16,
// or 0 if there is no grid (5 call sites).
#include "theseus.h"

void Grid_C0x10_Addr_0035d168(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t base = g.read32(g.arg(0) + 0x28);
    if (base == 0)
        g.ret(0);
    else
        g.ret(g.arg(1) * 0xC0 + base + (g.arg(2) << 4));
}
