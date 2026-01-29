message(STATUS "CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "posix")

  if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using strict GCC warnings for target")
    
    target_compile_options(app PRIVATE
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

      # GCC-specific gold
      -Wlogical-op
      -Wduplicated-cond
      -Wduplicated-branches
      -Wnull-dereference

      # Reduce noise from Zephyr
      -Wno-undef
      -Wno-strict-prototypes
    )
  endif()

  if(SANITIZER_BUILD)
    if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
      message(STATUS "Enabling ASan + UBSan for target")
      
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
      )

      target_compile_options(app PRIVATE
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
  endif()

else()
  message(STATUS "Skipping strict flags for board ${BOARD}")
endif()