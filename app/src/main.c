#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app);

int main(void)
{
    LOG_INF("Hello, Zephyr");
    return 0;
}

