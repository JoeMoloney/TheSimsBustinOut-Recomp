#include <ps2_runtime.h>
#include <runtime/ee_scheduler.h>
#include <raylib.h>

#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <array>
#include <string>
#include <thread>
#include <utility>

#ifdef SIMS_SUPPLEMENTAL_FUNCTIONS
void registerSupplementalFunctions(PS2Runtime &runtime);
#endif
#ifdef SIMS_NATIVE_BINDINGS
void registerNativeBindings(PS2Runtime &runtime);
#endif

int main(int argc, char **argv) {
    if (argc < 2 || argc > 4) {
        std::cerr << "Usage: sims_boot_probe ELF [seconds] [--visible]\n";
        return 2;
    }
    int seconds = 30;
    bool visible = false;
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--visible") {
            visible = true;
        } else {
            try {
                size_t end = 0;
                seconds = std::stoi(argv[i], &end);
                if (end != std::string(argv[i]).size() || seconds < 1 || seconds > 3600)
                    return 2;
            } catch (...) {
                return 2;
            }
        }
    }
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    PS2Runtime runtime;
    runtime.setMissingFunctionPolicy(PS2Runtime::MissingFunctionPolicy::Stop);
#ifdef SIMS_SUPPLEMENTAL_FUNCTIONS
    registerSupplementalFunctions(runtime);
#endif
#ifdef SIMS_NATIVE_BINDINGS
    registerNativeBindings(runtime);
#endif
    if (!visible) SetConfigFlags(FLAG_WINDOW_HIDDEN);
    if (!runtime.initialize("The Sims Bustin' Out — native boot probe")) return 3;
    if (!runtime.loadELF(argv[1])) return 4;

    // Keep probe saves separate from the extracted, user-owned disc files.
    auto paths = PS2Runtime::getIoPaths();
    paths.mcRoot = std::filesystem::absolute("work/sims-bustin-out/runtime-data/mc0");
    PS2Runtime::setIoPaths(paths);
    std::cout << "[probe] disc root: " << paths.cdRoot << '\n';
    std::mutex mutex;
    std::condition_variable finished;
    bool done = false;
    std::thread watchdog([&] {
        std::unique_lock lock(mutex);
        if (!finished.wait_for(lock, std::chrono::seconds(seconds), [&] { return done; })) {
            std::cerr << "[probe] time limit; requesting stop\n";
            runtime.requestStop();
        }
    });
    runtime.run();
    {
        std::lock_guard lock(mutex);
        done = true;
    }
    finished.notify_one();
    watchdog.join();
    std::cout << "[probe] runtime returned; DMA=" << runtime.memory().dmaStartCount()
              << " GIF=" << runtime.memory().gifCopyCount()
              << " GS=" << runtime.memory().gsWriteCount()
              << " VIF=" << runtime.memory().vifWriteCount() << '\n';
    for (const auto &[name, address] : std::array<std::pair<const char *, uint32_t>, 12>{
             {{"VIF0_CHCR", 0x10008000}, {"VIF0_QWC", 0x10008020}, {"VIF0_TADR", 0x10008030},
              {"VIF1_CHCR", 0x10009000}, {"VIF1_QWC", 0x10009020}, {"VIF1_TADR", 0x10009030},
              {"VIF0_STAT", 0x10003800}, {"VIF1_STAT", 0x10003c00},
              {"VIF0_FBRST", 0x10003810}, {"VIF1_FBRST", 0x10003c10},
              {"DMAC_STAT", 0x1000e010}, {"INTC_STAT", 0x1000f000}}})
        std::cout << "[probe:HW] " << name << "=0x" << std::hex << runtime.memory().read32(address)
                  << std::dec << '\n';
    const auto iop = runtime.iopDebugSnapshot();
    std::cout << "[probe] IOP modules=" << iop.emulatorLoadedModules
              << " threads=" << iop.emulatorThreads << " RPC servers=" << iop.emulatorRpcServers
              << " instructions=" << iop.emulatorInstructions << '\n';
    const auto ee = runtime.eeScheduler().snapshot();
    std::cout << "[probe] EE cycle=" << ee.eeCycle << " runningThread=" << ee.runningThreadId << '\n';
    for (const auto &thread : ee.threads) {
        std::cout << "[probe:EE] thread " << thread.id << " pc=0x" << std::hex << thread.pc
                  << " ra=0x" << thread.ra << " entry=0x" << thread.entry << std::dec
                  << " prio=" << thread.currentPriority
                  << " status=" << static_cast<int>(thread.status)
                  << " waitReason=" << static_cast<int>(thread.waitReason)
                  << " waitId=" << thread.waitId << '\n';
    }
    for (const auto &semaphore : ee.semaphores)
        std::cout << "[probe:EE] sema " << semaphore.id << " count=" << semaphore.count
                  << " max=" << semaphore.maxCount << " waiters=" << semaphore.waiters << '\n';
    for (const auto &flag : ee.eventFlags)
        std::cout << "[probe:EE] evf " << flag.id << " bits=0x" << std::hex << flag.bits << std::dec
                  << " waiters=" << flag.waiters << '\n';
    for (const uint32_t workerBase : std::array<uint32_t, 4>{0x3f9300, 0x450bdc, 0x44ed84, 0x29366c}) {
        std::cout << "[probe:EE] worker base=0x" << std::hex << workerBase << std::dec;
        for (uint32_t off = 0; off < 0x40; off += 4)
            std::cout << " +0x" << std::hex << off << '=' << runtime.memory().read32(workerBase + off);
        const uint32_t vt = runtime.memory().read32(workerBase + 0x20);
        if (vt & 0x1FFFFFFF & ~0u) {
            std::cout << " vt=0x" << vt << " vt+10=" << runtime.memory().read32(vt + 0x10)
                      << " vt+14=" << runtime.memory().read32(vt + 0x14)
                      << " vt+18=" << runtime.memory().read32(vt + 0x18);
        }
        std::cout << std::dec << '\n';
    }
    for (const auto &diagnostic : iop.diagnostics)
        std::cout << "[probe:IOP] " << diagnostic << '\n';
    for (const auto &service : iop.services) {
        if (!service.name.starts_with("IOP ")) continue;
        std::cout << "[probe:IOP] " << service.name;
        for (const auto sid : service.sids) std::cout << " sid=0x" << std::hex << sid << std::dec;
        for (const auto &metric : service.metrics) {
            std::cout << ' ' << metric.name << '=';
            if (metric.hexadecimal) std::cout << "0x" << std::hex;
            std::cout << metric.value << std::dec;
        }
        std::cout << '\n';
    }
}
