/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_timer.c
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
 * @fn      TMR_TimerInit
 *
 * @brief   Timer function initialization
 *
 * @param   t       - Timer duration, based on current system clock Tsys, maximum timing period 67108864
 *
 * @return  none
 */
void TMR_TimerInit(uint32_t t)
{
    R32_TMR_CNT_END = t;
    R8_TMR_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR_CTRL_MOD = RB_TMR_COUNT_EN;
}

/*********************************************************************
 * @fn      TMR_EXTSingleCounterInit
 *
 * @brief   Edge counting function initialization
 *
 * @param   cap     - Capture count type
 *
 * @return  none
 */
void TMR_EXTSingleCounterInit(CapModeTypeDef cap)
{
    R8_TMR_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_CAP_COUNT | RB_TMR_MODE_IN | (cap << 6);
}

/*********************************************************************
 * @fn      TMR_PWMInit
 *
 * @brief   PWM output initialization
 *
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   ts      - set pwm repeat times, refer to PWM_RepeatTsTypeDef
 *
 * @return  none
 */
void TMR_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts)
{
    R8_TMR_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR_CTRL_MOD = (pr << 4) | (ts << 6);
}

/*********************************************************************
 * @fn      TMR_CapInit
 *
 * @brief   External signal capture function initialization
 *
 * @param   cap     - select capture mode, refer to CapModeTypeDef
 *
 * @return  none
 */
void TMR_CapInit(CapModeTypeDef cap)
{
    R8_TMR_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_MODE_IN | (cap << 6);
}


/*********************************************************************
 * @fn      TMR_DMACfg
 *
 * @brief   Configure DMA function
 *
 * @param   s           - Enable or disable DMA function
 * @param   startAddr   - DMA start address
 * @param   endAddr     - DMA end address
 * @param   m           - Configure DMA mode
 *
 * @return  none
 */
void TMR_DMACfg(uint8_t s, uint32_t startAddr, uint32_t endAddr, DMAModeTypeDef m)
{
    if(s == DISABLE)
    {
        R8_TMR_CTRL_DMA = 0;
    }
    else
    {
        R16_TMR_DMA_BEG = startAddr & 0xFFFF;
        R16_TMR_DMA_END = endAddr & 0xFFFF;
        if(m)
            R8_TMR_CTRL_DMA = RB_TMR_DMA_LOOP | RB_TMR_DMA_ENABLE;
        else
            R8_TMR_CTRL_DMA = RB_TMR_DMA_ENABLE;
    }
}


/*********************************************************************
 * @fn      ENC_Config
 *
 * @brief   Configure encoder function
 *
 * @param   s           - Enable or disable encoder function
 * @param   encReg      - Encoder mode end value (maximum value 0xFFFF)
 * @param   m           - Configure ENC mode
 *
 * @return  none
 */
void ENC_Config(uint8_t s, uint32_t encReg, ENCModeTypeDef m)
{
    if(s == DISABLE)
    {
        R8_ENC_REG_CTRL &= ~(RB_START_ENC_EN);
    }
    else
    {
        R8_ENC_REG_CTRL |= (m << 1) | RB_START_ENC_EN;
        R32_ENC_REG_CEND |= encReg;
    }
}


