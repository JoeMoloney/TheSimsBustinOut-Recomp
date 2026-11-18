// The game's memcpy (EA library code, next to the assert handler at 0x268488; 221 callers).
// Replaces FUN_00268588. Copies n bytes 8 at a time when dst, src and n are all multiples of 8,
// else 4 at a time when they are all multiples of 4, else byte by byte (front to back, so
// overlapping buffers behave exactly as in the original).
// Quirk kept: returns NULL instead of dst when n == 0.
#include "theseus.h"

void Mem_Copy_00268588(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t dst = g.arg(0), src = g.arg(1);
    const uint32_t n = g.arg(2);
    if (n == 0)
    {
        g.ret(0);
        return;
    }
    const uint32_t alignment = dst | src | n;
    if ((alignment & 7) == 0)
    {
        for (uint32_t left = n >> 3; left != 0; --left, src += 8, dst += 8)
            g.write64(dst, g.read64(src));
    }
    else if ((alignment & 3) == 0)
    {
        for (uint32_t left = n >> 2; left != 0; --left, src += 4, dst += 4)
            g.write32(dst, g.read32(src));
    }
    else
    {
        for (uint32_t left = n; left != 0; --left, src += 1, dst += 1)
            g.write8(dst, g.read8(src));  // gate: equivalent read16: write8 keeps only the low byte
    }
    g.ret_copy_of_arg(0);
}
