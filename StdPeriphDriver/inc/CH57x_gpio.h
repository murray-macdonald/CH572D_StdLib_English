/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_gpio.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_GPIO_H__
#define __CH57x_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	GPIO_pins_define
 */
#define GPIO_Pin_0      (0x00000001) /*!< Pin 0 selected */
#define GPIO_Pin_1      (0x00000002) /*!< Pin 1 selected */
#define GPIO_Pin_2      (0x00000004) /*!< Pin 2 selected */
#define GPIO_Pin_3      (0x00000008) /*!< Pin 3 selected */
#define GPIO_Pin_4      (0x00000010) /*!< Pin 4 selected */
#define GPIO_Pin_5      (0x00000020) /*!< Pin 5 selected */
#define GPIO_Pin_6      (0x00000040) /*!< Pin 6 selected */
#define GPIO_Pin_7      (0x00000080) /*!< Pin 7 selected */
#define GPIO_Pin_8      (0x00000100) /*!< Pin 8 selected */
#define GPIO_Pin_9      (0x00000200) /*!< Pin 9 selected */
#define GPIO_Pin_10     (0x00000400) /*!< Pin 10 selected */
#define GPIO_Pin_11     (0x00000800) /*!< Pin 11 selected */
#define GPIO_Pin_12     (0x00001000) /*!< Pin 12 selected */
#define GPIO_Pin_13     (0x00002000) /*!< Pin 13 selected */
#define GPIO_Pin_14     (0x00004000) /*!< Pin 14 selected */
#define GPIO_Pin_15     (0x00008000) /*!< Pin 15 selected */
#define GPIO_Pin_16     (0x00010000) /*!< Pin 16 selected */
#define GPIO_Pin_17     (0x00020000) /*!< Pin 17 selected */
#define GPIO_Pin_18     (0x00040000) /*!< Pin 18 selected */
#define GPIO_Pin_19     (0x00080000) /*!< Pin 19 selected */
#define GPIO_Pin_20     (0x00100000) /*!< Pin 20 selected */
#define GPIO_Pin_21     (0x00200000) /*!< Pin 21 selected */
#define GPIO_Pin_22     (0x00400000) /*!< Pin 22 selected */
#define GPIO_Pin_23     (0x00800000) /*!< Pin 23 selected */
#define GPIO_Pin_All    (0xFFFFFFFF) /*!< All pins selected */

/**
 * @brief   GPIO_pins_remap_define
 */
#define REMAP_RXD_PA2   0x00  /*!<Default mapping (RXD/PA2) */
#define REMAP_RXD_PA3   0x01  /*!<Remapped (RXD/PA3) */
#define REMAP_RXD_PA0   0x02  /*!<Remapped (RXD/PA0) */
#define REMAP_RXD_PA1   0x03  /*!<Remapped (RXD/PA1) */
#define REMAP_RXD_PA4   0x04  /*!<Remapped (RXD/PA4) */
#define REMAP_RXD_PA9   0x05  /*!<Remapped (RXD/PA9) */
#define REMAP_RXD_PA10  0x06  /*!<Remapped (RXD/PA10) */
#define REMAP_RXD_PA11  0x07  /*!<Remapped (RXD/PA11) */

#define REMAP_TXD_PA3   0x00  /*!<Default mapping (TXD/PA3) */
#define REMAP_TXD_PA2   0x08  /*!<Remapped (TXD/PA2) */
#define REMAP_TXD_PA1   0x10  /*!<Remapped (TXD/PA1) */
#define REMAP_TXD_PA0   0x18  /*!<Remapped (TXD/PA0) */
#define REMAP_TXD_PA7   0x20  /*!<Remapped (TXD/PA7) */
#define REMAP_TXD_PA8   0x28  /*!<Remapped (TXD/PA8) */
#define REMAP_TXD_PA11  0x30  /*!<Remapped (TXD/PA11) */
#define REMAP_TXD_PA10  0x38  /*!<Remapped (TXD/PA10) */

#define REMAP_TMR_DEFAULT   0x00   /*!<Default mapping (PWM0/PA7, CAP_IN1/PA7, CAP_IN2/PA2) */
#define REMAP_TMR_MODE1     0x40   /*!<Remapping 1 (PWM0/PA2, CAP_IN1/PA2, CAP_IN2/PA7) */
#define REMAP_TMR_MODE2     0x80   /*!<Remapping 2 (PWM0/PA4, CAP_IN1/PA4, CAP_IN2/PA9) */
#define REMAP_TMR_MODE3     0xC0   /*!<Remapping 3 (PWM0/PA9, CAP_IN1/PA9, CAP_IN2/PA4) */

#define REMAP_I2C_DEFAULT   0x00  /*!<Default mapping (SCL/PA8, SDA/PA9) */
#define REMAP_I2C_MODE1     0x200 /*!<Remapping 1 (SCL/PA0, SDA/PA1) */
#define REMAP_I2C_MODE2     0x400 /*!<Remapping 2 (SCL/PA3, SDA/PA2) */
#define REMAP_I2C_MODE3     0x600 /*!<Remapping 3 (SCL/PA5, SDA/PA6) */

/**
 * @brief  Configuration GPIO Mode
 */
typedef enum
{
    GPIO_ModeIN_Floating, //Floating input
    GPIO_ModeIN_PU,       //Pull-up input
    GPIO_ModeIN_PD,       //Pull-down input
    GPIO_ModeOut_PP_5mA,  //Push-pull output max 5mA
    GPIO_ModeOut_PP_20mA, //Push-pull output max 20mA

} GPIOModeTypeDef;

/**
 * @brief  Configuration GPIO IT Mode
 */
typedef enum
{
    GPIO_ITMode_LowLevel,  //Low level trigger
    GPIO_ITMode_HighLevel, //High level trigger
    GPIO_ITMode_FallEdge,  //Falling edge trigger
    GPIO_ITMode_RiseEdge,  //Rising edge trigger

} GPIOITModeTpDef;

/**
 * @brief   GPIOA port pin mode configuration
 *
 * @param   pin     - PA0-PA15
 * @param   mode    - Input/output type
 */
void GPIOA_ModeCfg(uint32_t pin, GPIOModeTypeDef mode);

/**
 * @brief   GPIOA port pin output set low
 *
 * @param   pin     - PA0-PA15
 */
#define GPIOA_ResetBits(pin)      (R32_PA_CLR = pin)

/**
 * @brief   GPIOA port pin output set high
 *
 * @param   pin     - PA0-PA15
 */
#define GPIOA_SetBits(pin)        (R32_PA_SET = pin)

/**
 * @brief   GPIOA port pin output level toggle
 *
 * @param   pin     - PA0-PA15
 */
#define GPIOA_InverseBits(pin)    (R32_PA_OUT ^= pin)

/**
 * @brief   GPIOA port 32-bit data return, lower 16 bits valid
 *
 * @return  GPIOA port 32-bit data
 */
#define GPIOA_ReadPort()          (R32_PA_PIN)

/**
 * @brief   GPIOA port pin status, 0-pin low level, (!0)-pin high level
 *
 * @param   pin     - PA0-PA15
 *
 * @return  GPIOA port pin status
 */
#define GPIOA_ReadPortPin(pin)    (R32_PA_PIN & (pin))

/**
 * @brief   GPIOA pin interrupt mode configuration
 *
 * @param   pin     - PA0-PA15
 * @param   mode    - Trigger type
 */
void GPIOA_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode);

/**
 * @brief   Read GPIOA port interrupt flag status
 *
 * @return  GPIOA port interrupt flag status
 */
#define GPIOA_ReadITFlagPort()       (R16_PA_INT_IF)

/**
 * @brief   Read GPIOA port pin interrupt flag status
 *
 * @param   pin     - PA0-PA15
 *
 * @return  GPIOA port pin interrupt flag status
 */
#define GPIOA_ReadITFlagBit(pin)     (R16_PA_INT_IF & (pin))

/**
 * @brief   Clear GPIOA port pin interrupt flag status
 *
 * @param   pin     - PA0-PA15
 */
#define GPIOA_ClearITFlagBit(pin)    (R16_PA_INT_IF = pin)

/**
 * @brief   Peripheral function pin remapping
 *
 * @param   s       - Whether to enable remapping
 * @param   perph   - Specific remapping relationship, see GPIO_pins_remap_define
 */
void GPIOPinRemap(FunctionalState s, uint16_t perph);

/**
 * @brief   I/O pin digital function control
 *
 * @param   s       - Whether to enable the corresponding I/O pin digital function
 * @param   pin     - PA0-PA15
 */
void GPIOADigitalCfg(FunctionalState s, uint16_t pin);


#ifdef __cplusplus
}
#endif

#endif // __CH57x_GPIO_H__
