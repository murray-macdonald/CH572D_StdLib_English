/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_usbdev.c
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

uint8_t *pEP0_RAM_Addr;
uint8_t *pEP1_RAM_Addr;
uint8_t *pEP2_RAM_Addr;
uint8_t *pEP3_RAM_Addr;

/*********************************************************************
 * @fn      USB_DeviceInit
 *
 * @brief   USB device function initialization, 4 endpoints, 8 channels.
 *
 * @param   none
 *
 * @return  none
 */
void USB_DeviceInit(void)
{
    R8_USB_CTRL = 0x00; // First set mode, cancel RB_UC_CLR_ALL

    R8_UEP4_1_MOD = RB_UEP4_RX_EN | RB_UEP4_TX_EN | RB_UEP1_RX_EN | RB_UEP1_TX_EN; // Endpoint 4 OUT+IN, endpoint 1 OUT+IN
    R8_UEP2_3_MOD = RB_UEP2_RX_EN | RB_UEP2_TX_EN | RB_UEP3_RX_EN | RB_UEP3_TX_EN; // Endpoint 2 OUT+IN, endpoint 3 OUT+IN

    R16_UEP0_DMA = (uint16_t)(uint32_t)pEP0_RAM_Addr;
    R16_UEP1_DMA = (uint16_t)(uint32_t)pEP1_RAM_Addr;
    R16_UEP2_DMA = (uint16_t)(uint32_t)pEP2_RAM_Addr;
    R16_UEP3_DMA = (uint16_t)(uint32_t)pEP3_RAM_Addr;

    R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP4_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;

    R8_USB_DEV_AD = 0x00;
    R8_USB_CTRL = RB_UC_DEV_PU_EN | RB_UC_INT_BUSY | RB_UC_DMA_EN; // Enable USB device and DMA, automatically return NAK before interrupt flag is cleared during interrupt period
    R16_PIN_ALTERNATE |= RB_PIN_USB_EN | RB_UDP_PU_EN;         // Prevent USB port from floating and enable pull-up resistor
    R8_USB_INT_FG = 0xFF;                                          // Clear interrupt flags
    R8_UDEV_CTRL = RB_UD_PD_DIS | RB_UD_PORT_EN;                   // Enable USB port
    R8_USB_INT_EN = RB_UIE_SUSPEND | RB_UIE_BUS_RST | RB_UIE_TRANSFER;
}

/*********************************************************************
 * @fn      DevEP1_IN_Deal
 *
 * @brief   Endpoint 1 data upload
 *
 * @param   l   - Upload data length (<64B)
 *
 * @return  none
 */
void DevEP1_IN_Deal(uint8_t l)
{
    R8_UEP1_T_LEN = l;
    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
}

/*********************************************************************
 * @fn      DevEP2_IN_Deal
 *
 * @brief   Endpoint 2 data upload
 *
 * @param   l   - Upload data length (<64B)
 *
 * @return  none
 */
void DevEP2_IN_Deal(uint8_t l)
{
    R8_UEP2_T_LEN = l;
    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
}

/*********************************************************************
 * @fn      DevEP3_IN_Deal
 *
 * @brief   Endpoint 3 data upload
 *
 * @param   l   - Upload data length (<64B)
 *
 * @return  none
 */
void DevEP3_IN_Deal(uint8_t l)
{
    R8_UEP3_T_LEN = l;
    R8_UEP3_CTRL = (R8_UEP3_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
}

/*********************************************************************
 * @fn      DevEP4_IN_Deal
 *
 * @brief   Endpoint 4 data upload
 *
 * @param   l   - Upload data length (<64B)
 *
 * @return  none
 */
void DevEP4_IN_Deal(uint8_t l)
{
    R8_UEP4_T_LEN = l;
    R8_UEP4_CTRL = (R8_UEP4_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
}
