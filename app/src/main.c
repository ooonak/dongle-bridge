#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "sum.h"

LOG_MODULE_REGISTER(app);

int main(void)
{
    LOG_INF("Hello, Zephyr");

    printf("2 + 3 = %d\n", add(2, 3));

    return 0;
}

