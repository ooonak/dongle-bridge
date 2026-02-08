#pragma once
#include <stddef.h>
#include <stdbool.h>
#include "msg.h"

struct txn {
    bool busy;
    struct msg message;
};

