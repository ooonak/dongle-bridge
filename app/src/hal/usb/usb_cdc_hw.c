/**
 * Zephyr Dongle Bridge Application usb_cdc_hw.c
 * 
 * Copyright (c) 2026 Oonak Kanoo
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file usb_cdc_hw.c
 * @brief USB CDC ACM hardware implementation
 *
 * This module provides a concrete implementation of the USB interface using
 * Zephyr's CDC ACM (Communications Device Class - Abstract Control Model)
 * driver. It abstracts the Zephyr UART API to the application's generic
 * USB interface, enabling transparent switching between different USB
 * implementations.
 *
 * @ingroup usb_if
 */

#include "hal/usb/usb_if.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <errno.h>
#include <string.h>

/**
 * @brief CDC ACM private context structure
 *
 * Contains implementation-specific state for CDC ACM communication,
 * including the device reference, callbacks, and transmission state.
 */
struct usb_cdc_hw_ctx {
    /** @brief Zephyr UART device for CDC ACM communication */
    const struct device *uart_dev;

    /** @brief Registered application callbacks */
    const struct usb_if_cb *cb;

    /** @brief Transmission state flag (true if TX in progress) */
    bool tx_in_progress;

    /** @brief USB connection state */
    bool connected;
};

/* Forward declarations */
static int usb_cdc_hw_init(struct usb_if *iface,
                           const struct usb_if_cb *cb);
static int usb_cdc_hw_send(struct usb_if *iface,
                           const uint8_t *data,
                           size_t len);
static bool usb_cdc_hw_is_ready(struct usb_if *iface);

/** @brief Operations vtable for CDC ACM hardware */
static const struct usb_if_ops usb_cdc_hw_ops = {
    .init = usb_cdc_hw_init,
    .send = usb_cdc_hw_send,
    .is_ready = usb_cdc_hw_is_ready,
};

/** @brief Static CDC ACM interface instance */
static struct usb_if usb_cdc_hw_if = {
    .ops = &usb_cdc_hw_ops,
    .ctx = NULL,
};

/**
 * @brief UART interrupt callback for received data
 *
 * This callback is invoked by the Zephyr UART driver when data is available.
 * It polls the UART device and dispatches received data to the application
 * via the registered rx callback.
 *
 * @param[in] dev Zephyr UART device
 * @param[in] user_data Private context (pointer to usb_cdc_hw_ctx)
 */
static void usb_cdc_hw_uart_callback(const struct device *dev, void *user_data)
{
    struct usb_cdc_hw_ctx *ctx = (struct usb_cdc_hw_ctx *)user_data;
    uint8_t data[64];
    int len;

    if (ctx == NULL || ctx->cb == NULL) {
        return;
    }

    /* Poll data from UART - stub implementation */
    (void)dev;
    (void)data;
    (void)len;

    /* TODO: Implement UART polling and callback invocation */
    /*
     * while ((len = uart_fifo_read(dev, data, sizeof(data))) > 0) {
     *     if (ctx->cb->rx != NULL) {
     *         ctx->cb->rx(&usb_cdc_hw_if, data, len);
     *     }
     * }
     */
}

/**
 * @brief Initialize CDC ACM USB interface
 *
 * Initializes the USB CDC ACM hardware interface by:
 * - Obtaining the Zephyr CDC ACM UART device
 * - Registering the UART callback for received data
 * - Storing the application callbacks
 *
 * @param[in] iface USB interface instance
 * @param[in] cb    Application callbacks
 *
 * @return 0 on success, negative error code on failure
 * @retval 0        Initialization successful
 * @retval -EINVAL  Invalid parameters
 * @retval -ENODEV  CDC ACM device not found
 * @retval -EBUSY   Device already initialized
 *
 * @note The callback structure must remain valid for the lifetime of the interface.
 */
static int usb_cdc_hw_init(struct usb_if *iface,
                           const struct usb_if_cb *cb)
{
    struct usb_cdc_hw_ctx *ctx;
    const struct device *uart_dev;

    if (iface == NULL || cb == NULL) {
        return -EINVAL;
    }

    /* Allocate context if not already allocated */
    if (iface->ctx == NULL) {
        ctx = k_malloc(sizeof(struct usb_cdc_hw_ctx));
        if (ctx == NULL) {
            return -ENOMEM;
        }
        memset(ctx, 0, sizeof(*ctx));
        iface->ctx = ctx;
    } else {
        ctx = (struct usb_cdc_hw_ctx *)iface->ctx;
        if (ctx->uart_dev != NULL) {
            /* Already initialized */
            return -EBUSY;
        }
    }

    /* TODO: Get CDC ACM UART device from device tree
     *
     * Zephyr provides the CDC ACM device via DT or device_get_binding().
     * Common approaches:
     *
     * Option 1 (Device tree):
     *   uart_dev = DEVICE_DT_GET(DT_ALIAS(cdc_acm_uart));
     *
     * Option 2 (Device binding):
     *   uart_dev = device_get_binding("CDC_ACM_0");
     *
     * Option 3 (Zephyr new API):
     *   uart_dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
     */
    (void)uart_dev;

    /* Store context and callbacks */
    ctx->cb = cb;
    ctx->tx_in_progress = false;
    ctx->connected = false;

    /* TODO: Configure UART callbacks
     *
     * Register the interrupt callback with:
     *   uart_irq_callback_set(uart_dev, usb_cdc_hw_uart_callback, ctx);
     *   uart_irq_rx_enable(uart_dev);
     *
     * Optionally enable line control callbacks if needed:
     *   cdc_acm_dte_rate_callback_set(uart_dev, usb_cdc_hw_line_coding_cb);
     */

    /* TODO: Signal initial connection state if already connected */
    if (cb->connected != NULL) {
        /* Check if USB is already enumerated and configured */
        /* cb->connected(iface, true/false); */
    }

    return 0;
}

/**
 * @brief Submit data for transmission
 *
 * Submits a buffer of data to be transmitted to the host via USB.
 * The function operates asynchronously; transmission completion is
 * signaled via the tx_done callback.
 *
 * @param[in] iface USB interface instance
 * @param[in] data  Data buffer to transmit
 * @param[in] len   Buffer length in bytes
 *
 * @return 0 on success, negative error code on failure
 * @retval 0         Data accepted for transmission
 * @retval -EBUSY    Transmission already in progress
 * @retval -ENODEV   USB not ready or device not initialized
 * @retval -EINVAL   Invalid parameters (NULL pointers or zero length)
 * @retval -EMSGSIZE Data length exceeds maximum UART buffer
 *
 * @note Only one transmission can be in progress at a time.
 *       Subsequent calls will be rejected with -EBUSY until
 *       the tx_done callback is invoked.
 */
static int usb_cdc_hw_send(struct usb_if *iface,
                           const uint8_t *data,
                           size_t len)
{
    struct usb_cdc_hw_ctx *ctx;

    if (iface == NULL || data == NULL || len == 0) {
        return -EINVAL;
    }

    ctx = (struct usb_cdc_hw_ctx *)iface->ctx;
    if (ctx == NULL || ctx->uart_dev == NULL) {
        return -ENODEV;
    }

    /* Check if transmission is already in progress */
    if (ctx->tx_in_progress) {
        return -EBUSY;
    }

    /* Check USB readiness */
    if (!ctx->connected) {
        return -ENODEV;
    }

    /* TODO: Submit data to UART for transmission
     *
     * Mark transmission as in progress and submit data:
     *
     *   ctx->tx_in_progress = true;
     *   int ret = uart_tx(ctx->uart_dev, data, len, SYS_FOREVER_MS);
     *
     * If using poll-mode UART instead:
     *   ctx->tx_in_progress = true;
     *   for (size_t i = 0; i < len; i++) {
     *       uart_poll_out(ctx->uart_dev, data[i]);
     *   }
     *   ctx->tx_in_progress = false;
     *   if (ctx->cb->tx_done != NULL) {
     *       ctx->cb->tx_done(iface);
     *   }
     */
    (void)data;
    (void)len;

    return 0;
}

/**
 * @brief Check if the USB interface is ready for communication
 *
 * Returns true only when the USB interface is fully operational and
 * ready to transmit data. All of the following must be true:
 * - Interface is initialized
 * - USB device is connected and enumerated
 * - Device is configured by the host
 * - No transmission is currently in progress
 *
 * @param[in] iface USB interface instance
 *
 * @return true if ready to send data, false otherwise
 */
static bool usb_cdc_hw_is_ready(struct usb_if *iface)
{
    struct usb_cdc_hw_ctx *ctx;

    if (iface == NULL) {
        return false;
    }

    ctx = (struct usb_cdc_hw_ctx *)iface->ctx;
    if (ctx == NULL || ctx->uart_dev == NULL) {
        return false;
    }

    /* TODO: Check actual USB connection state
     *
     * The readiness check should verify:
     * 1. USB is physically connected
     * 2. USB enumeration is complete
     * 3. Device configuration has been established
     * 4. No transmission is in progress
     *
     * This could involve:
     *   - Checking uart_irq_is_pending() or similar
     *   - Querying device line control state
     *   - Checking DTR/DSR signals via uart_line_ctrl_get()
     *   - Verifying no async TX is in progress
     */

    return ctx->connected && !ctx->tx_in_progress;
}

/**
 * @brief Get the USB CDC ACM interface instance
 *
 * Provides access to the static USB interface instance configured
 * for CDC ACM hardware communication.
 *
 * @return Pointer to the USB interface instance
 */
struct usb_if *usb_cdc_hw_get_interface(void)
{
    return &usb_cdc_hw_if;
}
