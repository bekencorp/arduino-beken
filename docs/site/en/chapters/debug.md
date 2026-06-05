# 8. Debug Guide

## SWD System Debugging

> **⚠️ WARNING: Feature Under Development**<br>
> SWD debugging functionality is still under active development and currently unavailable for general use.

### Wiring Instructions

SWD (Serial Wire Debug) is a debug interface used for ARM microcontrollers. Below are the SWD wiring instructions for BK7239N development board:

| Pin | Signal Name | Description |
|------|----------|------|
| SWDIO | SWD Data Input/Output | Serial data line |
| SWCLK | SWD Clock | Serial clock line |
| GND | Ground | Ground |
| 3.3V | Power | Power (connect to debugger VTref) |

### Typical Wiring Configuration

**M239N DevKit Development Board:**

| Development Board Pin | Debugger Pin |
|------------|------------|
| SWDIO (GPIO1) | SWDIO |
| SWCLK (GPIO0) | SWCLK |
| 3.3V | VTref |
| GND | GND |


> **Note:**
> - Debugger must use 3.3V level
> - Ensure common ground between development board and debugger
> - Use high-quality connecting cables, keep length short

### Starting Debug Session in Arduino IDE

<a href="../_static/images/ide_debug.png" target="_blank"><img src="../_static/images/ide_debug.png" alt="Arduino IDE Debug Button" style="max-width: 600px; height: auto;"></a>

After completing the wiring, follow these steps to start debugging in Arduino IDE:

1. Connect the debugger to your computer
2. Open your sketch in Arduino IDE
3. Click the **Debug** button (▶️ with bug icon) in the toolbar
4. The IDE will compile the sketch and connect to the target device
5. Once connected, you can set breakpoints, inspect variables, and step through code

> **Note:** Ensure the debugger is properly recognized by your system before starting the debug session.

## Serial Debugging

### Serial Debugging Output

```cpp
Serial.begin(115200);
Serial.print("Value: ");
Serial.println(sensorValue);
Serial.printf("Temperature: %.2f C\n", temperature);
```



## Troubleshooting for Common Compilation Issues

| Issue | Solution |
|-------|----------|
| Missing board package | Install Beken BSP via Boards Manager |
| Library conflicts | Remove duplicate libraries |
| Memory errors | Reduce sketch size |
| Compilation errors | Check syntax and include paths |
| Python not found (Windows) | Install Python and add to PATH |

### Windows: Python Not Found Error

If you encounter a Python-related error during compilation on Windows:

<a href="../_static/images/python_compile_error.png" target="_blank"><img src="../_static/images/python_compile_error.png" alt="Python Compile Error on Windows" style="max-width: 100%; height: auto;"></a>

**Solution:**
1. Download Python from the [official website](https://www.python.org/downloads/windows/)
2. Run the installer, ensuring **"Add Python to PATH"** is checked
3. Restart Arduino IDE and try compiling again

### macOS: zstd Library Not Found Error

If you encounter a zstd library error during compilation on macOS:

```
dyld: Library not loaded: /usr/local/opt/zstd/lib/libzstd.1.dylib 
   Referenced from: /Users/beken/Library/Arduino15/packages/beken/tools/arm-none-eabi-gcc/13.3.1/bin/../libexec/gcc/arm-none-eabi/13.3.1/cc1plus 
   Reason: image not found 
```

**Solution:**
Install zstd using Homebrew as described in the **Environment Setup** chapter:

```bash
brew install zstd
```

## Troubleshooting for Download Issues

| Issue | Solution |
|-------|----------|
| Port not found | Install drivers, check USB connection |
| Upload failed | Reset device during upload, check boot mode |
| Connection timeout | Verify baud rate and serial settings |

## Hardware Adaptation Fault Troubleshooting

| Symptom | Possible Cause | Fix |
|---------|---------------|-----|
| No power | Wrong voltage, bad cable | Check power supply |
| No serial output | Wrong baud rate, wrong port | Verify settings |
| Peripheral not working | Wrong pin assignment | Check pinout |
| Wi-Fi not connecting | Wrong credentials, signal issues | Verify SSID/password |