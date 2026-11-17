# Permanent build for the Sims Bustin' Out native boot probe.
#
# Configure once from the repo root, then build incrementally:
#   cmake -S config/probe -B build/probe -DCMAKE_BUILD_TYPE=Release
#   cmake --build build/probe --target sims_boot_probe -j 8
# Run from the repo root:
#   build/probe/sims_boot_probe work/sims-bustin-out/disc-files/SLES_520.48 <seconds> [--visible]
#
# Inputs (kept out of Git): work/sims-bustin-out/{functions-only-output,supplemental-output,
# native-bindings.cpp, disc-files}. Regenerate the last two from config/ (see HANDOFF.md).
# Memory-card saves go to <repo>/runtime-data/{mc0,mc1}.
cmake_minimum_required(VERSION 3.21)
project(SimsBustinOutProbe LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(SIMS_REPO "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(SIMS_WORK "${SIMS_REPO}/work/sims-bustin-out")
set(SIMS_PS2RECOMP "${SIMS_REPO}/references/PS2Recomp")
set(SIMS_GENERATED_OPT "-O2" CACHE STRING "Optimisation flags for the ~11k generated game files")

# PS2Recomp's runtime uses SSE4.1/AVX2 intrinsics; the old /tmp build passed
# -DCMAKE_CXX_FLAGS=-mavx2 on the command line. Apply it to everything here instead.
add_compile_options(-mavx2)

set(PS2X_BUILD_RECOMP OFF CACHE BOOL "" FORCE)
set(PS2X_BUILD_RUNTIME ON CACHE BOOL "" FORCE)
set(PS2X_BUILD_ANALYZER OFF CACHE BOOL "" FORCE)
set(PS2X_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(PS2X_BUILD_STUDIO OFF CACHE BOOL "" FORCE)
set(PS2X_ENABLE_DEBUG_UI OFF CACHE BOOL "" FORCE)
set(PS2X_ENABLE_SCCACHE OFF CACHE BOOL "" FORCE)
add_subdirectory("${SIMS_PS2RECOMP}" ps2recomp)

# Main recompiled game functions.
file(GLOB SIMS_GENERATED CONFIGURE_DEPENDS "${SIMS_WORK}/functions-only-output/*.cpp")
add_library(sims_generated OBJECT ${SIMS_GENERATED})
target_include_directories(sims_generated PRIVATE "${SIMS_WORK}/functions-only-output")
separate_arguments(SIMS_GENERATED_OPT_LIST UNIX_COMMAND "${SIMS_GENERATED_OPT}")
target_compile_options(sims_generated PRIVATE -mavx2 ${SIMS_GENERATED_OPT_LIST})
target_link_libraries(sims_generated PRIVATE ps2_runtime)
target_precompile_headers(sims_generated PRIVATE <ps2_runtime_macros.h> <ps2_runtime.h>
    <ps2_recompiled_functions.h> <ps2_recompiled_stubs.h> <ps2_syscalls.h> <ps2_stubs.h>)

# Supplemental functions (config/supplemental-map.csv) and SDK bindings (config/native-bindings.csv).
add_library(sims_supplemental OBJECT "${SIMS_WORK}/supplemental-output/supplemental.cpp")
target_sources(sims_supplemental PRIVATE "${SIMS_WORK}/native-bindings.cpp")
target_include_directories(sims_supplemental PRIVATE "${SIMS_WORK}/supplemental-output"
    "${SIMS_REPO}/tools" "${SIMS_PS2RECOMP}/ps2xRuntime/src/lib")
target_compile_options(sims_supplemental PRIVATE -mavx2 ${SIMS_GENERATED_OPT_LIST})
target_link_libraries(sims_supplemental PRIVATE ps2_runtime)

add_executable(sims_boot_probe "${SIMS_REPO}/tools/boot_probe.cpp"
    $<TARGET_OBJECTS:sims_generated> $<TARGET_OBJECTS:sims_supplemental>)
target_compile_definitions(sims_boot_probe PRIVATE SIMS_SUPPLEMENTAL_FUNCTIONS SIMS_NATIVE_BINDINGS
    SIMS_RUNTIME_DATA_DIR="${SIMS_REPO}/runtime-data")
target_include_directories(sims_boot_probe PRIVATE "${SIMS_WORK}/functions-only-output")
target_compile_options(sims_boot_probe PRIVATE -mavx2 -O2)
target_link_libraries(sims_boot_probe PRIVATE ps2_runtime)
