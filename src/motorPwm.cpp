#include "motorPwm.hpp"


#define MOTOR_TIMER_CHANNEL_U LL_TIM_CHANNEL_CH1
#define MOTOR_TIMER_CHANNEL_V LL_TIM_CHANNEL_CH2
#define MOTOR_TIMER_CHANNEL_W LL_TIM_CHANNEL_CH3



motorPwm::motorPwm(TIM_TypeDef *TIMx, uint32_t prescaler) :
Timer(TIMx)
{
    /* Reset value is LL_TIM_COUNTERMODE_UP */
    LL_TIM_SetCounterMode(timerInst, LL_TIM_COUNTERMODE_CENTER_UP_DOWN);

    /* Set the pre-scaler value to have TIM2 counter clock equal to 10 kHz */
    LL_TIM_SetPrescaler(timerInst, 0);//__LL_TIM_CALC_PSC(SystemCoreClock, 10000));

    /* Enable TIM2_ARR register preload. Writing to or reading from the         */
    /* auto-reload register accesses the preload register. The content of the   */
    /* preload register are transferred into the shadow register at each update */
    /* event (UEV).                                                             */
    LL_TIM_EnableARRPreload(timerInst);

    /* Set the auto-reload value to have a counter frequency of 24 kHz */
    /* TIM2CLK = SystemCoreClock / (APB prescaler & multiplier)               */
    uint32_t TimOutClock = SystemCoreClock / 1;
    LL_TIM_SetAutoReload(timerInst, __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(timerInst), 24000));

    /* Set output mode */
    /* Reset value is LL_TIM_OCMODE_FROZEN */
    LL_TIM_OC_SetMode(timerInst, MOTOR_TIMER_CHANNEL_U, LL_TIM_OCMODE_PWM1);

    /* Set output channel polarity */
    /* Reset value is LL_TIM_OCPOLARITY_HIGH */
    // LL_TIM_OC_SetPolarity(TIM2, MOTOR_TIMER_CHANNEL_U, LL_TIM_OCPOLARITY_HIGH);

    /* Set compare value to half of the counter period (50% duty cycle ) */
    LL_TIM_OC_SetCompareCH1(timerInst, ((LL_TIM_GetAutoReload(timerInst) + 1) / 2));

    /* Enable TIM2_CCR1 register preload. Read/Write operations access the      */
    /* preload register. TIM2_CCR1 preload value is loaded in the active        */
    /* at each update event.                                                    */
    LL_TIM_OC_EnablePreload(timerInst, MOTOR_TIMER_CHANNEL_U);

    /* Enable output channel 1 */
    LL_TIM_CC_EnableChannel(timerInst, MOTOR_TIMER_CHANNEL_U);

    /* Enable counter */
    LL_TIM_EnableCounter(timerInst);

    /* Force update generation */
    LL_TIM_GenerateEvent_UPDATE(timerInst);
}

void motorPwm::enableOutputs(void)
{
    LL_TIM_OC_SetMode(timerInst, MOTOR_TIMER_CHANNEL_U, LL_TIM_OCMODE_ACTIVE);
    LL_TIM_OC_SetMode(timerInst, MOTOR_TIMER_CHANNEL_V, LL_TIM_OCMODE_ACTIVE);
    LL_TIM_OC_SetMode(timerInst, MOTOR_TIMER_CHANNEL_W, LL_TIM_OCMODE_ACTIVE);
}

void motorPwm::disableOutputs(void)
{
    LL_TIM_OC_SetMode(timerInst, MOTOR_TIMER_CHANNEL_U, LL_TIM_OCMODE_FORCED_INACTIVE);
    LL_TIM_OC_SetMode(timerInst, MOTOR_TIMER_CHANNEL_V, LL_TIM_OCMODE_FORCED_INACTIVE);
    LL_TIM_OC_SetMode(timerInst, MOTOR_TIMER_CHANNEL_W, LL_TIM_OCMODE_FORCED_INACTIVE);
}

void motorPwm::setDutyCycle(std::array<uint32_t, 3> dutyCycle)
{
}
