// -*- c++ -*-

#pragma once

#include <Arduino.h>

// Backward compatibility stuff here
#define HARDWARE_IMPLEMENTATION kaleidoscope::hardware::Keyboard
// End backcompat

#include "model01/KeyswitchData.h"
#include "model01/Color.h"
#include "model01/LedAddr.h"
#include "model01/KeyAddr.h"
#include "model01/Scanner.h"

#include <kaleidoscope/KeyswitchState.h>
#include <kaleidoscope/KeyswitchEvent.h>


namespace kaleidoscope {
namespace hardware {

// This needs to be a macro so we can check the keymap definitions
#define TOTAL_KEYS_STR "64"
constexpr byte total_keys = 64;


class Keyboard {

 public:
  // This class should really be a singleton, but it probably costs a few bytes for the
  // extra getInstance() method that would be required to do that.
  Keyboard();

  // New API
  void scanMatrix();

  // should probably return KeyswitchEvent instead
  KeyswitchEvent nextKeyswitchEvent(KeyAddr& k);
  // I really don't think we need this function, but maybe it will be useful
  KeyswitchState keyswitchState(KeyAddr k) const;

  // Update all LEDs to values set by set*Color() functions below
  void updateLeds();

  // These functions operate on LedAddr values, which are different from corresponding KeyAddr values
  Color getLedColor(LedAddr led) const;
  void  setLedColor(LedAddr led, Color color);

  // These are the KeyAddr versions, which call the LedAddr functions
  Color getKeyColor(KeyAddr k) const;
  void  setKeyColor(KeyAddr k, Color color);

  // I'm leaving these functions alone for now; they shall remain mysterious
  void setup();

  // This function is used by TestMode
  void setKeyscanInterval(byte interval);

 private:
  static constexpr byte HAND_BIT = B00100000;

  Scanner scanners_[2];

  union KeyswitchScan {
    KeyswitchData hands[2];
    byte banks[total_keys / 8];  // CHAR_BIT
  };
  KeyswitchScan curr_scan_;
  KeyswitchScan prev_scan_;

  // I'm not sure we need this conversion function. On the other hand, maybe it should be
  // public...
  LedAddr getLedAddr(KeyAddr key_addr) const;

  // special functions for Model01; make private if possible
  void enableHighPowerLeds();
  void enableScannerPower();
  boolean ledPowerFault();

  // This doesn't seem to be called anywhere
  void rebootBootloader();
};

} // namespace model01 {
} // namespace kaleidoscope {
