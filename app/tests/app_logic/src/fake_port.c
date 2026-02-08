#include <string.h>
#include "port.h"
#include "msg.h"

/* Test-visible state */
enum port_id fake_last_port;
size_t fake_last_len;
uint8_t fake_last_data[MSG_MAX_LEN];
int fake_send_count;

void fake_port_reset(void)
{
    fake_last_port = -1;
    fake_last_len = 0;
    fake_send_count = 0;
}

/* ---- port interface implementation ---- */

void port_send(enum port_id port,
               const uint8_t *data,
               size_t len)
{
    fake_last_port = port;
    fake_last_len = len;
    fake_send_count++;

    if (len > 0 && data != NULL) {
        memcpy(fake_last_data, data, len);
    }
}
