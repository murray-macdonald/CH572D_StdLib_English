/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_SYS.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_SYS_H__
#define __CH57x_SYS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  rtc interrupt event define
 */
typedef enum
{
    RST_STATUS_SW = 0, // Software reset
    RST_STATUS_RPOR,   // Power-on reset
    RST_STATUS_WTR,    // Watchdog timeout reset
    RST_STATUS_MR,     // External manual reset
    RST_STATUS_LRM0,   // Wake-up reset - caused by software reset
    RST_STATUS_GPWSM,  // Power-down mode wake-up reset
    RST_STATUS_LRM1,   // Wake-up reset - caused by watchdog
    RST_STATUS_LRM2,   // Wake-up reset - caused by manual reset

} SYS_ResetStaTypeDef;

/**
 * @brief  rtc interrupt event define
 */
typedef enum
{
    INFO_RESET_EN = 0x4,   // Whether RST# external manual reset input function is enabled
    INFO_BOOT_EN = 0x8,    // Whether system bootloader is enabled
    INFO_RST_PIN = 0x10,   // Reset pin selection: 1: PA7; 0: PA8
    INFO_LOADER  = 0x20,   // Whether the current system is in Bootloader area
    STA_SAFEACC_ACT = 0x30,// Whether the current system is in a safe access state, otherwise RWA attribute areas are not accessible

} SYS_InfoStaTypeDef;

/**
 * @brief  Get chip ID, generally a fixed value
 */
#define SYS_GetChipID()      R8_CHIP_ID

/**
 * @brief  Get security access ID, generally a fixed value
 */
#define SYS_GetAccessID()    R8_SAFE_ACCESS_ID

/**
 * @brief   Configure system running clock
 *
 * @param   sc      - System clock source selection, refer to SYS_CLKTypeDef
 */
void SetSysClock(SYS_CLKTypeDef sc);

/**
 * @brief   Get current system clock
 *
 * @return  Hz
 */
uint32_t GetSysClock(void);

/**
 * @brief   Get current system information status
 *
 * @param   i       - refer to SYS_InfoStaTypeDef
 *
 * @return  Whether enabled
 */
uint8_t SYS_GetInfoSta(SYS_InfoStaTypeDef i);

/**
 * @brief   Get system last reset status
 *
 * @return  refer to SYS_ResetStaTypeDef
 */
#define SYS_GetLastResetSta()    (R8_RESET_STATUS & RB_RESET_FLAG)

/**
 * @brief   Execute system software reset
 */
void SYS_ResetExecute(void);

/**
 * @brief   Set the value of reset save register, not affected by manual reset, software reset, watchdog reset, or normal wake-up reset
 *
 * @param   i       - refer to SYS_InfoStaTypeDef
 */
#define SYS_ResetKeepBuf(d)    (R8_GLOB_RESET_KEEP = d)

/**
 * @brief   Disable all interrupts and preserve current interrupt value
 *
 * @param   pirqv   - Current preserved interrupt value
 */
void SYS_DisableAllIrq(uint32_t *pirqv);

/**
 * @brief   Restore previously disabled interrupt value
 *
 * @param   irq_status  - Current preserved interrupt value
 */
void SYS_RecoverIrq(uint32_t irq_status);

/**
 * @brief   Get current system (SYSTICK) counter value
 *
 * @return  Current counter value
 */
uint32_t SYS_GetSysTickCnt(void);

/**
 * @brief   Load watchdog counter initial value, incremental type
 *
 * @param   c       - Watchdog counter initial value
 */
#define WWDG_SetCounter(c)    (R8_WDOG_COUNT = c)

/**
 * @brief   Watchdog timer overflow interrupt enable
 *
 * @param   s       - Whether to interrupt on overflow
 */
void WWDG_ITCfg(FunctionalState s);

/**
 * @brief   Watchdog timer reset function
 *
 * @param   s       - Whether to reset on overflow
 */
void WWDG_ResetCfg(FunctionalState s);

/**
 * @brief   Get current watchdog timer overflow flag
 *
 * @return  Watchdog timer overflow flag
 */
#define WWDG_GetFlowFlag()    (R8_RST_WDOG_CTRL & RB_WDOG_INT_FLAG)

/**
 * @brief   Clear watchdog interrupt flag, reloading counter value can also clear it
 */
void WWDG_ClearFlag(void);

/**
 * @brief   uS delay
 *
 * @param   t       - Time parameter
 */
void mDelayuS(uint16_t t);

/**
 * @brief   mS delay
 *
 * @param   t       - Time parameter
 */
void mDelaymS(uint16_t t);

/**
 * @brief Enter safe access mode.
 * 
 * @NOTE: After enter safe access mode, about 16 system frequency cycles 
 * are in safe mode, and one or more secure registers can be rewritten 
 * within the valid period. The safe mode will be automatically 
 * terminated after the above validity period is exceeded.
 *  if sys_safe_access_enable() is called,
 *  you must call sys_safe_access_disable() before call sys_safe_access_enable() again.
 */
#define sys_safe_access_enable()        do{volatile uint32_t mpie_mie;mpie_mie=__risc_v_disable_irq();SAFEOPERATE;\
                                        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;SAFEOPERATE;

#define sys_safe_access_disable()       R8_SAFE_ACCESS_SIG = 0;__risc_v_enable_irq(mpie_mie);SAFEOPERATE;}while(0)

#ifdef __cplusplus
}
#endif

#endif // __CH57x_SYS_H__
