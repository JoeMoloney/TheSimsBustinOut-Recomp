// *a0 |= *a1 (bytes). Returns a0 (v1 = the new byte, a2 = *a1). 1 call site. (The map joined other functions after the `jr $ra`.)
#include "theseus.h"

void Byte_OrFrom_00375aa8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint8_t other = g.read8(g.arg(1));  // gate: equivalent read16: kept as a byte
    const uint8_t value = g.read8(g.arg(0)) | other;  // gate: equivalent read16: kept as a byte
    g.write8(g.arg(0), value);
    SET_GPR_S32(ctx, 3, value);  // scratch register for callers (not ABI-compared); gate: equivalent
    SET_GPR_S32(ctx, 6, other);  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret_copy_of_arg(0);
}
