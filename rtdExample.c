/**
 * 
 * @file rtdExample.c 
 * 
 * @brief Example of RTD temperature reading
 * @warning This software utilizes TI Drivers
 * 
 * @copyright Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
 * All rights reserved. 
 * 
 * Software License Agreement
 * Texas Instruments (TI) is supplying this software for use solely and
 * exclusively on TI's Microcontroller and/or Data Converter products. 
 * The software is owned by TI and/or its suppliers, and is protected 
 * under applicable copyright laws.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 * NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
 * NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
  */ 
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

/* ADC related includes */
#include "inc/adchal.h"
#include "inc/ads124s08.h"
#include "inc/ads124s08EVM.h"

/* RTD related includes */
#include "inc/rtd.h"

/* Driver header files */
#include <ti/display/Display.h>

extern Display_Handle  displayHdl;

typedef enum RTDExampleDef{
    RTD_2_Wire_Fig15,     // 2-Wire RTD example using ADS124S08 EVM, User's Guide Figure 15
    RTD_3_Wire_Fig14,     // 3-Wire RTD example using ADS124S08 EVM, User's Guide Figure 14
    RTD_4_Wire_Fig16,     // 4-Wire RTD example using ADS124S08 EVM, User's Guide Figure 16
} RTD_Example;


/************************************************************************************//**
 *
 * @brief readRTDtemp()
 *             Reads RTD temperature
 *
 * @return     none
 */
void readRTDtemp( SPI_Handle spiHdl )
{
    RTD_Set     *rtdSet = NULL;
    RTD_Type    rtdType = Pt;
    RTD_Example rtdExample = RTD_2_Wire_Fig15;
    float       rtdRes, rtdTemp;
    ADCchar_Set adcChars, adcChars2;
    uint8_t     status;


    switch ( rtdType ) {
        case Pt:
            rtdSet = RTD_Init( Pt, 100, European );
            break;
        
        case Ni:
            rtdSet = RTD_Init( Ni, 100, European );
            break;
        
        case Cu:
            rtdSet = RTD_Init( Cu, 100, European );
            break;
        
        case NiFe:
            rtdSet = RTD_Init( NiFe, 100, European );
            break;
    }
    
    switch ( rtdExample ) {
        case RTD_2_Wire_Fig15:
            adcChars.inputMuxConfReg = RTD_TWO_WIRE_INPUT_MUX;
            adcChars.pgaReg          = RTD_TWO_WIRE_PGA;
            adcChars.dataRateReg     = RTD_TWO_WIRE_DATARATE; 
            adcChars.refSelReg       = RTD_TWO_WIRE_REF_SEL; 
            adcChars.IDACmagReg      = RTD_TWO_WIRE_IDACMAG;
            adcChars.IDACmuxReg      = RTD_TWO_WIRE_IDACMUX;
            adcChars.Vref            = RTD_TWO_WIRE_EXT_VREF;
            rtdSet->Rref             = RTD_TWO_WIRE_REF_RES;
            rtdSet->wiring           = Two_Wire_High_Side_Ref;
            break;
        case RTD_3_Wire_Fig14:
            adcChars.inputMuxConfReg = RTD_THREE_WIRE_INPUT_MUX;
            adcChars.pgaReg          = RTD_THREE_WIRE_PGA;
            adcChars.dataRateReg     = RTD_THREE_WIRE_DATARATE;
            adcChars.refSelReg       = RTD_THREE_WIRE_REF_SEL;
            adcChars.IDACmagReg      = RTD_THREE_WIRE_IDACMAG;
            adcChars.IDACmuxReg      = RTD_THREE_WIRE_IDACMUX;
            adcChars.Vref            = RTD_THREE_WIRE_EXT_VREF;
            rtdSet->Rref             = RTD_THREE_WIRE_REF_RES;
            rtdSet->wiring           = Three_Wire_High_Side_Ref_Two_IDAC;
            break;
        case RTD_4_Wire_Fig16:
            adcChars.inputMuxConfReg = RTD_FOUR_WIRE_INPUT_MUX;
            adcChars.pgaReg          = RTD_FOUR_WIRE_PGA;
            adcChars.dataRateReg     = RTD_FOUR_WIRE_DATARATE;
            adcChars.refSelReg       = RTD_FOUR_WIRE_REF_SEL;
            adcChars.IDACmagReg      = RTD_FOUR_WIRE_IDACMAG;
            adcChars.IDACmuxReg      = RTD_FOUR_WIRE_IDACMUX;
            adcChars.Vref            = RTD_FOUR_WIRE_INT_VREF;
            rtdSet->Rref             = RTD_FOUR_WIRE_REF_RES;
            rtdSet->wiring           = Four_Wire_High_Side_Ref;
            break;
    }
    adcChars.VBIASReg = RTD_VBIAS;
    
    /* add in device initialization */
    if ( !InitADCPeripherals( &adcChars, &spiHdl ) ) {
        Display_printf( displayHdl, 0, 0, "Error initializing master SPI\n" );
        while (1);
    }

    do {
        if ( waitForDRDYHtoL( TIMEOUT_COUNTER ) ) {
//            adcChars.adcValue1 = readConvertedData( spiHdl, &status, COMMAND );
            adcChars.adcValue1 = spi_read_data();
            // For 3-wire with one IDAC, multiple readings are needed. So reconfigure ADC to read 2nd channel
/*            if ( rtdSet->wiring == Three_Wire_High_Side_Ref_One_IDAC || rtdSet->wiring == Three_Wire_Low_Side_Ref_One_IDAC ) {
                adcChars2.inputMuxConfReg = RTD_THREE_WIRE_INPUT_MUX2;
                adcChars2.pgaReg          = RTD_THREE_WIRE_PGA;
                adcChars2.dataRateReg     = RTD_THREE_WIRE_DATARATE;
                adcChars2.refSelReg       = RTD_THREE_WIRE_REF_SEL;
                adcChars2.IDACmagReg      = RTD_THREE_WIRE_IDACMAG;
                adcChars2.IDACmuxReg      = RTD_THREE_WIRE_IDACMUX;
                adcChars2.Vref            = RTD_THREE_WIRE_EXT_VREF;
                adcChars2.VBIASReg        = VBIAS_DEFAULT;

                
                if ( !ReconfigureADC( &adcChars2, spiHdl ) ) {
                    Display_printf( displayHdl, 0, 0, "Error reconfiguring ADC\n" );
                    while (1);
                }
                // Store second channel value into previous ADC structure as adcValue2
                if ( waitForDRDYHtoL( TIMEOUT_COUNTER ) ) {
                   adcChars.adcValue2 = readConvertedData( spiHdl, &status, COMMAND );
                } else {
                    Display_printf( displayHdl, 0, 0, "Timeout on conversion\n" );
                    while (1);
                }
            }*/
            // Convert ADC values RTD resistance
            rtdRes = Convert_Code2RTD_Resistance( &adcChars, rtdSet  );
            // Convert RTD resistance to temperature and linearize
            rtdTemp = RTD_Linearization( rtdSet, rtdRes );
            Display_printf( displayHdl, 0, 0, "ADC conversion result 1: %i\n", adcChars.adcValue1 );
           if ( rtdSet->wiring == Three_Wire_High_Side_Ref_One_IDAC || rtdSet->wiring == Three_Wire_Low_Side_Ref_One_IDAC ) {
                Display_printf( displayHdl, 0, 0, "ADC conversion result 2: %i\n", adcChars.adcValue2 );
            }
            if ( isnan(rtdTemp) ) {
                Display_printf( displayHdl, 0, 0, "RTD temperature: NaN \n\n" );
            } else {
                Display_printf( displayHdl, 0, 0, "RTD temperature: %.3f (C)\n\n", rtdTemp );
            }
            
        } else {
            Display_printf( displayHdl, 0, 0, "Timeout on conversion\n" );
            while (1);
        }

    } while (1);

}

