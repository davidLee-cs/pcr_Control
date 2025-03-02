/*
 * hostUart.h
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#ifndef HOSTUART_H_
#define HOSTUART_H_


static void tempSet(void);
static void tempSingleSet(void);
static void motorSet(void);
static void pumpSet(void);
void hostCmd(void);
static void tempStartSet(void);
static void motorStartSet(void);
static void dacSet(void);
static void fanSet(void);



const char *tempSetCmd =            "$TEMP";  // 프로파일 온도 설정, 시간 설정
const char *tempSingleSetCmd =      "$STEMP";  // 싱글 온도 설정, 시간 설정
const char *stopCmd =               "$STOP";
const char *startCmd =              "$START";
const char *motorSetCmd =           "$MOTOR";
const char *pumpSetCmd =            "$PUMP";


const char *motorStartCmd =         "$MSTAR";
const char *tempStartCmd =          "$TSTAR";
const char *dacSetCmd =             "$DAC";
const char *fanSetCmd =             "$FAN";



#endif /* HOSTUART_H_ */
