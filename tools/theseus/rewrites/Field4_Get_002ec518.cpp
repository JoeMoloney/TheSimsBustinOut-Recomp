// Returns the word at a0+4. 7 call sites. (The map joined two more accessors after this
// `jr $ra`: a halfword setter and getter at +0xC.)
#include "theseus.h"

void Field4_Get_002ec518(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.ret(g.read32(g.arg(0) + 4));
}
