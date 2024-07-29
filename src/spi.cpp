#include "spi.hpp"

void Spi::init(void)
{
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    //LL_SPI_Init(SPI1, &SPI_InitStruct);
    LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_DisableNSSPulseMgt(SPI1);
}