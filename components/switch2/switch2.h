#pragma once

#include "esphome/core/component.h"

#include <cstdint>
#include <string>

namespace esphome {
namespace switch2 {

// Empty component. Exists so ESPHome compiles switch2.cpp and includes this
// header in main.cpp, making these helpers reachable from lambdas.
class Switch2Component : public Component {};

// Spoof the ESP32's Bluetooth MAC to the given "AA:BB:CC:DD:EE:FF" string.
// Records success internally (see can_wake). A non-17-char string is treated as
// "no saved MAC" (no-op, returns false). Implemented in switch2.cpp so the
// callers need no local <esp_mac.h>.
bool spoof_bt_mac(const std::string &mac_str);

// Build the 31-byte wake advertisement ONCE from the saved payload + flag byte.
// Call at boot; returns true if the payload was valid. Avoids rebuilding the
// advert on every wake / every burst.
bool prepare_wake_adv(const std::string &payload_hex, uint8_t flag_byte);

// True only when the device can actually wake: advert prepared AND the MAC
// spoof succeeded. Use this for readiness rather than "a MAC is saved".
bool can_wake();

// Wake sequence: load the prepared advert into the controller once per press
// (arm_wake), then start/stop advertising per burst.
void arm_wake();
void start_wake();
void stop_wake();

}  // namespace switch2
}  // namespace esphome
