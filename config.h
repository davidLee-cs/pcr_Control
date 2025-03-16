/*
 * config.h
 *
 *  Created on: 2025. 1. 14.
 *      Author: alllite
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ads1248_pt100.h"
#include "dac53508.h"
#include "drv8452.h"
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "myepwm.h"
#include "cmdMsg.h"
#include "operationlib.h"
#include "lib_mhsUart.h"
#include "timer.h"
#include "deviceRun.h"
#include "DCLF32.h"
#include "tempPID.h"
#include "xint_gpio.h"


#define PT100_CH0       0
#define PT100_CH1       1
#define PT100_CH2       2
#define PT100_CH3       3

#define TEMP_RUN    1
#define MOTOR_RUN   2
#define SET_MODE    3


#define RUN_OUT     (0) //(1) // out.
#define RUN_IN      (1) //(0) // home
#define CW          (0)
#define CCW         (1)


#define RELAY_ON_COOLING     (1)
#define RELAY_OFF_HEATING    (0)


#define PELTIER_4EA     (4)


enum {
    STOP_MODE = 0,
    HEAT_MODE,
    ONE_SHOT_MODE,
    COOLING_MODE,
    ONE_SHOT_OFF_MODE,
    COLLING_ONE_SHOT,
    HEAT_INDEX_MAX
};


void power_home_mode(void);
int16_t hostCmd(void);
extern void (*Can_State_Ptr)(void);        // 다음 수행될 모드를 가르키는 함수 포인터
extern void (*last_Can_State_Ptr)(void);        // 다음 수행될 모드를 가르키는 함수 포인터

//extern struct HostCmdMsg HostCmdMsg;

extern uint16_t jump;
extern uint16_t gSendTemp_en;
extern int16_t nowChannel;
extern uint64_t pulseCount[4];     // 현재까지 발생한 펄스 수
extern int16_t home_enable;
extern int16_t enablecheck[4];
extern int16_t pumpStop;
//extern int16_t gBoot_Rx_cnt;
extern DCL_PID pidTemp[6];


#endif /* CONFIG_H_ */
