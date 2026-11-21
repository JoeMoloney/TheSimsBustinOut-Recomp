// 1 if the signed bytes a0 and a1 are equal ignoring ASCII case (a lower-case letter matches its
// upper-case form), else 0. 1 call site.
#include "theseus.h"

void Char_EqualNoCase_002aee50(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    const int32_t c1 = static_cast<int8_t>(g.arg(0)), c2 = static_cast<int8_t>(g.arg(1));
    const auto lower = [](int32_t c) { return static_cast<uint32_t>(c - 0x61) < 26u; };  // gate: equivalent <=,27: '{' as lower would need '[' as upper, which it is not
    const auto upper = [](int32_t c) { return c >= 0x41 && c < 0x5B; };  // gate: equivalent <=,0x5c: '[' as upper would need '{' as lower, which it is not
    if (c1 == c2)
        g.ret(1);
    else if (lower(c1) && upper(c2))
        g.ret(c1 - 0x20 == c2 ? 1 : 0);
    else if (lower(c2) && upper(c1))
        g.ret(c2 - 0x20 == c1 ? 1 : 0);
    else
        g.ret(0);
}
