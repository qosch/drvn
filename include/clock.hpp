#pragma once

#include "stm32g4xx_ll_rcc.h"

class Clock
{
    public:
    static Clock& instance(void)
    {
        static Clock _instance;
        _instance.configure();
        return _instance;
    }
    
    uint32_t getHSE(void) const;
    uint32_t getPLLCLK(void) const;
    uint32_t getHCLK(void) const;
    uint32_t getPCLK1(void) const;
    uint32_t getPCLK2(void) const;
    private:
    Clock() { }
    Clock(const Clock&);
    Clock & operator= (const Clock&);
    void configure(void) const;

    uint32_t hseFreq = 24000000;
    uint32_t pllM = LL_RCC_PLLM_DIV_2;
    uint32_t pllN = 28;
    uint32_t pllP = LL_RCC_PLLP_DIV_2;
    uint32_t pllQ = LL_RCC_PLLQ_DIV_2;
    uint32_t pllR = LL_RCC_PLLR_DIV_2;
    uint32_t ahb = LL_RCC_SYSCLK_DIV_1;
    uint32_t apb1 = LL_RCC_APB1_DIV_1;
    uint32_t apb2 = LL_RCC_APB2_DIV_1;
};