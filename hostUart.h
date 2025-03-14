/*
 * hostUart.h
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#ifndef HOSTUART_H_
#define HOSTUART_H_


static int16_t tempSet(void);
static int16_t tempSingleSet(void);
static int16_t motorSet(void);
static void pumpSet(int16_t stop);
int16_t hostCmd(void);
static void tempStartSet(void);
static void tempOffset(void);
//int16_t motorStartSet(void);
static void dacSet(void);
static void fanSet(void);
static void home_mode(void);
static void motor_max_pulseSet(void);
static void button_status(void);
static void homing(void);
static void limite(void);
static void ppara(void);
static void mpara1(void);
static void mpapa0(void);
static void tpara1(void);
static void tpara(void);




const char *tempSetCmd =            "$TEMP";  // 프로파일 온도 설정, 시간 설정
const char *tempSingleSetCmd =      "$STEMP";  // 싱글 온도 설정, 시간 설정
const char *stopCmd =               "$STOP";
const char *startCmd =              "$START";
const char *motorSetCmd =           "$MOTOR";
const char *pumpSetCmd =            "$PUMP";
const char *tempStopCmd =           "$TSTOP";
const char *motorStartCmd =         "$MSTAR";
const char *tempStartCmd =          "$TSTAR";
//const char *dacSetCmd =             "$DAC";
const char *fanSetCmd =             "$FAN";
const char *homeCmd =               "$HOME";
//const char *readparaCmd =           "$RPARA";
const char *maxPulseCmd =           "$PMAX";

const char *tparaCmd =              "$TPARA0";
//const char *tpara1Cmd =             "$TPARA1";
const char *mpara0Cmd =             "$MPARA0";
const char *mpara1Cmd =             "$MPARA1";
const char *pparaCmd =              "$PPARA";
const char *limiteCmd =             "$LIMITE";
const char *homeSwitchCmd =         "$HSWT";
const char *buttonCmd =             "$BUTTON";


#endif /* HOSTUART_H_ */
