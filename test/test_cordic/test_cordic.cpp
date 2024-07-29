#include "unity_config.h"
#include "unity.h"
#include "stm32g4xx_ll_utils.h"

#include <random>
#include "dq_transform.hpp"

#define NUM_TEST_CYCLES 10000
#define MAX_DEVIATION 3.5f

DqTransform dqTransformObj;

void setUp(void) {
    
}

void tearDown(void) {
    // clean stuff up here
}

void runDqForwardSoftware(float u, float v, float w, uint16_t theta, float &d, float &q)
{
    float theta_f = theta * 2.0f * M_PI / static_cast<float>(std::numeric_limits<uint16_t>::max() + 1);
    d = ( 2.0f / 3.0f) * (u * cos(theta_f) + v * cos(theta_f - (M_PI * 2.0f / 3.0f)) + w * cos(theta_f - (M_PI * 4.0f / 3.0f)));
    q = (-2.0f / 3.0f) * (u * sin(theta_f) + v * sin(theta_f - (M_PI * 2.0f / 3.0f)) + w * sin(theta_f - (M_PI * 4.0f / 3.0f)));
}

void runDqInverseSoftware(float d, float q, uint16_t theta, float &u, float &v, float &w)
{
    float theta_f = theta * 2.0f * M_PI / static_cast<float>(std::numeric_limits<uint16_t>::max() + 1);
    u = (d * cos(theta_f)) - (q * sin(theta_f));
    v = (d * cos(theta_f - (M_PI * 2.0f / 3.0f))) - (q * sin(theta_f - (M_PI * 2.0f / 3.0f)));
    w = (d * cos(theta_f - (M_PI * 4.0f / 3.0f))) - (q * sin(theta_f - (M_PI * 4.0f / 3.0f)));
}

void testCordicForward(void)
{
    int16_t u, v, w;
    float d, q, d_true, q_true;
    uint16_t theta;
    for(uint32_t i = 0; i<NUM_TEST_CYCLES; i++)
    {
        // generate random inputs with a maximum of 0.75
        u = std::rand() / (87382);
        v = std::rand() / (87382);
        w = std::rand() / (87382);
        theta = static_cast<uint16_t>(std::rand());

        // run dq transform using cordic
        dqTransformObj.runForward(u, v, w, theta);
        dqTransformObj.getResultsForward(d, q);

        // calculate ground truth
        runDqForwardSoftware(static_cast<float>(u), static_cast<float>(v), static_cast<float>(w), static_cast<float>(theta), d_true, q_true);
        
        // check results
        TEST_ASSERT_FLOAT_WITHIN(MAX_DEVIATION, d_true, d);
        TEST_ASSERT_FLOAT_WITHIN(MAX_DEVIATION, q_true, q);
    }
}

void testCordicInverse(void)
{
    int16_t d, q;
    float u, v, w, u_true, v_true, w_true;
    uint16_t theta;
    for(uint32_t i = 0; i<NUM_TEST_CYCLES; i++)
    {
        // generate random inputs with a maximum of 0.75
        d = std::rand() / (87382);
        q = std::rand() / (87382);
        theta = static_cast<uint16_t>(std::rand());

        // run dq transform using cordic
        dqTransformObj.runInverse(d, q, theta);
        dqTransformObj.getResultsInverse(u, v, w);

        // calculate ground truth
        runDqInverseSoftware(static_cast<float>(d), static_cast<float>(q), static_cast<float>(theta), u_true, v_true, w_true);
        
        // check results
        TEST_ASSERT_FLOAT_WITHIN(MAX_DEVIATION, u_true, u);
        TEST_ASSERT_FLOAT_WITHIN(MAX_DEVIATION, v_true, v);
        TEST_ASSERT_FLOAT_WITHIN(MAX_DEVIATION, w_true, w);

    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testCordicForward);
    RUN_TEST(testCordicInverse);
    return UNITY_END();
}