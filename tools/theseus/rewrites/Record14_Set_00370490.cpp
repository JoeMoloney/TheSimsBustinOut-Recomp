// Copies 16 bytes from a1 to a0 (both read first) and stores a2 at a0+0x10. Returns a0 (v1 = the second 8 bytes). 1 call site.
#include "theseus.h"

void Record14_Set_00370490(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint64_t w0 = g.read64(g.arg(1)), w1 = g.read64(g.arg(1) + 8);
    g.write64(g.arg(0), w0);
    g.write64(g.arg(0) + 8, w1);
    g.write32(g.arg(0) + 0x10, g.arg(2));
    SET_GPR_U64(ctx, 3, w1);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}
