// Word +0x1C of the object at +0x14 of a0, or 0 if that pointer is null (4 call sites).
#include "theseus.h"

void Field14_Field1C_OrZero_0035bd28(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.read32(g.arg(0) + 0x14);
    g.ret(obj != 0 ? g.read32(obj + 0x1C) : 0);
}
