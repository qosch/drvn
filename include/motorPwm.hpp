#pragma once

#include "timer.hpp"
#include <array>

class motorPwm: public Timer
{
    motorPwm(TIM_TypeDef *TIMx, uint32_t prescaler);
    void enableOutputs(void);
    void disableOutputs(void);
    void setDutyCycle(std::array<uint32_t, 3> dutyCycle);
};