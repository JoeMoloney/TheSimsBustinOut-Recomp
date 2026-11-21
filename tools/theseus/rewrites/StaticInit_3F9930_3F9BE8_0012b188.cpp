// A static initialiser (runs when a1 == 0xFFFF and a0 != 0, whole registers): zeroes the 24 bytes at 0x3F9930 (the original zeroes +0xC..+0x17 through a stack temporary, then copies them to +0..+0xB) and the 3 words at 0x3F9BE8 (+0, +8, +4). Between the two it runs empty nested loops (array constructors that compiled to nothing). Returns 0x3F9BE8, or 0xFFFF when it does nothing. 2 call sites.
#include "theseus.h"

void StaticInit_3F9930_3F9BE8_0012b188(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) != 0xFFFF)
    {
        g.ret(0xFFFF);
        return;
    }
    SET_GPR_S32(ctx, 6, 0x400000);  // a2: the `lui` in the next branch's delay slot; gate: equivalent
    if (g.arg64(0) == 0)
    {
        g.ret(0xFFFF);
        return;
    }
    g.write64(0x3F993C, 0);
    g.write32(0x3F9944, 0);
    g.write64(0x3F9930, 0);
    g.write32(0x3F9938, 0);
    g.write32(0x3F9BE8, 0);
    g.write32(0x3F9BF0, 0);
    g.write32(0x3F9BEC, 0);
    // scratch registers as the original leaves them (a0/a1/v1 = -1 from the loops, a3/t0 the copied zeros, t2 the base)
    SET_GPR_S32(ctx, 4, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 3, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_U64(ctx, 7, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 8, 0);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 10, 0x3F9930);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0x3F9BE8);
}
