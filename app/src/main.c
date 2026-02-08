/* src/main.c */

#include <zephyr/kernel.h>

#include "usb_if.h"
#include "ble_if.h"

/* Optional: logging */
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    int ret;

    LOG_INF("USB ↔ BLE bridge starting");

    /* Initialize USB transport */
    ret = usb_if_init();
    if (ret) {
        LOG_ERR("USB init failed: %d", ret);
        return ret;
    }

    /* Initialize BLE transport */
    ret = ble_if_init();
    if (ret) {
        LOG_ERR("BLE init failed: %d", ret);
        return ret;
    }

    LOG_INF("Transports initialized");

    /* Nothing else to do:
     * - USB RX handled by usb_event
     * - BLE RX handled by ble_event
     * - app_logic reacts to events
     */
    return 0;
}
