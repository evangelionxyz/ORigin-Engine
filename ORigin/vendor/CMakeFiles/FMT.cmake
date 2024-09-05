add_library(FMT STATIC
    ${VENDOR_DIR}/FMT/src/fmt.cc
    ${VENDOR_DIR}/FMT/src/format.cc
    ${VENDOR_DIR}/FMT/src/os.cc
    ${VENDOR_DIR}/FMT/include/fmt/args.h
    ${VENDOR_DIR}/FMT/include/fmt/base.h
    ${VENDOR_DIR}/FMT/include/fmt/chrono.h
    ${VENDOR_DIR}/FMT/include/fmt/color.h
    ${VENDOR_DIR}/FMT/include/fmt/compile.h
    ${VENDOR_DIR}/FMT/include/fmt/core.h
    ${VENDOR_DIR}/FMT/include/fmt/format.h
    ${VENDOR_DIR}/FMT/include/fmt/os.h
    ${VENDOR_DIR}/FMT/include/fmt/ostream.h
    ${VENDOR_DIR}/FMT/include/fmt/printf.h
    ${VENDOR_DIR}/FMT/include/fmt/ranges.h
    ${VENDOR_DIR}/FMT/include/fmt/std.h
    ${VENDOR_DIR}/FMT/include/fmt/xchar.h
)

# Modules require C++20.
target_compile_features(FMT PRIVATE cxx_std_20)
if (CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(FMT PRIVATE -fmodules-ts)
endif ()

target_include_directories(FMT PRIVATE
    ${VENDOR_DIR}/FMT/include
)

target_compile_definitions(FMT PRIVATE
    FMT_MODULE
)