// CRC-32 of the NUL-terminated string at a0 with the table at 0x3A8650 (see Crc32). Characters
// are taken as signed bytes, so for bytes >= 0x80 the table index is out of range and the
// original reads before the table; that is reproduced. 3 call sites.
#include "theseus.h"

void Str_Crc32_002ad410(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
{
    theseus::Guest g{rdram, ctx, runtime};
    uint32_t p = g.arg(0), crc = 0xFFFFFFFFu;
    for (int32_t c; (c = g.sread8(p)) != 0; ++p)
        crc = (crc >> 8) ^ g.read32(0x3A8650 + (((crc & 0xFF) ^ static_cast<uint32_t>(c)) << 2));
    g.ret(~crc);
}
