// Initialises the heap a0 over the memory a1 .. a1 + a2: start rounded up to 4, size = what is left rounded down to 4. With at least 16 bytes it writes one free block header at the start (+0, +8, +0xC = 0, +4 = size * 8 | 1); with less there is no block (start and size 0). Then the heap fields: +0x64 = +0x54 = size, +0x44 = start + size, +0x48 = size + 1, +0x5C = (block present), +0x38/+0x3C/+0x40 = start, +0x50/+0x58/+0x60 = 0. Returns (block present). 2 call sites.
#include "theseus.h"

void Heap_InitArena_002b0870(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t heap = g.arg(0);
    uint32_t start = (g.arg(1) + 3) & ~3u;
    uint32_t size = (g.arg(2) - (start - g.arg(1))) & ~3u;
    if (size < 16)
    {
        size = 0;
        start = 0;
    }
    else
    {
        g.write32(start, 0);
        g.write32(start + 0xC, 0);
        g.write32(start + 8, 0);
        g.write32(start + 4, (size << 3) | 1);
    }
    const uint32_t present = start != 0;
    g.write32(heap + 0x64, size);
    g.write32(heap + 0x44, start + size);
    g.write32(heap + 0x48, size + 1);
    g.write32(heap + 0x5C, present);
    g.write32(heap + 0x40, start);
    g.write32(heap + 0x3C, start);
    g.write32(heap + 0x38, start);
    g.write32(heap + 0x50, 0);
    g.write32(heap + 0x54, size);
    g.write32(heap + 0x58, 0);
    g.write32(heap + 0x60, 0);
    g.ret(present);
}
