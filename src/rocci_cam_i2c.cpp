#include "rocci_cam_i2c.h"

#include <Arduino.h>
#include <Wire.h>
#include <string.h>

static uint8_t response_data[ROCCI_CAM_I2C_MAX_PAYLOAD];
static size_t response_length = 0;
static rocci_cam_i2c_receive_callback_t receive_callback = nullptr;
static portMUX_TYPE response_mux = portMUX_INITIALIZER_UNLOCKED;

static void handle_i2c_request()
{
    uint8_t response_snapshot[ROCCI_CAM_I2C_MAX_PAYLOAD];
    size_t snapshot_length;

    portENTER_CRITICAL_SAFE(&response_mux);
    snapshot_length = response_length;
    memcpy(response_snapshot, response_data, snapshot_length);
    portEXIT_CRITICAL_SAFE(&response_mux);

    if (snapshot_length > 0) {
        Wire.write(response_snapshot, snapshot_length);
    }
}

static void handle_i2c_receive(int incoming_length)
{
    uint8_t received_data[ROCCI_CAM_I2C_MAX_PAYLOAD];
    size_t received_length = 0;

    while (Wire.available() > 0) {
        int value = Wire.read();
        if (value >= 0 && received_length < sizeof(received_data)) {
            received_data[received_length++] = static_cast<uint8_t>(value);
        }
    }

    rocci_cam_i2c_receive_callback_t callback = receive_callback;
    if (callback != nullptr && received_length > 0) {
        callback(received_data, received_length);
    }

    (void)incoming_length;
}

bool rocci_cam_i2c_slave_begin(uint8_t address, int sda_pin, int scl_pin, uint32_t frequency)
{
    if (Wire.setBufferSize(ROCCI_CAM_I2C_MAX_PAYLOAD) == 0) {
        return false;
    }
    if (!Wire.begin(address, sda_pin, scl_pin, frequency)) {
        return false;
    }
    Wire.onReceive(handle_i2c_receive);
    Wire.onRequest(handle_i2c_request);
    return true;
}

void rocci_cam_i2c_slave_end(void)
{
    Wire.end();
}

bool rocci_cam_i2c_set_response(const uint8_t *data, size_t length)
{
    if (length > ROCCI_CAM_I2C_MAX_PAYLOAD || (length > 0 && data == nullptr)) {
        return false;
    }

    portENTER_CRITICAL_SAFE(&response_mux);
    if (length > 0) {
        memcpy(response_data, data, length);
    }
    response_length = length;
    portEXIT_CRITICAL_SAFE(&response_mux);
    return true;
}

void rocci_cam_i2c_on_receive(rocci_cam_i2c_receive_callback_t callback)
{
    receive_callback = callback;
}