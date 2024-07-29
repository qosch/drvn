#pragma once

#include "stm32g4xx_ll_tim.h"

class Timer
{
    public:
    Timer(TIM_TypeDef* TIMx);
    void enable(void) const;
    void disable(void) const;
    uint32_t getVal(void) const;
    void setVal(uint32_t val) const;

protected:
    TIM_TypeDef* timerInst;
};
