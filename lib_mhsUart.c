

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

//  uart ���� ���ͷ�Ʈ ���� ��ƾ �Լ�
__interrupt void Boot_Rx_ISR(void)
{
    uint16_t rDataA[2];

	//1. ���� ���ۿ��� �� ���ڸ� �о�� rDataA[0] ��ġ�� ����
    SCI_readCharArray(BOOT_SCI_BASE, rDataA, 1);

	// 2. ���ŵ� ���ڰ� 0x0D���� Ȯ��
    if((rDataA[0] & 0x00FFU) == 0x0DU)
    {
		// 2.1 ���� ���ڰ� 0x0D �̸� �޼��� �Է� �Ϸ�, gRx_done = 1�� �����Ͽ� �޼��� �Ϸ� ����, ���� ���� ī���ʹ� -1�� �ʱ�ȭ 
        gBoot_Rx_done = TRUE;
        gBoot_Rx_cnt = -1;

    }
    else
    {
    	// 2.2 ���ŵ� ���ڰ� ĳ���� ����(0x0D)�� �ƴ� ���, rDataPointA ������ gRx_cnt ��ġ�� �ش� ���ڸ� ����
        rBootData_Rx[gBoot_Rx_cnt] = rDataA[0];
    }

	// 3. SCI ����� �����÷� ���¸� �ʱ�ȭ
    SCI_clearOverflowStatus(BOOT_SCI_BASE);

	// 4. SCI ���� FIFO ����(RXFF)�� ���� ���ͷ�Ʈ ���� �÷��׸� �ʱ�ȭ
    SCI_clearInterruptStatus(BOOT_SCI_BASE, SCI_INT_RXFF);

	// 5. ���ͷ�Ʈ �׷� 9�� ACK �÷��׸� �ʱ�ȭ
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

	// 6. ī���� gRx_cnt�� 1 �������� ���� ���ŵ� ���ڰ� ���� �� rDataPointA �迭�� ���� ��ġ�� ����
    gBoot_Rx_cnt++;
}

//  uart �۽� ���ͷ�Ʈ ���� ��ƾ �Լ�
__interrupt void Boot_Tx_ISR(void)
{

	// 1.  SCI �۽� FIFO ����(TXFF)�� ���ͷ�Ʈ ���� �÷��׸� �ʱ�ȭ
    SCI_clearInterruptStatus(BOOT_SCI_BASE, SCI_INT_TXFF);
	
	// 2. ���ͷ�Ʈ �׷� 9�� ACK �÷��׸� �ʱ�ȭ
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}


//  uart ���� ���ͷ�Ʈ ���� ��ƾ �Լ�
__interrupt void Barcode_Rx_ISR(void)
{
    uint16_t rDataC[2];

    //1. ���� ���ۿ��� �� ���ڸ� �о�� rDataA[0] ��ġ�� ����
    SCI_readCharArray(BARCODE_SCI_BASE, rDataC, 1);

    // 2. ���ŵ� ���ڰ� 0x0D���� Ȯ��
    if((rDataC[0] & 0x00FFU) == 0x0DU)
    {
        // 2.1 ���� ���ڰ� 0x0D �̸� �޼��� �Է� �Ϸ�, gRx_done = 1�� �����Ͽ� �޼��� �Ϸ� ����, ���� ���� ī���ʹ� -1�� �ʱ�ȭ
        gBarcode_Rx_done = TRUE;
        gBarcode_Rx_cnt = -1;

    }
    else
    {
        // 2.2 ���ŵ� ���ڰ� ĳ���� ����(0x0D)�� �ƴ� ���, rDataPointA ������ gRx_cnt ��ġ�� �ش� ���ڸ� ����
        rBarcaodeData_Rx[gBarcode_Rx_cnt] = rDataC[0];
    }

    // 3. SCI ����� �����÷� ���¸� �ʱ�ȭ
    SCI_clearOverflowStatus(BARCODE_SCI_BASE);

    // 4. SCI ���� FIFO ����(RXFF)�� ���� ���ͷ�Ʈ ���� �÷��׸� �ʱ�ȭ
    SCI_clearInterruptStatus(BARCODE_SCI_BASE, SCI_INT_RXFF);

    // 5. ���ͷ�Ʈ �׷� 9�� ACK �÷��׸� �ʱ�ȭ
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    // 6. ī���� gRx_cnt�� 1 �������� ���� ���ŵ� ���ڰ� ���� �� rDataPointA �迭�� ���� ��ġ�� ����
    gBarcode_Rx_cnt++;
}

//  uart �۽� ���ͷ�Ʈ ���� ��ƾ �Լ�
__interrupt void Barcode_Tx_ISR(void)
{

    // 1.  SCI �۽� FIFO ����(TXFF)�� ���ͷ�Ʈ ���� �÷��׸� �ʱ�ȭ
    SCI_clearInterruptStatus(BARCODE_SCI_BASE, SCI_INT_TXFF);

    // 2. ���ͷ�Ʈ �׷� 9�� ACK �÷��׸� �ʱ�ȭ
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//  uart ���� ���ͷ�Ʈ ���� ��ƾ �Լ�
__interrupt void Rs485_Rx_ISR(void)
{
    uint16_t rDataB[2];

    //1. ���� ���ۿ��� �� ���ڸ� �о�� rDataA[0] ��ġ�� ����
    SCI_readCharArray(RS485_SCI_BASE, rDataB, 1);

    // 2. ���ŵ� ���ڰ� 0x0D���� Ȯ��
    if((rDataB[0] & 0x00FFU) == 0x0DU)
    {
        // 2.1 ���� ���ڰ� 0x0D �̸� �޼��� �Է� �Ϸ�, gRx_done = 1�� �����Ͽ� �޼��� �Ϸ� ����, ���� ���� ī���ʹ� -1�� �ʱ�ȭ
        gRs485_Rx_done = TRUE;
        gRs485_Rx_cnt = -1;

    }
    else
    {
        // 2.2 ���ŵ� ���ڰ� ĳ���� ����(0x0D)�� �ƴ� ���, rDataPointA ������ gRx_cnt ��ġ�� �ش� ���ڸ� ����
        rRs485Data_Rx[gRs485_Rx_cnt] = rDataB[0];
    }

    // 3. SCI ����� �����÷� ���¸� �ʱ�ȭ
    SCI_clearOverflowStatus(RS485_SCI_BASE);

    // 4. SCI ���� FIFO ����(RXFF)�� ���� ���ͷ�Ʈ ���� �÷��׸� �ʱ�ȭ
    SCI_clearInterruptStatus(RS485_SCI_BASE, SCI_INT_RXFF);

    // 5. ���ͷ�Ʈ �׷� 9�� ACK �÷��׸� �ʱ�ȭ
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    // 6. ī���� gRx_cnt�� 1 �������� ���� ���ŵ� ���ڰ� ���� �� rDataPointA �迭�� ���� ��ġ�� ����
    gRs485_Rx_cnt++;
}

//  uart �۽� ���ͷ�Ʈ ���� ��ƾ �Լ�
__interrupt void Rs485_Tx_ISR(void)
{

    // 1.  SCI �۽� FIFO ����(TXFF)�� ���ͷ�Ʈ ���� �÷��׸� �ʱ�ȭ
    SCI_clearInterruptStatus(RS485_SCI_BASE, SCI_INT_TXFF);

    // 2. ���ͷ�Ʈ �׷� 9�� ACK �÷��׸� �ʱ�ȭ
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//  uart ���� ���ͷ�Ʈ ���� ��ƾ �Լ�
__interrupt void uartTTL_Rx_ISR(void)
{
    uint16_t rDataD[2];

    //1. ���� ���ۿ��� �� ���ڸ� �о�� rDataA[0] ��ġ�� ����
    SCI_readCharArray(UART_TTL_BASE, rDataD, 1);

    // 2. ���ŵ� ���ڰ� 0x0D���� Ȯ��
    if((rDataD[0] & 0x00FFU) == 0x0DU)
    {
        // 2.1 ���� ���ڰ� 0x0D �̸� �޼��� �Է� �Ϸ�, gRx_done = 1�� �����Ͽ� �޼��� �Ϸ� ����, ���� ���� ī���ʹ� -1�� �ʱ�ȭ
        gUart_Rx_done = TRUE;
        gUart_Rx_cnt = -1;

    }
    else
    {
        // 2.2 ���ŵ� ���ڰ� ĳ���� ����(0x0D)�� �ƴ� ���, rDataPointA ������ gRx_cnt ��ġ�� �ش� ���ڸ� ����
        rUartData_Rx[gUart_Rx_cnt] = rDataD[0];
    }

    // 3. SCI ����� �����÷� ���¸� �ʱ�ȭ
    SCI_clearOverflowStatus(UART_TTL_BASE);

    // 4. SCI ���� FIFO ����(RXFF)�� ���� ���ͷ�Ʈ ���� �÷��׸� �ʱ�ȭ
    SCI_clearInterruptStatus(UART_TTL_BASE, SCI_INT_RXFF);

    // 5. ���ͷ�Ʈ �׷� 9�� ACK �÷��׸� �ʱ�ȭ
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    // 6. ī���� gRx_cnt�� 1 �������� ���� ���ŵ� ���ڰ� ���� �� rDataPointA �迭�� ���� ��ġ�� ����
    gUart_Rx_cnt++;
}

//  uart �۽� ���ͷ�Ʈ ���� ��ƾ �Լ�
__interrupt void uartTTL_Tx_ISR(void)
{

    // 1.  SCI �۽� FIFO ����(TXFF)�� ���ͷ�Ʈ ���� �÷��׸� �ʱ�ȭ
    SCI_clearInterruptStatus(UART_TTL_BASE, SCI_INT_TXFF);

    // 2. ���ͷ�Ʈ �׷� 9�� ACK �÷��׸� �ʱ�ȭ
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}


