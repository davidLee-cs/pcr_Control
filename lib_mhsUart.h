#ifndef LIB_MHSUART_H_
#define LIB_MHSUART_H_

#include "driverlib.h"
#include "device.h"
#include "board.h"

void sci_set(void);
__interrupt void Boot_Rx_ISR(void);
__interrupt void Boot_Tx_ISR(void);
__interrupt void Barcode_Rx_ISR(void);
__interrupt void Barcode_Tx_ISR(void);
__interrupt void Rs485_Rx_ISR(void);
__interrupt void Rs485_Tx_ISR(void);
__interrupt void uartTTL_Rx_ISR(void);
__interrupt void uartTTL_Tx_ISR(void);


extern char rBootData_Rx[100];
extern bool gBoot_Rx_done;
extern int16_t gBoot_Rx_cnt;

extern char rBarcaodeData_Rx[100];
extern bool gBarcode_Rx_done;
extern int16_t gBarcode_Rx_cnt;

extern char rRs485Data_Rx[100];
extern bool gRs485_Rx_done;
extern int16_t gRs485_Rx_cnt;

extern char rUartData_Rx[100];
extern bool gUart_Rx_done;
extern int16_t gUart_Rx_cnt;

#endif /* LIB_MHSUART_H_ */

