// Calls 0x1399C0(this), then sets word +0x80 of this to -1 and returns -1 (77 call sites).
#include "theseus.h"

void Call1399C0_Reset80_00139a60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t self = g.arg(0);
    g.call(0x1399C0, 0x139A74);
    g.write32(self + 0x80, 0xFFFFFFFFu);
    g.ret(0xFFFFFFFFu);
}
