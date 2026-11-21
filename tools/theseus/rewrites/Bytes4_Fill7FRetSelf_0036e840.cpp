// Sets the 4 bytes at a0 to 0x7F (in the order 0, 3, 2, 1); returns a0. 2 call sites. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void Bytes4_Fill7FRetSelf_0036e840(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t p = g.arg(0);
    g.write8(p, 0x7F);  // gate: equivalent write16: p+1 is stored last
    g.write8(p + 3, 0x7F);
    g.write8(p + 2, 0x7F);
    g.write8(p + 1, 0x7F);
    g.ret_copy_of_arg(0);
}
