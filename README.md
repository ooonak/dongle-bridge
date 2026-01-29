[![Zephyr Build & Test v4.3](https://github.com/ooonak/dongle-bridge/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/ooonak/dongle-bridge/actions/workflows/build_and_test.yml)

# dongle-bridge
Zephyr application implementing an USB to BLE bridge

## Build, test, run

```
$ docker run -ti -v <path>/zephyrproject:/workdir ghcr.io/zephyrproject-rtos/zephyr-build:main
$ cd dongle-bridge/

$ west build -b qemu_x86 app --pristine
$ west build -b nrf52840dongle app --pristine

$ west build -t run

$ west twister -T app -v --integration
```
