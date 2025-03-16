/*
 * timer.c
 *
 *  Created on: 2025. 1. 20.
 *      Author: alllite
 */


#include "config.h"

uint16_t cpuTimer0IntCount;
uint16_t cpuTimer1_OneShotCnt = 0;
uint16_t tempProfileCnt[4] = {0,};
uint16_t tempCycleCnt[4] = {0,};
uint16_t cpuTimer2IntCount;

bool cputimer0Flag;
uint16_t stepState;
uint32_t timrepulseCount;
bool timerStart = 0;

void timerSet(void)
{

    //
    // ISRs for each CPU Timer interrupt
    //
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);
    Interrupt_register(INT_TIMER1, &cpuTimer1ISR);
//    Interrupt_register(INT_TIMER2, &cpuTimer2ISR);

    initCPUTimers();

    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 10000);  // 10mS
    configCPUTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, 1000000);           // 1000ms
//    configCPUTimer(CPUTIMER2_BASE, DEVICE_SYSCLK_FREQ, 20);

    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
//    CPUTimer_enableInterrupt(CPUTIMER2_BASE);

    Interrupt_enable(INT_TIMER0);
    Interrupt_enable(INT_TIMER1);
//    Interrupt_enable(INT_TIMER2);

    CPUTimer_startTimer(CPUTIMER0_BASE);
    CPUTimer_startTimer(CPUTIMER1_BASE);
//    CPUTimer_startTimer(CPUTIMER2_BASE);

}

//
// initCPUTimers - This function initializes all three CPU timers
// to a known state.
//
void
initCPUTimers(void)
{
    //
    // Initialize timer period to maximum
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    //
    // Make sure timer is stopped
    //
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    //
    // Reload all counter register with period value
    //
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    //
    // Reset interrupt counter
    //
    cpuTimer0IntCount = 0;
    cpuTimer2IntCount = 0;
}


//
// configCPUTimer - This function initializes the selected timer to the
// period specified by the "freq" and "period" parameters. The "freq" is
// entered as Hz and the period in uSeconds. The timer is held in the stopped
// state after configuration.
//
void
configCPUTimer(uint32_t cpuTimer, float freq, float period)
{
    uint32_t temp;

    //
    // Initialize timer period:
    //
    temp = (uint32_t)(freq / 1000000 * period);
    CPUTimer_setPeriod(cpuTimer, temp);

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(cpuTimer, 0);

    //
    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    //
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(cpuTimer);

    //
    // Resets interrupt counters for the three cpuTimers
    //
    if (cpuTimer == CPUTIMER0_BASE)
    {
        cpuTimer0IntCount = 0;
    }
    else if(cpuTimer == CPUTIMER1_BASE)
    {
        tempProfileCnt[0] = 0;
        tempProfileCnt[1] = 0;
        tempProfileCnt[2] = 0;
        tempProfileCnt[3] = 0;
    }
    else if(cpuTimer == CPUTIMER2_BASE)
    {
        cpuTimer2IntCount = 0;
    }
}

//
// cpuTimer0ISR - Counter for CpuTimer0
//
__interrupt void
cpuTimer0ISR(void)
{
    cpuTimer0IntCount++;
    tempCheck10msCnt[0]++;
    tempCheck10msCnt[1]++;
    tempCheck10msCnt[2]++;
    tempCheck10msCnt[3]++;

    cputimer0Flag = TRUE;

//    // 펄스 갯수가 목표 값에 도달했으면 모터를 멈추고 인터럽트를 더 이상 발생시키지 않음
//    if (pulseCount >= targetPulseCount) {
//        // 모터 멈추기
//        GPIO_writePin(STEP_0, 0);;  // STEP = 0
//        EnableMotor(0);  // 모터 비활성화
//        return; // 펄스 갯수가 다 찼으면 인터럽트를 종료
//    }

//    if (stepState == 0) {
////        GPIO_writePin(STEP_0, 1);  // STEP = 1
//        stepState = 1;
//    } else {
////        GPIO_writePin(STEP_0, 0); // STEP = 0
//        stepState = 0;
//        pulseCount++;  // 펄스 카운트 증가
//    }

    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// cpuTimer1ISR - Counter for CpuTimer1
//
__interrupt void
cpuTimer1ISR(void)
{
    //
    // The CPU acknowledges the interrupt.
    //
    tempProfileCnt[0]++;
    tempProfileCnt[1]++;
    tempProfileCnt[2]++;
    tempProfileCnt[3]++;
    cpuTimer1_OneShotCnt++;
}

//
// cpuTimer2ISR - Counter for CpuTimer2
//
__interrupt void
cpuTimer2ISR(void)
{
    //
    // The CPU acknowledges the interrupt.
    //
    cpuTimer2IntCount++;

    // 펄스 갯수가 목표 값에 도달했으면 모터를 멈추고 인터럽트를 더 이상 발생시키지 않음
//    if (pulseCount >= targetPulseCount) {
//        // 모터 멈추기
//        GPIO_writePin(STEP_0, 0);;  // STEP = 0
//        EnableMotor(0);  // 모터 비활성화
//        return; // 펄스 갯수가 다 찼으면 인터럽트를 종료
//    }
//    if(timerStart)
//    {
//        EnableMotor(1);  // 모터 비활성화
//        if (stepState ^= 1) {
//            GPIO_writePin(STEP_0, 1);  // STEP = 1
//    //        stepState = 1;
//        } else {
//            GPIO_writePin(STEP_0, 0); // STEP = 0
//    //        stepState = 0;
//            timrepulseCount++;  // 펄스 카운트 증가
//        }
//    }
//    else
//    {
//        EnableMotor(0);  // 모터 비활성화
//    }

}

