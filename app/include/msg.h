#pragma once
#include <stddef.h>
#include <stdint.h>

#define MSG_MAX_LEN 238

struct msg {
    size_t len;
    uint8_t data[MSG_MAX_LEN];
};

