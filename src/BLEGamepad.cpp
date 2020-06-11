#include "BLEGamepad.h"

uint8_t emptyInputReportData[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0};

report_map_t _hidReportDescriptor = {
  USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x05, // USAGE (Gamepad)
  COLLECTION(1),       0x01, // COLLECTION (Application)
  USAGE(1),            0x01, //   USAGE (Pointer)
  COLLECTION(1),       0x00, //   COLLECTION (Physical)
  REPORT_ID(1),        0x01, //     REPORT_ID (1)
  // ------------------------------------------------- Buttons (1 to 14)
  USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
  USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
  USAGE_MAXIMUM(1),    0x0e, //     USAGE_MAXIMUM (Button 14)
  LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  REPORT_COUNT(1),     0x0e, //     REPORT_COUNT (14)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;14 button bits
  // ------------------------------------------------- Padding
  REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
  HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;2 bit padding
  // ------------------------------------------------- X/Y position, Z/rZ position
  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x30, //     USAGE (X)
  USAGE(1),            0x31, //     USAGE (Y)
  USAGE(1),            0x32, //     USAGE (Z)
  USAGE(1),            0x35, //     USAGE (rZ)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x04, //     REPORT_COUNT (4)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;4 bytes (X,Y,Z,rZ)

  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x33, //     USAGE (rX) Left Trigger
  USAGE(1),            0x34, //     USAGE (rY) Right Trigger
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;2 bytes rX, rY

  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x39, //     USAGE (Hat switch)
  USAGE(1),            0x39, //     USAGE (Hat switch)
  LOGICAL_MINIMUM(1),  0x01, //     LOGICAL_MINIMUM (1)
  LOGICAL_MAXIMUM(1),  0x08, //     LOGICAL_MAXIMUM (8)
  REPORT_SIZE(1),      0x04, //     REPORT_SIZE (4)
  REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;1 byte Hat1, Hat2

  END_COLLECTION(0),         //     END_COLLECTION
  END_COLLECTION(0)          //     END_COLLECTION
};

BLEGamepad::BLEGamepad(BLE &_ble) : HIDServiceBase(_ble, 
                                    _hidReportDescriptor, 
                                    sizeof(_hidReportDescriptor), 
                                    emptyInputReportData, 
                                    NULL, 
                                    NULL,
                                    sizeof(inputReport)),
                                    HIDDevice(_ble, ble::adv_data_appearance_t::GAMEPAD),
_buttons(0)
{

}

void BLEGamepad::setAxes(signed char x, signed char y, signed char z, signed char rZ, char rX, char rY, signed char hat)
{
  if (this->isConnected())
  {
    unsigned char m[9];
    m[0] = _buttons;
    m[1] = (_buttons >> 8);
    m[2] = x;
    m[3] = y;
    m[4] = z;
    m[5] = rZ;
    m[6] = (signed char)(rX - 128);
    m[7] = (signed char)(rY - 128);
    m[8] = hat;
    if (m[6] == (uint8_t) -128) { m[6] = -127; }
    if (m[7] == (uint8_t) -128) { m[7] = -127; }
    this->sendMap(_hidReportDescriptor, sizeof(_hidReportDescriptor));
    this->send(m, sizeof(m));
  }
}

void BLEGamepad::buttons(uint16_t b)
{
  if (b != _buttons)
  {
    _buttons = b;
    setAxes(0, 0, 0, 0, 0, 0, 0);
  }
}

void BLEGamepad::press(uint16_t b)
{
  buttons(_buttons | b);
}

void BLEGamepad::release(uint16_t b)
{
  buttons(_buttons & ~b);
}

bool BLEGamepad::isPressed(uint16_t b)
{
  if ((b & _buttons) > 0)
    return true;
  return false;
}