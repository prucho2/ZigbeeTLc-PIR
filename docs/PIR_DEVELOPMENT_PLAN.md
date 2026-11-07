# PIR firmware development — ZG-204ZL

Project: independent development based on [pvvx/ZigbeeTLc](https://github.com/pvvx/ZigbeeTLc).

## Baseline and scope (2026-11-07)
- Hardware: ZG-204ZL-z (Telink TLSR8253).
- MAIN Zigbee2MQTT: `bath_pir` and `parter_pir`, reporting application firmware 0.1 build 64 / stack 3.0 build 1 (Z2M software build `3001-0140`).
- Third disassembled test PIR: hardware and currently installed firmware must be verified before flashing.
- Working principle: keep Zigbee networking, OTA, battery reporting, interview and most upstream behavior intact.
- Objectives: isolate raw PIR GPIO state from occupancy state transitions and illuminance measurement; diagnose lost motion events; improve occupancy semantics while retaining battery life.
- No changes to MAIN sensors or firmware until a working image has been built and tested on TEST hardware.

## Upstream code map
- `src/board_zg204zl.h`: `GPIO_PD4` PIR input, `PIR_ON=1`, `GPIO_PC4` illuminance ADC, `GPIO_PD3` illuminance sensor power, occupancy default 35 s.
- `src/sensor_pir.c`: samples PIR GPIO; detects rising state using `zcl_occupAttr.state`; sets occupancy and report flag; schedules occupancy-off timer; configures GPIO wakeup.
- `src/sensor_lx.c`: ADC illuminance sampling and light threshold evaluation.
- `src/app_main.c`: `app_task()` invokes `task_pir()` and runs the reporting pathway `app_chk_report()`.
- `src/app_EpCfg.c`: occupancy attributes and persistence, illuminance configuration.

## Findings requiring validation
1. In `task_pir()`, occupancy is set and `FLG_CHECK_REPORT` is raised before checking `ILSC_ABOVE_TARGET`; light threshold presently gates the optional remote ON command, not occupancy itself. Do not claim that this condition directly suppresses motion reports.
2. The PIR event path runs `read_illumi_sensor()` on an occupancy transition for light-capable builds. Evaluate duration/ADC/power interactions, and wakeup after retention sleep.
3. `occupancyTimerCb()` can keep occupancy true while PIR GPIO remains asserted. Investigate behavior of short pulses, sampling, timer callback lifetime and duplicate events; do not change semantics without evidence.
4. The `zcl_occupAttr.state` bit and the Zigbee `occupancy` bitfield are separate quantities. Track both, plus report scheduling/result.
5. For hardware without light sensor, a distinct board configuration is likely needed; do not assume it is electrically identical or that the light pin is unconnected safely.

## Proposed instrumentation (design only; not yet implemented)
- Record counters for observed PIR rising/falling edges, occupancy transitions, report attempts/success/failures, and wake causes.
- Keep counters RAM-only initially. Do not emit a Zigbee message on every interrupt: this can significantly increase battery drain.
- Add an optional debug build/profile; do not change normal Z2M-visible clusters until converter impact is reviewed.
- Test in darkness, moderate illumination and bright light while recording raw PIR behavior vs Zigbee occupancy.
- Compare behavior with light ADC enabled/disabled in separate test builds, retaining unchanged GPIO and wakeup logic as a control.

## Safe release workflow
1. Preserve current upstream code as baseline. Create an isolated development branch for functional changes.
2. Reproducible clean compilation using the upstream Makefile and Telink toolchain.
3. Check binary target, firmware image metadata and OTA header before flashing; choose distinct test version and verify rollback path.
4. First flash and pair the disassembled test device only to TEST Home Assistant.
5. Verify motion, occupancy timeout, illuminance, Zigbee interview, rejoin and power use.
6. Do not restart MAIN or deploy to either MAIN PIR without explicit approval.

## Build notes
Upstream README documents `make install`, `make`. The ZG-204ZL target is selected with `BOARD_ZG204ZL`; verify exact command/OTA image parameters against the upstream build setup. Presence of source files does not itself establish a successful clean build.

## Licensing
Preserve upstream LICENSE and attribution in this fork.
