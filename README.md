# rocci_xiao_sense_library

PlatformIO Arduino library for camera capture, callbacks, serial image streaming, and drawing overlay frames on the Seeed XIAO ESP32S3 Sense.

## Install

Add the GitHub repository to a PlatformIO project's `platformio.ini`:

```ini
lib_deps =
    https://github.com/teeheee/rocci_xiao_sense_library.git#v0.1.0
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

`rocci_cam_process()` handles serial commands, captures a frame, calls the registered callback, and sends requested or streamed frames. The serial interface starts at 921600 baud. Supported text commands are `capture`, `start`, `stop`, `quiet`, and `loud`.

Image packets include frame width, height, pixel format, and payload length. Drawing helpers `draw_box`, `draw_point`, and `draw_line` emit overlay packets on the same serial connection.

The current pin configuration and default camera model target the XIAO ESP32S3 Sense.