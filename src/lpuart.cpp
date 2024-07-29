#include "lpuart.hpp"

#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_lpuart.h"

LpUart::LpUart(USART_TypeDef * lpUartInst, const uint32_t clock, const uint32_t speed) :
myInstance(lpUartInst)
{
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);

    LL_LPUART_Disable(myInstance);
    LL_LPUART_SetTransferDirection(myInstance, LL_LPUART_DIRECTION_TX_RX);
    LL_LPUART_ConfigCharacter(myInstance, LL_LPUART_DATAWIDTH_8B, LL_LPUART_PARITY_NONE, LL_LPUART_STOPBITS_1);
    LL_LPUART_SetPrescaler(myInstance, LL_LPUART_PRESCALER_DIV1);
    LL_LPUART_SetBaudRate(myInstance, clock, LL_LPUART_PRESCALER_DIV1, speed);
    LL_LPUART_Enable(myInstance);
}

void LpUart::enable(void)
{
    LL_LPUART_Enable(myInstance);
}

void LpUart::disable(void)
{
    LL_LPUART_Disable(myInstance);
}

void LpUart::transmit(const void* data, size_t sizeBytes, bool waitUntilFree) const
{
    const uint8_t* bytePointer = reinterpret_cast<const uint8_t*>(data);
    /* Send characters one per one, until last char to be sent */
    for (size_t i = 0; i < sizeBytes; i++)
    {
        /* Wait for TXE flag to be raised */
        while (!LL_LPUART_IsActiveFlag_TXE(myInstance))
        {
        }

        /* Write character in Transmit Data register.
           TXE flag is cleared by writing data in TDR register */
        //LL_LPUART_TransmitData8(myInstance, bytePointer[i]);
    }

    /* Wait for TC flag to be raised for last char */
    while (!LL_LPUART_IsActiveFlag_TC(myInstance))
    {
    }
}

void LpUart::transmit(const char message[], size_t length, bool waitUntilFree) const
{
    transmit(reinterpret_cast<const void*>(message), length, waitUntilFree);
}

void LpUart::transmit(const std::string message, bool waitUntilFree) const
{
    transmit(message.c_str(), message.length(), waitUntilFree);
}

volatile uint32_t *LpUart::getRxRegister(void) const
{
    return reinterpret_cast<uint32_t*>(LL_LPUART_DMA_GetRegAddr(myInstance, LL_LPUART_DMA_REG_DATA_RECEIVE));
}

volatile uint32_t *LpUart::getTxRegister(void) const
{
    return reinterpret_cast<uint32_t*>(LL_LPUART_DMA_GetRegAddr(myInstance, LL_LPUART_DMA_REG_DATA_TRANSMIT));
}

void LpUart::enableDmaReqRx(void)
{
    LL_LPUART_EnableDMAReq_RX(myInstance);
}

void LpUart::enableDmaReqTx(void)
{
    LL_LPUART_EnableDMAReq_TX(myInstance);
}
