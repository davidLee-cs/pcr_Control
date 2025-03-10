

#include "config.h"
#include "timer.h"


#pragma CODE_SECTION(SetOnOffControl, ".TI.ramfunc");
//#pragma CODE_SECTION(DCL_runPID_C4, ".TI.ramfunc");


uint32_t sData = 0;                  // Send data
uint32_t rData = 0;                  // Receive data

float pt100Temp;
int16_t gEn_motor = 0;
int16_t gEn_pwm = 0;
uint16_t dacData=0;
uint16_t gFan = 0;
uint16_t jump=0;
uint16_t gSendTemp_en=0;

void (*Can_State_Ptr)(void);        // 다음 수행될 모드를 가르키는 함수 포인터
struct HostCmdMsg HostCmdMsg[6];
struct OpCmdMsg OpCmdMsg[6];
struct OpSwitchStatus OpSwitchStatus;

uint16_t gloopCnt=0;

//
// Main
//
void main(void)
{

    char *msg = NULL;
    float ch0,ch1,ch2,ch3,ch4,ch5,ch6,ch7;

    Device_init();

    Device_initGPIO();

    Interrupt_initModule();

    Interrupt_initVectorTable();

    Board_init();

    sci_set();
    epwmSet();
    epwmDisableSet(PUMP_01);
    epwmDisableSet(PUMP_23);
    epwmDisableSet(STEP_23);
    epwmDisableSet(STEP_01);
    DEVICE_DELAY_US(200000);

    EINT;
    ERTM;

    GPIO_writePin(DAC_CLR, 1);
    GPIO_writePin(DAC_LOAD, 0);
    GPIO_writePin(DAC_SPI_SS, 1);
    GPIO_writePin(ADC_Reset, 1);
    GPIO_writePin(ADC_CS_1, 1);
    GPIO_writePin(ADC_CS_2, 1);
    GPIO_writePin(ADC_Start_1, 1);
    DEVICE_DELAY_US(100);
    DEVICE_DELAY_US(200000);

    // device init
    dac53508_init();

    DEVICE_DELAY_US(200000);
    ADS1248_Init_1();
    ADS1248_Init_2();
    DEVICE_DELAY_US(200000);
    select_Channel(0,0);
    select_Channel(1,0);

    DEVICE_DELAY_US(200000);
    SetMotorDirection(1,1);
    SetMotorDirection(2,1);
    SetMotorDirection(3,1);
    SetMotorDirection(4,1);
    drv8452_init();

//    EnableMotor(0);
    EnableMotor(1);  // 250225
    drv8452_outEnable(0);
    DEVICE_DELAY_US(2000);
    drv8452_outEnable(1);
    DEVICE_DELAY_US(2000);
    drv8452_outEnable(2);
    DEVICE_DELAY_US(2000);
    drv8452_outEnable(3);


    timerSet();
    fan_AllOff(); // fan Off
//    fan_AllOn();

    prameterInit();
    init_pid();

    DEVICE_DELAY_US(200000);

    Can_State_Ptr = &hostCmd;

    while(1)
    {

#if 0
//        if(jump == TEMP_RUN)            Can_State_Ptr = &temp_mode;
//        else if(jump == MOTOR_RUN)      Can_State_Ptr = &motor_mode;
//        else if(jump == SET_MODE)       Can_State_Ptr = &hostCmd;
////        else if(jump == 7U)      Can_State_Ptr = &operation_mode;
//        else                            Can_State_Ptr = &idle_mode;
#endif

        hostCmd();
        if(cputimer0Flag == TRUE)//50ms Timer flag
        {
            (*Can_State_Ptr)();

            if(gloopCnt++ > 20) // 1 초
            {
                if(gSendTemp_en == 1)
                {
                    fan_AllOn();

                    ch0 = read_pr100(0,PT100_CH0) * 10;
                    ch1 = read_pr100(0,PT100_CH1) * 10;
                    ch2 = read_pr100(0,PT100_CH2) * 10;
                    ch3 = read_pr100(0,PT100_CH3) * 10;
                    ch4 = read_pr100(1,PT100_CH0) * 10;
                    ch5 = read_pr100(1,PT100_CH1) * 10;
                    ch6 = read_pr100(1,PT100_CH2) * 10;
                    ch7 = read_pr100(1,PT100_CH3) * 10;

//                    ch0 = OpCmdMsg[0].tempSensor.nowTemp_S1 * 10;
//                    ch1 = OpCmdMsg[1].tempSensor.nowTemp_S1 * 10;
//                    ch2 = OpCmdMsg[2].tempSensor.nowTemp_S1 * 10;
//                    ch3 = OpCmdMsg[3].tempSensor.nowTemp_S1 * 10;
//
//                    ch4 = OpCmdMsg[0].tempSensor.nowTemp_S2 * 10;
//                    ch5 = OpCmdMsg[1].tempSensor.nowTemp_S2 * 10;
//                    ch6 = OpCmdMsg[2].tempSensor.nowTemp_S2 * 10;
//                    ch7 = OpCmdMsg[3].tempSensor.nowTemp_S2 * 10;


                    sprintf(msg,"$TEMP,%d,%d,%d,%d,", (int16_t)ch0, (int16_t)ch1, (int16_t)ch2, (int16_t)ch3);
                    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));

                    sprintf(msg,"%d,%d,%d,%d\r\n", (int16_t)ch4, (int16_t)ch5, (int16_t)ch6, (int16_t)ch7);
                    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
                }

                gloopCnt = 0;
//                fan_control(gSendTemp_en);

            }

            switchRead();

            cputimer0Flag = FALSE;
        }
//        DEVICE_DELAY_US(10000);
    }
}


//
// End File
//
