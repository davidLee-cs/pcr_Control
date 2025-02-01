/*
 * dac53508.c
 *
 *  Created on: 2025. 1. 14.
 *      Author: alllite
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "dac53508_reg.h"
#include "dac53508.h"


int32_t dac53508_WriteRegister(dac_reg* reg);


void dac53508_init(void)
{





}

uint16_t  dac53508_write(void)
{
    uint16_t data;

    data = dac53508_WriteRegister(&dac53508_regs[STATUS_TRIGGER]);

    return data;
}


int32_t dac53508_WriteRegister(dac_reg* reg)
{
    int32_t ret=0;
    int32_t regValue = 0;
    uint16_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t wrxBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t i;

    /* Build the Command word */
    wrBuf[0] = (reg->addr << 8);
    wrBuf[1] = (reg->value & 0xFF00);
    wrBuf[2] = (reg->value & 0x00FF) << 8;


    GPIO_writePin(DAC_SPI_SS, 0);

    for (i=0; i< reg->size ; i++)
    {
        // Transmit data
        SPI_writeDataNonBlocking(SPIB_BASE, (wrBuf[i]));             // 0xFF00 으로 8 bit 데이터 쓰기 동작 !!!
        wrxBuf[i] = SPI_readDataBlockingNonFIFO(SPIB_BASE);
    }

    GPIO_writePin(DAC_SPI_SS, 1);

    return ret;
}
