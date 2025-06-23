/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_clk.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_CLK_H__
#define __CH57x_CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  System main frequency definition
 */
typedef enum
{
    CLK_SOURCE_LSI = 0xC0,

    CLK_SOURCE_HSE_16MHz = (0x02),
    CLK_SOURCE_HSE_8MHz = (0x04),
    CLK_SOURCE_HSE_6_4MHz = (0x05),
    CLK_SOURCE_HSE_4MHz = (0x08),
    CLK_SOURCE_HSE_2MHz = (0x10),
    CLK_SOURCE_HSE_1MHz = (0x0),

    CLK_SOURCE_HSE_PLL_100MHz = (0x40 | 6),
    CLK_SOURCE_HSE_PLL_75MHz = (0x40 | 8),
    CLK_SOURCE_HSE_PLL_60MHz = (0x40 | 10),
    CLK_SOURCE_HSE_PLL_50MHz = (0x40 | 12),
    CLK_SOURCE_HSE_PLL_40MHz = (0x40 | 15),
    CLK_SOURCE_HSE_PLL_30MHz = (0x40 | 20),
    CLK_SOURCE_HSE_PLL_25MHz = (0x40 | 24),
    CLK_SOURCE_HSE_PLL_24MHz = (0x40 | 25),
    CLK_SOURCE_HSE_PLL_20MHz = (0x40 | 30),

} SYS_CLKTypeDef;

/**
 * @brief  32MHz crystal oscillator current gear positions
 */
typedef enum
{
    HSE_RCur_75 = 0,
    HSE_RCur_100,
    HSE_RCur_125,
    HSE_RCur_150

} HSECurrentTypeDef;

/**
 * @brief  32MHz crystal oscillator internal capacitance gear positions
 */
typedef enum
{
    HSECap_6p = 0,
    HSECap_8p,
    HSECap_10p,
    HSECap_12p,
    HSECap_14p,
    HSECap_16p,
    HSECap_18p,
    HSECap_20p

} HSECapTypeDef;

#define RTC_MAX_COUNT             0xA8C00000

#define MAX_DAY                   0x00004000
#define MAX_2_SEC                 0x0000A8C0
//#define	 MAX_SEC		0x545FFFFF

#define BEGYEAR                   2020
#define IsLeapYear(yr)            (!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define YearLength(yr)            (IsLeapYear(yr) ? 366 : 365)
#define monthLength(lpyr, mon)    (((mon) == 1) ? (28 + (lpyr)) : (((mon) > 6) ? (((mon) & 1) ? 31 : 30) : (((mon) & 1) ? 30 : 31)))

/**
 * @brief  the total number of cycles captured by the oscillator define
 */
typedef enum
{
    Count_1 = 0,
    Count_2,
    Count_4,
    Count_32,
    Count_64,
    Count_128,
    Count_1024,
    Count_2047,
} RTC_OSCCntTypeDef;

/**
 * @brief  rtc timer mode period define
 */
typedef enum
{
    Period_4096 = 0,
    Period_8192,
    Period_16384,
    Period_32768,
    Period_65536,
    Period_131072,
    Period_262144,
    Period_524288,
} RTC_TMRCycTypeDef;

/**
 * @brief  rtc interrupt event define
 */
typedef enum
{
    RTC_TRIG_EVENT = 0, // RTC trigger event
    RTC_TMR_EVENT,      // RTC periodic timing event

} RTC_EVENTTypeDef;

/**
 * @brief  rtc interrupt mode define
 */
typedef enum
{
    RTC_TRIG_MODE = 0, // RTC trigger mode
    RTC_TMR_MODE,      // RTC periodic timing mode

} RTC_MODETypeDef;

/**
 * @brief   HSE crystal bias current configuration
 *
 * @param   c   - 75%,100%,125%,150%
 */
void HSECFG_Current(HSECurrentTypeDef c);

/**
 * @brief   HSE crystal load capacitance configuration
 *
 * @param   c   - refer to HSECapTypeDef
 */
void HSECFG_Capacitance(HSECapTypeDef c);

/**
 * @brief   Initialize RTC clock, higher capture cycle count means longer initialization time but higher clock precision
 *
 * @param   cnt     - the total number of cycles captured by the oscillator
 *
 * @return  RTC clock, 24~42KHz
 */
uint32_t RTC_InitClock(RTC_OSCCntTypeDef cnt);

/**
 * @brief   Initialize RTC clock with current time
 *
 * @param   y       - configure year, MAX_Y = BEGYEAR + 44
 * @param   mon     - configure month, MAX_MON = 12
 * @param   d       - configure day, MAX_D = 31
 * @param   h       - configure hour, MAX_H = 23
 * @param   m       - configure minute, MAX_M = 59
 * @param   s       - configure second, MAX_S = 59
 */
void RTC_InitTime(uint16_t y, uint16_t mon, uint16_t d, uint16_t h, uint16_t m, uint16_t s);

/**
 * @brief   Get current time
 *
 * @param   py      - Retrieved year, MAX_Y = BEGYEAR + 44
 * @param   pmon    - Retrieved month, MAX_MON = 12
 * @param   pd      - Retrieved day, MAX_D = 31
 * @param   ph      - Retrieved hour, MAX_H = 23
 * @param   pm      - Retrieved minute, MAX_M = 59
 * @param   ps      - Retrieved second, MAX_S = 59
 */
void RTC_GetTime(uint16_t *py, uint16_t *pmon, uint16_t *pd, uint16_t *ph, uint16_t *pm, uint16_t *ps);

/**
 * @brief   Configure current RTC cycle count based on LSI clock
 *
 * @param   cyc     - Configure initial cycle count, MAX_CYC = 0xA8BFFFFF = 2831155199
 */
void RTC_SetCycleLSI(uint32_t cyc);

/**
 * @brief   Get current RTC cycle count based on LSI clock
 *
 * @return  Current cycle count, MAX_CYC = 0xA8BFFFFF = 2831155199
 */
uint32_t RTC_GetCycleLSI(void);

/**
 * @brief   RTC timer mode configuration (note that the timer reference is fixed at 32768Hz)
 *
 * @param   t   - refer to RTC_TMRCycTypeDef
 */
void RTC_TRIGFunCfg(uint32_t cyc);

/**
 * @brief   RTC timer mode configuration (note that the timer reference is fixed at 32768Hz)
 *
 * @param   t   - refer to RTC_TMRCycTypeDef
 */
void RTC_TMRFunCfg(RTC_TMRCycTypeDef t);

/**
 * @brief   Disable RTC mode function
 *
 * @param   m   - Current mode that needs to be closed
 */
void RTC_ModeFunDisable(RTC_MODETypeDef m);

/**
 * @brief   Get RTC interrupt flag
 *
 * @param   f   - refer to RTC_EVENTTypeDef
 *
 * @return  Interrupt flag status
 */
uint8_t RTC_GetITFlag(RTC_EVENTTypeDef f);

/**
 * @brief   Clear RTC interrupt flag
 *
 * @param   f   - refer to RTC_EVENTTypeDef
 */
void RTC_ClearITFlag(RTC_EVENTTypeDef f);

/**
 * @brief   Low frequency clock power configuration
 */
void LClk_Cfg(FunctionalState s);


#ifdef __cplusplus
}
#endif

#endif // __CH57x_CLK_H__
