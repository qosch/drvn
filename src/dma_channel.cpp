#include "dma_channel.hpp"
#include <assert.h>
#include "stm32g4xx_ll_bus.h"

// DMA instance and channel numbering starts from 1, element 0 is never used
constexpr std::array<DMA_TypeDef *, DMA_INSTANCE_COUNT + 1> dmaInstances = {0, DMA1, DMA2};
constexpr std::array<uint32_t, DMA_CHANNEL_COUNT + 1> dmaChannels = {0, LL_DMA_CHANNEL_1, LL_DMA_CHANNEL_2, LL_DMA_CHANNEL_3, LL_DMA_CHANNEL_4, LL_DMA_CHANNEL_5, LL_DMA_CHANNEL_6, LL_DMA_CHANNEL_7, LL_DMA_CHANNEL_8};
constexpr std::array<std::array<IRQn_Type, DMA_CHANNEL_COUNT + 1>, DMA_INSTANCE_COUNT + 1> dmaIRQs = {
    HardFault_IRQn, HardFault_IRQn    , HardFault_IRQn    , HardFault_IRQn    , HardFault_IRQn    , HardFault_IRQn    , HardFault_IRQn    , HardFault_IRQn    , HardFault_IRQn,
    HardFault_IRQn, DMA1_Channel1_IRQn, DMA1_Channel2_IRQn, DMA1_Channel3_IRQn, DMA1_Channel4_IRQn, DMA1_Channel5_IRQn, DMA1_Channel6_IRQn, DMA1_Channel7_IRQn, DMA1_Channel8_IRQn,
    HardFault_IRQn, DMA2_Channel1_IRQn, DMA2_Channel2_IRQn, DMA2_Channel3_IRQn, DMA2_Channel4_IRQn, DMA2_Channel5_IRQn, DMA2_Channel6_IRQn, DMA2_Channel7_IRQn, DMA2_Channel8_IRQn};

std::array<uint8_t, DMA_INSTANCE_COUNT + 1> DmaChannel::channelCounter = {0, 1, 1};
std::array<std::array<DmaChannel *, DMA_CHANNEL_COUNT + 1>, DMA_INSTANCE_COUNT + 1> DmaChannel::classInstancePointers;

DmaChannel::DmaChannel(void) : DmaChannel(getDmaInstWithLeastChannelsUsed())
{
}

DmaChannel::DmaChannel(DmaInst_t dmaInstArg) : dmaInst(dmaInstArg), dmaInstLL(dmaInstances[dmaInstArg]), channel(channelCounter[dmaInstArg]++), channelLL(dmaChannels[channel])
{
    // check if there is a channel left on this DMA instance
    assert(channel <= DMA_CHANNEL_COUNT);
    classInstancePointers[dmaInst][channel] = this;
}

void DmaChannel::enable(void)
{
    LL_DMA_EnableChannel(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL);
}

void DmaChannel::disable(void)
{
    LL_DMA_DisableChannel(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL);
}

DmaChannel::DmaInst_t DmaChannel::getDmaInstWithLeastChannelsUsed(void)
{
    DmaInst_t returnVal;
    if (channelCounter[DmaInst1] > channelCounter[DmaInst2])
        returnVal = DmaInst2;
    else
        returnVal = DmaInst1;
    return returnVal;
}

uint8_t DmaChannel::getDmaInst(void) const
{
    return dmaInst;
}

uint8_t DmaChannel::getChannelNumber(void) const
{
    return channel;
}

void DmaChannel::configTransfer(uint32_t config)
{
    // enable the relevant clocks
    if (dmaInst == DmaInst1)
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    else if (dmaInst == DmaInst2)
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
    // configAdresses() has to be called again if the transfer direction is changed!
    LL_DMA_ConfigTransfer(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL, config);
    LL_DMA_DisableIT_HT(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL);
    LL_DMA_DisableIT_TC(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL);
    LL_DMA_DisableIT_TE(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL);
}

void DmaChannel::configAddresses(const void* src, volatile void* dst)
{
    LL_DMA_ConfigAddresses(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL, reinterpret_cast<uintptr_t>(src), reinterpret_cast<uintptr_t>(dst), LL_DMA_GetDataTransferDirection(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL));
}

void DmaChannel::setDataLength(uint32_t length)
{
    LL_DMA_SetDataLength(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL, length);
}

void DmaChannel::setPeriphRequest(uint32_t periphRequest)
{
    LL_DMA_SetPeriphRequest(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL, periphRequest);
}

void DmaChannel::registerCallbackFunctionHT(DmaCallback callbackFunction)
{
    LL_DMA_EnableIT_HT(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL);
    callbackFunctionHT = callbackFunction;
    NVIC_EnableIRQ(dmaIRQs[dmaInst][channel]);
}

void DmaChannel::registerCallbackFunctionTC(DmaCallback callbackFunction)
{
    LL_DMA_EnableIT_TC(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL);
    callbackFunctionTC = callbackFunction;
    NVIC_EnableIRQ(dmaIRQs[dmaInst][channel]);
}

void DmaChannel::registerCallbackFunctionTE(DmaCallback callbackFunction)
{
    LL_DMA_EnableIT_TE(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL);
    callbackFunctionTE = callbackFunction;
    NVIC_EnableIRQ(dmaIRQs[dmaInst][channel]);
}

void DmaChannel::setIrqPriority(uint32_t priority)
{
    NVIC_SetPriority(dmaIRQs[dmaInst][channel], priority);
    NVIC_EnableIRQ(dmaIRQs[dmaInst][channel]);
}

uint32_t DmaChannel::getRemainingTransfers(void)
{
    return LL_DMA_GetDataLength(const_cast<DMA_TypeDef *>(dmaInstLL), channelLL);
}

bool DmaChannel::getFlagHT(void) const
{
    switch (channel)
    {
    case 1:
        return LL_DMA_IsActiveFlag_HT1(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 2:
        return LL_DMA_IsActiveFlag_HT2(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 3:
        return LL_DMA_IsActiveFlag_HT3(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 4:
        return LL_DMA_IsActiveFlag_HT4(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 5:
        return LL_DMA_IsActiveFlag_HT5(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 6:
        return LL_DMA_IsActiveFlag_HT6(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 7:
        return LL_DMA_IsActiveFlag_HT7(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 8:
        return LL_DMA_IsActiveFlag_HT8(const_cast<DMA_TypeDef *>(dmaInstLL));
    default:
        assert(false);
    }
}

bool DmaChannel::getFlagTC(void) const
{
    switch (channel)
    {
    case 1:
        return LL_DMA_IsActiveFlag_TC1(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 2:
        return LL_DMA_IsActiveFlag_TC2(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 3:
        return LL_DMA_IsActiveFlag_TC3(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 4:
        return LL_DMA_IsActiveFlag_TC4(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 5:
        return LL_DMA_IsActiveFlag_TC5(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 6:
        return LL_DMA_IsActiveFlag_TC6(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 7:
        return LL_DMA_IsActiveFlag_TC7(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 8:
        return LL_DMA_IsActiveFlag_TC8(const_cast<DMA_TypeDef *>(dmaInstLL));
    default:
        assert(false);
    }
}

bool DmaChannel::getFlagTE(void) const
{
    switch (channel)
    {
    case 1:
        return LL_DMA_IsActiveFlag_TE1(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 2:
        return LL_DMA_IsActiveFlag_TE2(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 3:
        return LL_DMA_IsActiveFlag_TE3(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 4:
        return LL_DMA_IsActiveFlag_TE4(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 5:
        return LL_DMA_IsActiveFlag_TE5(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 6:
        return LL_DMA_IsActiveFlag_TE6(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 7:
        return LL_DMA_IsActiveFlag_TE7(const_cast<DMA_TypeDef *>(dmaInstLL));
    case 8:
        return LL_DMA_IsActiveFlag_TE8(const_cast<DMA_TypeDef *>(dmaInstLL));
    default:
        assert(false);
    }
}

void DmaChannel::clearFlagHT(void)
{
    switch (channel)
    {
    case 1:
        LL_DMA_ClearFlag_HT1(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 2:
        LL_DMA_ClearFlag_HT2(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 3:
        LL_DMA_ClearFlag_HT3(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 4:
        LL_DMA_ClearFlag_HT4(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 5:
        LL_DMA_ClearFlag_HT5(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 6:
        LL_DMA_ClearFlag_HT6(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 7:
        LL_DMA_ClearFlag_HT7(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 8:
        LL_DMA_ClearFlag_HT8(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    default:
        assert(false);
    }
}

void DmaChannel::clearFlagTC(void)
{
    switch (channel)
    {
    case 1:
        LL_DMA_ClearFlag_TC1(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 2:
        LL_DMA_ClearFlag_TC2(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 3:
        LL_DMA_ClearFlag_TC3(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 4:
        LL_DMA_ClearFlag_TC4(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 5:
        LL_DMA_ClearFlag_TC5(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 6:
        LL_DMA_ClearFlag_TC6(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 7:
        LL_DMA_ClearFlag_TC7(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 8:
        LL_DMA_ClearFlag_TC8(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    default:
        assert(false);
    }
}

void DmaChannel::clearFlagTE(void)
{
    switch (channel)
    {
    case 1:
        LL_DMA_ClearFlag_TE1(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 2:
        LL_DMA_ClearFlag_TE2(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 3:
        LL_DMA_ClearFlag_TE3(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 4:
        LL_DMA_ClearFlag_TE4(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 5:
        LL_DMA_ClearFlag_TE5(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 6:
        LL_DMA_ClearFlag_TE6(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 7:
        LL_DMA_ClearFlag_TE7(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    case 8:
        LL_DMA_ClearFlag_TE8(const_cast<DMA_TypeDef *>(dmaInstLL));
        break;
    default:
        assert(false);
    }
}

void DmaChannel::handleIRQ(void)
{
    //LL_DMA_ClearFlag_GI1(const_cast<DMA_TypeDef *>(dmaInstLL));
    if (getFlagHT())
    {
        callbackFunctionHT();
        clearFlagHT();
    }
    if (getFlagTC())
    {
        callbackFunctionTC();
        clearFlagTC();
    }
    if (getFlagTE())
    {
        callbackFunctionTE();
        clearFlagTE();
    }
}

// ----------  IRQ handlers:  ----------

extern "C" void DMA1_Channel1_IRQHandler(void)
{
    DmaChannel::classInstancePointers[1][1]->handleIRQ();
}

extern "C" void DMA1_Channel2_IRQHandler(void)
{
    DmaChannel::classInstancePointers[1][2]->handleIRQ();
}

extern "C" void DMA1_Channel3_IRQHandler(void)
{
    DmaChannel::classInstancePointers[1][3]->handleIRQ();
}

extern "C" void DMA1_Channel4_IRQHandler(void)
{
    DmaChannel::classInstancePointers[1][4]->handleIRQ();
}

extern "C" void DMA1_Channel5_IRQHandler(void)
{
    DmaChannel::classInstancePointers[1][5]->handleIRQ();
}

extern "C" void DMA1_Channel6_IRQHandler(void)
{
    DmaChannel::classInstancePointers[1][6]->handleIRQ();
}

extern "C" void DMA1_Channel7_IRQHandler(void)
{
    DmaChannel::classInstancePointers[1][7]->handleIRQ();
}

extern "C" void DMA1_Channel8_IRQHandler(void)
{
    DmaChannel::classInstancePointers[1][8]->handleIRQ();
}

extern "C" void DMA2_Channel1_IRQHandler(void)
{
    DmaChannel::classInstancePointers[2][1]->handleIRQ();
}

extern "C" void DMA2_Channel2_IRQHandler(void)
{
    DmaChannel::classInstancePointers[2][2]->handleIRQ();
}

extern "C" void DMA2_Channel3_IRQHandler(void)
{
    DmaChannel::classInstancePointers[2][3]->handleIRQ();
}

extern "C" void DMA2_Channel4_IRQHandler(void)
{
    DmaChannel::classInstancePointers[2][4]->handleIRQ();
}

extern "C" void DMA2_Channel5_IRQHandler(void)
{
    DmaChannel::classInstancePointers[2][5]->handleIRQ();
}

extern "C" void DMA2_Channel6_IRQHandler(void)
{
    DmaChannel::classInstancePointers[2][6]->handleIRQ();
}

extern "C" void DMA2_Channel7_IRQHandler(void)
{
    DmaChannel::classInstancePointers[2][7]->handleIRQ();
}

extern "C" void DMA2_Channel8_IRQHandler(void)
{
    DmaChannel::classInstancePointers[2][8]->handleIRQ();
}
