/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_SPI.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_SPI_H__
#define __CH57x_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  SPI interrupt bit define
 */
#define SPI_IT_FST_BYTE    RB_SPI_IE_FST_BYTE  // First byte interrupt received in slave mode first byte command mode
#define SPI_IT_FIFO_OV     RB_SPI_IE_FIFO_OV   // FIFO overflow
#define SPI_IT_DMA_END     RB_SPI_IE_DMA_END   // DMA transfer complete
#define SPI_IT_FIFO_HF     RB_SPI_IE_FIFO_HF   // FIFO half-full
#define SPI_IT_BYTE_END    RB_SPI_IE_BYTE_END  // Single byte transfer complete
#define SPI_IT_CNT_END     RB_SPI_IE_CNT_END   // All bytes transfer complete

/**
 * @brief  Configuration data mode
 */
typedef enum
{
    Mode0_LowBitINFront = 0, // Mode 0, LSB first
    Mode0_HighBitINFront,    // Mode 0, MSB first
    Mode3_LowBitINFront,     // Mode 3, LSB first
    Mode3_HighBitINFront,    // Mode 3, MSB first
} ModeBitOrderTypeDef;

/**
 * @brief  Configuration SPI slave mode
 */
typedef enum
{
    Mode_DataStream = 0, // Data stream mode
    Mose_FirstCmd,       // First byte command mode
} Slave_ModeTypeDef;

/**
 * @brief   Master mode default initialization: Mode 0 + 3-wire full-duplex + 8MHz
 */
void SPI_MasterDefInit(void);

/**
 * @brief   Master 2-wire transmit mode initialization: Mode 1 + 2-wire half-duplex + 8MHz
 */
void SPI_2WIRE_MasterOutputInit(void);

/**
 * @brief   Master 2-wire receive mode initialization: Mode 1 + 2-wire half-duplex + 8MHz
 */
void SPI_2WIRE_MasterReceiveInit(void);

/**
 * @brief   Slave 2-wire receive mode initialization
 */
void SPI_2WIRE_SlaveInputInit(void);

/**
 * @brief   Slave 2-wire transmit mode initialization
 */
void SPI_2WIRE_SlaveOutputInit(void);

/**
 * @brief   SPI reference clock configuration, = d*Tsys
 *
 * @param   c       - Clock division factor
 */
void SPI_CLKCfg(uint8_t c);

/**
 * @brief   Set data stream mode
 *
 * @param   m       - Data stream mode, refer to ModeBitOrderTypeDef
 */
void SPI_DataMode(ModeBitOrderTypeDef m);

/**
 * @brief   Send single byte (buffer)
 *
 * @param   d       - Byte to send
 */
void SPI_MasterSendByte(uint8_t d);

/**
 * @brief   Receive single byte (buffer)
 *
 * @param   none
 */
uint8_t SPI_MasterRecvByte(void);

/**
 * @brief   Send multiple bytes continuously using FIFO
 *
 * @param   pbuf    - Pointer to the data to be sent
 * @param   len     - Length of data to send, maximum 4095
 */
void SPI_MasterTrans(uint8_t *pbuf, uint16_t len);

/**
 * @brief   Receive multiple bytes continuously using FIFO
 *
 * @param   pbuf    - Pointer to store received data
 * @param   len     - Length of data to receive, maximum 4095
 */
void SPI_MasterRecv(uint8_t *pbuf, uint16_t len);

/**
 * @brief   Continuously send data using DMA
 *
 * @param   pbuf    - Starting address of data to be sent, must be four-byte aligned
 * @param   len     - Length of data to be sent
 */
void SPI_MasterDMATrans(uint8_t *pbuf, uint16_t len);

/**
 * @brief   Continuously receive data using DMA
 *
 * @param   pbuf    - Starting address to store received data, must be four-byte aligned
 * @param   len     - Length of data to be received
 */
void SPI_MasterDMARecv(uint8_t *pbuf, uint16_t len);

/**
 * @brief   Load first byte data content
 *
 * @param   d       - First byte data content
 */
#define SetFirstData(d)    (R8_SPI_SLAVE_PRE = d)

/**
 * @brief   Slave mode initialization
 */
void SPI_SlaveInit(void);

/**
 * @brief   Slave 2-wire mode initialization
 */
void SPI_2WIRE_SlaveInit(void);

/**
 * @brief   Slave mode, send one byte of data
 *
 * @param   d       - Data to be sent
 */
void SPI_SlaveSendByte(uint8_t d);

/**
 * @brief   Slave mode, receive one byte of data
 *
 * @return  Received data
 */
uint8_t SPI_SlaveRecvByte(void);

/**
 * @brief   Slave mode, send multiple bytes of data
 *
 * @param   pbuf    - Pointer to the data to be sent
 * @param   len     - Length of data to send, maximum 4095
 */
void SPI_SlaveTrans(uint8_t *pbuf, uint16_t len);

/**
 * @brief   Slave mode, receive multiple bytes of data
 *
 * @param   pbuf    - Starting address to store received data
 * @param   len     - Length of data to receive
 */
void SPI_SlaveRecv(uint8_t *pbuf, uint16_t len);

/**
 * @brief   Continuously send data using DMA
 *
 * @param   pbuf    - Starting address of data to be sent, must be four-byte aligned
 * @param   len     - Length of data to be sent
 */
void SPI_SlaveDMATrans(uint8_t *pbuf, uint16_t len);

/**
 * @brief   Continuously receive data using DMA
 *
 * @param   pbuf    - Starting address to store received data, must be four-byte aligned
 * @param   len     - Length of data to be received
 */
void SPI_SlaveDMARecv(uint8_t *pbuf, uint16_t len);

/**
 * @brief   Configure SPI interrupt
 *
 * @param   s       - Enable/Disable
 * @param   f       - refer to SPI interrupt bit define
 */
#define SPI_ITCfg(s, f)       ((s) ? (R8_SPI_INTER_EN |= f) : (R8_SPI_INTER_EN &= ~f))

/**
 * @brief   Get interrupt flag status, 0-not set, (!0)-triggered
 *
 * @param   f       - refer to SPI interrupt bit define
 */
#define SPI_GetITFlag(f)      (R8_SPI_INT_FLAG & f)

/**
 * @brief   Clear current interrupt flag
 *
 * @param   f       - refer to SPI interrupt bit define
 */
#define SPI_ClearITFlag(f)    (R8_SPI_INT_FLAG = f)

/**
 * @brief   Disable SPI
 */
#define SPI_Disable()         (R8_SPI_CTRL_MOD &= ~(RB_SPI_MOSI_OE | RB_SPI_SCK_OE | RB_SPI_MISO_OE))

#ifdef __cplusplus
}
#endif

#endif // __CH57x_SPI_H__
