/*
 * tempPID.h
 *
 *  Created on: 2025. 2. 15.
 *      Author: alllite
 */

#ifndef TEMPPID_H_
#define TEMPPID_H_


// DAC ��ȯ �� (12��Ʈ)
#define DAC_MAX_VAL 4095.0f   // 12��Ʈ DAC �ִ밪 (0~4095)
#define DAC_VREF 3.3f         // DAC ���� ���� (0~3.3V)


extern void tempPidControl(int16_t runCh, int16_t targetTemp);
extern void init_pid(void);
extern void SetOnOffControl(float32_t readNowTemp, float32_t targetTemp, int16_t ch);

#endif /* TEMPPID_H_ */
