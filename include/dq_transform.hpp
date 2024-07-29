#include <cstdint>
#include <array>
#include "dma_channel.hpp"


class DqTransform
{
public:
    DqTransform(void);
    ~DqTransform(void);
    void runForward(int16_t u, int16_t v, int16_t w, uint16_t theta);
    void runForward(int16_t u, int16_t v, int16_t w, uint16_t theta, int32_t &d, int32_t &q);
    void runForward(int16_t u, int16_t v, int16_t w, uint16_t theta, float &d, float &q);
    void getResultsForward(int32_t &d, int32_t &q);
    void getResultsForward(float &d, float &q);
    void runInverse(int16_t d, int16_t q, uint16_t theta);
    void runInverse(int16_t d, int16_t q, uint16_t theta, int32_t &u, int32_t &v, int32_t &w);
    void runInverse(int16_t d, int16_t q, uint16_t theta, float &u, float &v, float &w);
    void getResultsInverse(int32_t &u, int32_t &v, int32_t &w);
    void getResultsInverse(float &u, float &v, float &w);
    DmaChannel dmaChannelRead;
    DmaChannel dmaChannelWrite;

private:
    
    struct cordic_args_t
    {
        volatile uint16_t angle;
        volatile int16_t amplitude;
    };
    struct cordic_res_t
    {
        int16_t cos;
        int16_t sin;
    };

    std::array<cordic_args_t, 6> m_cordic_args;
    std::array<cordic_res_t, 6> m_cordic_res;
};
