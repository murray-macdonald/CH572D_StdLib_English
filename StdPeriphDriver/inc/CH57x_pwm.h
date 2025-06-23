/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_pwm.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_PWM_H__
#define __CH57x_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  channel of PWM define
 */
#define CH_PWM1     0x01  // PWM1   channel
#define CH_PWM2     0x02  // PWM2   channel
#define CH_PWM3     0x04  // PWM3   channel
#define CH_PWM4     0x08  // PWM4   channel
#define CH_PWM5     0x10  // PWM5   channel
#define CH_PWM_ALL  0x1F  // PWM1-5 channels

/**
 * @brief  DMA channel of PWM
 */
typedef enum
{
    Mode_DMACH1_3 = 0, // DMA selects channels 1, 2, 3 output
    Mode_DMACH4_5,     // DMA selects channels 4, 5 output
    Mode_DMACH1_5,     // DMA selects channels 1, 2, 3, 4, 5 output
} PWM_DMAChannel;

/**
 * @brief  channel of PWM define
 */
typedef enum
{
    High_Level = 0, // Default low level, high level active
    Low_Level,      // Default high level, low level active
} PWMX_PolarTypeDef;

/**
 * @brief  Configuration PWM4_11 Cycle size
 */
typedef enum
{
    PWMX_Cycle_256 = 0, // 256 PWMX cycles
    PWMX_Cycle_255,     // 255 PWMX cycles
    PWMX_Cycle_128,     // 128 PWMX cycles
    PWMX_Cycle_127,     // 127 PWMX cycles
    PWMX_Cycle_64,      // 64 PWMX cycles
    PWMX_Cycle_63,      // 63 PWMX cycles
} PWMX_CycleTypeDef;

/**
 * @brief  Configuration DMA mode
 */
typedef enum
{
    PWM_ModeSINGLE = 0, // Single mode
    PWM_ModeLOOP,       // Loop mode
} PWM_DMAModeTypeDef;

/**
 * @brief   PWM channel base clock configuration
 *
 * @param   d   - Channel base clock = d*Tsys
 */
#define PWMX_CLKCfg(d)    (R16_PWM_CLOCK_DIV = d)

/**
 * @brief   PWM 8-bit cycle configuration
 *
 * @param   cyc - refer to PWMX_CycleTypeDef
 */
void PWMX_CycleCfg(PWMX_CycleTypeDef cyc);

/**
 * @brief   PWM 16-bit cycle configuration
 *
 * @param   ch  - select channel of pwm, refer to channel of PWM define
 *          cyc - 16-bit cycle
 */
void PWMX_16bit_CycleCfg(uint8_t ch, uint16_t cyc);

/**
 * @brief   PWM 16-bit data width enable
 */
#define PWM_16bit_CycleEnable()  (R8_PWM_CONFIG |= (3 << 1))

/**
 * @brief   PWM 16-bit data width disable
 */
#define PWM_16bit_CycleDisable()  (R8_PWM_CONFIG &= ~(3 << 1))

/**
 * @brief   Set PWM1 8-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM1_ActDataWidth(d)     (R8_PWM1_DATA = d)

/**
 * @brief   Set PWM2 8-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM2_ActDataWidth(d)     (R8_PWM2_DATA = d)

/**
 * @brief   Set PWM3 8-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM3_ActDataWidth(d)     (R8_PWM3_DATA = d)

/**
 * @brief   Set PWM4 8-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM4_ActDataWidth(d)     (R8_PWM4_DATA = d)

/**
 * @brief   Set PWM5 8-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM5_ActDataWidth(d)     (R8_PWM5_DATA = d)

/**
 * @brief   Set PWM1 16-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM1_16bit_ActDataWidth(d)     (R16_PWM1_DATA = d)

/**
 * @brief   Set PWM2 16-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM2_16bit_ActDataWidth(d)     (R16_PWM2_DATA = d)

/**
 * @brief   Set PWM3 16-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM3_16bit_ActDataWidth(d)     (R16_PWM3_DATA = d)

/**
 * @brief   Set PWM4 16-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM4_16bit_ActDataWidth(d)     (R16_PWM4_DATA = d)

/**
 * @brief   Set PWM5 16-bit effective data pulse width
 *
 * @param   d   - Effective data pulse width
 */
#define PWM5_16bit_ActDataWidth(d)     (R16_PWM5_DATA = d)

/**
 * @brief   PWM 8-bit output waveform configuration
 *
 * @param   ch      - select channel of pwm, refer to channel of PWM define
 * @param   da      - effective pulse width
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   s       - control pwmx function, ENABLE or DISABLE
 */
void PWMX_ACTOUT(uint8_t ch, uint8_t da, PWMX_PolarTypeDef pr, FunctionalState s);

/**
 * @brief   PWM 16-bit output waveform configuration
 *
 * @param   ch      - select channel of pwm, refer to channel of PWM define
 * @param   da      - effective pulse width
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   s       - control pwmx function, ENABLE or DISABLE
 */
void PWMX_16bit_ACTOUT(uint8_t ch, uint16_t da, PWMX_PolarTypeDef pr, FunctionalState s);

/**
 * @brief   PWM alternate output mode configuration
 *
 * @param   ch      - select group of PWM alternate output
 *                    RB_PWM4_5_STAG_EN     -  PWM4 and PWM5 channel alternate output
 * @param   s       - control pwmx function, ENABLE or DISABLE
 */
void PWMX_AlterOutCfg(uint8_t ch, FunctionalState s);

/**
 * @brief   PWM synchronous output mode configuration
 *
 * @param   s       - control pwmx function, ENABLE or DISABLE
 */
void PWMX_SyncOutCfg(FunctionalState s);

/**
 * @brief   Configure PWM DMA function
 *
 * @param   s           - Whether to enable DMA function
 * @param   startAddr   - DMA start address
 * @param   endAddr     - DMA end address
 * @param   m           - Configure DMA mode
 */
void PWM_DMACfg(uint8_t s, uint32_t startAddr, uint32_t endAddr, PWM_DMAModeTypeDef m, PWM_DMAChannel ch);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_PWM_H__
