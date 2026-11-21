// Releases block a2 (nothing for 0 or when its in-use flag, bit 0 of +0, is clear; v0 then untouched / 0): the stats a1 get min(+4) and sum(+8) of the block'\''s +4 / +8; the block is unlinked from a0'\''s two doubly linked lists (head +0x30 / tail +0x34 with next +0x1C / prev +0x18, and head +0x40 / tail +0x44 with next +0x24 / prev +0x20); its flag word is cleared and it is pushed onto the free list 0x4558D8 (link +0). Returns the old free-list head. 2 call sites.
#include "theseus.h"

void Block_Release_002aa248(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t owner = g.arg(0), stats = g.arg(1), block = g.arg(2);
    if (g.arg64(2) == 0)
    {
        g.ret_void();
        return;
    }
    const uint32_t flag = g.read32(block) & 1;
    if (flag == 0)
    {
        g.ret(0);
        return;
    }
    const int64_t block64 = g.sarg64(2);  // compared as a whole register
    const auto is_block = [&](uint32_t p) { return static_cast<int64_t>(static_cast<int32_t>(p)) == block64; };
    const uint32_t size = g.read32(block + 4);
    if (size < g.read32(stats + 4))
        g.write32(stats + 4, size);
    g.write32(stats + 8, g.read32(stats + 8) + g.read32(block + 8));
    // unlink from the list at owner + head_off (tail at +4), node links next_off / prev_off
    const auto unlink = [&](uint32_t head_off, uint32_t prev_off, uint32_t next_off) {
        const uint32_t list = owner + head_off;
        if (is_block(g.read32(list)))
            g.write32(list, g.read32(block + next_off));
        else
            g.write32(g.read32(block + prev_off) + next_off, g.read32(block + next_off));
        if (is_block(g.read32(list + 4)))
            g.write32(list + 4, g.read32(block + prev_off));
        else
            g.write32(g.read32(block + next_off) + prev_off, g.read32(block + prev_off));
    };
    unlink(0x30, 0x18, 0x1C);
    unlink(0x40, 0x20, 0x24);
    // the original first zeroes the flag word, which the push below overwrites at once
    const uint32_t old = g.read32(0x4558D8);
    g.write32(block, old);
    g.write32(0x4558D8, block);
    g.ret(old);
}
