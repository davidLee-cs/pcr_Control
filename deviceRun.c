/*
 * deviceRun.c
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#include "config.h"


#define PELTIER_4EA     4


void tempProfileReset(void);


void temp_mode(void)
{
    char *msg = NULL;
    int16_t ch;

    for(ch=0; ch< PELTIER_4EA; ch++)
    {
        // 채널별 run 실쟁과 동작 시간(단위 100ms)이 0 이상 설정 되었을 때만 동작 함.
        if((HostCmdMsg[ch].oprationSetBit.temperatureRun == 1) && (HostCmdMsg[ch].TempProfile.singleTimeTemp > 0))
        {

            tempPidControl(ch);

            // 첫번째 목표 온도에 도달하면 카운터 시작
            // 시간만큼 도달 전에는 nowTempStatus 상태 그대로 1로 유지
            if(OpCmdMsg[ch].nowTempStatus == 1)     // 목표 온도에 도달했다. 온도 유지 모드
            {
                if(tempProfileCnt[ch] > HostCmdMsg[ch].TempProfile.singleTimeTemp)
                {
                    sprintf(msg,"$ATEMP,%d,%d\r\n", ch, tempCycleCnt[ch] );
                    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));

                    if(++tempCycleCnt[ch] >= HostCmdMsg[ch].TempProfile.tempCycle)
                    {
                        sprintf(msg,"$CYCLE,%d, %d\r\n", ch, tempCycleCnt[ch] );
                        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));

                        tempCycleCnt[ch] = 0;
                        HostCmdMsg[ch].oprationSetBit.temperatureRun = 0;
                        stop_mode();
                    }

                    DEVICE_DELAY_US(20000);
                    tempProfileCnt[ch] = 0;
                }
            }
            else
            {
                tempProfileCnt[ch] = 0;

            }
        }
        else
        {
            dac53508_write(0, ch);

        }

        DEVICE_DELAY_US(2000);
    }

}


void motor_mode(void)
{

    if(HostCmdMsg[nowChannel].oprationSetBit.motorRun == 1)
    {

        if(HostCmdMsg[nowChannel].oprationSetBit.motorDirection == 1)
        {
            SetMotorDirection(1);
        }
        else
        {
            SetMotorDirection(0);
        }

        EnableMotor(1);

    }
    else
    {
        EnableMotor(0);
        epwmDisableSet(STEP_23);
    }

    uint16_t data = drv8452_read();  //

    Can_State_Ptr = &hostCmd;///normal mode
}

void motor_Parameterset(int16_t channel)
{

//    epwmEnableSet(STEP_23);
    stepperEpwmSet(channel, HostCmdMsg[channel].motorProfile.motorSpeed);
    stepperPulseSet(channel, HostCmdMsg[channel].motorProfile.set_PulseCnt);

}


void tempProfileReset(void)
{
    int16_t channel;

    for(channel=0; channel < 6; channel++)
    {
        HostCmdMsg[channel].TempProfile.targetTemp[0] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[1] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[2] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[3] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[4] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[5] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[0] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[1] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[2] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[3] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[4] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[5] = 0 ;

        HostCmdMsg[channel].TempProfile.tempCycle = 0 ;

        HostCmdMsg[channel].motorProfile.motorSpeed = 0;
        HostCmdMsg[channel].motorProfile.set_PulseCnt = 0;
    }
}

void prameterInit(void)
{

    int16_t channel;

    for(channel=0; channel < 6; channel++)
    {
        HostCmdMsg[channel].TempProfile.targetTemp[0] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[1] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[2] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[3] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[4] = 0 ;
        HostCmdMsg[channel].TempProfile.targetTemp[5] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[0] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[1] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[2] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[3] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[4] = 0 ;
        HostCmdMsg[channel].TempProfile.timeTemp[5] = 0 ;
        HostCmdMsg[channel].TempProfile.tempCycle = 0 ;

        HostCmdMsg[channel].motorProfile.motorSpeed = 0;
        HostCmdMsg[channel].motorProfile.set_PulseCnt = 0;

        HostCmdMsg[channel].oprationSetBit.motorDirection = 0;
        HostCmdMsg[channel].oprationSetBit.motorRun = 0;
        HostCmdMsg[channel].oprationSetBit.temperatureRun = 0;

        HostCmdMsg[channel].dacSet.dacValue = 0;

        OpCmdMsg[channel].motorMovingStatus.motor_End = 0;
        OpCmdMsg[channel].motorMovingStatus.motor_Home = 0;

        OpCmdMsg[channel].opDacSet.dacSet = 0;

        OpCmdMsg[channel].tempSensor.tempSensor_Peltier = 0;
        OpCmdMsg[channel].tempSensor.tempSensor_Metal = 0;

        OpCmdMsg[channel].nowTempStatus = 0;
        OpCmdMsg[channel].stepperPulseCnt = 0;
    }
}

void stop_mode(void)
{

    EnableMotor(0);
    epwmDisableSet(STEP_23);
//    dac53508_write(0);

    prameterInit();
    fan_AllOff();

    Can_State_Ptr = &hostCmd;///normal mode

}

void fan_AllOff(void)
{
    GPIO_writePin(FAN_0, 0);
    GPIO_writePin(FAN_1, 0);
    GPIO_writePin(FAN_2, 0);
    GPIO_writePin(FAN_3, 0);
    GPIO_writePin(FAN_4, 0);
}

void fan_AllOn(void)
{
    GPIO_writePin(FAN_0, 1);
    GPIO_writePin(FAN_1, 1);
    GPIO_writePin(FAN_2, 1);
    GPIO_writePin(FAN_3, 1);
    GPIO_writePin(FAN_4, 1);
}

void fan_control(int16_t heatFan)
{

    if(heatFan == 1)
    {
        GPIO_writePin(FAN_4, 1);
    }
    else
    {
        GPIO_writePin(FAN_4, 0);
    }


    if(HostCmdMsg[0].fanSet.fanEnable == 1)
    {
        GPIO_writePin(FAN_0, 1);
    }
    else
    {
        GPIO_writePin(FAN_0, 0);
    }

    if(HostCmdMsg[1].fanSet.fanEnable == 1)
    {
        GPIO_writePin(FAN_1, 1);
    }
    else
    {
        GPIO_writePin(FAN_1, 0);
    }

    if(HostCmdMsg[2].fanSet.fanEnable == 1)
    {
        GPIO_writePin(FAN_2, 1);
    }
    else
    {
        GPIO_writePin(FAN_2, 0);
    }

    if(HostCmdMsg[3].fanSet.fanEnable == 1)
    {
        GPIO_writePin(FAN_3, 1);
    }
    else
    {
        GPIO_writePin(FAN_3, 0);
    }


}

void idle_mode(void)
{

    Can_State_Ptr = &idle_mode;

}
