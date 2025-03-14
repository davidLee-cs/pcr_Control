/*
 * tempPID.c
 *
 *  Created on: 2025. 2. 15.
 *      Author: alllite
 */

#include "driverlib.h"
#include "device.h"
#include "DCLF32.h"
#include "tempPID.h"
#include "config.h"

// 온도 차이 임계값 (예: 0.5°C 이내로 도달하면 온/오프 방식으로 변경)
#define TEMPERATURE_THRESHOLD   (0.3f) // 도 타겟은 +/- 0.2도 지만 0.3도에서 제어를 시킴. 테스트 결과에 따라 변경 가능
#define COOL_TEMPERATURE_THRESHOLD   (0.5f) // V
#define ONE_SHOT_DAC_VOLT       (0.2f) // V
#define COOLING_DAC_VOLT       (2.0f) //(2.0f) //(1.0f) // V
#define DAC_MAX_VOLTAGE        (2.0f) //(2.0f) //(1.5f) // V     -> 2V -> 4A
//#define COOLING_DAC_VOLT       (0.2f) // 테스트용
//#define DAC_MAX_VOLTAGE        (1.0f) // 테스트용

#define EMA_FILTER_ALPHA        (0.7f)

void SetDACOutput(float32_t pid_output, int16_t ch, float32_t temperror) ;

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
float32_t alpha = EMA_FILTER_ALPHA;
float32_t temp_threshold = TEMPERATURE_THRESHOLD;
float32_t oneShotVolt = 1.0f; // 테스트 결과로 정함.
uint16_t offMode_flag = 0;
uint16_t oneShoeOfftTime = 5;   //seconds
uint16_t oneShotTime = 2;   // seconds

float32_t level[4] = {1.0f, 0.9f, 0.8f,0.2f};
uint16_t heatLevel = 0;

float32_t temperature_error;
float32_t temperature_level;

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



void SetDACOutput(float32_t pid_output, int16_t ch, float32_t tempError)
{

    float32_t dacOut;

    if(tempError >= 0.9f)
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

//    if(tempError >= 0.9f)
//    {
//        dacOut = targetVolt * 1.0f;
//    }
//    else if((0.9f > tempError) && (tempError >= 0.8f))
//    {
//        dacOut = targetVolt * 0.75f;
//    }
//    else if((0.8f > tempError) && (tempError >= 0.7f))
//    {
//        dacOut = targetVolt * 0.35f;
//    }
//    else if(0.7f > tempError)
//    {
//        dacOut = targetVolt * 0.1f;
//    }

    // (0.0 ~ 1.0) PID 출력을 (0V ~ 3.3V)로 매핑
    float32_t dac_voltage = pid_output * dacOut;  // 0.0 ~

    // DAC 값을 12비트 정수(0 ~ 4095)로 변환
    //    Duty = (uk / 2.0f + 0.5f) * 1000;
    OpCmdMsg[ch].opDacSet.dacSet = (uint16_t)((dac_voltage / DAC_VREF) * DAC_MAX_VAL);

    // DAC 채널 출력
    dac53508_write(OpCmdMsg[ch].opDacSet.dacSet, ch);

}


void SetOnOffControl(float32_t readNowTemp, float32_t targetTemp, int16_t ch) {

    char *msg = NULL;
    float32_t error=0.0;
    temperature_error = targetTemp - readNowTemp;  // 목표 온도와 현재 온도의 차이
    temperature_level = readNowTemp / targetTemp;  // 목표 온도와 현재 온도의 차이

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

            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], targetTemp, readNowTemp, clamp_flag[ch]);  // PID 연산

            int16_t clampactive = DCL_runClamp_C1(&pid_output[ch], upperlim, lowerlim);
            clamp_flag[ch] = (clampactive == 0U) ? 1.0f : 0.0f;

            SetDACOutput(pid_output[ch], ch, temperature_level);  // DAC 출력 (PID로 제어)

            OpCmdMsg[ch].control_mode = HEAT_MODE;
        }
        else
        {
            OpCmdMsg[ch].nowTempStatus = 1;     // 온도 유지 모드 set
#if 1 // 모드 제어 1
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
#else // 모드 제어2
            OpCmdMsg[ch].control_mode = ONE_SHOT_OFF_MODE;
            offMode_flag = 0;
#endif
            cpuTimer1_OneShotCnt = 0;  // one shot 초기화
        }

        // 테스트용 무조건 가열 모드로.
//        OpCmdMsg[ch].control_mode = HEAT_MODE;

        break;
    case ONE_SHOT_MODE :

        OpCmdMsg[ch].nowTempStatus = 1;     // 온도 유지 모드 set

#if 1 // 모드 제어 1
        // 목표 온도와 차이가 작은 경우 온/오프 방식으로 제어
        if (temperature_error < temp_threshold) {
            // 목표 온도보다 온도가 더 높으면 냉각
            SetDACOutput(0.0f, ch, temperature_level);  // DAC 0V (Peltier 냉각)
        } else {
            // 목표 온도보다 온도가 더 낮으면 가열
            // one shot 모드로 최대를 pid 값을 1로 하고 전류는 최대의 1/10 으로 가열한다. 테스트 필요 상수값 결정  필요
            // oneShotVolt 과 temperature_level 의 전류 측정 후 결정
            SetDACOutput(oneShotVolt, ch, temperature_level);
        }

        OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
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

    case ONE_SHOT_OFF_MODE :

#if 1 // 방법 1, 오차 확인 후 off 유지
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

#else   // 방법 2 , 시간 off 시간만 결정
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

#endif

        break;
    case COOLING_MODE :

        if (error > COOL_TEMPERATURE_THRESHOLD)
        {
            SetDACOutput(COOLING_DAC_VOLT, ch, temperature_level);  // DAC 3.3V (Peltier 가열)
            OpCmdMsg[ch].control_mode = COOLING_MODE;
        }
        else
        {
//            OpCmdMsg[ch].nowTempStatus = 1;     // 온도 유지 모드 set
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
            offMode_flag = 0;
            cpuTimer1_OneShotCnt = 0;  // one shot 초기화
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
    default:
        break;
    }

}


void tempPidControl(int16_t runCh, int16_t targetTemp)
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
