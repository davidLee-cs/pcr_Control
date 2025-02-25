/*
 * operationlib.h
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#ifndef OPERATIONLIB_H_
#define OPERATIONLIB_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


struct motorMovingStatus{

    uint16_t motor_Home;    //25 -1
    uint16_t motor_End;    //25 -1

//    uint16_t motor2_Home_bit      :1;    //25 -1
//    uint16_t motor2_End_bit     :1;    //25 -1
//
//    uint16_t motor3_Home_bit      :1;    //25 -1
//    uint16_t motor3_End_bit     :1;    //25 -1
//
//    uint16_t motor4_Home_bit      :1;    //25 -1
//    uint16_t motor4_End_bit     :1;    //25 -1

};
//struct motorMovingStatus lastmotorMovingStatus;


struct tempSensor{

    float32_t tempSensor_Peltier;      //paltier
    float32_t tempSensor_Metal;      //case
//    uint16_t tempSensor2_A;
//    uint16_t tempSensor2_B;
//
//    uint16_t tempSensor3_A;
//    uint16_t tempSensor3_B;
//    uint16_t tempSensor4_A;
//    uint16_t tempSensor4_B;

};


struct opDacSet{

    uint16_t dacSet;        // target DAC
//    uint16_t dacSet_2;
//    uint16_t dacSet_3;
//    uint16_t dacSet_4;
//    uint16_t dacSet_5;
//    uint16_t dacSet_6;

};


struct opFanSet{

    uint16_t fanEnable;        // target DAC

};


/***** Type declarations *****/
struct OpCmdMsg {

    struct motorMovingStatus    motorMovingStatus;
    struct tempSensor           tempSensor;
    struct opDacSet             opDacSet;
    struct opFanSet             opFanSet;

    uint16_t nowTempStatus;     // 1 : 온도 제어 모드
    uint64_t stepperPulseCnt;
    int16_t  lastTargetTemp;   // 이전 설정 온도
    uint16_t control_mode;
};
extern struct OpCmdMsg OpCmdMsg[6];
//struct OpCmdMsg OpCmdMsgServoMax[4];
//struct OpCmdMsg* lastAddedMaxOpCmdMsg = OpCmdMsgServoMax;


#endif /* OPERATIONLIB_H_ */
