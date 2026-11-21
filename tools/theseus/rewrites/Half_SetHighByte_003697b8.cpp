// Replaces the high byte of the halfword at a0 with a1 (low byte kept). Returns the new halfword with the high byte sign-extended. 1 call site.
#include "theseus.h"

void Half_SetHighByte_003697b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t value = g.read8(g.arg(0)) | (static_cast<uint32_t>(static_cast<int32_t>(static_cast<int8_t>(g.arg(1)))) << 8);
    g.write16(g.arg(0), static_cast<uint16_t>(value));
    g.ret(value);
}
