/*
 * deviceRun.c
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#include "config.h"

void tempProfileReset(void);


void temp_mode(void)
{
    float ftemp1;
    float ftemperror;
    char *msg = NULL;
    int16_t runCh;


    for(runCh=0; runCh<6; runCh++)
    {
        // 임시 1 채널만...
        if(HostCmdMsg[runCh].oprationSetBit.temperatureRun == 1)
        {

    //        ftemp1 = read_pr100(PT100_CH1);
            tempPidControl(runCh);
     //       dac53508_write(OpCmdMsg.opDacSet.dacSet_1);

    //        ftemperror = ftemp1 / HostCmdMsg.TempProfile.targetTemp1;
    //        if((1.01 > ftemperror) && (ftemperror > 0.99))
    //        {
    //            if(tempProfileCnt > HostCmdMsg.TempProfile.timeTemp1)
    //            {
    //                sprintf(msg,"$STOP\r\n");
    //                SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
    //
    //                stop_mode();
    //
    //            }
    //        }
    //        else
    //        {
    //            tempProfileCnt = 0;
    //
    //        }

            fan_control(1); // fan ON

        }
        else
        {
            dac53508_write(0, runCh);

        }

        DEVICE_DELAY_US(20000);
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

    }
}

void stop_mode(void)
{

    EnableMotor(0);
    epwmDisableSet(STEP_23);
//    dac53508_write(0);
//    tempProfileReset();
    prameterInit();
    fan_control(0);

    Can_State_Ptr = &hostCmd;///normal mode

}

void fan_control(uint16_t enable)
{

    if(enable == 1)
    {
        GPIO_writePin(FAN_0, 1);
        GPIO_writePin(FAN_1, 1);
        GPIO_writePin(FAN_2, 1);
        GPIO_writePin(FAN_3, 1);
        GPIO_writePin(FAN_4, 1);

    }
    else
    {
        GPIO_writePin(FAN_0, 0);
        GPIO_writePin(FAN_1, 0);
        GPIO_writePin(FAN_2, 0);
        GPIO_writePin(FAN_3, 0);
        GPIO_writePin(FAN_4, 0);
    }
}

void idle_mode(void)
{

    Can_State_Ptr = &idle_mode;

}
