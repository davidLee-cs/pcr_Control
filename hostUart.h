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
void hostCmd(void);
static void tempStartSet(void);
static void tempOffset(void);
static int16_t motorStartSet(void);
static void dacSet(void);
static void fanSet(void);
static void home_mode(void);
static void parameter_read_mode(void);

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
const char *homeCmd =               "$HOME";
const char *readparaCmd =           "$RPARA";


#endif /* HOSTUART_H_ */
