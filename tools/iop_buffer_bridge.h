#pragma once

#include "ps2_iop_transport.h"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace ps2_native {
// SDK ExecModuleBuffer takes an IOP pointer; LoadModuleBuffer takes an EE
// pointer. Keep these address spaces distinct even when their numbers match.
inline void execIopModuleBuffer(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime) {
    const uint32_t buffer = getRegU32(ctx, 4);
    const uint32_t size = getRegU32(ctx, 5);
    const uint32_t argumentsAddress = getRegU32(ctx, 6);
    const uint32_t resultAddress = getRegU32(ctx, 7);
    auto fail = [&] { setReturnS32(ctx, -1); };
    // A zero IOP address means the caller's sceSifAllocIopHeap failed.
    if (!runtime || !rdram || buffer == 0u || size > 64u * 1024u) return fail();
    std::vector<uint8_t> arguments(size);
    if (size) {
        if (argumentsAddress > UINT32_MAX - (size - 1u)) return fail();
        const auto *first = getConstMemPtr(rdram, argumentsAddress);
        const auto *last = getConstMemPtr(rdram, argumentsAddress + size - 1u);
        if (!first || !last || last < first || static_cast<size_t>(last - first) != size - 1u)
            return fail();
        std::memcpy(arguments.data(), first, size);
    }
    uint8_t *resultPointer = nullptr;
    if (resultAddress) {
        if (resultAddress > UINT32_MAX - 3u) return fail();
        resultPointer = getMemPtr(rdram, resultAddress);
        const auto *last = getMemPtr(rdram, resultAddress + 3u);
        if (!resultPointer || !last || last < resultPointer || last - resultPointer != 3)
            return fail();
    }
    const auto loaded = PS2IopTransport::loadModuleBufferFromIop(
        runtime, rdram, ctx, buffer, arguments.empty() ? nullptr : arguments.data(), size);
    if (resultPointer) std::memcpy(resultPointer, &loaded.startResult, sizeof(loaded.startResult));
    setReturnS32(ctx, loaded.moduleId);
    std::cerr << "[native:ExecModuleBuffer] iop=0x" << std::hex << buffer << std::dec
              << " module=" << loaded.moduleId << " start=" << loaded.startResult << '\n';
}

// SCE sceSifLoadStartModule(path, argLen, args, int *result): load an IRX by
// IOP path (e.g. "cdrom0:\DEV9.IRX;1") and run its start routine. On hardware
// this is loadfile RPC function 0; here the IOP emulator loads it directly.
inline void loadStartIopModule(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime) {
    constexpr uint32_t kMaxPath = 252u; // the SDK strncpy()s 0xFC bytes
    const uint32_t pathAddress = getRegU32(ctx, 4);
    const uint32_t argumentSize = getRegU32(ctx, 5);
    const uint32_t argumentsAddress = getRegU32(ctx, 6);
    const uint32_t resultAddress = getRegU32(ctx, 7);
    auto fail = [&] { setReturnS32(ctx, -1); };
    if (!runtime || !rdram || argumentSize > 64u * 1024u) return fail();

    std::string path;
    for (uint32_t i = 0; i < kMaxPath; ++i) {
        const auto *c = getConstMemPtr(rdram, pathAddress + i);
        if (!c) return fail();
        if (*c == 0) break;
        path.push_back(static_cast<char>(*c));
    }
    if (path.empty()) return fail();

    std::vector<uint8_t> arguments(argumentSize);
    for (uint32_t i = 0; i < argumentSize; ++i) {
        const auto *b = getConstMemPtr(rdram, argumentsAddress + i);
        if (!b) return fail();
        arguments[i] = *b;
    }

    const auto loaded = runtime->loadIopModule(
        path, arguments.empty() ? nullptr : arguments.data(), argumentSize);
    if (resultAddress) {
        if (auto *result = getMemPtr(rdram, resultAddress))
            std::memcpy(result, &loaded.startResult, sizeof(loaded.startResult));
    }
    setReturnS32(ctx, loaded.handled && loaded.moduleId > 0 ? loaded.moduleId : -1);
    std::cerr << "[native:LoadStartModule] path=" << path << " module=" << loaded.moduleId
              << " start=" << loaded.startResult << '\n';
}
}
