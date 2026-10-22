# rocci_xiao_sense_library

PlatformIO Arduino library for camera capture, callbacks, serial image streaming, and drawing overlay frames on the Seeed XIAO ESP32S3 Sense.

## Install

Add the GitHub repository to a PlatformIO project's `platformio.ini`:

```ini
lib_deps =
    https://github.com/teeheee/rocci_xiao_sense_library.git#v0.3.0
```

The ESP32 Arduino framework provides the `esp_camera` API used by this library.

## Basic use

```cpp
#include <rocci_cam.h>

void on_camera_frame(camera_fb_t *frame)
{
    // Inspect or process the camera framebuffer here.
}

void setup()
{
    rocci_cam_init();
    rocci_cam_register_callback(on_camera_frame);
}

void loop()
{
    rocci_cam_process();
}
```

`rocci_cam_process()` handles serial commands, captures a frame, calls the registered callback, and sends requested or streamed frames. The serial interface starts at 921600 baud. Supported text commands are `capture`, `start`, `stop`, `halt`, `go`, `quiet`, and `loud`. `halt` stops camera acquisition while continuing to service serial commands; `go` resumes acquisition. `process` must be followed by exactly 96x96 RGB565 pixels in camera wire byte order (big-endian).

Image packets include frame width, height, pixel format, and payload length. Drawing helpers `draw_box`, `draw_point`, and `draw_line` emit overlay packets on the same serial connection.

The current pin configuration and default camera model target the XIAO ESP32S3 Sense.

## I2C slave

The XIAO ESP32S3 Sense can expose a separate I2C slave bus while the camera uses GPIO 40/39 for its sensor bus. The library defaults to slave address `0x55`, SDA GPIO 5 (XIAO D4), SCL GPIO 6 (XIAO D5), and 100 kHz. Set the response whenever the value to be read by the master changes; response payloads are limited to 128 bytes.

```cpp
#include <rocci_cam_i2c.h>

uint8_t measurement = 0;

void on_master_write(const uint8_t *data, size_t length)
{
    // Consume the received bytes during this callback.
}

void setup()
{
    rocci_cam_i2c_on_receive(on_master_write);
    if (!rocci_cam_i2c_slave_begin()) {
        // Handle I2C initialization failure.
    }
    rocci_cam_i2c_set_response(&measurement, sizeof(measurement));
}
```

The master reads the current response with a normal I2C read transaction and writes are delivered to the registered callback. The receive callback's data pointer is valid only until the callback returns. See `examples/i2c_slave_camera` for a camera-derived brightness response.