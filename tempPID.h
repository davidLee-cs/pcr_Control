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


extern void tempPidControl();


#endif /* TEMPPID_H_ */
