// Destructor: stores the vtable 0x3F08A0 at +0 of this, then operator delete(this) if bit 0 of
// a1 is set. v0 is the callee's, or 0x3F08A0 (left from building the address). 50 call sites.
#include "theseus.h"

void Dtor_Vtable3f08a0_00338198(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0), 0x3F08A0);
    g.set_arg64(1, g.arg64(1) & 1);  // andi a1, a1, 1
    if (g.arg64(1) != 0)
    {
        g.call(0x102610, 0x3381BC);
        g.ret_void();
        return;
    }
    g.ret(0x3F08A0);
}
