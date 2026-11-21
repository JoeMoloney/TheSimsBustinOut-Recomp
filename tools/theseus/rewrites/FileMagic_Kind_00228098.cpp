// Identifies a file header by its first bytes (read big-endian): C0 FB -> 1, "BIGF" -> 2,
// "BIG" + any byte -> 3, else 0. (EA BIG archive headers; C0 FB is probably a compressed
// header.) 2 call sites.
#include "theseus.h"

void FileMagic_Kind_00228098(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    const uint32_t b0 = g.read8(p), b1 = g.read8(p + 1);
    if ((b0 << 8 | b1) == 0xC0FB)
    {
        g.ret(1);
        return;
    }
    const uint32_t magic = b0 << 24 | b1 << 16 | uint32_t(g.read8(p + 2)) << 8 | g.read8(p + 3);
    if (magic == 0x42494746)  // "BIGF"
        g.ret(2);
    else
        g.ret((magic & 0xFFFFFF00) == 0x42494700 ? 3 : 0);  // "BIG?"
}
