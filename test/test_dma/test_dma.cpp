#include "unity.h"
#include "stm32g4xx_ll_utils.h"
#include "dma_channel.hpp"

#include <random>

#define TEST_DATA_SIZE 3000

struct dmaTestData_t
{
    uint32_t dataBefore;
    uint32_t data[TEST_DATA_SIZE];
    uint32_t dataAfter;
};

enum class InterruptSequence_t
{
    NO_INTERRUPT,
    HT,
    TC,
    ERROR
};

InterruptSequence_t InterruptSequence1, InterruptSequence2 = InterruptSequence_t::NO_INTERRUPT;

void setUp(void) {}

void tearDown(void) {}

void fillWithRandom(dmaTestData_t arg)
{
    arg.dataBefore = std::rand();
    for (uint32_t i = 0; i < TEST_DATA_SIZE; i++)
    {
        arg.data[i] = std::rand();
    }
    arg.dataAfter = std::rand();
}

void htCallback1(void)
{
    TEST_ASSERT(InterruptSequence1 == InterruptSequence_t::NO_INTERRUPT);
    InterruptSequence1 = InterruptSequence_t::HT;
}

void tcCallback1(void)
{
    TEST_ASSERT(InterruptSequence1 == InterruptSequence_t::HT);
    InterruptSequence1 = InterruptSequence_t::TC;
}

void htCallback2(void)
{
    TEST_ASSERT(InterruptSequence2 == InterruptSequence_t::NO_INTERRUPT);
    InterruptSequence2 = InterruptSequence_t::HT;
}

void tcCallback2(void)
{
    TEST_ASSERT(InterruptSequence2 == InterruptSequence_t::HT);
    InterruptSequence2 = InterruptSequence_t::TC;
}

void erCallback1(void)
{
    TEST_FAIL_MESSAGE("transfer error interrupt occurred");
}

void erCallback2(void)
{
    TEST_FAIL_MESSAGE("transfer error interrupt occurred");
}

void testDma(void)
{
    // generate random data for destination and source
    dmaTestData_t referenceSourceData1;
    fillWithRandom(referenceSourceData1);
    dmaTestData_t sourceData1 = referenceSourceData1;

    dmaTestData_t referenceSourceData2;
    fillWithRandom(referenceSourceData2);
    dmaTestData_t sourceData2 = referenceSourceData2;

    dmaTestData_t referenceDestinationData1;
    fillWithRandom(referenceDestinationData1);
    dmaTestData_t destinationData1 = referenceDestinationData1;

    dmaTestData_t referenceDestinationData2;
    fillWithRandom(referenceDestinationData2);
    dmaTestData_t destinationData2 = referenceDestinationData2;

    // initialize all available DMA channels
    DmaChannel testDmaChannel[DMA_INSTANCE_COUNT * DMA_CHANNEL_COUNT];
    for (uint8_t i = 0; i < DMA_INSTANCE_COUNT * DMA_CHANNEL_COUNT; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(i / 2 + 1, testDmaChannel[i].getChannelNumber());
    }
    // TODO: try initializing more and catch the exception

    // to test dma arbitration using priority, we want two channels on the same dma instance
    TEST_ASSERT(testDmaChannel[0].getDmaInst() == testDmaChannel[DMA_INSTANCE_COUNT - 2].getDmaInst());

    // configure dma for the memory to memory transfer from sourceData to destinationData1 with high priority
    testDmaChannel[0].configTransfer(LL_DMA_DIRECTION_MEMORY_TO_MEMORY | LL_DMA_PRIORITY_LOW | LL_DMA_MODE_NORMAL |
                                     LL_DMA_PERIPH_INCREMENT | LL_DMA_MEMORY_INCREMENT |
                                     LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD);
    testDmaChannel[0].setPeriphRequest(LL_DMAMUX_REQ_MEM2MEM);
    testDmaChannel[0].configAddresses(sourceData1.data, destinationData1.data);
    testDmaChannel[0].setDataLength(TEST_DATA_SIZE);
    // enable interrupts by setting callback functions (default IRQ priority)
    testDmaChannel[0].registerCallbackFunctionHT(htCallback1);
    testDmaChannel[0].registerCallbackFunctionTC(tcCallback1);
    testDmaChannel[0].registerCallbackFunctionTE(erCallback1);
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);
    
    // configure dma for the memory to memory transfer from sourceData to destinationData2 with low priority
    testDmaChannel[2].configTransfer(LL_DMA_DIRECTION_MEMORY_TO_MEMORY | LL_DMA_PRIORITY_HIGH | LL_DMA_MODE_NORMAL |
                                                          LL_DMA_PERIPH_INCREMENT | LL_DMA_MEMORY_INCREMENT |
                                                          LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD);
    testDmaChannel[2].setPeriphRequest(LL_DMAMUX_REQ_MEM2MEM);
    testDmaChannel[2].configAddresses(sourceData2.data, destinationData2.data);
    testDmaChannel[2].setDataLength(TEST_DATA_SIZE);
    // enable interrupts by setting callback functions (default IRQ priority)
    testDmaChannel[2].registerCallbackFunctionHT(htCallback2);
    testDmaChannel[2].registerCallbackFunctionTC(tcCallback2);
    testDmaChannel[2].registerCallbackFunctionTE(erCallback2);
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_VERYHIGH);

    // enable both channels, starting with the one with low priority
    testDmaChannel[0].enable();
    testDmaChannel[2].enable();

    // wait until transfer should be finished
    LL_mDelay(1);

    // check that the last interrupt that occurred was the transfer complete interrupt
    TEST_ASSERT_EQUAL_UINT32(static_cast<uint32_t>(InterruptSequence_t::TC), static_cast<uint32_t>(InterruptSequence1));
    TEST_ASSERT_EQUAL_UINT32(static_cast<uint32_t>(InterruptSequence_t::TC), static_cast<uint32_t>(InterruptSequence2));

    // check that source data was not altered
    TEST_ASSERT_EQUAL_UINT32(referenceSourceData1.dataBefore, sourceData1.dataBefore);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(referenceSourceData1.data, sourceData1.data, TEST_DATA_SIZE);
    TEST_ASSERT_EQUAL_UINT32(referenceSourceData1.dataAfter, sourceData1.dataAfter);

    TEST_ASSERT_EQUAL_UINT32(referenceSourceData2.dataBefore, sourceData2.dataBefore);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(referenceSourceData2.data, sourceData2.data, TEST_DATA_SIZE);
    TEST_ASSERT_EQUAL_UINT32(referenceSourceData2.dataAfter, sourceData2.dataAfter);

    // check that data before and after the destination was not altered
    TEST_ASSERT_EQUAL_UINT32(referenceDestinationData1.dataBefore, destinationData1.dataBefore);
    TEST_ASSERT_EQUAL_UINT32(referenceDestinationData1.dataAfter, destinationData1.dataAfter);

    TEST_ASSERT_EQUAL_UINT32(referenceDestinationData2.dataBefore, destinationData2.dataBefore);
    TEST_ASSERT_EQUAL_UINT32(referenceDestinationData2.dataAfter, destinationData2.dataAfter);

    // check that the source data was copied to the destination
    TEST_ASSERT_EQUAL_UINT32_ARRAY(referenceSourceData1.data, destinationData1.data, TEST_DATA_SIZE);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(referenceSourceData2.data, destinationData2.data, TEST_DATA_SIZE);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testDma);
    return UNITY_END();
}
