// Searches the list from a1'\''s +8 (next +8) for the node whose +0xC equals a1'\''s +0xC; when found and a2 != 0, *a2 = the node'\''s +0x10. Returns the node, or 0. 1 call site.
#include "theseus.h"

void List8_FindKeyC_002b01d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t key = g.read32(g.arg(1) + 0xC);
    for (uint32_t node = g.read32(g.arg(1) + 8); node != 0; node = g.read32(node + 8))
    {
        if (g.read32(node + 0xC) == key)
        {
            if (g.arg64(2) != 0)
                g.write32(g.arg(2), g.read32(node + 0x10));
            g.ret(node);
            return;
        }
    }
    g.ret(0);
}
