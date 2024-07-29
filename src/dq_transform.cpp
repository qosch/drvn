#include "dq_transform.hpp"
#include <limits>
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_cordic.h"

#define CORDIC_INST CORDIC
constexpr uint16_t PI_TWO_THIRDS = std::numeric_limits<uint16_t>::max() * 2 / 3;
constexpr uint16_t PI_FOUR_THIRDS = std::numeric_limits<uint16_t>::max() * 1 / 3;

DqTransform::DqTransform(void) :
m_cordic_args({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
{
    // enable clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CORDIC);
    // configure cordic
    LL_CORDIC_Config(CORDIC_INST, LL_CORDIC_FUNCTION_COSINE, /* cosine function */
                     LL_CORDIC_PRECISION_6CYCLES,            /* leads to +-3.5 bit maximum error */
                     LL_CORDIC_SCALE_0,                      /* no scale */
                     LL_CORDIC_NBWRITE_1,                    /* Both input variables can be written in one 32bit write */
                     LL_CORDIC_NBREAD_1,                     /* Both output variables can be read in one 32bit write */
                     LL_CORDIC_INSIZE_16BITS,                /* q1.15 format for input data */
                     LL_CORDIC_OUTSIZE_16BITS);              /* q1.15 format for output data */
    
    // configure dma writing arguments to DMA
    dmaChannelWrite.configTransfer(LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_LOW | LL_DMA_MODE_NORMAL |
                                   LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
                                   LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD);
    dmaChannelWrite.configAddresses(&m_cordic_args, reinterpret_cast<void *>(LL_CORDIC_DMA_GetRegAddr(CORDIC_INST, LL_CORDIC_DMA_REG_DATA_IN)));
    dmaChannelWrite.setDataLength(6);
    dmaChannelWrite.setPeriphRequest(LL_DMAMUX_REQ_CORDIC_WRITE);

    dmaChannelRead.configTransfer(LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_PRIORITY_MEDIUM | LL_DMA_MODE_CIRCULAR |
                                  LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
                                  LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD);
    dmaChannelRead.configAddresses(reinterpret_cast<void *>(LL_CORDIC_DMA_GetRegAddr(CORDIC_INST, LL_CORDIC_DMA_REG_DATA_OUT)), &m_cordic_res);
    dmaChannelRead.setDataLength(6);
    dmaChannelRead.setPeriphRequest(LL_DMAMUX_REQ_CORDIC_READ);

    dmaChannelRead.enable();
    dmaChannelWrite.enable();
}

DqTransform::~DqTransform(void)
{
    LL_CORDIC_DeInit(CORDIC_INST);
}

void DqTransform::runForward(int16_t u, int16_t v, int16_t w, uint16_t theta)
{
    m_cordic_args[0].angle = theta;
    m_cordic_args[0].amplitude = u;
    m_cordic_args[1].angle = theta + PI_TWO_THIRDS;
    m_cordic_args[1].amplitude = v;
    m_cordic_args[2].angle = theta + PI_FOUR_THIRDS;
    m_cordic_args[2].amplitude = w;

    dmaChannelWrite.disable();
    dmaChannelWrite.setDataLength(6);
    dmaChannelWrite.enable();
    LL_CORDIC_EnableDMAReq_RD(CORDIC_INST);
    LL_CORDIC_EnableDMAReq_WR(CORDIC_INST);
}

void DqTransform::getResultsForward(int32_t &d, int32_t &q)
{
    // do something else after calling the run() method to avoid waiting for the cordic results in the while loop
    while((dmaChannelRead.getRemainingTransfers() != 6) || !dmaChannelRead.getFlagTC()) {}
    d =  2 * (m_cordic_res[0].cos + m_cordic_res[1].cos + m_cordic_res[2].cos) / 3;
    q = -2 * (m_cordic_res[0].sin + m_cordic_res[1].sin + m_cordic_res[2].sin) / 3;
}

void DqTransform::getResultsForward(float &d, float &q)
{
    // do something else after calling the run() method to avoid waiting for the cordic results in the while loop
    while((dmaChannelRead.getRemainingTransfers() != 6) || !dmaChannelRead.getFlagTC()) {}
    d = ( 2 * (m_cordic_res[0].cos + m_cordic_res[1].cos + m_cordic_res[2].cos)) / 3.0f;
    q = (-2 * (m_cordic_res[0].sin + m_cordic_res[1].sin + m_cordic_res[2].sin)) / 3.0f;
}

void DqTransform::runForward(int16_t u, int16_t v, int16_t w, uint16_t theta, int32_t &d, int32_t &q)
{
    runForward(u, v, w, theta);
    getResultsForward(d, q);
}

void DqTransform::runForward(int16_t u, int16_t v, int16_t w, uint16_t theta, float &d, float &q)
{
    runForward(u, v, w, theta);
    getResultsForward(d, q);
}

void DqTransform::runInverse(int16_t d, int16_t q, uint16_t theta)
{
    m_cordic_args[0].angle = theta;
    m_cordic_args[0].amplitude = d;
    m_cordic_args[1].angle = theta;
    m_cordic_args[1].amplitude = q;
    m_cordic_args[2].angle = theta + PI_TWO_THIRDS;
    m_cordic_args[2].amplitude = d;
    m_cordic_args[3].angle = theta + PI_TWO_THIRDS;
    m_cordic_args[3].amplitude = q;
    m_cordic_args[4].angle = theta + PI_FOUR_THIRDS;
    m_cordic_args[4].amplitude = d;
    m_cordic_args[5].angle = theta + PI_FOUR_THIRDS;
    m_cordic_args[5].amplitude = q;
    
    dmaChannelWrite.disable();
    dmaChannelWrite.setDataLength(6);
    dmaChannelWrite.enable();
    LL_CORDIC_EnableDMAReq_RD(CORDIC_INST);
    LL_CORDIC_EnableDMAReq_WR(CORDIC_INST);
}

void DqTransform::getResultsInverse(int32_t &u, int32_t &v, int32_t &w)
{
    u = m_cordic_res[0].cos - m_cordic_res[1].sin;
    v = m_cordic_res[2].cos - m_cordic_res[3].sin;
    w = m_cordic_res[4].cos - m_cordic_res[5].sin;
}

void DqTransform::getResultsInverse(float &u, float &v, float &w)
{
    u = m_cordic_res[0].cos - m_cordic_res[1].sin;
    v = m_cordic_res[2].cos - m_cordic_res[3].sin;
    w = m_cordic_res[4].cos - m_cordic_res[5].sin;
}

void DqTransform::runInverse(int16_t d, int16_t q, uint16_t theta, int32_t &u, int32_t &v, int32_t &w)
{
    runInverse(d, q, theta);
    getResultsInverse(u, v, w);
}

void DqTransform::runInverse(int16_t d, int16_t q, uint16_t theta, float &u, float &v, float &w)
{
    runInverse(d, q, theta);
    getResultsInverse(u, v, w);
}
