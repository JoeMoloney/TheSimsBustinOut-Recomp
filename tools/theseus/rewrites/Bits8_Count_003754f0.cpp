// The number of set bits in the low byte of a0. 1 call site.
#include "theseus.h"

void Bits8_Count_003754f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const __m128i v0_entry = GPR_VEC(ctx, 2), v1_entry = GPR_VEC(ctx, 3), a1_entry = GPR_VEC(ctx, 5);  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    const uint32_t bits = g.arg(0) & 0xFF;
    uint32_t count = 0;
    for (int bit = 0; bit < 8; ++bit)  // gate: equivalent <=,9: bit 8 of a masked byte is 0
        count += (bits >> bit) & 1;
    // v0's upper half as the original's chain of `movn v0, v1/a1` leaves it (bits 1-4 and 6 copy
    // v1, bits 5 and 7 copy a1; the last one set wins); the static caller check sees v0
    const __m128i upper = (bits & 0x80) ? a1_entry : (bits & 0x40) ? v1_entry : (bits & 0x20) ? a1_entry  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
                        : (bits & 0x1E) ? v1_entry : v0_entry;  // only scratch registers / v0's upper half, kept for the caller check (not in the ABI compare); gate: equivalent
    g.ret_with_upper(count, upper);
}
