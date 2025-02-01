/*
 * drv8453_reg.h
 *
 *  Created on: 2025. 1. 14.
 *      Author: alllite
 */

#ifndef DRV8452_REG_H_
#define DRV8452_REG_H_

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"


typedef struct _drv_reg
{
    uint16_t        addr;
    uint16_t        value;
    unsigned char   size;
}drv_reg;


/*! DRV8452 registers list*/
enum  DRV8452_registers
{
    FAULT = 0x00,
    DIAG1,
    DIAG2,
    DIAG3,
    CTRL1,
    CTRL2,
    CTRL3,
    CTRL4,
    CTRL5,
    CTRL6,
    CTRL7,
    CTRL8,
    CTRL9,
    CTRL10,
    CTRL11,
    CTRL12,
    CTRL13,
    INDEX1,
    INDEX2,
    INDEX3,
    INDEX4,
    INDEX5,
    CUSTOM_CTRL1,
    CUSTOM_CTRL2,
    CUSTOM_CTRL3,
    CUSTOM_CTRL4,
    CUSTOM_CTRL5,
    CUSTOM_CTRL6,
    CUSTOM_CTRL7,
    CUSTOM_CTRL8,
    CUSTOM_CTRL9,
    ATQ_CTRL1,
    ATQ_CTRL2,
    ATQ_CTRL3,
    ATQ_CTRL4,
    ATQ_CTRL5,
    ATQ_CTRL6,
    ATQ_CTRL7,
    ATQ_CTRL8,
    ATQ_CTRL9,
    ATQ_CTRL10,
    ATQ_CTRL11,
    ATQ_CTRL12,
    ATQ_CTRL13,
    ATQ_CTRL14,
    ATQ_CTRL15,
    ATQ_CTRL16,
    ATQ_CTRL17,
    ATQ_CTRL18,
    SS_CTRL1,
    SS_CTRL2,
    SS_CTRL3,
    SS_CTRL4,
    SS_CTRL5,

    DRV8452_REG_NO
};


drv_reg  DRV8452_regs[] =  // DRV8452 INIT 20161102
{

 // 레지스테 파리미터 설정은 24 bit로 설정해야 됨.
    {0x00, 0x0000, 2}, //FAULT = 0x00,
    {0x01, 0x0000, 2}, //DIAG1,
    {0x02, 0x0000, 2}, //DIAG2,
    {0x03, 0x0000, 2}, //DIAG3,
    {0x04, 0xce00, 2}, //CTRL1,
    {0x05, 0x0700, 2}, //CTRL2,
    {0x06, 0xb000, 2}, //CTRL3,
    {0x07, 0x0000, 2}, //CTRL4,
    {0x08, 0x0000, 2}, //CTRL5,
    {0x09, 0x0000, 2}, //CTRL6,
    {0x0A, 0x0000, 2}, //CTRL7,
    {0x0B, 0x0000, 2}, //CTRL8,
    {0x0C, 0x0100, 2}, //CTRL9,
    {0x0D, 0x0000, 2}, //CTRL10,
    {0x0E, 0x0000, 2}, //CTRL11,
    {0x0F, 0x0000, 2}, //CTRL12,
    {0x10, 0x0000, 2}, //CTRL13,
    {0x11, 0x0000, 2}, //INDEX1,
    {0x12, 0x0000, 2}, //INDEX2,
    {0x13, 0x0000, 2}, //INDEX3,
    {0x14, 0x0000, 2}, //INDEX4,
    {0x15, 0x0000, 2}, //INDEX5,
    {0x16, 0x0000, 2}, //CUSTOM_CTRL1,
    {0x17, 0x0000, 2}, //CUSTOM_CTRL2,
    {0x18, 0x0000, 2}, //CUSTOM_CTRL3,
    {0x19, 0x0000, 2}, //CUSTOM_CTRL4,
    {0x1A, 0x0000, 2}, //CUSTOM_CTRL5,
    {0x1B, 0x0000, 2}, //CUSTOM_CTRL6,
    {0x1C, 0x0000, 2}, //CUSTOM_CTRL7,
    {0x1D, 0x0000, 2}, //CUSTOM_CTRL8,
    {0x1E, 0x0000, 2}, //CUSTOM_CTRL9,
    {0x1F, 0x0000, 2}, // ATQ_CTRL1,
    {0x20, 0x0000, 2}, // ATQ_CTRL2,
    {0x21, 0x0000, 2}, //ATQ_CTRL3,
    {0x22, 0x0000, 2}, //ATQ_CTRL4,
    {0x23, 0x0000, 2}, //ATQ_CTRL5,
    {0x24, 0x0000, 2}, //ATQ_CTRL6,
    {0x25, 0x0000, 2}, //ATQ_CTRL7,
    {0x26, 0x0000, 2}, //ATQ_CTRL8,
    {0x27, 0x0000, 2}, //ATQ_CTRL9,
    {0x28, 0x0000, 2}, //ATQ_CTRL10,
    {0x29, 0x0000, 2}, //ATQ_CTRL11,
    {0x2A, 0x0000, 2}, //ATQ_CTRL12,
    {0x2B, 0x0000, 2}, //ATQ_CTRL13,
    {0x2C, 0x0000, 2}, //ATQ_CTRL14,
    {0x2D, 0x0000, 2}, //ATQ_CTRL15,
    {0x2E, 0x0000, 2}, //ATQ_CTRL16,
    {0x2F, 0x0000, 2}, // ATQ_CTRL17,
    {0x30, 0x0000, 2}, //ATQ_CTRL18,
    {0x31, 0x0000, 2}, //SS_CTRL1,
    {0x32, 0x0000, 2}, //SS_CTRL2,
    {0x33, 0x0000, 2}, //SS_CTRL3,
    {0x34, 0x0000, 2}, //SS_CTRL4,
    {0x35, 0x0000, 2}, //SS_CTRL5,

    {0xFF, 0x0000, 2}, // DRV8452_REG_NO
 //-------------------------------------------------------------------------------

};

// Bit Mask definitions for Registers
// Register 0x00: FAULT Register
#define OL_MASK               (0x01)         // Indicates open load condition
#define OT_MASK               (0x02)         // Logic OR of OTW and OTSD
#define STL_MASK              (0x04)         // Indicates motor stall
#define OCP_MASK              (0x08)         // Indicates over current fault condition
#define CPUV_MASK             (0x10)         // Indicates charge pump under voltage fault condition
#define UVLO_MASK             (0x20)         // Indicates an supply under voltage lockout fault condition
#define SPI_ERROR_MASK        (0x40)         // Indication SPI communication error
#define FAULT_MASK            (0x80)         // Inverse value of the nFAULT pin

// Register 0x01 : DIAG1 Register
#define OCP_HS1_A_MASK        (0x01)        // Indicates over current fault on the high-side FET of AOUT1
#define OCP_LS1_A_MASK        (0x02)        // Indicates over current fault on the low-side FET of AOUT1
#define OCP_HS2_A_MASK        (0x04)        // Indicates over current fault on the high-side FET of AOUT2
#define OCP_LS2_A_MASK        (0x08)        // Indicates over current fault on the low-side FET of AOUT2
#define OCP_HS1_B_MASK        (0x10)        // Indicates over current fault on the high-side FET of BOUT1
#define OCP_LS1_B_MASK        (0x20)        // Indicates over current fault on the low-side FET of BOUT1
#define OCP_HS2_B_MASK        (0x40)        // Indicates over current fault on the high-side FET of BOUT2
#define OCP_LS2_B_MASK        (0x80)        // Indicates over current fault on the low-side FET of BOUT2

// Register 0x02 : DIAG2 Register
#define OL_A_MASK             (0x01)        // Indicates open-load detection on AOUT
#define OL_B_MASK             (0x02)        // Indicates open-load detection on BOUT
#define ATQ_LRN_DONE_MASK     (0x04)        // Indicates auto torque learning was successful
#define STALL_MASK            (0x08)        // Indicates motor stall condition
#define STL_LRN_OK_MASK       (0x10)        // Indicates stall detection learning was successful
#define OTS_MASK              (0x20)        // Indicates over temperature shutdown
#define OTW_MASK              (0x40)        // Indicates over temperature warning
#define STSL_MASK             (0x80)        // Indicates motor standstill

// Register 0x03 : DIAG3 Register
#define RSVD0_MASK            (0x01)        // Reserved bit
#define RSVD1_MASK            (0x02)        // Reserved bit
#define NPOR_MASK             (0x04)        // Indicates a VCC UVLO event
#define SILENTSTEP_ERROR_MASK (0x08)        // Indicates Silent Step operation error
#define ATQ_CNT_UFLW_MASK     (0x10)        // Indicates ATQ_CNT is less than ATQ_LL
#define ATQ_CNT_OFLW_MASK     (0x20)        // Indicates ATQ_CNT is more than ATQ_UL
#define NHOME_MASK            (0x40)        // Indicates indexer is at home position of step table
#define SILENTSTEP_ON_MASK    (0x80)        // Indicates that device is working with silentstep decay mode

// Register 0x04 : CTRL1 Register
#define DECAY_MASK            (0x07)        // Bridge decay setting
#define TOFF_MASK             (0x18)        // Current regulation TOFF setting
#define RSVD2_MASK            (0x20)        // Reserved bit
#define SR_MASK               (0x40)        // Output driver rise and fall time selection
#define EN_OUT_MASK           (0x80)        // Hi-Z outputs bit OR-ed with DRVOFF

// Register 0x05 : CTRL2 Register
#define MICROSTEP_MODE_MASK   (0x0F)        // Microstep setting
#define SPI_STEP_MASK         (0x10)        // Enable SPI step control mode
#define SPI_DIR_MASK          (0x20)        // Enable SPI direction control mode
#define STEP_MASK             (0x40)        // Step control bit if SPI_STEP is enabled
#define DIR_MASK              (0x80)        // Direction control bit if SPI_DIR is enabled

// Register 0x06 : CTRL3 Register
#define TW_REP_MASK           (0x01)        // Report OTW on nFAULT
#define OTSD_MODE_MASK        (0x02)        // OTSD latch fault setting
#define OCP_MODE_MASK         (0x04)        // OCP latch fault setting
#define TOCP_MASK             (0x08)        // OCP deglitch time setting
#define LOCK_MASK             (0x70)        // Lock SPI registers
#define CLR_FLT_MASK          (0x80)        // Clear all fault bits

// Register 0x07 : CTRL4 Register
#define STEP_FREQ_TOL_MASK    (0x03)        // Programs the filter setting on the STEP frequency input.
#define RSVD3_MASK            (0x04)        // Reserved bit
#define STL_REP_MASK          (0x08)        // Report stall detection on nFAULT
#define EN_STL_MASK           (0x10)        // Enable stall detection
#define DIS_STL_MASK          (0xEF)        // Disable stall detection - clears EN_STL bit
#define STL_LRN_MASK          (0x20)        // Learn stall count threshold
#define TBLANK_TIME_MASK      (0x80)        // Controls the current sense blanking time

// Register 0x08 : CTRL5 Register
#define STALL_TH_MASK         (0xFF)        // Stall Threshold Lower 8-bits

// Register 0x09 : CTRL6 Register
#define STALL_TH_MSB_MASK     (0x0F)        // Stall Threshold Upper 4-bits
#define TRQ_SCALE_MASK        (0x80)        // Torque scaling 0: x1 and 1: x8
#define DIS_SSC_MASK          (0x40)        // Spread-spectrum disable for CP and OSC
#define RC_RIPPLE             (0xC0)        // Controls the current ripple in smart tune ripple control decay mode

// Register 0x0A : CTRL7 Register
#define TRQ_COUNT_MASK        (0xFF)        // Torque Count Lower 8-bits

// Register 0x0B : CTRL8 Register
#define TRQ_COUNT_MASK_MSB    (0x0F)        // Torque Count Upper 4-bits
#define REV_ID_MASK           (0xF0)        // Silicon Revision ID


#endif /* DRV8452_REG_H_ */
