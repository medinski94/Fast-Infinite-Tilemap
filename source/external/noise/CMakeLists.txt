message(STATUS "Fetching FastNoiseSIMD...")

FetchContent_Declare(
  fastnoise-simd
  URL https://github.com/Auburn/FastNoiseSIMD/archive/refs/tags/0.7.zip
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_GetProperties(fastnoise-simd)
if(NOT fastnoise-simd_POPULATED)
    FetchContent_Populate(fastnoise-simd)

    file(READ ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD.h data)
    string(REGEX REPLACE "#define FN_COMPILE_AVX2" "" out "${data}")
    file(WRITE ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD.h "${out}")

    add_library(fastnoise-simd STATIC
        ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD.cpp
        ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_avx2.cpp
        ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_avx512.cpp
        ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_internal.cpp
        ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_neon.cpp
        ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_sse2.cpp
        ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_sse41.cpp 
    )

    target_include_directories(fastnoise-simd PUBLIC ${fastnoise-simd_SOURCE_DIR}/FastNoiseSIMD)

    set_target_properties(fastnoise-simd PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY ${fastnoise-simd_BINARY_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${fastnoise-simd_BINARY_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${fastnoise-simd_BINARY_DIR}
    )
endif()
