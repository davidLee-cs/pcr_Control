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

    if(HostCmdMsg.oprationSetBit.temperatureRun == 1)
    {

//        ftemp1 = read_pr100(PT100_CH1);
        tempPidControl();
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
//        dac53508_write(0);

    }

}


void motor_mode(void)
{

    if(HostCmdMsg.oprationSetBit.motorRun == 1)
    {

        if(HostCmdMsg.oprationSetBit.motorDirection == 1)
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

void motor_Parameterset(void)
{

//    epwmEnableSet(STEP_23);
    stepperEpwmSet(HostCmdMsg.motorProfile.motorSpeed);
    stepperPulseSet(HostCmdMsg.motorProfile.set_PulseCnt);

}


void tempProfileReset(void)
{

    HostCmdMsg.TempProfile.targetTemp[0] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[1] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[2] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[3] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[4] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[5] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[0] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[1] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[2] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[3] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[4] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[5] = 0 ;

    HostCmdMsg.TempProfile.tempCycle = 0 ;

    HostCmdMsg.motorProfile.motorSpeed = 0;
    HostCmdMsg.motorProfile.set_PulseCnt = 0;
}

void prameterInit(void)
{

    HostCmdMsg.TempProfile.targetTemp[0] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[1] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[2] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[3] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[4] = 0 ;
    HostCmdMsg.TempProfile.targetTemp[5] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[0] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[1] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[2] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[3] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[4] = 0 ;
    HostCmdMsg.TempProfile.timeTemp[5] = 0 ;
    HostCmdMsg.TempProfile.tempCycle = 0 ;

    HostCmdMsg.motorProfile.motorSpeed = 0;
    HostCmdMsg.motorProfile.set_PulseCnt = 0;

    HostCmdMsg.oprationSetBit.motorDirection = 0;
    HostCmdMsg.oprationSetBit.motorRun = 0;
    HostCmdMsg.oprationSetBit.temperatureRun = 0;

    HostCmdMsg.dacSet.dac1 = 0;
    HostCmdMsg.dacSet.dac2 = 0;
    HostCmdMsg.dacSet.dac3 = 0;
    HostCmdMsg.dacSet.dac4 = 0;
    HostCmdMsg.dacSet.dac5 = 0;
    HostCmdMsg.dacSet.dac6 = 0;

    OpCmdMsg.motorMovingStatus.motor1_End_bit = 0;
    OpCmdMsg.motorMovingStatus.motor2_End_bit = 0;
    OpCmdMsg.motorMovingStatus.motor3_End_bit = 0;
    OpCmdMsg.motorMovingStatus.motor4_End_bit = 0;
    OpCmdMsg.motorMovingStatus.motor1_Home_bit = 0;
    OpCmdMsg.motorMovingStatus.motor2_Home_bit = 0;
    OpCmdMsg.motorMovingStatus.motor3_Home_bit = 0;
    OpCmdMsg.motorMovingStatus.motor4_Home_bit = 0;

    OpCmdMsg.opDacSet.dacSet_1 = 0;
    OpCmdMsg.opDacSet.dacSet_2 = 0;
    OpCmdMsg.opDacSet.dacSet_3 = 0;
    OpCmdMsg.opDacSet.dacSet_4 = 0;
    OpCmdMsg.opDacSet.dacSet_5 = 0;
    OpCmdMsg.opDacSet.dacSet_6 = 0;

    OpCmdMsg.tempSensor.tempSensor1_A = 0;
    OpCmdMsg.tempSensor.tempSensor1_B = 0;
    OpCmdMsg.tempSensor.tempSensor2_A = 0;
    OpCmdMsg.tempSensor.tempSensor2_B = 0;

    OpCmdMsg.tempSensor.tempSensor3_A = 0;
    OpCmdMsg.tempSensor.tempSensor3_B = 0;
    OpCmdMsg.tempSensor.tempSensor4_A = 0;
    OpCmdMsg.tempSensor.tempSensor4_B = 0;

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
