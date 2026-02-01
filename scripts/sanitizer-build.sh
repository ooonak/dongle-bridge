#!/bin/bash

export PATH=/usr/lib/llvm-20/bin:$PATH

west build -p -b native_sim -d build/ci-native-asan app -- \
  -DZEPHYR_TOOLCHAIN_VARIANT=llvm \
  -DEXTRA_CONF_FILE=overlays/sanitizers.conf

timeout 10 ./build/ci-native-asan/zephyr/zephyr.exe

#valgrind --leak-check=full ./build/ci-native-asan/zephyr/zephyr.exe

#west twister -v -p native_sim -T app/tests -- \
#  -c clang \
#  -T overlays/sanitizers.conf
