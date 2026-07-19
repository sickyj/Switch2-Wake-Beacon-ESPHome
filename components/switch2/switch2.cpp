#include "switch2.h"

#include "esphome/core/log.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <esp_mac.h>            // esp_iface_mac_addr_set(), ESP_MAC_BT
#include <esp_gap_ble_api.h>    // esp_ble_gap_* advertising APIs

namespace esphome {
namespace switch2 {

static const char *const TAG = "switch2";

// Built once at boot; replayed on every wake so nothing is rebuilt per burst.
static uint8_t s_adv[31];
static bool s_adv_ready = false;
static bool s_mac_spoofed = false;

static esp_ble_adv_params_t s_adv_params = {
    .adv_int_min = 0x0020,
    .adv_int_max = 0x0040,
    .adv_type = ADV_TYPE_NONCONN_IND,
    // PUBLIC: the radio already wears the Joy-Con MAC (spoofed at boot).
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .peer_addr = {0, 0, 0, 0, 0, 0},
    .peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

bool spoof_bt_mac(const std::string &mac_str) {
  s_mac_spoofed = false;
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
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Hardware MAC spoofing failed with error: %d", err);
    return false;
  }
  ESP_LOGI(TAG, "Hardware BT MAC successfully spoofed to Joy-Con MAC!");
  s_mac_spoofed = true;
  return true;
}

bool prepare_wake_adv(const std::string &payload_hex, uint8_t flag_byte) {
  s_adv_ready = false;
  if (payload_hex.length() < 48) {  // 24 bytes = 48 hex chars
    return false;
  }

  // 0x02 0x01 0x06         : Flags AD
  // 0x1B 0xFF              : length + Manufacturer-Specific type
  // 0x53 0x05              : Company ID 0x0553 (little-endian)
  static const uint8_t header[7] = {0x02, 0x01, 0x06, 0x1B, 0xFF, 0x53, 0x05};
  memcpy(s_adv, header, sizeof(header));

  for (int i = 0; i < 24; i++) {
    s_adv[7 + i] = (uint8_t) strtol(payload_hex.substr(i * 2, 2).c_str(), nullptr, 16);
  }
  s_adv[16] = flag_byte;  // the critical wake-trigger flag

  s_adv_ready = true;
  ESP_LOGI(TAG, "Wake advertisement prepared.");
  return true;
}

bool can_wake() { return s_adv_ready && s_mac_spoofed; }

void arm_wake() { esp_ble_gap_config_adv_data_raw(s_adv, sizeof(s_adv)); }
void start_wake() { esp_ble_gap_start_advertising(&s_adv_params); }
void stop_wake() { esp_ble_gap_stop_advertising(); }

}  // namespace switch2
}  // namespace esphome
