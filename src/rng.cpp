#include "rng.hpp"

#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_rng.h"

void RandomNumberGenerator::init(void)
{
    LL_RCC_SetRNGClockSource(LL_RCC_RNG_CLKSOURCE_PLL);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RNG);
    LL_RNG_Enable(RNG);
}

uint32_t RandomNumberGenerator::get(void)
{
    return LL_RNG_ReadRandData32(RNG);

}