
#include <stdint.h>
#include <inttypes.h>
#include "config.h"
#include "hostUart.h"
#include "operationlib.h"
#include "deviceRun.h"

void Example_Done(void);

int16_t nowChannel;
int16_t home_enable = 0;

void hostCmd(void)
{

    char *msg = NULL;

#if 0
    const char *bootCmd = "boot...\r\n";
    const char *barcodeCmd = "barcode...\r\n";
    const char *rs485dCmd = "rs485...\r\n";
    const char *uartCmd = "uart...\r\n";

    char *buffer;
    char *msg1;
    
    int test0= 1234;
    int test2= 5678;

    sprintf(msg1,"boot $%d,%d\r\n", test0, test2);
    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg1, strlen(msg1));

    sprintf(msg1,"barcode $%d,%d\r\n", test0, test2);
    SCI_writeCharArray(BARCODE_SCI_BASE, (uint16_t*)msg1, strlen(msg1));

    sprintf(msg1,"rs485 $%d,%d\r\n", test0, test2);
    SCI_writeCharArray(RS485_SCI_BASE, (uint16_t*)msg1, strlen(msg1));

    sprintf(msg1,"uart ttl  $%d,%d\r\n", test0, test2);
    SCI_writeCharArray(UART_TTL_BASE, (uint16_t*)msg1, strlen(msg1));
#endif

    if(gBoot_Rx_done == TRUE)
    {

        //command :  $START\r\n
        if(strncmp(rBootData_Rx, startCmd, 6) == 0)
        {

//            HostCmdMsg.oprationSetBit.temperatureRun = true;
            gSendTemp_en = 1;

            sprintf(msg,"$START\r\n");
            SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        }

        //command : $STOP\r
        if(strncmp(rBootData_Rx, stopCmd, 5) == 0)
        {

            pumpSet(1);
            DEVICE_DELAY_US(500000);
            stop_mode();

//            HostCmdMsg.oprationSetBit.temperatureRun = false;
            gSendTemp_en = 0;

            sprintf(msg,"$STOP\r\n");
            SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        }

        //command : $TEMP,x,100,10,101,11,102,12,103,13,104,14,5\r\n
        if(strncmp(rBootData_Rx, tempSetCmd, 5) == 0)
        {
            tempSet();
//            Example_Done();
        }

        //command : $STEMP,x,100,10,5\r\n  $STEMP,0,95,5000,5\r  $STEMP,0,30,5000,5\r
        if(strncmp(rBootData_Rx, tempSingleSetCmd, 6) == 0)
        {
            (void)tempSingleSet();
//            Example_Done();
        }

        //command : $MOTOR,ch,70, 6000000\r\n
        if(strncmp(rBootData_Rx, motorSetCmd, 6) == 0)
        {
            motorSet();
//            Example_Done();
        }

        //command : $PUMP,50,1,1,1,1\r\n
        if(strncmp(rBootData_Rx, pumpSetCmd, 5) == 0)
        {
            pumpSet(0);
//            Example_Done();
        }

        //command : $MSTART,1,1,1,1,1,1,1,1\r
        if(strncmp(rBootData_Rx, motorStartCmd, 6) == 0)
        {
            motorStartSet();
//            Example_Done();
        }

        //command : $TSTART,1,1,0,1\r\n $TSTART,1,0,0,0\r  $TSTART,0,0,0,0\r
        if(strncmp(rBootData_Rx, tempStartCmd, 6) == 0)
        {
            fan_AllOn();
            tempStartSet();
//            Example_Done();
        }

        //command : $DAC,1000,2000,3000,4000,0,0\r\n
        if(strncmp(rBootData_Rx, dacSetCmd, 4) == 0)
        {
            dacSet();
//            Example_Done();
        }

        //command : $FAN,1,1,1,1,1\r\n  (-> fan 1,2,3,4, heatFan)
        if(strncmp(rBootData_Rx, fanSetCmd, 4) == 0)
        {
            fanSet();
//            Example_Done();
        }

        //command : $HOME,1,1,1,1,1\r\n  (-> fan 1,2,3,4, heatFan)
        if(strncmp(rBootData_Rx, homeCmd, 5) == 0)
        {
            home_mode();
//            Example_Done();
        }


        //command : $HOME,1,1,1,1,1\r\n  (-> fan 1,2,3,4, heatFan)
        if(strncmp(rBootData_Rx, readparaCmd, 6) == 0)
        {
            parameter_read_mode();
//            Example_Done();
        }



        gBoot_Rx_done = 0;
        gBoot_Rx_cnt = 0;
        memset(rBootData_Rx, 0, 100);
    }

}

void Example_Done(void)
{
    __asm("    ESTOP0");
}

static void tempStartSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    char* tempset = strtok(&rBootData_Rx[8],comma);

    if( tempset != NULL)
    {

        HostCmdMsg[0].oprationSetBit.temperatureRun = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[1].oprationSetBit.temperatureRun = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[2].oprationSetBit.temperatureRun = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[3].oprationSetBit.temperatureRun = atoi(tempset) ;

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);


//        jump = TEMP_RUN;
        Can_State_Ptr = &temp_mode;
    }

}


static int16_t motorStartSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    char* motorset = strtok(&rBootData_Rx[8],comma);

    if( motorset != NULL)
    {

        HostCmdMsg[0].oprationSetBit.motorRun = atoi(motorset);
        motorset = strtok(NULL, comma);

        HostCmdMsg[0].oprationSetBit.motorDirection = atoi(motorset);
        motorset = strtok(NULL, comma);

        HostCmdMsg[1].oprationSetBit.motorRun = atoi(motorset);
        motorset = strtok(NULL, comma);

        HostCmdMsg[1].oprationSetBit.motorDirection = atoi(motorset);
        motorset = strtok(NULL, comma);

        HostCmdMsg[2].oprationSetBit.motorRun = atoi(motorset);
        motorset = strtok(NULL, comma);

        HostCmdMsg[2].oprationSetBit.motorDirection = atoi(motorset);
        motorset = strtok(NULL, comma);

        HostCmdMsg[3].oprationSetBit.motorRun = atoi(motorset);
        motorset = strtok(NULL, comma);

        HostCmdMsg[3].oprationSetBit.motorDirection = atoi(motorset);
        motorset = strtok(NULL, comma);

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);

//        if((nowChannel == 0) || (nowChannel == 1))
//        {
//            epwmEnableSet(STEP_23); // 설정 시 한번만 설정할것.
//        }
//        else if((nowChannel == 2) || (nowChannel == 3))
//        {
//            epwmEnableSet(STEP_01); // 설정 시 한번만 설정할것.
//        }
//        jump = MOTOR_RUN;
        Can_State_Ptr = &motor_mode;

    }

    return 0;
}

#if 0
static int16_t motorStartSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    char* motorset = strtok(&rBootData_Rx[8],comma);

    if( motorset != NULL)
    {
        nowChannel = atoi(motorset);
        motorset = strtok(NULL, comma);

        if(nowChannel > PELTIER_4EA)
        {
            return 1;
        }

        int16_t run = atoi(motorset);
        HostCmdMsg[nowChannel].oprationSetBit.motorRun = run;

        motorset = strtok(NULL, comma);

        int16_t dir = atoi(motorset);
        HostCmdMsg[nowChannel].oprationSetBit.motorDirection = dir;

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);

//        if((nowChannel == 0) || (nowChannel == 1))
//        {
//            epwmEnableSet(STEP_23); // 설정 시 한번만 설정할것.
//        }
//        else if((nowChannel == 2) || (nowChannel == 3))
//        {
//            epwmEnableSet(STEP_01); // 설정 시 한번만 설정할것.
//        }
//        jump = MOTOR_RUN;
        Can_State_Ptr = &motor_mode;

    }

    return 0;
}
#endif


static void parameter_read_mode(void)
{
    char *msg= NULL;

    char channel;

    for(channel=0; channel<4; channel++)
    {
        // Temp read
        sprintf(msg,"$TPARA0,%d,%d,%d,%d,%d,%d\r\n",
                channel,
                HostCmdMsg[channel].TempProfile.targetTemp[0],
                HostCmdMsg[channel].TempProfile.targetTemp[1],
                HostCmdMsg[channel].TempProfile.targetTemp[2],
                HostCmdMsg[channel].TempProfile.targetTemp[3],
                HostCmdMsg[channel].TempProfile.targetTemp[4]
          );
        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        DEVICE_DELAY_US(100000);

        sprintf(msg,"$TPARA1,%d,%d,%d,%d,%d,%d\r\n",
                channel,
                HostCmdMsg[channel].TempProfile.timeTemp[0],
                HostCmdMsg[channel].TempProfile.timeTemp[1],
                HostCmdMsg[channel].TempProfile.timeTemp[2],
                HostCmdMsg[channel].TempProfile.timeTemp[3],
                HostCmdMsg[channel].TempProfile.timeTemp[4]
         );
        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        DEVICE_DELAY_US(100000);


        sprintf(msg,"$TPARA2,%d,%d\r\n",
                channel,
                HostCmdMsg[channel].TempProfile.tempCycle
        );
        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        DEVICE_DELAY_US(100000);
    }

    for(channel=0; channel<4; channel++)
    {
        sprintf(msg,"$MPARA0,%d,%d,%d,",
                channel,
                HostCmdMsg[channel].motorProfile.motorSpeed,
                HostCmdMsg[channel].motorProfile.homeSpeed
        );
        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        DEVICE_DELAY_US(100000);

        sprintf(msg, "%" PRIu64 ",", HostCmdMsg[channel].motorProfile.set_PulseCnt);
        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        DEVICE_DELAY_US(100000);

        sprintf(msg, "%" PRIu64 "\r\n", HostCmdMsg[channel].motorProfile.home_PulseCnt);
        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        DEVICE_DELAY_US(100000);

        sprintf(msg,"$MPARA1,%d,%d,%d,%d\r\n",
                channel,
                HostCmdMsg[channel].oprationSetBit.motorDirection,
                HostCmdMsg[channel].oprationSetBit.motorRun,
                HostCmdMsg[channel].oprationSetBit.temperatureRun
        );
        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        DEVICE_DELAY_US(100000);

    }

    sprintf(msg,"$PPARA,%d,%d,%d,%d\r\n",
            HostCmdMsg[0].motorProfile.pumpDuty,
            HostCmdMsg[1].motorProfile.pumpDuty,
            HostCmdMsg[2].motorProfile.pumpDuty,
            HostCmdMsg[3].motorProfile.pumpDuty
    );
    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
    DEVICE_DELAY_US(100000);

    sprintf(msg,"$LIMITE,%d,%d,%d,%d\r\n",
            OpSwitchStatus.limie0,
            OpSwitchStatus.limie1,
            OpSwitchStatus.limie2,
            OpSwitchStatus.limie3

    );
    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
    DEVICE_DELAY_US(100000);


    sprintf(msg,"$HOME,%d,%d,%d,%d\r\n",
            OpSwitchStatus.home0,
            OpSwitchStatus.home1,
            OpSwitchStatus.home2,
            OpSwitchStatus.home3

    );
    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
    DEVICE_DELAY_US(100000);

    sprintf(msg,"$BUTTON,%d,%d,%d,%d\r\n",
            OpSwitchStatus.button0,
            OpSwitchStatus.button1,
            OpSwitchStatus.button2,
            OpSwitchStatus.button3

    );
    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
    DEVICE_DELAY_US(100000);


    Can_State_Ptr = &hostCmd;

}


static void home_mode(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};
    char buffer[100] = {0,};

    HostCmdMsg[0].oprationSetBit.stepperHome = 1;
    HostCmdMsg[1].oprationSetBit.stepperHome = 1;
    HostCmdMsg[2].oprationSetBit.stepperHome = 1;
    HostCmdMsg[3].oprationSetBit.stepperHome = 1;

    home_enable = 1;

    SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
    SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);

    Can_State_Ptr = &motor_mode;

    return 0;
}


static int16_t motorSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    char* motorset = strtok(&rBootData_Rx[6],comma);

    if( motorset != NULL)
    {
        int16_t channel = atoi(motorset);
        motorset = strtok(NULL, comma);

        if(channel > PELTIER_4EA)
        {
            return 1;
        }

        int16_t speed = atoi(motorset);
        HostCmdMsg[channel].motorProfile.motorSpeed = speed;

        motorset = strtok(NULL, comma);

        int64_t pulse = atoll(motorset);
        HostCmdMsg[channel].motorProfile.set_PulseCnt = pulse;

//        motor_Parameterset(channel);

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
    }

    Can_State_Ptr = &hostCmd;///normal mode

    return 0;
}

static void pumpSet(int16_t stop)
{
    int16_t run0;
    int16_t run1;
    int16_t run2;
    int16_t run3;

    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    char* pumpset = strtok(&rBootData_Rx[5],comma);

    if(stop == 1)
    {
//        HostCmdMsg[0].motorProfile.pumpDuty = 5;
//        HostCmdMsg[1].motorProfile.pumpDuty = 5;
//        HostCmdMsg[2].motorProfile.pumpDuty = 5;
//        HostCmdMsg[3].motorProfile.pumpDuty = 5;

        HostCmdMsg[1].oprationSetBit.pumpRun = 0;
        HostCmdMsg[0].oprationSetBit.pumpRun = 0;
        HostCmdMsg[3].oprationSetBit.pumpRun = 0;
        HostCmdMsg[2].oprationSetBit.pumpRun = 0;
    }
    else
    {
        if( pumpset != NULL)
        {
            int16_t duty = atoi(pumpset);
            pumpset = strtok(NULL, comma);

            HostCmdMsg[0].motorProfile.pumpDuty = duty;
            HostCmdMsg[1].motorProfile.pumpDuty = duty;
            HostCmdMsg[2].motorProfile.pumpDuty = duty;
            HostCmdMsg[3].motorProfile.pumpDuty = duty;

            run0 = atoi(pumpset);
            HostCmdMsg[1].oprationSetBit.pumpRun = run0;
            pumpset = strtok(NULL, comma);

            run1 = atoi(pumpset);
            HostCmdMsg[0].oprationSetBit.pumpRun = run1;
            pumpset = strtok(NULL, comma);

            run2 = atoi(pumpset);
            HostCmdMsg[3].oprationSetBit.pumpRun = run2;
            pumpset = strtok(NULL, comma);

            run3 = atoi(pumpset);
            HostCmdMsg[2].oprationSetBit.pumpRun = run3;


            pump_Parameterset(0);
            pump_Parameterset(1);
            pump_Parameterset(2);
            pump_Parameterset(3);


            if((run0 == 1) || (run1 == 1))
            {
                epwmEnableSet(PUMP_01); // 설정 시 한번만 설정할것.
            }

            if((run2 == 1) || (run3 == 1))
            {
                epwmEnableSet(PUMP_23); // 설정 시 한번만 설정할것.
            }


            SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
            SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
        }

    }


    Can_State_Ptr = &hostCmd;///normal mode

}


static int16_t tempSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

//    int16_t i;
//    for(i=0; i<100; i++)
//    {
//        buffer[i] = (char)rBootData_Rx[i];
//    }

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    char* tempset = strtok(&rBootData_Rx[6],comma);

    if( tempset != NULL)
    {
        int16_t channel = atoi(tempset);
        tempset = strtok(NULL, comma);

        if(channel > PELTIER_4EA)
        {
            return 1;
        }

        HostCmdMsg[channel].TempProfile.targetTemp[0] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.timeTemp[0] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.targetTemp[1] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.timeTemp[1] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.targetTemp[2] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.timeTemp[2] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.targetTemp[3] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.timeTemp[3] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.targetTemp[4] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.timeTemp[4] = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.tempCycle = atoi(tempset) ;

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
    }

    Can_State_Ptr = &hostCmd;///normal mode

    return 0;
}

static int16_t tempSingleSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    char* tempset = strtok(&rBootData_Rx[6],comma);

    if( tempset != NULL)
    {
        int16_t channel = atoi(tempset);
        tempset = strtok(NULL, comma);

        if(channel > PELTIER_4EA)
        {
            return 1;
        }

        HostCmdMsg[channel].TempProfile.singleTargetTemp = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.singleTimeTemp = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.tempCycle = atoi(tempset) ;


        if(HostCmdMsg[channel].TempProfile.singleTargetTemp >=  HostCmdMsg[channel].TempProfile.lastSingleTargetTemp){

            OpCmdMsg[channel].control_mode = HEAT_MODE;
        }
        else
        {

            dac53508_write(0, channel);
            DEVICE_DELAY_US(500000);

            OpCmdMsg[channel].control_mode = COOLING_MODE;

            if(channel == 0)            GPIO_writePin(DAC_REVERS_0, RELAY_ON_COOLING);  // 전류 쿨링
            else if(channel == 1)       GPIO_writePin(DAC_REVERS_1, RELAY_ON_COOLING);  // 전류 쿨링
            else if(channel == 2)       GPIO_writePin(DAC_REVERS_2, RELAY_ON_COOLING);  // 전류 쿨링
            else if(channel == 3)       GPIO_writePin(DAC_REVERS_3, RELAY_ON_COOLING);  // 전류 쿨링
            else
            {
                DEVICE_DELAY_US(500000);
            }

            DEVICE_DELAY_US(500000);
            DEVICE_DELAY_US(500000);
            DEVICE_DELAY_US(500000);
            DEVICE_DELAY_US(500000);
        }

        HostCmdMsg[channel].TempProfile.lastSingleTargetTemp = HostCmdMsg[channel].TempProfile.singleTargetTemp;


        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
    }

    Can_State_Ptr = &hostCmd;///normal mode

    return 0;
}



static void dacSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    char* tempset = strtok(&rBootData_Rx[5],comma);

    if( tempset != NULL)
    {
        int16_t channel = atoi(tempset);
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].dacSet.dacValue = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].dacSet.dacValue = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].dacSet.dacValue = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].dacSet.dacValue = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].dacSet.dacValue = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].dacSet.dacValue = atoi(tempset) ;

        OpCmdMsg[channel].opDacSet.dacSet = HostCmdMsg[channel].dacSet.dacValue;

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
    }

    Can_State_Ptr = &hostCmd;///normal mode

}

static void fanSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};
    int16_t heatFan;        // 방열판 팬 구동

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    char* tempset = strtok(&rBootData_Rx[5],comma);

    if( tempset != NULL)
    {

        HostCmdMsg[0].fanSet.fanEnable = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[1].fanSet.fanEnable = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[2].fanSet.fanEnable = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[3].fanSet.fanEnable = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        heatFan = atoi(tempset) ;

        fan_control(heatFan);


//        int16_t channel = atoi(tempset);
//        tempset = strtok(NULL, comma);

//        HostCmdMsg[channel].fanSet.fanEnable= atoi(tempset) ;
//        OpCmdMsg[channel].opFanSet.fanEnable = HostCmdMsg[channel].fanSet.fanEnable;

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
    }

    Can_State_Ptr = &hostCmd;///normal mode

}
