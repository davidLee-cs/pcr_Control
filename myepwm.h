/*
 * epwm.h
 *
 *  Created on: 2025. 1. 25.
 *      Author: alllite
 */

#ifndef MYEPWM_H_
#define MYEPWM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//
// Defines
//
#define EPWM1_TIMER_TBPRD  4000  // pump Period register
#define EPWM1_MAX_CMPA     3000
#define EPWM1_MIN_CMPA     1000
#define EPWM1_MAX_CMPB     3000
#define EPWM1_MIN_CMPB     1000

#define EPWM2_TIMER_TBPRD  2000  // pump  Period register
#define EPWM2_MAX_CMPA     1950
#define EPWM2_MIN_CMPA       50
#define EPWM2_MAX_CMPB     1950
#define EPWM2_MIN_CMPB       50

#define EPWM3_TIMER_TBPRD  2000  // stepper Period register
#define EPWM3_MAX_CMPA      950
#define EPWM3_MIN_CMPA       50
#define EPWM3_MAX_CMPB     1950
#define EPWM3_MIN_CMPB     1050

#define EPWM4_TIMER_TBPRD  4000  // stepper Period register
#define EPWM4_MAX_CMPA     2000
#define EPWM4_MIN_CMPA     2000
#define EPWM4_MAX_CMPB     2000
#define EPWM4_MIN_CMPB     2000

#define EPWM_CMP_UP           1
#define EPWM_CMP_DOWN         0


#define PUMP_01      1
#define PUMP_23      2
#define STEP_01      3
#define STEP_23      4

//
// Globals
//
typedef struct
{
    uint32_t epwmModule;
    uint16_t epwmCompADirection;
    uint16_t epwmCompBDirection;
    uint16_t epwmTimerIntCount;
    uint16_t epwmMaxCompA;
    uint16_t epwmMinCompA;
    uint16_t epwmMaxCompB;
    uint16_t epwmMinCompB;
} epwmInfo;


extern epwmInfo epwm1Info;
extern epwmInfo epwm2Info;
extern epwmInfo epwm3Info;
extern epwmInfo epwm4Info;


//
//  Function Prototypes
//
void epwmSet(void);
void initEPWM1(void);
void initEPWM2(void);
void initEPWM3(void);
void initEPWM4(void);
__interrupt void epwm1ISR(void);
__interrupt void epwm2ISR(void);
__interrupt void epwm3ISR(void);
__interrupt void epwm4ISR(void);
void epwmEnableSet(uint16_t ch);
void epwmDisableSet(uint16_t ch);
void updateCompare(epwmInfo *epwm_info);
void stepperEpwmSet(int16_t channel, uint16_t speed);
void pumpEpwmSet(uint16_t duty);
void stepperPulseSet(int16_t channel, uint64_t pulse);
#endif /* MYEPWM_H_ */
