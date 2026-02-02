/**
 * Zephyr Dongle Bridge Application app_core.c
 * 
 * Copyright (c) 2026 Oonak Kanoo
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file app_core.c
 * @brief Application core implementation
 *
 * @ingroup app_core
 */

#include "core/app_core.h"
#include <stdlib.h>

/**
 * @brief Application core instance
 *
 * Contains the core state and transport links.
 */
struct app_core {
    /** @brief Primary transport link (uplink) */
    const struct app_core_link *uplink;

    /** @brief Secondary transport link (downlink) */
    const struct app_core_link *downlink;
};

struct app_core *app_core_create(const struct app_core_link *uplink,
                                 const struct app_core_link *downlink)
{
    /* TODO: Implement core creation and initialization */
    (void)uplink;
    (void)downlink;
    return NULL;
}

void app_core_usb_rx(struct app_core *p,
                     const uint8_t *data,
                     size_t len)
{
    /* TODO: Implement USB RX processing */
    (void)p;
    (void)data;
    (void)len;
}

void app_core_ble_rx(struct app_core *p,
                     const uint8_t *data,
                     size_t len)
{
    /* TODO: Implement BLE RX processing */
    (void)p;
    (void)data;
    (void)len;
}

void app_core_tick(struct app_core *p)
{
    /* TODO: Implement periodic processing */
    (void)p;
}
