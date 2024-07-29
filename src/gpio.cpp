#include "gpio.hpp"

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_rcc.h"

GpioPin::GpioPin(gpio_pin_t pinArg, gpio_port_t portArg, gpio_mode_t modeArg, gpio_pupd_t pupdArg, gpio_otype_t otypeArg, gpio_speed_t speedArg, gpio_af_t afArg) : 
pin(pinArg), port(portArg), mode(modeArg), pupd(pupdArg), otype(otypeArg), speed(speedArg), af(afArg)
{
    switch (port)
    {
    case PortA:
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
        break;
    case PortB:
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
        break;
    case PortC:
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
        break;
    case PortD:
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
        break;
    case PortE:
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
        break;
    case PortF:
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
        break;
    case PortG:
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG);
        break;
    }
    LL_GPIO_SetPinMode(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin), static_cast<uint32_t>(mode));
    LL_GPIO_SetPinOutputType(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin), static_cast<uint8_t>(otype));
    LL_GPIO_SetPinSpeed(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin), static_cast<uint32_t>(speed));
    LL_GPIO_SetPinPull(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin), static_cast<uint32_t>(pupd));
    if(pin <= LL_GPIO_PIN_7)
        LL_GPIO_SetAFPin_0_7(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin), static_cast<uint32_t>(af));
    else
        LL_GPIO_SetAFPin_8_15(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin), static_cast<uint32_t>(af));
}

GpioPin::GpioPin(gpio_config_t config) :
GpioPin(config.pin, config.port, config.mode, config.pupd, config.otype, config.speed, config.af)
{
}

void GpioPin::set(void)
{
    LL_GPIO_SetOutputPin(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin));
}

void GpioPin::set(bool state)
{
    state ? set() : reset();
}

void GpioPin::reset(void)
{
    LL_GPIO_ResetOutputPin(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin));
}

void GpioPin::toggle(void)
{
    LL_GPIO_TogglePin(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin));
}

bool GpioPin::get(void)
{
    return LL_GPIO_IsInputPinSet(reinterpret_cast<GPIO_TypeDef *>(port), static_cast<uint32_t>(pin));
}
