# Changelog

All notable changes are documented here. This project uses [semantic versioning](https://semver.org/).
See the [GitHub releases](https://github.com/sickyj/Switch2-Wake-Beacon-ESPHome/releases) for the full notes.

## [2.4.0](https://github.com/sickyj/Switch2-Wake-Beacon-ESPHome/releases/tag/v2.4.0)
- Build the wake advertisement **once at boot** instead of rebuilding it on every burst;
  the wake path is now just arm → start/stop.
- **Ready** now reflects whether the MAC spoof actually succeeded, not just that a MAC is saved.
- New `scan_active` substitution (default `false`) + troubleshooting note for controllers
  that only expose data in a scan response.
- CI: cancel superseded in-progress runs (`concurrency`).
- Docs: simplified install — a single `packages:` import now pulls in the component, wake
  logic, Wi‑Fi/API/OTA, and the framework.

## [2.3.0](https://github.com/sickyj/Switch2-Wake-Beacon-ESPHome/releases/tag/v2.3.0)
- Add a **Ready** binary sensor — on once a Joy-Con is captured (the device can wake).
- CI now also validates the example `esp-home.yaml` (remote import path), not just the local build.

## [2.2.0](https://github.com/sickyj/Switch2-Wake-Beacon-ESPHome/releases/tag/v2.2.0)
- **Automatic capture on first boot** when nothing is saved — hands-off setup (`auto_capture`, default on).
- Capture Mode moved to the advanced/hidden controls (re-capture only).

## [2.1.0](https://github.com/sickyj/Switch2-Wake-Beacon-ESPHome/releases/tag/v2.1.0)
- **Advanced mode**: destructive/rarely-needed entities hidden by default (`hide_advanced`).
- `dashboard_import` for one-click adopt from the ESPHome dashboard.
- `esphome.project` for Home Assistant update notifications.

## [2.0.0](https://github.com/sickyj/Switch2-Wake-Beacon-ESPHome/releases/tag/v2.0.0)
- **Removed `custom_mac.h`**: the C++ MAC-spoof now ships as a remote external component
  (`components/switch2/`). Install is pure YAML — no local files.
- **Breaking:** replace the local header with an `external_components:` block.

## [1.0.0](https://github.com/sickyj/Switch2-Wake-Beacon-ESPHome/releases/tag/v1.0.0)
- First tagged release: capture a Joy-Con and wake the Switch 2 from Home Assistant.
- Capture auto-timeout, 3-burst wake, MAC-parse validation, tunable substitutions, CI build.
