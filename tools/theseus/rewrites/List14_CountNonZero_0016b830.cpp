// Walks the list starting at a1 (next pointer at +0x14) and counts the nodes whose first word
// is nonzero (6 call sites).
#include "theseus.h"

void List14_CountNonZero_0016b830(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t n = 0;
    if (g.arg64(1) != 0)
    {
        uint32_t node = g.arg(1);
        do
        {
            if (g.read32(node) != 0)
                ++n;
            node = g.read32(node + 0x14);
        } while (node != 0);
    }
    g.ret(n);
}
