#include "iuart.hpp"
#include "dma_channel.hpp"

typedef void (*callback_function)(void);

class UartDma
{
    public:
    using CallbackFunction = std::function<void()>;
    enum return_t {SUCCESS = 0, BUSY};
    UartDma(IUart& uart, CallbackFunction tx_callback_function = nullptr);
    return_t transmit(const char message[], uint32_t length, bool wait_until_free = false);
    void handleTransmissionComplete(void);


    private:
    IUart& uart_;
    DmaChannel tx_dma_channel_;
    CallbackFunction tx_callback_function_;
    bool transfer_running_;
};