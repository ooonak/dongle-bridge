#pragma once
#include <stddef.h>
#include <stdint.h>
#include "msg.h"

struct usb_evt {
    void *fifo_reserved;
    size_t len;
    uint8_t data[MSG_MAX_LEN];
};

int usb_if_init(void);

int usb_if_send_async(const uint8_t *data, size_t len);
