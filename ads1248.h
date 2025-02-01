/*
 * ads1248.h
 *
 *  Created on: 2025. 1. 11.
 *      Author: alllite
 */

#ifndef ADS1248_H_
#define ADS1248_H_


/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
//#include "ADS1248m0x.h"
#include "stdint.h"
#include "config.h"

/*****************************************************************************/
/******************* ADS1248 Constants ****************************************/
/*****************************************************************************/
#define CRC8_POLYNOMIAL_REPRESENTATION 0x07 // x8 + x3 + x + 1

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
/*! Reads the value of the specified register. */
int32_t ADS1248_ReadRegister(st_reg* pReg);

/*! Writes the value of the specified register. */
int32_t ADS1248_WriteRegister(st_reg* reg);

/*! Resets the device. */
int32_t ADS1248_Reset(void);

/*! Waits until a new conversion result is available. */
int32_t ADS1248_WaitForReady(uint32_t timeout);

/*! Reads the conversion result from the device. */
//int32_t ADS1248_ReadData(int32_t* pData);
//extern long ADS1248_ReadData(long* pData);
long ADS1248_ReadData(long* pData);
/*! Computes the CRC checksum for a data buffer. */
//uint16_t ADS1248_ComputeCRC8(uint8_t* pBuf, uint8_t bufSize);

/*! Computes the XOR checksum for a data buffer. */
//uint16_t ADS1248_ComputeXOR8(uint8_t * pBuf, uint8_t bufSize);

/*! Updates the CRC settings. */
void ADS1248_UpdateCRCSetting(void);

/*! Initializes the ADS1248. */
int32_t ADS1248_Setup(int a);
int32_t ADS1248_Setup1(void);

uint16_t readSingleRegister(st_reg* pReg);

int itoa(int32_t anInteger , char *str);
void doubleToString(int32_t d, char* str, int type);
void intToString(int v, char* str);


extern void ADS1248_Calibration(void);

extern int AD7177_2_ReadData(char ch);
void Send_Data();
void uartSend();
extern void readDataCPU1(void);
extern void writeDataCPU1(int16_t *adc);

//long int ADC_CONVERSION_DATA[3] = {0};
//unsigned long ADC_CONVERSION_DATA[3] = {0.0};
//unsigned long LAST_ADC_CONVERSION_DATA[3] = {0.0};
//uint16_t adc_samplecnt[3] ={0};

//long lastAdcReadData;

extern int16_t offsetArray[20];
extern int16_t cpu1RArray[256];
extern int16_t gReadyAdcRead_flag;
extern bool gGoCal;
extern bool setFlag;




#endif /* ADS1248_H_ */
