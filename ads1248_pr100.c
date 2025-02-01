/*
 * ads1248.c
 *
 *  Created on: 2025. 1. 11.
 *      Author: alllite
 */

#include "config.h"
#include "math.h"
#include "ads1248_reg.h"
#include "ads1248_LUT.h"

#define ONE_REGISTER_READ_WRITE     0
#define TWO_REGISTER_READ_WRITE     1
#define THREE_REGISTERS_READ_WRITE  2  // 3-1

#define REF_RESISTANCE          2000.0//2490.0  //3240.0

uint16_t Spi_write_data[8]= {0,};

void Spi_Read_write_8bit(uint16_t * Pbuff,uint16_t Length,uint16_t uiBase);

typedef enum RTDExampleDef{
    RTD_2_Wire_Fig15,     // 2-Wire RTD example using ADS124S08 EVM, User's Guide Figure 15
    RTD_3_Wire_Fig14,     // 3-Wire RTD example using ADS124S08 EVM, User's Guide Figure 14
    RTD_4_Wire_Fig16,     // 4-Wire RTD example using ADS124S08 EVM, User's Guide Figure 16
} RTD_Example;



float nowTemp =0.0;
float temp2=0.0;
uint32_t Rtd_adc=0;
/*
1.4 ADC Initialization for RTD sampling (Ratiometric)
占쎄땅 When ADC is initialized for RTD sampling, the excitation currents should be turned ON and routed
through IEXC1 and 2.
占쎄땅 The excitation current of 500 uA is chosen.
占쎄땅 Current swapping by routing the same current through the different excitation source and averaging them
will cancel out errors.
占쎄땅 Internal reference is always ON.
占쎄땅 The external reference of REF1 pair is selected.
占쎄땅 Gain of 16 and sampling rate of 20 samples per second settings are chosen.
占쎄땅 During initialization self-offset calibration process is initiated, followed by a settling time delay.
*/
void ADS1248_Init(void)
{
    //SPI pins are already configured
    //Reset low is done already
    //chip select
    GPIO_writePin(ADC_CS_1, 0);
    DEVICE_DELAY_US(5);
    //configure the ADC registers
    Spi_write_data[0] = (WREG | IDAC0) << 8;
    Spi_write_data[1] = ONE_REGISTER_READ_WRITE; //ONE_REGISTER_READ_WRITE;
    Spi_write_data[2] = 0x04U << 8;//excitation current 500uA (RTD)
    Spi_Read_write_8bit(Spi_write_data, 3, SPIC_BASE);
    DEVICE_DELAY_US(5);
    GPIO_writePin(ADC_CS_1, 1);

    DEVICE_DELAY_US(100000);

#if 1
    GPIO_writePin(ADC_CS_1, 0);
    DEVICE_DELAY_US(10);
    Spi_write_data[0] = (WREG | IDAC1) << 8;
    Spi_write_data[1] = ONE_REGISTER_READ_WRITE;
    Spi_write_data[2] = 0x89U << 8;//0xFF - to disconnect //select IDAC output pins
    Spi_Read_write_8bit(Spi_write_data, 3, SPIC_BASE);
    DEVICE_DELAY_US(10);
    GPIO_writePin(ADC_CS_1, 1);

    DEVICE_DELAY_US(100000);


    GPIO_writePin(ADC_CS_1, 0);
    DEVICE_DELAY_US(10);
    Spi_write_data[0] = (WREG | MUX1) << 8;
    Spi_write_data[1] = ONE_REGISTER_READ_WRITE;
    Spi_write_data[2] = 0x28 << 8;//internal osc, internal reference ON, REF1 input pair
    Spi_Read_write_8bit(Spi_write_data, 3, SPIC_BASE);
    DEVICE_DELAY_US(10);
    GPIO_writePin(ADC_CS_1, 1);

    DEVICE_DELAY_US(100000);

    GPIO_writePin(ADC_CS_1, 0);
    DEVICE_DELAY_US(10);
    Spi_write_data[0] = (WREG | SYS0) << 8;
    Spi_write_data[1] = ONE_REGISTER_READ_WRITE;
    Spi_write_data[2] = 0x42 << 8;//Select gain 16 and sampling rate 20sps
    Spi_Read_write_8bit(Spi_write_data, 3, SPIC_BASE);
    DEVICE_DELAY_US(10);
    GPIO_writePin(ADC_CS_1, 1);

    DEVICE_DELAY_US(10000);

    GPIO_writePin(ADC_CS_1, 0);
    DEVICE_DELAY_US(10);
    Spi_write_data[0] = (WREG | OFC0) << 8;
    Spi_write_data[1] = THREE_REGISTERS_READ_WRITE << 8;
    Spi_write_data[2] = 0x00;//OFFSET calibration
    Spi_write_data[3] = 0x00;
    Spi_write_data[4] = 0x00;
    Spi_Read_write_8bit(Spi_write_data, 5, SPIC_BASE);
    DEVICE_DELAY_US(10);
    GPIO_writePin(ADC_CS_1, 1);

    DEVICE_DELAY_US(10);

    GPIO_writePin(ADC_CS_1, 0);
    Spi_write_data[0] = 0x62 << 8;
    Spi_write_data[1] = ONE_REGISTER_READ_WRITE;
    Spi_Read_write_8bit(Spi_write_data, 2, SPIC_BASE);
    DEVICE_DELAY_US(10);
    GPIO_writePin(ADC_CS_1, 1);

#endif
    DEVICE_DELAY_US(802000);

}


/*2.6 RTD channel switching
The following code snippet, switches external multiplexer through I2C bus and the ADC internal multiplexer
settings through SPI bus.
For ADC, to process AIN0 as positive and AIN1 as negative, MUX0 register is chosen as 0x01.*/
void select_Channel(uint16_t ChnNum)
{
    GPIO_writePin(ADC_CS_1, 0);
    DEVICE_DELAY_US(10);
    switch (ChnNum)
    {
        case 0:
            GPIO_writePin(RTD_Sel0, 0);
            GPIO_writePin(RTD_Sel1, 0);         // 0
            //first take default readings for AIN0+ and AIN1- channel1
            Spi_write_data[0] = (WREG | MUX0) << 8;
            Spi_write_data[1] = ONE_REGISTER_READ_WRITE << 8;
            Spi_write_data[2] = 0x01U << 8;//AIN0 positive and AIN1 negative
            Spi_Read_write_8bit(Spi_write_data, 3, SPIC_BASE);
            break;
        case 1:
            GPIO_writePin(RTD_Sel0, 1);
            GPIO_writePin(RTD_Sel1, 1);         // 3
            //second channel
            Spi_write_data[0] = (WREG | MUX0) << 8;
            Spi_write_data[1] = ONE_REGISTER_READ_WRITE << 8;
            Spi_write_data[2] = 0x13U << 8;//AIN2 positive and AIN3 negative
            Spi_Read_write_8bit(Spi_write_data, 3, SPIC_BASE);
            break;
        case 2:
            GPIO_writePin(RTD_Sel0, 1);
            GPIO_writePin(RTD_Sel1, 0);         // 1
            //third channel
            Spi_write_data[0] = (WREG | MUX0) << 8;
            Spi_write_data[1] = ONE_REGISTER_READ_WRITE << 8;
            Spi_write_data[2] = 0x25U << 8;//AIN4 positive and AIN5 negative
            Spi_Read_write_8bit(Spi_write_data, 3, SPIC_BASE);
            break;
        case 3:
            GPIO_writePin(RTD_Sel0, 0);
            GPIO_writePin(RTD_Sel1, 1);         // 2
            //fourth channel
            Spi_write_data[0] = (WREG | MUX0) << 8;
            Spi_write_data[1] = ONE_REGISTER_READ_WRITE << 8;
            Spi_write_data[2] = 0x37U << 8;//AIN6 positive and AIN7 negative
            Spi_Read_write_8bit(Spi_write_data, 3, SPIC_BASE);
        break;
    }

    DEVICE_DELAY_US(10);
    GPIO_writePin(ADC_CS_1, 1);
}


/*2.7 Read ADC Data
Wait for /DRDY to be asserted. Then issue a read data command on SPI bus. This is followed by sending 0xFF
thrice to send 24 clock cycles to ADC.*/
uint32_t spi_read_data(void)
{
    uint32_t Rtd_data=0;
    uint16_t temp_array[8];
    uint16_t temp;
    uint16_t data;
    uint32_t maskData;

    temp_array[0] = RDATA << 8;//0x20;
    temp_array[1] = 0;//0xFFU << 8;
    temp_array[2] = 0;//0xFFU << 8;
    temp_array[3] = 0;//0xFFU << 8;

    GPIO_writePin(ADC_CS_1, 0);
    DEVICE_DELAY_US(10);

    Spi_Read_write_8bit(temp_array, 4, SPIC_BASE);

    // Get 24 bit data from ADC
    for(temp=1;temp<4;temp++)
    {
        data = temp_array[temp];
        Rtd_data = (Rtd_data<<8) | data;
    }

    DEVICE_DELAY_US(10);
    GPIO_writePin(ADC_CS_1, 1);

    maskData = Rtd_data & 0x00FFFFFF;

    return maskData;
}

uint32_t spi_read_id(void)
{
    uint32_t Rtd_data;
    uint16_t temp_array[4];
    uint16_t temp;
    uint16_t data;

    temp_array[0] = (RREG | IDAC0) << 8;
    temp_array[1] = TWO_REGISTER_READ_WRITE << 8; //TWO_REGISTER_READ_WRITE;
    temp_array[2] = 0;
    temp_array[3] = 0;

    GPIO_writePin(ADC_CS_1, 0);
    DEVICE_DELAY_US(5);

    Spi_Read_write_8bit(temp_array, 4, SPIC_BASE);

    // Get 24 bit data from ADC
    for(temp=0;temp<3;temp++)
    {
        data = temp_array[temp];
        Rtd_data = (Rtd_data<<8) | data;
    }

    DEVICE_DELAY_US(5);
    GPIO_writePin(ADC_CS_1, 1);

    return Rtd_data;
}


float read_pr100(uint16_t ChnNum)
{
//    float temp2=0.0;

    GPIO_writePin(ADC_Start_1, 1);
    DEVICE_DELAY_US(3);
    GPIO_writePin(ADC_Start_1, 0);

    select_Channel(ChnNum);

    DEVICE_DELAY_US(14000);

    temp2= read_temperature(ChnNum);
//                  temp2=RTD_Temp_Sample2;

    nowTemp = calc_Temp_rtd(temp2);

    return nowTemp;
}

float read_temperature(uint16_t slot)
{
    long Rtd_data=0;
    int gain;
    float resistance1 =0.0;//,resistance2 =0.0;

    gain = 16;
    Rtd_data = Read_RTD_data(slot);

#if 1
    double res2;
    //Ratio metric measurement of resistance
    double res0 = (Rtd_data*REF_RESISTANCE*2);
    res2 = (res0)/0x7FFFFF;
    resistance1 = res2/gain;

#else   // 16bit adc

    int rtd_data16 = 0;
    float res2;

    rtd_data16 = Rtd_data >> 8;

    //Ratio metric measurement of resistance
    float res0 = (rtd_data16*REF_RESISTANCE*2);

    res2 = (res0)/0x7FFF; //FF;
    resistance1 = res2/gain;


#endif
    return (resistance1);
}

uint32_t Read_RTD_data (uint16_t slot)
{
//  if(Error_status)
//      return 0;

    uint32_t Rtd_data=0;

    // Get the sampled data
    Rtd_data = Get_data_ADS1248(slot);

    return Rtd_data;
}

uint32_t Get_data_ADS1248(uint16_t slot)
{
    unsigned char temp=0xff;
//    uint32_t Rtd_data=0;

    // Loop to wait till DRDY goes low
    while(temp)
    {
        temp = GPIO_readPin(ADC_DRDY_1);
        DEVICE_DELAY_US(1);
    }

    Rtd_adc = spi_read_data();

    return Rtd_adc;
}

float calc_Temp_rtd(float resistance)
{
    float m=0.0,temp=0.0;

    // Check if there is an error
//    if(Error_status)
//        return 0;
    if((18.53>resistance)| (resistance>390.47))
    {
        //Check the resistance value is within the range, If not return  with error
//        Error_status = TRUE;
        return 0;
    }

    int index=0;
    // Find the index for the corresponding resistance in LUT
    index = search_rtd_array(resistance);
    // Line fit equation to get better accuracy
    m=(RTD_LUT[index+1]-RTD_LUT[index]);
    temp = ((1/m)*(resistance-RTD_LUT[index]))+index;
    // actual temperature is the index found - 200
    return temp-200;
}

uint16_t search_rtd_array(float search_element)
{
//    if(Error_status)
//        return 0;
    int J_max=10;

    int i,j,k;
    for (i=0;i<11;i++)
    {
        if(search_element <= RTD_LUT[i*100])
            break;
    }
    if(i==11)
        J_max=5;
    for(j=0;j<J_max;j++)
    {
        if(search_element <= RTD_LUT[((i-1)*100)+(j*10)])
            break;
    }
    for(k=0;k<=10;k++)
    {
        if(search_element <= RTD_LUT[((i-1)*100)+(10*(j-1))+k])
            break;
    }


    return ((i-1)*100)+((j-1)*10)+k-1;
}


void Spi_Read_write_8bit(uint8_t * Pbuff,uint16_t Length,uint16_t uiBase)
{
    uint32_t temp=0;
    uint16_t temp_count;

    for(temp_count=0;temp_count<Length;temp_count++)
    {
        SPI_writeDataNonBlocking(SPIC_BASE, *(Pbuff));             // 0xFF00 占쎌몵嚥∽옙 8 bit 占쎈쑓占쎌뵠占쎄숲 占쎈쾺疫뀐옙 占쎈짗占쎌삂 !!!
        temp = SPI_readDataBlockingNonFIFO(SPIC_BASE);
        *(Pbuff++) = temp;
    }
}
