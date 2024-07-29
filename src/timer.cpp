#include "timer.hpp"

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_rcc.h"

Timer::Timer(TIM_TypeDef* TIMx)
{
    timerInst = TIMx;
    if(timerInst == TIM1)
        LL_APB1_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
    else if(timerInst == TIM2)
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    else if(timerInst == TIM3)
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
    else if(timerInst == TIM4)
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
    else if(timerInst == TIM5)
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);
    else if(timerInst == TIM6)
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
    else if(timerInst == TIM7)
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
    else if(timerInst == TIM8)
        LL_APB1_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM8);
    else if(timerInst == TIM15)
        LL_APB1_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15);
    else if(timerInst == TIM16)
        LL_APB1_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
    else if(timerInst == TIM17)
        LL_APB1_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
}

void Timer::enable(void) const
{
    LL_TIM_EnableCounter(timerInst);
}

void Timer::disable(void) const
{
    LL_TIM_DisableCounter(timerInst);
}

uint32_t Timer::getVal(void) const
{
    return LL_TIM_GetCounter(timerInst);
}

void Timer::setVal(uint32_t val) const
{
    return LL_TIM_SetCounter(timerInst, val);
}
