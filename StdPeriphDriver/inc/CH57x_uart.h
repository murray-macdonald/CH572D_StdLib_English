/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_uart.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_UART_H__
#define __CH57x_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	LINE error and status define
 */
#define STA_ERR_BREAK     RB_LSR_BREAK_ERR    // Data interval error
#define STA_ERR_FRAME     RB_LSR_FRAME_ERR    // Data frame error
#define STA_ERR_PAR       RB_LSR_PAR_ERR      // Parity bit error
#define STA_ERR_FIFOOV    RB_LSR_OVER_ERR     // Received data overflow

#define STA_TXFIFO_EMP    RB_LSR_TX_FIFO_EMP  // Current TX FIFO is empty, can continue to fill with transmission data
#define STA_TXALL_EMP     RB_LSR_TX_ALL_EMP   // All transmission data has been sent completely
#define STA_RECV_DATA     RB_LSR_DATA_RDY     // Data currently received

/**
 * @brief  Configuration UART TrigByte num
 */
typedef enum
{
    UART_1BYTE_TRIG = 0, // 1 byte trigger
    UART_2BYTE_TRIG,     // 2 byte trigger
    UART_4BYTE_TRIG,     // 4 byte trigger
    UART_7BYTE_TRIG,     // 7 byte trigger

} UARTByteTRIGTypeDef;

/**
 * @brief  UART Tx Pin Remap Def
 */
typedef enum
{
    UART_TX_REMAP_PA3 = 0, /*!<Default mapping (TXD/PA3) */
    UART_TX_REMAP_PA2,     /*!<Remapped (TXD/PA2) */
    UART_TX_REMAP_PA1,     /*!<Remapped (TXD/PA1) */
    UART_TX_REMAP_PA0,     /*!<Remapped (TXD/PA0) */
    UART_TX_REMAP_PA7,     /*!<Remapped (TXD/PA7) */
    UART_TX_REMAP_PA8,     /*!<Remapped (TXD/PA8) */
    UART_TX_REMAP_PA11,    /*!<Remapped (TXD/PA11) */
    UART_TX_REMAP_PA10,    /*!<Remapped (TXD/PA10) */
} UARTTxPinRemapDef;

/**
 * @brief  UART Rx Pin Remap Def
 */
typedef enum
{
    UART_RX_REMAP_PA2 = 0, /*!<Default mapping (RXD/PA2) */
    UART_RX_REMAP_PA3,     /*!<Remapped (RXD/PA3) */
    UART_RX_REMAP_PA0,     /*!<Remapped (RXD/PA0) */
    UART_RX_REMAP_PA1,     /*!<Remapped (RXD/PA1) */
    UART_RX_REMAP_PA4,     /*!<Remapped (RXD/PA4) */
    UART_RX_REMAP_PA9,     /*!<Remapped (RXD/PA9) */
    UART_RX_REMAP_PA10,    /*!<Remapped (RXD/PA10) */
    UART_RX_REMAP_PA11,    /*!<Remapped (RXD/PA11) */
} UARTRxPinRemapDef;

/**
 * @brief   UART default initialization configuration
 */
void UART_DefInit(void);

/**
 * @brief   UART baud rate configuration
 *
 * @param   baudrate    - Baud rate
 */
void UART_BaudRateCfg(uint32_t baudrate);

/**
 * @brief   UART byte trigger interrupt configuration
 *
 * @param   b       - Trigger byte count, refer to UARTByteTRIGTypeDef
 */
void UART_ByteTrigCfg(UARTByteTRIGTypeDef b);

/**
 * @brief   UART interrupt configuration
 *
 * @param   s       - Interrupt control status, whether to enable the corresponding interrupt
 * @param   i       - Interrupt type
 *                    RB_IER_MODEM_CHG  - Modem input status change interrupt enable bit (only UART0 supported)
 *                    RB_IER_LINE_STAT  - Receive line status interrupt
 *                    RB_IER_THR_EMPTY  - Transmit holding register empty interrupt
 *                    RB_IER_RECV_RDY   - Receive data interrupt
 */
void UART_INTCfg(FunctionalState s, uint8_t i);

/**
 * @brief   Clear current receive FIFO
 */
#define UART_CLR_RXFIFO()    (R8_UART_FCR |= RB_FCR_RX_FIFO_CLR)

/**
 * @brief   Clear current transmit FIFO
 */
#define UART_CLR_TXFIFO()    (R8_UART_FCR |= RB_FCR_TX_FIFO_CLR)

/**
 * @brief   Get current interrupt flag
 *
 * @return  Current interrupt flag
 */
#define UART_GetITFlag()     (R8_UART_IIR & RB_IIR_INT_MASK)

/**
 * @brief   Get current communication status
 *
 * @return  refer to LINE error and status define
 */
#define UART_GetLinSTA()     (R8_UART_LSR)

/**
 * @brief   UART single byte transmission
 *
 * @param   b       Byte to be sent
 */
#define UART_SendByte(b)     (R8_UART_THR = b)

/**
 * @brief   UART multi-byte transmission
 *
 * @param   buf     - First address of data content to be sent
 * @param   l       - Length of data to be sent
 */
void UART_SendString(uint8_t *buf, uint16_t l);

/**
 * @brief   UART read single byte
 *
 * @return  Single byte read
 */
#define UART_RecvByte()    (R8_UART_RBR)

/**
 * @brief   UART read multiple bytes
 *
 * @param   buf     - First address of the buffer for storing read data
 *
 * @return  Length of data read
 */
uint16_t UART_RecvString(uint8_t *buf);

/**
 * @brief   UART pin mapping
 *
 * @param   s       - Whether to enable mapping
 * @param   perph   - Write Tx and Rx mapping relationship
 */
void UART_Remap(FunctionalState s, UARTTxPinRemapDef u_tx, UARTRxPinRemapDef u_rx);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_UART_H__
