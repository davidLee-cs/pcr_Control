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

// �µ� ���� �Ӱ谪 (��: 0.5��C �̳��� �����ϸ� ��/���� ������� ����)
#define TEMPERATURE_THRESHOLD   (0.3f) // �� Ÿ���� +/- 0.2�� ���� 0.3������ ��� ��Ŵ. �׽�Ʈ ����� ���� ���� ����
#define COOL_TEMPERATURE_THRESHOLD   (0.5f) // V
#define ONE_SHOT_DAC_VOLT       (0.2f) // V
#define COOLING_DAC_VOLT       (2.0f) //(2.0f) //(1.0f) // V
#define DAC_MAX_VOLTAGE        (2.0f) //(2.0f) //(1.5f) // V     -> 2V -> 4A
//#define COOLING_DAC_VOLT       (0.2f) // �׽�Ʈ��
//#define DAC_MAX_VOLTAGE        (1.0f) // �׽�Ʈ��

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
float32_t oneShotVolt = 1.0f; // �׽�Ʈ ����� ����.
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

    // (0.0 ~ 1.0) PID ����� (0V ~ 3.3V)�� ����
    float32_t dac_voltage = pid_output * dacOut;  // 0.0 ~

    // DAC ���� 12��Ʈ ����(0 ~ 4095)�� ��ȯ
    //    Duty = (uk / 2.0f + 0.5f) * 1000;
    OpCmdMsg[ch].opDacSet.dacSet = (uint16_t)((dac_voltage / DAC_VREF) * DAC_MAX_VAL);

    // DAC ä�� ���
    dac53508_write(OpCmdMsg[ch].opDacSet.dacSet, ch);

}


void SetOnOffControl(float32_t readNowTemp, float32_t targetTemp, int16_t ch) {

    char *msg = NULL;
    float32_t error=0.0;
    temperature_error = targetTemp - readNowTemp;  // ��ǥ �µ��� ���� �µ��� ����
    temperature_level = readNowTemp / targetTemp;  // ��ǥ �µ��� ���� �µ��� ����

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

        // �µ� ���� 0.3���� ����
        if (temperature_error >= temp_threshold) {

            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], targetTemp, readNowTemp, clamp_flag[ch]);  // PID ����

            int16_t clampactive = DCL_runClamp_C1(&pid_output[ch], upperlim, lowerlim);
            clamp_flag[ch] = (clampactive == 0U) ? 1.0f : 0.0f;

            SetDACOutput(pid_output[ch], ch, temperature_level);  // DAC ��� (PID�� ����)

            OpCmdMsg[ch].control_mode = HEAT_MODE;
        }
        else
        {
            OpCmdMsg[ch].nowTempStatus = 1;     // �µ� ���� ��� set
#if 1 // ��� ���� 1
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
#else // ��� ����2
            OpCmdMsg[ch].control_mode = ONE_SHOT_OFF_MODE;
            offMode_flag = 0;
#endif
            cpuTimer1_OneShotCnt = 0;  // one shot �ʱ�ȭ
        }

        // �׽�Ʈ�� ������ ���� ����.
//        OpCmdMsg[ch].control_mode = HEAT_MODE;

        break;
    case ONE_SHOT_MODE :

        OpCmdMsg[ch].nowTempStatus = 1;     // �µ� ���� ��� set

#if 1 // ��� ���� 1
        // ��ǥ �µ��� ���̰� ���� ��� ��/���� ������� ����
        if (temperature_error < temp_threshold) {
            // ��ǥ �µ����� �µ��� �� ������ �ð�
            SetDACOutput(0.0f, ch, temperature_level);  // DAC 0V (Peltier �ð�)
        } else {
            // ��ǥ �µ����� �µ��� �� ������ ����
            // one shot ���� �ִ븦 pid ���� 1�� �ϰ� ������ �ִ��� 1/10 ���� �����Ѵ�. �׽�Ʈ �ʿ� ����� ����  �ʿ�
            // oneShotVolt �� temperature_level �� ���� ���� �� ����
            SetDACOutput(oneShotVolt, ch, temperature_level);
        }

        OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
#else   // ��� ����2

        if(cpuTimer1_OneShotCnt < oneShotTime)  //�ð����� ���� ����
        {
            // ��ǥ �µ����� �µ��� �� ������ ����
            // one shot ���� �ִ븦 pid ���� 1�� �ϰ� ������ �ִ��� 1/10 ���� �����Ѵ�. �׽�Ʈ �ʿ� ����� ����  �ʿ�
            // oneShotVolt �� temperature_level �� ���� ���� �� ����
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

#if 1 // ��� 1, ���� Ȯ�� �� off ����
        if (temperature_error > temp_threshold)
        {
            offMode_flag = 1;  // 1ȸ Ȯ�� �� ��� ����..
            cpuTimer1_OneShotCnt = 0;
        }

        if(offMode_flag == 1)
        {
            if(cpuTimer1_OneShotCnt < oneShoeOfftTime)  // off �ð����� ���� ���� ��
            {
                SetDACOutput(0.0f, ch, temperature_level);  // DAC 0V (Peltier �ð�)
                OpCmdMsg[ch].control_mode = ONE_SHOT_OFF_MODE;
            }
            else
            {
                OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
                cpuTimer1_OneShotCnt = 0;
            }
        }

#else   // ��� 2 , �ð� off �ð��� ����
        if(cpuTimer1_OneShotCnt < oneShoeOfftTime)  // off �ð����� ���� ���� ��
        {
            SetDACOutput(0.0f, ch, temperature_level);  // DAC 0V (Peltier �ð�)
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
            SetDACOutput(COOLING_DAC_VOLT, ch, temperature_level);  // DAC 3.3V (Peltier ����)
            OpCmdMsg[ch].control_mode = COOLING_MODE;
        }
        else
        {
//            OpCmdMsg[ch].nowTempStatus = 1;     // �µ� ���� ��� set
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
            offMode_flag = 0;
            cpuTimer1_OneShotCnt = 0;  // one shot �ʱ�ȭ
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
//            if(ch == 0)            GPIO_writePin(DAC_REVERS_0, RELAY_OFF_HEATING);  // ����
//            else if(ch == 1)       GPIO_writePin(DAC_REVERS_1, RELAY_OFF_HEATING);  // ����
//            else if(ch == 2)       GPIO_writePin(DAC_REVERS_2, RELAY_OFF_HEATING);  // ����
//            else if(ch == 3)       GPIO_writePin(DAC_REVERS_3, RELAY_OFF_HEATING);  // ����
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
           // �µ� ���̰� �Ӱ谪 �����̸� ��/���� ����, �׷��� ������ PID ����
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

           // �µ� ���̰� �Ӱ谪 �����̸� ��/���� ����, �׷��� ������ PID ����
    SetOnOffControl(OpCmdMsg[runCh].tempSensor.tempSensorEma_Peltier, (float32_t)targetTemp, runCh);

#endif

}


#if 0
static int16_t ema_filter(float64_t adc, float64_t alpha, int16_t previous_ema_filtered_value)
{
    float64_t new_ema_filtered_value;

    // 1. �����̵����(EMA)
    // ���� : EMAnew=(1?��)��ADC+�᡿EMAprev
    new_ema_filtered_value  = ((1.0 - alpha) * adc) + (alpha * (float64_t)previous_ema_filtered_value);

    // 2. ���͵� �� ����
    return  (int16_t)new_ema_filtered_value;
}
#endif
