/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_pwr.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_PWR_H__
#define __CH57x_PWR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_CFG_ADR_HW       0x7F00C            // config address for hardware config for LDO&OSC and etc

/**
 * @brief  wakeup mode define, select wakeup delay
 */
typedef enum
{
    Fsys_Delay_3584 = 0,
    Fsys_Delay_512,
    Fsys_Delay_64,
    Fsys_Delay_1,
    Fsys_Delay_8191,
    Fsys_Delay_7168,
    Fsys_Delay_6144,
    Fsys_Delay_4096,

} WakeUP_ModeypeDef;

/**
 * @brief  wakeup mode define
 */
typedef enum
{
    /* The following levels will use low power monitoring, 1uA consumption */
    LPLevel_1V8 = 0,
    LPLevel_2V0,
    LPLevel_2V2,
    LPLevel_2V4,

} VolM_LevelypeDef;

/**
 * @brief   Peripheral clock control bit
 *
 * @param   s       - Whether to enable the corresponding peripheral clock
 * @param   perph   - please refer to Peripher CLK control bit define
 */
void PWR_PeriphClkCfg(FunctionalState s, uint16_t perph);

/**
 * @brief   Sleep wakeup source configuration
 *
 * @param   s       - Whether to enable this peripheral sleep wakeup function
 * @param   perph   - Wakeup source to be set
 *                    RB_SLP_USB_WAKE   -  USB as wakeup source
 *                    RB_SLP_RTC_WAKE   -  RTC as wakeup source
 *                    RB_SLP_GPIO_WAKE  -  GPIO as wakeup source
 *                    RB_SLP_BAT_WAKE   -  BAT as wakeup source
 * @param   mode    - refer to WakeUP_ModeypeDef
 */
void PWR_PeriphWakeUpCfg(FunctionalState s, uint8_t perph, WakeUP_ModeypeDef mode);

/**
 * @brief   Power monitoring
 *
 * @param   s       - Whether to enable this function
 * @param   vl      - refer to VolM_LevelypeDef
 */
void PowerMonitor(FunctionalState s, VolM_LevelypeDef vl);

/**
 * @brief   Low power - Idle mode
 */
void LowPower_Idle(void);

/**
 * @brief   Low power - Halt mode, this low power mode switches to HSI/5 clock operation, after wakeup the user needs to re-select the system clock source
 */
void LowPower_Halt(void);

/**
 * @brief   Low power - Sleep mode, this low power mode switches to HSI/5 clock operation, after wakeup the user needs to re-select the system clock source
 *          @note Note that when calling this function, DCDC function is forcibly turned off, it can be manually turned on again after wakeup
 *
 * @param   rm      - Power supply module selection
 *                    RB_PWR_RAM2K  -   2K retention SRAM power supply
 *                    RB_PWR_RAM16K -   16K main SRAM power supply
 *                    RB_PWR_EXTEND -   USB and BLE unit retention area power supply
 *                    RB_PWR_XROM   -   FlashROM power supply
 *                    NULL          -   All above units are powered off
 */
void LowPower_Sleep(uint16_t rm);

/**
 * @brief   Low power - Shutdown mode, this low power mode switches to HSI/5 clock operation, after wakeup the user needs to re-select the system clock source
 *          @note Note that when calling this function, DCDC function is forcibly turned off, it can be manually turned on again after wakeup
 *
 * @param   rm      - Power supply module selection
 *                    RB_PWR_RAM2K  -   2K retention SRAM power supply
 *                    RB_PWR_RAM16K -   16K main SRAM power supply
 *                    NULL          -   All above units are powered off
 */
void LowPower_Shutdown(uint16_t rm);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_PWR_H__
