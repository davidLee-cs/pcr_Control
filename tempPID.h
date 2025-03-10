/*
 * tempPID.h
 *
 *  Created on: 2025. 2. 15.
 *      Author: alllite
 */

#ifndef TEMPPID_H_
#define TEMPPID_H_


// DAC 변환 값 (12비트)
#define DAC_MAX_VAL 4095.0f   // 12비트 DAC 최대값 (0~4095)
#define DAC_VREF 3.3f         // DAC 기준 전압 (0~3.3V)


extern void tempPidControl(int16_t runCh, int16_t targetTemp);
extern void init_pid(void);
extern void SetOnOffControl(float32_t readNowTemp, float32_t targetTemp, int16_t ch);

#endif /* TEMPPID_H_ */
