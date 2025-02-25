/*
 * drv8452.h
 *
 *  Created on: 2025. 1. 14.
 *      Author: alllite
 */

#ifndef DRV8452_H_
#define DRV8452_H_


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define OPCODE_RREG                             ((uint16_t) 0x4000)
#define OPCODE_WREG                             ((uint16_t) 0x0000)


uint16_t dac53508_read(void);
void SetMotorDirection(int direction) ;
void EnableMotor(int enable);
uint16_t  drv8452_read(void);
uint16_t  drv8452_write(void);
void drv8452_init(void);
void drv8452_outDisable(void);
void drv8452_outEnable(void);

#endif /* DRV8452_H_ */
