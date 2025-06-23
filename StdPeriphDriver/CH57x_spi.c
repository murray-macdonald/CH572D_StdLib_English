/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_SPI.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : source file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

/*********************************************************************
 * @fn      SPI_MasterDefInit
 *
 * @brief   Master mode default initialization: Mode 0 + 3-wire full-duplex + 8MHz
 *
 * @param   none
 *
 * @return  none
 */
void SPI_MasterDefInit(void)
{
    R8_SPI_CLOCK_DIV = 4; // Main clock divided by 4
    R8_SPI_CTRL_MOD = RB_SPI_ALL_CLEAR;
    R8_SPI_CTRL_MOD = RB_SPI_MOSI_OE | RB_SPI_SCK_OE;
    R8_SPI_CTRL_CFG |= RB_SPI_AUTO_IF;     // Automatically clear IF_BYTE_END flag when accessing BUFFER/FIFO
    R8_SPI_CTRL_CFG &= ~RB_SPI_DMA_ENABLE; // Do not enable DMA mode
}

/*********************************************************************
 * @fn      SPI_2WIRE_MasterOutputInit
 *
 * @brief   Master 2-wire transmission mode initialization: Mode 1 + 2-wire half-duplex + 8MHz
 *
 * @param   none
 *
 * @return  none
 */
void SPI_2WIRE_MasterOutputInit(void)
{
    R8_SPI_CLOCK_DIV = 4; // Main clock divided by 4
    R8_SPI_CTRL_MOD = RB_SPI_ALL_CLEAR;
    R8_SPI_CTRL_MOD =  RB_SPI_MOSI_OE | RB_SPI_SCK_OE | RB_SPI_2WIRE_MOD;  // Enable two-wire mode
    R8_SPI_CTRL_CFG |= RB_SPI_AUTO_IF;     // Automatically clear IF_BYTE_END flag when accessing BUFFER/FIFO
    R8_SPI_CTRL_CFG &= ~RB_SPI_DMA_ENABLE; // Do not enable DMA mode
}

/*********************************************************************
 * @fn      SPI_2WIRE_MasterReceiveInit
 *
 * @brief   Master 2-wire reception mode initialization: Mode 1 + 2-wire half-duplex + 8MHz
 *
 * @param   none
 *
 * @return  none
 */
void SPI_2WIRE_MasterReceiveInit(void)
{
    R8_SPI_CLOCK_DIV = 4; // Main clock divided by 4
    R8_SPI_CTRL_MOD = RB_SPI_ALL_CLEAR;
    R8_SPI_CTRL_MOD = RB_SPI_SCK_OE | RB_SPI_2WIRE_MOD;  // Enable two-wire mode
    R8_SPI_CTRL_CFG |= RB_SPI_AUTO_IF;     // Automatically clear IF_BYTE_END flag when accessing BUFFER/FIFO
    R8_SPI_CTRL_CFG &= ~RB_SPI_DMA_ENABLE; // Do not enable DMA mode
}

/*********************************************************************
 * @fn      SPI_2WIRE_SlaveInputInit
 *
 * @brief   Slave 2-wire reception mode initialization: Mode 1 + 2-wire half-duplex + 8MHz
 *
 * @param   none
 *
 * @return  none
 */
void SPI_2WIRE_SlaveInputInit(void)
{
    R8_SPI_CTRL_MOD =  RB_SPI_ALL_CLEAR;
    R8_SPI_CTRL_MOD =  RB_SPI_2WIRE_MOD | RB_SPI_MODE_SLAVE;
    R8_SPI_CTRL_CFG |= RB_SPI_AUTO_IF | RB_MST_CLK_SEL;
}

/*********************************************************************
 * @fn      SPI_2WIRE_SlaveOutputInit
 *
 * @brief   Slave 2-wire transmission mode initialization: Mode 1 + 2-wire half-duplex + 8MHz
 *
 * @param   none
 *
 * @return  none
 */
void SPI_2WIRE_SlaveOutputInit(void)
{
    R8_SPI_CTRL_MOD =  RB_SPI_ALL_CLEAR;
    R8_SPI_CTRL_MOD =  RB_SPI_MISO_OE | RB_SPI_2WIRE_MOD | RB_SPI_MODE_SLAVE;
    R8_SPI_CTRL_CFG |= RB_SPI_AUTO_IF;
}

/*********************************************************************
 * @fn      SPI_CLKCfg
 *
 * @brief   SPI reference clock configuration, = d*Tsys
 *
 * @param   c       - Clock division factor
 *
 * @return  none
 */
void SPI_CLKCfg(uint8_t c)
{
    if(c == 2)
    {
        R8_SPI_CTRL_CFG |= RB_SPI_MST_DLY_EN;
    }
    else
    {
        R8_SPI_CTRL_CFG &= ~RB_SPI_MST_DLY_EN;
    }
    R8_SPI_CLOCK_DIV = c;
}

/*********************************************************************
 * @fn      SPI_DataMode
 *
 * @brief   Set data stream mode
 *
 * @param   m       - Data stream mode, refer to ModeBitOrderTypeDef
 *
 * @return  none
 */
void SPI_DataMode(ModeBitOrderTypeDef m)
{
    switch(m)
    {
        case Mode0_LowBitINFront:
            R8_SPI_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD;
            R8_SPI_CTRL_CFG |= RB_SPI_BIT_ORDER;
            break;
        case Mode0_HighBitINFront:
            R8_SPI_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD;
            R8_SPI_CTRL_CFG &= ~RB_SPI_BIT_ORDER;
            break;
        case Mode3_LowBitINFront:
            R8_SPI_CTRL_MOD |= RB_SPI_MST_SCK_MOD;
            R8_SPI_CTRL_CFG |= RB_SPI_BIT_ORDER;
            break;
        case Mode3_HighBitINFront:
            R8_SPI_CTRL_MOD |= RB_SPI_MST_SCK_MOD;
            R8_SPI_CTRL_CFG &= ~RB_SPI_BIT_ORDER;
            break;
        default:
            break;
    }
}

/*********************************************************************
 * @fn      SPI_MasterSendByte
 *
 * @brief   Send a single byte (buffer)
 *
 * @param   d       - Byte to send
 *
 * @return  none
 */
void SPI_MasterSendByte(uint8_t d)
{
    R8_SPI_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R16_SPI_TOTAL_CNT = 1;
    R8_SPI_FIFO = d;
    while(!(R8_SPI_INT_FLAG & RB_SPI_FREE));
}

/*********************************************************************
 * @fn      SPI_MasterRecvByte
 *
 * @brief   Receive a single byte (buffer)
 *
 * @param   none
 *
 * @return  Received byte
 */
uint8_t SPI_MasterRecvByte(void)
{
    R8_SPI_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R8_SPI_BUFFER = 0xFF; // Start transmission
    while(!(R8_SPI_INT_FLAG & RB_SPI_FREE));
    return (R8_SPI_BUFFER);
}

/*********************************************************************
 * @fn      SPI_MasterTrans
 *
 * @brief   Use FIFO to continuously send multiple bytes
 *
 * @param   pbuf    - Starting address of data content to be sent
 * @param   len     - Requested length of data to be sent, maximum 4095
 *
 * @return  none
 */
void SPI_MasterTrans(uint8_t *pbuf, uint16_t len)
{
    uint16_t sendlen;

    sendlen = len;
    R8_SPI_CTRL_MOD &= ~RB_SPI_FIFO_DIR; // Set data direction to output
    R16_SPI_TOTAL_CNT = sendlen;         // Set the length of data to be sent
    R8_SPI_INT_FLAG = RB_SPI_IF_CNT_END;
    while(sendlen)
    {
        if(R8_SPI_FIFO_COUNT < SPI_FIFO_SIZE)
        {
            R8_SPI_FIFO = *pbuf;
            pbuf++;
            sendlen--;
        }
    }
    while(R8_SPI_FIFO_COUNT != 0); // Wait for all data in FIFO to complete transmission
}

/*********************************************************************
 * @fn      SPI_MasterRecv
 *
 * @brief   Use FIFO to continuously receive multiple bytes
 *
 * @param   pbuf    - Starting address for received data
 * @param   len     - Length of data to be received, maximum 4095
 *
 * @return  none
 */
void SPI_MasterRecv(uint8_t *pbuf, uint16_t len)
{
    uint16_t readlen;

    readlen = len;
    R8_SPI_CTRL_MOD |= RB_SPI_FIFO_DIR; // Set data direction to input
    R16_SPI_TOTAL_CNT = len;            // Set the length of data to be received, transmission starts if FIFO direction is input and length is not 0 */
    R8_SPI_INT_FLAG = RB_SPI_IF_CNT_END;
    while(readlen)
    {
        if(R8_SPI_FIFO_COUNT)
        {
            *pbuf = R8_SPI_FIFO;
            pbuf++;
            readlen--;
        }
    }
}

/*********************************************************************
 * @fn      SPI_MasterDMATrans
 *
 * @brief   DMA mode for continuous data transmission
 *
 * @param   pbuf    - Starting address of data to be sent, must be four-byte aligned
 * @param   len     - Length of data to be sent
 *
 * @return  none
 */
void SPI_MasterDMATrans(uint8_t *pbuf, uint16_t len)
{
    R8_SPI_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R16_SPI_DMA_BEG = (uint32_t)pbuf;
    R16_SPI_DMA_END = (uint32_t)(pbuf + len);
    R16_SPI_TOTAL_CNT = len;
    R8_SPI_INT_FLAG = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END;
    R8_SPI_CTRL_CFG |= RB_SPI_DMA_ENABLE;
    while(!(R8_SPI_INT_FLAG & RB_SPI_IF_CNT_END));
    R8_SPI_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}

/*********************************************************************
 * @fn      SPI_MasterDMARecv
 *
 * @brief   DMA mode for continuous data reception
 *
 * @param   pbuf    - Starting address to store received data, must be four-byte aligned
 * @param   len     - Length of data to be received
 *
 * @return  none
 */
void SPI_MasterDMARecv(uint8_t *pbuf, uint16_t len)
{
    R8_SPI_CTRL_MOD |= RB_SPI_FIFO_DIR;
    R16_SPI_DMA_BEG = (uint32_t)pbuf;
    R16_SPI_DMA_END = (uint32_t)(pbuf + len);
    R16_SPI_TOTAL_CNT = len;
    R8_SPI_INT_FLAG = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END;
    R8_SPI_CTRL_CFG |= RB_SPI_DMA_ENABLE;
    while(!(R8_SPI_INT_FLAG & RB_SPI_IF_CNT_END));
    R8_SPI_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}

/*********************************************************************
 * @fn      SPI_SlaveInit
 *
 * @brief   Default initialization for device mode, recommended to set the GPIO for MISO as input mode
 *
 * @return  none
 */
void SPI_SlaveInit(void)
{
    R8_SPI_CTRL_MOD = RB_SPI_ALL_CLEAR;
    R8_SPI_CTRL_MOD = RB_SPI_MISO_OE | RB_SPI_MODE_SLAVE;
    R8_SPI_CTRL_CFG |= RB_SPI_AUTO_IF;
}

/*********************************************************************
 * @fn      SPI_2WIRE_SlaveInit
 *
 * @brief   Device two-wire mode initialization
 *
 * @return  none
 */
void SPI_2WIRE_SlaveInit(void)
{
    R8_SPI_CTRL_MOD = RB_SPI_ALL_CLEAR;
    R8_SPI_CTRL_MOD = RB_SPI_MISO_OE | RB_SPI_2WIRE_MOD | RB_SPI_MODE_SLAVE;
    R8_SPI_CTRL_CFG |= RB_SPI_AUTO_IF;
}

/*********************************************************************
 * @fn      SPI_SlaveRecvByte
 *
 * @brief   Slave mode, receive one byte of data
 *
 * @return  Received data
 */
uint8_t SPI_SlaveRecvByte(void)
{
    R8_SPI_CTRL_MOD |= RB_SPI_FIFO_DIR;
    while(R8_SPI_FIFO_COUNT == 0);
    return R8_SPI_FIFO;
}

/*********************************************************************
 * @fn      SPI_SlaveSendByte
 *
 * @brief   Slave mode, send one byte of data
 *
 * @param   d       - Data to be sent
 *
 * @return  none
 */
void SPI_SlaveSendByte(uint8_t d)
{
    R8_SPI_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R16_SPI_TOTAL_CNT = 1;
    R8_SPI_FIFO = d;
    while(R8_SPI_FIFO_COUNT != 0); // Wait for transmission to complete
}

/*********************************************************************
 * @fn      SPI_SlaveRecv
 *
 * @brief   Slave mode, receive multiple bytes of data
 *
 * @param   pbuf    - Starting address to store received data
 * @param   len     - Requested length of data to receive
 *
 * @return  none
 */
__HIGH_CODE
void SPI_SlaveRecv(uint8_t *pbuf, uint16_t len)
{
    uint16_t revlen;

    revlen = len;
    R8_SPI_CTRL_MOD |= RB_SPI_FIFO_DIR;
    R8_SPI_INT_FLAG = RB_SPI_IF_CNT_END;
    while(revlen)
    {
        if(R8_SPI_FIFO_COUNT)
        {
            *pbuf = R8_SPI_FIFO;
            pbuf++;
            revlen--;
        }
    }
}

/*********************************************************************
 * @fn      SPI_SlaveTrans
 *
 * @brief   Slave mode, send multiple bytes of data
 *
 * @param   pbuf    - Starting address of data content to be sent
 * @param   len     - Requested length of data to be sent, maximum 4095
 *
 * @return  none
 */
__HIGH_CODE
void SPI_SlaveTrans(uint8_t *pbuf, uint16_t len)
{
    uint16_t sendlen;

    sendlen = len;
    R8_SPI_CTRL_MOD &= ~RB_SPI_FIFO_DIR; // Set data direction to output
    R8_SPI_INT_FLAG = RB_SPI_IF_CNT_END;
    while(sendlen)
    {
        if(R8_SPI_FIFO_COUNT < SPI_FIFO_SIZE)
        {
            R8_SPI_FIFO = *pbuf;
            pbuf++;
            sendlen--;
        }
    }
    while(R8_SPI_FIFO_COUNT != 0); // Wait for all data in FIFO to complete transmission
}

/*********************************************************************
 * @fn      SPI_SlaveDMARecv
 *
 * @brief   DMA mode for continuous data reception
 *
 * @param   pbuf    - Starting address to store received data, must be four-byte aligned
 * @param   len     - Length of data to be received
 *
 * @return  none
 */
void SPI_SlaveDMARecv(uint8_t *pbuf, uint16_t len)
{
    R8_SPI_CTRL_MOD |= RB_SPI_FIFO_DIR;
    R16_SPI_DMA_BEG = (uint32_t)pbuf;
    R16_SPI_DMA_END = (uint32_t)(pbuf + len);
    R16_SPI_TOTAL_CNT = len;
    R8_SPI_INT_FLAG = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END;
    R8_SPI_CTRL_CFG |= RB_SPI_DMA_ENABLE;
    while(!(R8_SPI_INT_FLAG & RB_SPI_IF_CNT_END));
    R8_SPI_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}

/*********************************************************************
 * @fn      SPI_SlaveDMATrans
 *
 * @brief   DMA mode for continuous data transmission
 *
 * @param   pbuf    - Starting address of data to be sent, must be four-byte aligned
 * @param   len     - Length of data to be sent
 *
 * @return  none
 */
void SPI_SlaveDMATrans(uint8_t *pbuf, uint16_t len)
{
    R8_SPI_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R16_SPI_DMA_BEG = (uint32_t)pbuf;
    R16_SPI_DMA_END = (uint32_t)(pbuf + len);
    R16_SPI_TOTAL_CNT = len;
    R8_SPI_INT_FLAG = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END;
    R8_SPI_CTRL_CFG |= RB_SPI_DMA_ENABLE;
    while(!(R8_SPI_INT_FLAG & RB_SPI_IF_CNT_END));
    R8_SPI_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}
