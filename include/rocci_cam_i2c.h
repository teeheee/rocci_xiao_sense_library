#ifndef ROCCI_CAM_I2C_H
#define ROCCI_CAM_I2C_H

#include <stddef.h>
#include <stdint.h>

static const size_t ROCCI_CAM_I2C_MAX_PAYLOAD = 128;

typedef void (*rocci_cam_i2c_receive_callback_t)(const uint8_t *data, size_t length);

bool rocci_cam_i2c_slave_begin(
    uint8_t address = 0x55,
    int sda_pin = 5,
    int scl_pin = 6,
    uint32_t frequency = 100000
);
void rocci_cam_i2c_slave_end(void);
bool rocci_cam_i2c_set_response(const uint8_t *data, size_t length);
void rocci_cam_i2c_on_receive(rocci_cam_i2c_receive_callback_t callback);

#endif