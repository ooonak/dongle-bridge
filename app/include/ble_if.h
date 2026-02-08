#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "msg.h"

/*
 * BLE transport interface
 *
 * - TX only
 * - RX handled by ble_event.c
 * - Asynchronous
 * - One message at a time (enforced by app_logic)
 */

int ble_if_init(void);

/*
 * Send a message over BLE.
 *
 * Returns:
 *  0          message accepted for transmission
 * -EBUSY      BLE stack busy
 * -EINVAL     invalid length (> MSG_MAX_LEN)
 * -ENOTCONN   not connected
 */
int ble_if_send_async(const uint8_t *data, size_t len);
