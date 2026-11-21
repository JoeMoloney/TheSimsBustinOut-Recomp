// The table at 0x3A46C0 has 7 columns of 9 words (the first column canonical, the other six
// aliases). Returns a0 unchanged (all 64 bits) if it is in column 0 or nowhere; else the column-0
// entry in the same row as the first alias column (in column order) that contains it. 1 call site.
#include "theseus.h"

void Table3A46C0_Canonical_0015eba8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    constexpr uint32_t kTable = 0x003A46C0;
    const int64_t key = g.sarg64(0);
    const auto at = [&](uint32_t column, uint32_t row) { return g.read32(kTable + 0x24 * column + 4 * row); };
    for (uint32_t row = 0; row < 9; ++row)
        if (static_cast<int32_t>(at(0, row)) == key)
        {
            g.ret_copy_of_arg(0);
            return;
        }
    for (uint32_t column = 1; column < 7; ++column)
        for (uint32_t row = 0; row < 9; ++row)
            if (static_cast<int32_t>(at(column, row)) == key)
            {
                g.ret(at(0, row));
                return;
            }
    g.ret_copy_of_arg(0);
}
