#pragma once

#include "stm32g4xx_ll_spi.h"

class Spi
{
    public:
    Spi(SPI_TypeDef spiInst);
    void init(void);
    void transmitBlocking();

    private:
    SPI_TypeDef spiInst;
};