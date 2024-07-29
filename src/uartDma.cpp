#include "uartDma.hpp"

#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_lpuart.h"

UartDma::UartDma(IUart& uart, CallbackFunction tx_callback_function_arg) : uart_(uart), tx_dma_channel_(DmaChannel::DmaInst1), tx_callback_function_(tx_callback_function_arg), transfer_running_(false)
{
    tx_dma_channel_.configTransfer(LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_LOW | LL_DMA_MODE_NORMAL |
                                LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
                                LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE);
    //tx_dma_channel_.configInterrupts(DmaChannel::InterruptDisabled, DmaChannel::InterruptEnabled, DmaChannel::InterruptDisabled);
    tx_dma_channel_.setPeriphRequest(LL_DMAMUX_REQ_LPUART1_TX);
    LL_LPUART_Enable(LPUART1);
}

UartDma::return_t UartDma::transmit(const char message[], uint32_t length, bool wait_until_free)
{
    return_t return_val = SUCCESS;
    // if there is a transfer in progress and we shall not wait, we transmit nothing and return an error code
    if (transfer_running_ == true && wait_until_free == false)
    {
        return_val = BUSY;
    }
    else
    {
        // wait for transfer to finish
        while (transfer_running_)
        {
        }
        transfer_running_ = true;
        tx_dma_channel_.configAddresses(message, reinterpret_cast<void *>(LL_LPUART_DMA_GetRegAddr(LPUART1, LL_LPUART_DIRECTION_TX)));
        tx_dma_channel_.setDataLength(length);
        uart_.enableDmaReqRx();
        tx_dma_channel_.enable();
    }
    return return_val;
}

void UartDma::handleTransmissionComplete(void)
{
    if(tx_callback_function_)
    {
        tx_callback_function_();
    }
}
