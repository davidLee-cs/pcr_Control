/*
 * drv8452.c
 *
 *  Created on: 2025. 1. 14.
 *      Author: alllite
 */

#include "config.h"
#include "drv8452_reg.h"

int32_t drv8452_ReadRegister(drv_reg* pReg, int16_t channel);
//int32_t drv8452_WriteRegister(drv_reg* reg);
int32_t drv8452_WriteRegister(drv_reg* reg, int16_t channel);

void drv8452_init(void)
{
    uint32_t data;
    int16_t ch;

    for(ch=0; ch<4; ch++)
    {

        if(ch == 0)            GPIO_writePin(SPI_SS_0, 1);
        else if(ch == 1)       GPIO_writePin(SPI_SS_1, 1);
        else if(ch == 2)       GPIO_writePin(SPI_SS_2, 1);
        else if(ch == 3)       GPIO_writePin(SPI_SS_3, 1);
        DEVICE_DELAY_US(10);

        data = drv8452_WriteRegister(&DRV8452_regs[CTRL3], ch);
        data = drv8452_WriteRegister(&DRV8452_regs[CTRL2], ch);
        data = drv8452_WriteRegister(&DRV8452_regs[CTRL1], ch);
        data = drv8452_WriteRegister(&DRV8452_regs[CTRL9], ch);

    }
//    GPIO_writePin(ENABLE_0, 1);

}

void drv8452_outDisable(int16_t channel)
{
    uint32_t data;

    if(channel == 0)        GPIO_writePin(SPI_SS_0, 1);
    else if(channel == 1)   GPIO_writePin(SPI_SS_1, 1);
    else if(channel == 2)   GPIO_writePin(SPI_SS_2, 1);
    else if(channel == 3)   GPIO_writePin(SPI_SS_3, 1);

    DEVICE_DELAY_US(10);

    DRV8452_regs[CTRL1].value = 0x4e00;
    data = drv8452_WriteRegister(&DRV8452_regs[CTRL1], channel);

}

void drv8452_outEnable(int16_t channel)
{
    uint32_t data;

    if(channel == 0)        GPIO_writePin(SPI_SS_0, 1);
    else if(channel == 1)   GPIO_writePin(SPI_SS_1, 1);
    else if(channel == 2)   GPIO_writePin(SPI_SS_2, 1);
    else if(channel == 3)   GPIO_writePin(SPI_SS_3, 1);

    DEVICE_DELAY_US(10);

    DRV8452_regs[CTRL1].value = 0xce00;
    data = drv8452_WriteRegister(&DRV8452_regs[CTRL1], channel);

}

uint16_t  drv8452_read(int16_t channel)
{
    uint16_t data;

    data = drv8452_ReadRegister(&DRV8452_regs[FAULT], channel);

    return data;
}

uint16_t  drv8452_write(int16_t channel)
{
    uint16_t data;

    data = drv8452_WriteRegister(&DRV8452_regs[CTRL1], channel);

    return data;
}

int32_t drv8452_ReadRegister(drv_reg* pReg, int16_t channel)
{
    int32_t ret;
    uint16_t i;
    uint16_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t wrxBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//    uint16_t index         = 0;

    /* Build the Command word */
    buffer[0] = OPCODE_RREG | (pReg->addr << 8);
    buffer[1] = pReg->value << 8;

    if(channel == 0)        GPIO_writePin(SPI_SS_0, 0);
    else if(channel == 1)   GPIO_writePin(SPI_SS_1, 0);
    else if(channel == 2)   GPIO_writePin(SPI_SS_2, 0);
    else if(channel == 3)   GPIO_writePin(SPI_SS_3, 0);

    DEVICE_DELAY_US(1);

    for (i=0; i< pReg->size; i++)
    {
        SPI_writeDataNonBlocking(SPIA_BASE, (buffer[i]));             // 0xFF00 �쑝濡� 8 bit �뜲�씠�꽣 �벐湲� �룞�옉 !!!
        wrxBuf[i] = SPI_readDataBlockingNonFIFO(SPIA_BASE);
    }

    DEVICE_DELAY_US(1);
    if(channel == 0)        GPIO_writePin(SPI_SS_0, 1);
    else if(channel == 1)   GPIO_writePin(SPI_SS_1, 1);
    else if(channel == 2)   GPIO_writePin(SPI_SS_2, 1);
    else if(channel == 3)   GPIO_writePin(SPI_SS_3, 1);

    ret = (wrxBuf[0] << 8) | wrxBuf[1];
    return ret;
}

int32_t drv8452_WriteRegister(drv_reg* reg, int16_t channel)
{
    int32_t ret=0;
    int32_t regValue = 0;
    uint16_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t wrxBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t i;

    /* Build the Command word */
    wrBuf[0] = OPCODE_WREG | (reg->addr << 8);
    wrBuf[1] = reg->value;

    if(channel == 0)        GPIO_writePin(SPI_SS_0, 0);
    else if(channel == 1)   GPIO_writePin(SPI_SS_1, 0);
    else if(channel == 2)   GPIO_writePin(SPI_SS_2, 0);
    else if(channel == 3)   GPIO_writePin(SPI_SS_3, 0);

    for (i=0; i< reg->size ; i++)
    {
        // Transmit data
        SPI_writeDataNonBlocking(SPIA_BASE, (wrBuf[i]));             // 0xFF00 �쑝濡� 8 bit �뜲�씠�꽣 �벐湲� �룞�옉 !!!
        wrxBuf[i] = SPI_readDataBlockingNonFIFO(SPIA_BASE);
    }

    if(channel == 0)        GPIO_writePin(SPI_SS_0, 1);
    else if(channel == 1)   GPIO_writePin(SPI_SS_1, 1);
    else if(channel == 2)   GPIO_writePin(SPI_SS_2, 1);
    else if(channel == 3)   GPIO_writePin(SPI_SS_3, 1);

    return ret;
}

void SetMotorDirection(int16_t channel, int16_t direction) {

    if(channel == 0)
    {
        if (direction == 1)     GPIO_writePin(DIR_0, 1);
        else                    GPIO_writePin(DIR_0, 0);
    }
    else if(channel == 1)
    {
        if (direction == 1)     GPIO_writePin(DIR_1, 1);
        else                    GPIO_writePin(DIR_1, 0);
    }
    else if(channel == 2)
    {
        if (direction == 1)     GPIO_writePin(DIR_2, 1);
        else                    GPIO_writePin(DIR_2, 0);
    }
    else if(channel == 3)
    {
        if (direction == 1)     GPIO_writePin(DIR_3, 1);
        else                    GPIO_writePin(DIR_3, 0);

    }

}

void EnableMotor(int enable) {

    if (enable == 1) {
        GPIO_writePin(ENABLE_0, 1);
    } else {
        GPIO_writePin(ENABLE_0, 0);
    }
}

