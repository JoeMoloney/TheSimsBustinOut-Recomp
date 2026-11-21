// Initialiser: +8 = 0x3D3D28, +0 = 3. Returns a0 (v1 = 3). (The original also writes two words of a temporary in its stack frame.) 1 call site.
#include "theseus.h"

void Object_Init3D3D28_00222988(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 8, 0x3D3D28);
    g.write32(g.arg(0), 3);
    SET_GPR_S32(ctx, 3, 3);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}
