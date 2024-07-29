#undef UNITY_OUTPUT_CHAR(c)
#define UNITY_OUTPUT_CHAR(c)    uartObj.transmit(&c, 1, true);

#include "unity.h"
#include "stm32g4xx_ll_utils.h"

#include "clock.hpp"
#include "lpuart.hpp"

const Clock &clockConfig = Clock::instance();
LpUart uartObj(LPUART1, clockConfig.getPCLK1(), 115200);




void setUp(void) {
    
}

void tearDown(void) {
    // clean stuff up here
}

void testLpUartSend(void)
{
    TEST_ASSERT(true);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testLpUartSend);
    return UNITY_END();
}