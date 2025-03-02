/*
 * ads1248_pt100.h
 *
 *  Created on: 2025. 1. 11.
 *      Author: alllite
 */

#ifndef ADS1248_PT100_H_
#define ADS1248_PT100_H_

#include <stdint.h>
//#include "hw_types.h"


void Spi_Read_write_8bit(uint16_t * Pbuff,uint16_t Length,uint16_t uiBase);
void ADS1248_Init_1(void);
void ADS1248_Init_2(void);
void select_Channel(int16_t adcModuleCh, uint16_t ChnNum);
uint32_t spi_read_data(int16_t adcModuleCh);
void Spi_Read_write_8bit(uint16_t * Pbuff,uint16_t Length,uint16_t uiBase);
uint32_t spi_read_id(int16_t adcModuleCh);
int32_t ADS1248_readRegister();
int32_t ADS1248_writeRegister();
uint16_t readAdc (void) ;
uint32_t Get_data_ADS1248(int16_t tmepCh, uint16_t slot);
uint32_t Read_RTD_data (int16_t tmepCh, uint16_t slot);
float read_temperature(int16_t tmepCh, uint16_t slot);
float read_pr100(int16_t tmepCh, uint16_t ChnNum);
float calc_Temp_rtd(float resistance);
uint16_t search_rtd_array(float search_element);
float readRTDtemp(uint16_t ChnNum );

extern const float RTD_LUT [1051];

#endif /* ADS1248_PT100_H_ */
