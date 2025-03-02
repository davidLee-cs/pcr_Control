/*
 * cmdMsg.h
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#ifndef CMDMSG_H_
#define CMDMSG_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/***** Type declarations *****/
struct TempProfile {

    int16_t targetTemp[6];
    uint16_t timeTemp[6];
    int16_t singleTargetTemp;
    int16_t lastSingleTargetTemp;   // 이전 설정 온도
    uint16_t singleTimeTemp;        // 단위: 100 mS

    uint16_t tempCycle;

};
//struct TempProfile TempProfile;

struct motorProfile {

    uint64_t set_PulseCnt;               // set pulse count
    uint16_t motorSpeed;                // 속도 55 % ~ 150%

    uint16_t pumpDuty;
};
//struct TempProfile TempProfile;


struct oprationSetBit {

    uint16_t temperatureRun;
    uint16_t motorRun;
    uint16_t motorDirection;
    uint16_t pumpRun;

};

struct dacSet {

    uint16_t dacValue;
};

struct fanSet {

    uint16_t fanEnable;

};


/***** Type declarations *****/
struct HostCmdMsg {
    struct motorProfile motorProfile;
    struct TempProfile TempProfile;
    struct oprationSetBit oprationSetBit;
    struct dacSet  dacSet;
    struct fanSet  fanSet;


    uint16_t all_Reset;                   // Reserved 0x00
    uint16_t msg2;                   // Reserved 0x00
};
extern struct HostCmdMsg HostCmdMsg[6];
//struct HostCmdMsg latestActiveHostCmdMsg;



#endif /* CMDMSG_H_ */
