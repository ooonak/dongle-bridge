#!/bin/bash

export PATH=/usr/lib/llvm-20/bin:$PATH

west build -p -b native_sim -d build/ci-native-asan app/tests/app_logic -- \
  -DZEPHYR_TOOLCHAIN_VARIANT=llvm \
  -DEXTRA_CONF_FILE=$(pwd)/overlays/sanitizers.conf \
  -DSANITIZER_BUILD=1
west build -t run -d build/ci-native-asan

valgrind --leak-check=full --show-leak-kinds=all ./build/ci-native-asan/zephyr/zephyr.exe
