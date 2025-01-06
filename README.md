# Aurora Lamp Project

A fun and educational project that combines 3D printing, laser cutting, and microcontroller programming to create a customizable lamp with vibrant LED animations! This guide walks you through the steps to assemble, customize, and program your Aurora Lamp.

## Features

- **Customizable Base**: Edit the 3D-printed parts using Tinkercad.
- **Laser-Cut Acrylic**: Create unique designs for the lamp's cover.
- **ESP32 with Neopixels**: Program beautiful light effects with Arduino or WLED.

---

## Components

- **3D Printed Parts**:
  - `base.stl`: Houses the ESP32 microcontroller.
  - `Top.stl`: Features a slot for the LED strip and optional custom text or images.
  - `Clip-on brace.stl`: Secures the USB wire in place.
- **Laser-Cut Acrylic**: Use Inkscape to design and trace bitmap images, then integrate them into `aurora.svg`.
- **Electronics**:
  - ESP32 microcontroller.
  - 4 Neopixel LEDs (WS2812B or similar).
  - USB cable

---

## Project Instructions

### 1. Laser-Cutting the Acrylic
- Open `aurora.svg` in **Inkscape**.
- Replace the default image with a traced bitmap design of your choice.
- Export the modified SVG for laser cutting.

### 2. Customizing the 3D-Printed Parts (Optional)
- Open the provided `.stl` files in **Tinkercad**.
- For `Top.stl`, add text or images while ensuring no overhangs to avoid the need for supports during 3D printing.
- Export the updated files for printing.

### 3. Printing the Parts
- Print all 3D files using PLA or ABS material.
- Assemble the components:
  - Secure the ESP32 into the `base.stl`.
  - Attach the Neopixel LED strip to the slot in `Top.stl`.
  - Clip the USB cable with `Clip-on brace.stl`.

---

## Programming the ESP32

### Option 1: Using Arduino IDE

1. **Install Required Tools**:
   - Install the ESP32 core in Arduino IDE: [Installation Guide](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).
   - If drivers aren't automatically installed, install them manually: [Driver Installation](https://randomnerdtutorials.com/install-esp32-esp8266-usb-drivers-cp210x-windows/).

2. **Libraries**:
   - Install the following libraries in Arduino IDE:
     - Adafruit NeoPixel

3. **Upload Code**:
   - Open the provided `.ino` sketch file.
   - Customize animations and thresholds in the sketch.
   - Upload the code to your ESP32.

### Option 2: Using WLED

1. **Install WLED**:
   - Follow the browser-based installation guide: [WLED Installation](https://install.wled.me/).

2. **Configure WLED**:
   - Set the number of LEDs in **LED Preferences**.
   - Update the hotspot name and password in **WiFi Settings**.
   - Select a light effect and save it as the default.

---

## Usage

- Power up the ESP32 using a USB cable.
- Enjoy your Aurora Lamp with its vibrant LED effects!

---

## Notes

- The project encourages creativity. Feel free to experiment with designs, animations, and configurations.
- Ensure you have a stable USB connection to avoid interruptions during programming.
