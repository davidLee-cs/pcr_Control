/*
 * deviceRun.c
 *
 *  Created on: 2025. 2. 1.
 *      Author: alllite
 */

#include <stdint.h>
#include <inttypes.h>
#include "config.h"

#define PROFILE_0           0
#define PROFILE_1           1
#define PROFILE_2           2
#define PROFILE_3           3
#define PROFILE_4           4
#define PROFILE_CYCLE       5
#define PROFILE_CONTINUE    6
#define PROFILE_IDLE        7

int16_t pumpStop = 0;
uint16_t profileTargetTemp[4] = {0,};
uint16_t profileTime[4] = {0,};
uint16_t lastprofileTargetTemp[4] = {0,};
uint16_t lastprofileTime[4] = {0,};


void tempProfileReset(void);


void profie_checkLevel(int16_t ch, int16_t level)
{
    switch (level){

        case PROFILE_0 :

            profileTargetTemp[ch] = HostCmdMsg[ch].TempProfile.targetTemp[0] + HostCmdMsg[ch].TempProfile.tempOffset;
            profileTime[ch] = HostCmdMsg[ch].TempProfile.timeTemp[0];

            if(profileTargetTemp[ch] != 0)    break;

        case PROFILE_1 :

            profileTargetTemp[ch] = HostCmdMsg[ch].TempProfile.targetTemp[1] + HostCmdMsg[ch].TempProfile.tempOffset;
            profileTime[ch] = HostCmdMsg[ch].TempProfile.timeTemp[1];

            if(profileTargetTemp[ch] != 0)    break;

        case PROFILE_2 :

            profileTargetTemp[ch] = HostCmdMsg[ch].TempProfile.targetTemp[2] + HostCmdMsg[ch].TempProfile.tempOffset;
            profileTime[ch] = HostCmdMsg[ch].TempProfile.timeTemp[2];

            if(profileTargetTemp[ch] != 0)    break;

        case PROFILE_3 :

            profileTargetTemp[ch] = HostCmdMsg[ch].TempProfile.targetTemp[3] + HostCmdMsg[ch].TempProfile.tempOffset;
            profileTime[ch] = HostCmdMsg[ch].TempProfile.timeTemp[3];

            if(profileTargetTemp[ch] != 0)    break;

        case PROFILE_4 :

            profileTargetTemp[ch] = HostCmdMsg[ch].TempProfile.targetTemp[4] + HostCmdMsg[ch].TempProfile.tempOffset;
            profileTime[ch] = HostCmdMsg[ch].TempProfile.timeTemp[4];

            if(profileTargetTemp[ch] != 0)    break;

        case PROFILE_CYCLE :

            if(++tempCycleCnt[ch] >= HostCmdMsg[ch].TempProfile.tempCycle)
            {
                tempCycleCnt[ch] = 0;
                OpCmdMsg[ch].gCycleDone = 1;

                // 사이클 끝나고 마지막 온도로 계속 유지.. 정지 전까지.
                profileTargetTemp[ch] =  lastprofileTargetTemp[ch];
                profileTime[ch] = lastprofileTime[ch];
            }
            else
            {
                // 처음부터 다시
                OpCmdMsg[ch].profileLevel = 0;
                profileTargetTemp[ch] = HostCmdMsg[ch].TempProfile.targetTemp[0] + HostCmdMsg[ch].TempProfile.tempOffset;
                profileTime[ch] = HostCmdMsg[ch].TempProfile.timeTemp[0];
            }

            break;

        default :

            break;
    }


    if(profileTargetTemp[ch] > 0)
    {
        if(profileTargetTemp[ch] >=  lastprofileTargetTemp[ch]){

            OpCmdMsg[ch].control_mode = HEAT_MODE;
        }
        else
        {
            dac53508_write(0, ch);
            DEVICE_DELAY_US(100000);

            OpCmdMsg[ch].control_mode = COOLING_MODE;

            if(ch == 0)            GPIO_writePin(DAC_REVERS_0, RELAY_ON_COOLING);  // 전류 쿨링
            else if(ch == 1)       GPIO_writePin(DAC_REVERS_1, RELAY_ON_COOLING);  // 전류 쿨링
            else if(ch == 2)       GPIO_writePin(DAC_REVERS_2, RELAY_ON_COOLING);  // 전류 쿨링
            else if(ch == 3)       GPIO_writePin(DAC_REVERS_3, RELAY_ON_COOLING);  // 전류 쿨링
            else
            {
                DEVICE_DELAY_US(500000);
            }

            DEVICE_DELAY_US(100000);
        }

        lastprofileTargetTemp[ch] = profileTargetTemp[ch];
        lastprofileTime[ch] = profileTime[ch];
    }


}

void temp_Profilemode(void)
{
    char *msg = NULL;
    int16_t ch;

    for(ch=0; ch< PELTIER_4EA; ch++)
    {

        if(OpCmdMsg[ch].gCycleDone == 1)
        {
            tempProfileCnt[ch] = 0;
        }

        // 채널별 run 실쟁과 동작 시간(단위 100ms)이 0 이상 설정 되었을 때만 동작 함.
        if(HostCmdMsg[ch].oprationSetBit.temperatureRun == 1)
        {

            tempPidControl(ch, profileTargetTemp[ch]);

            // 첫번째 목표 온도에 도달하면 카운터 시작
            // 시간만큼 도달 전에는 nowTempStatus 상태 그대로 1로 유지
            if(OpCmdMsg[ch].nowTempStatus == 1)     // 목표 온도에 도달했다. 온도 유지 모드
            {
                if(tempProfileCnt[ch] >= profileTime[ch]) // 단위 초
                {
                    OpCmdMsg[ch].nowTempStatus = 0;
                    tempProfileCnt[ch] = 0;

                    sprintf(msg,"$TCYCLE,%d,%d,%d\r\n", ch, profileTargetTemp[ch],  profileTime[ch]);
                    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));

                    profie_checkLevel(ch, OpCmdMsg[ch].profileLevel++);
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

//        DEVICE_DELAY_US(2000);
    }

}

void temp_mode(void)
{
    char *msg = NULL;
    int16_t ch;

    for(ch=0; ch< PELTIER_4EA; ch++)
    {
        // 채널별 run 실쟁과 동작 시간(단위 100ms)이 0 이상 설정 되었을 때만 동작 함.
        if((HostCmdMsg[ch].oprationSetBit.temperatureRun == 1) && (HostCmdMsg[ch].TempProfile.singleTimeTemp > 0))
        {

            tempPidControl(ch, HostCmdMsg[ch].TempProfile.singleTargetTemp);

            // 첫번째 목표 온도에 도달하면 카운터 시작
            // 시간만큼 도달 전에는 nowTempStatus 상태 그대로 1로 유지
            if(OpCmdMsg[ch].nowTempStatus == 1)     // 목표 온도에 도달했다. 온도 유지 모드
            {
                if(tempProfileCnt[ch] >= HostCmdMsg[ch].TempProfile.singleTimeTemp)
                {
//                    sprintf(msg,"$ATEMP,%d,%d\r\n", ch, tempCycleCnt[ch] );
//                    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
                    if(++tempCycleCnt[ch] >= HostCmdMsg[ch].TempProfile.tempCycle)
                    {
//                        sprintf(msg,"$CYCLE,%d, %d\r\n", ch, tempCycleCnt[ch] );
//                        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
                        sprintf(msg,"$TCYCLE,%d,%d,%d\r\n", ch, tempProfileCnt[ch], tempCycleCnt[ch]);
                        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));

                        tempCycleCnt[ch] = 0;
                        HostCmdMsg[ch].oprationSetBit.temperatureRun = 0;
                        OpCmdMsg[ch].control_mode = STOP_MODE;
                        dac53508_write(0, ch);
//                        stop_mode();
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
    char *msg = NULL;
    int16_t ch=0;

    if(home_enable == 1)
    {
        home_enable = 0;

        for(ch = 0; ch<4; ch++)
        {
            if(HostCmdMsg[ch].oprationSetBit.stepperHome == 1)
            {

                drv8452_outEnable(ch);
                DEVICE_DELAY_US(2000);
                drv8452_outEnable(ch);
                DEVICE_DELAY_US(2000);
                drv8452_outEnable(ch);
                DEVICE_DELAY_US(2000);
                drv8452_outEnable(ch);

                HostCmdMsg[ch].motorProfile.motorSpeed = HostCmdMsg[ch].motorProfile.homeSpeed;
                HostCmdMsg[ch].motorProfile.set_PulseCnt = HostCmdMsg[ch].motorProfile.home_PulseCnt;
                HostCmdMsg[ch].motorProfile.nowSumPulseCnt = 0;

                SetMotorDirection(ch, RUN_IN); // home -> 1

                motor_Parameterset(ch);

                if((ch == 0) || (ch == 1))
                {
                    epwmEnableSet(STEP_23); // 설정 시 한번만 설정할것.
                }
                else if((ch == 2) || (ch == 3))
                {
                    epwmEnableSet(STEP_01); // 설정 시 한번만 설정할것.
                }

                pulseCount[ch] = 0;     // 현재까지 발생한 펄스 수
                enablecheck[ch] = 0;
            }
        }
    }
    else
    {
        for(ch = 0; ch<4; ch++)
        {

            int64_t sum = HostCmdMsg[ch].motorProfile.nowSumPulseCnt + HostCmdMsg[ch].motorProfile.set_PulseCnt;
            if(sum >= HostCmdMsg[ch].motorProfile.set_MaxPulseCnt)
            {
                if(HostCmdMsg[ch].oprationSetBit.motorDirection == RUN_OUT)
                {
                    HostCmdMsg[ch].motorProfile.set_PulseCnt = HostCmdMsg[ch].motorProfile.set_MaxPulseCnt -  HostCmdMsg[ch].motorProfile.nowSumPulseCnt;
                }
            }

            // 초기화 후 구동 시 동작 안하도록 함.
            if(HostCmdMsg[ch].motorProfile.set_PulseCnt == 0) {
                HostCmdMsg[ch].oprationSetBit.motorRun = 0;
                HostCmdMsg[0].motorProfile.set_PulseCnt = HostCmdMsg[0].motorProfile.set_PulseCnt_byHost;

            }

            if(HostCmdMsg[ch].oprationSetBit.motorRun == 1)
            {

                if(HostCmdMsg[ch].oprationSetBit.motorDirection == CW)
                {
                    SetMotorDirection(ch, RUN_IN);  // home -> 1
                }
                else
                {
                    SetMotorDirection(ch, RUN_OUT);
                }

                drv8452_outEnable(ch);
                DEVICE_DELAY_US(2000);
                drv8452_outEnable(ch);
                DEVICE_DELAY_US(2000);
                drv8452_outEnable(ch);
                DEVICE_DELAY_US(2000);
                drv8452_outEnable(ch);
                EnableMotor(1);

                motor_Parameterset(ch);

                if((ch == 0) || (ch == 1))
                {
                    epwmEnableSet(STEP_23); // 설정 시 한번만 설정할것.
                }
                else if((ch == 2) || (ch == 3))
                {
                    epwmEnableSet(STEP_01); // 설정 시 한번만 설정할것.
                }

                pulseCount[ch] = 0;     // 현재까지 발생한 펄스 수
                pulseCount[ch] = 0;     // 현재까지 발생한 펄스 수
                enablecheck[ch] = 0;

            }
            else
            {
                drv8452_outDisable(ch);
        //        EnableMotor(0);

                if(HostCmdMsg[ch].oprationSetBit.lastmotorRun == 1)
                {
                    sprintf(msg,"$MDONE,%d,", ch);
                    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));

                    HostCmdMsg[0].motorProfile.nowSumPulseCnt += pulseCount[ch];
                    sprintf(msg, "%" PRIu64 "\r\n", HostCmdMsg[0].motorProfile.nowSumPulseCnt);
                    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
                    DEVICE_DELAY_US(1000);
                }

                pulseCount[ch] = 0;     // 현재까지 발생한 펄스 수
                pulseCount[ch] = 0;     // 현재까지 발생한 펄스 수
                enablecheck[ch] = 0;

                if((HostCmdMsg[0].oprationSetBit.motorRun == 0) && (HostCmdMsg[1].oprationSetBit.motorRun == 0))
                {
                    epwmDisableSet(STEP_23); // 설정 시 한번만 설정할것.
                }
                else if((HostCmdMsg[2].oprationSetBit.motorRun == 0) && (HostCmdMsg[3].oprationSetBit.motorRun == 0))
                {
                    epwmDisableSet(STEP_01); // 설정 시 한번만 설정할것.
                }

        //        epwmDisableSet(STEP_23);
            }

            HostCmdMsg[ch].oprationSetBit.lastmotorRun = HostCmdMsg[ch].oprationSetBit.motorRun;
            uint16_t data = drv8452_read(nowChannel);  //
        }
    }


    Can_State_Ptr = &hostCmd;///normal mode
}

void motor_Parameterset(int16_t channel)
{

//    epwmEnableSet(STEP_23);
    stepperEpwmSet(channel, HostCmdMsg[channel].motorProfile.motorSpeed);
    stepperPulseSet(channel, HostCmdMsg[channel].motorProfile.set_PulseCnt);
//    stepperPulseSet(channel, OpCmdMsg[channel].stepperPulseCnt);
}

void pump_Parameterset(int16_t channel)
{

//    epwmEnableSet(STEP_23);
    pumpEpwmSet(channel, HostCmdMsg[channel].motorProfile.pumpDuty);

}


void tempProfileReset(void)
{
    int16_t channel;

    for(channel=0; channel < 4; channel++)
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

    for(channel=0; channel < 4; channel++)
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
        HostCmdMsg[channel].TempProfile.tempOffset = 0;

        HostCmdMsg[channel].motorProfile.motorSpeed = 0;
        HostCmdMsg[channel].motorProfile.set_PulseCnt = 0;
        HostCmdMsg[channel].motorProfile.homeSpeed = 80; // rpm
        HostCmdMsg[channel].motorProfile.home_PulseCnt = 17066L * 10; // 20 회전
        HostCmdMsg[channel].motorProfile.nowSumPulseCnt = 0;
//        HostCmdMsg[channel].motorProfile.set_MaxPulseCnt = 0;


        HostCmdMsg[channel].oprationSetBit.motorDirection = 0;
        HostCmdMsg[channel].oprationSetBit.motorRun = 0;
        HostCmdMsg[channel].oprationSetBit.temperatureRun = 0;

        HostCmdMsg[channel].TempProfile.lastSingleTargetTemp = 0;
        HostCmdMsg[channel].TempProfile.singleTargetTemp = 0;
        HostCmdMsg[channel].TempProfile.singleTimeTemp = 0;
        HostCmdMsg[channel].TempProfile.tempCycle = 0;

        HostCmdMsg[channel].dacSet.dacValue = 0;

        OpCmdMsg[channel].motorMovingStatus.motor_End = 0;
        OpCmdMsg[channel].motorMovingStatus.motor_Home = 0;

        OpCmdMsg[channel].opDacSet.dacSet = 0;

        OpCmdMsg[channel].tempSensor.tempSensorEma_Peltier = 0;
        OpCmdMsg[channel].tempSensor.tempSensorEma_Block = 0;
        OpCmdMsg[channel].tempSensor.tempSensor_Metal = 0;

        OpCmdMsg[channel].nowTempStatus = 0;
        OpCmdMsg[channel].stepperPulseCnt = 0;
        OpCmdMsg[channel].gCycleDone = 0;

        pulseCount[channel] = 0;     // 현재까지 발생한 펄스 수
        enablecheck[channel] = 0;

        OpSwitchStatus.lasthome0 = 1;
        OpSwitchStatus.lasthome1 = 1;
        OpSwitchStatus.lasthome2 = 1;
        OpSwitchStatus.lasthome3 = 1;


    }
}


void stop_mode(void)
{

//    EnableMotor(0);
    drv8452_outDisable(0);
    drv8452_outDisable(1);
    drv8452_outDisable(2);
    drv8452_outDisable(3);

//    epwmDisableSet(PUMP_01);
//    epwmDisableSet(PUMP_23);
    epwmDisableSet(STEP_23);
    epwmDisableSet(STEP_01);

    dac53508_write(0, 0);
    dac53508_write(0, 1);
    dac53508_write(0, 2);
    dac53508_write(0, 3);

    GPIO_writePin(DAC_REVERS_0, RELAY_OFF_HEATING);  // 가열
    DEVICE_DELAY_US(100000);
    GPIO_writePin(DAC_REVERS_1, RELAY_OFF_HEATING);  // 가열
    DEVICE_DELAY_US(100000);
    GPIO_writePin(DAC_REVERS_2, RELAY_OFF_HEATING);  // 가열
    DEVICE_DELAY_US(100000);
    GPIO_writePin(DAC_REVERS_3, RELAY_OFF_HEATING);  // 가열

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

void switchRead(void)
{

    OpSwitchStatus.limie0 = GPIO_readPin(LIMIT0);
    OpSwitchStatus.limie1 = GPIO_readPin(LIMIT1);
    OpSwitchStatus.limie2 = GPIO_readPin(LIMIT2);
    OpSwitchStatus.limie3 = GPIO_readPin(LIMIT3);

    OpSwitchStatus.home0 = GPIO_readPin(HOME0);
    OpSwitchStatus.home1 = GPIO_readPin(HOME1);
    OpSwitchStatus.home2 = GPIO_readPin(HOME2);
    OpSwitchStatus.home3 = GPIO_readPin(HOME3);

    OpSwitchStatus.button0 = GPIO_readPin(BUTTON0);
    OpSwitchStatus.button1 = GPIO_readPin(BUTTON1);
    OpSwitchStatus.button2 = GPIO_readPin(BUTTON2);
    OpSwitchStatus.button3 = GPIO_readPin(BUTTON3);

    if((OpSwitchStatus.button0 == 0) || (OpSwitchStatus.button1 == 0))
    {
        pump_Parameterset(0);
        pump_Parameterset(1);
        epwmEnableSet(PUMP_01); // 설정 시 한번만 설정할것.
    }

    if((OpSwitchStatus.button2 == 0) || (OpSwitchStatus.button3 == 0))
    {
        pump_Parameterset(2);
        pump_Parameterset(3);

        epwmEnableSet(PUMP_23); // 설정 시 한번만 설정할것.
    }



}

void idle_mode(void)
{

    Can_State_Ptr = &idle_mode;

}
