/**
 * Zephyr Dongle Bridge Application app_core.h
 * 
 * Copyright (c) 2026 Oonak Kanoo
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file core.h
 * @brief Application core module for data routing between transports
 *
 * @defgroup app_core Application Core
 * @{
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

/** @brief Opaque application core handle */
struct app_core;

/**
 * @brief Generic transport link interface
 *
 * Represents a communication transport (USB, BLE, etc.) with a send function.
 */
struct app_core_link {
    /**
     * @brief Send function for this transport
     *
     * @param[in] ctx  Transport-specific context
     * @param[in] data Data to transmit
     * @param[in] len  Data length
     *
     * @return 0 on success, negative error code on failure
     */
    int (*send)(void *ctx, const uint8_t *data, size_t len);

    /** @brief Transport context pointer */
    void *ctx;
};

/**
 * @brief Create application core instance
 *
 * @param[in] uplink   Primary transport link (e.g., USB)
 * @param[in] downlink Secondary transport link (e.g., BLE)
 *
 * @return Pointer to core instance, or NULL on failure
 */
struct app_core *app_core_create(const struct app_core_link *uplink,
                                 const struct app_core_link *downlink);

/**
 * @brief Process data received from USB
 *
 * @param[in] p    Core instance
 * @param[in] data Received data
 * @param[in] len  Data length
 */
void app_core_usb_rx(struct app_core *p,
                     const uint8_t *data,
                     size_t len);

/**
 * @brief Process data received from BLE
 *
 * @param[in] p    Core instance
 * @param[in] data Received data
 * @param[in] len  Data length
 */
void app_core_ble_rx(struct app_core *p,
                     const uint8_t *data,
                     size_t len);

/**
 * @brief Periodic core processing tick
 *
 * @param[in] p Core instance
 */
void app_core_tick(struct app_core *p);

/**
 * @}
 */
