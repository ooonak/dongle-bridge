#pragma once
#include <stddef.h>
#include <stdint.h>

enum port_id {
    PORT_USB,
    PORT_BLE,
    // future ports
};

void port_send(enum port_id port, const uint8_t *data, size_t len);

void port_register_rx_callback(enum port_id port,
                               void (*cb)(const uint8_t *data, size_t len));

