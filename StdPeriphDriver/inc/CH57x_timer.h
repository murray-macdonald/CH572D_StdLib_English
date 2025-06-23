/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_timer.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_TIMER_H__
#define __CH57x_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DataBit_25            (1 << 25)

/**
 * @brief  TMR interrupt bit define
 */
#define TMR_IT_CYC_END     0x01  // Cycle end flag: Capture-timeout, Timer-cycle end, PWM-cycle end
#define TMR_IT_DATA_ACT    0x02  // Data valid flag: Capture-new data, PWM-effective level end
#define TMR_IT_FIFO_HF     0x04  // FIFO half-full: Capture- FIFO>=4, PWM- FIFO<4
#define TMR_IT_DMA_END     0x08  // DMA end, supported by TMR-TMR3
#define TMR_IT_FIFO_OV     0x10  // FIFO overflow: Capture- FIFO full, PWM- FIFO empty

/**
 * @brief  ENC interrupt bit define
 */
#define RB_IE_DIR_INC      0x01  // Forward interrupt enable
#define RB_IE_DIR_DEC      0x02  // Backward interrupt enable

/**
 * @brief  Configuration PWM effective level repeat times
 */
typedef enum
{
    PWM_Times_1 = 0, // PWM effective output repeated 1 time
    PWM_Times_4,     // PWM effective output repeated 4 times
    PWM_Times_8,     // PWM effective output repeated 8 times
    PWM_Times_16,    // PWM effective output repeated 16 times
} PWM_RepeatTsTypeDef;

/**
 * @brief  Configuration Cap mode
 */
typedef enum
{
    CAP_NULL = 0,         // No capture & No count
    Edge_To_Edge,         // Between any edges & Count any edges
    FallEdge_To_FallEdge, // Falling edge to falling edge & Count falling edges
    RiseEdge_To_RiseEdge, // Rising edge to rising edge & Count rising edges
} CapModeTypeDef;

/**
 * @brief  Configuration DMA mode
 */
typedef enum
{
    Mode_Single = 0, // Single mode
    Mode_LOOP,       // Loop mode
} DMAModeTypeDef;

/**
 * @brief  Configuration ENC mode
 */
typedef enum
{
    Mode_IDLE = 0,   // IDLE mode
    Mode_T2 ,        // T2 edge counting mode
    Mode_T1 ,        // T1 edge counting mode
    Mode_T1T2 ,      // T1 and T2 edge counting mode
} ENCModeTypeDef;

/**
 * @brief   Timer function initialization
 *
 * @param   t       - Timing duration, based on current system clock Tsys, maximum timing period 67108864
 */
void TMR_TimerInit(uint32_t t);

/**
 * @brief   Edge counting function initialization
 *
 * @param   cap     - Capture count type
 */
void TMR_EXTSingleCounterInit(CapModeTypeDef cap);

/**
 * @brief   Set count statistics overflow size, maximum 67108863
 *
 * @param   cyc     - Count statistics overflow size
 */
#define TMR_CountOverflowCfg(cyc)    (R32_TMR_CNT_END = (cyc + 2))

/**
 * @brief   Get current count value, maximum 67108863
 *
 * @return  Current count value
 */
#define TMR_GetCurrentCount()        R32_TMR_COUNT

/**
 * @brief   PWM0 channel output waveform period configuration, maximum 67108863
 *
 * @param   cyc     - Output waveform period
 */
#define TMR_PWMCycleCfg(cyc)         (R32_TMR_CNT_END = cyc)

/**
 * @brief   PWM output initialization
 *
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   ts      - set pwm repeat times, refer to PWM_RepeatTsTypeDef
 */
void TMR_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

/**
 * @brief   PWM0 effective data pulse width, maximum 67108864
 *
 * @param   d       - Effective data pulse width
 */
#define TMR_PWMActDataWidth(d)    (R32_TMR_FIFO = d)

/**
 * @brief   CAP0 capture level timeout configuration, maximum 33554432
 *
 * @param   cyc     - Capture level timeout
 */
#define TMR_CAPTimeoutCfg(cyc)    (R32_TMR_CNT_END = cyc)

/**
 * @brief   External signal capture function initialization
 *
 * @param   cap     - select capture mode, refer to CapModeTypeDef
 */
void TMR_CapInit(CapModeTypeDef cap);

/**
 * @brief   Get pulse data
 *
 * @return  Pulse data
 */
#define TMR_CAPGetData()        R32_TMR_FIFO

/**
 * @brief   Get current number of captured data
 *
 * @return  Current number of captured data
 */
#define TMR_CAPDataCounter()    R8_TMR_FIFO_COUNT

/**
 * @brief   Configure DMA function
 *
 * @param   s           - Whether to enable DMA function
 * @param   startAddr   - DMA start address
 * @param   endAddr     - DMA end address
 * @param   m           - Configure DMA mode
 */
void TMR_DMACfg(uint8_t s, uint32_t startAddr, uint32_t endAddr, DMAModeTypeDef m);

/**
 * @brief   Configure ENC function
 *
 * @param   s           - Whether to enable encoder function
 * @param   encReg      - Encoder mode end value (maximum 0xFFFF)
 * @param   m           - Configure ENC mode
 *
 * @return  none
 */
void ENC_Config(uint8_t s, uint32_t encReg, ENCModeTypeDef m);

/**
 * @brief   Disable TMR PWM output
 */
#define TMR_PWMDisable()           (R8_TMR_CTRL_MOD &= ~RB_TMR_OUT_EN)

/**
 * @brief   Enable TMR PWM output
 */
#define TMR_PWMEnable()           (R8_TMR_CTRL_MOD |= RB_TMR_OUT_EN)

/**
 * @brief   Disable TMR
 */
#define TMR_Disable()           (R8_TMR_CTRL_MOD &= ~RB_TMR_COUNT_EN)

/**
 * @brief   Enable TMR
 */
#define TMR_Enable()            (R8_TMR_CTRL_MOD |= RB_TMR_COUNT_EN)

/**
 * @brief   TMR interrupt configuration
 *
 * @param   s       - Enable/Disable
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR_ITCfg(s, f)         ((s) ? (R8_TMR_INTER_EN |= f) : (R8_TMR_INTER_EN &= ~f))

/**
 * @brief   Clear TMR interrupt flag
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR_ClearITFlag(f)      (R8_TMR_INT_FLAG = f)

/**
 * @brief   Query interrupt flag status
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR_GetITFlag(f)        (R8_TMR_INT_FLAG & f)

/**
 * @brief   Get encoder current direction
 *
 * @return  Direction value  0:Forward  1:Backward
 */
#define ENC_GetCurrentDir       (R8_ENC_REG_CTRL>>5 & 0x01)

/**
 * @brief   Get encoder current count value
 */
#define ENC_GetCurrentCount      R32_ENC_REG_CCNT

/**
 * @brief   Read encoder count and clear to 0
 */
#define ENC_GetCountandReset()  R8_ENC_REG_CTRL |= RB_RD_CLR_EN

/**
 * @brief   ENC interrupt configuration
 *
 * @param   s       - Enable/Disable
 * @param   f       - refer to ENC interrupt bit define
 */
#define ENC_ITCfg(s, f)         ((s) ? (R8_ENC_INTER_EN |= f) : (R8_ENC_INTER_EN &= ~f))

/**
 * @brief   Clear ENC interrupt flag
 *
 * @param   f       - refer to ENC interrupt bit define
 */
#define ENC_ClearITFlag(f)      (R8_ENC_INT_FLAG = f)

/**
 * @brief   Query interrupt flag status
 *
 * @param   f       - refer to ENC interrupt bit define
 */
#define ENC_GetITFlag(f)        (R8_ENC_INT_FLAG & f)

#ifdef __cplusplus
}
#endif

#endif // __CH57x_TIMER_H__
