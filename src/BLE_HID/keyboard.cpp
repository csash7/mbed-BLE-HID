#include "keyboard.h"

/// "keys pressed" report
static uint8_t inputReportData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
/// "keys released" report
static const uint8_t emptyInputReportData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
/// LEDs report
static uint8_t outputReportData[] = { 0 };

/* */

BLEKeyboard::BLEKeyboard(BLE &ble) : HIDServiceBase(ble,
                       KEYBOARD_REPORT_MAP, 
                       sizeof(KEYBOARD_REPORT_MAP),
                       emptyInputReportData,
                       outputReportData,
                       NULL,
                       sizeof(inputReport),
                       sizeof(outputReportData),
                       0)
{
}

size_t BLEKeyboard::write(uint8_t k) {
  uint8_t code;
  uint8_t modifier;

  this->keyToCodeAndModifier(k, code, modifier);

  return (this->press(code, modifier) && this->release(code, modifier));
}

size_t BLEKeyboard::press(uint8_t code, uint8_t modifiers) {
  size_t written = 0;

  if (code != 0) {
    for (unsigned int i = 2; i < sizeof(this->_value); i++) {
      if (this->_value[i] == 0) {
        this->_value[0] |= modifiers;
        this->_value[i] = code;

        written = 1;
        break;
      }
    }
  } else if (modifiers) {
    this->_value[0] |= modifiers;
    written = 1;
  }

  if (written) {
    this->sendValue();
  }

  return written;
}

size_t BLEKeyboard::release(uint8_t code, uint8_t modifiers) {
  size_t cleared = 0;

  if (code != 0) {
    for (unsigned int i = 2; i < sizeof(this->_value); i++) {
      if (this->_value[i] == code) {
        this->_value[0] &= ~modifiers;
        this->_value[i] = 0;

        cleared = 1;
        break;
      }
    }
  } else if (modifiers) {
    this->_value[0] &= ~modifiers;
    cleared = 1;
  }

  if (cleared) {
    this->sendValue();
  }

  return cleared;
}

void BLEKeyboard::releaseAll(void) {
  memset(this->_value, 0, sizeof(this->_value));

  this->sendValue();
}



void BLEKeyboard::sendValue() {
  send(this->_value);
}


#define SHIFT 0x80

static const unsigned char asciiMap[] = {
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  KEYCODE_BACKSPACE,          // '\b'
  KEYCODE_TAB,                // '\t'
  KEYCODE_ENTER,              // '\n'
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  KEYCODE_SPACE,              // ' '
  0x1e | SHIFT,               // '!'
  0x34 | SHIFT,               // '"'
  0x20 | SHIFT,               // '#'
  0x21 | SHIFT,               // '$'
  0x22 | SHIFT,               // '%'
  0x24 | SHIFT,               // '&'
  0x34,                       // '''
  0x26 | SHIFT,               // '('
  0x27 | SHIFT,               // ')'
  0x25 | SHIFT,               // '*'
  0x2e | SHIFT,               // '+'
  KEYCODE_COMMA,              // ','
  KEYCODE_MINUS,              // '-'
  KEYCODE_PERIOD,             // '.'
  KEYCODE_SLASH,              // '/'
  KEYCODE_0,                  // '0'
  KEYCODE_1,                  // '1'
  KEYCODE_2,                  // '2'
  KEYCODE_3,                  // '3'
  KEYCODE_4,                  // '4'
  KEYCODE_5,                  // '5'
  KEYCODE_6,                  // '6'
  KEYCODE_7,                  // '7'
  KEYCODE_8,                  // '8'
  KEYCODE_9,                  // '9'
  0x33 | SHIFT,               // ':'
  0x33,                       // ';'
  0x36 | SHIFT,               // '<'
  KEYCODE_EQUAL,              // '='
  0x37 | SHIFT,               // '>'
  0x38 | SHIFT,               // '?'
  0x1f | SHIFT,               // '@'
  KEYCODE_A | SHIFT,          // 'A'
  KEYCODE_B | SHIFT,          // 'B'
  KEYCODE_C | SHIFT,          // 'C'
  KEYCODE_D | SHIFT,          // 'D'
  KEYCODE_E | SHIFT,          // 'E'
  KEYCODE_F | SHIFT,          // 'F'
  KEYCODE_G | SHIFT,          // 'G'
  KEYCODE_H | SHIFT,          // 'H'
  KEYCODE_I | SHIFT,          // 'I'
  KEYCODE_J | SHIFT,          // 'J'
  KEYCODE_K | SHIFT,          // 'K'
  KEYCODE_L | SHIFT,          // 'L'
  KEYCODE_M | SHIFT,          // 'M'
  KEYCODE_N | SHIFT,          // 'N'
  KEYCODE_O | SHIFT,          // 'O'
  KEYCODE_P | SHIFT,          // 'P'
  KEYCODE_Q | SHIFT,          // 'Q'
  KEYCODE_R | SHIFT,          // 'R'
  KEYCODE_S | SHIFT,          // 'S'
  KEYCODE_T | SHIFT,          // 'T'
  KEYCODE_U | SHIFT,          // 'U'
  KEYCODE_V | SHIFT,          // 'V'
  KEYCODE_W | SHIFT,          // 'W'
  KEYCODE_X | SHIFT,          // 'X'
  KEYCODE_Y | SHIFT,          // 'Y'
  KEYCODE_Z | SHIFT,          // 'Z'
  KEYCODE_SQBRAK_LEFT,        // '['
  KEYCODE_BACKSLASH,          // '\'
  KEYCODE_SQBRAK_RIGHT,       // ']'
  0x23 | SHIFT,               // '^'
  0x2d | SHIFT,               // '_'
  0x35,                       // '`'
  KEYCODE_A,                  // 'a'
  KEYCODE_B,                  // 'b'
  KEYCODE_C,                  // 'c'
  KEYCODE_D,                  // 'd'
  KEYCODE_E,                  // 'e'
  KEYCODE_F,                  // 'f'
  KEYCODE_G,                  // 'g'
  KEYCODE_H,                  // 'h'
  KEYCODE_I,                  // 'i'
  KEYCODE_J,                  // 'j'
  KEYCODE_K,                  // 'k'
  KEYCODE_L,                  // 'l'
  KEYCODE_M,                  // 'm'
  KEYCODE_N,                  // 'n'
  KEYCODE_O,                  // 'o'
  KEYCODE_P,                  // 'p'
  KEYCODE_Q,                  // 'q'
  KEYCODE_R,                  // 'r'
  KEYCODE_S,                  // 's'
  KEYCODE_T,                  // 't'
  KEYCODE_U,                  // 'u'
  KEYCODE_V,                  // 'v'
  KEYCODE_W,                  // 'w'
  KEYCODE_X,                  // 'x'
  KEYCODE_Y,                  // 'y'
  KEYCODE_Z,                  // 'z'
  0x2f | SHIFT,               // '{'
  0x31 | SHIFT,               // '|'
  0x30 | SHIFT,               // '}'
  0x35 | SHIFT,               // '~'
  0x0
};

void BLEKeyboard::keyToCodeAndModifier(uint8_t k, uint8_t& code, uint8_t& modifier) {
  code = 0;
  modifier = 0;

  if (k < 128) {
    code = pgm_read_byte(asciiMap + k);

    if (code & 0x80) {
      modifier = KEYCODE_MOD_LEFT_SHIFT;
      code &= 0x7f;
    }
  }
}