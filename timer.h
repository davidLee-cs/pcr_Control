/*
 * timer.h
 *
 *  Created on: 2025. 1. 20.
 *      Author: alllite
 */

#ifndef TIMER_H_
#define TIMER_H_


__interrupt void cpuTimer0ISR(void);
//__interrupt void cpuTimer1ISR(void);
//__interrupt void cpuTimer2ISR(void);
void initCPUTimers(void);
void configCPUTimer(uint32_t, float, float);
void timerSet(void);

#endif /* TIMER_H_ */
