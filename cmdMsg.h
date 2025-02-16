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
    uint16_t singleTimeTemp;

    uint16_t tempCycle;

};
//struct TempProfile TempProfile;

struct motorProfile {

    uint64_t set_PulseCnt;               // set pulse count
    uint16_t motorSpeed;                // �ӵ� 55 % ~ 150%
};
//struct TempProfile TempProfile;


struct oprationSetBit {

    uint16_t temperatureRun;
    uint16_t motorRun;
    uint16_t motorDirection;

};

struct dacSet {

    uint16_t dacValue;
//    uint16_t dac2;
//    uint16_t dac3;
//    uint16_t dac4;
//    uint16_t dac5;
//    uint16_t dac6;

};


/***** Type declarations *****/
struct HostCmdMsg {
    struct motorProfile motorProfile;
    struct TempProfile TempProfile;
    struct oprationSetBit oprationSetBit;
    struct dacSet  dacSet;

    uint16_t all_Reset;                   // Reserved 0x00
    uint16_t msg2;                   // Reserved 0x00
};
extern struct HostCmdMsg HostCmdMsg[6];
//struct HostCmdMsg latestActiveHostCmdMsg;



#endif /* CMDMSG_H_ */
