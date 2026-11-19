// Calls 0x276B48 with the same a0..a3 and t0 = 1, t1 = 0 (the 5th and 6th arguments in the EE
// ABI); v0 is the callee's. 156 call sites.
#include "theseus.h"

void Call276B48_T0is1_00276f10(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.set_arg(4, 1);   // addiu t0, $zero, 1
    g.set_arg64(5, 0); // daddu t1, $zero, $zero
    g.call(0x276B48, 0x276F24);
    g.ret_void();
}
