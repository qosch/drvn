#include "unity.h"
#include "unity_config.h"
#include "stm32g4xx_ll_utils.h"

void setUp(void) {
    
}

void tearDown(void) {
    // clean stuff up here
}

void testUartSend(void)
{
    TEST_ASSERT(true);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testUartSend);
    return UNITY_END();
}