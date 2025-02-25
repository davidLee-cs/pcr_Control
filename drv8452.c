/*
 * drv8452.c
 *
 *  Created on: 2025. 1. 14.
 *      Author: alllite
 */

#include "config.h"
#include "drv8452_reg.h"

int32_t drv8452_ReadRegister(drv_reg* pReg);
int32_t drv8452_WriteRegister(drv_reg* reg);


void drv8452_init(void)
{
    uint32_t data;

    GPIO_writePin(SPI_SS_0, 1);
    DEVICE_DELAY_US(10);

    data = drv8452_WriteRegister(&DRV8452_regs[CTRL3]);
    data = drv8452_WriteRegister(&DRV8452_regs[CTRL2]);
    data = drv8452_WriteRegister(&DRV8452_regs[CTRL1]);
    data = drv8452_WriteRegister(&DRV8452_regs[CTRL9]);

//    GPIO_writePin(ENABLE_0, 1);

}

void drv8452_outDisable(void)
{
    uint32_t data;

    GPIO_writePin(SPI_SS_0, 1);
    DEVICE_DELAY_US(10);

    DRV8452_regs[CTRL1].value = 0x4e00;
    data = drv8452_WriteRegister(&DRV8452_regs[CTRL1]);

}

void drv8452_outEnable(void)
{
    uint32_t data;

    GPIO_writePin(SPI_SS_0, 1);
    DEVICE_DELAY_US(10);

    DRV8452_regs[CTRL1].value = 0xce00;
    data = drv8452_WriteRegister(&DRV8452_regs[CTRL1]);

}

uint16_t  drv8452_read(void)
{
    uint16_t data;

    data = drv8452_ReadRegister(&DRV8452_regs[FAULT]);

    return data;
}

uint16_t  drv8452_write(void)
{
    uint16_t data;

    data = drv8452_WriteRegister(&DRV8452_regs[CTRL1]);

    return data;
}

int32_t drv8452_ReadRegister(drv_reg* pReg)
{
    int32_t ret;
    uint16_t i;
    uint16_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t wrxBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//    uint16_t index         = 0;

    /* Build the Command word */
    buffer[0] = OPCODE_RREG | (pReg->addr << 8);
    buffer[1] = pReg->value << 8;

    GPIO_writePin(SPI_SS_0, 0);
    DEVICE_DELAY_US(1);

    for (i=0; i< pReg->size; i++)
    {
        SPI_writeDataNonBlocking(SPIA_BASE, (buffer[i]));             // 0xFF00 �쑝濡� 8 bit �뜲�씠�꽣 �벐湲� �룞�옉 !!!
        wrxBuf[i] = SPI_readDataBlockingNonFIFO(SPIA_BASE);
    }

    DEVICE_DELAY_US(1);
    GPIO_writePin(SPI_SS_0, 1);

    ret = (wrxBuf[0] << 8) | wrxBuf[1];
    return ret;
}


int32_t drv8452_WriteRegister(drv_reg* reg)
{
    int32_t ret=0;
    int32_t regValue = 0;
    uint16_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t wrxBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t i;

    /* Build the Command word */
    wrBuf[0] = OPCODE_WREG | (reg->addr << 8);
    wrBuf[1] = reg->value;

    GPIO_writePin(SPI_SS_0, 0);

    for (i=0; i< reg->size ; i++)
    {
        // Transmit data
        SPI_writeDataNonBlocking(SPIA_BASE, (wrBuf[i]));             // 0xFF00 �쑝濡� 8 bit �뜲�씠�꽣 �벐湲� �룞�옉 !!!
        wrxBuf[i] = SPI_readDataBlockingNonFIFO(SPIA_BASE);
    }

    GPIO_writePin(SPI_SS_0, 1);

    return ret;
}


void SetMotorDirection(int direction) {
    if (direction == 1) {
        GPIO_writePin(DIR_0, 1);
    } else {
        GPIO_writePin(DIR_0, 0);
    }
}

void EnableMotor(int enable) {
    if (enable == 1) {
        GPIO_writePin(ENABLE_0, 1);
    } else {
        GPIO_writePin(ENABLE_0, 0);
    }
}

