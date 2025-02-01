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

    uint16_t targetTemp1;
    uint16_t timeTemp1;

    uint16_t targetTemp2;
    uint16_t timeTemp2;

    uint16_t targetTemp3;
    uint16_t timeTemp3;

    uint16_t targetTemp4;
    uint16_t timeTemp4;

    uint16_t targetTemp5;
    uint16_t timeTemp5;

    uint16_t tempCycle;
};
//struct TempProfile TempProfile;

struct motorProfile {

    uint16_t set_PulseCnt;               // set pulse count
    uint16_t motorSpeed;
};
//struct TempProfile TempProfile;


struct oprationSetBit {

    uint16_t temperatureRun     :1;
    uint16_t motorRun           :1;
    uint16_t motorDirection     :1;

};

struct dacSet {

    uint16_t dac1;
    uint16_t dac2;
    uint16_t dac3;
    uint16_t dac4;
    uint16_t dac5;
    uint16_t dac6;

};


/***** Type declarations *****/
struct HostCmdMsg {
    struct motorProfile     motorProfile;
    struct TempProfile      TempProfile;
    struct oprationSetBit   oprationSetBit;
    struct dacSet           dacSet;

    uint16_t                all_Reset;                   // Reserved 0x00
    uint16_t                msg2;                   // Reserved 0x00
};
extern struct HostCmdMsg HostCmdMsg;
//struct HostCmdMsg latestActiveHostCmdMsg;



#endif /* CMDMSG_H_ */
