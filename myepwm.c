/*
 * epwm.c
 *
 *  Created on: 2025. 1. 25.
 *      Author: alllite
 */


#include "driverlib.h"
#include "device.h"
#include "config.h"
#include "cmdMsg.h"
#include "operationlib.h"


epwmInfo epwm1Info;
epwmInfo epwm2Info;
epwmInfo epwm3Info;
epwmInfo epwm4Info;

volatile uint64_t pulseCount[4] = {0,};     // 현재까지 발생한 펄스 수
volatile uint64_t targetPulseCount = 100000; // 목표 펄스 수 (예: 1000펄스)
volatile uint32_t stepDelay = 1000;   // STEP 펄스 간격 (속도 제어용)

volatile uint16_t compAVal, compBVal;



void epwmEnableSet(uint16_t ch)
{
    switch(ch)
    {
        case PUMP_01 :

            EPWM_setTimeBaseCounterMode(myStepMotorEPWM1_BASE, EPWM_COUNTER_MODE_UP);
            EPWM_enableInterrupt(myStepMotorEPWM1_BASE);
            Interrupt_enable(INT_EPWM1);
            break;

        case PUMP_23 :
            EPWM_setTimeBaseCounterMode(myStepMotorEPWM2_BASE, EPWM_COUNTER_MODE_UP);
            EPWM_enableInterrupt(myStepMotorEPWM2_BASE);
            Interrupt_enable(INT_EPWM2);
            break;

        case STEP_01 :
            EPWM_setTimeBaseCounterMode(myStepMotorEPWM3_BASE, EPWM_COUNTER_MODE_UP);
            EPWM_enableInterrupt(myStepMotorEPWM3_BASE);
            Interrupt_enable(INT_EPWM3);
            break;

        case STEP_23 :
            EPWM_setTimeBaseCounterMode(myStepMotorEPWM4_BASE, EPWM_COUNTER_MODE_UP);
            EPWM_enableInterrupt(myStepMotorEPWM4_BASE);
            Interrupt_enable(INT_EPWM4);
            break;
    }
}

void epwmDisableSet(uint16_t ch)
{

    switch(ch)
    {

        case PUMP_01 :

            EPWM_setTimeBaseCounterMode(myStepMotorEPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
            EPWM_disableInterrupt(myStepMotorEPWM1_BASE);
            Interrupt_disable(INT_EPWM1);
            break;

        case PUMP_23 :
            EPWM_setTimeBaseCounterMode(myStepMotorEPWM2_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
            EPWM_disableInterrupt(myStepMotorEPWM2_BASE);
            Interrupt_disable(INT_EPWM2);
            break;

        case STEP_01 :
            EPWM_setTimeBaseCounterMode(myStepMotorEPWM3_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
            EPWM_disableInterrupt(myStepMotorEPWM3_BASE);
            Interrupt_disable(INT_EPWM3);
            break;

        case STEP_23 :
            EPWM_setTimeBaseCounterMode(myStepMotorEPWM4_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
            EPWM_disableInterrupt(myStepMotorEPWM4_BASE);
            Interrupt_disable(INT_EPWM4);
            break;

         default :
            break;

    }

//    Interrupt_disable(INT_EPWM1);
//    Interrupt_disable(INT_EPWM2);
//    Interrupt_disable(INT_EPWM3);

}

void epwmSet(void)
{

    //
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    //
    Interrupt_register(INT_EPWM1, &epwm1ISR);
    Interrupt_register(INT_EPWM2, &epwm2ISR);
    Interrupt_register(INT_EPWM3, &epwm3ISR);
    Interrupt_register(INT_EPWM4, &epwm4ISR);

    //
    // Disable sync(Freeze clock to PWM as well)
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    initEPWM1();
    initEPWM2();
    initEPWM3();
    initEPWM4();
    //
    // Enable sync and clock to PWM
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    EPWM_disableInterrupt(myStepMotorEPWM1_BASE);     //
    EPWM_disableInterrupt(myStepMotorEPWM2_BASE);     //
    EPWM_disableInterrupt(myStepMotorEPWM3_BASE);     //
    EPWM_disableInterrupt(myStepMotorEPWM4_BASE);     //
    //
    // Enable interrupts required for this example
    //
//    Interrupt_enable(INT_EPWM1);
//    Interrupt_enable(INT_EPWM2);
//    Interrupt_enable(INT_EPWM3);
//    Interrupt_enable(INT_EPWM4);
}


//
// epwm1ISR - EPWM1 ISR to update compare values
//
__interrupt void epwm1ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
//    updateCompare(&epwm1Info);
    int16_t enablecheck1=0, enablecheck2=0;

    if((OpSwitchStatus.button0 == 0) || (HostCmdMsg[0].oprationSetBit.pumpRun == 0))
    {
        enablecheck1 = 1;
//        EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_B);
//        EPWM_setActionQualifierSWAction(myStepMotorEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW);
        EPWM_setCounterCompareValue(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_B, 2001); // 하드웨어 방법
//            epwmDisableSet(STEP_01);
    }

    if((OpSwitchStatus.button1 == 0) || (HostCmdMsg[1].oprationSetBit.pumpRun == 0))
    {
        enablecheck2 = 1;
//        EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_A);
//        EPWM_setActionQualifierSWAction(myStepMotorEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW);
            EPWM_setCounterCompareValue(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_A, 2001); // 하드웨어 방법
//            epwmDisableSet(STEP_01);
    }


    if((enablecheck1 == 1) && (enablecheck2 == 1))
    {
        epwmDisableSet(PUMP_01);
    }

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(myStepMotorEPWM1_BASE);

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

#if 1
//
// epwm2ISR - EPWM2 ISR to update compare values
//
__interrupt void epwm2ISR(void)
{


    int16_t enablecheck1=0, enablecheck2=0;

    if((OpSwitchStatus.button2 == 0) || (HostCmdMsg[2].oprationSetBit.pumpRun == 0))
    {
        enablecheck1 = 1;

        EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_B);
//        EPWM_setActionQualifierSWAction(myStepMotorEPWM2_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW);
        EPWM_setCounterCompareValue(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_B, 2001); // 하드웨어 방법
//            epwmDisableSet(STEP_01);
    }

    if((OpSwitchStatus.button3 == 0) || (HostCmdMsg[3].oprationSetBit.pumpRun == 0))
    {
        enablecheck2 = 1;
        EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_A);
//        EPWM_setActionQualifierSWAction(myStepMotorEPWM2_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW);
        EPWM_setCounterCompareValue(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_A, 2001); // 하드웨어 방법
//            epwmDisableSet(STEP_01);
    }


    if((enablecheck1 == 1) && (enablecheck2 == 1))
    {
        epwmDisableSet(PUMP_23);
    }

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(myStepMotorEPWM2_BASE);

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

//
// epwm3ISR - EPWM3 ISR to update compare values
//
__interrupt void epwm3ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
//    updateCompare(&epwm3Info);
    if(HostCmdMsg[2].oprationSetBit.motorRun == 1)
    {
        if(pulseCount[2] > OpCmdMsg[2].stepperPulseCnt)

        {
            // 모터 멈추기
    //        EPWM_setTimeBaseCounterMode(myStepMotorEPWM4_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);

    //        EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A);
            drv8452_outDisable(2);
            EPWM_setActionQualifierSWAction(myStepMotorEPWM3_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW);
//            EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM3_BASE, EPWM_COUNTER_COMPARE_B);  //
    //        EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM3_BASE, EPWM_COUNTER_COMPARE_B);
            EnableMotor(1);  // 모터 활성화
            epwmDisableSet(STEP_01);
            pulseCount[2] = 0;

        }
        else
        {
             pulseCount[2]++;  // 펄스 카운트 증가

        }
    }

    if(HostCmdMsg[3].oprationSetBit.motorRun == 1)
    {
        if(pulseCount[3] > OpCmdMsg[3].stepperPulseCnt)

        {
            // 모터 멈추기
    //        EPWM_setTimeBaseCounterMode(myStepMotorEPWM4_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);

    //        EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A);
            drv8452_outDisable(3);
            EPWM_setActionQualifierSWAction(myStepMotorEPWM3_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW);
//            EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM3_BASE, EPWM_COUNTER_COMPARE_A);  //
    //        EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM3_BASE, EPWM_COUNTER_COMPARE_B);
            EnableMotor(1);  // 모터 활성화
            epwmDisableSet(STEP_01);
            pulseCount[3] = 0;

        }
        else
        {
             pulseCount[3]++;  // 펄스 카운트 증가

        }
    }
    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(myStepMotorEPWM3_BASE);

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

//
// epwm3ISR - EPWM3 ISR to update compare values
//
__interrupt void epwm4ISR(void)
{

//    updateCompare(&epwm4Info);
//    if ((pulseCount >= targetPulseCount) ||
//            (OpCmdMsg.motorMovingStatus.motor4_Home_bit == 0) ||
//            (OpCmdMsg.motorMovingStatus.motor4_End_bit == 0))

    if(HostCmdMsg[0].oprationSetBit.motorRun == 1)
    {
        if(pulseCount[0] > OpCmdMsg[0].stepperPulseCnt)

        {
            // 모터 멈추기
            drv8452_outDisable(0);
//            EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_B);
            EPWM_setActionQualifierSWAction(myStepMotorEPWM4_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW);
            EnableMotor(1);  // 모터 활성화
            epwmDisableSet(STEP_23);
            pulseCount[0] = 0;

        }
        else
        {
             pulseCount[0]++;  // 펄스 카운트 증가

        }
    }

    if(HostCmdMsg[1].oprationSetBit.motorRun == 1)
    {
        if(pulseCount[1] > OpCmdMsg[1].stepperPulseCnt)

        {
            // 모터 멈추기
            drv8452_outDisable(1);
//            EPWM_disableCounterCompareShadowLoadMode(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A);
            EPWM_setActionQualifierSWAction(myStepMotorEPWM4_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW);
//            EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A, 2001); // 하드웨어 방법
            EnableMotor(1);  // 모터 활성화
            epwmDisableSet(STEP_23);
            pulseCount[1] = 0;

        }
        else
        {
             pulseCount[1]++;  // 펄스 카운트 증가

        }
    }


    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(myStepMotorEPWM4_BASE); // 0225 수정

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

#endif
//
// initEPWM1 - Initialize EPWM1 values
//
void initEPWM1()
{
    //
    // Setup TBCLK
    //
    EPWM_setTimeBaseCounterMode(myStepMotorEPWM1_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setTimeBasePeriod(myStepMotorEPWM1_BASE, EPWM1_TIMER_TBPRD);
    EPWM_disablePhaseShiftLoad(myStepMotorEPWM1_BASE);
    EPWM_setPhaseShift(myStepMotorEPWM1_BASE, 0U);
    EPWM_setTimeBaseCounter(myStepMotorEPWM1_BASE, 0U);

    //
    // Set ePWM clock pre-scaler
    //
    EPWM_setClockPrescaler(myStepMotorEPWM1_BASE,
                           EPWM_CLOCK_DIVIDER_2,
                           EPWM_HSCLOCK_DIVIDER_2);

    //
    // Setup shadow register load on ZERO
    //
    EPWM_setCounterCompareShadowLoadMode(myStepMotorEPWM1_BASE,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(myStepMotorEPWM1_BASE,
                                         EPWM_COUNTER_COMPARE_B,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    //
    // Set Compare values
    //
    EPWM_setCounterCompareValue(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_A,
                                EPWM1_MIN_CMPA);
    EPWM_setCounterCompareValue(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_B,
                                EPWM1_MIN_CMPB);

    //
    // Set actions for ePWM1A & ePWM1B
    //
    // Set PWM1A on Zero
    EPWM_setActionQualifierAction(myStepMotorEPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    // Clear PWM1A on event A, up count
    EPWM_setActionQualifierAction(myStepMotorEPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    // Set PWM1B on Zero
    EPWM_setActionQualifierAction(myStepMotorEPWM1_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    // Clear PWM1B on event B, up count
    EPWM_setActionQualifierAction(myStepMotorEPWM1_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);

    //                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    // Interrupt where we will change the Compare Values
    //
    EPWM_setInterruptSource(myStepMotorEPWM1_BASE, EPWM_INT_TBCTR_ZERO);
    EPWM_enableInterrupt(myStepMotorEPWM1_BASE);
    EPWM_setInterruptEventCount(myStepMotorEPWM1_BASE, 3U);

   //
   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   //

    // Start by increasing CMPA & CMPB
    epwm1Info.epwmCompADirection = EPWM_CMP_UP;
    epwm1Info.epwmCompBDirection = EPWM_CMP_UP;

    // Clear interrupt counter
    epwm1Info.epwmTimerIntCount = 0;

    // Set base as ePWM1
//    epwm1Info.epwmModule = myStepMotorEPWM4_BASE;

    // Setup min/max CMPA/CMP values
    epwm1Info.epwmMaxCompA = EPWM1_MAX_CMPA;
    epwm1Info.epwmMinCompA = EPWM1_MIN_CMPA;
    epwm1Info.epwmMaxCompB = EPWM1_MAX_CMPB;
    epwm1Info.epwmMinCompB = EPWM1_MIN_CMPB;
}

#if 1

//
// initEPWM2 - Initialize EPWM2 values
//
void initEPWM2()
{
    //
    // Setup TBCLK
    //
    EPWM_setTimeBaseCounterMode(myStepMotorEPWM2_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setTimeBasePeriod(myStepMotorEPWM2_BASE, EPWM2_TIMER_TBPRD);
    EPWM_disablePhaseShiftLoad(myStepMotorEPWM2_BASE);
    EPWM_setPhaseShift(myStepMotorEPWM2_BASE, 0U);
    EPWM_setTimeBaseCounter(myStepMotorEPWM2_BASE, 0U);

    //
    // Set ePWM clock pre-scaler
    //
    EPWM_setClockPrescaler(myStepMotorEPWM2_BASE,
                           EPWM_CLOCK_DIVIDER_2,
                           EPWM_HSCLOCK_DIVIDER_2);

    //
    // Setup shadow register load on ZERO
    //
    EPWM_setCounterCompareShadowLoadMode(myStepMotorEPWM2_BASE,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(myStepMotorEPWM2_BASE,
                                         EPWM_COUNTER_COMPARE_B,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    //
    // Set Compare values
    //
    EPWM_setCounterCompareValue(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_A,
                                EPWM2_MIN_CMPA);
    EPWM_setCounterCompareValue(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_B,
                                EPWM2_MAX_CMPB);

    //
    // Set actions for ePWM1A & ePWM1B
    //
    // Clear PWM2A on period and set on event A, up-count
    EPWM_setActionQualifierAction(myStepMotorEPWM2_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(myStepMotorEPWM2_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    // Clear PWM2B on Period & set on event B, up-count
    EPWM_setActionQualifierAction(myStepMotorEPWM2_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(myStepMotorEPWM2_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);

    //
    // Interrupt where we will change the Compare Values
    //
    EPWM_setInterruptSource(myStepMotorEPWM2_BASE, EPWM_INT_TBCTR_ZERO);
    EPWM_enableInterrupt(myStepMotorEPWM2_BASE);
    EPWM_setInterruptEventCount(myStepMotorEPWM2_BASE, 3U);

    //
    // Information this example uses to keep track
    // of the direction the CMPA/CMPB values are
    // moving, the min and max allowed values and
    // a pointer to the correct ePWM registers
    //

    // Start by increasing CMPA & decreasing CMPB
    epwm2Info.epwmCompADirection = EPWM_CMP_UP;
    epwm2Info.epwmCompBDirection = EPWM_CMP_DOWN;

    // Clear interrupt counter
    epwm2Info.epwmTimerIntCount = 0;

    // Set base as ePWM2
    epwm2Info.epwmModule = myStepMotorEPWM2_BASE;

    // Setup min/max CMPA/CMP values
    epwm2Info.epwmMaxCompA = EPWM2_MAX_CMPA;
    epwm2Info.epwmMinCompA = EPWM2_MIN_CMPA;
    epwm2Info.epwmMaxCompB = EPWM2_MAX_CMPB;
    epwm2Info.epwmMinCompB = EPWM2_MIN_CMPB;
}

//
// initEPWM3 - Initialize EPWM3 values
//
void initEPWM3(void)
{
    //
    // Setup TBCLK
    //
    EPWM_setTimeBaseCounterMode(myStepMotorEPWM3_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setTimeBasePeriod(myStepMotorEPWM3_BASE, EPWM3_TIMER_TBPRD);
    EPWM_disablePhaseShiftLoad(myStepMotorEPWM3_BASE);
    EPWM_setPhaseShift(myStepMotorEPWM3_BASE, 0U);
    EPWM_setTimeBaseCounter(myStepMotorEPWM3_BASE, 0U);

    //
    // Set ePWM clock pre-scaler
    //
    EPWM_setClockPrescaler(myStepMotorEPWM3_BASE,
                           EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_1);

    //
    // Setup shadow register load on ZERO
    //
    EPWM_setCounterCompareShadowLoadMode(myStepMotorEPWM3_BASE,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(myStepMotorEPWM3_BASE,
                                         EPWM_COUNTER_COMPARE_B,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    //
    // Set Compare values
    //
    EPWM_setCounterCompareValue(myStepMotorEPWM3_BASE, EPWM_COUNTER_COMPARE_A,
                                EPWM3_MIN_CMPA);
    EPWM_setCounterCompareValue(myStepMotorEPWM3_BASE, EPWM_COUNTER_COMPARE_B,
                                EPWM3_MAX_CMPB);

    //
    // Set actions for ePWM1A & ePWM1B
    //
    // Set PWM3A on event B, up-count & clear on event B, up-count
//    EPWM_setActionQualifierAction(myStepMotorEPWM3_BASE,
//                                  EPWM_AQ_OUTPUT_A,
//                                  EPWM_AQ_OUTPUT_HIGH,
//                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
//    EPWM_setActionQualifierAction(myStepMotorEPWM3_BASE,
//                                  EPWM_AQ_OUTPUT_A,
//                                  EPWM_AQ_OUTPUT_LOW,
//                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(myStepMotorEPWM3_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_TOGGLE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);


    // Toggle EPWM3B on counter = zero
    EPWM_setActionQualifierAction(myStepMotorEPWM3_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_TOGGLE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);

    //
    // Interrupt where we will change the Compare Values
    //
    EPWM_setInterruptSource(myStepMotorEPWM3_BASE, EPWM_INT_TBCTR_ZERO);
    EPWM_enableInterrupt(myStepMotorEPWM3_BASE);
    EPWM_setInterruptEventCount(myStepMotorEPWM3_BASE, 3U);

   //
   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   //

    // Start by increasing CMPA & decreasing CMPB
    epwm3Info.epwmCompADirection = EPWM_CMP_UP;
    epwm3Info.epwmCompBDirection = EPWM_CMP_UP;

    // Start the count at 0
    epwm3Info.epwmTimerIntCount = 0;

    // Set base as ePWM1
    epwm3Info.epwmModule = myStepMotorEPWM3_BASE;

    // Setup min/max CMPA/CMP values
    epwm3Info.epwmMaxCompA = EPWM3_MAX_CMPA;
    epwm3Info.epwmMinCompA = EPWM3_MIN_CMPA;
    epwm3Info.epwmMaxCompB = EPWM3_MAX_CMPB;
    epwm3Info.epwmMinCompB = EPWM3_MIN_CMPB;
}

#if 1
void initEPWM4(void)
{
    //
    // Setup TBCLK
    //
    EPWM_setTimeBaseCounterMode(myStepMotorEPWM4_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setTimeBasePeriod(myStepMotorEPWM4_BASE, EPWM4_TIMER_TBPRD);
    EPWM_disablePhaseShiftLoad(myStepMotorEPWM4_BASE);
    EPWM_setPhaseShift(myStepMotorEPWM4_BASE, 0U);
    EPWM_setTimeBaseCounter(myStepMotorEPWM4_BASE, 0U);

    //
    // Set ePWM clock pre-scaler
    //
    EPWM_setClockPrescaler(myStepMotorEPWM4_BASE,
                           EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_1);

    //
    // Setup shadow register load on ZERO
    //
    EPWM_setCounterCompareShadowLoadMode(myStepMotorEPWM4_BASE,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(myStepMotorEPWM4_BASE,
                                         EPWM_COUNTER_COMPARE_B,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    //
    // Set Compare values
    //
    EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A,
                                EPWM4_MIN_CMPA);
    EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_B,
                                EPWM4_MAX_CMPB);

    //
    // Set actions for ePWM1A & ePWM1B
    //
    // Set PWM3A on event B, up-count & clear on event B, up-count
    EPWM_setActionQualifierAction(myStepMotorEPWM4_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(myStepMotorEPWM4_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);

    // Toggle EPWM3B on counter = zero
    EPWM_setActionQualifierAction(myStepMotorEPWM4_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_TOGGLE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);

    //
    // Interrupt where we will change the Compare Values
    //
    EPWM_setInterruptSource(myStepMotorEPWM4_BASE, EPWM_INT_TBCTR_ZERO);
    EPWM_enableInterrupt(myStepMotorEPWM4_BASE);
    EPWM_setInterruptEventCount(myStepMotorEPWM4_BASE, 3U);

   //
   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   //

    // Start by increasing CMPA & decreasing CMPB
    epwm4Info.epwmCompADirection = EPWM_CMP_UP;
    epwm4Info.epwmCompBDirection = EPWM_CMP_UP;

    // Start the count at 0
    epwm4Info.epwmTimerIntCount = 0;

    // Set base as ePWM1
    epwm4Info.epwmModule = myStepMotorEPWM4_BASE;

    // Setup min/max CMPA/CMP values
    epwm4Info.epwmMaxCompA = EPWM4_MAX_CMPA;
    epwm4Info.epwmMinCompA = EPWM4_MIN_CMPA;
    epwm4Info.epwmMaxCompB = EPWM4_MAX_CMPB;
    epwm4Info.epwmMinCompB = EPWM4_MIN_CMPB;

}

#endif
//
// updateCompare - Update the compare values for the specified EPWM
//
void updateCompare(epwmInfo *epwm_info)
{

       compAVal = EPWM_getCounterCompareValue(epwm_info->epwmModule,
                                              EPWM_COUNTER_COMPARE_A);
       compBVal = EPWM_getCounterCompareValue(epwm_info->epwmModule,
                                              EPWM_COUNTER_COMPARE_B);

       EPWM_setCounterCompareValue(epwm_info->epwmModule,
                                   EPWM_COUNTER_COMPARE_A, ++compAVal);

       EPWM_setCounterCompareValue(epwm_info->epwmModule,
                                   EPWM_COUNTER_COMPARE_B, --compAVal);



   return;
}


void stepperEpwmSet(int16_t channel, uint16_t speed)
{

    uint16_t targetSpeed = (float)(speed *0.01) * 2200;
    if(targetSpeed > 3000)
    {
        targetSpeed = 3000;
    }

    if(targetSpeed < 1100)
    {
        targetSpeed = 1100;
    }


    uint16_t compare = targetSpeed / 2;

#if 1
    if(channel == 3)
    {
        EPWM_setTimeBasePeriod(myStepMotorEPWM3_BASE, targetSpeed);
        EPWM_setCounterCompareValue(myStepMotorEPWM3_BASE, EPWM_COUNTER_COMPARE_A, compare);
//        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_B, compare);

    }
    else if(channel == 2)
    {
        EPWM_setTimeBasePeriod(myStepMotorEPWM3_BASE, targetSpeed);
//        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A, compare);
        EPWM_setCounterCompareValue(myStepMotorEPWM3_BASE, EPWM_COUNTER_COMPARE_B, compare);

    }
    else if(channel == 1)
    {
        EPWM_setTimeBasePeriod(myStepMotorEPWM4_BASE, targetSpeed);
        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A, compare);
//        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_B, compare);

    }
    else if(channel == 0)
    {
        EPWM_setTimeBasePeriod(myStepMotorEPWM4_BASE, targetSpeed);
//        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A, compare);
        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_B, compare);
    }
#endif
}


void pumpEpwmSet(int16_t channel, uint16_t duty)
{

    uint16_t targetSpeed = 2000;

    if(duty > 90)
    {
        duty = 90;
    }

    uint16_t compare = targetSpeed * (float)(duty * 0.01);


#if 1
    if(channel == 3)
    {
        EPWM_setTimeBasePeriod(myStepMotorEPWM2_BASE, targetSpeed);
        EPWM_setCounterCompareValue(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_A, compare);
//        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_B, compare);

    }
    else if(channel == 2)
    {
        EPWM_setTimeBasePeriod(myStepMotorEPWM2_BASE, targetSpeed);
//        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A, compare);
        EPWM_setCounterCompareValue(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_B, compare);

    }
    else if(channel == 1)
    {
        EPWM_setTimeBasePeriod(myStepMotorEPWM1_BASE, targetSpeed);
        EPWM_setCounterCompareValue(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_A, compare);
//        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_B, compare);

    }
    else if(channel == 0)
    {
        EPWM_setTimeBasePeriod(myStepMotorEPWM1_BASE, targetSpeed);
//        EPWM_setCounterCompareValue(myStepMotorEPWM4_BASE, EPWM_COUNTER_COMPARE_A, compare);
        EPWM_setCounterCompareValue(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_B, compare);
    }
#endif
}


//void pumpEpwmSet(uint16_t duty)
//{
//
//    uint16_t targetSpeed = 2000;
//
//    if(duty > 90)
//    {
//        duty = 90;
//    }
//
//    if(duty < 10)
//    {
//        duty = 10;
//    }
//
//    uint16_t compare = targetSpeed * duty * 0.01;
//
//    EPWM_setTimeBasePeriod(myStepMotorEPWM1_BASE, targetSpeed);
//    EPWM_setTimeBasePeriod(myStepMotorEPWM2_BASE, targetSpeed);
//
//    EPWM_setCounterCompareValue(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_A, compare);
//    EPWM_setCounterCompareValue(myStepMotorEPWM1_BASE, EPWM_COUNTER_COMPARE_B, compare);
//    EPWM_setCounterCompareValue(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_A, compare);
//    EPWM_setCounterCompareValue(myStepMotorEPWM2_BASE, EPWM_COUNTER_COMPARE_B, compare);
//
//}


void stepperPulseSet(int16_t channel, uint64_t pulse)
{

    OpCmdMsg[channel].stepperPulseCnt = pulse;

}

#endif




