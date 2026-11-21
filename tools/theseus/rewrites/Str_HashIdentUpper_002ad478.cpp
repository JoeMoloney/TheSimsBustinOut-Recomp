// CRC-32 (table at 0x3A8650) of a string normalised as an identifier: a-z upper-cased, A-Z and
// 0-9 kept, anything else hashed as '_'. Starts from 0xFFFFFFFF, or from [0x3A88D0] ^ 0xFFFFFF
// when the string starts with a digit; returns the complement. 2 call sites.
#include "theseus.h"

void Str_HashIdentUpper_002ad478(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t s = g.arg(0);
    const uint8_t first = g.read8(s);  // gate: equivalent read16: truncated to uint8_t
    uint32_t crc = static_cast<uint32_t>(first - '0') < 10 ? g.read32(0x003A88D0) ^ 0x00FFFFFF : 0xFFFFFFFF;
    if (first != 0)
    {
        for (int8_t c = g.sread8(s); c != 0; c = g.sread8(++s))  // gate: equivalent read8: stored in an int8_t
        {
            int8_t ch = c;
            if (c >= 'a' && c <= 'z')
                ch = static_cast<int8_t>(c - 0x20);
            else if (!(c >= 'A' && c <= 'Z') && !(c >= '0' && c <= '9'))
                ch = '_';
            crc = (crc >> 8) ^ g.read32(0x003A8650 + 4 * ((crc ^ static_cast<uint32_t>(ch)) & 0xFF));
        }
    }
    g.ret(~crc);
}
