/**
 * Zephyr Dongle Bridge Application usb_if.h
 * 
 * Copyright (c) 2026 Oonak Kanoo
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file usb_if.h
 * @brief USB interface abstraction layer for host communication
 *
 * This module provides a hardware-abstraction-layer (HAL) interface for USB
 * communication with the host. It defines a callback-based architecture that
 * allows the application layer to send and receive data over USB independently
 * of the underlying USB controller implementation.
 *
 * @defgroup usb_if USB Interface HAL
 * @{
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/** @brief Opaque USB interface handle */
struct usb_if;

/**
 * @brief USB interface callback structure
 *
 * Defines the callback interface for USB events and data transfer notifications.
 * Callbacks are used to signal the application of incoming data, transmission
 * completion, and connection state changes.
 */
struct usb_if_cb {
    /**
     * @brief Callback for received data from the host
     *
     * Invoked when USB data is received from the host. This callback may be
     * called multiple times with arbitrary chunk sizes. The callback must copy
     * data if retention is required, as the buffer is only valid during the
     * callback invocation.
     *
     * @note This callback may be invoked from interrupt service routine (ISR)
     *       or cooperative context. Implementations must be re-entrant and
     *       non-blocking.
     *
     * @param[in] iface  Pointer to the USB interface instance
     * @param[in] data   Pointer to the received data buffer
     * @param[in] len    Length of the received data in bytes
     */
    void (*rx)(struct usb_if *iface,
               const uint8_t *data,
               size_t len);

    /**
     * @brief Callback for TX transmission completion
     *
     * Invoked when a previously submitted transmit buffer has been sent and
     * has left the USB stack. This signals that the transmit buffer is no
     * longer in use and a new transmit request may be submitted.
     *
     * @note This callback does not guarantee that the host has consumed the
     *       data; it only confirms that the USB controller has transmitted it.
     *
     * @param[in] iface  Pointer to the USB interface instance
     */
    void (*tx_done)(struct usb_if *iface);

    /**
     * @brief Optional callback for connection state changes
     *
     * Invoked when the USB connection state changes. This includes events such
     * as USB enumeration completion, device configuration, disconnection, or
     * suspend/resume events.
     *
     * @note This callback may be NULL if connection state monitoring is not required.
     *
     * @param[in] iface      Pointer to the USB interface instance
     * @param[in] connected  true if USB is connected and configured, false otherwise
     */
    void (*connected)(struct usb_if *iface, bool connected);
};

/**
 * @brief USB interface operations (vtable)
 *
 * Defines the function pointers for USB interface operations. Implementations
 * must provide these operations to manage USB communication.
 */
struct usb_if_ops {
    /**
     * @brief Initialize the USB interface
     *
     * Initializes the USB interface and registers callbacks. This must be called
     * before any other operations on the interface.
     *
     * @param[in] iface  Pointer to the USB interface instance
     * @param[in] cb     Pointer to the callback structure (must remain valid)
     *
     * @return 0 on success, negative error code on failure
     * @retval -EINVAL   Invalid parameters
     * @retval -ENODEV   USB device not available
     */
    int (*init)(struct usb_if *iface,
                const struct usb_if_cb *cb);

    /**
     * @brief Submit data for transmission to the host
     *
     * Submits a data buffer for transmission to the host. This operation is
     * non-blocking; the actual transmission occurs asynchronously. Completion
     * is signaled via the tx_done() callback.
     *
     * Only one transmission may be in progress at a time. Subsequent calls to
     * send() will be rejected with -EBUSY until the previous transmission is
     * complete.
     *
     * @param[in] iface  Pointer to the USB interface instance
     * @param[in] data   Pointer to the data buffer to transmit
     * @param[in] len    Length of the data to transmit in bytes
     *
     * @return 0 on success, negative error code on failure
     * @retval 0         Data accepted for transmission
     * @retval -EBUSY    Transmission already in progress; retry after tx_done() callback
     * @retval -ENODEV   USB not ready or disconnected
     * @retval -EINVAL   Invalid parameters or data pointer
     * @retval -EMSGSIZE Data length exceeds maximum endpoint buffer size
     * @retval Other     Implementation-specific error code
     */
    int (*send)(struct usb_if *iface,
                const uint8_t *data,
                size_t len);

    /**
     * @brief Check if the USB interface is ready for communication
     *
     * Determines whether the USB interface is ready to accept data transmission.
     * This returns true only when all of the following conditions are met:
     * - USB is physically connected
     * - USB enumeration is complete
     * - Device configuration has been established
     * - No transmission is currently in progress
     *
     * @param[in] iface  Pointer to the USB interface instance
     *
     * @return true if the interface is ready to send data, false otherwise
     */
    bool (*is_ready)(struct usb_if *iface);
};

/**
 * @brief USB interface instance
 *
 * Represents a USB interface instance, containing a pointer to the operations
 * vtable and an opaque context pointer for implementation-specific data.
 */
struct usb_if {
    /** @brief Pointer to operations function table */
    const struct usb_if_ops *ops;

    /** @brief Implementation-specific context data */
    void *ctx;
};

/**
 * @}
 */
