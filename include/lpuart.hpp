#pragma once

#include "iuart.hpp"
#include "stm32g4xx_ll_lpuart.h"

class LpUart: IUart
{
public:
    LpUart(USART_TypeDef* lpUartInst, const uint32_t clock, const uint32_t speed);
    void enable(void);
    void disable(void);
    void transmit(const void* data, size_t sizeBytes=1, bool waitUntilFree=false) const;
    void transmit(const char message[], size_t length=1, bool waitUntilFree=false) const;
    void transmit(const std::string message, bool waitUntilFree=false) const;
    volatile uint32_t* getRxRegister(void) const;
    volatile uint32_t* getTxRegister(void) const;
    void enableDmaReqRx(void);
    void enableDmaReqTx(void);

    private:
    USART_TypeDef* myInstance;
};
