// Read `count` bits starting at bit `start` from a bit array of u32 words (22 callers).
// Replaces FUN_002ac6b8 (array = a0: its first word points to the words; start = a1;
// count = a2). Bits are numbered from the least significant bit of word 0.
// MIPS shifts use only the low 5 bits of the amount (C++ shifts of 32 or more are undefined), so
// every shift amount is masked with & 31 explicitly. That gives the original's edge cases:
//  - count == 32 (or a multiple of 32) inside one word: the mask is 0 and the whole word returns
//  - the word index of `start` uses an arithmetic shift, the index of the last bit a logical one
#include "theseus.h"

void BitArray_Read_002ac6b8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t array = g.arg(0), start = g.arg(1), count = g.arg(2);
    if (g.arg64(2) == 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t end = start + count;
    const uint32_t first = static_cast<uint32_t>(static_cast<int32_t>(start) >> 5);
    const uint32_t last = (end - 1) >> 5;
    const uint32_t bit = start & 31;
    if (first == last)
    {
        const uint32_t mask = (1u << (count & 31)) - 1;
        const uint32_t word = g.read32(g.read32(array) + first * 4);
        g.ret(mask == 0 ? word : (word >> bit) & mask);
        return;
    }
    // The bits straddle two words: high part from word `last`, low part from word `first`.
    const uint32_t words = g.read32(array);
    const uint32_t high = g.read32(words + last * 4);
    const uint32_t low = g.read32(words + first * 4);
    const uint32_t high_mask = (1u << (end & 31)) - 1;
    g.ret(((high & high_mask) << ((0u - bit) & 31)) | (low >> bit));
}
