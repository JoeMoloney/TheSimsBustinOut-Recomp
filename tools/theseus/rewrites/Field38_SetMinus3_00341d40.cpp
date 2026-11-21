// Sets the word at a0+0x38 to -3 and returns -3. 3 call sites.
#include "theseus.h"

void Field38_SetMinus3_00341d40(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    g.write32(g.arg(0) + 0x38, 0xFFFFFFFDu);
    g.ret(0xFFFFFFFDu);
}
