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

    uint16_t motor1_Home_bit      :1;    //25 -1
    uint16_t motor1_End_bit     :1;    //25 -1

    uint16_t motor2_Home_bit      :1;    //25 -1
    uint16_t motor2_End_bit     :1;    //25 -1

    uint16_t motor3_Home_bit      :1;    //25 -1
    uint16_t motor3_End_bit     :1;    //25 -1

    uint16_t motor4_Home_bit      :1;    //25 -1
    uint16_t motor4_End_bit     :1;    //25 -1

};
//struct motorMovingStatus motorMovingStatus;


struct tempSensor{

    uint16_t tempSensor1_A;
    uint16_t tempSensor1_B;
    uint16_t tempSensor2_A;
    uint16_t tempSensor2_B;

    uint16_t tempSensor3_A;
    uint16_t tempSensor3_B;
    uint16_t tempSensor4_A;
    uint16_t tempSensor4_B;

};


struct opDacSet{

    uint16_t dacSet_1;
    uint16_t dacSet_2;
    uint16_t dacSet_3;
    uint16_t dacSet_4;
    uint16_t dacSet_5;
    uint16_t dacSet_6;

};


/***** Type declarations *****/
struct OpCmdMsg {

    struct motorMovingStatus    motorMovingStatus;
    struct tempSensor           tempSensor;
    struct opDacSet             opDacSet;

};
extern struct OpCmdMsg OpCmdMsg;
//struct OpCmdMsg OpCmdMsgServoMax[4];
//struct OpCmdMsg* lastAddedMaxOpCmdMsg = OpCmdMsgServoMax;


#endif /* OPERATIONLIB_H_ */
