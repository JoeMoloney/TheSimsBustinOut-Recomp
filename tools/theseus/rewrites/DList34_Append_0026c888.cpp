// Appends node a1 to the doubly linked list at a0+0x34 (head +0x34, tail +0x38; node prev +0xB0, next +0xB4, list +0xAC). Returns the list address (a0 + 0x34). 2 call sites. (The map joined other functions' epilogues after the `jr $ra`.)
#include "theseus.h"

void DList34_Append_0026c888(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t list = g.arg(0) + 0x34, node = g.arg(1);
    g.write32(node + 0xB0, g.read32(list + 4));
    const uint32_t tail = g.read32(list + 4);
    if (tail == 0)
        g.write32(list, node);
    else
        g.write32(tail + 0xB4, node);
    g.write32(node + 0xB4, 0);
    g.write32(list + 4, node);
    g.write32(node + 0xAC, list);
    g.ret(list);
}
