/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_usbdev.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_USBDEV_H__
#define __CH57x_USBDEV_H__

#ifdef __cplusplus
extern "C" {
#endif

/* HID class requests */
#define DEF_USB_GET_IDLE           0x02                                         /* get idle for key or mouse */
#define DEF_USB_GET_PROTOCOL       0x03                                         /* get protocol for bios type */
#define DEF_USB_SET_REPORT         0x09                                         /* set report for key */
#define DEF_USB_SET_IDLE           0x0A                                         /* set idle for key or mouse */
#define DEF_USB_SET_PROTOCOL       0x0B                                         /* set protocol for bios type */

/* The following buffer areas are data buffers used by the USB module for communication, a total of 9 channels (9 cache blocks), users can define corresponding buffer areas according to the actual number of channels used */
extern uint8_t *pEP0_RAM_Addr; //ep0(64)+ep4_out(64)+ep4_in(64)
extern uint8_t *pEP1_RAM_Addr; //ep1_out(64)+ep1_in(64)
extern uint8_t *pEP2_RAM_Addr; //ep2_out(64)+ep2_in(64)
extern uint8_t *pEP3_RAM_Addr; //ep3_out(64)+ep3_in(64)

#define pSetupReqPak          ((PUSB_SETUP_REQ)pEP0_RAM_Addr)
#define pEP0_DataBuf          (pEP0_RAM_Addr)
#define pEP1_OUT_DataBuf      (pEP1_RAM_Addr)
#define pEP1_IN_DataBuf       (pEP1_RAM_Addr + 64)
#define pEP2_OUT_DataBuf      (pEP2_RAM_Addr)
#define pEP2_IN_DataBuf       (pEP2_RAM_Addr + 64)
#define pEP3_OUT_DataBuf      (pEP3_RAM_Addr)
#define pEP3_IN_DataBuf       (pEP3_RAM_Addr + 64)
#define pEP4_OUT_DataBuf      (pEP0_RAM_Addr + 64)
#define pEP4_IN_DataBuf       (pEP0_RAM_Addr + 128)

/**
 * @brief   USB device function initialization, 4 endpoints, 8 channels.
 */
void USB_DeviceInit(void);

/**
 * @brief   USB device response transfer processing
 */
void USB_DevTransProcess(void);

/**
 * @brief   Endpoint 1 downstream data processing
 *
 * @param   l   - Length of data to process (<64B)
 */
void DevEP1_OUT_Deal(uint8_t l);

/**
 * @brief   Endpoint 2 downstream data processing
 *
 * @param   l   - Length of data to process (<64B)
 */
void DevEP2_OUT_Deal(uint8_t l);

/**
 * @brief   Endpoint 3 downstream data processing
 *
 * @param   l   - Length of data to process (<64B)
 */
void DevEP3_OUT_Deal(uint8_t l);

/**
 * @brief   Endpoint 4 downstream data processing
 *
 * @param   l   - Length of data to process (<64B)
 */
void DevEP4_OUT_Deal(uint8_t l);

/**
 * @brief   Endpoint 1 data upload
 *
 * @param   l   - Upload data length (<64B)
 */
void DevEP1_IN_Deal(uint8_t l);

/**
 * @brief   Endpoint 2 data upload
 *
 * @param   l   - Upload data length (<64B)
 */
void DevEP2_IN_Deal(uint8_t l);

/**
 * @brief   Endpoint 3 data upload
 *
 * @param   l   - Upload data length (<64B)
 */
void DevEP3_IN_Deal(uint8_t l);

/**
 * @brief   Endpoint 4 data upload
 *
 * @param   l   - Upload data length (<64B)
 */
void DevEP4_IN_Deal(uint8_t l);

/**
 * @brief   Check if endpoint 1 upload is completed
 *
 * @return  0-Not completed  (!0)-Completed
 */
#define EP1_GetINSta()    (R8_UEP1_CTRL & UEP_T_RES_NAK)

/**
 * @brief   Check if endpoint 2 upload is completed
 *
 * @return  0-Not completed  (!0)-Completed
 */
#define EP2_GetINSta()    (R8_UEP2_CTRL & UEP_T_RES_NAK)

/**
 * @brief   Check if endpoint 3 upload is completed
 *
 * @return  0-Not completed  (!0)-Completed
 */
#define EP3_GetINSta()    (R8_UEP3_CTRL & UEP_T_RES_NAK)

/**
 * @brief   Check if endpoint 4 upload is completed
 *
 * @return  0-Not completed  (!0)-Completed
 */
#define EP4_GetINSta()    (R8_UEP4_CTRL & UEP_T_RES_NAK)

/**
 * @brief   Disable USB pull-up resistor
 */
#define USB_DisablePin()  (R16_PIN_ANALOG_IE &= ~(RB_PIN_USB_IE | RB_PIN_USB_DP_PU))

/**
 * @brief   Disable USB
 */
#define USB_Disable()     (R32_USB_CONTROL = 0)

#ifdef __cplusplus
}
#endif

#endif // __CH57x_USBDEV_H__
