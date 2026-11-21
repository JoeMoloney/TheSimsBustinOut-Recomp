// Address for offset a1 through the page table [a0+0x20] (4 KB pages): table[a1 / 4096] +
// (a1 & 0xFFF), the division truncating toward zero (sign of a1 taken on all 64 bits). 1 call site.
#include "theseus.h"

void PageTable20_Address_002c3890(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i v0_entry = GPR_VEC(ctx, 2), a1_entry = GPR_VEC(ctx, 5);  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    const uint32_t offset = g.arg(1);
    const bool negative = g.sarg64(1) < 0;  // gate: equivalent <=,1: a1 == 0 rounds the same either way
    const uint32_t rounded = negative ? offset + 0xFFF : offset;
    const int32_t page = static_cast<int32_t>(rounded) >> 12;
    const uint32_t address = g.read32(g.read32(g.arg(0) + 0x20) + (static_cast<uint32_t>(page) << 2)) + (offset & 0xFFF);
    // v0's upper half: `movn v0, a1` copies all of a1 when a1 >= 0 (the static caller check sees v0)
    g.ret_with_upper(static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(address))), negative ? v0_entry : a1_entry);
}
