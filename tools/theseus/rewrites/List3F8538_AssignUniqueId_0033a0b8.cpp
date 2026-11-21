// Gives node a0 an id no other node in the global list (head 0x3F8538; +0 id, +4 next) uses:
// starting at a1, the candidate is incremented and the scan restarted whenever another node
// holds it; then *a0 = the id. v0 = the id last read from a node (untouched if none was read).
// 4 call sites. (The map joined another function after the `jr $ra`.)
#include "theseus.h"

void List3F8538_AssignUniqueId_0033a0b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t self = g.arg(0);
    const int64_t self64 = g.sarg64(0);
    int64_t id = g.sarg64(1);  // compared as a whole register; `addiu` makes it 32-bit
    bool read_any = false;
    uint32_t last = 0;  // gate: equivalent 1: only returned after a read sets it
    uint32_t node = g.read32(0x3F8538);
    while (node != 0)
    {
        if (static_cast<int64_t>(static_cast<int32_t>(node)) != self64)
        {
            last = g.read32(node);
            read_any = true;
            if (static_cast<int32_t>(last) == id)
            {
                id = static_cast<int32_t>(static_cast<uint32_t>(id) + 1);
                node = g.read32(0x3F8538);
                continue;
            }
        }
        node = g.read32(node + 4);
    }
    g.write32(self, static_cast<uint32_t>(id));
    if (read_any)
        g.ret(last);
    else
        g.ret_void();
}
