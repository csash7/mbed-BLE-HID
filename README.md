# mbed-BLE-HID
### BLE HID library for Arduino boards with BLE support and mbed OS.
This library turns Arduino Board into a BLE Mouse which can be connected to devices such as Windows PC, android phones etc.
This library is mostly written using Mbed OS's BLE API and also supports pairing with the Central device.

## Usage
This library is similar to the Arduino's Mouse, keyboard library and supports all the functions which are in those libraries.
It also has additional features such as setDeviceName, setManufacturerName and setBatteryLevel.
```
Some items are still pending TODO
 
 ## Compatible Hardware
 - Arduino Nano BLE
 - Arduino Nano BLE Sense
 - Any board with Mbed OS and BLE support.
 
 ## Installation
### Using the Arduino IDE Library Manager

1. Choose `Sketch` -> `Include Library` -> `Add .ZIP Library...`
2. Select the library downloaded from GitHub as ZIP.

### Using Git

```sh
cd ~/Documents/Arduino/libraries/
git clone https://github.com/csash7/mbed-BLE-HID
```

## Examples

See [examples](examples) folder.

## License

This library is [licensed](LICENSE-2.0.txt) under the [APACHE Licence](http://www.apache.org/licenses/LICENSE-2.0).
