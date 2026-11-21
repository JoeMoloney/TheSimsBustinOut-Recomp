// Sets the global 0x3F8EB0 = 0x469028 + 0x469050 (= 0x8D2078, as the original computes it). v0 = 0x469050, v1 = the sum. 1 call site.
#include "theseus.h"

void Global3F8EB0_Set_00263128(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(0x3F8EB0, 0x469028u + 0x469050u);
    SET_GPR_S32(ctx, 3, static_cast<int32_t>(0x469028u + 0x469050u));  // scratch register for callers (not ABI-compared); gate: equivalent
    g.ret(0x469050);
}
