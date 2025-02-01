/*
 * dac53508_reg.h
 *
 *  Created on: 2025. 1. 14.
 *      Author: alllite
 */

#ifndef DAC53508_REG_H_
#define DAC53508_REG_H_


#include <assert.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct _dac_reg
{
    uint16_t        addr;
    uint16_t        value;
    unsigned char   size;
}dac_reg;


/*! dac53508 registers list*/
enum  dac53508_registers
{
    RESERVED = 0x00,
    DEVICE_CONFIG,
    STATUS_TRIGGER,
    BRDCAST,

    RESERVED0,
    RESERVED1,
    RESERVED2,
    RESERVED3,

    DACA_DATA,
    DACB_DATA,
    DACC_DATA,
    DACD_DATA,
    DACE_DATA,
    DACF_DATA,
    DACG_DATA,
    DACH_DATA,

    dac53508_REG_NO
};


dac_reg  dac53508_regs[] =  // dac53508 INIT 20161102
{

     // 레지스테 파리미터 설정은 24 bit로 설정해야 됨.
     {0x00, 0x0000, 3}, // RESERVED = 0x00,
     {0x01, 0x0000, 3}, //DEVICE_CONFIG, fe
     {0x02, 0x0000, 3}, // STATUS_TRIGGER,
     {0x03, 0x0000, 3}, //BRDCAST,

     {0x04, 0x0000, 3}, //RESERVED,
     {0x05, 0x0000, 3}, //RESERVED,
     {0x06, 0x0000, 3}, //RESERVED,
     {0x07, 0x0000, 3}, // RESERVED,

     {0x08, 0x0400, 3}, //DACA_DATA,
     {0x09, 0x0400, 3}, //DACB_DATA,
     {0x0A, 0x0400, 3}, //DACC_DATA,
     {0x0B, 0x0400, 3}, // DACD_DATA,
     {0x0C, 0x0400, 3}, //DACE_DATA,
     {0x0D, 0x0400, 3}, //DACF_DATA,
     {0x0E, 0x0400, 3},//DACG_DATA,
     {0x0F, 0x0400, 3}, //DACH_DATA,

     {0xFF, 0x0000, 32} // dac53508_REG_NO
 //-------------------------------------------------------------------------------

};



#endif /* DAC53508_REG_H_ */
