

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

    timerSet();
    fan_AllOff(); // fan Off
//    fan_AllOn();

    prameterInit();
    init_pid();

    DEVICE_DELAY_US(200000);

    Can_State_Ptr = &hostCmd;

    while(1)
    {

#if 1

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

                    float ch0 = read_pr100(0,PT100_CH0) * 10;
                    float ch1 = read_pr100(0,PT100_CH1) * 10;
                    float ch2 = read_pr100(0,PT100_CH2) * 10;
                    float ch3 = read_pr100(0,PT100_CH3) * 10;
                    float ch4 = read_pr100(1,PT100_CH0) * 10;
                    float ch5 = read_pr100(1,PT100_CH1) * 10;
                    float ch6 = read_pr100(1,PT100_CH2) * 10;
                    float ch7 = read_pr100(1,PT100_CH3) * 10;

                    sprintf(msg,"$TEMP,%d,%d,%d,%d,", (int16_t)ch0, (int16_t)ch1, (int16_t)ch2, (int16_t)ch3);
                    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));

                    sprintf(msg,"%d,%d,%d,%d\r\n", (int16_t)ch4, (int16_t)ch5, (int16_t)ch6, (int16_t)ch7);
                    SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
                }

                gloopCnt = 0;
                fan_control(gSendTemp_en);
            }

            switchRead();

            cputimer0Flag = FALSE;
        }
#endif

#if 0   // Pump  motor


        if(gEn_pwm)
        {
            gEn_pwm = 0;

            if(gEn_motor)
            {
                epwmEnableSet(PUMP_01);
            }
            else
            {
                epwmDisableSet(PUMP_01);
            }
        }

#endif

#if 0   // stepper motor


        if(gEn_pwm)
        {
            gEn_pwm = 0;

            EnableMotor(gEn_motor);
            if(gEn_motor)
            {
                stepperEpwmSet(HostCmdMsg.motorProfile.motorSpeed);
                epwmEnableSet(STEP_23);
            }
            else
            {
                epwmDisableSet(STEP_23);
            }
        }
        uint16_t data = drv8452_read();  // 0xE020
//        drv8452_write(&DRV8452_regs[CTRL1])
#endif

#if 0

        tempPidControl(0);
        DEVICE_DELAY_US(20000);
#endif


#if 0   // dac
        dac53508_write(dacData,0);
//        DEVICE_DELAY_US(10000);

#endif


#if 0  // pt100
        pt100Temp = read_pr100(PT100_CH0);
//        pt100Temp = read_pr100(PT100_CH1);

//        sprintf(msg,"$TEMP,%d\r\n", (int16_t)pt100Temp);
//        SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
//            rData = spi_read_data();
//            rData = spi_read_id();
//            rData = ADS1248_readRegister();
//            rData = readAdc();
            DEVICE_DELAY_US(20000);

#endif


#if 0

        if(gFan == 1)
        {
            GPIO_writePin(FAN_0, 1);
            GPIO_writePin(FAN_1, 1);
            GPIO_writePin(FAN_2, 1);
            GPIO_writePin(FAN_3, 1);
            GPIO_writePin(FAN_4, 1);

            GPIO_writePin(LASER0, 1);
            GPIO_writePin(LASER1, 1);
            GPIO_writePin(LASER2, 1);
            GPIO_writePin(LASER3, 1);

            GPIO_writePin(LED0, 1);
            GPIO_writePin(LED1, 1);
            GPIO_writePin(LED2, 1);

            GPIO_writePin(DAC_REVERS_0, 1);
            GPIO_writePin(DAC_REVERS_1, 1);
            GPIO_writePin(DAC_REVERS_2, 1);
            GPIO_writePin(DAC_REVERS_3, 1);

        }
        else
        {
            GPIO_writePin(FAN_0, 0);
            GPIO_writePin(FAN_1, 0);
            GPIO_writePin(FAN_2, 0);
            GPIO_writePin(FAN_3, 0);
            GPIO_writePin(FAN_4, 0);

            GPIO_writePin(LASER0, 0);
            GPIO_writePin(LASER1, 0);
            GPIO_writePin(LASER2, 0);
            GPIO_writePin(LASER3, 0);

            GPIO_writePin(LED0, 0);
            GPIO_writePin(LED1, 0);
            GPIO_writePin(LED2, 0);

            GPIO_writePin(DAC_REVERS_0, 0);
            GPIO_writePin(DAC_REVERS_1, 0);
            GPIO_writePin(DAC_REVERS_2, 0);
            GPIO_writePin(DAC_REVERS_3, 0);


        }
#endif

#if 0

        limie0 = GPIO_readPin(LIMIT0);
        limie1 = GPIO_readPin(LIMIT1);
        limie2 = GPIO_readPin(LIMIT2);
        limie3 = GPIO_readPin(LIMIT3);

        home0 = GPIO_readPin(HOME0);
        home1 = GPIO_readPin(HOME1);
        home2 = GPIO_readPin(HOME2);
        home3 = GPIO_readPin(HOME3);

        button0 = GPIO_readPin(BUTTON0);
        button1 = GPIO_readPin(BUTTON1);
        button2 = GPIO_readPin(BUTTON2);
        button3 = GPIO_readPin(BUTTON3);


#endif

#if 0

        factory_mode();

#endif

//        DEVICE_DELAY_US(10000);
    }
}




//
// End File
//
