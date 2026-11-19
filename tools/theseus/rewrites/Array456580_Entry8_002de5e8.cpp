// Address of 8-byte entry a0 of the array whose base is the global at 0x456580 (4 call sites).
#include "theseus.h"

void Array456580_Entry8_002de5e8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(0x456580) + (g.arg(0) << 3));
}
