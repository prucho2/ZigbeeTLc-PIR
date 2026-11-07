# ZG-204ZL-z: Zigbee2MQTT vs PVVX ZHA quirk (2026-11-07)

Sources inspected:
- Upstream [zigbee-herdsman-converters `src/devices/zigbeetlc.ts`](https://github.com/Koenkk/zigbee-herdsman-converters/blob/master/src/devices/zigbeetlc.ts), device `ZG-204ZL-z`.
- PVVX [`zhaquirks/custom/zigbeetlc.py`](https://github.com/pvvx/ZigbeeTLc/blob/master/zhaquirks/custom/zigbeetlc.py), `QuirkBuilder("Sonoff", "ZG-204ZL-z")`.
- PVVX `src/app_EpCfg.c` cluster tables.
- ZHC version 26.126.0 (2026-11-03) added remote On/Off, light level and threshold support to ZG-204ZL-z; see upstream PR #13635.

## Features implemented in current upstream converter
- battery and battery voltage; occupancy; illuminance
- `power_on_behavior` and remote On/Off binding
- `pir_timeout` via Occupancy Sensing cluster `pirOToUDelay`
- `light_level` via Illuminance Level Sensing `levelStatus`
- `illuminance_threshold` (0x5003, uint24; physical lux threshold)
- `illuminance_measurement_interval` (0x5000)
- `illuminance_offset` (0x5001)
- `illuminance_maximum` (0x5002)

## PVVX ZHA quirk comparison
The ZHA quirk explicitly defines `level_status`, `lx_ligth_level` (0x5003), `lx_zero` (0x5001), `lx_coef` (0x5002), `illuminance_target_level` (standard 0x0010), and `meas_interval` (0x5000). It does not explicitly create a new `pir_timeout` entity for this model; that value is an existing standard Zigbee occupancy attribute and may be surfaced by ZHA's standard cluster facilities.

Difference worth examining: the ZHA quirk additionally exposes raw `illuminance_target_level` (0x0010) distinct from the 0x5003 physical-lux threshold. The latest Zigbee2MQTT model does not explicitly expose 0x0010. Confirm the relationship and scaling before writing an extra control: it may duplicate the user-facing 0x5003 field, not a genuinely independent setting.

**Installed converter may lag upstream**: the user's HA TEST screenshot shows only five exposes, not the recently merged controls. Check the installed zigbee-herdsman-converters version before developing a replacement or overriding it.

## Local firmware development
For board ZG-204ZL only, `APP_BUILD=0x41` (0.1.4.1-dev) and `DEF_OCCUPANCY_DELAY=2`. Values previously saved as a Zigbee `pir_timeout` in non-volatile memory take precedence, and this change deliberately does not wipe settings. The existing debug counters remain present; algorithmic PIR/illuminance behavior remains unchanged.

Note: `DEV` is a release label in source/artifact naming, not encoded as a separate Zigbee app build suffix.
