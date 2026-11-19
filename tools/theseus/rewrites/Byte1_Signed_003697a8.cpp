// Bits 8..15 of a0 as a signed byte (`sll 16` + `sra 24`). 4 call sites.
#include "theseus.h"

void Byte1_Signed_003697a8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(static_cast<uint32_t>(static_cast<int32_t>(static_cast<int8_t>(g.arg(0) >> 8))));
}
