// Finds the node in the list whose head pointer is at a0 (next pointer at +8) whose first word
// equals a1 (all 64 bits, as `bne`); 0 if none (5 call sites).
#include "theseus.h"

void List8_Find_002b3d08(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t node = g.read32(g.arg(0));
    while (node != 0)
    {
        if (static_cast<int64_t>(static_cast<int32_t>(g.read32(node))) == g.sarg64(1))
        {
            g.ret(node);
            return;
        }
        node = g.read32(node + 8);
    }
    g.ret(0);
}
