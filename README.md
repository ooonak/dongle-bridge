[![Zephyr Build & Test v4.3](https://github.com/ooonak/dongle-bridge/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/ooonak/dongle-bridge/actions/workflows/build_and_test.yml)

# dongle-bridge
Zephyr application implementing an USB to BLE bridge

## Build, test, run

```
$ docker run -ti -v <path>/zephyrproject:/workdir ghcr.io/zephyrproject-rtos/zephyr-build:main
$ cd dongle-bridge

# Build for target
$ west build -p -b nrf52840dongle app

# Build and run test by hand
$ west build -b native_sim app/tests/app_logic
$ west build -t run

# Or with Twister
$ west twister -v -p native_sim -T app/tests
```
