#pragma once

#include "esphome/core/component.h"

#include <cstdint>
#include <string>

namespace esphome {
namespace switch2 {

// Empty component. Exists so ESPHome compiles switch2.cpp and includes this
// header in main.cpp, making spoof_bt_mac() reachable from lambdas.
class Switch2Component : public Component {};

// Spoof the ESP32's Bluetooth MAC to the given "AA:BB:CC:DD:EE:FF" string.
// Returns true on success. A non-17-char string is treated as "no saved MAC"
// (no-op, returns false). Implemented in switch2.cpp so <esp_mac.h> is included
// at file scope - which is why the caller needs no local header.
bool spoof_bt_mac(const std::string &mac_str);

}  // namespace switch2
}  // namespace esphome
