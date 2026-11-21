// Heap allocation inside the free block a3 (block header: +0 previous physical block, +4 size * 8 | flags, +8 next free, +0xC previous free) at the user pointer a1 (header at a1 - 8) for a2 bytes (rounded up to 4, at least 8), heap a0 (free-list ends +0x38 / +0x3C, start of the last block +0x40, heap end +0x44, stats +0x50..+0x60). A leading gap of 16 bytes or more stays a free block (shrunk); a smaller one leaves the free list and is given to the previous physical block. A trailing gap of 16 bytes or more becomes a new free block linked next to the old one; a smaller one stays in the allocation. The allocated block'\''s size word gets flags 0, and the stats and the global allocation counter 0x3A8A8C are updated. Returns a1. 2 call sites.
#include "theseus.h"

void Heap_CarveBlock_002b0d90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t heap = g.arg(0), user = g.arg(1), block = g.arg(3);
    const __m128i entry_v0 = GPR_VEC(ctx, 2);  // scratch register for callers (not ABI-compared); gate: equivalent
    bool v0_upper_zeroed = false;  // `movz v0, $zero` (next block past the heap end) clears all 128 bits
    uint32_t size = (g.arg(2) + 3) & ~3u;
    if (!(7 < size))
        size = 8;
    const uint32_t header = user - 8;
    const uint32_t block_end = block + (g.read32(block + 4) >> 3);
    const uint32_t lead = header - block;
    const uint32_t trail = block_end - (user + size);
    uint32_t total = size + 8;
    // the next physical block, or 0 past the heap end
    const auto next_phys = [&](uint32_t b) {
        const uint32_t n = b + (g.read32(b + 4) >> 3);
        return n < g.read32(heap + 0x44) ? n : 0;
    };
    uint32_t neighbour;  // the free block the trailing gap is linked next to (0: append)
    uint32_t v0;
    if (!(lead < 16))
    {
        // keep the leading part as a free block
        const uint32_t n = block_end < g.read32(heap + 0x44) ? block_end : 0;
        neighbour = block;
        if (n != 0)
            g.write32(n, header);
        g.write32(user - 8, block);
        g.write32(block + 4, (g.read32(block + 4) & 7) | (lead << 3));
        g.write32(heap + 0x5C, g.read32(heap + 0x5C) + 1);
        v0 = g.read32(header + 4);
    }
    else
    {
        // take the block off the free list
        neighbour = g.read32(block + 8);
        const uint32_t prev_free = g.read32(block + 0xC);
        if (neighbour != 0)
            g.write32(neighbour + 0xC, prev_free);
        else
            g.write32(heap + 0x38, prev_free);
        const uint32_t p = g.read32(block + 0xC), n = g.read32(block + 8);
        if (p != 0)
            g.write32(p + 8, n);
        else
            g.write32(heap + 0x3C, n);
        if (lead == 0)
            v0 = g.read32(header + 4);
        else
        {
            // give the small leading gap to the previous physical block
            const uint32_t after = next_phys(block);
            v0_upper_zeroed |= after == 0;
            const uint32_t prev_phys = g.read32(block);
            g.write32(header, prev_phys);
            if (after != 0)
                g.write32(after, header);
            if (prev_phys == 0)
                g.write32(heap + 0x40, header);
            else
            {
                const uint32_t s = g.read32(prev_phys + 4);
                g.write32(prev_phys + 4, (s & 7) | (((s >> 3) + lead) << 3));
                g.write32(heap + 0x58, g.read32(heap + 0x58) + lead);
            }
            g.write32(heap + 0x54, g.read32(heap + 0x54) - lead);
            v0 = g.read32(header + 4);
        }
    }
    const uint32_t with_trail = total + trail;
    g.write32(header + 4, (v0 & 7) | (with_trail << 3));
    if (trail < 16)
        total = with_trail;
    else
    {
        // the trailing gap becomes a free block
        const uint32_t fb = header + total;
        g.write32(fb, header);
        g.write32(fb + 4, (trail << 3) | 1);
        const uint32_t after = next_phys(header);
        v0_upper_zeroed |= after == 0;
        if (after != 0)
            g.write32(after, fb);
        if (neighbour == 0)
        {
            // append at the +0x38 end
            g.write32(fb + 0xC, g.read32(heap + 0x38));
            const uint32_t last = g.read32(heap + 0x38);
            if (last == 0)
                g.write32(heap + 0x3C, fb);
            else
                g.write32(last + 8, fb);
            g.write32(fb + 8, 0);
            g.write32(heap + 0x38, fb);
        }
        else
        {
            const uint32_t before = g.read32(neighbour + 0xC);
            if (before == 0)
            {
                // insert at the +0x3C end
                g.write32(fb + 8, g.read32(heap + 0x3C));
                g.write32(g.read32(heap + 0x3C) + 0xC, fb);
                g.write32(fb + 0xC, 0);
                g.write32(heap + 0x3C, fb);
            }
            else
            {
                // insert before neighbour
                g.write32(before + 8, fb);
                g.write32(fb + 0xC, g.read32(neighbour + 0xC));
                g.write32(neighbour + 0xC, fb);
                g.write32(fb + 8, neighbour);
            }
        }
        g.write32(heap + 0x5C, g.read32(heap + 0x5C) + 1);
    }
    g.write32(header + 4, total << 3);
    const uint32_t count = g.read32(0x3A8A8C), allocs = g.read32(heap + 0x50), free_bytes = g.read32(heap + 0x54);
    const uint32_t used_bytes = g.read32(heap + 0x58), allocs2 = g.read32(heap + 0x60), free_blocks = g.read32(heap + 0x5C);
    g.write32(0x3A8A8C, count + 1);
    g.write32(heap + 0x50, allocs + 1);
    g.write32(heap + 0x5C, free_blocks - 1);
    g.write32(heap + 0x54, free_bytes - total);
    g.write32(heap + 0x58, used_bytes + total);
    g.write32(heap + 0x60, allocs2 + 1);
    // daddu v0, a1: v0's low 64 bits; its upper half is what the movz left (not ABI-compared)
    g.ret_with_upper(g.arg64(1), v0_upper_zeroed ? _mm_setzero_si128() : entry_v0);  // gate: equivalent true,false: only v0's upper half
}
