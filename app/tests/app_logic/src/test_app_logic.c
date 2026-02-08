#include <zephyr/ztest.h>
#include "app_logic.h"
#include "port.h"
#include "usb_if.h"
#include "ble_if.h"

/* From fake_port.c */
extern enum port_id fake_last_port;
extern size_t fake_last_len;
extern uint8_t fake_last_data[];
extern int fake_send_count;
void fake_port_reset(void);

/* ---------------- Test helpers ---------------- */

static const uint8_t REQ_USB[] = { 0x01, 0x02 };
static const uint8_t RESP_USB[] = { 0x10 };

static const uint8_t REQ_BLE[] = { 0x03 };
static const uint8_t RESP_BLE[] = { 0x20, 0x21 };

/* ---------------- Tests ---------------- */

ZTEST(app_logic, test_usb_initiates_transaction)
{
    fake_port_reset();

    /* USB sends request */
    app_on_rx(PORT_USB, REQ_USB, sizeof(REQ_USB));

    zassert_equal(fake_last_port, PORT_BLE);
    zassert_equal(fake_last_len, sizeof(REQ_USB));

    /* BLE responds */
    app_on_response(PORT_BLE, RESP_BLE, sizeof(RESP_BLE));

    zassert_equal(fake_last_port, PORT_USB);
    zassert_equal(fake_last_len, sizeof(RESP_BLE));
}

ZTEST(app_logic, test_ble_initiates_transaction)
{
    fake_port_reset();

    /* BLE sends request */
    app_on_rx(PORT_BLE, REQ_BLE, sizeof(REQ_BLE));

    zassert_equal(fake_last_port, PORT_USB);
    zassert_equal(fake_last_len, sizeof(REQ_BLE));

    /* USB responds */
    app_on_response(PORT_USB, RESP_USB, sizeof(RESP_USB));

    zassert_equal(fake_last_port, PORT_BLE);
    zassert_equal(fake_last_len, sizeof(RESP_USB));
}

ZTEST(app_logic, test_independent_directions_allowed)
{
    fake_port_reset();

    /* USB initiates USB→BLE */
    app_on_rx(PORT_USB, REQ_USB, sizeof(REQ_USB));
    zassert_equal(fake_last_port, PORT_BLE);

    /* BLE initiates BLE→USB simultaneously */
    app_on_rx(PORT_BLE, REQ_BLE, sizeof(REQ_BLE));
    zassert_equal(fake_last_port, PORT_USB);

    /* BLE responds to USB request */
    app_on_response(PORT_BLE, RESP_BLE, sizeof(RESP_BLE));
    zassert_equal(fake_last_port, PORT_USB);

    /* USB responds to BLE request */
    app_on_response(PORT_USB, RESP_USB, sizeof(RESP_USB));
    zassert_equal(fake_last_port, PORT_BLE);
}

ZTEST(app_logic, test_busy_rejected_per_direction)
{
    fake_port_reset();

    /* USB initiates */
    app_on_rx(PORT_USB, REQ_USB, sizeof(REQ_USB));
    zassert_equal(fake_last_port, PORT_BLE);

    /* USB tries again while busy */
    app_on_rx(PORT_USB, REQ_USB, sizeof(REQ_USB));

    /* Should reply BUSY to USB */
    zassert_equal(fake_last_port, PORT_USB);
    zassert_true(fake_last_len > 0);
}

/* ---------------- Suite ---------------- */

ZTEST_SUITE(app_logic, NULL, NULL, NULL, NULL, NULL);
