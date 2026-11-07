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

These counters are not exposed in Z2M, are not persisted to Flash and do not generate any extra Zigbee packets. The counters can be inspected with suitable debugging tools; **Telink Burning Tool alone is not established as a live RAM-inspection interface**. A separate diagnostic transport is required before they are usable during normal HA TEST testing. This first code change is intentionally non-invasive; it does not yet resolve lost motion.

## Build
The development branch contains a GitHub Actions job (`.github/workflows/build-zg204zl-pir.yml`) to compile a test binary from bundled Telink SDK/toolchain. Check the workflow result and binary checksums before attempting to flash; committing workflow source does not guarantee a successful build. Generated artifacts are not flashed automatically.

## Next tasks
1. Confirm compiler success and inspect image metadata, flash layout and rollback procedure.
2. Expose debugging data safely (wired SWS logging or read-only manufacturer-specific Zigbee attributes) with controlled power usage.
3. Pair device on HA TEST and compare raw events vs. occupancy and Zigbee reports in low/medium/high lighting.
4. Separate PIR edge sampling from illuminance ADC operations if evidence supports interference.
5. Design occupancy timeout/hold semantics, with explicit user requirements before changing behavior.
