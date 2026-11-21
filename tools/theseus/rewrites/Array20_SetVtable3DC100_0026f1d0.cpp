// Stores the vtable 0x3DC100 in the first word of each of a1 0x20-byte objects at a0 (a constructor loop). Nothing for a1 == 0 (whole register; a count whose low 32 bits are 0 runs 2^32 times, as in the original). v0 = the last object (untouched for a1 == 0). 1 call site.
#include "theseus.h"

void Array20_SetVtable3DC100_0026f1d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    if (g.arg64(1) == 0)
    {
        g.ret_void();
        return;
    }
    uint32_t p = g.arg(0), left = g.arg(1), last = 0;
    do
    {
        last = p;
        g.write32(p, 0x3DC100);
        p += 0x20;
    } while (--left != 0);
    SET_GPR_S32(ctx, 6, -1);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 5, 0x3DC100);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(last);
}
