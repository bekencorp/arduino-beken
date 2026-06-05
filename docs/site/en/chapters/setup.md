# 4. Environment Setup

This chapter explains how to set up the development environment for Beken Arduino, including Arduino IDE installation, platform preparation, and validation.

## Supported Operating Systems

The Beken Arduino development environment supports the following operating systems:

| Operating System | Version | Status |
|-----------------|---------|--------|
| Windows | 10/11 | ✅ Supported |
| Linux | Ubuntu 20.04+ LTS, Debian 11+ | ✅ Supported |
| macOS | 11.5+ | ✅ Supported |


## 1. Install Python Environment

<a href="../_static/images/python_icon.png" target="_blank"><img src="../_static/images/python_icon.png" alt="Python Logo" style="max-width: 200px; height: auto;"></a>

Python is required for building and compiling Beken Arduino sketches.

### Windows

1. Download Python from the [official website](https://www.python.org/downloads/windows/)
2. Run the installer, ensuring **"Add Python to PATH"** is checked
3. Verify installation by opening Command Prompt and running:
   ```shell
   python --version
   ```



### Linux

Python 3 is usually pre-installed. Verify with:
```bash
python3 --version
```

For Debian/Ubuntu based systems:
```bash
sudo apt update
sudo apt install python3 python3-pip
```

### macOS

We recommend installing Python via Homebrew as we'll need Homebrew to install zstd later:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install python3
```

Verify the installation with:
```bash
python3 --version
```

## 2. Install Arduino IDE

<a href="../_static/images/arduino_icon.png" target="_blank"><img src="../_static/images/arduino_icon.png" alt="Arduino IDE Logo" style="max-width: 150px; height: auto;"></a>

### Windows

1. Download Arduino IDE from the [official website](https://www.arduino.cc/en/software)

<a href="../_static/images/arduino_windows.png" target="_blank"><img src="../_static/images/arduino_windows.png" alt="Arduino IDE Windows Download" style="max-width: 100%; height: auto;"></a>

2. Run the installer and follow the wizard
3. Launch Arduino IDE after installation

<a href="../_static/images/en_arduino_ide_windows.png" target="_blank"><img src="../_static/images/en_arduino_ide_windows.png" alt="Arduino IDE Windows Installer" style="max-width: 100%; height: auto;"></a>



### Linux

1. Download the AppImage from the [official website](https://www.arduino.cc/en/software)

<a href="../_static/images/arduino_ide_linux.png" target="_blank"><img src="../_static/images/arduino_ide_linux.png" alt="Arduino IDE Linux AppImage" style="max-width: 100%; height: auto;"></a>

2. Make the AppImage executable:
```bash
chmod +x arduino-ide_*.AppImage
```
3. Run the AppImage directly or integrate it into your system
   - If the AppImage reports missing dependencies, install them as prompted

### macOS

**Important: Manual Installation of bk_uploader and zstd Required**

Before using the Arduino IDE with Beken boards on macOS, you need to manually install bk_uploader first:

- For Intel-based Macs (x86_64): [Download bk_uploader for Intel Mac](https://dl.bekencorp.com/tools/bkfil/v4/cli/macos/bk_loader_macos_x86_64_4.1.2.260525141.pkg)
- For Apple Silicon Macs (arm64): [Download bk_uploader for Apple Silicon](https://dl.bekencorp.com/tools/bkfil/v4/cli/macos/bk_loader_macos_arm64_4.1.2.260525141.pkg)

<a href="../_static/images/install_guide.png" target="_blank"><img src="../_static/images/install_guide.png" alt="Install Guide" style="max-width: 100%; height: auto;"></a>

During the installation process, select the default configuration options.

After installing bk_uploader, the toolchain requires zstd to be installed via Homebrew:

```bash
brew install zstd
```

Then:

1. Download Arduino IDE from the [official website](https://www.arduino.cc/en/software)
2. Extract the downloaded file and move Arduino IDE to Applications folder
3. Launch Arduino IDE

## 3. Install Beken Board Support Package (BSP)

### Using Boards Manager (Recommended)

1. **Open Preferences**: 
   - Go to **File > Preferences**
   
     <a href="../_static/images/file-preference.png" target="_blank"><img src="../_static/images/file-preference.png" alt="Open Preferences" style="max-width: 100%; height: auto;"></a>

2. **Add Beken Board URL**:
   
   <a href="../_static/images/url_github.png" target="_blank"><img src="../_static/images/url_github.png" alt="Add Beken Board URL" style="max-width: 100%; height: auto;"></a>
   - In the **Preferences** window, find the **Additional Boards Manager URLs** field
   - Click the **+** button to add a new URL
   - Paste the following URL:
     ```
     https://github.com/bekencorp/arduino-beken/releases/download/V1.0.0/package_beken_bk7239n_index.json
     ```
   - Click **OK** to save

3. **Install Beken Board Package**:
   - Go to **Tools > Board > Boards Manager**
   - Search for "Beken" in the search bar
   - Click **Install** to install the "BK7239N" package
   
     <a href="../_static/images/board_import.png" target="_blank"><img src="../_static/images/board_import.png" alt="Install Beken Board Package" style="max-width: 100%; height: auto;"></a>

4. **Select Your Board**:
   - After installation, go to **Tools > Board > Beken**
   - Select BK7239N from the list

## 4. Verify Installation

1. Connect your Beken board to your computer via USB
2. Create a new sketch with the following code:
   ```cpp
   // the setup function runs once when you press reset or power the board
   void setup() {
     // initialize digital pin LED_BUILTIN as an output.
     pinMode(LED_BUILTIN, OUTPUT);
   }

   // the loop function runs over and over again forever
   void loop() {
     digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(1000);                       // wait for a second
     digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
     delay(1000);                       // wait for a second
   }
   ```
3. Select your board from **Tools > Board > Beken**
4. Select the correct port from **Tools > Port**
5. Click the upload button (▶️)
6. You should see the built-in LED blinking on your board

## Examples

The example set is the fastest way to understand what the current Arduino-facing layer can actually do on hardware.

### Full Example Inventory

- `ADCReadSerial`
- `BLEAdvertise`
- `BLEWriteTest`
- `Blink`
- `GPIOInterrupt`
- `HardwareTimer`
- `MultiInoBlink`
- `MultiSourceBlink`
- `PWMFade`
- `SPILoopback`
- `SerialEcho`
- `TickerBlink`
- `WiFiConnect`
- `WiFiScan`
- `WireMaster`
- `WireScanner`
- `WireSlave`

### Read the Examples As Validation Layers

The examples are not just demos. They are also a layered validation set:

- bring-up examples: `Blink`, `SerialEcho`
- connectivity examples: `WiFiScan`, `WiFiConnect`, `BLEAdvertise`
- bus examples: `WireScanner`, `SPILoopback`
- peripheral examples: `ADCReadSerial`, `PWMFade`, `HardwareTimer`, `DACSineWave`
- build-shape examples: `MultiInoBlink`, `MultiSourceBlink`

### Suggested Validation Order

For a fresh platform validation run:

1. `Blink`
2. `SerialEcho`
3. `WiFiScan`
4. `WireScanner`

Then move to peripheral-specific examples such as `ADCReadSerial`, `PWMFade`, or `HardwareTimer`.

### Example-Specific Caveats

- Most examples print status at `115200` baud.
- `SPILoopback` expects `MOSI` to be connected to `MISO`.
- `ADCReadSerial` depends on the variant analog pin mapping.