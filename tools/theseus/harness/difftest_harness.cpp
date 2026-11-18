// Differential test: original recompiled EE function vs readable rewrite (Theseus).
//
// Built by tools/theseus/difftest.py, which links a small generated bind.cpp that forwards
// theseus_original/theseus_rewrite to the two functions under test. Both run on
// identical random CPU states and guest memory; the results are compared at two levels:
//   strict - every byte of the CPU context and the compared memory is equal
//   abi    - only what a caller may rely on under the EE calling convention:
//            v0 low 32 bits (the return value), callee-saved s0-s7, gp, sp, fp, ra (all 128
//            bits), f20-f31, fcr31, the pc returned to, memory, and runtime traffic
//            (calls, hardware accesses, syscalls). Scratch registers may differ.
// The upper 32 bits of v0's low half are reported separately (v0_high32): whether a caller
// ever reads them is a property of the callers.
//
// Guest memory: two 32 MiB RAM images (PS2 addresses wrap at 32 MiB, as in the generated code),
// each preloaded with the game's ELF segment so globals and constants hold real data. Per case:
//   - a 64 KB data window at 0x01000000 is refilled with random bytes (with frequent zero
//     bytes so strings end); pointer arguments point into its first 32 KB
//   - a 32 KB stack below 0x01F00000 is refilled; sp starts 4 KB below its top
// The window and the stack from sp up are compared after every case. All of RAM is compared
// every kScanEvery cases and at the end: memory outside the window and stack is never reset,
// so a stray write that differs stays visible until the next scan.
//
// Input rules (optional, tools/theseus/rewrites/<name>.rules, turned into difftest_rules.inc by
// difftest.py) fix what an argument register holds. Registers: a0..a7 ($4..$11) or rN.
//   a0 ptr | ptr_or_null | int | u32 | any | len N | slen N | fixed V | oneof V1 .. V8
//                                    (int/u32: a quarter of the time an edge value such as
//                                    0xFFFF, 0x10000, 0x7FFFFFFF or 0xFFFFFFFF)
//   a1 str N | wstr16 N | wstr32 N   pointer to a NUL-terminated string of 8/16/32-bit
//                                    characters, length 0..N (a quarter at N-1..N+1)
//   mem32 0x456500 ptr               a 32-bit global, set before each case
//   mem32 @a0+12 ptr                 a field of the struct an argument points to
//   mem32 *0x456500+12 len 4         a field of the struct a global points to
//   data 0x3e69a8 41424300           fixed bytes before each case
#include "ps2_runtime_macros.h"
#include "ps2_runtime.h"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

#include "runtime_stub.h"

void theseus_original(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime);  // bind.cpp
void theseus_rewrite(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime);

namespace
{
    constexpr uint32_t kRam = PS2_RAM_SIZE;
    constexpr uint32_t kWindow = 0x01000000, kWindowSize = 0x10000;
    constexpr uint32_t kStackTop = 0x01F00000, kStackSize = 0x8000, kSpOffset = 0x1000;
    constexpr uint32_t kDefaultGp = 0x003FFAF0;  // the game's $gp (seen in every probe run)
    constexpr long kScanEvery = 1000;

    struct Rng
    {
        uint64_t s;
        uint64_t next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return s; }
        uint32_t below(uint32_t n) { return uint32_t(next() % n); }
    };

    enum Kind { kAny, kPtr, kPtrOrNull, kInt, kU32, kLen, kSLen, kFixed, kOneOf, kStr8, kStr16, kStr32 };
    struct ArgRule { int reg; Kind kind; uint32_t max; uint32_t alts[8]; uint32_t n_alts; };
    struct MemRule { uint32_t addr; Kind kind; uint32_t max; int base; uint32_t deref; uint32_t alts[8]; uint32_t n_alts; };
    struct DataRule { uint32_t addr; const char *hex; };
#ifdef HAS_RULES
#include "difftest_rules.inc"
#else
    const ArgRule kRules[] = {{0, kAny, 0, {}, 0}};  // $0: never an argument, so unused
    const MemRule kMemRules[] = {{0, kAny, 0, 0, 0, {}, 0}};
    const DataRule kDataRules[] = {{0u, ""}};
#endif

    uint32_t window_ptr(Rng &r) { return kWindow + r.below(kWindowSize / 2); }

    uint64_t sext32(uint32_t v) { return uint64_t(int64_t(int32_t(v))); }

    // Values where carries, sign changes and width truncation show up (a quarter of u32/int).
    uint32_t edge32(Rng &r)
    {
        static const uint32_t edges[] = {0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, 0x100u, 0x7FFFu, 0x8000u, 0xFFFEu,
                                         0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu, 0xFFFFFFFFu};
        return edges[r.below(sizeof(edges) / sizeof(edges[0]))];
    }

    uint64_t ruled_value(Rng &r, Kind kind, uint32_t max, const uint32_t *alts, uint32_t n_alts)
    {
        switch (kind)
        {
        case kPtr: case kStr8: case kStr16: case kStr32: return sext32(window_ptr(r));
        case kPtrOrNull: return r.below(8) == 0 ? 0 : sext32(window_ptr(r));
        case kInt: return sext32(r.below(4) == 0 ? edge32(r) : uint32_t(r.next()));
        case kU32: return r.below(4) == 0 ? edge32(r) : uint32_t(r.next());
        case kLen: return r.below(max + 1);
        case kSLen: return sext32(uint32_t(int32_t(r.below(2 * max + 1)) - int32_t(max)));
        case kFixed: return sext32(max);
        case kOneOf: return sext32(alts[r.below(n_alts)]);
        default: return r.next();
        }
    }

    // Default argument mix when no rule applies.
    uint64_t arg_value(Rng &r)
    {
        switch (r.below(6))
        {
        case 0: case 1: return sext32(window_ptr(r));     // pointer into data
        case 2: return r.below(64);                       // small int / length
        case 3: return uint32_t(r.next());                // zero-extended 32-bit
        case 4: return sext32(uint32_t(r.next()));        // sign-extended 32-bit (canonical)
        default: return r.next();                         // any 64-bit value
        }
    }

    // A NUL-terminated string of `width`-byte characters at `at`, written into both RAM images.
    void put_string(uint8_t *a, uint8_t *b, Rng &r, uint32_t at, uint32_t max, uint32_t width)
    {
        const uint32_t len = r.below(4) == 0 ? max - 1 + r.below(3) : r.below(max + 1);
        for (uint32_t i = 0; i <= len; ++i)
        {
            uint32_t ch = 0;
            if (i < len)
            {
                // Mostly small characters (ASCII-like), sometimes any nonzero value of the width.
                const uint32_t mask = width == 1 ? 0xFFu : width == 2 ? 0xFFFFu : 0xFFFFFFFFu;
                ch = r.below(2) ? 1 + r.below(255) : (uint32_t(r.next()) & mask);
                if (ch == 0) ch = 1;
            }
            for (uint32_t k = 0; k < width; ++k)
            {
                const uint32_t addr = (at + i * width + k) & (kRam - 1);
                a[addr] = b[addr] = uint8_t(ch >> (8 * k));
            }
        }
    }

    void put32(uint8_t *a, uint8_t *b, uint32_t addr, uint32_t v)
    {
        for (uint32_t k = 0; k < 4; ++k)
            a[(addr + k) & (kRam - 1)] = b[(addr + k) & (kRam - 1)] = uint8_t(v >> (8 * k));
    }

    uint32_t get32(const uint8_t *m, uint32_t addr)
    {
        uint32_t v = 0;
        for (uint32_t k = 0; k < 4; ++k) v |= uint32_t(m[(addr + k) & (kRam - 1)]) << (8 * k);
        return v;
    }

    void load_elf(uint8_t *mem, const char *path)
    {
        // Single LOAD segment of SLES_520.48 (readelf -lW): file offset 0x1000 -> 0x100000.
        FILE *f = std::fopen(path, "rb");
        if (!f) { std::perror(path); std::exit(2); }
        std::fseek(f, 0x1000, SEEK_SET);
        const size_t n = std::fread(mem + 0x100000, 1, 0x2F88A8, f);
        std::fclose(f);
        if (n != 0x2F88A8) { std::fprintf(stderr, "short read from %s\n", path); std::exit(2); }
    }

    // Compares all RAM; returns differing 4 KB pages and copies a's version into b.
    long scan_ram(uint8_t *a, uint8_t *b, uint32_t *first_page)
    {
        long bad = 0;
        for (uint32_t p = 0; p < kRam; p += 4096)
            if (std::memcmp(a + p, b + p, 4096) != 0)
            {
                if (!bad && first_page) *first_page = p;
                ++bad;
                std::memcpy(b + p, a + p, 4096);
            }
        return bad;
    }

    struct Field { const char *name; size_t off, size; bool abi; };
    std::vector<Field> make_fields()
    {
        static const char *gpr[32] = {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
                                      "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                                      "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
                                      "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};
        std::vector<Field> f;
        for (int i = 0; i < 32; ++i)
        {
            const bool saved = (i >= 16 && i <= 23) || i >= 28;
            // v0 is checked separately (low 32 bits are ABI, the rest is reported).
            f.push_back({gpr[i], offsetof(R5900Context, r) + 16 * i, 16, saved});
        }
        f.push_back({"pc", offsetof(R5900Context, pc), sizeof(uint32_t), true});
        f.push_back({"hi", offsetof(R5900Context, hi), 8, false});
        f.push_back({"lo", offsetof(R5900Context, lo), 8, false});
        f.push_back({"hi1", offsetof(R5900Context, hi1), 8, false});
        f.push_back({"lo1", offsetof(R5900Context, lo1), 8, false});
        f.push_back({"sa", offsetof(R5900Context, sa), 4, false});
        static char fnames[32][8];
        for (int i = 0; i < 32; ++i)
        {
            std::snprintf(fnames[i], sizeof(fnames[i]), "f%d", i);
            f.push_back({fnames[i], offsetof(R5900Context, f) + 4 * i, 4, i >= 20});
        }
        f.push_back({"f_acc", offsetof(R5900Context, f_acc), 4, false});
        f.push_back({"fcr31", offsetof(R5900Context, fcr31), 4, true});
        f.push_back({"llbit", offsetof(R5900Context, llbit), 4, false});
        return f;
    }

    alignas(64) unsigned char g_runtime_storage[sizeof(PS2Runtime)];
}

int main(int argc, char **argv)
{
    const long cases = argc > 1 ? std::atol(argv[1]) : 200000;
    const uint64_t seed = argc > 2 ? std::strtoull(argv[2], nullptr, 0) : 0x9E3779B97F4A7C15ull;
    const bool stop_early = std::getenv("DIFFTEST_STOP") != nullptr;
    auto *runtime = reinterpret_cast<PS2Runtime *>(g_runtime_storage);

    std::unique_ptr<uint8_t[]> ram_a(new uint8_t[kRam]()), ram_b(new uint8_t[kRam]());
    uint8_t *mem_a = ram_a.get(), *mem_b = ram_b.get();
    const char *elf = std::getenv("THESEUS_ELF");
    if (!elf) { std::fprintf(stderr, "set THESEUS_ELF to the game ELF (difftest.py does)\n"); return 2; }
    load_elf(mem_a, elf);
    std::memcpy(mem_b, mem_a, kRam);

    const std::vector<Field> fields = make_fields();
    std::vector<long> field_bad(fields.size());
    Rng rng{seed};
    std::vector<uint8_t> window(kWindowSize), stack(kStackSize);

    long strict_bad = 0, abi_bad = 0, v0_bad = 0, v0_high_bad = 0, mem_bad = 0, frame_bad = 0, event_bad = 0;
    long first_abi_case = -1, stray_pages = 0, first_stray_scan = -1;
    uint32_t first_stray_page = 0;
    long total_returns = 0, total_transfers = 0, total_special = 0;

    for (long c = 0; c < cases; ++c)
    {
        // 8 random bytes per RNG step; in the window about 1 byte in 12 is zero (strings end).
        for (size_t i = 0; i < kWindowSize; i += 8)
        {
            uint64_t v = rng.next(), z = rng.next();
            for (int k = 0; k < 8; ++k, v >>= 8, z >>= 8)
                window[i + k] = (z & 0xFF) < 21 ? 0 : uint8_t(v);
        }
        for (size_t i = 0; i < kStackSize; i += 8)
        {
            const uint64_t v = rng.next();
            std::memcpy(stack.data() + i, &v, 8);
        }
        std::memcpy(mem_a + kWindow, window.data(), kWindowSize);
        std::memcpy(mem_b + kWindow, window.data(), kWindowSize);
        std::memcpy(mem_a + kStackTop - kStackSize, stack.data(), kStackSize);
        std::memcpy(mem_b + kStackTop - kStackSize, stack.data(), kStackSize);

        R5900Context init;
        auto *bytes = reinterpret_cast<uint8_t *>(&init);
        for (size_t i = 0; i < sizeof(init); ++i) bytes[i] = uint8_t(rng.next());
        init.r[0] = _mm_setzero_si128();
        auto set_low64 = [&](int reg, uint64_t v) { Ps2SetGprLow64(&init, reg, _mm_cvtsi64_si128(int64_t(v))); };
        for (int reg = 4; reg <= 11; ++reg) set_low64(reg, arg_value(rng));
        for (const ArgRule &rule : kRules)
            if (rule.reg >= 1 && rule.reg <= 31)
                set_low64(rule.reg, ruled_value(rng, rule.kind, rule.max, rule.alts, rule.n_alts));
        for (const ArgRule &rule : kRules)
            if (rule.kind == kStr8 || rule.kind == kStr16 || rule.kind == kStr32)
                put_string(mem_a, mem_b, rng, GPR_U32((&init), rule.reg), rule.max,
                           rule.kind == kStr8 ? 1 : rule.kind == kStr16 ? 2 : 4);
        const uint32_t sp = kStackTop - kSpOffset;
        set_low64(29, sp);
        bool gp_ruled = false;
        for (const ArgRule &rule : kRules) gp_ruled |= rule.reg == 28;
        if (!gp_ruled) set_low64(28, kDefaultGp);
        set_low64(31, 0x00100000u + 4 * rng.below(0x2A0000 / 4));  // a code address to return to
        // Addresses set by rules are reset every case, which would also erase a difference there
        // before the periodic RAM scan: compare them after every case instead.
        uint32_t ruled_addrs[64];
        size_t n_ruled = 0;
        for (const MemRule &m : kMemRules)
        {
            if (!m.addr && !m.base && !m.deref) continue;
            uint32_t addr = m.base ? GPR_U32((&init), m.base) + m.addr : m.addr;
            if (m.deref) addr = get32(mem_a, m.deref) + m.addr;
            put32(mem_a, mem_b, addr, uint32_t(ruled_value(rng, m.kind, m.max, m.alts, m.n_alts)));
            if (n_ruled < 64) ruled_addrs[n_ruled++] = addr;
        }
        for (const DataRule &d : kDataRules)
        {
            if (!d.addr) continue;
            if (n_ruled < 64) ruled_addrs[n_ruled++] = d.addr;  // first 4 bytes compared per case
            for (size_t i = 0; d.hex[2 * i]; ++i)
            {
                const char byte[3] = {d.hex[2 * i], d.hex[2 * i + 1], 0};
                mem_a[(d.addr + i) & (kRam - 1)] = mem_b[(d.addr + i) & (kRam - 1)] = uint8_t(std::strtoul(byte, nullptr, 16));
            }
        }
        init.in_delay_slot = false;
        init.pc = 0;

        R5900Context a = init, b = init;
        theseus_stub::g_events = {};
        theseus_original(mem_a, &a, runtime);
        const theseus_stub::Events ev_a = theseus_stub::g_events;
        theseus_stub::g_events = {};
        theseus_rewrite(mem_b, &b, runtime);
        const theseus_stub::Events ev_b = theseus_stub::g_events;
        total_returns += ev_a.returns;
        total_transfers += ev_a.transfers;
        total_special += ev_a.special_loads + ev_a.special_stores;

        const bool events_same = ev_a.hash == ev_b.hash && ev_a.returns == ev_b.returns &&
                                 ev_a.transfers == ev_b.transfers && ev_a.syscalls == ev_b.syscalls;
        event_bad += !events_same;
        bool ruled_same = true;
        for (size_t i = 0; i < n_ruled; ++i)
            ruled_same &= get32(mem_a, ruled_addrs[i]) == get32(mem_b, ruled_addrs[i]);
        const bool memory_same =
            ruled_same &&
            std::memcmp(mem_a + kWindow, mem_b + kWindow, kWindowSize) == 0 &&
            std::memcmp(mem_a + sp, mem_b + sp, kStackTop - sp) == 0;
        const bool frame_same = std::memcmp(mem_a + kStackTop - kStackSize, mem_b + kStackTop - kStackSize,
                                            kStackSize - kSpOffset) == 0;  // below sp: dead after return
        frame_bad += !frame_same;
        const bool v0_same = GPR_U32((&a), 2) == GPR_U32((&b), 2);
        const bool v0_high_same = (GPR_U64((&a), 2) >> 32) == (GPR_U64((&b), 2) >> 32);
        bool abi_fields_same = true;
        for (size_t i = 0; i < fields.size(); ++i)
        {
            const Field &f = fields[i];
            if (std::memcmp(reinterpret_cast<uint8_t *>(&a) + f.off, reinterpret_cast<uint8_t *>(&b) + f.off, f.size) != 0)
            {
                ++field_bad[i];
                if (f.abi) abi_fields_same = false;
            }
        }
        const bool abi_ok = memory_same && v0_same && abi_fields_same && events_same;
        const bool strict_ok = abi_ok && v0_high_same && frame_same && std::memcmp(&a, &b, sizeof(a)) == 0;
        strict_bad += !strict_ok;
        abi_bad += !abi_ok;
        v0_bad += !v0_same;
        v0_high_bad += !v0_high_same;
        mem_bad += !memory_same;
        if (!abi_ok && first_abi_case < 0)
        {
            first_abi_case = c;
            std::printf("first ABI failure (case %ld): a0 %016llx a1 %016llx a2 %016llx -> v0 original %016llx rewrite %016llx"
                        " | events original %ld ret/%ld out, rewrite %ld ret/%ld out\n",
                        c, (unsigned long long)GPR_U64((&init), 4), (unsigned long long)GPR_U64((&init), 5),
                        (unsigned long long)GPR_U64((&init), 6), (unsigned long long)GPR_U64((&a), 2),
                        (unsigned long long)GPR_U64((&b), 2), ev_a.returns, ev_a.transfers, ev_b.returns, ev_b.transfers);
            for (size_t i = 0; i < fields.size(); ++i)
                if (fields[i].abi && std::memcmp(reinterpret_cast<uint8_t *>(&a) + fields[i].off,
                                                 reinterpret_cast<uint8_t *>(&b) + fields[i].off, fields[i].size) != 0)
                    std::printf("  register %s differs\n", fields[i].name);
            int shown = 0;
            for (uint32_t off = 0; off < kWindowSize && shown < 16; off += 4)
                if (std::memcmp(mem_a + kWindow + off, mem_b + kWindow + off, 4) != 0)
                {
                    std::printf("  mem %08x: was %08x original %08x rewrite %08x\n", kWindow + off,
                                get32(window.data(), off), get32(mem_a, kWindow + off), get32(mem_b, kWindow + off));
                    ++shown;
                }
        }
        if ((c + 1) % kScanEvery == 0 || c + 1 == cases)
        {
            const long n = scan_ram(mem_a, mem_b, first_stray_scan < 0 ? &first_stray_page : nullptr);
            if (n && first_stray_scan < 0) first_stray_scan = c;
            stray_pages += n;
        }
        if (stop_early && (abi_bad || stray_pages))
        {
            if (!stray_pages) stray_pages = scan_ram(mem_a, mem_b, &first_stray_page);
            std::printf("stopped early after case %ld (DIFFTEST_STOP)\n", c);
            break;
        }
    }

    std::printf("cases=%ld seed=0x%llx\n", cases, (unsigned long long)seed);
    std::printf("abi_mismatches=%ld strict_mismatches=%ld\n", abi_bad, strict_bad);
    std::printf("v0_low32_mismatches=%ld v0_high32_mismatches=%ld memory_mismatches=%ld"
                " frame_mismatches=%ld (below sp, strict only)\n", v0_bad, v0_high_bad, mem_bad, frame_bad);
    std::printf("runtime_event_mismatches=%ld (original: returns=%ld calls/jumps out=%ld hardware accesses=%ld)\n",
                event_bad, total_returns, total_transfers, total_special);
    std::printf("differing_fields:");
    for (size_t i = 0; i < fields.size(); ++i)
        if (field_bad[i]) std::printf(" %s%s=%ld", fields[i].name, fields[i].abi ? "(ABI!)" : "", field_bad[i]);
    std::printf("\n");
    if (first_abi_case >= 0) std::printf("first_abi_failure_case=%ld\n", first_abi_case);
    std::printf("ram_page_mismatches=%ld", stray_pages);
    if (first_stray_scan >= 0) std::printf(" first_at_page=0x%08x by_case<=%ld", first_stray_page, first_stray_scan);
    std::printf(" (all RAM compared every %ld cases)\n", kScanEvery);
    std::printf("rules=%s\n", (sizeof(kRules) / sizeof(kRules[0]) > 1 || kRules[0].reg || kMemRules[0].addr || kMemRules[0].base || kDataRules[0].addr) ? "yes" : "none");
    const bool fail = abi_bad || stray_pages;
    std::printf("RESULT %s\n", fail ? "FAIL" : (strict_bad ? "PASS-ABI" : "PASS-STRICT"));
    return fail ? 1 : 0;
}
