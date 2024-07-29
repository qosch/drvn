#include <stdint.h>
#include <string>

class IUart
{
public:
    //virtual IUart(USART_TypeDef* uartInst, const uint32_t clock, const uint32_t speed) = 0;
    virtual ~IUart() = default;
    virtual void enable(void) = 0;
    virtual void disable(void) = 0;
    virtual void transmit(const void* data, size_t sizeBytes=1, bool waitUntilFree=false) const = 0;
    virtual void transmit(const char message[], size_t length=1, bool waitUntilFree=false) const = 0;
    virtual void transmit(const std::string message, bool waitUntilFree=false) const = 0;
    virtual volatile uint32_t* getRxRegister(void) const = 0;
    virtual volatile uint32_t* getTxRegister(void) const = 0;
    virtual void enableDmaReqRx(void) = 0;
    virtual void enableDmaReqTx(void) = 0;
};