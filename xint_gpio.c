/*
 * xint_gpio.c
 *
 *  Created on: 2025. 3. 14.
 *      Author: alllite
 */


#include "driverlib.h"
#include "device.h"
#include "config.h"
#include "cmdMsg.h"
#include "operationlib.h"


interrupt void xint1_isr(void);
interrupt void xint2_isr(void);

volatile uint32_t XINT1Count;
volatile uint32_t XINT2Count;
volatile uint32_t loopCount;


void xint_init(void)
{
    Interrupt_register(HOME0_XINT, &INT_HOME0_XINT_ISR);
//    Interrupt_register(HOME0_XIN2, &xint2_isr);

//    Interrupt_enable(HOME0_XINT);
//    Interrupt_enable(HOME0_XIN2);

}


int16_t xint_cnt=0;

interrupt void INT_HOME0_XINT_ISR(void)
{
    int16_t signal_stable = 1;

    DEVICE_DELAY_US(10000);
    if (GPIO_readPin(HOME0) != 0)  // 신호가 High로 바뀌면
    {
        signal_stable = 0;  // 안정적이지 않음
    }

    if (signal_stable)
    {
        XINT1Count++;
    }

    OpSwitchStatus.lasthome0 = OpSwitchStatus.home0;

    //
    // Acknowledge this interrupt to get more from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);

}

//
// xint2_isr - External Interrupt 2 ISR
//
interrupt void xint2_isr(void)
{
    XINT2Count++;

    //
    // Acknowledge this interrupt to get more from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
