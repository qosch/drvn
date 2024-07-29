#pragma once

#include <array>
#include <functional>

#include "stm32g4xx_ll_dma.h"

// STM32G47x has 2 DMA instances with 8 channels each
constexpr uint8_t DMA_INSTANCE_COUNT = 2;
constexpr uint8_t DMA_CHANNEL_COUNT  = 8;

class DmaChannel {
    public:
    enum InterruptEnable_t {InterruptDisabled = 0, InterruptEnabled = 1};
    enum DmaInst_t {DmaInst1 = 1, DmaInst2 = 2};
    using DmaCallback = std::function<void()>;
    // TODO: find a way to make this private or get rid of it
    static std::array<std::array<DmaChannel*, DMA_CHANNEL_COUNT+1>, DMA_INSTANCE_COUNT+1> classInstancePointers;
    DmaChannel(void);
    DmaChannel(DmaInst_t  dmaInst);
    void enable(void);
    void disable(void);
    DmaInst_t getDmaInstWithLeastChannelsUsed(void);
    void configTransfer(uint32_t config);
    uint8_t getDmaInst(void) const;
    uint8_t getChannelNumber(void) const;
    void configAddresses(const void* src, volatile void* dst);
    void setDataLength(uint32_t length);
    void configInterrupts(InterruptEnable_t halfTransferInt, InterruptEnable_t transferCompleteInt, InterruptEnable_t transferErrorInt);
    void setPeriphRequest(uint32_t periphRequest);
    void registerCallbackFunctionHT(DmaCallback callbackFunction);
    void registerCallbackFunctionTC(DmaCallback callbackFunction);
    void registerCallbackFunctionTE(DmaCallback callbackFunction);
    void setIrqPriority(uint32_t priority);
    uint32_t getRemainingTransfers(void);
    bool getFlagHT(void) const;
    bool getFlagTC(void) const;
    bool getFlagTE(void) const;
    void clearFlagHT(void);
    void clearFlagTC(void);
    void clearFlagTE(void);
    void handleIRQ(void);

private:
    const DmaInst_t dmaInst;
    const DMA_TypeDef* dmaInstLL;
    const uint8_t channel;
    const uint32_t channelLL;
    DmaCallback callbackFunctionHT;
    DmaCallback callbackFunctionTC;
    DmaCallback callbackFunctionTE;
    static std::array<uint8_t, DMA_INSTANCE_COUNT+1> channelCounter;
};