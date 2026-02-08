#pragma once
#include "msg.h"
#include "port.h"
#include "transaction.h"

void app_on_rx(enum port_id from, const uint8_t *data, size_t len);

void app_on_response(enum port_id from, const uint8_t *data, size_t len);
