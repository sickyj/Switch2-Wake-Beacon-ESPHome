# 🎮 ESPHome Switch 2 Wake Beacon

Wake your **Nintendo Switch 2** directly from Home Assistant using an ESP32.

By capturing a paired Joy-Con's Bluetooth MAC address and advertising payload, this
beacon **hardware-spoofs its own Bluetooth radio** to impersonate your controller,
waking the console instantly — without interrupting the ESP32's Wi-Fi connection to
Home Assistant.

> 🤖 The ESPHome YAML and C++ hardware-spoofing logic here were AI-generated
> (Google Gemini) and are based on the BLE wake-protocol research in
> [alexvnesta/switch2controller](https://github.com/alexvnesta/switch2controller).

---

## ✅ Requirements

- An **ESP32** board (tested on FireBeetle ESP32; most ESP32 variants work).
- ESPHome **2024.11.0+**.
- The **esp-idf** framework — the Arduino BLE stack can't do raw advertising or
  hardware MAC spoofing. (The example base config sets this for you.)
- A Switch 2 with a paired **Joy-Con 2**.

## 🚀 Install

1. Start from [`esp-home.yaml`](esp-home.yaml) as your device's base config (copy the
   parts you need into your own config), or add the package block below to an existing
   esp-idf ESP32 config:

   ```yaml
   packages:
     switch2_wake:
       url: https://github.com/sickyj/Switch2-Wake-Beacon-ESPHome
       files: [switch2_master.yaml]
       refresh: 1d
   ```

2. Install / flash the firmware to your ESP32.

There's no separate header file to download anymore — the required ESP-IDF headers are
pulled in via build flags inside the package.

## 🎮 How to use

After flashing, these entities appear in Home Assistant:

| Entity | Type | What it does |
|---|---|---|
| **Capture Mode** | switch | Listen for your Joy-Con and learn its identity (auto-off after 60s) |
| **Wake Switch 2** | button | Broadcast the wake beacon (3 short bursts) |
| **Clear Saved Data** | button | Wipe the saved payload/MAC and reboot to restore the real BT MAC |
| **Reboot Device** | button | Restart the ESP32 |
| **Saved Nintendo Payload** | sensor | The captured 24-byte payload (hex) |
| **Saved Joy-Con MAC** | sensor | The captured Joy-Con MAC |
| **Wake Status** | sensor | Human-readable status of the last action |

### Step 1 — Capture (do this once)

1. Turn **ON** the *Capture Mode* switch.
2. Wake your controller by pressing & holding **Home** on the Joy-Con 2.
3. The ESP32 intercepts the BLE advertisement, saves the MAC + payload to flash, and
   **reboots** to apply the hardware MAC spoof. Capture Mode auto-disables.

Confirm it worked via the **Saved Nintendo Payload** / **Saved Joy-Con MAC** sensors.

### Step 2 — Wake the console

Press **Wake Switch 2**. The ESP32 broadcasts the 31-byte raw advertisement with the
`0x81` wake flag, and the console powers on. **Wake Status** shows the result.

## 🔧 Troubleshooting

- **Nothing captured** — make sure the Joy-Con is actively advertising (hold Home) while
  Capture Mode is on; it only saves a 24-byte Nintendo (company ID `0x0553`) packet.
- **Wake does nothing** — verify both sensors are populated. If empty, run capture again.
- **Want to start over / restore the real MAC** — press *Clear Saved Data* (it reboots).

## ⚠️ Notes

This impersonates a device's Bluetooth identity to wake **your own** console. Use it only
on hardware you own. Spoofing the BT MAC is done in RAM at boot and does not permanently
alter the ESP32.

## 🙏 Credits

- **Reverse engineering** the Switch 2 BLE wake protocol and the 31-byte payload
  structure: [alexvnesta/switch2controller](https://github.com/alexvnesta/switch2controller).
- **ESPHome adaptation** (YAML packaging + C++ lambda workarounds): generated with
  Google Gemini, refined afterwards.
