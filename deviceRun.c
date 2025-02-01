/*
 * deviceRun.c
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#include "config.h"




void temp_mode(void)
{
    float ftemp1;
    float ftemperror;
    char *msg = NULL;

    if(HostCmdMsg.oprationSetBit.temperatureRun == 1)
    {

        dac53508_write(OpCmdMsg.opDacSet.dacSet_1);
        ftemp1 = read_pr100(PT100_CH1);

        ftemperror = ftemp1 / HostCmdMsg.TempProfile.targetTemp1;
        if((1.01 > ftemperror) && (ftemperror > 0.99))
        {
            if(cpuTimer1IntCount > HostCmdMsg.TempProfile.timeTemp1)
            {
                sprintf(msg,"$STOP\r\n");
                SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));

                stop_mode();

            }
        }
        else
        {
            cpuTimer1IntCount = 0;

        }

    }
    else
    {
        dac53508_write(0);

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

void motor_set(void)
{

//    epwmEnableSet(STEP_23);
    stepperEpwmSet(HostCmdMsg.motorProfile.motorSpeed);
    stepperPulseSet(HostCmdMsg.motorProfile.set_PulseCnt);

}

void stop_mode(void)
{

    EnableMotor(0);
    epwmDisableSet(STEP_23);
    dac53508_write(0);

    Can_State_Ptr = &hostCmd;///normal mode

}

void idle_mode(void)
{

    Can_State_Ptr = &idle_mode;

}
