#include <rocci_cam.h>
#include <rocci_cam_i2c.h>

static uint8_t average_brightness = 0;

static void publish_frame_brightness(camera_fb_t *frame)
{
    if (frame->format != PIXFORMAT_RGB565 || frame->len < 2) {
        return;
    }

    uint32_t total = 0;
    size_t pixel_count = frame->len / 2;
    for (size_t index = 0; index < pixel_count; ++index) {
        uint16_t pixel = (static_cast<uint16_t>(frame->buf[index * 2]) << 8) | frame->buf[index * 2 + 1];
        uint16_t red = ((pixel >> 11) & 0x1F) * 255 / 31;
        uint16_t green = ((pixel >> 5) & 0x3F) * 255 / 63;
        uint16_t blue = (pixel & 0x1F) * 255 / 31;
        total += (77 * red + 150 * green + 29 * blue) >> 8;
    }
    average_brightness = total / pixel_count;
    rocci_cam_i2c_set_response(&average_brightness, sizeof(average_brightness));
}

static void handle_master_write(const uint8_t *data, size_t length)
{
    if (length > 0 && length <= ROCCI_CAM_I2C_MAX_PAYLOAD) {
        rocci_cam_i2c_set_response(data, length);
    }
}

void setup()
{
    rocci_cam_init();
    rocci_cam_register_callback(publish_frame_brightness);
    rocci_cam_i2c_on_receive(handle_master_write);
    if (!rocci_cam_i2c_slave_begin()) {
        send_to_terminal("I2C slave initialization failed");
    }
}

void loop()
{
    rocci_cam_process();
}