/*
 * deviceRun.h
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#ifndef DEVICERUN_H_
#define DEVICERUN_H_



void temp_mode(void);
void motor_mode(void);
void idle_mode(void);
void motor_Parameterset(int16_t channel);
void pump_Parameterset(int16_t channel);
void stop_mode(void);
void prameterInit(void);
void fan_control(int16_t heatFan);
void fan_AllOff(void);
void fan_AllOn(void);
void switchRead(void);

#endif /* DEVICERUN_H_ */
