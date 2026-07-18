<div align="center">

# 🎮 ESPHome Switch 2 Wake Beacon

**Wake your Nintendo Switch 2 remotely from Home Assistant — no Pro Controller required.**

An ESP32 captures your Joy‑Con's Bluetooth identity, then impersonates it on demand to
power the console on.

![Platform](https://img.shields.io/badge/platform-ESP32-000000?logo=espressif&logoColor=white)
![Framework](https://img.shields.io/badge/framework-ESP--IDF-red)
![ESPHome](https://img.shields.io/badge/ESPHome-2024.11%2B-1f8dd6?logo=esphome&logoColor=white)
![Home Assistant](https://img.shields.io/badge/Home%20Assistant-integrated-41bdf5?logo=homeassistant&logoColor=white)
![License](https://img.shields.io/badge/license-MIT-green)
![Build](https://img.shields.io/badge/esphome%20compile-passing-brightgreen)

</div>

---

## Contents

- [How it works](#how-it-works)
- [Requirements](#-requirements)
- [Install](#-install)
- [How to use](#-how-to-use)
- [Home Assistant entities](#home-assistant-entities)
- [Repository layout](#-repository-layout)
- [Troubleshooting](#-troubleshooting)
- [FAQ](#-faq)
- [Safety & legality](#-safety--legality)
- [Credits](#-credits)
- [License](#-license)

---

## How it works

A Switch 2 can be woken by a BLE advertisement that looks exactly like one from its own
paired Joy‑Con. Two things must match: the **Bluetooth MAC address** it comes from, and
the **Nintendo manufacturer payload** inside it. So the ESP32 works in two phases —
**learn** the Joy‑Con's identity, then **replay** it whenever you press a button.

```mermaid
flowchart TD
    subgraph Capture["🔎 Capture (once)"]
        A[Turn on Capture Mode] --> B[Scan BLE adverts]
        B --> C{Nintendo packet?<br/>company 0x0553, 24 bytes}
        C -- no --> B
        C -- yes --> D[Save MAC + payload to flash]
        D --> E[Reboot]
    end
    subgraph Boot["⚙️ Every boot"]
        E --> F[Spoof the ESP32's BT MAC<br/>to the Joy-Con MAC]
    end
    subgraph Wake["📡 Wake (on demand)"]
        F --> G[Press Wake Switch 2]
        G --> H[Build 31-byte advert<br/>set byte 16 = 0x81 wake flag]
        H --> I[Broadcast 3 short bursts]
        I --> J[🎮 Switch 2 powers on]
    end
```

Because the ESP32 wears the Joy‑Con's MAC at the hardware level, it advertises from a
**public** address and doesn't need to drop its Wi‑Fi/Home Assistant connection to do it.

## ✅ Requirements

| | |
|---|---|
| **Board** | Any **ESP32** (developed on a DFRobot FireBeetle ESP32; most ESP32 variants work) |
| **ESPHome** | **2024.11.0** or newer |
| **Framework** | **ESP‑IDF** — the Arduino BLE stack can't do raw advertising or hardware MAC spoofing |
| **Console** | Nintendo **Switch 2** with a paired **Joy‑Con 2** |

## 🚀 Install

1. Use [`esp-home.yaml`](esp-home.yaml) as a starting point for your device config, or add
   the package block below to an existing **esp‑idf** ESP32 config:

   ```yaml
   packages:
     switch2_wake:
       url: https://github.com/sickyj/Switch2-Wake-Beacon-ESPHome
       files: [switch2_master.yaml]
       refresh: 1d
   ```

2. Install / flash the firmware to your ESP32 from ESPHome.

> [!NOTE]
> Importing from the **repository** (`url:` + `files:`) means ESPHome fetches the sibling
> `custom_mac.h` automatically — there's no header file to download or URL to paste by
> hand. `custom_mac.h` only exists to make `<esp_mac.h>` visible for the boot‑time MAC
> spoof.

## 🎮 How to use

### Step 1 — Capture (do this once)

1. In Home Assistant, turn **ON** the **Capture Mode** switch.
2. Wake your controller by pressing & holding **Home** on the Joy‑Con 2.
3. The ESP32 grabs the BLE advertisement, saves the MAC + payload to flash, and **reboots**
   to apply the hardware MAC spoof. Capture Mode switches itself off (and auto‑times out
   after 60 s if nothing is found).

Confirm it worked via the **Saved Nintendo Payload** and **Saved Joy‑Con MAC** sensors.

### Step 2 — Wake the console

Press **Wake Switch 2**. The ESP32 broadcasts the 31‑byte advertisement with the `0x81`
wake flag (three short bursts), and the console powers on. **Wake Status** reports the
result.

### Home Assistant entities

| Entity | Type | What it does |
|---|---|---|
| **Capture Mode** | switch | Listen for your Joy‑Con and learn its identity (auto‑off after 60 s) |
| **Wake Switch 2** | button | Broadcast the wake beacon (3 short bursts) |
| **Clear Saved Data** | button | Wipe the saved payload/MAC and reboot to restore the real BT MAC |
| **Reboot Device** | button | Restart the ESP32 |
| **Saved Nintendo Payload** | sensor | The captured 24‑byte payload (hex) |
| **Saved Joy‑Con MAC** | sensor | The captured Joy‑Con MAC |
| **Wake Status** | sensor | Human‑readable status of the last action |

## 📂 Repository layout

| File | Purpose |
|---|---|
| [`switch2_master.yaml`](switch2_master.yaml) | The wake/capture package — import this into your config |
| [`esp-home.yaml`](esp-home.yaml) | Example base config (board, Wi‑Fi, framework) showing how to import the package |
| [`custom_mac.h`](custom_mac.h) | One‑line header that exposes `<esp_mac.h>` for the boot‑time MAC spoof |

## 🔧 Troubleshooting

| Symptom | Fix |
|---|---|
| **Nothing captured** | Hold **Home** on the Joy‑Con while Capture Mode is on. Only a 24‑byte Nintendo packet (company ID `0x0553`) is saved. |
| **Wake does nothing** | Check both the payload and MAC sensors are populated. If empty, run capture again. |
| **Want to start over** | Press **Clear Saved Data** — it wipes storage and reboots to restore the real BT MAC. |
| **Build fails on `ESP_MAC_BT`** | Make sure `custom_mac.h` is present next to `switch2_master.yaml` (it is, when imported via the git package form above). |

## ❓ FAQ

**Does this need the Pro Controller 2?**
No — that's the point. It mimics a standard Joy‑Con 2.

**Will it disconnect the ESP32 from Home Assistant while advertising?**
No. The wake advert is a non‑connectable broadcast and Wi‑Fi stays up.

**Does spoofing the MAC permanently change my ESP32?**
No. The MAC is set in RAM at each boot; clearing saved data and rebooting restores the
factory BT MAC.

**Can one ESP32 wake multiple consoles?**
This build stores one Joy‑Con identity at a time. Re‑capture to switch targets.

## ⚠️ Safety & legality

This impersonates a Bluetooth identity to wake **hardware you own**. Use it only on your
own console and controller. Nothing here is affiliated with or endorsed by Nintendo.

## 🙏 Credits

- **Reverse engineering** of the Switch 2 BLE wake protocol and the 31‑byte payload
  structure: [alexvnesta/switch2controller](https://github.com/alexvnesta/switch2controller).
- **ESPHome adaptation** (YAML packaging + C++ lambda logic): originally generated with
  Google Gemini, then refined and verified to compile against ESP‑IDF.

## 📄 License

Released under the [MIT License](LICENSE).
