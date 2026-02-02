/**
 * Zephyr Dongle Bridge Application usb_cdc_mock.c
 * 
 * Copyright (c) 2026 Oonak Kanoo
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file usb_mock.c
 * @brief USB interface mock implementation for testing
 *
 * This module provides a mock implementation of the USB interface suitable
 * for unit testing and native simulation builds (native_sim).
 * 
 * @ingroup usb_if
 */

#include "usb_if.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/**
 * @brief Mock USB interface statistics
 *
 * Tracks usage statistics for test analysis and verification.
 */
struct usb_mock_stats {
    /** @brief Total bytes received */
    uint32_t rx_bytes;

    /** @brief Total bytes transmitted */
    uint32_t tx_bytes;

    /** @brief Number of rx callbacks invoked */
    uint32_t rx_callbacks;

    /** @brief Number of tx_done callbacks invoked */
    uint32_t tx_callbacks;

    /** @brief Number of connection state changes */
    uint32_t connection_changes;
};

/**
 * @brief Mock USB private context structure
 *
 * Contains simulation state, buffers, and configuration for the mock
 * USB interface implementation.
 */
struct usb_mock_ctx {
    /** @brief Registered application callbacks */
    const struct usb_if_cb *cb;

    /** @brief Simulated connection state */
    bool connected;

    /** @brief Transmission state flag (true if TX in progress) */
    bool tx_in_progress;

    /** @brief Loopback mode enabled (RX receives TX data) */
    bool loopback_enabled;

    /** @brief TX buffer for loopback simulation */
    uint8_t tx_buffer[256];

    /** @brief TX buffer length for loopback */
    size_t tx_len;

    /** @brief Failure injection flag (next send will fail) */
    bool inject_tx_failure;

    /** @brief Connection failure injection (next connect will fail) */
    bool inject_connect_failure;

    /** @brief Statistics tracking */
    struct usb_mock_stats stats;
};

/* Forward declarations */
static int usb_mock_init(struct usb_if *iface,
                         const struct usb_if_cb *cb);
static int usb_mock_send(struct usb_if *iface,
                         const uint8_t *data,
                         size_t len);
static bool usb_mock_is_ready(struct usb_if *iface);

/** @brief Operations vtable for mock USB */
static const struct usb_if_ops usb_mock_ops = {
    .init = usb_mock_init,
    .send = usb_mock_send,
    .is_ready = usb_mock_is_ready,
};

/** @brief Static mock USB interface instance */
static struct usb_if usb_mock_if = {
    .ops = &usb_mock_ops,
    .ctx = NULL,
};

/**
 * @brief Initialize mock USB interface
 *
 * Initializes the mock USB interface for simulation. The interface
 * starts in disconnected state and must be explicitly connected for
 * testing.
 *
 * @param[in] iface USB interface instance
 * @param[in] cb    Application callbacks
 *
 * @return 0 on success, negative error code on failure
 * @retval 0        Initialization successful
 * @retval -EINVAL  Invalid parameters
 * @retval -ENOMEM  Memory allocation failed
 * @retval -EBUSY   Already initialized
 */
static int usb_mock_init(struct usb_if *iface,
                         const struct usb_if_cb *cb)
{
    struct usb_mock_ctx *ctx;

    if (iface == NULL || cb == NULL) {
        return -EINVAL;
    }

    /* Allocate context if not already allocated */
    if (iface->ctx == NULL) {
        ctx = malloc(sizeof(struct usb_mock_ctx));
        if (ctx == NULL) {
            return -ENOMEM;
        }
        memset(ctx, 0, sizeof(*ctx));
        iface->ctx = ctx;
    } else {
        ctx = (struct usb_mock_ctx *)iface->ctx;
        if (ctx->cb != NULL) {
            /* Already initialized */
            return -EBUSY;
        }
    }

    /* Initialize context with defaults */
    ctx->cb = cb;
    ctx->connected = false;
    ctx->tx_in_progress = false;
    ctx->loopback_enabled = false;
    ctx->tx_len = 0;
    ctx->inject_tx_failure = false;
    ctx->inject_connect_failure = false;
    memset(&ctx->stats, 0, sizeof(ctx->stats));

    return 0;
}

/**
 * @brief Submit data for transmission (mock)
 *
 * In mock mode, this function simulates data transmission. Behavior
 * depends on configuration:
 * - If loopback is enabled, data is immediately reflected back via rx callback
 * - Otherwise, data is buffered and tx_done is called when "transmission" completes
 * - Failure injection can simulate transmission errors
 *
 * @param[in] iface USB interface instance
 * @param[in] data  Data buffer to transmit
 * @param[in] len   Buffer length in bytes
 *
 * @return 0 on success, negative error code on failure
 * @retval 0         Data accepted for transmission
 * @retval -EBUSY    Transmission already in progress
 * @retval -ENODEV   USB not connected or not initialized
 * @retval -EINVAL   Invalid parameters (NULL or zero length)
 * @retval -EMSGSIZE Data length exceeds buffer capacity
 *
 * @note In loopback mode, rx callback is invoked synchronously during
 *       this call. In non-loopback mode, tx_done callback is invoked
 *       synchronously to simulate immediate completion.
 */
static int usb_mock_send(struct usb_if *iface,
                         const uint8_t *data,
                         size_t len)
{
    struct usb_mock_ctx *ctx;

    if (iface == NULL || data == NULL || len == 0) {
        return -EINVAL;
    }

    ctx = (struct usb_mock_ctx *)iface->ctx;
    if (ctx == NULL) {
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

    /* Check buffer capacity */
    if (len > sizeof(ctx->tx_buffer)) {
        return -EMSGSIZE;
    }

    /* Inject transmission failure if configured */
    if (ctx->inject_tx_failure) {
        ctx->inject_tx_failure = false;
        return -EIO;
    }

    ctx->tx_in_progress = true;
    ctx->tx_len = len;
    memcpy(ctx->tx_buffer, data, len);
    ctx->stats.tx_bytes += len;

    /* TODO: Simulate transmission behavior
     *
     * Options for mock behavior:
     *
     * 1. Synchronous mode (current):
     *    - Immediately complete transmission
     *    - Invoke callbacks synchronously
     *
     * 2. Deferred mode (for timing tests):
     *    - Schedule tx_done callback via work queue or timer
     *    - Allows testing async behavior
     *
     * 3. Loopback mode:
     *    if (ctx->loopback_enabled && ctx->cb->rx != NULL) {
     *        ctx->cb->rx(iface, data, len);
     *    }
     */

    /* Simulate transmission completion */
    if (ctx->loopback_enabled && ctx->cb != NULL && ctx->cb->rx != NULL) {
        ctx->cb->rx(iface, data, len);
        ctx->stats.rx_bytes += len;
        ctx->stats.rx_callbacks++;
    }

    ctx->tx_in_progress = false;
    if (ctx->cb != NULL && ctx->cb->tx_done != NULL) {
        ctx->cb->tx_done(iface);
        ctx->stats.tx_callbacks++;
    }

    return 0;
}

/**
 * @brief Check if the mock USB interface is ready
 *
 * Returns the simulated connection state. In mock mode, readiness
 * is purely based on the connection flag set by test code.
 *
 * @param[in] iface USB interface instance
 *
 * @return true if connected, false otherwise
 */
static bool usb_mock_is_ready(struct usb_if *iface)
{
    struct usb_mock_ctx *ctx;

    if (iface == NULL) {
        return false;
    }

    ctx = (struct usb_mock_ctx *)iface->ctx;
    if (ctx == NULL) {
        return false;
    }

    return ctx->connected && !ctx->tx_in_progress;
}

/**
 * @brief Get the mock USB interface instance
 *
 * Provides access to the static mock USB interface instance.
 *
 * @return Pointer to the mock USB interface
 */
struct usb_if *usb_mock_get_interface(void)
{
    return &usb_mock_if;
}

/**
 * @brief Set simulated USB connection state (test helper)
 *
 * Simulates USB connection or disconnection by updating the connection
 * state and invoking the connected callback if registered.
 *
 * @param[in] iface      USB interface instance
 * @param[in] connected  true to simulate connection, false for disconnection
 *
 * @return 0 on success, negative error code on failure
 * @retval 0        Connection state changed successfully
 * @retval -EINVAL  Invalid parameters
 * @retval -ENODEV  Interface not initialized
 *
 * @note This function is for testing only and would not be present
 *       in production hardware implementations.
 */
int usb_mock_set_connected(struct usb_if *iface, bool connected)
{
    struct usb_mock_ctx *ctx;

    if (iface == NULL) {
        return -EINVAL;
    }

    ctx = (struct usb_mock_ctx *)iface->ctx;
    if (ctx == NULL) {
        return -ENODEV;
    }

    if (ctx->connected != connected) {
        ctx->connected = connected;
        ctx->stats.connection_changes++;

        if (ctx->cb != NULL && ctx->cb->connected != NULL) {
            ctx->cb->connected(iface, connected);
        }
    }

    return 0;
}

/**
 * @brief Enable or disable loopback mode (test helper)
 *
 * When loopback is enabled, transmitted data is immediately reflected
 * back to the application via the rx callback, simulating echo behavior
 * useful for round-trip testing.
 *
 * @param[in] iface    USB interface instance
 * @param[in] enabled  true to enable loopback, false to disable
 *
 * @return 0 on success, negative error code on failure
 * @retval 0        Loopback mode changed successfully
 * @retval -EINVAL  Invalid parameters
 * @retval -ENODEV  Interface not initialized
 */
int usb_mock_set_loopback(struct usb_if *iface, bool enabled)
{
    struct usb_mock_ctx *ctx;

    if (iface == NULL) {
        return -EINVAL;
    }

    ctx = (struct usb_mock_ctx *)iface->ctx;
    if (ctx == NULL) {
        return -ENODEV;
    }

    ctx->loopback_enabled = enabled;
    return 0;
}

/**
 * @brief Inject transmission failure (test helper)
 *
 * Configures the mock interface to simulate a transmission failure on
 * the next send() call. Useful for testing error handling paths.
 *
 * @param[in] iface USB interface instance
 *
 * @return 0 on success, negative error code on failure
 * @retval 0        Failure injection configured
 * @retval -EINVAL  Invalid parameters
 * @retval -ENODEV  Interface not initialized
 */
int usb_mock_inject_tx_failure(struct usb_if *iface)
{
    struct usb_mock_ctx *ctx;

    if (iface == NULL) {
        return -EINVAL;
    }

    ctx = (struct usb_mock_ctx *)iface->ctx;
    if (ctx == NULL) {
        return -ENODEV;
    }

    ctx->inject_tx_failure = true;
    return 0;
}

/**
 * @brief Get interface statistics (test helper)
 *
 * Retrieves accumulated statistics for the mock interface, including
 * bytes transmitted/received and callback invocation counts.
 *
 * @param[in]  iface   USB interface instance
 * @param[out] stats   Pointer to statistics structure to fill
 *
 * @return 0 on success, negative error code on failure
 * @retval 0        Statistics retrieved successfully
 * @retval -EINVAL  Invalid parameters
 * @retval -ENODEV  Interface not initialized
 */
int usb_mock_get_stats(struct usb_if *iface, struct usb_mock_stats *stats)
{
    struct usb_mock_ctx *ctx;

    if (iface == NULL || stats == NULL) {
        return -EINVAL;
    }

    ctx = (struct usb_mock_ctx *)iface->ctx;
    if (ctx == NULL) {
        return -ENODEV;
    }

    memcpy(stats, &ctx->stats, sizeof(*stats));
    return 0;
}

/**
 * @brief Reset interface statistics (test helper)
 *
 * Clears all accumulated statistics counters. Useful for isolating
 * test phases or resetting between test cases.
 *
 * @param[in] iface USB interface instance
 *
 * @return 0 on success, negative error code on failure
 * @retval 0        Statistics reset successfully
 * @retval -EINVAL  Invalid parameters
 * @retval -ENODEV  Interface not initialized
 */
int usb_mock_reset_stats(struct usb_if *iface)
{
    struct usb_mock_ctx *ctx;

    if (iface == NULL) {
        return -EINVAL;
    }

    ctx = (struct usb_mock_ctx *)iface->ctx;
    if (ctx == NULL) {
        return -ENODEV;
    }

    memset(&ctx->stats, 0, sizeof(ctx->stats));
    return 0;
}

/**
 * @brief Get raw TX buffer contents (test helper)
 *
 * Retrieves the contents of the TX buffer for verification that
 * correct data was transmitted.
 *
 * @param[in]  iface     USB interface instance
 * @param[out] buffer    Pointer to buffer to receive TX data
 * @param[in]  buf_size  Size of output buffer
 * @param[out] data_len  Pointer to receive actual TX data length
 *
 * @return 0 on success, negative error code on failure
 * @retval 0         TX buffer retrieved successfully
 * @retval -EINVAL   Invalid parameters
 * @retval -ENODEV   Interface not initialized
 * @retval -ENODATA  No TX data available
 */
int usb_mock_get_tx_buffer(struct usb_if *iface,
                           uint8_t *buffer,
                           size_t buf_size,
                           size_t *data_len)
{
    struct usb_mock_ctx *ctx;

    if (iface == NULL || buffer == NULL || data_len == NULL) {
        return -EINVAL;
    }

    ctx = (struct usb_mock_ctx *)iface->ctx;
    if (ctx == NULL) {
        return -ENODEV;
    }

    if (ctx->tx_len == 0) {
        return -ENODATA;
    }

    if (buf_size < ctx->tx_len) {
        return -EOVERFLOW;
    }

    memcpy(buffer, ctx->tx_buffer, ctx->tx_len);
    *data_len = ctx->tx_len;
    return 0;
}
