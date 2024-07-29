#ifndef PIO_UNIT_TESTING
#include "main.hpp"
#include "gpio.hpp"
#include "dma_channel.hpp"
#include "lpuart.hpp"
#include "timer.hpp"
#include "dq_transform.hpp"
#include "rng.hpp"

#include <string>

#include "stm32g4xx_ll_utils.h"
#include "statistics.h"
#include "tinyformat.h"

const Clock &clockConfig = Clock::instance();
LpUart uartObj(LPUART1, clockConfig.getPCLK1(), 115200);

RandomNumberGenerator RngObj;

int main(void)
{
    SCB->CPACR |= ((3UL << (10*2))|(3UL << (11*2)));  /* set CP10 and CP11 Full Access */
    
    GpioPin Lpuart1Tx(GpioPin::Pin2, GpioPin::PortA, GpioPin::AlternateFunction, GpioPin::None, GpioPin::PushPull, GpioPin::HIGH, GpioPin::AF12);
    GpioPin Lpuart1Rx(GpioPin::Pin3, GpioPin::PortA, GpioPin::AlternateFunction, GpioPin::None, GpioPin::PushPull, GpioPin::HIGH, GpioPin::AF12);

    GpioPin Led2(GpioPin::Pin5, GpioPin::PortA, GpioPin::Output, GpioPin::None, GpioPin::PushPull, GpioPin::LOW, GpioPin::AF0);

    GpioPin TimingPin(GpioPin::Pin8, GpioPin::PortC, GpioPin::Output, GpioPin::None, GpioPin::PushPull, GpioPin::LOW, GpioPin::AF0);

    while (1)
    {
        Led2.toggle();
        LL_mDelay(100);
        
        uartObj.transmit(std::string("test\n"), false);
    }
}



#endif
