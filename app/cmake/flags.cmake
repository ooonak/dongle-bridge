# cmake/flags.cmake
# Zephyr 4.3 sanitized compiler flags
# Safe for GCC (ARM) and Clang (native + ASAN)

# -------------------------------
# Debug: print compiler and target info
# -------------------------------
message(STATUS "=== Zephyr build flags info ===")
message(STATUS "CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_C_COMPILER_ID=${CMAKE_C_COMPILER_ID}")
message(STATUS "CONFIG_GCC=${CONFIG_GCC}")
message(STATUS "CONFIG_CLANG=${CONFIG_CLANG}")
message(STATUS "CONFIG_NATIVE_APPLICATION=${CONFIG_NATIVE_APPLICATION}")
message(STATUS "SANITIZER_BUILD=${SANITIZER_BUILD}")
message(STATUS "================================")

# -------------------------------
# GCC-only flags (ARM embedded)
# -------------------------------
zephyr_compile_options_ifdef(CONFIG_GCC
  -Wall
  -Wextra
  -Werror
  -Wshadow
  -Wformat=2
  -Wundef
  -Wcast-align
  -Wstrict-prototypes
  -Wmissing-prototypes
  -Wdouble-promotion

  # GCC-specific optimizations/warnings
  -Wlogical-op
  -Wduplicated-cond
  -Wduplicated-branches
  -Wnull-dereference

  # Reduce Zephyr/GCC noise
  -Wno-undef
  -Wno-strict-prototypes
)

# -------------------------------
# Clang-only flags (native + ASan)
# -------------------------------
if(CONFIG_CLANG AND SANITIZER_BUILD)
  message(STATUS "Enabling Clang ASan + UBSan flags")

  target_compile_options(app PRIVATE
    -Weverything
    -Werror

    # High-value warnings
    -Wshadow
    -Wformat=2
    -Wundef
    -Wcast-align
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wdouble-promotion

    # Suppress Zephyr/LLVM noise
    -Wno-padded
    -Wno-disabled-macro-expansion
    -Wno-gnu-offsetof-extensions
    -Wno-reserved-id-macro

    # Sanitizers
    -fsanitize=address,undefined,integer
    -fno-sanitize-recover=all
    -fno-omit-frame-pointer
    -O1
    -g
  )

  target_link_options(app PRIVATE
    -fsanitize=address,undefined,integer
  )
endif()

# -------------------------------
# Optional: custom native-only warnings
# -------------------------------
if(CONFIG_NATIVE_APPLICATION AND CONFIG_CLANG AND NOT SANITIZER_BUILD)
  message(STATUS "Native Clang build (non-ASan) — optional warnings")
  target_compile_options(app PRIVATE
    -Wall
    -Wextra
    -Wshadow
    -Wformat=2
    -Wundef
  )
endif()

# -------------------------------
# End of flags.cmake
# -------------------------------
