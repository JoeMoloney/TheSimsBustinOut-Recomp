// Pads a word buffer (+0x70 write pointer, +0x74 start) with zero words to a multiple of 4
// words, then ors (words / 4 - 1, at least 0) into the buffer's first word (a block-count
// header) and returns that count. v0's upper half is what the original's movn/movz leave.
// 4 call sites.
#include "theseus.h"

void WordBuf_PadTo16_002a53d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i entry_v1 = GPR_VEC(ctx, 3);  // gate: equivalent 4: only v0's upper half (not ABI-compared)
    const uint32_t buf = g.arg(0);
    // only (end - start) mod 16 matters here, so a 16-bit read of either pointer is equivalent
    const uint32_t used = g.read32(buf + 0x70) - g.read32(buf + 0x74);  // gate: equivalent read16: only the low 4 bits matter
    int32_t pad = static_cast<int32_t>(static_cast<uint32_t>(-(static_cast<int32_t>(used) >> 2)) & 3);
    for (; pad > 0; --pad)
    {
        const uint32_t p = g.read32(buf + 0x70);
        g.write32(p, 0);
        g.write32(buf + 0x70, p + 4);
    }
    const uint32_t start = g.read32(buf + 0x74);
    const int32_t words = static_cast<int32_t>(g.read32(buf + 0x70) - start) >> 2;
    const uint32_t header = g.read32(start);
    // the original divides by 4 rounding toward zero (movn picks words or words + 3 before
    // the shift); after the padding words is a multiple of 4, so a plain shift is exact
    const int32_t blocks = (words >> 2) - 1;
    const uint32_t count = blocks > -1 ? static_cast<uint32_t>(blocks) : 0;
    g.write32(start, header | count);  // gate: equivalent write16: count < 0x10000 for any buffer in reach (the upper half stays the header's)
    if (blocks <= -1)
        g.ret_with_upper(0, _mm_setzero_si128());  // movz v0, $zero: all 128 bits
    else
        g.ret_with_upper(static_cast<uint64_t>(static_cast<int64_t>(blocks)), entry_v1);  // blocks >= 0 means words >= 4: movn took v1
}
