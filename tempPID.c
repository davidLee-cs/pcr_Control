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
#define TEMPERATURE_THRESHOLD 50.0f //0.5f  0.5 x 100


// global  variables
long IdleLoopCount = 0;
long IsrCount = 0;
float rk = 0.25f;
float yk;
float lk;
float uk;
DCL_PID pid1 = PID_DEFAULTS;
float Duty;
float upperlim = 0.95f;
float lowerlim = 0.05f;
unsigned int clampactive;
float32_t clamp_flag;
float32_t pid_output;



void init_pid(void)
{

    /* initialise controller variables */
    pid1.Kp = 9.0f;
    pid1.Ki = 0.015f;
    pid1.Kd = 0.35f;
    pid1.Kr = 1.0f;
    pid1.c1 = 188.0296600613396f;
    pid1.c2 = 0.880296600613396f;
    pid1.d2 = 0.0f;
    pid1.d3 = 0.0f;
    pid1.i10 = 0.0f;
    pid1.i14 = 1.0f;
    pid1.Umax = 1.0f;
    pid1.Umin = 0.0f;
//    pid1.Umin = -1.0f;

    rk = 0.25f;                             // initial value for control reference
    lk = 1.0f;                              // control loop not saturated

}

uint16_t dac_value;
void SetDACOutput(float32_t pid_output, int16_t ch) {

    // (0.0 ~ 1.0) PID ����� (0V ~ 3.3V)�� ����
    float32_t dac_voltage = pid_output * DAC_VREF;  // 0.0 ~ 3.3V

    if(dac_voltage > 2.0f)
    {
        dac_voltage = 2.0f;
    }

    // DAC ���� 12��Ʈ ����(0 ~ 4095)�� ��ȯ
    //    Duty = (uk / 2.0f + 0.5f) * 1000;
    dac_value = (uint16_t)((dac_voltage / DAC_VREF) * DAC_MAX_VAL);

    // DAC ä�� ���
    dac53508_write(dac_value, ch);

}


void SetOnOffControl(float32_t readNowTemp, float32_t targetTemp, int16_t ch) {

    float32_t temperature_error = (float32_t)HostCmdMsg.TempProfile.targetTemp[ch] - readNowTemp;  // ��ǥ �µ��� ���� �µ��� ����

    if (abs(temperature_error*100) > TEMPERATURE_THRESHOLD) {
        // ��ǥ �µ��� ���̰� ū ��� ����/�ð��� PID�� ����
        pid_output = DCL_runPID_C4(&pid1, targetTemp, readNowTemp, clamp_flag);  // PID ����
        int16_t clampactive = DCL_runClamp_C1(&pid_output, upperlim, lowerlim);
        clamp_flag = (clampactive == 0U) ? 1.0f : 0.0f;

        SetDACOutput(pid_output, ch);  // DAC ��� (PID�� ����)
    } else {
        // ��ǥ �µ��� ���̰� ���� ��� ��/���� ������� ����
        if (temperature_error < 0.0f) {
            // ��ǥ �µ����� �µ��� �� ������ �ð�
            SetDACOutput(0.0f, ch);  // DAC 0V (Peltier �ð�)
        } else {
            // ��ǥ �µ����� �µ��� �� ������ ����
            SetDACOutput(1.0f, ch);  // DAC 3.3V (Peltier ����)
        }
    }
}


void tempPidControl()
{

    int16_t ch;
    float32_t readNowTemp;
    float32_t temperature_error;

    for(ch=0; ch<6; ch++)
    {
        readNowTemp = read_pr100(ch);
               // �µ� ���̰� �Ӱ谪 �����̸� ��/���� ����, �׷��� ������ PID ����
        SetOnOffControl(readNowTemp, (float32_t)HostCmdMsg.TempProfile.targetTemp[ch], ch);
        DEVICE_DELAY_US(20000);
    }

}
