#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "hal/usb/usb_if.h"
#include "core/app_core.h"

LOG_MODULE_REGISTER(app);

int main(void)
{
    LOG_INF("Hello from Dongle Bridge Application (HW)!");

    return 0;
}