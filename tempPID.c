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
#define TEMPERATURE_THRESHOLD   (0.1f) // V
#define COOL_TEMPERATURE_THRESHOLD   (0.5f) // V
#define ONE_SHOT_DAC_VOLT       (0.2f) // V
#define COOLING_DAC_VOLT       (1.0f) // V
#define DAC_MAX_VOLTAGE         (1.5f) // V

#define EMA_FILTER_ALPHA        (0.7f)

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


void SetDACOutput(float32_t pid_output, int16_t ch) {

    // (0.0 ~ 1.0) PID ����� (0V ~ 3.3V)�� ����
    float32_t dac_voltage = pid_output * DAC_VREF;  // 0.0 ~ 3.3V

    if(dac_voltage > DAC_MAX_VOLTAGE)
    {
        dac_voltage = DAC_MAX_VOLTAGE;
    }

    // DAC ���� 12��Ʈ ����(0 ~ 4095)�� ��ȯ
    //    Duty = (uk / 2.0f + 0.5f) * 1000;
    OpCmdMsg[ch].opDacSet.dacSet = (uint16_t)((dac_voltage / DAC_VREF) * DAC_MAX_VAL);

    // DAC ä�� ���
    dac53508_write(OpCmdMsg[ch].opDacSet.dacSet, ch);

}

void SetOnOffControl(float32_t readNowTemp, float32_t targetTemp, int16_t ch) {

    char *msg = NULL;
    float32_t error;
    float32_t temperature_error = targetTemp - readNowTemp;  // ��ǥ �µ��� ���� �µ��� ����

    if(temperature_error < 0.0f)
    {
        error = temperature_error * -1.0f;
    }
    else
    {
        error = temperature_error;
    }


    switch(OpCmdMsg[ch].control_mode){

    case HEAT_MODE :

        if (error >= TEMPERATURE_THRESHOLD) {

            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], targetTemp, readNowTemp, clamp_flag[ch]);  // PID ����

            int16_t clampactive = DCL_runClamp_C1(&pid_output[ch], upperlim, lowerlim);
            clamp_flag[ch] = (clampactive == 0U) ? 1.0f : 0.0f;

            SetDACOutput(pid_output[ch], ch);  // DAC ��� (PID�� ����)

            OpCmdMsg[ch].control_mode = HEAT_MODE;
        }
        else
        {
            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
        }


        break;
    case ONE_SHOT_MODE :

        if (error < TEMPERATURE_THRESHOLD) {

            OpCmdMsg[ch].nowTempStatus = 1;     // �µ� ���� ��� set

            // ��ǥ �µ��� ���̰� ���� ��� ��/���� ������� ����
            if (temperature_error < 0.0f) {
                // ��ǥ �µ����� �µ��� �� ������ �ð�
                SetDACOutput(0.0f, ch);  // DAC 0V (Peltier �ð�)
            } else {
                // ��ǥ �µ����� �µ��� �� ������ ����
                SetDACOutput(ONE_SHOT_DAC_VOLT, ch);  // DAC 3.3V (Peltier ����)
            }

            OpCmdMsg[ch].control_mode = ONE_SHOT_MODE;
        }
        else
        {
            OpCmdMsg[ch].control_mode = HEAT_MODE;
        }

        break;
    case COOLING_MODE :

        if (error > COOL_TEMPERATURE_THRESHOLD)
        {
            SetDACOutput(COOLING_DAC_VOLT, ch);  // DAC 3.3V (Peltier ����)
            OpCmdMsg[ch].control_mode = COOLING_MODE;
        }
        else
        {
            OpCmdMsg[ch].control_mode = HEAT_MODE;
        }

        break;
    default:
        break;


    }


#if 0
    if(targetTemp >= OpCmdMsg[ch].lastTargetTemp)
    {
        if (error > TEMPERATURE_THRESHOLD) {

            pid_output[ch] = DCL_runPID_C4(&pidTemp[ch], targetTemp, readNowTemp, clamp_flag[ch]);  // PID ����

            int16_t clampactive = DCL_runClamp_C1(&pid_output[ch], upperlim, lowerlim);
            clamp_flag[ch] = (clampactive == 0U) ? 1.0f : 0.0f;

            SetDACOutput(pid_output[ch], ch);  // DAC ��� (PID�� ����)

        } else {

            OpCmdMsg[ch].nowTempStatus = 1;     // �µ� ���� ��� set

            // ��ǥ �µ��� ���̰� ���� ��� ��/���� ������� ����
            if (temperature_error < 0.0f) {
                // ��ǥ �µ����� �µ��� �� ������ �ð�
                SetDACOutput(0.0f, ch);  // DAC 0V (Peltier �ð�)
            } else {
                // ��ǥ �µ����� �µ��� �� ������ ����
                SetDACOutput(ONE_SHOT_DAC_VOLT, ch);  // DAC 3.3V (Peltier ����)
            }
        }
    }
    else
    {

        if (error > TEMPERATURE_THRESHOLD)
        {
            SetDACOutput(ONE_SHOT_DAC_VOLT, ch);  // DAC 3.3V (Peltier ����)
        }

    }
#endif
}


void tempPidControl(int16_t runCh)
{

    float32_t nowTemp = read_pr100(runCh);
    //ema filter
    OpCmdMsg[runCh].tempSensor.tempSensor_Peltier  = ((1.0f - EMA_FILTER_ALPHA) * nowTemp) +
                                                     (EMA_FILTER_ALPHA * OpCmdMsg[runCh].tempSensor.tempSensor_Peltier);

//        OpCmdMsg[runCh].tempSensor.tempSensor_Metal = read_pr100(runCh);

           // �µ� ���̰� �Ӱ谪 �����̸� ��/���� ����, �׷��� ������ PID ����
    SetOnOffControl(OpCmdMsg[runCh].tempSensor.tempSensor_Peltier,
                    (float32_t)HostCmdMsg[runCh].TempProfile.singleTargetTemp,
                    runCh);


//    OpCmdMsg[runCh].lastTargetTemp = HostCmdMsg[runCh].TempProfile.singleTimeTemp;

//    DEVICE_DELAY_US(20000);


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
