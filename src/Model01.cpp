// Why don't we #include "Model01.h"? I guess we get it via <Kaleidoscope.h>
#include <Kaleidoscope.h>
// Does HID interface stuff really belong here?
#include <KeyboardioHID.h>
// todo: look up wdt.h
#include <avr/wdt.h>

namespace kaleidoscope {
namespace model01 {

// Why don't we do these things in the constructor? Why are they static? There's only one object...
Scanner Keyboard::leftHand(0);
Scanner Keyboard::rightHand(3);
bool Keyboard::isLEDChanged = true;

// *INDENT-OFF*
static constexpr uint8_t key_led_map[TOTAL_KEYS] = {
  27, 26, 20, 19, 12, 11,  4,  3,
  28, 25, 21, 18, 13, 10,  5,  2,
  29, 24, 22, 17, 14,  9,  6,  1,
  30, 31, 23, 16, 15,  8,  7,  0,

  60, 59, 52, 51, 44, 43, 37, 36,
  61, 58, 53, 50, 45, 42, 38, 35,
  62, 57, 54, 49, 46, 41, 39, 34,
  63, 56, 55, 48, 47, 40, 32, 33,
};
// *INDENT-ON*


void Keyboard::scanMatrix() {
  // copy current keyswitch state array to previous
  memcpy(prev_state_, curr_state_, sizeof(prev_state_));

  // scan left hand
  if (scanners_[0].readKeys())
    curr_state_[0] = scanners_[0].getKeyData();
  // scan right hand
  if (scanners_[1].readKeys())
    curr_state_[1] = scanners_[1].getKeyData();
}


// get the address of the next key that changed state (if any)
KeyAddr Keyboard::getNextKeyswitchEvent(KeyAddr key_addr) {
  for (byte r = (key_addr / 8); r < 8; ++r) {
    if (keyboard_state_[r] == prev_keyboard_state_[r])
      continue;
    for (byte c = (key_addr % 8); c < 8; ++c) {
      if (bitRead(keyboard_state_[r], c) != bitRead(prev_keyboard_state_[r], c))
	return keyaddr::addr(r, c);
    }
  }
  return UNKNOWN_KEY_ADDR;
}


LedAddr Keyboard::getLedAddr(KeyAddr key_addr) {
  return key_led_map[key_addr];
}

Color Keyboard::getLedColor(LedAddr led_addr) {
  bool hand = led_addr & HAND_BIT; // B10000000
  return scanners_[hand].getLedColor(led_addr & ~HAND_BIT);
}

void Keyboard::setLedColor(LedAddr led_addr, Color color) {
  bool hand = led_addr & HAND_BIT; // B10000000
  scanners_[hand].setLedColor(led_addr & ~HAND_BIT, color);
}

Color Keyboard::getKeyColor(KeyAddr key_addr) {
  LedAddr led_addr = getLedAddr(key_addr);
  return getLedColor(led_addr);
}

void Keyboard::setKeyColor(KeyAddr key_addr, Color color) {
  LedAddr led_addr = getLedAddr(key_addr);
  setLedColor(led_addr, color);
}


// This function is a bit better now, but I still feel the desire to write this as an
// explicit loop
void Keyboard::updateLeds() {
  scanners_[0].updateNextLedBank();
  scanners_[1].updateNextLedBank();

  scanners_[0].updateNextLedBank();
  scanners_[1].updateNextLedBank();

  scanners_[0].updateNextLedBank();
  scanners_[1].updateNextLedBank();

  scanners_[0].updateNextLedBank();
  scanners_[1].updateNextLedBank();
}


// My question here is why this is done in a separate setup() function; I suppose it's
// because we need other objects to start up before calling functions that affect the
// scanners
void Keyboard::setup() {
  wdt_disable();
  delay(100);
  enableScannerPower();

  // Consider not doing this until 30s after keyboard
  // boot up, to make it easier to rescue things
  // in case of power draw issues.
  enableHighPowerLeds();
  leftHandState.all = 0;
  rightHandState.all = 0;

  TWBR = 12; // This is 400mhz, which is the fastest we can drive the ATTiny
}


void Keyboard::enableScannerPower() {
  // PC7
  //pinMode(13, OUTPUT);
  //digitalWrite(13, HIGH);
  // Turn on power to the LED net
  DDRC |= _BV(7);
  PORTC |= _BV(7);
}


// This lets the keyboard pull up to 1.6 amps from
// the host. That violates the USB spec. But it sure
// is pretty looking
void Keyboard::enableHighPowerLeds() {
  // PE6
  //    pinMode(7, OUTPUT);
  //    digitalWrite(7, LOW);
  DDRE |= _BV(6);
  PORTE &= ~_BV(6);

  // Set B4, the overcurrent check to an input with an internal pull-up
  DDRB &= ~_BV(4);	// set bit, input
  PORTB &= ~_BV(4);	// set bit, enable pull-up resistor
}


boolean Keyboard::ledPowerFault() {
  if (PINB & _BV(4)) {
    return true;
  } else {
    return false;
  }
}


void Keyboard::rebootBootloader() {
  // Set the magic bits to get a Caterina-based device
  // to reboot into the bootloader and stay there, rather
  // than run move onward
  //
  // These values are the same as those defined in
  // Caterina.c

  uint16_t bootKey = 0x7777;
  uint16_t *const bootKeyPtr = reinterpret_cast<uint16_t *>(0x0800);

  // Stash the magic key
  *bootKeyPtr = bootKey;

  // Set a watchdog timer
  wdt_enable(WDTO_120MS);

  while (1) {} // This infinite loop ensures nothing else
  // happens before the watchdog reboots us
}


void Keyboard::setKeyscanInterval(uint8_t interval) {
  leftHand.setKeyscanInterval(interval);
  rightHand.setKeyscanInterval(interval);
}

// Why do we declare this object here?
HARDWARE_IMPLEMENTATION KeyboardHardware;

} // namespace hardware {
} // namespace kaleidoscope {
