

#include "config.h"
#include "timer.h"


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
struct HostCmdMsg HostCmdMsg;
struct OpCmdMsg OpCmdMsg;

//
// Main
//
void main(void)
{

    char *msg = NULL;
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pullups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Board initialization
    //
    Board_init();

    sci_set();
    epwmSet();
    epwmDisableSet(PUMP_01);
    epwmDisableSet(STEP_23);
    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    GPIO_writePin(DAC_CLR, 1);
    GPIO_writePin(DAC_LOAD, 0);
    GPIO_writePin(DAC_SPI_SS, 1);

//    GPIO_writePin(SPI_SS_0, 1);
//    GPIO_writePin(ENABLE_0, 1);

    GPIO_writePin(ADC_Reset, 1);
    GPIO_writePin(ADC_CS_1, 1);
    GPIO_writePin(ADC_Start_1, 1);
    DEVICE_DELAY_US(100);

    dac53508_init();

    ADS1248_Init();
    select_Channel(0);

    SetMotorDirection(1);
    drv8452_init();
    EnableMotor(0);
    timerSet();

    Can_State_Ptr = &hostCmd;///normal mode

    while(1)
    {

#if 0

        if(jump == TEMP_RUN)            Can_State_Ptr = &temp_mode;
        else if(jump == MOTOR_RUN)      Can_State_Ptr = &motor_mode;
        else if(jump == SET_MODE)       Can_State_Ptr = &hostCmd;
//        else if(jump == 7U)      Can_State_Ptr = &operation_mode;
        else                            Can_State_Ptr = &idle_mode;
#endif

        if(cputimer0Flag == TRUE)//50ms Timer flag
        {
            (*Can_State_Ptr)();

            if(gSendTemp_en == 1)
            {
                float ch0 = read_pr100(PT100_CH0) * 10;
                float ch1 = read_pr100(PT100_CH1) * 10;
                float ch2 = read_pr100(PT100_CH2) * 10;
                float ch3 = read_pr100(PT100_CH3) * 10;

                sprintf(msg,"$TEMP,%d,%d,%d,%d\r\n", (int16_t)ch0, (int16_t)ch1, (int16_t)ch2, (int16_t)ch3);
                SCI_writeCharArray(BOOT_SCI_BASE, (uint16_t*)msg, strlen(msg));
            }


            cputimer0Flag = FALSE;
        }


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

#if 0   // dac
        dac53508_write(dacData);
#endif


#if 0   // pt100
//        pt100Temp = readRTDtemp(PT100_CH0);
        pt100Temp = read_pr100(PT100_CH1);
//            rData = spi_read_data();
//            rData = spi_read_id();
//            rData = ADS1248_readRegister();
//            rData = readAdc();
//            DEVICE_DELAY_US(100000);

#endif


#if 0

        if(gFan == 1)
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
