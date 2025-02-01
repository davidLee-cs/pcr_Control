

#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "lib_mhsUart.h"


char rBootData_Rx[100] = {0};
bool gBoot_Rx_done = false;
int16_t gBoot_Rx_cnt	= 0;

char rBarcaodeData_Rx[100] = {0};
bool gBarcode_Rx_done = false;
int16_t gBarcode_Rx_cnt = 0;

char rRs485Data_Rx[100] = {0};
bool gRs485_Rx_done = false;
int16_t gRs485_Rx_cnt = 0;

char rUartData_Rx[100] = {0};
bool gUart_Rx_done = false;
int16_t gUart_Rx_cnt = 0;



void sci_set(void)
{

    Interrupt_register(INT_SCIA_RX, &Boot_Rx_ISR);
    Interrupt_register(INT_SCIB_RX, &Rs485_Rx_ISR);
    Interrupt_register(INT_SCIC_RX, &Barcode_Rx_ISR);
    Interrupt_register(INT_SCID_RX, &uartTTL_Rx_ISR);

}

//  uart 수신 인터럽트 서비스 루틴 함수
__interrupt void Boot_Rx_ISR(void)
{
    uint16_t rDataA[2];

	//1. 수신 버퍼에서 한 문자를 읽어와 rDataA[0] 위치에 저장
    SCI_readCharArray(BOOT_SCI_BASE, rDataA, 1);

	// 2. 수신된 문자가 0x0D인지 확인
    if((rDataA[0] & 0x00FFU) == 0x0DU)
    {
		// 2.1 수신 문자가 0x0D 이면 메세지 입력 완료, gRx_done = 1로 설정하여 메세지 완료 전달, 수신 버퍼 카운터는 -1로 초기화 
        gBoot_Rx_done = TRUE;
        gBoot_Rx_cnt = -1;

    }
    else
    {
    	// 2.2 수신된 문자가 캐리지 리턴(0x0D)이 아닐 경우, rDataPointA 버퍼의 gRx_cnt 위치에 해당 문자를 저장
        rBootData_Rx[gBoot_Rx_cnt] = rDataA[0];
    }

	// 3. SCI 모듈의 오버플로 상태를 초기화
    SCI_clearOverflowStatus(BOOT_SCI_BASE);

	// 4. SCI 수신 FIFO 버퍼(RXFF)에 대한 인터럽트 상태 플래그를 초기화
    SCI_clearInterruptStatus(BOOT_SCI_BASE, SCI_INT_RXFF);

	// 5. 인터럽트 그룹 9의 ACK 플래그를 초기화
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

	// 6. 카운터 gRx_cnt를 1 증가시켜 다음 수신된 문자가 있을 때 rDataPointA 배열의 다음 위치에 저장
    gBoot_Rx_cnt++;
}

//  uart 송신 인터럽트 서비스 루틴 함수
__interrupt void Boot_Tx_ISR(void)
{

	// 1.  SCI 송신 FIFO 버퍼(TXFF)의 인터럽트 상태 플래그를 초기화
    SCI_clearInterruptStatus(BOOT_SCI_BASE, SCI_INT_TXFF);
	
	// 2. 인터럽트 그룹 9의 ACK 플래그를 초기화
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}


//  uart 수신 인터럽트 서비스 루틴 함수
__interrupt void Barcode_Rx_ISR(void)
{
    uint16_t rDataC[2];

    //1. 수신 버퍼에서 한 문자를 읽어와 rDataA[0] 위치에 저장
    SCI_readCharArray(BARCODE_SCI_BASE, rDataC, 1);

    // 2. 수신된 문자가 0x0D인지 확인
    if((rDataC[0] & 0x00FFU) == 0x0DU)
    {
        // 2.1 수신 문자가 0x0D 이면 메세지 입력 완료, gRx_done = 1로 설정하여 메세지 완료 전달, 수신 버퍼 카운터는 -1로 초기화
        gBarcode_Rx_done = TRUE;
        gBarcode_Rx_cnt = -1;

    }
    else
    {
        // 2.2 수신된 문자가 캐리지 리턴(0x0D)이 아닐 경우, rDataPointA 버퍼의 gRx_cnt 위치에 해당 문자를 저장
        rBarcaodeData_Rx[gBarcode_Rx_cnt] = rDataC[0];
    }

    // 3. SCI 모듈의 오버플로 상태를 초기화
    SCI_clearOverflowStatus(BARCODE_SCI_BASE);

    // 4. SCI 수신 FIFO 버퍼(RXFF)에 대한 인터럽트 상태 플래그를 초기화
    SCI_clearInterruptStatus(BARCODE_SCI_BASE, SCI_INT_RXFF);

    // 5. 인터럽트 그룹 9의 ACK 플래그를 초기화
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    // 6. 카운터 gRx_cnt를 1 증가시켜 다음 수신된 문자가 있을 때 rDataPointA 배열의 다음 위치에 저장
    gBarcode_Rx_cnt++;
}

//  uart 송신 인터럽트 서비스 루틴 함수
__interrupt void Barcode_Tx_ISR(void)
{

    // 1.  SCI 송신 FIFO 버퍼(TXFF)의 인터럽트 상태 플래그를 초기화
    SCI_clearInterruptStatus(BARCODE_SCI_BASE, SCI_INT_TXFF);

    // 2. 인터럽트 그룹 9의 ACK 플래그를 초기화
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//  uart 수신 인터럽트 서비스 루틴 함수
__interrupt void Rs485_Rx_ISR(void)
{
    uint16_t rDataB[2];

    //1. 수신 버퍼에서 한 문자를 읽어와 rDataA[0] 위치에 저장
    SCI_readCharArray(RS485_SCI_BASE, rDataB, 1);

    // 2. 수신된 문자가 0x0D인지 확인
    if((rDataB[0] & 0x00FFU) == 0x0DU)
    {
        // 2.1 수신 문자가 0x0D 이면 메세지 입력 완료, gRx_done = 1로 설정하여 메세지 완료 전달, 수신 버퍼 카운터는 -1로 초기화
        gRs485_Rx_done = TRUE;
        gRs485_Rx_cnt = -1;

    }
    else
    {
        // 2.2 수신된 문자가 캐리지 리턴(0x0D)이 아닐 경우, rDataPointA 버퍼의 gRx_cnt 위치에 해당 문자를 저장
        rRs485Data_Rx[gRs485_Rx_cnt] = rDataB[0];
    }

    // 3. SCI 모듈의 오버플로 상태를 초기화
    SCI_clearOverflowStatus(RS485_SCI_BASE);

    // 4. SCI 수신 FIFO 버퍼(RXFF)에 대한 인터럽트 상태 플래그를 초기화
    SCI_clearInterruptStatus(RS485_SCI_BASE, SCI_INT_RXFF);

    // 5. 인터럽트 그룹 9의 ACK 플래그를 초기화
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    // 6. 카운터 gRx_cnt를 1 증가시켜 다음 수신된 문자가 있을 때 rDataPointA 배열의 다음 위치에 저장
    gRs485_Rx_cnt++;
}

//  uart 송신 인터럽트 서비스 루틴 함수
__interrupt void Rs485_Tx_ISR(void)
{

    // 1.  SCI 송신 FIFO 버퍼(TXFF)의 인터럽트 상태 플래그를 초기화
    SCI_clearInterruptStatus(RS485_SCI_BASE, SCI_INT_TXFF);

    // 2. 인터럽트 그룹 9의 ACK 플래그를 초기화
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//  uart 수신 인터럽트 서비스 루틴 함수
__interrupt void uartTTL_Rx_ISR(void)
{
    uint16_t rDataD[2];

    //1. 수신 버퍼에서 한 문자를 읽어와 rDataA[0] 위치에 저장
    SCI_readCharArray(UART_TTL_BASE, rDataD, 1);

    // 2. 수신된 문자가 0x0D인지 확인
    if((rDataD[0] & 0x00FFU) == 0x0DU)
    {
        // 2.1 수신 문자가 0x0D 이면 메세지 입력 완료, gRx_done = 1로 설정하여 메세지 완료 전달, 수신 버퍼 카운터는 -1로 초기화
        gUart_Rx_done = TRUE;
        gUart_Rx_cnt = -1;

    }
    else
    {
        // 2.2 수신된 문자가 캐리지 리턴(0x0D)이 아닐 경우, rDataPointA 버퍼의 gRx_cnt 위치에 해당 문자를 저장
        rUartData_Rx[gUart_Rx_cnt] = rDataD[0];
    }

    // 3. SCI 모듈의 오버플로 상태를 초기화
    SCI_clearOverflowStatus(UART_TTL_BASE);

    // 4. SCI 수신 FIFO 버퍼(RXFF)에 대한 인터럽트 상태 플래그를 초기화
    SCI_clearInterruptStatus(UART_TTL_BASE, SCI_INT_RXFF);

    // 5. 인터럽트 그룹 9의 ACK 플래그를 초기화
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    // 6. 카운터 gRx_cnt를 1 증가시켜 다음 수신된 문자가 있을 때 rDataPointA 배열의 다음 위치에 저장
    gUart_Rx_cnt++;
}

//  uart 송신 인터럽트 서비스 루틴 함수
__interrupt void uartTTL_Tx_ISR(void)
{

    // 1.  SCI 송신 FIFO 버퍼(TXFF)의 인터럽트 상태 플래그를 초기화
    SCI_clearInterruptStatus(UART_TTL_BASE, SCI_INT_TXFF);

    // 2. 인터럽트 그룹 9의 ACK 플래그를 초기화
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}


