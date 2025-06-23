/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_pwr.c
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
 * @fn      PWR_PeriphClkCfg
 *
 * @brief   Peripheral clock control bit
 *
 * @param   s       - Whether to enable the corresponding peripheral clock
 * @param   perph   - please refer to Peripher CLK control bit define
 *
 * @return  none
 */
void PWR_PeriphClkCfg(FunctionalState s, uint16_t perph)
{
    uint32_t sleep_ctrl = R32_SLEEP_CONTROL;

    if(s == DISABLE)
    {
        sleep_ctrl |= perph;
    }
    else
    {
        sleep_ctrl &= ~perph;
    }

    sys_safe_access_enable();
    R32_SLEEP_CONTROL = sleep_ctrl;
    sys_safe_access_disable();
}

/*********************************************************************
 * @fn      PWR_PeriphWakeUpCfg
 *
 * @brief   Sleep wake-up source configuration
 *
 * @param   s       - Whether to enable the wake-up function for this peripheral
 * @param   perph   - Wake-up source to be configured
 *                    RB_SLP_USB_WAKE   -  USBFS as wake-up source
 *                    RB_SLP_USB2_WAKE  -  USBHS as wake-up source
 *                    RB_SLP_RTC_WAKE   -  RTC as wake-up source
 *                    RB_SLP_GPIO_WAKE  -  GPIO as wake-up source
 *                    RB_SLP_BAT_WAKE   -  BAT as wake-up source
 *                    RB_SLP_GPIO_EDGE_MODE -  GPIO can wake up on both rising and falling edges
 * @param   mode    - refer to WakeUP_ModeypeDef
 *
 * @return  none
 */
void PWR_PeriphWakeUpCfg(FunctionalState s, uint8_t perph, WakeUP_ModeypeDef mode)
{
    if(s == DISABLE)
    {
        sys_safe_access_enable();
        R8_SLP_WAKE_CTRL &= ~perph;
        sys_safe_access_disable();
    }
    else
    {
        sys_safe_access_enable();
        R8_SLP_WAKE_CTRL |= RB_WAKE_EV_MODE | perph;
        sys_safe_access_disable();
        sys_safe_access_enable();
        R8_SLP_POWER_CTRL &= ~(RB_WAKE_DLY_MOD);
        sys_safe_access_disable();
        sys_safe_access_enable();
        R8_SLP_POWER_CTRL |= mode;
        sys_safe_access_disable();
    }
}

/*********************************************************************
 * @fn      PowerMonitor
 *
 * @brief   Power monitoring
 *
 * @param   s       - Whether to enable this function
 * @param   vl      - refer to VolM_LevelypeDef
 *
 * @return  none
 */
void PowerMonitor(FunctionalState s, VolM_LevelypeDef vl)
{
    uint8_t ctrl = R8_BAT_DET_CTRL;
    uint8_t cfg = R8_BAT_DET_CFG;

    if(s == DISABLE)
    {
        sys_safe_access_enable();
        R8_BAT_DET_CTRL = 0;
        sys_safe_access_disable();
    }
    else
    {
        cfg = vl & 0x03;
        ctrl = RB_BAT_MON_EN;
        sys_safe_access_enable();
        R8_BAT_DET_CTRL = ctrl;
        R8_BAT_DET_CFG = cfg;
        sys_safe_access_disable();

        mDelayuS(1);
        sys_safe_access_enable();
        R8_BAT_DET_CTRL |= RB_BAT_LOW_IE;
        sys_safe_access_disable();
    }
}

/*********************************************************************
 * @fn      LowPower_Idle
 *
 * @brief   Low power - Idle mode
 *
 * @param   none
 *
 * @return  none
 */
__HIGH_CODE
void LowPower_Idle(void)
{
    FLASH_ROM_SW_RESET();
    R8_FLASH_CTRL = 0x04; //flash closed

    PFIC->SCTLR &= ~(1 << 2); // sleep
    __WFI();
    __nop();
    __nop();
}

/*********************************************************************
 * @fn      LowPower_Halt
 *
 * @brief   Low power - Halt mode, this low power mode switches to HSI/5 clock, after waking up, the user needs to reselect the system clock source
 *
 * @param   none
 *
 * @return  none
 */
__HIGH_CODE
void LowPower_Halt(void)
{
    uint8_t x32Mpw;

    FLASH_ROM_SW_RESET();
    R8_FLASH_CTRL = 0x04; //flash closed
    x32Mpw = R8_XT32M_TUNE;
    if(!(R8_HFCK_PWR_CTRL&RB_CLK_XT32M_KEEP))
    {
        x32Mpw = (x32Mpw & 0xfc) | 0x03; // 150% rated current
    }

    sys_safe_access_enable();
    R8_BAT_DET_CTRL = 0; // Turn off voltage monitoring
    sys_safe_access_disable();
    sys_safe_access_enable();
    R8_XT32M_TUNE = x32Mpw;
    sys_safe_access_disable();
    sys_safe_access_enable();
    R8_PLL_CONFIG |= (1 << 5);
    sys_safe_access_disable();

    PFIC->SCTLR |= (1 << 2); //deep sleep
    __WFI();
    __nop();
    __nop();
    sys_safe_access_enable();
    R8_PLL_CONFIG &= ~(1 << 5);
    sys_safe_access_disable();
}

/*******************************************************************************
* Function Name  : LowPower_Sleep
* Description    : Low power - Sleep mode.
* Input          : rm:
                    RB_PWR_RAM12K	-	Power supply to 12K retention SRAM
                    RB_PWR_EXTEND	-	Power supply to USB and BLE unit reserved areas
                    RB_PWR_XROM   - Power supply to FlashROM
                   NULL	-	All of the above units power off
* Return         : None
*******************************************************************************/
__HIGH_CODE
void LowPower_Sleep(uint16_t rm)
{
    uint8_t x32Mpw;
    uint16_t power_plan;
    uint8_t clk_sys_cfg;
    uint16_t hfck_pwr_ctrl;

    clk_sys_cfg = R8_CLK_SYS_CFG;
    hfck_pwr_ctrl = R8_HFCK_PWR_CTRL;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw & 0xfc) | 0x03; // 150% rated current

    sys_safe_access_enable();
    R8_BAT_DET_CTRL = 0; // Turn off voltage monitoring
    sys_safe_access_disable();
    sys_safe_access_enable();
    R8_XT32M_TUNE = x32Mpw;
    sys_safe_access_disable();

    PFIC->SCTLR |= (1 << 2); //deep sleep

    power_plan |= RB_PWR_PLAN_EN | RB_PWR_CORE | rm | (2<<11);

    sys_safe_access_enable();

    // Number of test cycles needed, currently using 3584
    R8_SLP_POWER_CTRL |= 0x40;

    R16_POWER_PLAN = power_plan;
    sys_safe_access_disable();
    sys_safe_access_enable();
    R8_CLK_SYS_CFG = CLK_SOURCE_HSE_PLL_24MHz;
    sys_safe_access_disable();
//    if((R16_CLK_SYS_CFG & RB_CLK_SYS_MOD) == 0x40)
//    {
//        sys_safe_access_enable();
//        R16_CLK_SYS_CFG = (R16_CLK_SYS_CFG&(~RB_CLK_PLL_DIV))|24;
//        sys_safe_access_disable();
//    }
//    sys_safe_access_enable();
//    R8_PLL_CONFIG |= (1 << 5);
//    sys_safe_access_disable();

    __WFI();
    __nop();
    __nop();

    sys_safe_access_enable();
    R8_CLK_SYS_CFG = clk_sys_cfg;
//    R8_HFCK_PWR_CTRL = hfck_pwr_ctrl;
    sys_safe_access_disable();
    sys_safe_access_enable();
    R16_POWER_PLAN &= ~RB_PWR_PLAN_EN;
    R16_POWER_PLAN &= ~RB_XT_PRE_EN;
    sys_safe_access_disable();
//    sys_safe_access_enable();
//    R8_PLL_CONFIG &= ~(1 << 5);
//    sys_safe_access_disable();
    DelayUs(40);
}

/*********************************************************************
 * @fn      LowPower_Shutdown
 *
 * @brief   Low power - Shutdown mode, this low power mode switches to HSI/5 clock, after waking up, the user needs to reselect the system clock source
 *          @note When calling this function, DCDC function is forcibly turned off, and can be manually turned on again after wake-up
 *
 * @param   rm      - Power module selection
 *                    RB_PWR_RAM12K   -   Power supply to 12K retention SRAM
 *                    RB_PWR_EXTEND   -   Power supply to USB and BLE unit reserved areas
 *                    NULL          -   All of the above units power off
 *
 * @return  none
 */
__HIGH_CODE
void LowPower_Shutdown(uint16_t rm)
{
    uint8_t x32Mpw;

    FLASH_ROM_SW_RESET();
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw & 0xfc) | 0x03; // 150% rated current

    sys_safe_access_enable();
    R8_BAT_DET_CTRL = 0; // Turn off voltage monitoring
    sys_safe_access_disable();
    sys_safe_access_enable();
    R8_XT32M_TUNE = x32Mpw;
    sys_safe_access_disable();

    PFIC->SCTLR |= (1 << 2); //deep sleep

    SetSysClock(CLK_SOURCE_HSE_PLL_24MHz);
    sys_safe_access_enable();
    R8_SLP_POWER_CTRL |= 0x40;
    sys_safe_access_disable();
    sys_safe_access_enable();
    R16_POWER_PLAN = RB_PWR_PLAN_EN | rm;
    sys_safe_access_disable();
    __WFI();
    __nop();
    __nop();
    FLASH_ROM_SW_RESET();
    sys_safe_access_enable();
    R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
    sys_safe_access_disable();
}
