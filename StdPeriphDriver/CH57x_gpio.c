/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_gpio.c
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
 * @fn      GPIOA_ModeCfg
 *
 * @brief   GPIOA port pin mode configuration
 *
 * @param   pin     - PA0-PA15
 * @param   mode    - Input/output type
 *
 * @return  none
 */
void GPIOA_ModeCfg(uint32_t pin, GPIOModeTypeDef mode)
{
    switch(mode)
    {
        case GPIO_ModeIN_Floating:
            R32_PA_PD_DRV &= ~pin;
            R32_PA_PU &= ~pin;
            R32_PA_DIR &= ~pin;
            break;

        case GPIO_ModeIN_PU:
            R32_PA_PD_DRV &= ~pin;
            R32_PA_PU |= pin;
            R32_PA_DIR &= ~pin;
            break;

        case GPIO_ModeIN_PD:
            R32_PA_PD_DRV |= pin;
            R32_PA_PU &= ~pin;
            R32_PA_DIR &= ~pin;
            break;

        case GPIO_ModeOut_PP_5mA:
            R32_PA_PD_DRV &= ~pin;
            R32_PA_DIR |= pin;
            break;

        case GPIO_ModeOut_PP_20mA:
            R32_PA_PD_DRV |= pin;
            R32_PA_DIR |= pin;
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      GPIOA_ITModeCfg
 *
 * @brief   GPIOA pin interrupt mode configuration
 *
 * @param   pin     - PA0-PA15
 * @param   mode    - Trigger type
 *
 * @return  none
 */
void GPIOA_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode)
{
    switch(mode)
    {
        case GPIO_ITMode_LowLevel: // Low level trigger
            R16_PA_INT_MODE &= ~pin;
            R32_PA_CLR |= pin;
            break;

        case GPIO_ITMode_HighLevel: // High level trigger
            R16_PA_INT_MODE &= ~pin;
            R32_PA_OUT |= pin;
            break;

        case GPIO_ITMode_FallEdge: // Falling edge trigger
            R16_PA_INT_MODE |= pin;
            R32_PA_CLR |= pin;
            break;

        case GPIO_ITMode_RiseEdge: // Rising edge trigger
            R16_PA_INT_MODE |= pin;
            R32_PA_OUT |= pin;
            break;

        default:
            break;
    }
    R16_PA_INT_IF = pin;
    R16_PA_INT_EN |= pin;
}

/*********************************************************************
 * @fn      GPIOPinRemap
 *
 * @brief   Peripheral function pin mapping
 *
 * @param   s       - Whether to enable mapping
 * @param   perph   - The specific mapping relationship, see GPIO_pins_remap_define for details
 *
 *
 * @return  none
 */
void GPIOPinRemap(FunctionalState s, uint16_t perph)
{
    if(s)
    {
        R16_PIN_ALTERNATE_H |= perph;
    }
    else
    {
        R16_PIN_ALTERNATE_H &= ~perph;
    }
}

/*********************************************************************
 * @fn      GPIOADigitalCfg
 *
 * @brief   I/O pin digital function control
 *
 * @param   s       - Whether to enable the corresponding I/O pin digital function
 * @param   pin     - PA0-PA11
 */
void GPIOADigitalCfg(FunctionalState s, uint16_t pin)
{
    if(s)
    {
        R16_PIN_ALTERNATE &= ~pin;
    }
    else
    {
        R16_PIN_ALTERNATE |= pin;
    }
}

