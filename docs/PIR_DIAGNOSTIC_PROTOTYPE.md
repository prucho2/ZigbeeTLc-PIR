# ZG-204ZL PIR diagnostic prototype (development branch)

Status: **source changes committed; build and hardware functionality not yet verified**.

## Safety
- Only for the disassembled ZG-204ZL with illuminance hardware on HA TEST.
- Do not flash either `bath_pir` or `parter_pir` on MAIN.
- Keep the 512 KiB full-flash backup offline and keep its checksum.
- Do not perform a chip erase or write a 512 KiB backup as a normal firmware update.
- Verify Telink Burning Tool's firmware load address/bootloader layout **before any write**.
- Application firmware version/OTA headers have not been changed yet. Do not deploy development OTA until image type, manufacturer, version and hardware variant are verified.

## Current instrumentation
The branch enables `PIR_DIAG_ENABLE` in `src/board_zg204zl.h`.
`src/sensor_pir.c` exposes a volatile RAM object `g_pir_diag` with counters:
- `raw_rise`: transitions sampled from GPIO low to active.
- `raw_fall`: transitions sampled from GPIO active to inactive.
- `occupancy_on`: occupancy transitions on the basic PIR event path.
- `occupancy_off`: timer callbacks that clear occupancy.
- `timer_callbacks`: calls to the occupancy timer callback.
- `light_above_target`: motion-on events with illuminance above configured target.
- `report_requested`: occupancy paths that request reporting; **not a count of transmitted frames or Z2M acknowledgements**.

The counters are **also exposed as read-only Zigbee attributes**, on cluster `msOccupancySensing` (0x0406), endpoint 1. The existing standard attributes (`occupancy`, `pirOToUDelay`) remain intact. The debug fields are not reported automatically, and no custom Z2M converter is required merely to issue a raw attribute read.

| Hex attribute | Diagnostic field | Data type |
| --- | --- | --- |
| 0xF100 | raw_rise | uint32 |
| 0xF101 | raw_fall | uint32 |
| 0xF102 | occupancy_on | uint32 |
| 0xF103 | occupancy_off | uint32 |
| 0xF104 | timer_callbacks | uint32 |
| 0xF105 | light_above_target | uint32 |
| 0xF106 | report_requested | uint32 |
| 0xF107 | raw_level | uint8 |

To try reading on HA TEST via Zigbee2MQTT, first pair the test PIR. Use its **Dev console** to read cluster `msOccupancySensing` (0x0406), endpoint 1, one numeric attribute (e.g. 0xF100 = 61696). Whether the installed Z2M UI accepts numeric custom attribute IDs requires testing; an external converter may be needed for a convenient permanent dashboard. **Do not issue automatic reads every second**: the PIR is battery powered and asleep much of the time. Polling should be occasional and manual, preferably shortly after a wakeup, and a timeout does not prove that the firmware is faulty.

Counters reset when firmware restarts. The `raw_level` is the most recently sampled GPIO state, not continuously streaming motion; narrow pulses can still be missed. `report_requested` counts requests to check reporting but is **not proof of transmission or coordinator acknowledgement**. This build is diagnostic only and does not yet fix lost motion.

## Build
The development branch contains a GitHub Actions job (`.github/workflows/build-zg204zl-pir.yml`) to compile a test binary from bundled Telink SDK/toolchain. Check the workflow result and binary checksums before attempting to flash; committing workflow source does not guarantee a successful build. Generated artifacts are not flashed automatically.

## Next tasks
1. Confirm compiler success and inspect image metadata, flash layout and rollback procedure.
2. Expose debugging data safely (wired SWS logging or read-only manufacturer-specific Zigbee attributes) with controlled power usage.
3. Pair device on HA TEST and compare raw events vs. occupancy and Zigbee reports in low/medium/high lighting.
4. Separate PIR edge sampling from illuminance ADC operations if evidence supports interference.
5. Design occupancy timeout/hold semantics, with explicit user requirements before changing behavior.
