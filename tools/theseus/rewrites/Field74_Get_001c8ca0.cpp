// Returns the word at a0+0x74. 8 call sites. (The function map joined the code after this
// `jr $ra`, 0x1c8ca8, which sets the global 0x3F7FE0: another function, not reached from here.)
#include "theseus.h"

void Field74_Get_001c8ca0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 0x74));
}
