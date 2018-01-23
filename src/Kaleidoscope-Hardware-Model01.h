// -*- c++ -*-

#pragma once

// Basically, this file is here to make the model01::Keyboard class available to
// Kaleidoscope
#include "model01/Keyboard.h"

#include "model01/KeyAddr.h"
#include "model01/LedAddr.h"
#include "model01/Color.h"

namespace kaleidoscope {

// Not sure this is the correct syntax
namespace hardware = mode01;

// Maybe this is a better way to get the one object we want to export, as opposed to the
// whole namespace alias above:
using hardware::Keyboard = model01::Keyboard;

// These aliases are the types exported for other modules to use
using KeyAddr = model01::KeyAddr;
using LedAddr = model01::LedAddr;
using Color   = model01::Color;

} // namespace kaleidoscope {
