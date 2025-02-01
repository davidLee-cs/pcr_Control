/*
 * hostUart.h
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#ifndef HOSTUART_H_
#define HOSTUART_H_


static void tempSet(void);
static void motorSet(void);
void hostCmd(void);
static void tempStartSet(void);
static void motorStartSet(void);
static void dacSet(void);

const char *tempSetCmd =    "$TEMP";  // 온도 설정, 시간 설정
const char *stopCmd =       "$STOP";
const char *startCmd =      "$START";
const char *motorSetCmd =   "$MOTOR";

const char *motorStartCmd = "$MSTAR";
const char *tempStartCmd =  "$TSTAR";
const char *dacSetCmd =     "$DAC";



#endif /* HOSTUART_H_ */
