/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_keyscan.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2024/12/17
 * Description        : source file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

/*********************************************************************
 * @fn      KeyScan_Cfg
 *
 * @brief   Configure key scanning function
 *
 * @param   s            -  Set whether to enable key scanning function
 * @param   keyScanPin   -  Set the IO pins enabled for key scanning
 * @param   ClkDiv       -  Set scan clock division, clock source is LSI
 * @param   Rep          -  Set the number of times to scan the same key value
 *
 * @return  none
 */
void KeyScan_Cfg(uint8_t s, uint16_t keyScanPin, uint16_t ClkDiv, uint16_t Rep)
{
    if(s == DISABLE)
    {
        R16_KEY_SCAN_CTRL &= ~(RB_SCAN_START_EN);
    }
    else
    {
        R16_KEY_SCAN_CTRL |= keyScanPin | ClkDiv | Rep;
        R16_KEY_SCAN_CTRL |= RB_SCAN_START_EN;
    }
}

/*********************************************************************
 * @fn      KeyPress_Wake
 *
 * @brief   Enable key press wake up from sleep
 *
 * @param   s            -  Set whether to enable key wake-up function
 *
 * @return  none
 */
void KeyPress_Wake(uint8_t s)
{
    if(s == DISABLE)
    {
        sys_safe_access_enable();
        R8_SLP_CLK_OFF0 &= ~(RB_SLP_KEYSCAN_WAKE);
        sys_safe_access_disable();
    }
    else
    {
        sys_safe_access_enable();
        R8_SLP_CLK_OFF0 |= RB_SLP_KEYSCAN_WAKE;
        sys_safe_access_disable();
    }
}
