# grafix

A bare metal C++ graphics library for driving 0.96" SSD1306 OLED displays (128x64) using Arduino. This library was implemented from scratch based on the SSD1306 datasheet to provide a minimal driver without relying on general purpose graphics frameworks like Adafruit GFX.

## What does it implement ?

*   SSD1306 initialization sequence and page addressing mode.
*   Buffer Management of the 1KB framebuffer (128x64 bits) for rendering.
*   Pixel plotting and line drawing using Bresenham's algorithm.
*   Relies only on the standard Arduino `Wire` library for I2C communication.

## Hardware Requirements

*   **Microcontroller:** Arduino Uno, Mega, or compatible AVR boards.
*   **Display:** 0.96" OLED Display (Driver: SSD1306, Resolution: 128x64).
*   **Interface:** I2C (Address: 0x3C).

### Pinout (Arduino Uno)
*   **VCC:** 5V or 3.3V
*   **GND:** GND
*   **SDA:** A4
*   **SCL:** A5

## Installation & Setup (PlatformIO)

Library is structured to be used within a PlatformIO environment.

1.  Clone or download this repository.
2.  Add the library to your project's `platformio.ini`. If you are developing locally, you can reference the library path directly:

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
; Adjust the path relative to your project location
lib_deps = file:///path/to/Grafix
```

## Basic Usage

Include the header and instantiate the `Grafix` class. Note that `display()` must be called to send the buffer to the screen.

```cpp
#include "grafix.h"

// Initialize I2C and OLED
display.begin();

// Clear existing buffer
display.clear();

// Draw operations
display.drawPixel(10, 10, 1);
display.drawLine(0, 0, 127, 63, 1);

// Push buffer to display
display.display();
```

## Running the Examples

There are examples demonstrating the library's capabilities.

### 3D Cube Wireframe
This example renders a rotating 3D cube using a software based projection pipeline (rotation matrices and perspective projection) mapped to the 2D display.

1.  Navigate to the example directory:
    ```sh
    cd examples/threedcube
    ```
2.  The `platformio.ini` in this folder is pre configured to link to the library root.
3.  Compile and upload:
    ```sh
    pio run -t upload
    ```

## Implementation Status and ToDo's

**Implemented:**
*   I2C Communication & Initialization
*   Framebuffer management (`clear`, `display`)
*   `drawPixel`
*   `drawLine` (Bresenham's Algorithm)

**Planned / In-Progress:**
*   `drawRect` / `fillRect`
*   Text Rendering Engine (`setCursor`, `print`)
*   Brightness Control (`setBrightness`)
*   Pomodoro Timer Example