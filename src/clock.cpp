#include <clock.hpp>

#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_pwr.h"

void Clock::configure(void) const
{
    /* Flash Latency configuration */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

    /* Enable boost mode to be able to reach 170MHz */
    LL_PWR_EnableRange1BoostMode();

    /* HSE configuration and activation */
    LL_RCC_HSE_Enable();
    while (LL_RCC_HSE_IsReady() != 1)
    {
    };

    /* Main PLL configuration and activation */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, pllM, pllN, pllR);
    LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, pllM, pllN, pllQ);
    LL_RCC_PLL_ConfigDomain_ADC(LL_RCC_PLLSOURCE_HSE, pllM, pllN, pllP);



    LL_RCC_PLL_Enable();
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_EnableDomain_48M();
    while (LL_RCC_PLL_IsReady() != 1)
    {
    };

    /* Sysclk activation on the main PLL */
    LL_RCC_SetAHBPrescaler(ahb);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
    };

    /* Insure 1µs transition state at intermediate medium speed clock based on DWT */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    while (DWT->CYCCNT < 100)
        ;
    /* Set APB1 & APB2 prescaler*/
    LL_RCC_SetAPB1Prescaler(apb1);
    LL_RCC_SetAPB2Prescaler(apb2);

    /* Set systick */
    LL_Init1msTick(__LL_RCC_CALC_PLLCLK_FREQ(24000000, pllM, pllN, pllR));

    /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
    LL_SetSystemCoreClock(__LL_RCC_CALC_PLLCLK_FREQ(24000000, pllM, pllN, pllR));
}

uint32_t Clock::getHSE(void) const
{
    return hseFreq;
}

uint32_t Clock::getPLLCLK(void) const
{
    return __LL_RCC_CALC_PLLCLK_FREQ(hseFreq, pllM, pllN, pllR);
}

uint32_t Clock::getHCLK(void) const
{
    return __LL_RCC_CALC_HCLK_FREQ(getPLLCLK(), ahb);
}

uint32_t Clock::getPCLK1(void) const
{
    return __LL_RCC_CALC_PCLK1_FREQ(getHCLK(), apb1);
}

uint32_t Clock::getPCLK2(void) const
{
    return __LL_RCC_CALC_PCLK2_FREQ(getHCLK(), apb2);
}
