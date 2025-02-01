//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "drv8452.h"
#include "ads1248_pt100.h"
#include "dac53508.h"
#include "timer.h"

//
// Function Prototypes
//

#define PT100_CH0       0
#define PT100_CH1       1
#define PT100_CH2       2
#define PT100_CH3       3



uint32_t sData = 0;                  // Send data
uint32_t rData = 0;                  // Receive data

float pt100Temp;
int16_t gEn_motor = 0;
//
// Main
//
void main(void)
{

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

    ADS1248_Init();
    select_Channel(0);

    SetMotorDirection(1);
    drv8452_init();
    EnableMotor(0);
    timerSet();

    while(1)
    {

#if 1   // stepper motor

        EnableMotor(gEn_motor);
        if(gEn_motor)
        {
            CPUTimer_startTimer(CPUTIMER0_BASE);
        }
        else
        {
            CPUTimer_stopTimer(CPUTIMER0_BASE);
        }

        uint16_t data = drv8452_read();  // 0xE020
//        drv8452_write(&DRV8452_regs[CTRL1])
#endif

#if 0   // dac
        dac53508_write();
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

        DEVICE_DELAY_US(10000);

    }
}

//
// End File
//
