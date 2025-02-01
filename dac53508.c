/*
 * dac53508.c
 *
 *  Created on: 2025. 1. 14.
 *      Author: alllite
 */

#include "config.h"
#include "dac53508_reg.h"


int32_t dac53508_WriteRegister(dac_reg* reg);


void dac53508_init(void)
{
    uint16_t data;
    data = dac53508_WriteRegister(&dac53508_regs[DEVICE_CONFIG]);

}

uint16_t  dac53508_write(uint16_t data)
{
    uint16_t done;

    dac53508_regs[DACA_DATA].value = data;
    dac53508_regs[DACB_DATA].value = data;
    dac53508_regs[DACC_DATA].value = data;
    dac53508_regs[DACD_DATA].value = data;
//    dac53508_regs[DACE_DATA].value = data;
//    dac53508_regs[DACF_DATA].value = data;

    done = dac53508_WriteRegister(&dac53508_regs[DACA_DATA]);
    done = dac53508_WriteRegister(&dac53508_regs[DACB_DATA]);
    done = dac53508_WriteRegister(&dac53508_regs[DACC_DATA]);
    done = dac53508_WriteRegister(&dac53508_regs[DACD_DATA]);
//    done = dac53508_WriteRegister(&dac53508_regs[DACE_DATA]);
//    done = dac53508_WriteRegister(&dac53508_regs[DACF_DATA]);

    return done;
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
