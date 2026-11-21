// Converts two signed bytes at a1 into two words at a0, each (b << 4) | 8 (a 12.4 fixed-point
// value at the centre of its cell): a0+0 from a1[1], a0+4 from a1[0], stored as one 8-byte
// write. Returns a0. 4 call sites. (The original builds the pair in its stack frame first.)
#include "theseus.h"

void Bytes2_ToFixed4Pair_002cc260(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t src = g.arg(1);
    const uint32_t first = (static_cast<uint32_t>(static_cast<int32_t>(g.sread8(src + 1))) << 4) | 8;
    const uint32_t second = (static_cast<uint32_t>(static_cast<int32_t>(g.sread8(src))) << 4) | 8;
    g.write64(g.arg(0), (static_cast<uint64_t>(second) << 32) | first);
    g.ret_copy_of_arg(0);
}
