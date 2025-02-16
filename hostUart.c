
#include "config.h"
#include "hostUart.h"
#include "operationlib.h"
#include "deviceRun.h"

void Example_Done(void);

int16_t nowChannel;

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

            stop_mode();

//            HostCmdMsg.oprationSetBit.temperatureRun = false;
            gSendTemp_en = 0;

            sprintf(msg,"$STOP\r\n");
            SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
        }

        //command : $TEMP,ch,100,10,101,11,102,12,103,13,104,14,5\r\n
        if(strncmp(rBootData_Rx, tempSetCmd, 5) == 0)
        {
            tempSet();
//            Example_Done();
        }

        //command : $STEMP,ch,100,10,5\r\n
        if(strncmp(rBootData_Rx, tempSingleSetCmd, 6) == 0)
        {
            tempSingleSet();
//            Example_Done();
        }

        //command : $MOTOR,ch,70, 6000000\r\n
        if(strncmp(rBootData_Rx, motorSetCmd, 6) == 0)
        {
            motorSet();
//            Example_Done();
        }

        //command : $MSTART,1,0,1,1\r\n
        if(strncmp(rBootData_Rx, motorStartCmd, 6) == 0)
        {
            motorStartSet();
//            Example_Done();
        }

        //command : $TSTART,1,1,0,1\r\n
        if(strncmp(rBootData_Rx, tempStartCmd, 6) == 0)
        {
            tempStartSet();
//            Example_Done();
        }

        //command : $DAC,1000,2000,3000,4000,0,0\r\n
        if(strncmp(rBootData_Rx, dacSetCmd, 4) == 0)
        {
            dacSet();
//            Example_Done();
        }

        if(strncmp(rBootData_Rx, fanSetCmd, 4) == 0)
        {
            fanSet();
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

static void motorStartSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));


    // 1. ¼o½AμE ¹®AU¿­·I ºIAI comma ´UA§ ºÐAU¿­·I ºÐ¸®
    char* motorset = strtok(&rBootData_Rx[8],comma);

    if( motorset != NULL)
    {
        nowChannel = atoi(motorset);
        motorset = strtok(NULL, comma);

        int16_t run = atoi(motorset);
        HostCmdMsg[nowChannel].oprationSetBit.temperatureRun = run;

        motorset = strtok(NULL, comma);

        int16_t dir = atoi(motorset);
        HostCmdMsg[nowChannel].oprationSetBit.motorDirection = dir;


        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);

//        jump = MOTOR_RUN;
        Can_State_Ptr = &motor_mode;
        epwmEnableSet(STEP_23); // 설정 시 한번만 설정할것.
    }

}

static void motorSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));


    // 1. ¼o½AμE ¹®AU¿­·I ºIAI comma ´UA§ ºÐAU¿­·I ºÐ¸®
    char* motorset = strtok(&rBootData_Rx[6],comma);

    if( motorset != NULL)
    {
        int16_t channel = atoi(motorset);
        motorset = strtok(NULL, comma);

        int16_t speed = atoi(motorset);
        HostCmdMsg[channel].motorProfile.motorSpeed = speed;

        motorset = strtok(NULL, comma);

        int16_t pulse = atoll(motorset);
        HostCmdMsg[channel].motorProfile.set_PulseCnt = pulse;

//        motorset = strtok(NULL, comma);

        motor_Parameterset(channel);

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
    }

    Can_State_Ptr = &hostCmd;///normal mode

}


static void tempSet(void)
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

    // 1. ¼o½AμE ¹®AU¿­·I ºIAI comma ´UA§ ºÐAU¿­·I ºÐ¸®
    char* tempset = strtok(&rBootData_Rx[6],comma);

    if( tempset != NULL)
    {
        int16_t channel = atoi(tempset);
        tempset = strtok(NULL, comma);

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

}

static void tempSingleSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    // 1. ¼o½AμE ¹®AU¿­·I ºIAI comma ´UA§ ºÐAU¿­·I ºÐ¸®
    char* tempset = strtok(&rBootData_Rx[6],comma);

    if( tempset != NULL)
    {
        int16_t channel = atoi(tempset);
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.singleTargetTemp = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.singleTimeTemp = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].TempProfile.tempCycle = atoi(tempset) ;

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
    }

    Can_State_Ptr = &hostCmd;///normal mode

}



static void dacSet(void)
{
    const char* comma = ",";
    const char end[] = {'\r', '\n'};

    char buffer[100] = {0,};

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    // 1. ¼o½AμE ¹®AU¿­·I ºIAI comma ´UA§ ºÐAU¿­·I ºÐ¸®
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

    memcpy(&buffer[0],&rBootData_Rx[0], strlen(&rBootData_Rx[0]));

    // 1. ¼o½AμE ¹®AU¿­·I ºIAI comma ´UA§ ºÐAU¿­·I ºÐ¸®
    char* tempset = strtok(&rBootData_Rx[5],comma);

    if( tempset != NULL)
    {
        int16_t channel = atoi(tempset);
        tempset = strtok(NULL, comma);

        HostCmdMsg[channel].fanSet.fanEnable= atoi(tempset) ;
        OpCmdMsg[channel].opFanSet.fanEnable = HostCmdMsg[channel].fanSet.fanEnable;

        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
    }

    Can_State_Ptr = &hostCmd;///normal mode

}
