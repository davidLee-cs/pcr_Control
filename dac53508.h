/*
 * dac53508.h
 *
 *  Created on: 2025. 1. 14.
 *      Author: alllite
 */

#ifndef DAC53508_H_
#define DAC53508_H_


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

uint16_t  dac53508_write(uint16_t data, int16_t ch);
void dac53508_init(void);


#endif /* DAC53508_H_ */
