#include "app_logic.h"
#include <string.h>

//static struct txn usb_to_ble;
//static struct txn ble_to_usb;

void app_on_rx(enum port_id from, const uint8_t *data, size_t len)
{
    // Process incoming data and prepare a response if necessary
    // For example, you can parse the data and determine the appropriate response
    // Then call app_on_response to send the response back to the sender
    // Example:
    // uint8_t response_data[] = { /* your response data */ };
    // app_on_response(from, response_data, sizeof(response_data));
}

void app_on_response(enum port_id from, const uint8_t *data, size_t len)
{
    // Send the response data back to the sender
    // This function can be implemented to use the appropriate communication protocol
    // For example, you can use a function like send_data(from, data, len) to send the response
}
