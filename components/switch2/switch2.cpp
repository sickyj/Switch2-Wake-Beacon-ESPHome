#include "switch2.h"

#include "esphome/core/log.h"

#include <cstdio>
#include <esp_mac.h>  // esp_iface_mac_addr_set(), ESP_MAC_BT - file scope, legal here

namespace esphome {
namespace switch2 {

static const char *const TAG = "switch2";

bool spoof_bt_mac(const std::string &mac_str) {
  if (mac_str.length() != 17) {
    ESP_LOGI(TAG, "No saved MAC - running with the real hardware MAC.");
    return false;
  }

  uint8_t mac[6];
  int parsed = sscanf(mac_str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2],
                      &mac[3], &mac[4], &mac[5]);
  if (parsed != 6) {
    ESP_LOGE(TAG, "Saved MAC failed to parse (%d/6) - skipping spoof.", parsed);
    return false;
  }

  esp_err_t err = esp_iface_mac_addr_set(mac, ESP_MAC_BT);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Hardware BT MAC successfully spoofed to Joy-Con MAC!");
    return true;
  }
  ESP_LOGE(TAG, "Hardware MAC spoofing failed with error: %d", err);
  return false;
}

}  // namespace switch2
}  // namespace esphome
