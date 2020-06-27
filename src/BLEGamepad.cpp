#include "BLEGamepad.h"

uint8_t emptyInputReportData[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0};

BLEGamepad::BLEGamepad(BLE &ble) : HIDServiceBase(ble, 
                                    _hidReportDescriptor, 
                                    sizeof(_hidReportDescriptor), 
                                    emptyInputReportData, 
                                    NULL, 
                                    NULL,
                                    sizeof(inputReport)),
                                    HIDDevice(ble, ble::adv_data_appearance_t::GAMEPAD),
_buttons(0)
{

}

void BLEGamepad::setAxes(signed char x, signed char y, signed char z, signed char rZ, char rX, char rY, signed char hat)
{
    uint8_t m[9];
    m[0] = this->_buttons;
    m[1] = (this->_buttons >> 8);
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

void BLEGamepad::buttons(uint16_t b)
{
  if (b != this->_buttons)
  {
    this->_buttons = b;
    this->setAxes(0, 0, 0, 0, 0, 0, 0);
  }
}

void BLEGamepad::press(uint16_t b)
{
  this->buttons(this->_buttons | b);
}

void BLEGamepad::release(uint16_t b)
{
  this->buttons(this->_buttons & ~b);
}

bool BLEGamepad::isPressed(uint16_t b)
{
  if ((b & this->_buttons) > 0)
    return true;
  return false;
}