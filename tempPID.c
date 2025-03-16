/*
 * tempPID.c
 *
 *  Created on: 2025. 2. 15.
 *      Author: alllite
 *
 *
 *      온도 가열 냉각 기능 수정
 */

#include "driverlib.h"
#include "device.h"
#include "DCLF32.h"
#include "tempPID.h"
#include "config.h"

// 온도 차이 임계값 (예: 0.5°C 이내로 도달하면 온/오프 방식으로 변경)
#define TEMPERATURE_THRESHOLD   (0.2f) //(0.3f) // 도 타겟은 +/- 0.2도 지만 0.3도에서 제어를 시킴. 테스트 결과에 따라 변경 가능
#define COOL_TEMPERATURE_THRESHOLD   (0.2f) //(0.5f) // V
#define ONE_SHOT_DAC_VOLT       (0.2f) // V
#define COOLING_DAC_VOLT       (2.0f) //(2.0f) //(1.0f) // V
#define DAC_MAX_VOLTAGE        (1.2f) //(2.0f) //(1.5f) // V     -> 2V -> 4A
//#define COOLING_DAC_VOLT       (0.2f) // 테스트용
//#define DAC_MAX_VOLTAGE        (1.0f) // 테스트용

#define EMA_FILTER_ALPHA        (0.94f)  // 95도에서 0.94 설정

void SetDACOutput(float32_t pid_output, int16_t ch, float32_t tempError, float32_t targetTemp) ;

// global  variables
long IdleLoopCount = 0;
long IsrCount = 0;
float rk = 0.25f;
float yk;
float lk;
float uk;
DCL_PID pidTemp[6] = PID_DEFAULTS;
float Duty;
float upperlim = 0.95f;
float lowerlim = 0.00f;
unsigned int clampactive;
float32_t clamp_flag[6];
float32_t pid_output[6];

float32_t targetVolt = DAC_MAX_VOLTAGE;
float32_t heatStablityVolt[4] = {1.5f, 1.5f, 1.5f,1.5f};      // 95도 1.5
float32_t coolStablityVolt[4] = {0.2f, 0.2f, 0.2f,0.2f};      // 95도 1.5
float32_t alpha = EMA_FILTER_ALPHA;
float32_t temp_threshold = TEMPERATURE_THRESHOLD;
float32_t cooling_temp_threshold = COOL_TEMPERATURE_THRESHOLD;

float32_t oneShotVolt = 0.83f; // 테스트 결과로 정함.
uint16_t offMode_flag = 0;
uint16_t oneShoeOfftTime = 5;   //seconds
uint16_t oneShotTime = 2;   // seconds

float32_t level[4] = {1.0f, 1.0f, 0.9f,0.9f};
uint16_t heatLevel = 0;
uint16_t debug0=0;
float32_t temperature_error;
float32_t temperature_level;

// 히팅 91.08000004ㅊ  //쿨링 0.1       // 70 도 : 0.699999988(상온 24도), 0.64(상온 26도)
// 1.02(상온 26도)  , 30- 0.1,(상온26도)
float32_t dacV[4]= {0.8f, 0.8f, 0.8f,0.8f}; // 히팅 1.08000004ㅊ  //쿨링 0.1       // 70 도 : 0.699999988
float32_t dacHeat = 1.1f;
float32_t dacCool = 0.15f;
int16_t tempCheck10msCnt[4] = {0,};
uint16_t gNowStabilityCnt[4] = {0,};




void init_pid(void)
{

    int16_t ch;

    for(ch = 0; ch<6; ch++)
    {
        /* initialise controller variables */
        pidTemp[ch].Kp = 2.0f;
        pidTemp[ch].Ki = 0.015f;
        pidTemp[ch].Kd = 0.35f;
        pidTemp[ch].Kr = 1.0f;
        pidTemp[ch].c1 = 188.0296600613396f;
        pidTemp[ch].c2 = 0.880296600613396f;
        pidTemp[ch].d2 = 0.0f;
        pidTemp[ch].d3 = 0.0f;
        pidTemp[ch].i10 = 0.0f;
        pidTemp[ch].i14 = 1.0f;
        pidTemp[ch].Umax = 1.0f;
        pidTemp[ch].Umin = 0.0f;
    //    pid1.Umin = -1.0f;

    }

    rk = 0.25f;                             // initial value for control reference
    lk = 1.0f;                              // control loop not saturated

    upperlim = 0.95f;
    lowerlim = 0.00f;
}


void SetDACOutput(float32_t pid_output, int16_t ch, float32_t tempError, float32_t targetTemp)
{

#if 0
    float32_t dacOut;

    if(OpCmdMsg[ch].control_mode == ONE_SHOT_MODE)
    {
        dacOut = stablityVolt * level[3];
        heatLevel = 5;

    }
    else
    {
        if(tempError >= 0.99f)
        {
            dacOut = targetVolt * level[3];
            heatLevel = 4;
        }
        else if((0.9f > tempError) && (tempError >= 0.8f))
        {
            dacOut = targetVolt * level[2];
            heatLevel = 3;
        }
        else if((0.8f > tempError) && (tempError >= 0.7f))
        {
            dacOut = targetVolt * level[1];
            heatLevel = 2;
        }
        else if(0.7f > tempError)
        {
            dacOut = targetVolt * level[0];
            heatLevel = 1;
        }
    }
#endif

#if 1

    float32_t dacOut = pid_output;
    float32_t dac_voltage;

    if(tempError < 0)
    {
        tempError = tempError * -1.0f;
    }

    if(tempError >= 0.96f)
    {
//            targetVolt = 1.0f;
//        dacOut = stablityVolt * level[3];


//        float32_t errorblock = targetTemp - OpCmdMsg[ch].tempSensor.tempSensorEma_Block;
//        if((0.2f < errorblock ) && (errorblock < -0.2f))
//        {
//            dac_voltage = stablityVolt[ch]; //dacV[ch];  // 0.0 ~ 1.04999995
//        }
//        else
        {
            dacOut = targetVolt * level[3];
            heatLevel = 4;
            dac_voltage = pid_output * dacOut;  // 0.0 ~
        }

    }
    else if((0.96f > tempError) && (tempError >= 0.85f))
    {
        dacOut = targetVolt * level[2];
        heatLevel = 3;
        dac_voltage = dacOut;  // 0.0 ~
    }
    else if((0.85f > tempError) && (tempError >= 0.7f))
    {
        dacOut = targetVolt * level[1];
        heatLevel = 2;
        dac_voltage = dacOut;  // 0.0 ~
    }
    else if(0.7f > tempError)
    {
        dacOut = targetVolt * level[0];
        heatLevel = 1;
        // (0.0 ~ 1.0) PID 출력을 (0V ~ 3.3V)로 매핑
        dac_voltage = dacOut;  // 0.0 ~

    }

//    // (0.0 ~ 1.0) PID 출력을 (0V ~ 3.3V)로 매핑
//    dac_voltage = pid_output * dacOut;  // 0.0 ~


    if((OpCmdMsg[ch].control_mode == ONE_SHOT_MODE) || (OpCmdMsg[ch].control_mode == COLLING_ONE_SHOT))
    {
        // pid 5,  dacV = 1.0,
        // (0.0 ~ 1.0) PID 출력을 (0V ~ 3.3V)로 매핑
//        dac_voltage = 1 * dacV;  // 0.0 ~ 0.68

        float32_t errorblock = targetTemp - OpCmdMsg[ch].tempSensor.tempSensorEma_Block;
        if(OpCmdMsg[ch].control_mode == COLLING_ONE_SHOT)
        {
            if((0.2f < errorblock ) || (errorblock < -0.2f))
            {
//                if(OpCmdMsg[ch].control_mode == ONE_SHOT_MODE)   dac_voltage = heatStablityVolt[ch]; //dacV[ch];  // 0.0 ~ 1.04999995
//                else                                                         dac_voltage = coolStablityVolt[ch];
                dac_voltage = coolStablityVolt[ch];
            }
            else
            {
                dac_voltage = pid_output * dacV[ch];  // 0.0 ~ 1.04999995
            }
        }
        else
        {
            dac_voltage = pid_output * dacV[ch];  // 0.0 ~ 1.04999995
        }

    }
#endif


    //
    // (0.0 ~ 1.0) PID 출력을 (0V ~ 3.3V)로 매핑
//    float32_t dac_voltage = pid_output * dacOut;  // 0.0 ~

    // DAC 값을 12비트 정수(0 ~ 4095)로 변환
    //    Duty = (uk / 2.0f + 0.5f) * 1000;
    OpCmdMsg[ch].opDacSet.dacSet = (uint16_t)((dac_voltage / DAC_VREF) * DAC_MAX_VAL);

    // DAC 채널 출력
    dac53508_write(OpCmdMsg[ch].opDacSet.dacSet, ch);

}

void SetOnOffControl(float32_t readNowTemp, float32_t targetTemp, int16_t ch) {

    char *msg = NULL;
    float32_t error=0.0;
    int16_t clampactive;
    float32_t offset;

    temperature_error = targetTemp - readNowTemp;  // + : 현재값이 낮다. - : 현재값이 높다

    if(temperature_error >=0)     temperature_level = readNowTemp / targetTemp;  // 목표 온도와 현재 온도의 차이
    else                          temperature_level = targetTemp / readNowTemp;  // 목표 온도와 현재 온도의 차이

    if(temperature_error < 0.0f)
    {
        error = (-temperature_error);
    }
    else
    {
        error = temperature_error;
    }

    switch(OpCmdMsg[ch].control_mode){

    case HEAT_MODE :

        // 온도 오차 0.3으로 설정
        if (temperature_error >= temp_threshold) {

            pidTemp[ch].Kp = 20.0f;
            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], targetTemp, readNowTemp, clamp_flag[ch]);  // PID 연산

            clampactive = DCL_runClamp_C1(&pid_output[ch], upperlim, lowerlim);
            clamp_flag[ch] = (clampactive == 0U) ? 1.0f : 0.0f;

            SetDACOutput(pid_output[ch], ch, temperature_level, targetTemp);  // DAC 출력 (PID로 제어)

            OpCmdMsg[ch].control_mode = HEAT_MODE;
            OpCmdMsg[ch].nowTempStatus = 0;     // 온도 유지 모드 set
        }
        else
        {
            dacV[ch]= 1.08f;
            pidTemp[ch].Kp = 5.0f;
            OpCmdMsg[ch].nowTempStatus = 1;     // 온도 유지 모드 set
#if 1 // 모드 제어 1
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
#else // 모드 제어2
            OpCmdMsg[ch].control_mode = ONE_SHOT_OFF_MODE;
            offMode_flag = 0;
#endif
            cpuTimer1_OneShotCnt = 0;  // one shot 초기화

#if 0
            // 테스트용
            OpCmdMsg[ch].control_mode = STOP_MODE;
            dac53508_write(0, ch);
#endif
        }

        // 테스트용 무조건 가열 모드로.
//        OpCmdMsg[ch].control_mode = HEAT_MODE;

        break;
    case ONE_SHOT_MODE :
        debug0 = 1;

        OpCmdMsg[ch].nowTempStatus = 1;     // 온도 유지 모드 set

#if 1 // 모드 제어 1

//        if((temp_threshold >= temperature_error) && (temperature_error > -temp_threshold))
//        {
//            dacV = 1.08f;
//        }
//        else
//        {
//            dacV = 1.1f;
//        }

//        if (temperature_error >= temp_threshold + 0.5f) // 가열
//        {
//            dac53508_write(0, ch);
//            DEVICE_DELAY_US(10);
//
//            if(ch == 0)            GPIO_writePin(DAC_REVERS_0, RELAY_OFF_HEATING);  // 전류 쿨링
//            else if(ch == 1)       GPIO_writePin(DAC_REVERS_1, RELAY_OFF_HEATING);  // 전류 쿨링
//            else if(ch == 2)       GPIO_writePin(DAC_REVERS_2, RELAY_OFF_HEATING);  // 전류 쿨링
//            else if(ch == 3)       GPIO_writePin(DAC_REVERS_3, RELAY_OFF_HEATING);  // 전류 쿨링
//            else
//            {
//                DEVICE_DELAY_US(1);
//            }
//            DEVICE_DELAY_US(100);
//            heatLevel  = 5;
////            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], targetTemp, readNowTemp, clamp_flag[ch]);  // PID 연산
////
////            int16_t clampactive = DCL_runClamp_C1(&pid_output[ch], upperlim, lowerlim);
////            clamp_flag[ch] = (clampactive == 0U) ? 1.0f : 0.0f;
////
////            SetDACOutput(pid_output[ch], ch, temperature_level);  // DAC 출력 (PID로 제어)
//
//    //        SetDACOutput(oneShotVolt, ch, temperature_level);
//
////            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
//        }
//        if(temperature_error >= 0)
//        {
//            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], targetTemp, readNowTemp, clamp_flag[ch]);  // PID 연산
//        }
//        else
//        {
//            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], readNowTemp, targetTemp, clamp_flag[ch]);  // PID 연산
//        }
        pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], targetTemp, readNowTemp, clamp_flag[ch]);  // PID 연산
        int16_t clampactive = DCL_runClamp_C1(&pid_output[ch], upperlim, lowerlim);
        clamp_flag[ch] = (clampactive == 0U) ? 1.0f : 0.0f;

        SetDACOutput(pid_output[ch], ch, temperature_level, targetTemp);  // DAC 출력 (PID로 제어)


        if( OpCmdMsg[ch].nowcontrol_mode == HEAT_MODE)
        {
            offset = 0.5f;
        }
        else
        {
            offset = 0.3f;
        }

        if(temperature_error < (temp_threshold + offset) * -1.0f)
        {
            dac53508_write(0, ch);
            DEVICE_DELAY_US(10);


            if(ch == 0)            GPIO_writePin(DAC_REVERS_0, RELAY_ON_COOLING);  // 전류 쿨링
            else if(ch == 1)       GPIO_writePin(DAC_REVERS_1, RELAY_ON_COOLING);  // 전류 쿨링
            else if(ch == 2)       GPIO_writePin(DAC_REVERS_2, RELAY_ON_COOLING);  // 전류 쿨링
            else if(ch == 3)       GPIO_writePin(DAC_REVERS_3, RELAY_ON_COOLING);  // 전류 쿨링
            else
            {
                DEVICE_DELAY_US(1);
            }
            //DEVICE_DELAY_US(100);
            heatLevel = 6;
            tempCheck10msCnt[ch] = -500;
            gNowStabilityCnt[ch] = 1;
            OpCmdMsg[ch].control_mode = COLLING_ONE_SHOT;
        }
        else
        {
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;

        }


#else   // 모드 제어2

        if(cpuTimer1_OneShotCnt < oneShotTime)  //시간동안 전류 공급
        {
            // 목표 온도보다 온도가 더 낮으면 가열
            // one shot 모드로 최대를 pid 값을 1로 하고 전류는 최대의 1/10 으로 가열한다. 테스트 필요 상수값 결정  필요
            // oneShotVolt 과 temperature_level 의 전류 측정 후 결정
            SetDACOutput(oneShotVolt, ch, temperature_level);
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
        }
        else
        {
            OpCmdMsg[ch].control_mode = ONE_SHOT_OFF_MODE;
            cpuTimer1_OneShotCnt = 0;
            offMode_flag = 0;
        }
#endif


        break;
    case ONE_SHOT_OFF_MODE :

#if 0 // 방법 1, 오차 확인 후 off 유지
        if (temperature_error > temp_threshold)
        {
            offMode_flag = 1;  // 1회 확인 후 계속 유지..
            cpuTimer1_OneShotCnt = 0;
        }

        if(offMode_flag == 1)
        {
            if(cpuTimer1_OneShotCnt < oneShoeOfftTime)  // off 시간동안 유지 단위 초
            {
                SetDACOutput(0.0f, ch, temperature_level);  // DAC 0V (Peltier 냉각)
                OpCmdMsg[ch].control_mode = ONE_SHOT_OFF_MODE;
            }
            else
            {
                OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
                cpuTimer1_OneShotCnt = 0;
            }
        }

#else   // 모드 2 on off  방법 2 , 시간 off 시간만 결정
        if(cpuTimer1_OneShotCnt < oneShoeOfftTime)  // off 시간동안 유지 단위 초
        {
            SetDACOutput(0.0f, ch, temperature_level, targetTemp);  // DAC 0V (Peltier 냉각)
            OpCmdMsg[ch].control_mode = ONE_SHOT_OFF_MODE;
        }
        else
        {
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
            cpuTimer1_OneShotCnt = 0;
        }

#endif

        break;
    case COOLING_MODE :


        // 에러가 - 면 현재값이 타넷값보다 높다.
        if (error > COOL_TEMPERATURE_THRESHOLD)
        {
            pidTemp[ch].Kp = 20.0f;
            // 타겟, 현재값 위치 변경 - 쿨링모드에서
//            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], readNowTemp, targetTemp, clamp_flag[ch]);  // PID 연산
            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], targetTemp, readNowTemp, clamp_flag[ch]);  // PID 연산

            clampactive = DCL_runClamp_C1(&pid_output[ch], upperlim, lowerlim);
            clamp_flag[ch] = (clampactive == 0U) ? 1.0f : 0.0f;
            SetDACOutput(COOLING_DAC_VOLT, ch, temperature_level, targetTemp);  // DAC 3.3V (Peltier 가열)
            OpCmdMsg[ch].control_mode = COOLING_MODE;

            OpCmdMsg[ch].nowTempStatus = 0;

        }
        else
        {
            dacV[ch] = 0.1f;
            pidTemp[ch].Kp = 5.0f;
            tempCheck10msCnt[ch] = -10000;
            gNowStabilityCnt[ch] = 1;
            OpCmdMsg[ch].nowTempStatus = 1;     // 온도 유지 모드 set
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;  // 추가 하락을 막기위해서 먼저 가열 모드로 보냄
        }
//        else
//        {
//
//            dac53508_write(0, ch);
//            DEVICE_DELAY_US(100000);
//
//            OpCmdMsg[ch].control_mode = HEAT_MODE;
////            OpCmdMsg[ch].control_mode = STOP_MODE;
//
//            if(ch == 0)            GPIO_writePin(DAC_REVERS_0, RELAY_OFF_HEATING);  // 가열
//            else if(ch == 1)       GPIO_writePin(DAC_REVERS_1, RELAY_OFF_HEATING);  // 가열
//            else if(ch == 2)       GPIO_writePin(DAC_REVERS_2, RELAY_OFF_HEATING);  // 가열
//            else if(ch == 3)       GPIO_writePin(DAC_REVERS_3, RELAY_OFF_HEATING);  // 가열
//            else
//            {
//                DEVICE_DELAY_US(10000);
//            }
//
//            DEVICE_DELAY_US(10000);
////            DEVICE_DELAY_US(500000);
////            DEVICE_DELAY_US(500000);
////            DEVICE_DELAY_US(500000);
//        }

        break;
    case COLLING_ONE_SHOT :

        debug0 = 2;

        if((temp_threshold >= temperature_error) && (temperature_error > -temp_threshold))
        {
            if( OpCmdMsg[ch].tempSensor.tempSensorEma_Block ==  OpCmdMsg[ch].tempSensor.lasttempSensorEma_Block)
            {
                gNowStabilityCnt[ch]++;
            }

        }

        float32_t errorblock = targetTemp - OpCmdMsg[ch].tempSensor.tempSensorEma_Block;
        if((0.2f < errorblock ) || (errorblock < -0.2f))
        {
            if(gNowStabilityCnt[ch] >= 5)
            {
                gNowStabilityCnt[ch] = 0;

//                if(gNowStabilityCnt[ch] > 500)
                {
                    gNowStabilityCnt[ch] = 0;
                    tempCheck10msCnt[ch] = 0;

                    if(temperature_error > 0.2f)
                    {
                        dacV[ch] = dacV[ch] + 0.01f; //0.1f;
                    }
                    else if(temperature_error < -0.2f)
                    {
                        dacV[ch] = dacV[ch] - 0.01f;// //1.0f;
                    }
                }
            }
        }
//

//        }
        // 타겟, 현재값 위치 변경 - 쿨링 모드
//        pid_output[ch] = DCL_runPID_C4(&pidTemp[ch],  targetTemp, readNowTemp, clamp_flag[ch]);  // PID 연산
        pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], readNowTemp, targetTemp, clamp_flag[ch]);  // PID 연산 변대 지정

        clampactive = DCL_runClamp_C1(&pid_output[ch], upperlim, lowerlim);
        clamp_flag[ch] = (clampactive == 0U) ? 1.0f : 0.0f;

        SetDACOutput(pid_output[ch], ch, temperature_level, targetTemp);  // DAC 출력 (PID로 제어)

//        SetDACOutput(oneShotVolt, ch, temperature_level);

        if( OpCmdMsg[ch].nowcontrol_mode == HEAT_MODE)
        {
            offset = 0.5f;
        }
        else
        {
            offset = 0.3f;
        }

        if (temperature_error >= temp_threshold + offset) // 가열
        {
            dac53508_write(0, ch);
            DEVICE_DELAY_US(10);

            if(ch == 0)            GPIO_writePin(DAC_REVERS_0, RELAY_OFF_HEATING);  // 전류 쿨링
            else if(ch == 1)       GPIO_writePin(DAC_REVERS_1, RELAY_OFF_HEATING);  // 전류 쿨링
            else if(ch == 2)       GPIO_writePin(DAC_REVERS_2, RELAY_OFF_HEATING);  // 전류 쿨링
            else if(ch == 3)       GPIO_writePin(DAC_REVERS_3, RELAY_OFF_HEATING);  // 전류 쿨링
            else
            {
                DEVICE_DELAY_US(1);
            }
            //DEVICE_DELAY_US(100);
            heatLevel  = 5;
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;

        }
        else
        {
            OpCmdMsg[ch].control_mode = COLLING_ONE_SHOT;

        }


        break;
    default:
        break;
    }
     OpCmdMsg[ch].tempSensor.lasttempSensorEma_Block =  OpCmdMsg[ch].tempSensor.tempSensorEma_Block;
}


void tempPidControl(int16_t runCh, float32_t targetTemp)
{

#if 0
//    float32_t nowTemp = read_pr100(0, runCh);
    select_Channel(-1, runCh);
    OpCmdMsg[runCh].tempSensor.nowTemp_S1 = read_pr100(0, runCh);
    OpCmdMsg[runCh].tempSensor.nowTemp_S2 = read_pr100(1, runCh);
           // 온도 차이가 임계값 이하이면 온/오프 제어, 그렇지 않으면 PID 제어
    SetOnOffControl( OpCmdMsg[runCh].tempSensor.nowTemp_S1, (float32_t)targetTemp, runCh);

#else

//    float32_t nowTemp = read_pr100(0, runCh);
//    select_Channel(-1, runCh);
    OpCmdMsg[runCh].tempSensor.nowTemp_S1 = read_pr100(0, runCh);
    OpCmdMsg[runCh].tempSensor.nowTemp_S2 = read_pr100(1, runCh);


    //ema filter
    OpCmdMsg[runCh].tempSensor.tempSensorEma_Peltier  = ((1.0f - alpha) * OpCmdMsg[runCh].tempSensor.nowTemp_S1) +
                                                     (alpha * OpCmdMsg[runCh].tempSensor.tempSensorEma_Peltier);


    OpCmdMsg[runCh].tempSensor.tempSensorEma_Block  = ((1.0f - alpha) * OpCmdMsg[runCh].tempSensor.nowTemp_S2) +
                                                     (alpha * OpCmdMsg[runCh].tempSensor.tempSensorEma_Block);

//        OpCmdMsg[runCh].tempSensor.tempSensor_Metal = read_pr100(runCh);

           // 온도 차이가 임계값 이하이면 온/오프 제어, 그렇지 않으면 PID 제어
    SetOnOffControl(OpCmdMsg[runCh].tempSensor.tempSensorEma_Peltier, (float32_t)targetTemp, runCh);

#endif

}


#if 0
static int16_t ema_filter(float64_t adc, float64_t alpha, int16_t previous_ema_filtered_value)
{
    float64_t new_ema_filtered_value;

    // 1. 지수이동평균(EMA)
    // 수식 : EMAnew=(1?α)×ADC+α×EMAprev
    new_ema_filtered_value  = ((1.0 - alpha) * adc) + (alpha * (float64_t)previous_ema_filtered_value);

    // 2. 필터된 값 리턴
    return  (int16_t)new_ema_filtered_value;
}
#endif
