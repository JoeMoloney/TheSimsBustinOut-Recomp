// Resets an object: zeroes 19 fields (+0x34, +0x48..+0x7C, +0x84, +0x88, +0x98, +0x9C), clears
// the vector at +0x24 (0x14-byte elements, nothing to destroy: end = begin), sets +0x80 = 1 and
// +0x90 = 0, then clears the vector at +0x14 (0xC-byte elements). The original walks both
// vectors with empty loops (the elements' destructors compiled to nothing); v0 is what the
// second walk leaves: begin when the vector was empty, else end + 0xC. 6 call sites.
#include "theseus.h"

void Object_Reset24_14_0033bf18(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const uint32_t obj = g.arg(0);
    static const uint8_t kZero[] = {0x34, 0x48, 0x50, 0x54, 0x58, 0x5C, 0x60, 0x64, 0x68, 0x6C,
                                    0x4C, 0x70, 0x74, 0x78, 0x7C, 0x84, 0x88, 0x98, 0x9C};
    for (uint8_t off : kZero)
        g.write32(obj + off, 0);
    g.write32(obj + 0x28, g.read32(obj + 0x24));
    g.write32(obj + 0x80, 1);
    const uint32_t begin = g.read32(obj + 0x14);
    g.write32(obj + 0x90, 0);
    const uint32_t end = g.read32(obj + 0x18);
    g.write32(obj + 0x18, begin);
    g.ret(begin == end ? begin : end + 0xC);
}
