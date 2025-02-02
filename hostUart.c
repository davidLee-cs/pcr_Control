
#include "config.h"
#include "hostUart.h"
#include "operationlib.h"
#include "deviceRun.h"

void Example_Done(void);

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

        //command : $TEMP,100,10,101,11,102,12,103,13,104,14,5\r\n
        if(strncmp(rBootData_Rx, tempSetCmd, 5) == 0)
        {
            tempSet();
//            Example_Done();
        }

        //command : $MOTOR,70, 6000000\r\n
        if(strncmp(rBootData_Rx, motorSetCmd, 6) == 0)
        {
            motorSet();
//            Example_Done();
        }

        //command : $MSTART,1,1\r\n
        if(strncmp(rBootData_Rx, motorStartCmd, 6) == 0)
        {
            motorStartSet();
//            Example_Done();
        }

        //command : $TSTART,1\r\n
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


    // 1. ¼o½AμE ¹®AU¿­·I ºIAI comma ´UA§ ºÐAU¿­·I ºÐ¸®
    char* tempset = strtok(&rBootData_Rx[8],comma);

    if( tempset != NULL)
    {
        HostCmdMsg.oprationSetBit.temperatureRun = atoi(tempset) ;

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
    char* tempset = strtok(&rBootData_Rx[8],comma);

    if( tempset != NULL)
    {
        HostCmdMsg.oprationSetBit.motorRun = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.oprationSetBit.motorDirection = atoi(tempset) ;

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
    char* tempset = strtok(&rBootData_Rx[6],comma);

    if( tempset != NULL)
    {
        HostCmdMsg.motorProfile.motorSpeed = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.motorProfile.set_PulseCnt = atoll(tempset) ;
        tempset = strtok(NULL, comma);

        motor_Parameterset();

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
        HostCmdMsg.TempProfile.targetTemp1 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.timeTemp1 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.targetTemp2 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.timeTemp2 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.targetTemp3 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.timeTemp3 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.targetTemp4 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.timeTemp4 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.targetTemp5 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.timeTemp5 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.TempProfile.tempCycle = atoi(tempset) ;


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
        HostCmdMsg.dacSet.dac1 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.dacSet.dac2 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.dacSet.dac3 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.dacSet.dac4 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.dacSet.dac5 = atoi(tempset) ;
        tempset = strtok(NULL, comma);

        HostCmdMsg.dacSet.dac6 = atoi(tempset) ;


        OpCmdMsg.opDacSet.dacSet_1 = HostCmdMsg.dacSet.dac1;
        OpCmdMsg.opDacSet.dacSet_2 = HostCmdMsg.dacSet.dac2;
        OpCmdMsg.opDacSet.dacSet_3 = HostCmdMsg.dacSet.dac3;
        OpCmdMsg.opDacSet.dacSet_4 = HostCmdMsg.dacSet.dac4;
        OpCmdMsg.opDacSet.dacSet_5 = HostCmdMsg.dacSet.dac5;
        OpCmdMsg.opDacSet.dacSet_6 = HostCmdMsg.dacSet.dac6;


        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)buffer, (uint16_t)strlen(buffer));
        SCI_writeCharArray(BOOT_SCI_BASE, (const char*)end, 2U);
    }

    Can_State_Ptr = &hostCmd;///normal mode

}
