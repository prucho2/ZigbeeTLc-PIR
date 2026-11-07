/*
 * pir_sensor.h
 *
 *  Created on: 3 апр. 2026 г.
 *      Author: pvvx
 */

#ifndef _SENSOR_PIR_H_
#define _SENSOR_PIR_H_



/*
 * Read-only diagnostics, only compiled for the development PIR board.
 * Counts are sampled GPIO transitions, not guaranteed hardware interrupts.
 * Resets on device reboot; these fields are deliberately not written to flash.
 */
#if PIR_DIAG_ENABLE && (DEV_SERVICES & SERVICE_PIR)
typedef struct {
	u32 raw_rise;
	u32 raw_fall;
	u32 occupancy_on;
	u32 occupancy_off;
	u32 timer_callbacks;
	u32 light_above_target;
	u32 report_requested;
	u8 raw_level; /* latest sampled PIR logic level, 0/1 */
} pir_diag_t;
extern volatile pir_diag_t g_pir_diag;
#endif

void task_pir(void);


#endif /* _SENSOR_PIR_H_ */
